/************************************************************
 * WALRUS.H                                        17.12.2019
 *
 ************************************************************/
#include "waCrossPlatform.h"
#include "waTasks.h"
#include "waAI.h"
#include "waScore.h"
#include "waShuffle.h"
#include "waSemantic.h"
#include "waMulti.h"
#include "waIO.h"

// The main class -- named after Walter the Walrus, whose expertise in and devotion to 
// the Work point count are matched only by the utter mess he makes of bidding and play (c)
class Walrus
{
   friend struct Semantics;
   friend struct WaMulti;
   friend struct WaConfig;
public:
   Walrus();
   Walrus(Walrus *other, const char *nameH, ucell ourShare);
   ~Walrus();

   // top level
   bool InitByConfig();
   void Main();
   void ScanAsHelper();

   // access
   Progress* GetProgress()     { return &progress; }
   Semantics* GetSemantics()   { return &sem; }
protected:
   // "main"
   void ScanFixedTask(void);
   void AfterMath();
   void ReportState();
   void ScanStray(void);
   void EndStray(void);

   // inits
   void PrepareBaseDeal(struct deal &dlBase);
   bool StartOscar();
   void InitDeck(void);
   void DetectGoals();
   void DeprDetectGoals(void);

    // withdrawals
    void WithdrawByInput();
    void WithdrawHolding(uint hld, uint waSuitByDds);
    void WithdrawDeuce(uint rankBit, u64 waSuit);
    void WithdrawRank(uint rankBit, u64 waSuit, uint waSuitByDds);

    // scans
    void ScanTrivial();
    void ScanKeycards();
    void Scan4Hands();
    void Scan3FixedWest();
    void Scan3FixedNorth();

    // solving
    void AllocFilteredTasksBuf();
    void SolveSavedTasks();
    void SolveOneByOne(struct deal &dlBase);
    void SolveInChunks();
    void SolveOneChunk(uint i, uint step);
    void HandleSolvedChunk(struct boards& bo, struct solvedBoards& chunk);
    void SolveSecondTime(struct boards& bo, struct solvedBoards& chunk);
    void NoticeMagicFly(uint trickSuit, uint tricksNT);
    void NoticeBidProfit(uint tOurs, uint tTheirs);
    void CompareOurContracts(uint tricksA, uint tricksB);
    void CompareSlams(uint tricksA, uint tricksB);
    void HandleDDSFail(int res);

    // multi-thread
    void  LaunchHelpers();
    void  ShowEffortSplit(Walrus &hA, Walrus &hB);
    void  DoIteration();
    uint  NumFiltered() const   { return mul.countToSolve; }
    ucell CloudSize() const     { return mul.countShare; }
    ucell Remains() const { return (mul.countIterations < CloudSize()) ? CloudSize() - mul.countIterations : 0; }
    void  CoWork(Walrus * other);
    void  Supervise(void);
    void  MergeResults(Walrus *other);
    void  ClearHelpers();

    // joined effort
    ucell DoTheShare();
    bool IsRunning(void) const  { return mul.isRunning; }
    const char *GetName() const { return mul.nameHlp; }

    // semantics
    bool InitSemantics();
    void FillSemantic(void);
    void NOP() {}
    void VoidDepScoring(DdsTricks &tr) {}
    void VoidPostmortem(DdsTricks& tr, deal& cards) {}
    void VoidAdded(twContext* lay) {}
    void VoidSecondSolve(boards& bo, solvedBoards& solved) {}
    void VoidCompare(uint trickSuit, uint tricksNT) {}
    void GrabSplinterVariant(twContext* lay);
    u64  SumFirstHand();
    u64  SumSecondHand();
    u64  Sum3rdHand();
    Semantics &sem;

    // scoring
    CumulativeScore cumulScore;
    void ScoreWithPrimary(DdsTricks &tr);
    void ScoreWithSecondary(DdsTricks &tr);
    void Score_OpLead3Major(DdsTricks &tr);
    void Score_OpLead3NT(DdsTricks &tr);
    void Score_OpLead3NTX(DdsTricks &tr);
    void Score_OpLead5D(DdsTricks &tr);
    void Score_OpLead5DX(DdsTricks &tr);
    void AddScorerValues(char* tail);
    void HitByTricks(DdsTricks &tr, uint made, uint row = IO_ROW_OUR_DOWN);
    void PostmortemHCP(DdsTricks& tr, deal& cards);
    void PostmortemSuit(DdsTricks& tr, deal& cards);

    // progress and reports
    Progress progress;
    void SummarizeFiltering();
    void ReportTime();
    void ReportAllLines();
    bool ConsiderNormalZeroLine(int i, ucell sumline);
    void DisplaySolvingStats(int i, ucell sumline);
    void DisplayFilterNumbers(uint ip);
    void DisplayStatCell(ucell cell);
    void RepeatLineWithPercentages(int i, ucell sumline);
    void HandleFilterLine(uint ip);
    bool IsFilterLine(int i);
    void MiniReport(ucell toGo);
    void ShowProgress(ucell idx);
    bool RegularBalanceCheck();
    void UpdateFarColumnUI();
    void ShowPercentages(s64 sumRows);
    void ShowBiddingLevel(s64 sumRows);
    void ShowTheirScore(s64 doneTheirs);
    void ShowOptionalReports(s64 doneOurs, s64 doneTheirs);
    void ShowDetailedReportHighcards();
    void AddOverallStats(int idx);
    void ShowAdvancedStatistics(int idx);
    void AddSetContracts(int idx);
    void AddMadeContracts(int idx);
    void DisplayGraphStatData(int idx);
    void ShowDetailedReportControls();
    void ShowDetailedReportSuit();
    void ShowMiniHits(ucell* hitsRow, ucell* hitsCamp);

    // UI
    MiniUI ui;
    void InitMiniUI();
    void DisplayBoard(twContext* lay) { ui.DisplayBoard(lay); }
    int  PokeScorerForTricks();
    int  PokeOtherScorer();

    // Multi-thread
    WaMulti mul;

private:
   Shuffler shuf;
   WaFilter filter;

   // scan patterns
   // -- common parts of scans
   void ClassifyAndPull(twContext* lay);
   void ClassifyAndPush(twContext* lay);
   void ClassifyOnPermute(twContext* lay);
   // -- 3-hands scan is like orbiting around a hand
   void SemanticsToOrbitFixedHand(void);
   void Orb_Interrogate(DdsTricks &tr, deal &cards, struct futureTricks &fut);
   bool Orb_ApproveByFly(deal& cards);
   void Orb_ReSolveAndShow(deal &cards);
   void OrbNorthClassify(twContext *lay);
   // -- 4-hands scan
   void FourHandsFillSem(void);
   void Permute6(SplitBits a, SplitBits b, SplitBits c);
   void Permute24(SplitBits a, SplitBits b, SplitBits c);
   void Classify6(twContext *lay);
   // -- other scans
   uint KeyCards_ClassifyHand(uint &ba, SplitBits &sum);
   uint CountKeyCards(SplitBits &hand);
};

