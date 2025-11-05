/************************************************************
 * WALRUS.H                                        17.12.2019
 *
 ************************************************************/
#include "waCrossPlatform.h"
#include "waTasks.h"
#include "waAI.h"
#include "waScore.h"
#include "waShuffle.h"
#include "waMulti.h"
#include "waSemantic.h"
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
   bool StartOscar();
   void InitDeck();
   twlHCP ParsePbnDeal();
   void WithdrawByInput();
   void DetectScorerGoals();

   // scans
   void ScanTrivial();
   void ScanKeycards();
   void Scan4Hands();
   void Scan3FixedWest();
   void Scan3FixedNorth();

   // solving
   void SolveSavedTasks();
   void SolveOneByOne(struct deal &dlBase);
   void SolveInChunks();
   void SolveOneChunk(uint i, uint step);
   void HandleSolvedChunk(struct boards& bo, struct solvedBoards& chunk);
   void SolveSecondTime(struct boards& bo, const struct solvedBoards& chunk);
   void NoticeMagicFly(uint trickSuit, uint tricksNT, const deal& cards);
   void ComparePrimaSecunda(uint tricksA, uint tricksB, const deal& cards);
   void HandleDDSFail(int res);

   // multi-thread & joined effort
   void  LaunchHelpers();
   void  ShowEffortSplit(Walrus &hA, Walrus &hB);
   ucell DoTheShare();
   void  DoIteration();
   void  Supervise(void);
   void  CoWork(Walrus * other);
   void  MergeResults(Walrus *other);
   void  ClearHelpers();
   const char *GetName() const { return mul.nameHlp; }
   uint  NumFiltered() const   { return mul.countToSolve; }
   WaMulti mul;

   // semantics
   bool InitSemantics();
   void FillSemantic(void);
   void AddForSolving(twContext* lay);
   void GrabSplinterVariant(twContext* lay);
   void GrabPrecisionVariant(twContext* lay);
   Semantics &sem;
   // -- no-operations in semantics
   void NOP() {}
   void VoidSingleScoring(DdsTricks &tr) {}
   void VoidFirstMarks(DdsTricks& tr, const deal& cards) {}
   void VoidSecondMarks(uint camp, const deal& cards) {}
   void VoidSecondSolve(boards& bo, const solvedBoards& solved) {}
   void VoidCompare(uint trickSuit, uint tricksNT, const deal& cards) {}

   // scoring & progress
   CumulativeScore cumulScore;
   void ScoreWithPrimary(DdsTricks &tr);
   void ScoreWithSecondary(DdsTricks &tr);
   void AddScorerValues(char* tail);
   Progress progress;
   void AddMarksByHCP(DdsTricks& tr, const deal& cards);
   void AddMarksByControls(DdsTricks& tr, const deal& cards);
   void AddMarksByOpLeads(DdsTricks& tr, const deal& cards);
   void AddMarksBySuit(DdsTricks& tr, const deal& cards);
   void AddMarksInHugeMatch(const deal& cards, uint camp, s64 impsPrima);

   // UI progress and reports
   MiniUI ui;
   void SummarizeFiltering();
   void ReportTimeMem();
   void ReportAllLines();
   bool ConsiderDashOutLine(ucell sumline);
   void DisplaySolvingStats(int i, ucell sumline);
   void DisplayFilterNumbers(uint ip);
   void DisplayStatCell(ucell cell);
   void RepeatLineWithPercentages(int i, ucell sumline);
   void HandleFilterLine(uint ip);
   void MiniReport(ucell toGo);
   void ShowProgress(ucell idx);
   bool RegularBalanceCheck();
   void UpdateFarColumnUI();
   void ShowPercentages(s64 sumRows);
   void ShowBiddingLevel(s64 sumRows);
   void ShowTheirScore(s64 doneTheirs);
   void ShowOptionalReports(s64 doneOurs, s64 doneTheirs);
   void AddOverallStats(int idx);
   void ShowAdvancedStatistics(int idx);
   void AddSetContracts(int idx);
   void AddMadeContracts(int idx);
   void ShowSummarizedExtraMarks();
   void ShowDetailedReportHighcards();
   void ShowDetailedReportCompWithHcp();
   void ShowDetailedReportControls();
   void ShowDetailedReportOpLeads();
   void ShowDetailedReportSuit();

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
   bool Orb_ApproveByFly(const deal& cards);
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

