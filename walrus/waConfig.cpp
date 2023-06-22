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
      int slashToDel = 2;
      for (int i = rl; --i >= 0;) {
         if (buf[i] == '\\') {
            if (--slashToDel == 0) {
               break;
            }
         }
         buf[i] = 0;
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
   const char* fname = namesBase.StartFrom;

   FILE* stream;
   char line[100];

   if (!fopen_s(&stream, fname, "r") == 0) {
      return;
   }

   while (!feof(stream))
   {
      if (!fgets(line, sizeof(line), stream)) {
         break;
      }
      printf("%s", line);
   }

   fclose(stream);
}

bool Walrus::InitByConfig()
{
   // may read something
   cfgTask.ReadStart();

   // prepare basing on config
   FillSemantic();
   InitDeck();
   memset(progress.hitsCount, 0, sizeof(progress.hitsCount));
   shuf.SeedRand();

   return true;
}

