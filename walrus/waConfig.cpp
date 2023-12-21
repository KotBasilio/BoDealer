/************************************************************
* Walrus project                                        2023
* Configuration
*
************************************************************/
#define  _CRT_SECURE_NO_WARNINGS
#include "waCrossPlatform.h"
#include HEADER_SLEEP
#include <string.h>
#include "walrus.h"
#include HEADER_CURSES

void waFileNames::Build()
{
   // make path
   char* buf = StartFrom;
   size_t size = sizeof(StartFrom);
   #ifdef WIN_DETECT_PATH
      int rl = GetModuleFileName(NULL, buf, (DWORD)size);
      const char *key = "Dealer";
      char* in = strstr(buf, key);
      if (in) {
         in += strlen(key);
         in[1] = 0;
      }
   #else
      buf[0] = 0;
   #endif // WIN_DETECT_PATH

   // duplicate
   memcpy(Command, buf, size);
   memcpy(Progress, buf, size);
   memcpy(Solution, buf, size);

   // make real names
   strcat(StartFrom, START_FROM_FNAME);
   strcat(Command, COMMAND_FNAME);
   strcat(Progress, PROGRESS_FNAME);
   strcat(Solution, OUT_FNAME);
}

void WaConfig::ReadStart()
{
   // goals
   primGoal = 0;
   otherGoal = 0;

   const char* fname = namesBase.StartFrom;
   //printf("Reading config from: %s\n", fname);
   //PLATFORM_GETCH();

   FILE* stream;
   if (!fopen_s(&stream, fname, "r") == 0) {
      return;
   }

   while (!feof(stream)) {
      char line[100];
      if (!fgets(line, sizeof(line), stream)) {
         break;
      }
      //printf("%s", line);
   }

   fclose(stream);

   //PLATFORM_GETCH();
}

void Walrus::DetectGoals()
{
   char tail[128], chunk[20];
   DdsTricks tr;

   // primary
   cfgTask.primGoal = PokeScorerForTricks();
   printf("Primary scorer (%s, %d tr):", ui.declTrump, cfgTask.primGoal);
   strcpy(tail, "  / ");
   for (tr.plainScore = 7; tr.plainScore <= 13 ; tr.plainScore++) {
      cumulScore.bidGame = 0;
      cumulScore.partscore = 0;
      (this->*sem.onScoring)(tr);
      printf(" %lld", cumulScore.bidGame);
      if (cumulScore.partscore) {
         sprintf(chunk, " %lld", cumulScore.partscore);
         strcat(tail, chunk);
      }
   }
   printf("%s\n", tail);

   // secondary
   if (sem.solveSecondTime == &Walrus::SolveSecondTime) {
      cfgTask.otherGoal = PokeOtherScorer();
      printf("Secondary scorer (%s, %d tr):", ui.theirTrump, cfgTask.otherGoal);
      strcpy(tail, "  / ");
      for (tr.plainScore = 7; tr.plainScore <= 13; tr.plainScore++) {
         cumulScore.oppContract = 0;
         cumulScore.oppCtrDoubled = 0;
         (this->*sem.onSolvedTwice)(tr);
         printf(" %lld", - cumulScore.oppContract);
         if (cumulScore.oppCtrDoubled) {
            sprintf(chunk, " %lld", - cumulScore.oppCtrDoubled);
            strcat(tail, chunk);
         }
      }
      printf("%s\n", tail);
   }

   // all this poking left some score
   CumulativeScore zeroes;
   cumulScore = zeroes;
   //PLATFORM_GETCH();
}


bool Walrus::InitByConfig()
{
   // may read something
   cfgTask.ReadStart();

   // prepare, basing on config
   FillSemantic();
   sem.MiniLink();
   InitDeck();
   memset(progress.hitsCount, 0, sizeof(progress.hitsCount));
   shuf.SeedRand();
   InitMiniUI();
   DetectGoals();

   return StartOscar();
}

// Oscar part ----------------------------------------------------------
static HANDLE g_PipeOut = NULL;
static HANDLE g_PipeFromOwl = NULL;
OscarTheOwl owl;
char OscarTheOwl::buffer[OscarTheOwl::bufferSize];

bool Walrus::StartOscar()
{
   const DWORD bufferSize = MAX_PATH;
   CHAR oscarPath[bufferSize];

   DWORD dwRet = GetCurrentDirectory(bufferSize, oscarPath);
   if (dwRet == 0) {
      return false;
   }

   strcat(oscarPath, "\\x64\\Debug\\Oscar.exe");
   //printf("Path to Oscar: %s\n", oscarPath);

   HANDLE g_hChildStd_IN_Rd = NULL;
   HANDLE g_hChildStd_OUT_Wr = NULL;

   SECURITY_ATTRIBUTES saAttr;
   saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
   saAttr.bInheritHandle = TRUE;
   saAttr.lpSecurityDescriptor = NULL;

   // Create the input pipe for sending data from parent to child
   if (!CreatePipe(&g_hChildStd_IN_Rd, &g_PipeOut, &saAttr, 0)) {
      //std::cerr << "CreatePipe failed";
      return false;
   }

   if (!SetHandleInformation(g_PipeOut, HANDLE_FLAG_INHERIT, 0)) {
      //std::cerr << "SetHandleInformation failed";
      return false;
   }

   // Create the output pipe for receiving data from child to parent
   if (!CreatePipe(&g_PipeFromOwl, &g_hChildStd_OUT_Wr, &saAttr, 0)) {
      //std::cerr << "CreatePipe failed";
      return false;
   }

   if (!SetHandleInformation(g_PipeFromOwl, HANDLE_FLAG_INHERIT, 0)) {
      //std::cerr << "SetHandleInformation failed";
      return false;
   }

   PROCESS_INFORMATION piProcInfo;
   STARTUPINFO siStartInfo;
   BOOL bSuccess = FALSE;

   ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
   ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
   siStartInfo.cb = sizeof(STARTUPINFO);
   siStartInfo.hStdInput = g_hChildStd_IN_Rd;
   siStartInfo.hStdError = g_hChildStd_OUT_Wr;
   siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

   // Create the child process.
   bSuccess = CreateProcess( oscarPath,
      NULL,     // Command line
      NULL,     // Process handle not inheritable
      NULL,     // Thread handle not inheritable
      TRUE,     // Set handle inheritance to TRUE
      NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE /*| CREATE_NEW_PROCESS_GROUP*/,  // creation flags
      NULL,     // Use parent's environment block
      NULL,     // Use parent's starting directory
      &siStartInfo,  // Pointer to STARTUPINFO structure
      &piProcInfo); // Pointer to PROCESS_INFORMATION structure

   if (!bSuccess) {
      //std::cerr << "CreateProcess failed";
      return false;
   } else {
      // Close handles to the child process and its primary thread.
      CloseHandle(piProcInfo.hProcess);
      CloseHandle(piProcInfo.hThread);
   }

   // Now, in this process we can write to g_PipeOut and the child process will be able to read from it.
   char *message = "Senior kibitzer Oscar is observing a task:\n" TITLE_VERSION "\n";
   owl.Send(message);

   // Receive data from the child process
   char buffer[256];
   DWORD bytesRead = 0;
   if (!ReadFile(g_PipeFromOwl, buffer, sizeof(buffer), &bytesRead, NULL)) {
      //std::cerr << "Read from output pipe failed";
      return false;
   }
   if (bytesRead < sizeof(buffer)) {
      buffer[bytesRead] = 0;
   }
   printf("%s", buffer);

   owl.Show("message %d %s\n", 10, "xxx");

   return true;
}

void OscarTheOwl::Show(const char* format, ...)
{
   va_list args;
   va_start(args, format);

   std::vsnprintf(buffer, bufferSize, format, args);

   va_end(args);

   printf(buffer);
   Send(buffer);
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
   if (g_PipeOut) {
      DWORD bytesWritten;
      WriteFile(g_PipeOut, message, (DWORD)strlen(message), &bytesWritten, NULL);
   }
}

void OscarTheOwl::Goodbye()
{
   Show(GRIFFINS_CLUB_IS_CLOSING);
   CloseHandle(g_PipeOut);
   g_PipeOut = NULL;
}
