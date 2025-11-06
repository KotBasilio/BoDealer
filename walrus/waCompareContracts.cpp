/************************************************************
 * Walrus project                                        2020
 * AI parts
 *
 ************************************************************/
#include <string.h>

#include "walrus.h"

static s64 SingleIMP(s64 diff) 
{
   s64 sign = (diff > 0) ? 1 : -1;
   s64 x = diff * sign;

   if (x < 20) return 0;
   if (x < 50) return 1 * sign;
   if (x < 90) return 2 * sign;
   if (x < 130) return 3 * sign;
   if (x < 170) return 4 * sign;
   if (x < 220) return 5 * sign;
   if (x < 270) return 6 * sign;
   if (x < 320) return 7 * sign;
   if (x < 370) return 8 * sign;
   if (x < 430) return 9 * sign;
   if (x < 500) return 10 * sign;
   if (x < 600) return 11 * sign;
   if (x < 750) return 12 * sign;
   if (x < 900) return 13 * sign;
   if (x < 1100) return 14 * sign;
   if (x < 1300) return 15 * sign;
   if (x < 1500) return 16 * sign;
   if (x < 1750) return 17 * sign;
   if (x < 2000) return 18 * sign;
   if (x < 2250) return 19 * sign;
   if (x < 2500) return 20 * sign;
   if (x < 3000) return 21 * sign;
   if (x < 3500) return 22 * sign;
   if (x < 4000) return 23 * sign;

   return 24 * sign;
}

static void DbgShowComparison(s64 gainPrima, s64 gainSecunda, s64 delta)
{
   if (!config.dbg.verboseComparisons) {
      return;
   }

   printf("  Actions: A: %4lld; B: %4lld. ", gainPrima, gainSecunda);
   const char* action = "A";
   if (delta < 0) {
      action = "B";
      delta = -delta;
   }
   if (delta > 0) {
      printf("%s is better by %lld points, %lld IMPs\n", action, delta, SingleIMP(delta));
   } else {
      printf("Any action is OK\n");
   }
}

void Walrus::NoticeMagicFly(uint trickSuit, uint tricksNT, const deal& cards)
{
   if (tricksNT > trickSuit) {
      progress.SolvedExtraMark(IO_ROW_COMPARISON, IO_CAMP_MORE_NT);
   } else if (tricksNT == trickSuit) {
      progress.SolvedExtraMark(IO_ROW_COMPARISON, IO_CAMP_SAME_NT);
   } else {
      progress.SolvedExtraMark(IO_ROW_COMPARISON, IO_CAMP_PREFER_SUIT);
   }
}

void Walrus::ComparePrimaSecunda(uint tricksA, uint tricksB, const deal& cards)
{
   // detect score
   auto gainPrima   = cumulScore.prima.Get(tricksA);
   auto gainSecunda = cumulScore.secunda.Get(tricksB);

   // the second scorer can be for the other side
   if (config.solve.seekDecisionCompete) {
      gainSecunda = -gainSecunda;
   }

   // mark
   uint camp = (gainPrima >  gainSecunda) ? IO_CAMP_PREFER_PRIMA :
               (gainPrima == gainSecunda) ? IO_CAMP_NO_DIFF : IO_CAMP_PREFER_SECUNDA;
   progress.SolvedExtraMark(IO_ROW_COMPARISON, camp);

   // add up, converting to imps
   auto delta = gainPrima - gainSecunda;
   auto impsPrima = SingleIMP(delta);
   ui.primaBetterBy += impsPrima;

   // store in the row
   AddMarksInHugeMatch(cards, camp, impsPrima);

   // dbg
   DbgShowComparison(gainPrima, gainSecunda, delta);
}

// looks unused
//#define min(a,b) (((a) < (b)) ? (a) : (b))

