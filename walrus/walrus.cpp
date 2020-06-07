/************************************************************
* Walrus project                                        2019
* Main entry point
*
************************************************************/
#define  _CRT_SECURE_NO_WARNINGS
#include "walrus.h"
#include <conio.h> // _getch()
#include <chrono>

bool Walrus::AfterMath()
{
   // no op => back off
   if (sem.onAfterMath == (&Walrus::NOP)) {
      return false;
   }

   // relay
   (this->*sem.onAfterMath)();
   return true;
}

Walrus::CumulativeScore::CumulativeScore()
   : ideal(0L)
   , bidGame(0L)
   , partscore(0L)
{
}

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

void main(int argc, char *argv[])
{
	printf(TITLE_VERSION" started.\n");
	ChronoStart();

	Walrus walter;
	if (walter.LoadInitialStatistics(namesBase.StartFrom)) {
		// phase 1
		walter.MainScan();
		auto delta1 = ChronoRound();

		// phase 2
		if (walter.AfterMath()) {
			auto delta2 = ChronoRound();
			walter.ReportState("\nFinal result:\n");
			printf("The search took %llu.%llu + an aftermath %llu.%llu sec.\n"
				, delta1 / 1000, (delta1 % 1000) / 100
				, delta2 / 1000, (delta2 % 1000) / 100);
		} else {
			walter.ReportState("\nEnding with:\n");
			printf("The search is done in %llu.%llu sec.\n", delta1 / 1000, (delta1 % 1000) / 100);
		}

	}
	printf("Press any key.\n");
	//sample_main_PlayBin();
	//sample_main_SolveBoard();
	//sample_main_SolveBoard_S1();
	//sample_main_JK_Solve();
	_getch();
}
