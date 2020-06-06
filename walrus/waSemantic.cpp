/************************************************************
* Walrus project                                        2019
* Semantic parts of the tasks. They all differ by two small parts:
* -- how we init the deck;
* -- how we do the iteration.
*
************************************************************/
#include "walrus.h"

// a dummy pattern for all scans
void Walrus::ScanDummy()
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
#ifdef SEMANTIC_RED55_KINGS_PART_15_16

void Walrus::FillSemantic(void)
{
   sem.onInit = &Walrus::WithdrawByInput;
   sem.onShareStart = &Walrus::AllocFilteredTasksBuf;
   sem.fillFlipover = &Walrus::FillFO_39Double;
   sem.onScanCenter = &Walrus::ScanRed55;
   sem.scanCover = ACTUAL_CARDS_COUNT * 2; // since we flip the hands
   sem.onAfterMath = &Walrus::SolveSavedTasks;
}

void Walrus::ScanRed55()
{
   // we have some cards starting from each position
   SplitBits sum(SumFirstHand());
   SplitBits sec(SumSecondHand());
   for (int idxHandStart = 0;;) {
      SplitBits third(checkSum - sum.card.jo - sec.card.jo);
      uint bar = 0;
      uint foo = R55_ClassifyHands(bar, sum, sec, third);

      // account the deal
      hitsCount[foo][bar]++;

      // flip hands within the same deal, account it too
      bar = 0;
      foo = R55_ClassifyHands(bar, sum, third, sec);
      hitsCount[foo][bar]++;

      // advance to account next hand
      sum.card.jo -= deck[idxHandStart].card.jo;
      u64 flipcd = deck[13 + idxHandStart].card.jo;
      sec.card.jo -= flipcd;
      sum.card.jo += flipcd;
      sec.card.jo += deck[26 + idxHandStart++].card.jo;

      // smart-exit using highBits
      if (sec.IsEndIter()) {
         break;
      }
   }
}

// RET: foo/bar -- indices for accounting hand
// OUT: camp
uint Walrus::R55_ClassifyHands(uint &camp, SplitBits &lho, SplitBits &partner, SplitBits &rho)
{
   uint fo = R55_FilterOut(partner, camp, lho, rho);
   if (!fo) {
      // recruit methods
      camp = fo = 1;
      Red_SaveForSolver(partner, lho, rho);
   }

   return fo;
}

// OUT: camp
uint Walrus::R55_FilterOut(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
   // checks are intermixed according to profiler results
   // partner: 15-16 balanced. exactly 4 hearts. exactly 2 diamonds
   const uint ORDER_BASE = 5;
   const uint SKIP_BY_NT = 1;

   twlHCP hcpPart(partner);
   if (hcpPart.total < 15 || 16 < hcpPart.total) {
      camp = SKIP_BY_NT;
      return ORDER_BASE + 3; // wrong points count
   }

   twLengths lenPart(partner);
   if (lenPart.h != 4) {
      camp = SKIP_BY_NT;
      return ORDER_BASE + 4; // no 4-cards fit
   }

   if (lenPart.d != 2) {
      camp = SKIP_BY_NT;
      return ORDER_BASE + 5; // no doubleton diamonds
   }

   if (lenPart.s < 2 ||
      lenPart.c < 2) {
      camp = SKIP_BY_NT;
      return ORDER_BASE + 6; // a shortage in NT
   }

   if (lenPart.s > 4 ||
       lenPart.c > 4) {
      camp = SKIP_BY_NT;
      return ORDER_BASE + 7; // long black suits
   }

   // seems it passes
   return 0;
}

#endif // SEMANTIC_RED55_KINGS_PART_15_16

// OUT: camp
uint Walrus::Red_Reclassify(uint plainScore, uint &row)
{
   if (plainScore > 9) {
      row = 1;
      return 1 + plainScore - 9;
   }
   return 1 + 10 - plainScore;
}


#ifdef SEMANTIC_TRICOLOR_STRONG

void Walrus::FillSemantic(void)
{
   sem.onInit = &Walrus::WithdrawByInput;
   sem.onShareStart = &Walrus::AllocFilteredTasksBuf;
   sem.fillFlipover = &Walrus::FillFO_39Double;
   sem.onScanCenter = &Walrus::ScanTricolor;
   sem.scanCover = ACTUAL_CARDS_COUNT * 2; // since we flip the hands
   sem.onAfterMath = &Walrus::SolveSavedTasks;
}

void Walrus::ScanTricolor()
{
   // we have some cards starting from each position
   SplitBits sum(SumFirstHand());
   SplitBits sec(SumSecondHand());
   for (int idxHandStart = 0;;) {
      SplitBits third(checkSum - sum.card.jo - sec.card.jo);
      uint bar = 0;
      uint foo = Tricolor_ClassifyHands(bar, sum, sec, third);

      // account the deal
      hitsCount[foo][bar]++;

      // flip hands within the same deal, account it too
      bar = 0;
      foo = Tricolor_ClassifyHands(bar, sum, third, sec);
      hitsCount[foo][bar]++;

      // advance to account next hand
      sum.card.jo -= deck[idxHandStart].card.jo;
      u64 flipcd = deck[13 + idxHandStart].card.jo;
      sec.card.jo -= flipcd;
      sum.card.jo += flipcd;
      sec.card.jo += deck[26 + idxHandStart++].card.jo;

      // smart-exit using highBits
      if (sec.IsEndIter()) {
         break;
      }
   }
}

// RET: foo/bar -- indices for accounting hand
// OUT: camp
uint Walrus::Tricolor_ClassifyHands(uint &camp, SplitBits &lho, SplitBits &partner, SplitBits &rho)
{
   //uint fo = Tricolor_FilterOut(partner, camp, lho, rho);
   uint fo = TriSunday_FilterOut(partner, camp, lho, rho);
   if (!fo) {
      // recruit methods
      camp = fo = 1;
      Red_SaveForSolver(partner, lho, rho);
   }

   return fo;
}

// OUT: camp
uint Walrus::Tricolor_FilterOut(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
   const uint ORDER_BASE = 5;
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP  = 2;

   twlHCP hcpPart(partner);
   if (hcpPart.total != 6) {
      camp = SKIP_BY_PART;
      return ORDER_BASE; // wrong points count
   }

   twlHCP hcpResp(rho);
   if (hcpResp.total > 6) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // resp points count
   }
   twlHCP hcpOpp(lho);
   if (hcpOpp.total < 12) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // wrong points count
   }

   twLengths lenOpp(lho);
   if (lenOpp.d < 5 || lenOpp.c < 4) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 3; // suit bid
   }
   if (lenOpp.c == 4) {
      if (hcpOpp.total < 14) {
         camp = SKIP_BY_OPP;
         return ORDER_BASE + 4; // points when 5-4 minors
      }
   }

   twLengths lenPart(partner);
   if (lenPart.h > lenPart.s) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 4; // bid spades
   }
   if (lenPart.h == lenPart.s) {
      if (hcpPart.h > hcpPart.s) {
         camp = SKIP_BY_PART;
         return ORDER_BASE + 5; // bid spades
      }
   }
   if (lenPart.s < 2) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 6; // bid spades
   }


   // seems it passes
   return 0;
}

uint Walrus::TriSunday_FilterOut(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
   const uint ORDER_BASE = 5;
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;

   twlHCP hcpPart(partner);
   if (hcpPart.total != 3) {
      camp = SKIP_BY_PART;
      return ORDER_BASE; // wrong points count
   }

   twLengths lenPart(partner);
   if (lenPart.h != 5) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // bid 2h 
   }

   twlHCP hcpOpp(rho);
   if (hcpOpp.total < 11 || 16 < hcpOpp.total) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // wrong points count
   }

   twLengths lenOpp(rho);
   if (lenOpp.d != 6) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 3; // bid d twice
   }

   twlHCP hcpResp(lho);
   if (hcpResp.total > 8) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 4; // resp points count
   }

   // seems it passes
   return 0;
}


#endif // SEMANTIC_TRICOLOR_STRONG


