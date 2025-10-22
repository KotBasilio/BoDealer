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
   SplitBits sum(shuf.SumFirstHand());
   for (int idxHandStart = 0;;) {
      // a dummy part, count nothing, just check total
      uint foo = 0;
      uint bar = 0;

      // account the hand
      progress.FilteredOutMark(foo, bar);

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
   SplitBits sum(shuf.SumFirstHand());
   SplitBits sec(shuf.SumSecondHand());
   for (u64 idxHandStart = 0; idxHandStart < SYMM;) {
      // do all permutations of 3 hands around W
      SplitBits third(shuf.CheckSum() - sum.card.jo - sec.card.jo);
      Permute6(sum, sec, third);

      // advance to account next hand
      sum.card.jo -= shuf.deck[idxHandStart].card.jo;
      u64 flipcd = shuf.deck[SYMM + idxHandStart].card.jo;
      sec.card.jo -= flipcd;
      sum.card.jo += flipcd;
      sec.card.jo += shuf.deck[SYMM2 + idxHandStart++].card.jo;
   }
}

void Walrus::SemanticsToOrbitFixedHand(void)
{
   sem.onInit = &Walrus::WithdrawByInput;
   sem.onShareStart = &WaMulti::AllocFilteredTasksBuf;
   sem.fillFlipover = &Shuffler::FillFlipOver39Double;
   sem.onScanCenter = &Walrus::Scan3FixedNorth;
   sem.scanCover = SYMM * ORBIT_PERMUTE_FACTOR;
   sem.onAfterMath = &Walrus::SolveSavedTasks;

#ifdef FIXED_HAND_WEST
   sem.onScanCenter = &Walrus::Scan3FixedWest;
#endif
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

   // a fly can look for specific condition
   #ifdef SEEK_MAGIC_FLY
      if (ui.irFly != IO_CAMP_OFF) {
         bool fits = Orb_ApproveByFly(cards);
         if (!fits) {
            return;
         }
      }
   #endif

   // relay
   Orb_ReSolveAndShow(cards);

   // may contemplate
   if (ui.firstAutoShow) {
      ui.firstAutoShow = false;
   } else {
      //ui.WaitAnyKey();
   }

   // interrogation is over
   ui.irGoal = 0;
}

void Walrus::Orb_ReSolveAndShow(deal &cards)
{
   // auto show may be very short
   if (ui.firstAutoShow && !config.solve.shouldSolveTwice) {
      OwlOutBoard("example:\n", cards);
      return;
   }

   // to show leads we need to re-solve it
   // -- params for re-solving
   futureTricks futUs;
   int target = -1;
   int solEveryLead = 3; // DOC: 3 -- Return all cards that can be legally played, with their scores in descending order.
   int mode = 0;
   int threadIndex = 0;
   // -- re-solve & show
   int res = SolveBoard(cards, target, solEveryLead, mode, &futUs, threadIndex);
   if (res != RETURN_NO_FAULT) {
      HandleErrorDDS(cards, res);
      return;
   }

   // single side solution => ok print
   if (!config.solve.shouldSolveTwice) {
      char lead[] = "";
      OwlOutBoard("example:\n", cards);
      OwlOneFut(lead, &futUs);
      printf("shown to Oscar.");
      return;
   }

   // score alternative contract
   futureTricks futTheirs;
   cards.trump = config.secondary.trump;
   cards.first = ON_LEAD_INTERROGATED;
   target = -1;
   res = SolveBoard(cards, target, solEveryLead, mode, &futTheirs, threadIndex);
   if (res != RETURN_NO_FAULT) {
      HandleErrorDDS(cards, res);
      return;
   }
   DdsTricks tr;
   tr.Init(futTheirs);

   // build header
   char header[60];
   if (config.io.DisplayingOtherContract()) {
      sprintf(header, "%s has %d tricks.", config.txt.secLongName, tr.plainScore);
   } else if (config.io.showMagicFly) {
      sprintf(header, "NT contract has %d tricks.", tr.plainScore);
   } else {
      DEBUG_UNEXPECTED;
   }

   // tricks for all possible leads
   OwlOutBoard("example:\n", cards);
   OwlTwoFut(header, &futUs, &futTheirs);

   printf("shown to Oscar.");
}

bool Walrus::Orb_ApproveByFly(const deal& cards)
{
   // solve for the fly
   futureTricks fut;
   int target = -1;
   int mode = 0;
   int threadIndex = 0;
   deal flyDeal = cards;
   flyDeal.trump = config.secondary.trump;
   int res = SolveBoard(flyDeal, target, config.solve.ddsSol, mode, &fut, threadIndex);
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

   return false;
}

void Walrus::Scan3FixedNorth()
{
   // we have some cards starting from each position
   SplitBits fixedN(shuf.thrownOut);
   SplitBits sum(shuf.SumFirstHand());
   SplitBits sec(shuf.SumSecondHand());
   SplitBits stop(sec);
   int idxHandStart = 0;
   for (; sum != stop;) {
      // do all permutations of 3 hands around N
      twPermutedContexts xArr(fixedN, sum, sec, NORTH);
      OrbNorthClassify(xArr.lay);

      // advance to account next hand
      sum.card.jo -= shuf.deck[idxHandStart].card.jo;
      u64 flipcd = shuf.deck[SYMM + idxHandStart].card.jo;
      sec.card.jo -= flipcd;
      sum.card.jo += flipcd;
      sec.card.jo += shuf.deck[SYMM2 + idxHandStart++].card.jo;
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

twPermutedContexts::twPermutedContexts
(const SplitBits& a, const SplitBits& b, const SplitBits& c)
   : xA(a), xB(b), xC(c)
{
   // after constructors above work, we have lay[0..2] in place
   // (fixed == WEST)
   // let's copy to form a certain order:
   // A-B-C-A-B-A-C-B-A-D
   // 0 1 2 3 4 5 6 7 8 9
   lay[ 3] = xA;
   lay[ 4] = xB;
   lay[ 5] = xA;
   lay[ 6] = xC;
   lay[ 7] = xB;
   lay[ 8] = xA;
   lay[ 9] = twContext( SplitBits(a, b, c) );
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

void Walrus::Permute6(SplitBits a, SplitBits b, SplitBits c)
{
   twPermutedContexts xArr(a,b,c);
   Classify6(xArr.lay);
}

void Walrus::Classify6(twContext *lay)
{
   // when a hand D is fixed in the end, we get 6 options to lay A,B,C
   ClassifyAndPull  (lay + 6);// CBAD
   ClassifyAndPull  (lay + 5);// ACBD
   ClassifyOnPermute(lay + 4);// BACD
   lay[5] = lay[9];           // double-pull D
   ClassifyAndPull  (lay + 2);// CABD
   ClassifyAndPull  (lay + 1);// BCAD
   ClassifyOnPermute(lay + 0);// ABCD
}

