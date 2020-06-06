/************************************************************
* Walrus project                                        2019
* Split bits part -- variant for suits
*
************************************************************/
#include "walrus.h"

// ----------------------- FLIP-OUT-fills
void Walrus::FillFO_MaxDeck() // when we pick one hand only
{
   for (int i = FLIP_OVER_START_IDX, j = 0; i < DECK_ARR_SIZE; i++, j++) {
      deck[i] = deck[j];
   }
   // no need to place highBits -- it's always in place after the deck end
}

void Walrus::FillFO_39Single() // when we pick one hand, then the other hand. total of two
{
   int i = FLIP_OVER_START_IDX, j = 0;
   for (; j < REMOVED_CARDS_COUNT - 1; i++, j++) {
      deck[i] = deck[j];
   }
   deck[i] = highBits;
}

void Walrus::FillFO_39Double() // when we pick one hand, then the other, then the next one
{
   int i = FLIP_OVER_START_IDX, j = 0;
   for (; j < 2*REMOVED_CARDS_COUNT - 1; i++, j++) {
      deck[i] = deck[j];
   }
   // no need to place highBits -- it's always in place after the deck end
}

