/************************************************************
 * WALRUS shuffler                                       2024
 ************************************************************/

 // all cards + flip over 12 cards
constexpr uint FLIP_OVER_SIZE = 12;
constexpr uint DECK_ARR_SIZE = 64;// random does work to this range
constexpr uint RIDX_SIZE = 4;

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

   // withdrawals
   void WithdrawHolding(uint hld, uint waSuitByDds);
   void WithdrawDeuce(uint rankBit, u64 waSuit);
   void WithdrawRank(uint rankBit, u64 waSuit, uint waSuitByDds);

   // combining
   u64  SumFirstHand();
   u64  SumSecondHand();
   u64  Sum3rdHand();

   // controlling
   void StoreCheckSum();
   void VerifyCheckSum();
   u64  CheckSum() { return checkSum; }
   bool AssertDeckSize(uint wish, char const* hint);

   // preparations
   void InitDeck(void);
   void WithdrawCard(u64 jo);
   void FillFlipOverMaxDeck();
   void FillFlipOver39Single();
   void FillFlipOver39Double();
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
