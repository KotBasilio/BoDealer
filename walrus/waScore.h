/************************************************************
 * Walrus Shuffling & Scoring
 ************************************************************/

struct CumulativeScore {
   CumulativeScore();
   s64    ideal;
   s64    bidPartscore, bidGame, bidSlam;
   union {
      struct {
         s64    leadS, leadH, leadD, leadC;
      };
      struct {
         s64    oppContract, oppCtrDoubled;
         s64    ourOther, ourHedging;
      };
   };
   // -- opening lead
   void OpLead3NT(s64& sum, uint tricks);
   void OpLead3NTX(s64& sum, uint tricks);
   void OpLead3Major(s64& sum, uint tricks);
   void OpLead5minor(s64& sum, uint tricks);
   void OpLead5mX(s64& sum, uint tricks);
   // -- opp contract
   void Score_Opp4Major(uint tricks);
   void Score_Opp3Major(uint tricks);
   void Score_Opp5Minor(uint tricks);
   void Score_Opp3NT(uint tricks);
   // -- our contracts
   void VoidScoring(uint tricks) {}
   void OurNV6m(uint tricks);
   void OurNV6Maj(uint tricks);
   void OurNV6_No(uint tricks);
   void Our6_No(uint tricks);
   void OurNV7m(uint tricks);
   void Our3NT(uint tricks);
   void Our4M(uint tricks);
   void Our1NT(uint tricks);
   void Our2M(uint tricks);
   void Our4minor(uint tricks);
   void OurNV4M(uint tricks);
   void OurNV4MX(uint tricks);
   void Our5M(uint tricks);
   void OurNV5M(uint tricks);
   void Our5minor(uint tricks);
   void OurNV5minor(uint tricks);
   void OurNV5mX(uint tricks);
   // -- allowing for scorer2
   void Our2_NV7NT(uint tricks);

   // linear-score oriented
   struct Adjustable {
      Adjustable() : linearBase(nullptr), outSum(nullptr) {}
      bool Init(s64 &out, const char* code);
      void operator () (uint tricks);
   private:
      const s64* linearBase;
      s64* outSum;
   };
   Adjustable  prima, secunda, tertia;
   void Primary   (uint tricks) { prima(tricks); }
   void Secondary (uint tricks) { secunda(tricks); }
   void Tertiary  (uint tricks) { tertia(tricks); }

private:
   void Opp_3MajX(s64& sum, uint tricks);
   void Opp_3Major(s64& sum, uint tricks);
   void Opp_4M(s64& sum, uint tricks);
   void Opp_4MajX(s64& sum, uint tricks);
   void Opp_NV_4MajX(s64& sum, uint tricks);
   void Opp_3NT(s64& sum, uint tricks);
   void Opp_3NTX(s64& sum, uint tricks);
   void Opp_5minor(s64& sum, uint tricks);
   void Opp_5minorX(s64& sum, uint tricks);
   void Opp_2m(s64& sum, uint tricks);
   void Opp_2mX(s64& sum, uint tricks);
   void OurNV7NT(s64& sum, uint tricks);
};

// ------------------------------------------------------------------- SHUFFLER part
// all cards + flip over 12 cards
const uint FLIP_OVER_SIZE = 12;
const uint FLIP_OVER_START_IDX = ACTUAL_CARDS_COUNT;
const uint DECK_ARR_SIZE = 64;// random does work to this range
const uint RIDX_SIZE = 4;
const uint SUPERVISE_CHUNK = 10;
const uint SUPERVISE_REASONABLE = SUPERVISE_CHUNK * ACTUAL_CARDS_COUNT * 2;

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

extern void PrepareLinearScores();
extern const s64* FindLinearScore(const char* code);

