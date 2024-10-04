/************************************************************
 * Walrus semantics                               18.12.2019
 *
 * NOTES on hitsCount[][] array dimensions and distribution:
 * -- rows are big factor (aka hcp, 0 - 40)
 * -- columns are smaller factor (aka controls, 0 - 12)
 * Its layout has two independed areas:
 * -- lines 0 to (IO_ROW_FILTERING - 1)               <-- for statistics on solved boards
 * -- lines IO_ROW_FILTERING to (HITS_LINES_SIZE - 1) <-- for statistics during boards filtering
 ************************************************************/
#pragma once

 // output rows: 
 // -- we down, we make, a blank line
constexpr uint IO_ROW_OUR_BASE   = 0;
constexpr uint IO_ROW_OUR_DOWN   = IO_ROW_OUR_BASE;
constexpr uint IO_ROW_OUR_MADE   = IO_ROW_OUR_BASE + 1;
// -- they down, they make
constexpr uint IO_ROW_CONTRACT_B = IO_ROW_OUR_MADE + 2;
constexpr uint IO_ROW_THEIRS     = IO_ROW_CONTRACT_B;
// -- comparison
constexpr uint IO_ROW_COMPARISON = IO_ROW_CONTRACT_B + 2;
constexpr uint IO_ROW_MAGIC_FLY  = IO_ROW_COMPARISON;
constexpr uint IO_ROW_SACRIFICE  = IO_ROW_COMPARISON;
// -- extra marks like on hcp, controls, lead etc.
constexpr uint IO_ROW_POSTMORTEM = 7;
constexpr uint IO_SHIFT_FOR_EXTRA_MARKS = 27;
// -- filtering
constexpr uint IO_ROW_FILTERING = (3 + IO_SHIFT_FOR_EXTRA_MARKS);
constexpr uint IO_ROW_SELECTED = IO_ROW_FILTERING;

// output columns:
// -- comparing prima and secunda
constexpr uint IO_CAMP_OFF = 0;
constexpr uint IO_CAMP_PREFER_PRIMA = 1;
constexpr uint IO_CAMP_NO_DIFF = 2;
constexpr uint IO_CAMP_PREFER_SECUNDA = 3;
// -- a match
constexpr uint IO_CAMP_IMPS_PRIMA = 5;
constexpr uint IO_CAMP_IMPS_SECUNDA = 6;
// -- derived from that comparison
constexpr uint IO_CAMP_PREFER_SUIT = IO_CAMP_PREFER_PRIMA;
constexpr uint IO_CAMP_SAME_NT     = IO_CAMP_NO_DIFF;
constexpr uint IO_CAMP_MORE_NT     = IO_CAMP_PREFER_SECUNDA;
constexpr uint IO_CAMP_PREFER_TO_BID   = IO_CAMP_PREFER_PRIMA;
constexpr uint IO_CAMP_REFRAIN_BIDDING = IO_CAMP_PREFER_SECUNDA;

// dimensions
constexpr uint IPR_COMPACTION = 16;// see CellByIPR()
constexpr uint HITS_LINES_SIZE = IO_ROW_FILTERING + IPR_COMPACTION;
constexpr uint HITS_COLUMNS_SIZE = 16;// gives fast padding

// hits count and others
struct Progress {
   friend class Walrus;
   Progress();
   ucell step, went, margin;
   u64  delta1, delta2;
   bool isDoneAll;
   void Init(ucell _step);
   bool Step();
   void Up(ucell idx);
   void StoreCountToGo(ucell count);

   // operating marks
   // -- on filtering
   void SelectedMark();
   void FilteredOutMark(uint ip, uint reason);
   void FOutExtraMark(uint ip, uint reason);
   void RemoveFOutExtraMark(uint ip, uint reason);
   // -- get filtering stats
   ucell GetDiscardedBoardsCount();
   ucell PeekByIPR(uint ip, uint reason) { return CellByIPR(ip, reason); }
   // -- on solving
   void HitByTricks(uint amount, uint made, uint row, bool isExtraMark = true);
   void SolvedExtraMark(uint row, uint col);
   void AddImps(uint row, uint col, ucell imps);
private:
   ucell hitsCount[HITS_LINES_SIZE][HITS_COLUMNS_SIZE];
   ucell countExtraMarks;
   ucell& CellByIPR(uint ip, uint reason);
};
extern Progress *mainProgress;

#include "WaSemMicro.h"

struct MiniUI;

// A class to rule task logic. Its values are constant through entire solving.
// So it should be filled on init. Preferrably -- fully configurable. 
typedef void (Shuffler::*        SemShufflerFunc)();
typedef void (Walrus::*          SemFuncType)();
typedef void (CumulativeScore::* SemGenScoring)(DdsTricks &tr);
typedef void (CumulativeScore::* SemSingleScoring)(uint tricks);
typedef void (Walrus::*          SemComparing)(uint trickA, uint tricksB, const deal& cards);
typedef void (Walrus::*          SemFirstMarks)(DdsTricks& tr, const deal& cards);
typedef void (Walrus::*          SemSecondMarks)(uint camp, const deal& cards);
typedef void (Walrus::*          SemOnBoardFound)(twContext* lay);
typedef void (MiniUI::*          SemOnBoardAdded)(twContext* lay);
typedef void (Walrus::*          SemSecondSolver)(struct boards& bo, const struct solvedBoards& solved);
struct Semantics {
   SemFuncType              onInit;
   SemFuncType              onShareStart;
   SemFuncType              onScanCenter;
   SemShufflerFunc          fillFlipover;
   std::vector<MicroFilter> vecFilters;
   SemOnBoardFound          onBoardFound;
   SemOnBoardAdded          onBoardAdded;
   SemFuncType              onAfterMath;
   SemSecondSolver          solveSecondTime;
   SemComparing             onCompareContracts;
   SemFirstMarks            onMarkAfterSolve;
   SemSecondMarks           onSecondMarks;
   // scorers:
   // -- primary
   SemGenScoring            onPrimaryScoring;
   SemSingleScoring         onSinglePrimary;
   // -- secondary
   SemGenScoring            onSecondScoring;
   SemSingleScoring         onSingleSecondary;

   uint scanCover; // how much iterations covers one scan
   struct deal* dlBase;
   Semantics();
   void SetOurPrimaryScorer(CumulativeScore &cs, const char* code);
   void SetOurSecondaryScorer(CumulativeScore &cs, const char* code);
   void SetTheirScorer(CumulativeScore &cs, const char* code);
   void SetBiddingGameScorer(CumulativeScore &cs, const char* code);
   void SetOpeningLeadScorer(CumulativeScore &cs, const char* code);
   void PrepareBaseDeal();
   bool IsInitOK() { return isInitSuccess; }
   bool IsClosingBracket(int idx);
   bool IsOpeningBracket(int idx);

   bool Compile(const char* sourceCode, size_t sizeSourceCode, std::vector<MicroFilter>& out);
   bool MiniLink(std::vector<MicroFilter> &filters);
   void MiniLinkFilters();
private:
   void SetSecondaryScorer(CumulativeScore &cs, s64 &target, const char* code);
   bool CompileOneLine(struct CompilerContext &ctx);
   bool IsListStart(const MicroFilter& mic);
   bool isInitSuccess = true;
};

extern Semantics semShared;
