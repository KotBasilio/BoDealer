/************************************************************
* Walrus project                                        2019
* Semantic parts of the tasks. They all differ by two small parts:
* -- how we init the deck;
* -- how we do the iteration.
*
************************************************************/
#include "walrus.h"

#ifdef SINGLE_HAND_TASK
void Walrus::FillSemantic(void)
{
   sem.fillFlipover = &Walrus::FillFO_MaxDeck;
   sem.onScanCenter = &Walrus::ScanKeycards;
   sem.scanCover = ACTUAL_CARDS_COUNT;
}

void Walrus::ScanKeycards()
{
   // we have some cards starting from each position
   SplitBits sum(SumFirstHand());
   for (int idxHandStart = 0;;) {
      // a dummy part, count nothing, just check total
      uint bar = 0;
      uint foo = KC_ClassifyHand(bar, sum);

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

uint Walrus::KC_ClassifyHand(uint &camp, SplitBits &partner)
{
   const uint ORDER_BASE = 4;
   const uint SKIP_BY_PART = 1;

   // ----------- filter out
   twlHCP hcpPart(partner);
   //if (hcpPart.total != 12) {
   //if (hcpPart.total < 10 || 12 < hcpPart.total) {
   if (hcpPart.total < 10 || 11 < hcpPart.total) {
      camp = SKIP_BY_PART;
      return ORDER_BASE; // wrong points count
   }

   twLengths lenPart(partner);
   if (lenPart.h < 4 || lenPart.s < 4) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // no majors
   }
   if (lenPart.c > 5 || lenPart.s < 5) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2; // too long minors
   }

   // ----------- classify 

   // which king do we need
   u64 king = lenPart.s >= lenPart.h
      ? 0x4000000000000000LL
      : 0x0000400000000000LL;

   // get keycards
   u64 kc_mask = 0x8000800080008000LL | king;
   u64 keycards = partner.card.jo & kc_mask;

   // sum bits
   uint x =  (keycards & 0xF000000000000000LL) >> (16*3 + 12 + 2);
   uint y =  (keycards & 0x0000F00000000000LL) >> (16*2 + 12 + 2);
   uint z = ((keycards & 0x00000000F0000000LL) >> (16   + 12 + 3)) +
            ((keycards & 0x000000000000F000LL) >> (       12 + 3));
   camp = (x & 0x1) + ((x & 0x2) >> 1) +
          (y & 0x1) + ((y & 0x2) >> 1) + z;

   return 0;
}

#endif // SINGLE_HAND_TASK



