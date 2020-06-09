/************************************************************
 * WALRUS.H                                        17.12.2019
 *
 ************************************************************/
#include <stdio.h> // printf
#include "waSemantic.h"

#ifdef _DEBUG
   //const uint MAX_ITERATION = 20*1000000;// 20 mln 
   const uint MAX_ITERATION = 1000000;// 1 mln 
   //const uint MAX_ITERATION = 100000;// 0.1 mln
   //const uint MAX_ITERATION = 10000;
   const uint MAX_TASKS_TO_SOLVE = 10240;
   //#define SKIP_HELPERS
#else
   //const uint MAX_ITERATION = 4001000000;// 4 mlrd + 1 millon gratis
   //const uint MAX_ITERATION = 2201000000;// 2 mlrd + 1 millon gratis
   //const uint MAX_ITERATION = 1001000000;// 1 mlrd + 1 millon gratis
   const uint MAX_ITERATION = 501000000;// half mlrd
   //const uint MAX_ITERATION = 1201000000;// 1201 mln
   const uint MAX_TASKS_TO_SOLVE = 4*1000*1000;
#endif

// all cards + flip over 12 cards
const uint FLIP_OVER_SIZE = 12;
const uint FLIP_OVER_START_IDX = ACTUAL_CARDS_COUNT;
const uint DECK_ARR_SIZE = 64;// random does work to this range
const uint RIDX_SIZE = 4;
const uint SUPERVISE_CHUNK = 10;
const uint SUPERVISE_REASONABLE = SUPERVISE_CHUNK * ACTUAL_CARDS_COUNT * 2;

// hitsCount[][]; distribution 
// -- rows are big factor (aka hcp, 0 - 40)
// -- columns are smaller factor (aka controls, 0 - 12)
const int HCP_SIZE = 40 + 1;// to address 0 - 40
const int CTRL_SIZE = 12 + 4;// to get 16 as padding

// file names
#define START_FROM_FNAME "start_from.txt"
#define INDICES_FNAME  "Indices.txt"
#define OUT_FNAME      "stm%d%d%d%d%d%d%d%d%d%d.txt"
#define PROGRESS_FNAME "w09progress.txt"
const int FNAME_SIZE = 128 + 64;
static struct tFileNames {
    char StartFrom[FNAME_SIZE];
    char Indices[FNAME_SIZE];
    char Progress[FNAME_SIZE];
    char Solution[FNAME_SIZE];
} namesBase;

// split bits card to operate super-fast
struct SplitBits {
    union SBUnion {// low bytes first, so never overflow
        SBITS_CHARS_LAYOUT;
        u64 jo;
    };
    SBUnion card;

    SplitBits()                  { card.jo = 0L; }
    explicit SplitBits(u64 jo)   { card.jo = jo; }
    SplitBits(uint hld, uint waPos);
    u16 CountAll();
    SBITS_SEMANTIC_OPS
};
inline u16 SplitBits::CountAll()
{
   return card.w.c.Count() + 
          card.w.d.Count() + 
          card.w.h.Count() + 
          card.w.s.Count();
}
extern SplitBits sbBlank;

// twelve-layout lets counting some parameters in parallel, then queried
// -- high-card points
struct twlHCP 
{
   twlHCP(SplitBits &hand);
   uint s, h, d, c, total;
};
// -- lengths
struct twLengths
{
   twLengths(SplitBits &hand);
   uint s, h, d, c;
};

#include "waAI.h"

// main class -- named after Walter the Walrus, 
// whose expertise in and devotion to 
// the Work point count are matched only 
// by the utter mess he makes of bidding and play (c)
class Walrus
{
public:
   Walrus();
   Walrus(Walrus *other, const char *nameH, int ourShare);
   ~Walrus();

   bool LoadInitialStatistics(const char *fname);
   void MainScan(void);
   uint DoTheShare();
   void ReportState(char *header);
   bool AfterMath();
   bool IsRunning(void) const { return isRunning; }
   const char *GetName() const { return nameHlp; }
   uint NumFiltered() const { return countToSolve; }

protected:
    void BuildFileNames(void);
    void MiniReport(uint toGo);
    void FillSemantic(void);
    void InitDeck(void);
    int  InitSuit(u64 suit, int idx);
    u64  SumFirstHand();
    u64  SumSecondHand();
    void LaunchHelpers(Walrus &hA, Walrus &hB);
    void DoIteration();

    u64  CalcCheckSum();
    void VerifyCheckSum();

    void FillFO_MaxDeck();
    void FillFO_39Single();
    void FillFO_39Double();
    void ClearFlipover();

    void ScanTrivial();
    void ScanOrb();

    void WithdrawByInput();
    void AllocFilteredTasksBuf();
    void SolveSavedTasks();
    void SolveOneByOne(struct deal &dlBase);
    void SolveInChunks(struct deal &dlBase);
    void SolveOneChunk(struct deal &dlBase, struct boards &bo, uint i, uint step);

    uint Remains() const { return (countIterations < countShare) ? countShare - countIterations : 0; }
    void CoWork(Walrus * other);
    void Supervise(Walrus *helperA, Walrus *helperB);
    void MergeResults(Walrus *other);

    void SeedRand();
    void StepAsideRand(uint stepAside);
    uint Rand();
    void RandIndices();
    void TestSeed(void);
    void Shuffle();

    typedef void (Walrus::*SemFuncType)();
    typedef uint (Walrus::*SemFilterOut)(SplitBits &part, uint &camp, SplitBits &lho, SplitBits &rho);
    typedef void (Walrus::*SemScoring)(DdsTricks &tr);
    void NOP() {}
    uint FilterRejectAll(SplitBits &part, uint &camp, SplitBits &lho, SplitBits &rho) { camp = 2; return 1; }
    void VoidScoring(DdsTricks &tr) {}
    struct Semantics {
       SemFuncType  onInit;
       SemFuncType  onShareStart;
       SemFuncType  onScanCenter;
       SemFilterOut onFilter;
       SemFuncType  fillFlipover;
       SemScoring   onScoring;
       SemFuncType  onAfterMath;
       uint scanCover; // how much iterations covers one scan
       Semantics();
    } sem;

    struct CumulativeScore {
       CumulativeScore();
       u64    ideal;
       s64    bidGame;
       s64    partscore;
       s64    leadS, leadH, leadD, leadC;
       void OpLead3NT(s64 &lead, uint tricks);
    } cumulScore;

    struct Progress
    {
       uint step, went, margin;
       void Init(uint _step);
       bool Step();
       void Up(uint idx);
    } progress;

    struct MiniUI
    {
       bool  exitRequested;
       int   irGoal;
       int   irBase;
       MiniUI();
       void Run();
    } ui;

private:
   const char *     nameHlp;
   bool             isRunning;
   uint             countIterations, countShare, countSolo;
   SplitBits        deck[DECK_ARR_SIZE];
   SplitBits        highBits;
   SaDeck           saDeck;
   u64              checkSum;
   uint             oldRand;
   uint             hitsCount[HCP_SIZE][CTRL_SIZE];
   uint             ridx[RIDX_SIZE];// RandIndices() <-> Shuffle()
   DdsPack    *     arrToSolve;
   uint             countToSolve;

   void Orb_FillSem(void);
   uint Orb_ClassifyHands(uint &foo, SplitBits &sum, SplitBits &lho, SplitBits &rho);
   void Orb_SaveForSolver(SplitBits &partner, SplitBits &resp, SplitBits &notrump);
   void Orb_Interrogate(int &irGoal, DdsTricks &tr, deal &cards, struct futureTricks &fut);
   void Orb_ReSolveAndShow(deal &cards);

   void ShowProgress(uint idx);
   void InitMiniUI();
   void RunMiniUI();

   void Score_4Major(DdsTricks &tr);
   void Score_3NT(DdsTricks &tr);
   void Score_OpLead3NT(DdsTricks &tr);
   void Score_Cumul4M(DdsTricks &tr);
   void Score_Cumul3NT(DdsTricks &tr);

   void WithdrawHolding(uint hld, uint waSuitByDds);
   void WithdrawDeuce(uint rankBit, u64 waSuit);
   void WithdrawRank (uint rankBit, u64 waSuit, uint waSuitByDds);
   void WithdrawCard(u64 jo);
   void PrepareBaseDeal(struct deal &dlBase);

   // filters
   uint R55_FilterOut(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint JuneVZ_FilterOut(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint Tricolor_FilterOut(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint TriSunday_FilterOut(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
};

