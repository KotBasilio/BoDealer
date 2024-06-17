/************************************************************
 * WALRUS.H                                        17.12.2019
 *
 ************************************************************/
#include "waCrossPlatform.h"
#include "waTasks.h"
#include "waLayout.h"
#include "waAI.h"
#include "waScore.h"
#include "waSemantic.h"
#include "waMulti.h"
#include "waIO.h"

// The main class -- named after Walter the Walrus, whose expertise in and devotion to 
// the Work point count are matched only by the utter mess he makes of bidding and play (c)
class Walrus
{
   friend struct Semantics;
   friend struct WaMulti;
public:
   Walrus();
   Walrus(Walrus *other, const char *nameH, ucell ourShare);
   ~Walrus();

   bool InitByConfig();
   void DetectGoals();
   void MainScan(void);
   ucell DoTheShare();
   void ReportState();
   bool AfterMath();
   bool IsRunning(void) const { return mul.isRunning; }
   const char *GetName() const { return mul.nameHlp; }
   ucell NumFiltered() const { return mul.countToSolve; }
   ucell CloudSize() const { return mul.countShare; }
   Progress* GetProgress() { return &progress; }
   Semantics* GetSemantics() { return &sem; }

protected:
    // Start
    void PrepareBaseDeal(struct deal &dlBase);
    bool StartOscar();
    void InitDeck(void);
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
    void NoticeSacrificePossible(uint tOurs, uint tTheirs);
    void NoticeBidProfit(uint tOurs, uint tTheirs);
    void CompareOurContracts(uint tricksA, uint tricksB);
    void CompareSlams(uint tricksA, uint tricksB);
    void HandleDDSFail(int res);

    // multi-thread
    void LaunchHelpers(Walrus &hA, Walrus &hB);
    void ShowEffortSplit(Walrus &hA, Walrus &hB);
    void DoIteration();
    ucell Remains() const { return (mul.countIterations < mul.countShare) ? mul.countShare - mul.countIterations : 0; }
    uint Gathered() const { return mul.Gathered(); }
    void CoWork(Walrus * other);
    void Supervise(void);
    void MergeResults(Walrus *other);

    // semantics
    void FillSemantic(void);
    void NOP() {}
    void VoidScoring(DdsTricks &tr) {}
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
    void AnnounceSolving();
    void ReportTime();
    void ReportAllLines();
    bool ConsiderNormalZeroLine(int i, ucell sumline);
    void DisplayStatNumbers(int i, ucell sumline);
    void RepeatLineWithPercentages(int i, ucell sumline);
    void HandleFilterLine(int i);
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
   void OrbNorthFillSem(void);
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

