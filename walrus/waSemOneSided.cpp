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
   Orb_FillSem();
   sem.onFilter = &WaFilter::SomeInvite;
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
         return ORDER_BASE + 1; // wrong points count, would overcall
      }
   }
   if (lenSand.h + lenSand.s > 10) {
      camp = SKIP_BY_SANDWICH;
      return ORDER_BASE + 2; // wrong points count, would overcall
   }

   // seems it passes
   return 0;
}

#endif // SEMANTIC_FEB_3NT_REBID

