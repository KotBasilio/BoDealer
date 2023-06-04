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
      progress.hitsCount[foo][bar]++;

      // advance to account next hand
      sum.card.jo -= shuf.deck[idxHandStart].card.jo;
      sum.card.jo += shuf.deck[13 + idxHandStart++].card.jo;

      // smart-exit using highBits placed after shuf.deck
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

#endif // SEMANTIC_KEYCARDS_10_12

#ifdef SEMANTIC_SPADE_4_WHEN_1H
void Walrus::FillSemantic(void)
{
   sem.onInit = &Walrus::WithdrawByInput;
   sem.fillFlipover = &Walrus::FillFO_39Double;
   sem.onScanCenter = &Walrus::ScanOrb;
   sem.scanCover = ACTUAL_CARDS_COUNT * 2; // since we flip the hands
   sem.onFilter = &WaFilter::Spade4;
}

uint WaFilter::Spade4(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
   const uint PASS_BASE = ANSWER_ROW_IDX;
   const uint ORDER_BASE = 3;
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_RESP = 2;
   const uint SKIP_BY_OPP = 3;

   // lho passed before 1h
   twlHCP hcpResp(rho);
   if (hcpResp.total > 11) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE; // open something
   }

   // shape for 1h, ignoring hcp
   twLengths lenPart(partner);
   if (lenPart.h < 5) {
      camp = SKIP_BY_PART;
      return ORDER_BASE; // 5+h
   }
   if (lenPart.s >= lenPart.h) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // would open 1s
   }

   // apply hcp to partner
   twlHCP hcpPart(partner);
   if (hcpPart.total < 11 || 16 < hcpPart.total) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2; // wrong points count
   }

   // opp passed on 1h
   twLengths lenOpp(lho);
   if (lenOpp.h < 5) {
      twlHCP hcpOpp(lho);
      camp = SKIP_BY_OPP;
      if (13 < hcpOpp.total) {
         return ORDER_BASE; // bid something (NT, dbl)
      }
      if (10 < hcpOpp.total) {
         if (lenOpp.c > 4 || 
             lenOpp.d > 4 ||
             lenOpp.s > 4) {
            return ORDER_BASE + 1; // bid a suit
         }
      }
   }

   // seems it passes
   if (lenPart.s < 4) {
      camp = lenPart.s;
   } else {
      camp = 4;
   }
   return PASS_BASE;
}

#endif // SEMANTIC_SPADE_4_WHEN_1H

