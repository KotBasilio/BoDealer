/************************************************************
* Walrus project                                        2019
* Randomizer and shuffler parts
* replicate system rand(), strip the code out of thread handling
*
************************************************************/
#include <ctime>
#include <stdlib.h>  // RAND_MAX
#include "walrus.h"
#include HEADER_CURSES

#define HIBITS  ((BO_SPADS+BO_HEART+BO_DIAMD+BO_CLUBS) << 1) // it counts as a hand with two deuces in each suit -- easily detected and doesn't cause an overflow

Shuffler::Shuffler()
   : highBits(HIBITS) 
   , thrownOut(0)
   , checkSum(0)
   , oldRand(0)
   , cardsInDeck(0)
{}

void Shuffler::SeedRand()
{
   oldRand = (uint)time(0);
}

uint Shuffler::Rand()
{
   return (((oldRand = oldRand * 214013L + 2531011L) >> 16) & RAND_MAX);
}

void Shuffler::StepAsideRand(uint stepAside)
{
   oldRand += stepAside;
}

u64 Shuffler::CalcCheckSum()
{
   u64 jo = 0;
   for (auto sb : deck) {
      jo += sb.card.jo;
   }
   return jo;
}

void Shuffler::StoreCheckSum()
{
   checkSum = CalcCheckSum();
   thrownOut.AsComplement(checkSum);
}

void Shuffler::VerifyCheckSum()
{
#ifdef _DEBUG
   if (CalcCheckSum() != checkSum) {
      printf("\nERROR: checksum failure\n");
      PLATFORM_GETCH();
   }
#endif
}

void Shuffler::AssertDeckSize(uint wish, char const* hint)
{
   if (cardsInDeck != wish) {
      printf("\nERROR: Wrong count of cards discarded: %d is left.\nHint: %s\n", cardsInDeck, hint);
      PLATFORM_GETCH();
   }
}

void Shuffler::ClearFlipover()
{
   for (int i = FLIP_OVER_START_IDX; i < DECK_ARR_SIZE; i++) {
      deck[i] = sbBlank;
   }
}

void Shuffler::TestSeed(const char* nameHlp)
{
   RandIndices();
   printf("%s\t:%02d %02d %02d %02d ", nameHlp
      , ridx[0], ridx[1], ridx[2], ridx[3]);
   RandIndices();
   printf("%02d %02d %02d %02d\n"
      , ridx[0], ridx[1], ridx[2], ridx[3]);
}

// ----------------------- FLIP-OUT-fills
void Shuffler::FillFO_MaxDeck() // when we pick one hand only
{
   for (int i = FLIP_OVER_START_IDX, j = 0; i < DECK_ARR_SIZE; i++, j++) {
      deck[i] = deck[j];
   }
   // no need to place highBits -- it's always in place after the deck end
}

void Shuffler::FillFO_39Single() // when we pick one hand, then the other hand. total of two
{
   int i = FLIP_OVER_START_IDX, j = 0;
   for (; j < REMOVED_CARDS_COUNT - 1; i++, j++) {
      deck[i] = deck[j];
   }
   deck[i] = highBits;
}

void Shuffler::FillFO_39Double() // when we pick one hand, then the other, then the next one
{
   int i = FLIP_OVER_START_IDX, j = 0;
   for (; j < 2*REMOVED_CARDS_COUNT - 1; i++, j++) {
      deck[i] = deck[j];
   }
   // no need to place highBits -- it's always in place after the deck end
}

void Shuffler::RandIndices()
{
   // we get 4 random indices from one big random 
   uint r = Rand();
   ridx[0] = r & 0x3f;
   ridx[1] = (r >> 3) & 0x3f;
   ridx[2] = (r >> 6) & 0x3f;
   ridx[3] = (r >> 9) & 0x3f;
}

void Shuffler::Roll(uint i)
{
   // send 4 cards from its positions along random loop
   SplitBits swp(
      deck[ridx[3]]);
      deck[ridx[3]] = deck[i + 0];
                      deck[i + 0] =
      deck[ridx[0]];
      deck[ridx[0]] = deck[i + 1];
                      deck[i + 1] =
      deck[ridx[1]];
      deck[ridx[1]] = deck[i + 2];
                      deck[i + 2] = 
      deck[ridx[2]];
      deck[ridx[2]] = deck[i + 3];
                      deck[i + 3] = swp;
}

void Shuffler::Shuffle()
{
   // full indices
   for (uint i = 0; i < ACTUAL_CARDS_COUNT; i += RIDX_SIZE) {
      RandIndices();
      Roll(i);
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
      for (;; low++) {
         if (deck[low].IsBlank()) {
            // copy & forget. break the checksum
            deck[low] = deck[high];
            break;
         }
      }
   }
}

void Shuffler::InitDeck(void)
{
   int i = 0;
   i = InitSuit(BO_CLUBS, i);
   i = InitSuit(BO_DIAMD, i);
   i = InitSuit(BO_HEART, i);
   i = InitSuit(BO_SPADS, i);
   cardsInDeck = i;
}

void Shuffler::WithdrawCard(u64 jo)
{
   int high = SOURCE_CARDS_COUNT;
   while (deck[high].IsBlank()) {
      high--;
   }

   for (uint i = 0; i < SOURCE_CARDS_COUNT; i++) {
      if (deck[i].card.jo == jo) {
         deck[i] = deck[high];
         deck[high--] = sbBlank;
         cardsInDeck--;
         return;
      }
   }

   printf("card to withdraw is not found.\n");
   DEBUG_UNEXPECTED;
}

