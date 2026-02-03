/************************************************************
* Walrus project                                        2019
* Main entry point
*
************************************************************/
#define  _CRT_SECURE_NO_WARNINGS
#include "walrus.h"
#include HEADER_CURSES
#include <chrono>
#include <cstring> // For std::strcmp

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

void ReadCLI(int argc, char* argv[])
{
   for (int i = 1; i < argc; ++i) {
      // check for the exit control
      if (std::strcmp(argv[i], "-exitondone") == 0) {
         config.cli.exitOnDone = true;
      }

      // check for the parameter to enforce HTTP mode
      if (std::strcmp(argv[i], "-http") == 0) {
         config.cowl.isHttp = true;
      }

      // check for the parameter to enforce DEV mode
      if (std::strcmp(argv[i], "-dev") == 0) {
         config.cowl.isHttp = true;
         config.cowl.isDevMode = true;
      }

      // check for the config name
      if (std::strcmp(argv[i], "-cfgname") == 0 && i + 1 < argc) {
         auto last = sizeof(config.cli.nameFileConfig) - 1;
         std::strncpy(config.cli.nameFileConfig, argv[i + 1], last);  
         config.cli.nameFileConfig[last] = '\0';
         ++i;
      }

      // check for the log result parameter
      if (std::strcmp(argv[i], "-logresult") == 0 && i + 1 < argc) {
         auto last = sizeof(config.cli.nameFileOutput) - 1;
         std::strncpy(config.cli.nameFileOutput, argv[i + 1], last);
         config.cli.nameFileOutput[last] = '\0';
         ++i;
      }
   }

}

int main(int argc, char *argv[])
{
   ChronoStart();
   ReadCLI(argc, argv);

   Walrus walter;
   if (walter.InitByConfig()) {
      walter.Main();
   }

   DoSelfTests();

   if (!config.cli.exitOnDone) {
      printf("Press any key.\n");
      PLATFORM_GETCH();
   }

   owl.Goodbye();

   return 0;
}

