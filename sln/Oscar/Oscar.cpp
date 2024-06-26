//
// Oscar the Owl, Senior Kibitzer at the Griffins, 
// whose role is usually limited to acting 
// as an audience for HH's exploits
//

#define  _CRT_SECURE_NO_WARNINGS
#include "walrus.h"
#include "Oscar.h"
#include HEADER_CURSES

struct OscarEcho {
   OscarEcho() : gossip() {
      // stdout is a way to display
      printf("\n\n%s", 
         "      |\\_______/|      \n"
         "    / | [o] [o] | \\     \n"
         "   / /|    V    |\\ \\   \n"
         "      |         |      \n"
         "      -ooo---ooo-      \n\n");

      // stderr is a pipe back to Walrus
      fprintf(stderr, "%s", "Oscar is watching\n");
   }

   bool Retell();

private:
   bool NewsAre(const char *test);
   bool IsFeelingLost();

   int countEmpty = 0;
   char gossip[512];
};
extern void PrepareLinearScores();
extern s64  gLinearScores[];
const s64* FindLinearScore(const char* code);

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

   printf("Waiting for the club to start.\n");
   char tmpBuf[10];
   gets_s(tmpBuf, 5);
}

int main(int argc, char* argv[])
{
   if (argc < 2) {
      ShowAllScores();
      return 0;
   }

   OscarEcho owl;
   while (owl.Retell()) {}

   return 0;
}
