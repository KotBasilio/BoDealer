//
// Oscar the Owl, Senior Kibitzer at the Griffins, 
// whose role is usually limited to acting 
// as an audience for HH's exploits
//

#define  _CRT_SECURE_NO_WARNINGS
#include <fstream>  
#include "Oscar.h"
#include "OscarCLI.h"

//#pragma message("Oscar.cpp REV: hello v1.0")

static const char* artOscar[] = {
   "        |\\_______/|      \n",
   "      / | [o] [o] | \\     \n",
   "     / /|    V    |\\ \\   \n",
   "        |         |      \n",
   "        -ooo---ooo-      \n\n",
};

extern bool OscarAttemptHttpRun(int argc, char** argv);
extern void ShowAllScores();

// config
#define DEFAULT_LOGFILE_NAME "oscar_log.txt"
static CHAR logFileName[MAX_PATH];
static bool bWaitAttach = false;

static void PaintOscar()
{
   printf("\n\n");
   for (auto art : artOscar) {
      printf(art);
   }
}

struct OscarEcho {
   OscarEcho() : gossip() {
      // stdout is a way to display
      SetupConsoleColors();
      PaintOscar();

      // stderr is a pipe back to Walrus
      fprintf(stderr, "%s", "Oscar is watching\n");

      // Delete the file after the loop ends
      remove(logFileName);
   }

   bool Retell();

private:
   bool NewsAre(const char *test);
   bool IsFeelingLost();
   void SetupConsoleColors();

   int countEmpty = 0;
   char gossip[4096];
};
extern void PrepareLinearScores();

void OscarEcho::SetupConsoleColors()
{
   HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
   if (hConsole != INVALID_HANDLE_VALUE) {
      SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
   }
}

bool OscarEcho::IsFeelingLost()
{
   if (gossip[0] == '\n' || gossip[0] == 0) {
      if (++countEmpty > 10) {
         return true;
      }
   } else {
      countEmpty = 0;
   }

   // something happens, Oscar is not lonely
   return false;
}

bool OscarEcho::NewsAre(const char *test)
{
   return strncmp(gossip, test, sizeof(gossip)) == 0;
}

bool OscarEcho::Retell()
{
   // echo from incoming pipe
   gets_s(gossip, sizeof(gossip));
   printf("%s\n", gossip);

   // watch for exit signals
   if (NewsAre(GRIFFINS_CLUB_IS_CLOSING) || IsFeelingLost()) {
      return false;
   }

   // Append gossip to a file
   FILE* file = fopen(logFileName, "a");
   if (!file) {
      printf("Failed to open %s\n", logFileName);
      return false;
   }
   fprintf(file, "%s\n", gossip);
   fclose(file);

   // TODO: recognize other commands

   // stay content
   return true;
}

bool ReadCLIParams(int argc, char* argv[])
{
   // too few => fail
   if (argc < 2) {
      return false;
   }

   // default log file name
   strcpy(logFileName, DEFAULT_LOGFILE_NAME);
   printf("Command-line arguments:\n");
   for (int i = 0; i < argc; ++i) {
      // for debug: print all CLI parameters, numbered
      //printf("Arg %d: %s\n", i, argv[i]);

      // check for the "-logresult" CLI param
      if (std::strcmp(argv[i], ARG_LOGRESULT) == 0 && i + 1 < argc) {
         auto last = sizeof(logFileName) - 1;
         std::strncpy(logFileName, argv[i + 1], last);
         logFileName[last] = '\0';
      }

      // check for the "-waitattach" CLI param
      if (std::strcmp(argv[i], ARG_WAITATTACH) == 0) {
         bWaitAttach = true;
      }
   }
   printf("log to : %s\n", logFileName);
   return true;
}

static void ConsiderWaitForAttach()
{
   if (!bWaitAttach) {
      return;
   }

   printf("Waiting debugger attach...\n");
   while (bWaitAttach) {
      Sleep(100);
      if (false) {
         bWaitAttach = false; // set breakpoint on Sleep(), then set next statement here => set free
      }
   }
}

int main(int argc, char* argv[])
{
   if (!ReadCLIParams(argc, argv)) {
      ShowAllScores();
      printf("Waiting for the club to start.\n");
      printf("Press Enter to exit.\n");
      return 0;
   }
   ConsiderWaitForAttach();

   OscarEcho owl;
   if (!OscarAttemptHttpRun(argc, argv)) {
      while (owl.Retell()) {}
   }

   //_getch();
   return 0;
}
