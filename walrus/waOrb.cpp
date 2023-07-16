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
#include "walrus.h"
#include HEADER_CURSES

#define ORBIT_PERMUTE_FACTOR 6

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
      progress.hitsCount[foo][bar]++;

      // advance to account next hand
      sum.card.jo -= shuf.deck[idxHandStart].card.jo;
      sum.card.jo += shuf.deck[SYMM + idxHandStart++].card.jo;

      // smart-exit using highBits
      if (sum.IsEndIter()) {
         break;
      }
   }
}

void Walrus::Scan3FixedWest()
{
   // we have some cards starting from each position
   SplitBits sum(SumFirstHand());
   SplitBits sec(SumSecondHand());
   for (int idxHandStart = 0;;) {
      // full permutation
      SplitBits third(shuf.CheckSum() - sum.card.jo - sec.card.jo);
      Permute6(sum, sec, third);

      // advance to account next hand
      sum.card.jo -= shuf.deck[idxHandStart].card.jo;
      u64 flipcd = shuf.deck[SYMM + idxHandStart].card.jo;
      sec.card.jo -= flipcd;
      sum.card.jo += flipcd;
      sec.card.jo += shuf.deck[SYMM2 + idxHandStart++].card.jo;

      // simple exit using count -- it became faster than highBits
      if (idxHandStart >= ACTUAL_CARDS_COUNT) {
         break;
      }
   }
}

// Orb: three other hands for later double-dummy solving
// using 2.0 filtering -- now deprecated
void Walrus::ScanOrb()
{
   // we have some cards starting from each position
   SplitBits sum(SumFirstHand());
   SplitBits sec(SumSecondHand());
   for (int idxHandStart = 0;;) {
      // small permutation
      SplitBits third(shuf.CheckSum() - sum.card.jo - sec.card.jo);
      Orb_DepClassify(sum, sec, third);
      Orb_DepClassify(sum, third, sec);

      // advance to account next hand
      sum.card.jo -= shuf.deck[idxHandStart].card.jo;
      u64 flipcd = shuf.deck[SYMM + idxHandStart].card.jo;
      sec.card.jo -= flipcd;
      sum.card.jo += flipcd;
      sec.card.jo += shuf.deck[SYMM2 + idxHandStart++].card.jo;

      // simple exit using count -- it became faster than highBits
      if (idxHandStart >= ACTUAL_CARDS_COUNT) {
         break;
      }
   }
}

// using 2.0 filtering -- now deprecated
void Walrus::Orb_DepClassify(SplitBits& lho, SplitBits& partner, SplitBits& rho)
{
   uint camp = 0;
   uint reason = (filter.*sem.onDepFilter)(partner, camp, lho, rho);
   if (reason) {
      // there's some reason to skip this hand. mark it
      progress.hitsCount[reason][camp]++;
   } else {
      // save only two hands
      if (mul.countToSolve < mul.maxTasksToSolve) {
         mul.arrToSolve[mul.countToSolve++].Init(partner, rho);
      }

      // mark all saved together
      progress.hitsCount[1][1]++;
   }
}

void Walrus::Orb_DepFillSem(void)
{
   sem.onInit = &Walrus::WithdrawByInput;
   sem.onShareStart = &Walrus::AllocFilteredTasksBuf;
   sem.fillFlipover = &Shuffler::FillFO_39Double;
   sem.onScanCenter = &Walrus::ScanOrb;
   sem.scanCover = ACTUAL_CARDS_COUNT * 2; // since we flip the hands
   sem.onAfterMath = &Walrus::SolveSavedTasks;
}

void Walrus::OrbNorthFillSem(void)
{
   Orb_DepFillSem();
   sem.onScanCenter = &Walrus::Scan3FixedNorth;
   sem.scanCover = SYMM * ORBIT_PERMUTE_FACTOR;
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

   // fly can look for specific condition
   if (ui.irFly != IO_CAMP_OFF) {
      bool fits = Orb_ApproveByFly(cards);
      if (!fits) {
         return;
      }
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
   PrintHand("example:\n", cards);

   // auto show may be very short
   if (ui.firstAutoShow) {
      #ifndef SOLVE_TWICE_HANDLED_CHUNK
         return;
      #endif
   }

   // to show leads we need to re-solve it
   // -- params for re-solving
   futureTricks futUs;
   int target = -1;
   int solutions = 3; // 3 -- Return all cards that can be legally played, with their scores in descending order.
   int mode = 0;
   int threadIndex = 0;
   // -- re-solve & show
   int res = SolveBoard(cards, target, solutions, mode, &futUs, threadIndex);
   if (res != RETURN_NO_FAULT) {
      HandleErrorDDS(cards, res);
      return;
   }

   // single side solution => ok print
   #ifndef SOLVE_TWICE_HANDLED_CHUNK
      char lead[] = "";
      PrintFut(lead, &futUs);
   #else 
      // score alternative contract
      futureTricks futTheirs;
      cards.trump = TWICE_TRUMPS;
      cards.first = TWICE_ON_LEAD_INSPECT;
      target = -1;
      res = SolveBoard(cards, target, solutions, mode, &futTheirs, threadIndex);
      if (res != RETURN_NO_FAULT) {
         HandleErrorDDS(cards, res);
         return;
      }
      DdsTricks tr;
      tr.Init(futTheirs);

      // build header
      char header[60];
      #ifdef SCORE_THE_OTHER_CONTRACT
         const char *whos = "Their";
         #ifdef THE_OTHER_IS_OURS
            whos = "A";
         #endif
         sprintf(header, "%s contract in %s has %d tricks.", whos, ui.theirTrump, tr.plainScore);
      #elif defined(SEEK_MAGIC_FLY) 
         sprintf(header, "NT contract has %d tricks.", tr.plainScore);
      #else 
         sprintf(header, "<not filled title>");
      #endif 

      // output
      PrintTwoFutures(header, &futUs, &futTheirs);
   #endif // SOLVE_TWICE_HANDLED_CHUNK
}

bool Walrus::Orb_ApproveByFly(deal& cards)
{
   #ifdef SEEK_MAGIC_FLY
      // solve for the fly
      futureTricks fut;
      int target = -1;
      int solutions = PARAM_SOLUTIONS_DDS;
      int mode = 0;
      int threadIndex = 0;
      deal flyDeal = cards;
      flyDeal.trump = TWICE_TRUMPS;
      int res = SolveBoard(flyDeal, target, solutions, mode, &fut, threadIndex);
      if (res != RETURN_NO_FAULT) {
         HandleErrorDDS(flyDeal, res);
         return false;
      }
      DdsTricks tr;
      tr.Init(fut);

      // may accept
      switch (ui.irFly) {
         case IO_CAMP_MORE_NT:
            return (int)tr.plainScore > ui.irGoal;
         case IO_CAMP_SAME_NT:
            return (int)tr.plainScore == ui.irGoal;
         case IO_CAMP_PREFER_SUIT:
            return (int)tr.plainScore < ui.irGoal;
      }
   #endif // SEEK_MAGIC_FLY

   return false;
}

void Walrus::Scan3FixedNorth()
{
   // we have some cards starting from each position
   SplitBits fixedN(shuf.thrownOut);
   SplitBits sum(SumFirstHand());
   SplitBits sec(SumSecondHand());
   for (int idxHandStart = 0;;) {
      // full permutation
      twPermutedContexts xArr(fixedN, sum, sec, NORTH);
      OrbNorthClassify(xArr.lay);

      // advance to account next hand
      sum.card.jo -= shuf.deck[idxHandStart].card.jo;
      u64 flipcd = shuf.deck[SYMM + idxHandStart].card.jo;
      sec.card.jo -= flipcd;
      sum.card.jo += flipcd;
      sec.card.jo += shuf.deck[SYMM2 + idxHandStart++].card.jo;

      // simple exit using count -- it became faster than highBits
      if (idxHandStart >= ACTUAL_CARDS_COUNT) {
         break;
      }
   }
}

// a chunk for watching permutations in debugger
// xA.hand.card.jo = 0xaaaaaaaaaaaaaaaaLL;
// xB.hand.card.jo = 0xbbbbbbbbbbbbbbbbLL;
// xC.hand.card.jo = 0xccccccccccccccccLL;

twPermutedContexts::twPermutedContexts
(const SplitBits& a, const SplitBits& b, const SplitBits& c, uint fixed)
   : xA(a), xB(b), xC(c)
{
   // after constructors above work, we have lay[0..2] in place

   if (fixed == NORTH) {
      // let's copy to form a certain order:
      // A-B-C-D-B-C-B-D-C-B
      // 0 1 2 3 4 5 6 7 8 9
      lay[ 3] = twContext( SplitBits(a, b, c) );
      lay[ 4] = xB;
      lay[ 5] = xC;
      lay[ 6] = xB;
      lay[ 7] = lay[3];
      lay[ 8] = xC;
      lay[ 9] = xB;
   } else {
      DEBUG_UNEXPECTED;
   }
}


void Walrus::OrbNorthClassify(twContext * lay)
{
   // when a hand A is fixed in the start, we get 6 options to lay B,C,D
   ClassifyAndPush  (lay + 0);// ABCD
   ClassifyAndPush  (lay + 1);// ACDB
   ClassifyOnPermute(lay + 2);// ADBC
   lay[4] = lay[0];           // double-push A
   ClassifyAndPush  (lay + 4);// ACBD
   ClassifyAndPush  (lay + 5);// ABDC
   ClassifyOnPermute(lay + 6);// ADCB
}

