/************************************************************
 * WALRUS.H                                        17.12.2019
 *
 ************************************************************/
#include "waCrossPlatform.h"
#include "waTasks.h"
#include "waLayout.h"
#include "waSemantic.h"
#include "waAI.h"
#include "waScore.h"

// The main class -- named after Walter the Walrus, whose expertise in and devotion to 
// the Work point count are matched only by the utter mess he makes of bidding and play (c)
class Walrus
{
public:
   Walrus();
   Walrus(Walrus *other, const char *nameH, int ourShare);
   ~Walrus();

   bool InitByConfig();
   void MainScan(void);
   uint DoTheShare();
   void ReportState();
   bool AfterMath();
   bool IsRunning(void) const { return mul.isRunning; }
   const char *GetName() const { return mul.nameHlp; }
   uint NumFiltered() const { return mul.countToSolve; }

protected:
    // Start
    void PrepareBaseDeal(struct deal &dlBase);
    void InitDeck(void);
    waFileNames namesBase;

    // withdrawals
    void WithdrawByInput();
    void WithdrawHolding(uint hld, uint waSuitByDds);
    void WithdrawDeuce(uint rankBit, u64 waSuit);
    void WithdrawRank(uint rankBit, u64 waSuit, uint waSuitByDds);

    // scans
    void ScanTrivial();
    void ScanOrb();
    void ScanKeycards();
    void Scan4Hands();
    void Permute(SplitBits a, SplitBits b, SplitBits c);
    void ClassifyOnScan(SplitBits a, SplitBits b, SplitBits c);

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
    typedef void (Walrus::* SemFuncType)();
    typedef void (Shuffler::* ShufflerFunc)();
    typedef void (Walrus::*SemScoring)(DdsTricks &tr);
    void NOP() {}
    void VoidScoring(DdsTricks &tr) {}
    u64  SumFirstHand();
    u64  SumSecondHand();
    u64  Sum3rdHand();
    struct Semantics {
       SemFuncType  onInit;
       SemFuncType  onShareStart;
       SemFuncType  onScanCenter;
       ShufflerFunc fillFlipover;
       DepFilterOut onFilter;
       SemScoring   onScoring;
       SemScoring   onSolvedTwice;
       SemFuncType  onAfterMath;
       uint scanCover; // how much iterations covers one scan
       Semantics();
    } sem;

    // scoring
    CumulativeScore cumulScore;
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

    // UI -- parts will migrate to Oscar project
    Progress progress;
    struct MiniUI {
       bool  exitRequested;
       bool  firstAutoShow;
       int   irGoal, irBase, irFly;
       int   farCol;
       char  declTrump[10], declSeat[10], seatOnLead[10], theirTrump[10];
       MiniUI();
       void Run();
    } ui;
    void InitMiniUI(int trump, int first);
    void DetectFarColumn();
    void ReportState(char* header, u64 delta1, u64 delta2 = 0);
    void ReportLine(uint sumline, int i);
    void MiniReport(uint toGo);
    void ReportFilteringResults();
    void CalcHitsForMiniReport(uint* hitsRow, uint* hitsCamp);
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
       uint       maxTasksToSolve;
       DdsTask*   arrToSolve;
       uint       countToSolve;
    } mul;

private:
   Shuffler shuf;
   WaFilter filter;

   // scan patterns
   // -- 3-hands scan is like orbiting around a hand
   void Orb_FillSem(void);
   void Orb_Classify(SplitBits& a, SplitBits& b, SplitBits& c);
   void Orb_Interrogate(DdsTricks &tr, deal &cards, struct futureTricks &fut);
   bool Orb_ApproveByFly(deal& cards);
   void Orb_ReSolveAndShow(deal &cards);
   // -- others
   uint KeyCards_ClassifyHand(uint &ba, SplitBits &sum);
   uint CountKeyCards(SplitBits &hand);
};

