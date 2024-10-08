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
   // replicate system rand(), strip out the code of thread handling
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

bool Shuffler::AssertDeckSize(uint wish, char const* hint)
{
   if (cardsInDeck == wish) {
      return true;
   }

   printf("\nERROR: Wrong count of cards discarded: %d is left.\nHint: %s\n", cardsInDeck, hint);
   return false;
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

u64 Shuffler::SumFirstHand()
{
   // sum up the first 13 cards -- kind of unrolled loop
   return (
      deck[ 0].card.jo +
      deck[ 1].card.jo +
      deck[ 2].card.jo +
      deck[ 3].card.jo +
      deck[ 4].card.jo +
      deck[ 5].card.jo +
      deck[ 6].card.jo +
      deck[ 7].card.jo +
      deck[ 8].card.jo +
      deck[ 9].card.jo +
      deck[10].card.jo +
      deck[11].card.jo +
      deck[12].card.jo
   );
}

u64 Shuffler::SumSecondHand()
{
   // sum up next 13 cards
   return (
      deck[13].card.jo +
      deck[14].card.jo +
      deck[15].card.jo +
      deck[16].card.jo +
      deck[17].card.jo +
      deck[18].card.jo +
      deck[19].card.jo +
      deck[20].card.jo +
      deck[21].card.jo +
      deck[22].card.jo +
      deck[23].card.jo +
      deck[24].card.jo +
      deck[25].card.jo
   );
}

u64 Shuffler::Sum3rdHand()
{
   // sum up yet next 13 cards
   return (
      deck[26].card.jo +
      deck[27].card.jo +
      deck[28].card.jo +
      deck[29].card.jo +
      deck[30].card.jo +
      deck[31].card.jo +
      deck[32].card.jo +
      deck[33].card.jo +
      deck[34].card.jo +
      deck[35].card.jo +
      deck[36].card.jo +
      deck[37].card.jo +
      deck[38].card.jo
   );
}

void Shuffler::WithdrawHolding(uint hld, uint waPosByDds)
{
   // scan cards
   u64 waSuit = 1LL << waPosByDds;
   WithdrawDeuce(hld & R2, waSuit);
   WithdrawRank (hld & R3, waSuit, waPosByDds);
   WithdrawRank (hld & R4, waSuit, waPosByDds);
   WithdrawRank (hld & R5, waSuit, waPosByDds);
   WithdrawRank (hld & R6, waSuit, waPosByDds);
   WithdrawRank (hld & R7, waSuit, waPosByDds);
   WithdrawRank (hld & R8, waSuit, waPosByDds);
   WithdrawRank (hld & R9, waSuit, waPosByDds);
   WithdrawRank (hld & RT, waSuit, waPosByDds);
   WithdrawRank (hld & RJ, waSuit, waPosByDds);
   WithdrawRank (hld & RQ, waSuit, waPosByDds);
   WithdrawRank (hld & RK, waSuit, waPosByDds);
   WithdrawRank (hld & RA, waSuit, waPosByDds);
}

