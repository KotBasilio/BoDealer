/************************************************************
* Walrus project                                        2023
* communication part
*
************************************************************/
#define  _CRT_SECURE_NO_WARNINGS
#include "waCrossPlatform.h"
#include HEADER_SLEEP
#include HEADER_CURSES
#include "waDoubleDeal.h"
#include "../dds-develop/examples/hands.h"

#pragma message("waOwl.cpp REV: hello v0.9")

struct OwlImpl {
    HANDLE PipeOut = NULL;
    HANDLE PipeFromOwl = NULL;
    char viscr[DDS_HAND_LINES][DDS_FULL_LINE]{};

    // HTTP transport -- enabled when config.cowl.isHttp
    std::string taskId;
    std::unique_ptr<IOwlTransport> http;
};

OscarTheOwl owl;
static OwlImpl impl;

#ifdef _DEBUG
   #define OWL_CONFIG_SUFFIX  "\\x64\\Debug"
   #define OWL_VSCODE_SUFFIX  "\\sln\\Bo\\x64\\Debug"
#else
   #define OWL_CONFIG_SUFFIX  "\\x64\\Release"
   #define OWL_VSCODE_SUFFIX  "\\sln\\Bo\\x64\\Release"
#endif

OscarTheOwl::OscarTheOwl()
{
   buffer[0] = 0;
   earlyLine[0] = 0;
   ClearViScreen();
}

static BOOL _AttemptStartOscar(CHAR *workDirPath, CHAR* suffix, STARTUPINFO& siStartInfo, PROCESS_INFORMATION& piProcInfo)
{
   CHAR oscarPath[MAX_PATH];
   strcpy(oscarPath, workDirPath);
   strcat(oscarPath, suffix);

   //printf("Attempt path to Oscar: %s\n", oscarPath);

   CHAR exeCLI[MAX_PATH];
   strcpy(exeCLI, GRIFFINS_CLUB_RUNS);

   // if HTTP mode desired:
   if (config.cowl.isHttp) {
      char tmp[64];
      sprintf(tmp, "-http %d ", config.cowl.port);   // e.g. 3042
      strcat(exeCLI, tmp);
   }

   // keep logresult
   if (config.cli.nameFileOutput[0]) {
      strcat(exeCLI, "-logresult ");
      strcat(exeCLI, "\"");
      strcat(exeCLI, config.cli.nameFileOutput);
      strcat(exeCLI, "\" ");
   }

   // Create the child process.
   return CreateProcess(oscarPath,
      exeCLI,   // Command line
      NULL,     // Process handle not inheritable
      NULL,     // Thread handle not inheritable
      TRUE,     // Set handle inheritance to TRUE
      NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE /*| CREATE_NEW_PROCESS_GROUP*/,  // creation flags
      NULL,     // Use parent's environment block
      NULL,     // Use parent's starting directory
      &siStartInfo,  // Pointer to STARTUPINFO structure
      &piProcInfo); // Pointer to PROCESS_INFORMATION structure
}

bool Walrus::StartOscar()
{
   // consider HTTP transport
   if (config.cowl.isHttp) {
      config.TaskID = config.txt.nameTask;
      impl.taskId = config.TaskID;
      impl.http = CreateOwlTransport();
      if (impl.http->InitAndHandshake()) {
         return true;
      }

      printf("Failed to init HTTP transport to Oscar.\n");
      printf("Fallback to pipes\n");
      impl.http->Shutdown();
      impl.http.reset();
   }

   // go pipe-way
   const DWORD bufferSize = MAX_PATH;
   CHAR oscarPath0[bufferSize];

   DWORD dwRet = GetCurrentDirectory(bufferSize, oscarPath0);
   if (dwRet == 0) {
      return false;
   }

   HANDLE g_hChildStd_IN_Rd = NULL;
   HANDLE g_hChildStd_OUT_Wr = NULL;

   SECURITY_ATTRIBUTES saAttr;
   saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
   saAttr.bInheritHandle = TRUE;
   saAttr.lpSecurityDescriptor = NULL;

   // Create the input pipe for sending data from parent to child
   if (!CreatePipe(&g_hChildStd_IN_Rd, &impl.PipeOut, &saAttr, 0)) {
      //std::cerr << "CreatePipe failed";
      return false;
   }

   if (!SetHandleInformation(impl.PipeOut, HANDLE_FLAG_INHERIT, 0)) {
      //std::cerr << "SetHandleInformation failed";
      return false;
   }

   // Create the output pipe for receiving data from child to parent
   if (!CreatePipe(&impl.PipeFromOwl, &g_hChildStd_OUT_Wr, &saAttr, 0)) {
      //std::cerr << "CreatePipe failed";
      return false;
   }

   if (!SetHandleInformation(impl.PipeFromOwl, HANDLE_FLAG_INHERIT, 0)) {
      //std::cerr << "SetHandleInformation failed";
      return false;
   }

   PROCESS_INFORMATION piProcInfo;
   STARTUPINFO siStartInfo;

   ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
   ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
   siStartInfo.cb = sizeof(STARTUPINFO);
   siStartInfo.hStdInput = g_hChildStd_IN_Rd;
   siStartInfo.hStdError = g_hChildStd_OUT_Wr;
   siStartInfo.dwFlags |= STARTF_USESTDHANDLES;


   CHAR oscarPath1[bufferSize];
   strcpy(oscarPath1, oscarPath0);
   strcat(oscarPath1, OWL_CONFIG_SUFFIX);
   CHAR oscarPath2[bufferSize];
   strcpy(oscarPath2, oscarPath0);
   strcat(oscarPath2, OWL_VSCODE_SUFFIX);

   if (      !_AttemptStartOscar(oscarPath0, "\\Oscar.exe", siStartInfo, piProcInfo)) {
      if (   !_AttemptStartOscar(oscarPath1, "\\Oscar.exe", siStartInfo, piProcInfo)) {
         if (!_AttemptStartOscar(oscarPath2, "\\Oscar.exe", siStartInfo, piProcInfo)) {
            printf("Oscar is absent.\n");
            return false;
         }
      }
   }

   // Close handles to the child process and its primary thread.
   CloseHandle(piProcInfo.hProcess);
   CloseHandle(piProcInfo.hThread);

   // Now, in this process we can write to g_PipeOut and the child process will be able to read from it.
   char *message = "Senior kibitzer Oscar is observing a task:\n";
   owl.Send(message);

   // Receive data from the child process
   char buffer[256];
   DWORD bytesRead = 0;
   if (!ReadFile(impl.PipeFromOwl, buffer, sizeof(buffer), &bytesRead, NULL)) {
      //std::cerr << "Read from output pipe failed";
      return false;
   }
   if (bytesRead < sizeof(buffer)) {
      buffer[bytesRead] = 0;
   }
   printf("%s", buffer);

   // follow with early line
   owl.OnStart();

   // Test variable parameters
   // owl.Show("message %d %s\n", 10, "xxx");

   return true;
}

void OscarTheOwl::Show(const char* format, ...)
{
   va_list args;
   va_start(args, format);

   std::vsnprintf(buffer, bufferSize, format, args);

   va_end(args);

   printf("%s", buffer);
   Send(buffer);
}

void OscarTheOwl::OnProgress(const char* format, ...)
{
   va_list args;
   va_start(args, format);

   std::vsnprintf(buffer, bufferSize, format, args);

   va_end(args);

   if (mainProgress->isDoneAll) {
      printf("%s", buffer);
   } else {
      Send(buffer);
   }
}

void OscarTheOwl::OnDone(const char* format, ...)
{
   va_list args;
   va_start(args, format);

   std::vsnprintf(buffer, bufferSize, format, args);

   va_end(args);

   if (mainProgress->isDoneAll) {
      Send(buffer);
   } else {
      printf("%s", buffer);
   }
}

void OscarTheOwl::Silent(const char* format, ...)
{
   va_list args;
   va_start(args, format);

   std::vsnprintf(buffer, bufferSize, format, args);

   va_end(args);

   Send(buffer);
}

void OscarTheOwl::Send(char* message)
{
   // HTTP path
   if (impl.http) {
      OwlEvent e;
      e.type = OwlEventType::Log;
      e.message = message ? std::string(message) : std::string();
      e.silent = false; // first step: treat Send() as non-silent
      impl.http->Enqueue(e);
      return;
   }

   // PIPE path
   if (impl.PipeOut) {
      DWORD bytesWritten;
      WriteFile(impl.PipeOut, message, (DWORD)strlen(message), &bytesWritten, NULL);
   } else {
      strcat(earlyLine, message);
   }
}

void OscarTheOwl::OnStart()
{
   // the intention of OnStart is in 
   // sending accumulated earlyLine 
   // -- valid both for HTTP and PIPE paths
   if (earlyLine[0]) {
      Send(earlyLine);
      earlyLine[0] = 0;
   }
}

void OscarTheOwl::Goodbye()
{
   // HTTP path
   if (impl.http) {
      OwlEvent d;
      d.type = OwlEventType::Done;
      d.message = GRIFFINS_CLUB_IS_CLOSING;
      d.silent = false;
      impl.http->Enqueue(d);

      impl.http->Flush(std::chrono::milliseconds(200));
      impl.http->Shutdown();
      impl.http.reset();
      return;
   }

   // PIPE path
   Silent("%s\n", GRIFFINS_CLUB_IS_CLOSING);
   PLATFORM_SLEEP(100);
   CloseHandle(impl.PipeOut);
   impl.PipeOut = NULL;
}

///////////////////////////////////////////////////////////////////////
// From here forth, the code originated from DDS, with our additions //

extern unsigned short int dbitMapRank[16];
extern unsigned char dcardSuit[5];
extern unsigned char dcardHand[4];
extern unsigned char dcardRank[16];

void equals_to_string(int equals, char* res);

void PrintFut(char title[], futureTricks * fut)
{
   printf("%s\n", title);

   printf("%6s %-6s %-6s %-6s %-6s\n",
      "card", "suit", "rank", "equals", "score");

   for (int i = 0; i < fut->cards; i++)
   {
      char res[15] = "";
      equals_to_string(fut->equals[i], res);
      printf("%6d %-6c %-6c %-6s %-6d\n",
         i,
         dcardSuit[ fut->suit[i] ],
         dcardRank[ fut->rank[i] ],
         res,
         fut->score[i]);
   }
   printf("\n");
}

void OwlOneFut(char title[], futureTricks * fut)
{
   owl.Silent("%s\n", title);

   owl.Silent("%6s %-6s %-6s %-6s %-6s\n",
      "card", "suit", "rank", "equals", "score");

   for (int i = 0; i < fut->cards; i++)
   {
      char res[15] = "";
      equals_to_string(fut->equals[i], res);
      owl.Silent("%6d %-6c %-6c %-6s %-6d\n",
         i,
         dcardSuit[ fut->suit[i] ],
         dcardRank[ fut->rank[i] ],
         res,
         fut->score[i]);
   }
   owl.Silent("\n");
}

void _ClearViScreen()
{
   // clear virtual screen
   for (int l = 0; l < DDS_HAND_LINES; l++) {
      memset(impl.viscr[l], ' ', DDS_FULL_LINE);
      impl.viscr[l][DDS_FULL_LINE - 1] = '\0';
   }
}

void _SilentViScreen(int count, char scr[][DDS_FULL_LINE])
{
   // print the v-screen
   for (int i = 0; i < count; i++) {
      scr[i][DDS_FULL_LINE - 1] = '\0';
      owl.Silent("   %s\n", scr[i]);
   }
   //owl.Silent("\n\n");
}

void OscarTheOwl::ClearViScreen()
{
   // clear virtual screen
   for (int l = 0; l < DDS_HAND_LINES; l++) {
      memset(screen[l], ' ', DDS_FULL_LINE);
      impl.viscr[l][DDS_FULL_LINE - 1] = '\0';
   }
}

void OscarTheOwl::SilentViScreen(int count)
{
   _SilentViScreen(count, screen);
}

static void FillVScreen(const deal& dl)
{
   int c, h, s, r;

   _ClearViScreen();

   // for each hand
   for (h = 0; h < DDS_HANDS; h++)
   {
      // detect location
      int offset, line;
      if (h == 0) {
         offset = DDS_HAND_OFFSET;
         line = 0;
      } else if (h == 1) {
         offset = 2 * DDS_HAND_OFFSET;
         line = 4;
      } else if (h == 2) {
         offset = DDS_HAND_OFFSET;
         line = 8;
      } else {
         offset = 0;
         line = 4;
      }

      // print hand to v-screen
      for (s = 0; s < DDS_SUITS; s++) {
         c = offset;
         for (r = 14; r >= 2; r--) {
            if ((dl.remainCards[h][s] >> 2) & dbitMapRank[r])
               impl.viscr[line + s][c++] = static_cast<char>(dcardRank[r]);
         }

         if (c == offset)
            impl.viscr[line + s][c++] = '-';

         if (h == SOUTH || h == EAST)
            impl.viscr[line + s][c] = '\0';
      }
   }

   // add HCP and controls
   uint ctrl;
   sprintf(impl.viscr[DDS_STATS_LINE  ] + DDS_STATS_OFFSET, "HCP : %d", CalcNSLineHCP(dl, ctrl));
   sprintf(impl.viscr[DDS_STATS_LINE+1] + DDS_STATS_OFFSET, "CTRL: %d", ctrl);
}

void PrintHand(char title[], const deal& dl)
{
   FillVScreen(dl);

   // start with title and underline it
   owl.Silent(title);
   char dashes[80];
   int l = static_cast<int>(strlen(title)) - 1;
   for (int i = 0; i < l; i++)
      dashes[i] = '-';
   dashes[l] = '\0';
   owl.Silent("%s\n", dashes);

   _SilentViScreen(DDS_HAND_LINES, impl.viscr);
}

void OwlOutBoard(char title[], const deal& dl)
{
   FillVScreen(dl);

   // start with title and underline it
   owl.Silent(title);
   char dashes[80];
   int l = static_cast<int>(strlen(title)) - 1;
   for (int i = 0; i < l; i++)
      dashes[i] = '-';
   dashes[l] = '\0';
   owl.Silent("%s\n", dashes);

   _SilentViScreen(DDS_HAND_LINES, impl.viscr);
}

void OwlTNTBoard(char title[], const deal& dl)
{
}

void PrintTwoFutures(char title[], futureTricks * fut1, futureTricks * fut2)
{
   static char bigScr[DDS_OPLEAD_LINES][DDS_FULL_LINE];

   // clear virtual screen
   for (int lidx = 0; lidx < DDS_OPLEAD_LINES; lidx++) {
      memset(bigScr[lidx], ' ', DDS_FULL_LINE);
      bigScr[lidx][DDS_FULL_LINE - 1] = '\0';
   }

   // fill it with bigScr info
   int off2 = 35;
   sprintf(bigScr[0] + off2, "%s", title);

   sprintf(bigScr[1], " %-6s %-6s %-6s              %-6s %-6s %-6s",
      "suit", "rank", "score",
      "suit", "rank", "score"
   );

   for (int i = 0; i < fut1->cards; i++) {
      sprintf(bigScr[2+i], "   %-6c %-6c %-6d",
         dcardSuit[fut1->suit[i]],
         dcardRank[fut1->rank[i]],
         fut1->score[i]);
      bigScr[2+i][23] = ' ';
   }

   for (int i = 0; i < fut2->cards; i++) {
      sprintf(bigScr[2 + i] + off2, "  %-6c %-6c %-6d",
         dcardSuit[fut2->suit[i]],
         dcardRank[fut2->rank[i]],
         fut2->score[i]);
   }

   // print the v-screen
   auto maxline = __max(fut1->cards, fut2->cards) + 2;
   _SilentViScreen(maxline, bigScr);
}

void OwlTwoFut(char title[], futureTricks * fut1, futureTricks * fut2)
{
   char bigScr[DDS_OPLEAD_LINES][DDS_FULL_LINE];

   // clear virtual screen
   for (int lidx = 0; lidx < DDS_OPLEAD_LINES; lidx++) {
      memset(bigScr[lidx], ' ', DDS_FULL_LINE);
      bigScr[lidx][DDS_FULL_LINE - 1] = '\0';
   }

   // fill it with bigScr info
   int off2 = 34;
   sprintf(bigScr[0] + off2, "%s", title);

   sprintf(bigScr[1], " %-6s %-6s %-6s             %-6s %-6s %-6s",
      "suit", "rank", "score",
      "suit", "rank", "score"
   );

   for (int i = 0; i < fut1->cards; i++) {
      sprintf(bigScr[2+i], "   %-6c %-6c %-6d",
         dcardSuit[fut1->suit[i]],
         dcardRank[fut1->rank[i]],
         fut1->score[i]);
      bigScr[2+i][23] = ' ';
   }

   for (int i = 0; i < fut2->cards; i++) {
      sprintf(bigScr[2 + i] + off2, "  %-6c %-6c %-6d",
         dcardSuit[fut2->suit[i]],
         dcardRank[fut2->rank[i]],
         fut2->score[i]);
   }

   // print the v-screen
   auto maxline = __max(fut1->cards, fut2->cards) + 2;
   _SilentViScreen(maxline, bigScr);
}

