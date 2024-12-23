/************************************************************
* Walrus project                                        2019
* Main entry point
*
************************************************************/
#define  _CRT_SECURE_NO_WARNINGS
#include "walrus.h"
#include HEADER_CURSES
#include <chrono>

std::chrono::steady_clock::duration timeChronoStart;
extern void DoSelfTests();

void ChronoStart()
{
   timeChronoStart = std::chrono::high_resolution_clock::now().time_since_epoch();
}

u64 ChronoRound()
{
   // calc delta from the previous round
   auto finish = std::chrono::high_resolution_clock::now().time_since_epoch();
   auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(finish - timeChronoStart).count();

   // start new round
   timeChronoStart = finish;

   return delta;
}

void Walrus::AfterMath()
{
   // relay, then measure perf
   (this->*sem.onAfterMath)();
   progress.solveTime = ChronoRound();
   progress.isDoneAll = true;
}

CumulativeScore::CumulativeScore()
   : ideal(0L)
   , bidGame(0L)
   , bidPartscore(0L)
   , oppContract(0L)
   , oppCtrDoubled(0L)
   , ourOther     (0L)
   , ourHedging   (0L)
{
   leadS = leadH = leadD = leadC = 0L;
}

void Walrus::Main()
{
   switch (config.solve.opMode) {
      case OPMODE_FIXED_TASK: {
         ScanFixedTask();
         AfterMath();
         ReportState();
         break;
      }

      case OPMODE_SEMI_STRAY: {
         ScanStray();
         EndStray();
         break;
      }

      case OPMODE_STRAY: {
         printf("STRAY -- TO DO.\n");
         break;
      }

      case OPMODE_DEMO_STATISTICS: {
         printf("Unused op mode -- DEMO_STATISTICS.\n");
         break;
      }
   }
}

int main(int argc, char *argv[])
{
   ChronoStart();

   Walrus walter;
   if (walter.InitByConfig()) {
      walter.Main();
   }

   DoSelfTests();
   printf("Press any key.\n");
   PLATFORM_GETCH();
   owl.Goodbye();

   return 0;
}
