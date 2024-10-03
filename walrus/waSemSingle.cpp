/************************************************************
* Walrus project                                        2019
* Semantic parts of the tasks. They all differ by two small parts:
* -- how do we init the deck;
* -- how do we iterate
*
************************************************************/
#include "walrus.h"

#ifdef SEMANTIC_KEYCARDS_10_12
void Walrus::FillSemantic(void)
{
   sem.fillFlipover = &Shuffler::FillFO_MaxDeck;
   sem.onScanCenter = &Walrus::ScanKeycards;
   sem.scanCover = ACTUAL_CARDS_COUNT;
}

void Walrus::ScanKeycards()
{
   // we have some cards starting from each position
   SplitBits sum(shuf.SumFirstHand());
   for (int idxHandStart = 0;;) {
      // a dummy part, count nothing, just check total
      uint bar = 0;
      uint foo = KeyCards_ClassifyHand(bar, sum);

      // account the hand
      progress.FilteredOutMark(foo, bar);

      // advance to account next hand
      sum.card.jo -= shuf.deck[idxHandStart].card.jo;
      sum.card.jo += shuf.deck[13 + idxHandStart++].card.jo;

      // smart-exit using highBits placed after shuf.deck
      if (sum.IsEndIter()) {
         break;
      }
   }
}

uint Walrus::KeyCards_ClassifyHand(uint &camp, SplitBits &partner)
{
   const uint SKIP_BY_PART = 1;
   constexpr uint ORDER_BASE = 7;

   // ----------- filter out
   twlHCP hcpPart(partner);
   //if (hcpPart.total != 12) {
   //if (hcpPart.total < 10 || 12 < hcpPart.total) {
   //if (hcpPart.total < 10 || 11 < hcpPart.total) {
   if (hcpPart.total < 17) {
      camp = SKIP_BY_PART;
      return ORDER_BASE; // wrong points count
   }

   twLengths lenPart(partner);
   if (lenPart.h < 4 || lenPart.s < 4) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // no majors
   }
   if (lenPart.c > 5 || lenPart.d > 5) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2; // too long minors
   }

   // ----------- classify 

   // which king that we need
   u64 king = lenPart.s >= lenPart.h
      ? 0x4000000000000000LL
      : 0x0000400000000000LL;

   // get keycards
   u64 kc_mask = ANY_ACE | king;
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

#endif // SEMANTIC_KEYCARDS_10_12


