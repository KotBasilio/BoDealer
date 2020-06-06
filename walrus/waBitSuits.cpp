/************************************************************
* Walrus project                                        2019
* Split bits part -- variant for suits
*
************************************************************/
#include "walrus.h"

#ifdef SBITS_LAYOUT_FOR_SUITS

// IN: nominal 2,3,...11=J, 12=Q, 13=K, 14=A
SplitBits::SplitBits(int hcp, int ctrl, uint suit, UCHAR nominal)
{
   card.w = suit;
   SetHCP(hcp);

   // we can store high enough nominal
   if (nominal >= 7) {
      uint bit = (suit == SPADS) ? AIB_SPADS :
                 (suit == HEART) ? AIB_HEART :
                 (suit == DIAMD) ? AIB_DIAMD :
                                   AIB_CLUBS;
      card.hi = bit << (nominal - 7);
   }

}

void SplitBits::SetHCP(int hcp)
{
   if (hcp) {
      uint suit = Suit();
      uint bit = (suit == SPADS) ? HIPTS_SPADS :
                 (suit == HEART) ? HIPTS_HEART :
                 (suit == DIAMD) ? HIPTS_DIAMD :
                                   HIPTS_CLUBS;
      card.pts = (bit >> 16) * hcp;
   } else {
      card.pts = 0;
   }
}

static char ranks[] = { '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A' };
int Walrus::InitSuit(uint suit, int idx)
{
   int nominal = 2;
   for each (char r in ranks) {
      deck[idx++] = SplitBits(0, 0, suit, nominal++);
   }
   deck[idx - 1].SetHCP(4);
   deck[idx - 2].SetHCP(3);
   deck[idx - 3].SetHCP(2);
   deck[idx - 4].SetHCP(1);

   return idx;
}

bool SplitBits::IsWeakOpening(void)
{
   // preempts
   SplitBits preempt(card.jo + EACH_SUIT);
   if (preempt.HasAnyEightPlus()) {
      return true;
   }

   // too weak
   if (HighCardPoints() < 7) {
      return false;
   }

   // multi
   SplitBits multi(card.jo + (HEART+SPADS)*2);
   if (multi.HasAnyEightPlus()) {
      return true;
   }

   // 4+4 majors
   if ((card.jo & MAJORS_4P4P) == MAJORS_4P4P) {
      return true;
   }

   // 5+5 any
   return HasAny55();
}

bool SplitBits::HasAny55()
{
   uint bits5(card.w + EACH_SUIT*3);
   uint countHighBits = 
      ((bits5 & 0x000800) >> ( 8+3)) +
      ((bits5 & 0x008000) >> (12+3)) +
      ((bits5 & 0x080000) >> (16+3)) +
      ((bits5 & 0x800000) >> (20+3));
   return countHighBits > 1;
}

void Walrus::WithdrawSuit(int suit, int count)
{
   int high = SOURCE_CARDS_COUNT;
   while (deck[high].IsBlank()) {
      high--;
   }
   for (uint i=0; i<SOURCE_CARDS_COUNT; i ++ ) {
      if (deck[i].Suit() == suit) {
         deck[i] = deck[high];
         deck[high--] = sbBlank;
         count--;
         if (!count) {
            return;
         }
      }
   }
}

void Walrus::WithdrawPts(int suit, int hcp)
{
   int high = SOURCE_CARDS_COUNT;
   while (deck[high].IsBlank()) {
      high--;
   }
   for (uint i=0; i<SOURCE_CARDS_COUNT; i ++ ) {
      if (deck[i].Suit() == suit && deck[i].HighCardPoints() == hcp) {
         deck[i] = deck[high];
         deck[high--] = sbBlank;
         return;
      }
   }
}

#endif // SBITS_LAYOUT_FOR_SUITS

