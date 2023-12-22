//
// Oscar the Owl, Senior Kibitzer at the Griffins, 
// whose role is usually limited to acting 
// as an audience for HH's exploits
//

#define  _CRT_SECURE_NO_WARNINGS
#include "walrus.h"
#include HEADER_CURSES

struct OscarEcho {
   OscarEcho() : gossip() {
      fprintf(stderr, "%s", "Oscar is watching\n");
   }

   bool Retell();

private:
   bool NewsAre(const char *test);
   bool IsFeelingLost();

   int countEmpty = 0;
   char gossip[256];
};

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

   // TODO: recoginze other commands

   // stay content
   return true;
}

int main()
{
   OscarEcho owl;
   while (owl.Retell()) {}
}

