/************************************************************
 * WALRUS shuffler                                       2024
 ************************************************************/

 // all cards + flip over 12 cards
const uint FLIP_OVER_SIZE = 12;
const uint FLIP_OVER_START_IDX = ACTUAL_CARDS_COUNT;
const uint DECK_ARR_SIZE = 64;// random does work to this range
const uint RIDX_SIZE = 4;

struct Shuffler {
   Shuffler();
   SplitBits deck[DECK_ARR_SIZE];
   SplitBits highBits;  // placed intentionally after deck
   SplitBits thrownOut; // cards that are not for shuffle

   // rand work
   void SeedRand();
   void TestSeed(const char* nameHlp);
   void StepAsideRand(uint stepAside);
   void Shuffle();
   void NOP() {}

   // controlling
   void StoreCheckSum();
   void VerifyCheckSum();
   u64  CheckSum() { return checkSum; }
   void AssertDeckSize(uint wish, char const* hint);

   // preparations
   void InitDeck(void);
   void WithdrawCard(u64 jo);
   void FillFO_MaxDeck();
   void FillFO_39Single();
   void FillFO_39Double();
   void ClearFlipover();

private:
   u64       checkSum;
   uint      oldRand;
   uint      ridx[RIDX_SIZE];// RandIndices() <-> Shuffle()
   uint      cardsInDeck;

   int  InitSuit(u64 suit, int idx);
   uint Rand();
   void RandIndices();
   void Roll(uint i);
   u64  CalcCheckSum();
};
