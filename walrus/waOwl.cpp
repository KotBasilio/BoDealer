/************************************************************
* Walrus project                                        2023
* communication part
*
************************************************************/
#define  _CRT_SECURE_NO_WARNINGS
#include "waCrossPlatform.h"
#include HEADER_SLEEP
#include HEADER_C_LEGACY
#include "waDoubleDeal.h"
#include "OscarCLI.h"
#include "../dds-develop/examples/hands.h"
#include <algorithm>
#include <filesystem>
#include <vector>

#if !defined(_WIN32)
#include <fcntl.h>
#include <unistd.h>
#if defined(__APPLE__)
#include <mach-o/dyld.h>
#elif !defined(__linux__)
#error "Executable discovery is only implemented for macOS and Linux"
#endif
#endif

//#pragma message("waOwl.cpp REV: hello v1.0")

struct OwlImpl {
#if defined(_WIN32)
    HANDLE PipeOut = NULL;
    HANDLE PipeFromOwl = NULL;
#else
    int PipeOut = -1;
    int PipeFromOwl = -1;
#endif
    char viscr[DDS_HAND_LINES][DDS_FULL_LINE]{};

    // HTTP transport -- enabled when config.cowl.isHttp
    std::string taskId;
    std::unique_ptr<IOwlTransport> http;
};

OscarTheOwl owl;
static OwlImpl impl;

OscarTheOwl::OscarTheOwl()
{
   buffer[0] = 0;
   earlyLine[0] = 0;
   ClearViScreen();
}

#if defined(_WIN32)
#ifdef _DEBUG
   #define OWL_CONFIG_SUFFIX  "\\x64\\Debug"
   #define OWL_VSCODE_SUFFIX  "\\sln\\Bo\\x64\\Debug"
#else
   #define OWL_CONFIG_SUFFIX  "\\x64\\Release"
   #define OWL_VSCODE_SUFFIX  "\\sln\\Bo\\x64\\Release"
#endif

static BOOL _AttemptStartOscar(const CHAR *workDirPath, const CHAR* suffix, STARTUPINFO& siStartInfo, PROCESS_INFORMATION& piProcInfo)
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

   // may pass waitAttach
   if (config.cli.waitAttach) {
      strcat(exeCLI, ARG_WAITATTACH);
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

static bool _SeekOscar(STARTUPINFO& siStartInfo, PROCESS_INFORMATION& piProcInfo)
{
   const DWORD bufferSize = MAX_PATH;
   CHAR executablePath[bufferSize];
   DWORD executablePathSize = GetModuleFileName(NULL, executablePath, bufferSize);
   if (executablePathSize > 0 && executablePathSize < bufferSize) {
      auto executableDirectory = std::filesystem::path(executablePath).parent_path().string();
      if (_AttemptStartOscar(executableDirectory.c_str(), "\\Oscar.exe", siStartInfo, piProcInfo)) {
         return true;
      }
   }

   CHAR oscarPath0[bufferSize];
   DWORD dwRet = GetCurrentDirectory(bufferSize, oscarPath0);
   if (dwRet == 0) {
      return false;
   }

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

   return true;
}

static bool _AttemptOscarTransports()
{
   // prepare for process creation
   PROCESS_INFORMATION piProcInfo;
   STARTUPINFO siStartInfo;
   ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
   ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));

   // consider HTTP transport
   if (config.cowl.isHttp) {
      config.TaskID = config.txt.nameTask;
      impl.taskId = config.TaskID;
      impl.http = CreateOwlTransport();
      if (impl.http->InitHeated()) {
         return true;
      }
      if (!_SeekOscar(siStartInfo, piProcInfo)) {
         return false;
      }
      if (impl.http->HandshakeAttempt()) {
         return true;
      }

      printf("Failed to init HTTP transport to Oscar.\n");
      printf("Fallback to pipes\n");
      impl.http->Shutdown();
      impl.http.reset();
   }

   // go pipe-way
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

   siStartInfo.cb = sizeof(STARTUPINFO);
   siStartInfo.hStdInput = g_hChildStd_IN_Rd;
   siStartInfo.hStdError = g_hChildStd_OUT_Wr;
   siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

   if (!_SeekOscar(siStartInfo, piProcInfo)) {
      return false;
   }

   // Close handles to the child process and its primary thread.
   CloseHandle(piProcInfo.hProcess);
   CloseHandle(piProcInfo.hThread);

   // Now, in this process we can write to g_PipeOut and the child process will be able to read from it.
   char* message = "Senior kibitzer Oscar is observing a task:\n";
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

   return true;
}
#else
static std::filesystem::path ExecutableDirectory()
{
#if defined(__APPLE__)
   uint32_t size = 0;
   _NSGetExecutablePath(nullptr, &size);
   std::vector<char> path(size);
   if (_NSGetExecutablePath(path.data(), &size) != 0) {
      return std::filesystem::current_path();
   }
   return std::filesystem::weakly_canonical(path.data()).parent_path();
#elif defined(__linux__)
   std::vector<char> path(4096);
   auto size = readlink("/proc/self/exe", path.data(), path.size() - 1);
   if (size <= 0) {
      return std::filesystem::current_path();
   }
   path[size] = 0;
   return std::filesystem::path(path.data()).parent_path();
#endif
}

static bool StartOscarProcess(int childInput = -1, int childOutput = -1)
{
   auto oscarPath = ExecutableDirectory() / "oscar";
   if (!std::filesystem::exists(oscarPath)) {
      printf("Oscar is absent: %s\n", oscarPath.c_str());
      return false;
   }

   std::vector<std::string> args{oscarPath.string(), GRIFFINS_CLUB_RUNS};
   if (config.cowl.isHttp) {
      args.emplace_back(ARG_HTTP);
      args.emplace_back(std::to_string(config.cowl.port));
   }
   if (config.cli.nameFileOutput[0]) {
      args.emplace_back(ARG_LOGRESULT);
      args.emplace_back(config.cli.nameFileOutput);
   }
   if (config.cli.waitAttach) {
      args.emplace_back(ARG_WAITATTACH);
   }

   auto pid = fork();
   if (pid < 0) {
      return false;
   }
   if (pid == 0) {
      if (childInput >= 0) {
         dup2(childInput, STDIN_FILENO);
      }
      if (childOutput >= 0) {
         dup2(childOutput, STDERR_FILENO);
      }

      std::vector<char*> argv;
      argv.reserve(args.size() + 1);
      for (auto& arg : args) {
         argv.push_back(arg.data());
      }
      argv.push_back(nullptr);
      execv(oscarPath.c_str(), argv.data());
      _exit(127);
   }
   return true;
}

static bool _AttemptOscarTransports()
{
   if (config.cowl.isHttp) {
      config.TaskID = config.txt.nameTask;
      impl.taskId = config.TaskID;
      impl.http = CreateOwlTransport();
      if (impl.http->InitHeated()) {
         return true;
      }
      if (!StartOscarProcess()) {
         return false;
      }
      if (impl.http->HandshakeAttempt()) {
         return true;
      }

      printf("Failed to init HTTP transport to Oscar.\n");
      printf("Fallback to pipes\n");
      impl.http->Shutdown();
      impl.http.reset();
   }

   int toChild[2];
   int fromChild[2];
   if (pipe(toChild) != 0) {
      return false;
   }
   if (pipe(fromChild) != 0) {
      close(toChild[0]);
      close(toChild[1]);
      return false;
   }

   // Oscar must not inherit the unused ends. In particular, inheriting
   // toChild[1] prevents it from ever observing EOF when Walrus exits.
   for (int descriptor : {toChild[0], toChild[1], fromChild[0], fromChild[1]}) {
      fcntl(descriptor, F_SETFD, fcntl(descriptor, F_GETFD) | FD_CLOEXEC);
   }

   impl.PipeOut = toChild[1];
   impl.PipeFromOwl = fromChild[0];
   if (!StartOscarProcess(toChild[0], fromChild[1])) {
      return false;
   }
   close(toChild[0]);
   close(fromChild[1]);

   char message[] = "Senior kibitzer Oscar is observing a task:\n";
   owl.Send(message);

   char buffer[256];
   auto bytesRead = read(impl.PipeFromOwl, buffer, sizeof(buffer) - 1);
   if (bytesRead <= 0) {
      return false;
   }
   buffer[bytesRead] = 0;
   printf("%s", buffer);
   return true;
}
#endif

bool Walrus::StartOscar()
{
   // pipes or HTTP transport
   if (!_AttemptOscarTransports()) {
      return false;
   }

   // follow with early line
   owl.OnStart();

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
#if defined(_WIN32)
   if (impl.PipeOut) {
      DWORD bytesWritten;
      WriteFile(impl.PipeOut, message, (DWORD)strlen(message), &bytesWritten, NULL);
   } else {
#else
   if (impl.PipeOut >= 0) {
      write(impl.PipeOut, message, strlen(message));
   } else {
#endif
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
#if defined(_WIN32)
   CloseHandle(impl.PipeOut);
   impl.PipeOut = NULL;
#else
   close(impl.PipeOut);
   impl.PipeOut = -1;
#endif
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
   auto maxline = std::max(fut1->cards, fut2->cards) + 2;
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
   auto maxline = std::max(fut1->cards, fut2->cards) + 2;
   _SilentViScreen(maxline, bigScr);
}
