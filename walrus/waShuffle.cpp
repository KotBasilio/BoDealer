/************************************************************
* Walrus project                                        2019
* Randomizer and shuffler parts
* replicate system rand(), strip the code out of thread handling
*
************************************************************/
#include <ctime>
#include <stdlib.h>  // RAND_MAX
#include "walrus.h"

Walrus::Shuf::Shuf()
// highBitscounts as many two cards in any suit. easily detected. doesn't cause an overflow
   : highBits(HIBITS)
   , checkSum(0)
   , oldRand(0)
   , cardsInDeck(0)
{}

void Walrus::SeedRand()
{
   shuf.oldRand = (uint)time(0);
}

uint Walrus::Rand()
{
   return (((shuf.oldRand = shuf.oldRand * 214013L + 2531011L) >> 16) & RAND_MAX);
}

void Walrus::StepAsideRand(uint stepAside)
{
   shuf.oldRand += stepAside;
}

void Walrus::TestSeed(void)
{
   RandIndices();
   printf("%s\t:%02d %02d %02d %02d ", mul.nameHlp
      , shuf.ridx[0], shuf.ridx[1], shuf.ridx[2], shuf.ridx[3]);
   RandIndices();
   printf("%02d %02d %02d %02d\n"
      , shuf.ridx[0], shuf.ridx[1], shuf.ridx[2], shuf.ridx[3]);
}

// ----------------------- FLIP-OUT-fills
void Walrus::FillFO_MaxDeck() // when we pick one hand only
{
   for (int i = FLIP_OVER_START_IDX, j = 0; i < DECK_ARR_SIZE; i++, j++) {
      shuf.deck[i] = shuf.deck[j];
   }
   // no need to place highBits -- it's always in place after the deck end
}

void Walrus::FillFO_39Single() // when we pick one hand, then the other hand. total of two
{
   int i = FLIP_OVER_START_IDX, j = 0;
   for (; j < REMOVED_CARDS_COUNT - 1; i++, j++) {
      shuf.deck[i] = shuf.deck[j];
   }
   shuf.deck[i] = shuf.highBits;
}

void Walrus::FillFO_39Double() // when we pick one hand, then the other, then the next one
{
   int i = FLIP_OVER_START_IDX, j = 0;
   for (; j < 2*REMOVED_CARDS_COUNT - 1; i++, j++) {
      shuf.deck[i] = shuf.deck[j];
   }
   // no need to place highBits -- it's always in place after the deck end
}

void Walrus::RandIndices()
{
   uint r = Rand();
   shuf.ridx[0] = r & 0x3f;
   shuf.ridx[1] = (r >> 3) & 0x3f;
   shuf.ridx[2] = (r >> 6) & 0x3f;
   shuf.ridx[3] = (r >> 9) & 0x3f;
}

void Walrus::Shuffle()
{
   // full indices
   for (uint i = 0; i < ACTUAL_CARDS_COUNT; i += RIDX_SIZE) {
      RandIndices();
      SplitBits swp(shuf.deck[shuf.ridx[3]]);
      shuf.deck[shuf.ridx[3]] = shuf.deck[i];
      shuf.deck[i]       = shuf.deck[shuf.ridx[0]];
      shuf.deck[shuf.ridx[0]] = shuf.deck[i + 1];
      shuf.deck[i + 1]   = shuf.deck[shuf.ridx[1]];
      shuf.deck[shuf.ridx[1]] = shuf.deck[i + 2];
      shuf.deck[i + 2]   = shuf.deck[shuf.ridx[2]];
      shuf.deck[shuf.ridx[2]] = shuf.deck[i + 3];
      shuf.deck[i + 3]   = swp;
   }
   VerifyCheckSum();

   // compact down
   uint low = 0;
   uint high = ACTUAL_CARDS_COUNT;
   for (int high = ACTUAL_CARDS_COUNT; high < DECK_ARR_SIZE; high++) {
      if (shuf.deck[high].IsBlank()) {
         continue;
      }

      // find a place for it
      for (;; low++) {
         if (shuf.deck[low].IsBlank()) {
            // copy & forget. break the checksum
            shuf.deck[low] = shuf.deck[high];
            break;
         }
      }
   }
}
