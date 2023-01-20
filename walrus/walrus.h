/************************************************************
 * WALRUS.H                                        17.12.2019
 *
 ************************************************************/
#include "waCrossPlatform.h"
#include "waTasks.h"
#include "waLayout.h"
#include "waSemantic.h"
#include "waAI.h"

// The main class -- named after Walter the Walrus, 
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
   bool IsRunning(void) const { return mul.isRunning; }
   const char *GetName() const { return mul.nameHlp; }
   uint NumFiltered() const { return mul.countToSolve; }

protected:
    // Start
    void BuildFileNames(void);
    void PrepareBaseDeal(struct deal &dlBase);
    void InitDeck(void);
    int  InitSuit(u64 suit, int idx);
    u64  CalcCheckSum();
    void VerifyCheckSum();

    // Flip over
    void FillFO_MaxDeck();
    void FillFO_39Single();
    void FillFO_39Double();
    void ClearFlipover();

    // withdrawals
    void WithdrawByInput();
    void WithdrawHolding(uint hld, uint waSuitByDds);
    void WithdrawDeuce(uint rankBit, u64 waSuit);
    void WithdrawRank(uint rankBit, u64 waSuit, uint waSuitByDds);
    void WithdrawCard(u64 jo);

    // shuffling
    void SeedRand();
    void StepAsideRand(uint stepAside);
    uint Rand();
    void RandIndices();
    void TestSeed(void);
    void Shuffle();

    // scans
    void ScanTrivial();
    void ScanOrb();
    void ScanKeycards();

    // solving
    void AllocFilteredTasksBuf();
    void SolveSavedTasks();
    void SolveOneByOne(struct deal &dlBase);
    void SolveInChunks(struct deal &dlBase);
    void SolveOneChunk(struct deal &dlBase, struct boards &bo, uint i, uint step);
    void HandleSolvedChunk(struct boards& bo, struct solvedBoards& chunk);
    void HandleSolvedBoard(DdsTricks &tr, deal &cards, futureTricks &fut);
    void NoticeMagicFly(uint trickSuit, uint tricksNT);
    void CountComboScore(uint trickSuit, uint tricksNT);
    void HandleDDSFail(int res);

    // multi-thread
    void LaunchHelpers(Walrus &hA, Walrus &hB);
    void ShowEffortSplit(Walrus &hA, Walrus &hB);
    void DoIteration();
    uint Remains() const { return (mul.countIterations < mul.countShare) ? mul.countShare - mul.countIterations : 0; }
    void CoWork(Walrus * other);
    void Supervise(Walrus *helperA, Walrus *helperB);
    void MergeResults(Walrus *other);

    // semantics
    void FillSemantic(void);
    typedef void (Walrus::*SemFuncType)();
    typedef void (Walrus::*SemScoring)(DdsTricks &tr);
    void NOP() {}
    void VoidScoring(DdsTricks &tr) {}
    u64  SumFirstHand();
    u64  SumSecondHand();
    struct Semantics {
       SemFuncType  onInit;
       SemFuncType  onShareStart;
       SemFuncType  onScanCenter;
       SemFilterOut onFilter;
       SemFuncType  fillFlipover;
       SemScoring   onScoring;
       SemScoring   onSolvedTwice;
       SemFuncType  onAfterMath;
       uint scanCover; // how much iterations covers one scan
       Semantics();
    } sem;

    // scoring
    struct CumulativeScore {
       CumulativeScore();
       s64    ideal;
       s64    bidGame, bidSlam;
       s64    partscore;
       s64    leadS, leadH, leadD, leadC;
       s64    oppContract, oppCtrDoubled;
       s64    ourOther, ourCombo;
       // -- opening lead
       void OpLead3NT    (s64 &sum, uint tricks);
       void OpLead3NTX   (s64 &sum, uint tricks);
       void OpLead3Major (s64 &sum, uint tricks);
       void OpLead5minor (s64 &sum, uint tricks);
       void OpLead5mX    (s64 &sum, uint tricks);
       // -- opp contract
       void Opp_3MajX    (s64 &sum, uint tricks);
       void Opp_3Major   (s64 &sum, uint tricks);
       void Opp_4M       (s64 &sum, uint tricks);
       void Opp_NV_4MajX (s64 &sum, uint tricks);
       void Opp_3NT      (s64 &sum, uint tricks);
       void Opp_3NTX     (s64 &sum, uint tricks);
       void Opp_5minor   (s64& sum, uint tricks);
       void Opp_2m       (s64& sum, uint tricks);
       void Opp_2mX      (s64 &sum, uint tricks);
       // -- our contracts
       void OurNV6m      (uint tricks);
       void OurNV6Maj    (uint tricks);
       void OurNV6_No    (uint tricks);
       void Our3NT       (uint tricks);
       void Our4M        (uint tricks);
       void OurNV4M      (uint tricks);
       void OurNV4MX     (uint tricks);
       void Our5M        (uint tricks);
       void OurNV5M      (uint tricks);
       void Our5minor    (uint tricks);
       void OurNV5minor  (uint tricks);
    } cumulScore;
    void Score_4Major(DdsTricks &tr);
    void Score_NV_4Major(DdsTricks &tr);
    void Score_NV_Doubled4Major(DdsTricks &tr);
    void Score_5Major(DdsTricks &tr);
    void Score_NV_5Major(DdsTricks &tr);
    void Score_5Minor(DdsTricks& tr);
    void Score_NV_5Minor(DdsTricks &tr);
    void Score_3NT(DdsTricks &tr);
    void Score_MagicFly(DdsTricks& tr);
    void Score_2m(DdsTricks &tr);
    void Score_Doubled3NT(DdsTricks &tr);
    void Score_NV6Major(DdsTricks &tr);
    void Score_NV6Minor(DdsTricks &tr);
    void Score_NV6NoTrump(DdsTricks &tr);
    void Score_OpLead3Major(DdsTricks &tr);
    void Score_OpLead3NT(DdsTricks &tr);
    void Score_OpLead3NTX(DdsTricks &tr);
    void Score_OpLead5D(DdsTricks &tr);
    void Score_OpLead5DX(DdsTricks &tr);
    void Score_Opp3MajorDoubled(DdsTricks& tr);
    void Score_Opp3Major(DdsTricks& tr);
    void Score_Opp4MajorDoubled(DdsTricks &tr);
    void Score_Opp4Major(DdsTricks& tr);
    void Score_Opp3NT(DdsTricks& tr);
    void HitByScore(DdsTricks &tr, uint made, uint row = IO_ROW_OUR_DOWN);

    // UI
    struct Progress {
       Progress();
       uint hitsCount[HCP_SIZE][CTRL_SIZE];
       uint step, went, margin;
       uint countExtraMarks;
       void Init(uint _step);
       bool Step();
       void Up(uint idx);
    } progress;
    struct MiniUI {
       bool  exitRequested;
       bool  firstAutoShow;
       int   irGoal, irBase, irFly;
       char  declTrump[10], declSeat[10], seatOnLead[10], theirTrump[10];
       MiniUI();
       void Run();
    } ui;
    void InitMiniUI(int trump, int first);
    void MiniReport(uint toGo);
    void CalcHitsForMiniReport(uint * hitsRow, uint * hitsCamp);
    int  PokeScorerForTricks();
    void CleanupStats();
    void ShowProgress(uint idx);

    // Multi-thread
    struct Multi {
       Multi();
       // main scan part
       bool             isRunning;
       const char *     nameHlp;
       uint             countIterations, countShare, countSolo;
       // aftermath double-dummy
       uint             maxTasksToSolve;
       DdsPack    *     arrToSolve;
       uint             countToSolve;
    } mul;

    // Shuffling
    struct Shuf {
       Shuf();
       SplitBits        deck[DECK_ARR_SIZE];
       SplitBits        highBits;
       u64              checkSum;
       uint             oldRand;
       uint             ridx[RIDX_SIZE];// RandIndices() <-> Shuffle()
       uint             cardsInDeck;
    } shuf;

private:
   WaFilter         filter;

   // scan patterns
   void Orb_FillSem(void);
   uint Orb_ClassifyHands(uint &camp, SplitBits &sum, SplitBits &lho, SplitBits &rho);
   void Orb_SaveForSolver(SplitBits &partner, SplitBits &resp, SplitBits &notrump);
   void Orb_Interrogate(DdsTricks &tr, deal &cards, struct futureTricks &fut);
   bool Orb_ApproveByFly(deal& cards);
   void Orb_ReSolveAndShow(deal &cards);
   uint KC_ClassifyHand(uint &ba, SplitBits &sum);
   uint CountKeyCards(SplitBits &hand);
};


