/************************************************************
* Walrus project                                        2019
* Randomizer parts
* replicate system rand(), strip the code out of thread handling
*
************************************************************/
#include <ctime>
#include <stdlib.h>  // RAND_MAX
#include "walrus.h"

void Walrus::SeedRand()
{
   oldRand = (uint)time(0);
}

uint Walrus::Rand() 
{
   return ( ((oldRand = oldRand * 214013L + 2531011L) >> 16) & RAND_MAX );
}

void Walrus::StepAsideRand(uint stepAside)
{
   oldRand += stepAside;
}

void Walrus::TestSeed(void)
{
   RandIndices();
   printf("%s\t:%02d %02d %02d %02d ", nameHlp
      , ridx[0], ridx[1], ridx[2], ridx[3]);
   RandIndices();
   printf("%02d %02d %02d %02d\n"
      , ridx[0], ridx[1], ridx[2], ridx[3]);
}

void Walrus::RandIndices()
{
   uint r = Rand();
   ridx[0] =  r       & 0x3f;
   ridx[1] = (r >> 3) & 0x3f;
   ridx[2] = (r >> 6) & 0x3f;
   ridx[3] = (r >> 9) & 0x3f;
}

void Walrus::Shuffle()
{
   // full indices
   for (uint i=0; i<ACTUAL_CARDS_COUNT; i += RIDX_SIZE) {
      RandIndices();
      SplitBits swp(  deck[ridx[3]] );
      deck[ridx[3]] = deck[i];
      deck[i      ] = deck[ridx[0]];
      deck[ridx[0]] = deck[i+1];
      deck[i + 1  ] = deck[ridx[1]];
      deck[ridx[1]] = deck[i+2];
      deck[i + 2  ] = deck[ridx[2]];
      deck[ridx[2]] = deck[i+3];
      deck[i + 3  ] = swp;
   }
   VerifyCheckSum();

   // compact down
   uint low = 0;
   uint high = ACTUAL_CARDS_COUNT;
   for (int high = ACTUAL_CARDS_COUNT; high < DECK_ARR_SIZE; high++) {
      if (deck[high].IsBlank()) {
         continue;
      }

      // find a place for it
      for (;;low++) {
         if (deck[low].IsBlank()) {
            // copy & forget. break the checksum
            deck[low] = deck[high];
            break;
         } 
      }
   }
}


