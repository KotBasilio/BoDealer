/************************************************************
* Walrus project                                        2019
* Orb scan patterns:
* -- how we init the deck;
* -- how we do the iteration.
*
************************************************************/
#define  _CRT_SECURE_NO_WARNINGS
#include "../dds-develop/include/dll.h"
#include "../dds-develop/examples/hands.h"
//#include <conio.h>
#include <curses.h>
#include "walrus.h"

// Trivial: one hand only; can be used as a pattern for all scans
void Walrus::ScanTrivial()
{
   // we have some cards starting from each position
   SplitBits sum(SumFirstHand());
   for (int idxHandStart = 0;;) {
      // a dummy part, count nothing, just check total
      uint foo = 0;
      uint bar = 0;

      // account the hand
      hitsCount[foo][bar]++;

      // advance to account next hand
      sum.card.jo -= deck[idxHandStart].card.jo;
      sum.card.jo += deck[13 + idxHandStart++].card.jo;

      // smart-exit using highBits
      if (sum.IsEndIter()) {
         break;
      }
   }
}

// Orb: three other hands for later double-dummy solving
void Walrus::ScanOrb()
{
   // we have some cards starting from each position
   SplitBits sum(SumFirstHand());
   SplitBits sec(SumSecondHand());
   for (int idxHandStart = 0;;) {
      SplitBits third(checkSum - sum.card.jo - sec.card.jo);
      uint bar = 0;
      uint foo = Orb_ClassifyHands(bar, sum, sec, third);

      // account the deal
      hitsCount[foo][bar]++;

      // flip hands within the same deal, account it too
      bar = 0;
      foo = Orb_ClassifyHands(bar, sum, third, sec);
      hitsCount[foo][bar]++;

      // advance to account next hand
      sum.card.jo -= deck[idxHandStart].card.jo;
      u64 flipcd = deck[13 + idxHandStart].card.jo;
      sec.card.jo -= flipcd;
      sum.card.jo += flipcd;
      sec.card.jo += deck[26 + idxHandStart++].card.jo;

      // simple exit using count -- it became faster that highBits
      if (idxHandStart >= ACTUAL_CARDS_COUNT) {
         break;
      }
   }
}

// RET: foo/bar -- indices for accounting hand
// OUT: camp
uint Walrus::Orb_ClassifyHands(uint &camp, SplitBits &lho, SplitBits &partner, SplitBits &rho)
{
   uint fo = (filter.*sem.onFilter)(partner, camp, lho, rho);
   if (!fo) {
      // recruit methods
      camp = fo = 1;
      Orb_SaveForSolver(partner, lho, rho);
   }

   return fo;
}

void Walrus::Orb_FillSem(void)
{
   sem.onInit = &Walrus::WithdrawByInput;
   sem.onShareStart = &Walrus::AllocFilteredTasksBuf;
   sem.fillFlipover = &Walrus::FillFO_39Double;
   sem.onScanCenter = &Walrus::ScanOrb;
   sem.scanCover = ACTUAL_CARDS_COUNT * 2; // since we flip the hands
   sem.onAfterMath = &Walrus::SolveSavedTasks;
}

void Walrus::Orb_SaveForSolver(SplitBits &partner, SplitBits &lho, SplitBits &rho)
{
   if (countToSolve >= maxTasksToSolve) {
      return;
   }

   DdsPack pack;
   pack.task.Init(partner, rho);
   arrToSolve[countToSolve++] = pack;
}

void Walrus::Orb_Interrogate(DdsTricks &tr, deal &cards, futureTricks &fut)
{
   // no interrogation goal => skip
   if (!ui.irGoal) {
      return;
   }

   // take any board with exact amount of tricks
   if (tr.plainScore != ui.irGoal) {
      return;
   }

   // relay
   Orb_ReSolveAndShow(cards);

   // may contemplate
   if (ui.firstAutoShow) {
      ui.firstAutoShow = false;
   } else {
      printf("Any key to continue...");
      PLATFORM_GETCH();
      printf("ok");
   }

   // interrogation is over
   ui.irGoal = 0;
}

void Walrus::Orb_ReSolveAndShow(deal &cards)
{
   // board first 
   PrintHand("example:\n", cards.remainCards);
   if (ui.firstAutoShow) {
      return;
   }

   // to show leads we need to re-solve it
   // -- params for re-solving
   char line[80];
   futureTricks fut;
   int target = -1;
   int solutions = 3; // 3 -- Return all cards that can be legally played, with their scores in descending order.
   int mode = 0;
   int threadIndex = 0;
   // -- re-solve & show
   int res = SolveBoard(cards, target, solutions, mode, &fut, threadIndex);
   if (res == RETURN_NO_FAULT) {
      char lead[] = "";
      PrintFut(lead, &fut);
      return;
   }

   // error-handling
   sprintf(line, "Problem hand on solve: leads %s, trumps: %s\n", haPlayerToStr(cards.first), haTrumpToStr(cards.trump));
   PrintHand(line, cards.remainCards);
   ErrorMessage(res, line);
   printf("DDS error: %s\n", line);
}

