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

bool Walrus::AfterMath()
{
   // no op => back off
   if (sem.onAfterMath == (&Walrus::NOP)) {
      return false;
   }

   // relay
   (this->*sem.onAfterMath)();

   // perf
   progress.delta2 = ChronoRound();

   return true;
}

CumulativeScore::CumulativeScore()
   : ideal(0L)
   , bidGame(0L)
   , bidSlam(0L)
   , partscore(0L)
   , oppContract(0L)
   , oppCtrDoubled(0L)
   , ourOther(0L)
   , ourCombo(0L)
{
   leadS = leadH = leadD = leadC = 0L;
}

void Walrus::CleanupStats()
{
   CumulativeScore zeroes;
   cumulScore = zeroes;
}

extern void DoSelfTests();

int main(int argc, char *argv[])
{
   printf(TITLE_VERSION" started.\n");
   ChronoStart();

   Walrus walter;
   if (walter.InitByConfig()) {
      // phases 1 and 2
      walter.MainScan();
      walter.AfterMath();

      // report
      walter.ReportState();
   }

   printf("Press any key.\n");
   DoSelfTests();
   PLATFORM_GETCH();
   return 0;
}
