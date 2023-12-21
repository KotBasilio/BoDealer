//
// Oscar the Owl, Senior Kibitzer at the Griffins, 
// whose role is usually limited to acting 
// as an audience for HH's exploits
//

#define  _CRT_SECURE_NO_WARNINGS
#include "walrus.h"
#include HEADER_CURSES

int main()
{
   fprintf(stderr, "%s", "Oscar is watching\n");
   int countEmpty = 0;

   char str[256];
   for (;;) {
      gets_s(str, sizeof(str));
      printf("%s\n", str);

      // variants of exits
      if (strncmp(str, GRIFFINS_CLUB_IS_CLOSING, sizeof(str)) == 0) {
         break;
      }

      // lonely
      if (str[0] == '\n' || str[0] == 0) {
         if (++countEmpty > 10) {
            break;
         }
      }
   }
   //PLATFORM_GETCH();
}

