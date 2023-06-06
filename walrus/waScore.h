/************************************************************
 * Walrus Shuffling & Scoring
 ************************************************************/

struct CumulativeScore {
   CumulativeScore();
   s64    ideal;
   s64    bidGame, bidSlam;
   s64    partscore;
   s64    leadS, leadH, leadD, leadC;
   s64    oppContract, oppCtrDoubled;
   s64    ourOther, ourCombo;
   // -- opening lead
   void OpLead3NT(s64& sum, uint tricks);
   void OpLead3NTX(s64& sum, uint tricks);
   void OpLead3Major(s64& sum, uint tricks);
   void OpLead5minor(s64& sum, uint tricks);
   void OpLead5mX(s64& sum, uint tricks);
   // -- opp contract
   void Opp_3MajX(s64& sum, uint tricks);
   void Opp_3Major(s64& sum, uint tricks);
   void Opp_4M(s64& sum, uint tricks);
   void Opp_NV_4MajX(s64& sum, uint tricks);
   void Opp_3NT(s64& sum, uint tricks);
   void Opp_3NTX(s64& sum, uint tricks);
   void Opp_5minor(s64& sum, uint tricks);
   void Opp_2m(s64& sum, uint tricks);
   void Opp_2mX(s64& sum, uint tricks);
   // -- our contracts
   void OurNV6m(uint tricks);
   void OurNV6Maj(uint tricks);
   void OurNV6_No(uint tricks);
   void Our3NT(uint tricks);
   void Our4M(uint tricks);
   void OurNV4M(uint tricks);
   void OurNV4MX(uint tricks);
   void Our5M(uint tricks);
   void OurNV5M(uint tricks);
   void Our5minor(uint tricks);
   void OurNV5minor(uint tricks);
};

struct Shuffler {
   Shuffler();
   SplitBits deck[DECK_ARR_SIZE];
   SplitBits highBits; // placed intentionally after deck

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
   void AssertDeckSize(uint wish);

   // preparations
   void InitDeck(void);
   int  InitSuit(u64 suit, int idx);
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

   void Roll(uint i);
   uint Rand();
   void RandIndices();
   u64  CalcCheckSum();
};
