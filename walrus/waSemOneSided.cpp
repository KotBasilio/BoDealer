/************************************************************
* Walrus project                                        2019
* Semantic parts of the tasks. They all differ by two small parts:
* -- how we init the deck;
* -- how we do the iteration.
*
************************************************************/
#include "walrus.h"

#ifdef SEMANTIC_FEB_3NT_REBID
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::SomeInvite;
   sem.onScoring = &Walrus::Score_3NT;
}

uint WaFilter::SomeInvite(SplitBits& partner, uint& camp, SplitBits& sand, SplitBits& direct)
{
   twlHCP hcpPart(partner);
   if (hcpPart.total < 7 || 9 < hcpPart.total) {//part 1NT on 1d
      camp = SKIP_BY_PART;
      return ORDER_BASE; // wrong points count
   }
   twLengths lenPart(partner);
   if (lenPart.d > 3) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; 
   }
   if (3 < lenPart.s ||
      3 < lenPart.h ||
      5 < lenPart.c) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2; 
   }

   twLengths lenDir(direct);
   twlHCP hcpDir(direct);
   if (hcpDir.total > 11 && lenDir.d < 3) {
      camp = SKIP_BY_DIRECT;
      return ORDER_BASE; // would overcall
   }
   if (hcpDir.total > 5) {
      if (lenDir.h > 5 ||
         lenDir.s > 5) {
         camp = SKIP_BY_DIRECT;
         return ORDER_BASE + 1;// no preempt
      }
      if (lenDir.h + lenDir.s > 9) {
         camp = SKIP_BY_DIRECT;
         return ORDER_BASE + 2;// michaels
      }
   }
   if (lenDir.h + lenDir.s > 10) {
      camp = SKIP_BY_DIRECT;
      return ORDER_BASE + 2;// michaels
   }
   if (lenDir.h > 6 ||
      lenDir.s > 6 ||
      lenDir.c > 6) {
      camp = SKIP_BY_DIRECT;
      return ORDER_BASE + 5; // no 7+ suits
   }

   if (hcpDir.h > 4 && lenDir.h > 5) {
      camp = SKIP_BY_DIRECT;
      return ORDER_BASE + 3;// h preempt
   }
   if (hcpDir.s > 4 && lenDir.s > 5) {
      camp = SKIP_BY_DIRECT;
      return ORDER_BASE + 3;// s preempt
   }

   if (hcpDir.total > 7) {
      if (lenDir.h > 4 ||
         lenDir.s > 4) {
         camp = SKIP_BY_DIRECT;
         return ORDER_BASE + 4; // would overcall
      }
   }

   if (lenDir.d < 1 ||
      lenDir.h < 1 ||
      lenDir.s < 1 || 
      lenDir.c < 1) {
      camp = SKIP_BY_DIRECT;
      return ORDER_BASE + 5;// no voids
   }
   if (lenDir.d < 2 ||
      lenDir.h < 2 ||
      lenDir.s < 2 ||
      lenDir.c < 2) {
      camp = SKIP_BY_DIRECT;
      return ORDER_BASE + 6;// no singletons
   }

   twLengths lenSand(sand);
   if (lenSand.h > 6 ||
      lenSand.s > 6 ||
      lenSand.c > 6) {
      camp = SKIP_BY_SANDWICH;
      return ORDER_BASE;// no 7+ suits
   }
   twlHCP hcpSand(sand);
   if (hcpSand.total > 8) {
      if (lenSand.h + lenSand.s > 8) {
         camp = SKIP_BY_SANDWICH;
         return ORDER_BASE + 1; // wrong majors, would overcall
      }
   }
   if (lenSand.h + lenSand.s > 10) {
      camp = SKIP_BY_SANDWICH;
      return ORDER_BASE + 2; // wrong majors, would overcall
   }
   if (lenSand.d < 1 ||
      lenSand.h < 1 ||
      lenSand.s < 1 ||
      lenSand.c < 1) {
      camp = SKIP_BY_SANDWICH;
      return ORDER_BASE + 3;// no voids
   }
   if (lenSand.d < 2 ||
      lenSand.h < 2 ||
      lenSand.s < 2 ||
      lenSand.c < 2) {
      camp = SKIP_BY_SANDWICH;
      return ORDER_BASE + 3;// no singletons
   }
   if (hcpSand.total > 11) {
      if (lenSand.h > 4 || lenSand.s > 4) {
         camp = SKIP_BY_SANDWICH;
         return ORDER_BASE + 4; // wrong majors, would overcall
      }
   }


   // seems it passes
   return 0;
}
#endif // SEMANTIC_FEB_3NT_REBID

// ------------------------------------------------------------------------
// 3.0 micro-filters
#ifdef SEMANTIC_JULY_MOROZOV_HAND_EVAL
void Walrus::FillSemantic(void)
{
   OrbNorthFillSem();
   //sem.onBoardAdded = &Walrus::DisplayBoard;
   //sem.onScoring = &Walrus::Score_3NT;
   sem.onScoring = &Walrus::Score_6NT;
   sem.vecFilters.clear();
   //ADD_2PAR_FILTER( SOUTH, PointsRange, 15, 17);
   ADD_2PAR_FILTER( SOUTH, PointsRange, 16, 17);
   ADD_2PAR_FILTER( SOUTH, SpadesLen,   2, 3);
   ADD_2PAR_FILTER( SOUTH, HeartsLen,   2, 3);
   ADD_2PAR_FILTER( SOUTH, DiamondsLen, 2, 5);
   ADD_2PAR_FILTER( SOUTH, ClubsLen,    2, 5);
}
#endif // SEMANTIC_JULY_MOROZOV_HAND_EVAL

#ifdef SEMANTIC_OCT_SEEK_6D
void Walrus::FillSemantic(void)
{
   OrbNorthFillSem();
   //sem.onBoardAdded = &Walrus::DisplayBoard;
   sem.onScoring = &Walrus::Score_NV6Minor;
   sem.onPostmortem = &Walrus::PostmortemHCP;
   sem.vecFilters.clear();
   ADD_4PAR_FILTER(SOUTH, ExactShape, 3, 2, 5, 3);
   ADD_2PAR_FILTER(SOUTH, PointsRange, 11, 16);
   ADD_0PAR_FILTER(EAST, NoOvercall);
   ADD_0PAR_FILTER(WEST, NoOvercall);
   ADD_3PAR_FILTER(SOUTH, KeyCardsRange, SOL_DIAMONDS, 2, 2);
   //ADD_3PAR_FILTER(SOUTH, KeyCardsRange, SOL_DIAMONDS, 1, 2);
}
#endif // SEMANTIC_OCT_SEEK_6D

