/************************************************************
 * Walrus project                                        2020
 * AI parts
 *
 ************************************************************/
#include "waDoubleDeal.h"

static boards _chunkBoards;
static solvedBoards _solved;
static solvedBoards _twiceSolved;

void Walrus::InitMiniUI()
{
   static deal dlBase;
   PrepareBaseDeal(dlBase);
   ui.Init(dlBase.trump, dlBase.first);
   sem.dlBase = &dlBase;
}

void Walrus::SolveSavedTasks()
{
   // say
   AnnounceSolving();

   // finalize preparations
   SetMaxThreads(0);
   progress.StoreCountToGo(0);

   // solve in some manner
   #ifdef SOLVE_ONE_BY_ONE
      SolveOneByOne(*sem.dlBase);
   #else
      SolveInChunks();
   #endif
}

void Walrus::SolveInChunks()
{
   // do big chunks
   uint step = WALRUS_CHUNK_SIZE;
   progress.Init(step);
   uint chunkStart = 0;
   for (; chunkStart+step < mul.countToSolve ; chunkStart+=step ) {
      // main work
      SolveOneChunk(_chunkBoards, chunkStart, step);

      // show some progress or just a dot
      progress.StoreCountToGo(mul.countToSolve - chunkStart - step);
      ShowProgress(chunkStart);

      // fast exit
      if (ui.exitRequested) {
         return;
      }
   }

   // do a tail work
   if ( chunkStart < mul.countToSolve ) {
      step = mul.countToSolve - chunkStart;
      SolveOneChunk(_chunkBoards, chunkStart, step);
   }
   progress.StoreCountToGo(0);
}

void Walrus::SolveOneChunk(boards& bo, uint ofs, uint step)
{
   bo.noOfBoards = (int)step;

   // refill & copy all deals
   for (int i = 0; i < bo.noOfBoards; i++) {
      DdsDeal dl(*sem.dlBase, mul.arrToSolve[ofs + i]);
      bo.deals[i] = dl.dl;
      bo.target[i] = -1;
      bo.solutions[i] = PARAM_SOLUTIONS_DDS;
      bo.mode[i] = 0;
   }

   // solve in parallel
   int res = SolveAllBoardsN(bo, _solved);
   HandleDDSFail(res);

   // read user commands
   ui.Run();

   // relay the chunk
   HandleSolvedChunk(bo, _solved);

   // sometimes we solve the same chunk in a different suit and/or declared
   (this->*sem.solveSecondTime)(bo, _solved);
}

void Walrus::HandleSolvedChunk(boards& bo, solvedBoards& solved)
{
   // pass to statistics and/or UI
   for (int handno = 0; handno < solved.noOfBoards; handno++) {
      futureTricks& fut = solved.solvedBoard[handno];
      DdsTricks tr; tr.Init(fut);
      deal& cards(bo.deals[handno]);

      // pass to basic statistics
      HitByTricks(tr, config.primGoal);
      (cumulScore.*sem.onScoring)(tr.plainScore);

      // some detailed postmortem is possible
      (this->*sem.onPostmortem)(tr, cards);

      // ui
      Orb_Interrogate(tr, cards, fut);
   }
}

void Walrus::SolveSecondTime(boards& bo, solvedBoards& chunk)
{
   // show a life sign and allow early quit
   printf(".");
   ui.Run();
   if (ui.exitRequested) {
      return;
   }

   // overwrite trumps and lead
   for (int i = 0; i < bo.noOfBoards; i++) {
      bo.deals[i].trump = OC_TRUMPS;
      bo.deals[i].first = OC_ON_LEAD;
   }

   // solve second time
   int res = SolveAllBoardsN(bo, _twiceSolved);
   HandleDDSFail(res);

   // score the other contract and maybe compare them
   DdsTricks trFirst, trSecond;
   for (int handno = 0; handno < _twiceSolved.noOfBoards; handno++) {
      // pass to basic statistics
      trSecond.Init(_twiceSolved.solvedBoard[handno]);
      HitByTricks(trSecond, config.secGoal, IO_ROW_THEIRS);
      (cumulScore.*sem.onSolvedTwice)(trSecond.plainScore);
      progress.countExtraMarks++;

      // pass to comparison
      trFirst.Init(chunk.solvedBoard[handno]);
      (this->*sem.onCompareContracts)(trFirst.plainScore, trSecond.plainScore);

      // may monitor TNT -- TODO
      //deal& cards(bo.deals[handno]);
      //(this->*sem.onMonitorTNT)(cards, trFirst.plainScore + trSecond.plainScore);

      // may debug each board
      // cards.trump = config.soTrump;
      // cards.first = config.soFirst;
      // Orb_ReSolveAndShow(cards);
      // ui.WaitAnyKey();
   }
}

// a mode only for sessions to debug solver integration. 
// the mode doesn't support a lot of new features.
void Walrus::SolveOneByOne(deal& dlBase)
{
   uint freqMiniReport = 0x3f; // 0xff to make rare

   for (uint i = 0; i < mul.countToSolve; i++) {
      // refill & solve the next deal
      DdsDeal dl(dlBase, mul.arrToSolve[i]);
      dl.Solve(i);

      // pass
      (cumulScore.*sem.onScoring)(dl.tr.plainScore);

      // may report
      if (!(i & 0xf)) {
         printf(".");
         if (i && !(i & freqMiniReport)) {
            MiniReport(mul.countToSolve - i);
         }
      }
   }
}

// unused chunk to cater for unplayable boards -- we change board result on some percentage boards
#ifdef UNPLAYABLE_ONE_OF
bool isDecimated = false;
if (tr.plainScore == config.primGoal) {
   static int cycleCatering = UNPLAYABLE_ONE_OF;
   if (0 == --cycleCatering) {
      tr.plainScore--;
      isDecimated = true;
      cycleCatering = UNPLAYABLE_ONE_OF;
   }
}

// results are a fake => forget the board asap
if (isDecimated) {
   continue;
}

// example
#define UNPLAYABLE_ONE_OF  6
#endif

