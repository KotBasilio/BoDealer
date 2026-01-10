//
// Oscar the Owl, Senior Kibitzer at the Griffins, 
// whose role is usually limited to acting 
// as an audience for HH's exploits
//

#define  _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <wincon.h>
#include "walrus.h"
#include "Oscar.h"
#include HEADER_CURSES

static const char* artOscar[] = {
   "        |\\_______/|      \n",
   "      / | [o] [o] | \\     \n",
   "     / /|    V    |\\ \\   \n",
   "        |         |      \n",
   "        -ooo---ooo-      \n\n",
};

CHAR LOGFILE_NAME[MAX_PATH];

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
      remove(LOGFILE_NAME);
   }

   bool Retell();

private:
   bool NewsAre(const char *test);
   bool IsFeelingLost();
   void SetupConsoleColors();

   int countEmpty = 0;
   char gossip[512];
};
extern void PrepareLinearScores();
extern s64  gLinearScores[];
const s64* FindLinearScore(const char* code);

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
   FILE* file = fopen(LOGFILE_NAME, "a");
   if (!file) {
      printf("Failed to open %s\n", LOGFILE_NAME);
      return false;
   }
   fprintf(file, "%s\n", gossip);
   fclose(file);

   // TODO: recognize other commands

   // stay content
   return true;
}

constexpr uint SCO_TRICKS = 14;// 0..13

static void PrintCode(const char* code)
{
   auto cur = FindLinearScore(code);
   printf("code : %s\n", code);
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      printf(" %5lld", *cur++);
   }
   printf("\n\n");
}

static void ShowAllScores()
{
   PrepareLinearScores();

   const s64* cur = gLinearScores;
   while (*cur) {
      for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
         printf(" %5lld", *cur++);
      }
      printf("\n");
      for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
         printf(" %5lld", *cur++);
      }
      printf("\n\n");
   }

   PrintCode("V7NR");
   PrintCode("V2C");
   PrintCode("V3NX");
   PrintCode("V2SX");
}

void ReadCLIParams(int argc, char* argv[])
{
   strcpy(LOGFILE_NAME, "oscar_log.txt");
   printf("Command-line arguments:\n");
   for (int i = 0; i < argc; ++i) {
      // Print all CLI parameters, numbered
      printf("Arg %d: %s\n", i, argv[i]);

      // check for the "-logresult" CLI parameter
      if (std::strcmp(argv[i], "-logresult") == 0 && i + 1 < argc) {
         auto last = sizeof(LOGFILE_NAME) - 1;
         std::strncpy(LOGFILE_NAME, argv[i + 1], last);
         LOGFILE_NAME[last] = '\0';
      }
   }
   printf("log to : %s\n", LOGFILE_NAME);
}

int main(int argc, char* argv[])
{
   if (argc < 2) {
      ShowAllScores();
      printf("Waiting for the club to start.\n");
      printf("Press Enter to exit.\n");
      char tmpBuf[10];
      gets_s(tmpBuf, 5);

      return 0;
   }

   ReadCLIParams(argc, argv);

   OscarEcho owl;
   while (owl.Retell()) {}

   return 0;
}

