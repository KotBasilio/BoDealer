/************************************************************
 * Walrus project                                        2020
 * AI parts
 *
 ************************************************************/
#include "waDoubleDeal.h"

#ifdef _DEBUG
   const uint WALRUS_CHUNK_SIZE = 100;
#else
   const uint WALRUS_CHUNK_SIZE = MAXNOOFBOARDS;
#endif

void Walrus::SolveSavedTasks()
{
   // say
   SummarizeFiltering();

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
   for (; chunkStart+step < NumFiltered() ; chunkStart+=step ) {
      // main work
      SolveOneChunk(chunkStart, step);
      progress.StoreCountToGo(NumFiltered() - chunkStart - step);

      // show some progress or just a dot
      ShowProgress(chunkStart);

      // fast exit
      if (ui.exitRequested) {
         return;
      }
   }

   // do a tail work
   if ( chunkStart < NumFiltered() ) {
      step = NumFiltered() - chunkStart;
      SolveOneChunk(chunkStart, step);
   }
   progress.StoreCountToGo(0);
}

void Walrus::SolveOneChunk(uint chunkStartIdx, uint boardsCount)
{
   // a chunk of tasks lies in arrToSolve. Indices are: from chunkStartIdx, boardsCount tasks

   // reconstruct all deals to fill the chunk
   static boards _chunkBoards;
   _chunkBoards.noOfBoards = (int)boardsCount;
   for (int i = 0; i < _chunkBoards.noOfBoards; i++) {
      DdsDeal dl(*sem.dlBase, mul.arrToSolve[chunkStartIdx + i]);
      _chunkBoards.deals[i] = dl.dl;
      _chunkBoards.target[i] = -1;
      _chunkBoards.solutions[i] = config.solve.ddsSol;
      _chunkBoards.mode[i] = 0;
   }

   // solve in parallel
   static solvedBoards _solved;
   int res = SolveAllBoardsN(_chunkBoards, _solved);
   HandleDDSFail(res);

   // read user commands
   ui.Run();

   // relay the chunk
   HandleSolvedChunk(_chunkBoards, _solved);

   // sometimes we solve the same chunk in a different suit and/or declared
   (this->*sem.solveSecondTime)(_chunkBoards, _solved);
}

void Walrus::ScoreWithPrimary(DdsTricks& tr)
{
   // respect old tasks (backward compatibility)
   #ifdef USE_ONLY_SINGLE_SCORING
      (cumulScore.*sem.onSinglePrimary)(tr.plainScore);
   #else
      (cumulScore.*sem.onPrimaryScoring)(tr);
   #endif
}

void Walrus::ScoreWithSecondary(DdsTricks& tr)
{
   // respect old tasks (backward compatibility)
   #ifdef USE_ONLY_SINGLE_SCORING 
      (cumulScore.*sem.onSingleSecondary)(tr.plainScore);
   #else
      (cumulScore.*sem.onSecondScoring)(tr);
   #endif
}

void Walrus::HandleSolvedChunk(boards& arrSrc, solvedBoards& solved)
{
   // pass to statistics and/or UI
   for (int handno = 0; handno < solved.noOfBoards; handno++) {
      futureTricks& fut = solved.solvedBoard[handno];
      DdsTricks tr; tr.Init(fut);
      deal& cards(arrSrc.deals[handno]);

      // pass to statistics. any extra marks are on postmortem
      progress.HitByTricks(tr.plainScore, config.prim.goal, IO_ROW_OUR_BASE, false);
      ScoreWithPrimary(tr);
      (this->*sem.onMarkAfterSolve)(tr, cards);

      // ui
      Orb_Interrogate(tr, cards, fut);
   }
}

void Walrus::SolveSecondTime(boards& arrSrc, const solvedBoards& chunk)
{
   // show a life sign and allow early quit
   printf(".");
   ui.Run();
   if (ui.exitRequested) {
      return;
   }

   // overwrite trumps and lead
   for (int i = 0; i < arrSrc.noOfBoards; i++) {
      arrSrc.deals[i].trump = config.secondary.trump;
      arrSrc.deals[i].first = config.secondary.first;
   }

   // solve second time
   static solvedBoards _twiceSolved;
   int res = SolveAllBoardsN(arrSrc, _twiceSolved);
   HandleDDSFail(res);

   // score the other contract and maybe compare them
   DdsTricks trFirst, trSecond;
   for (int handno = 0; handno < _twiceSolved.noOfBoards; handno++) {
      // pass to basic statistics
      trSecond.Init(_twiceSolved.solvedBoard[handno]);
      progress.HitByTricks(trSecond.plainScore, config.secondary.goal, IO_ROW_THEIRS);
      ScoreWithSecondary(trSecond);

      // pass to comparison
      deal& cards(arrSrc.deals[handno]);
      trFirst.Init(chunk.solvedBoard[handno]);
      (this->*sem.onCompareContracts)(trFirst.plainScore, trSecond.plainScore, cards);

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

   for (uint i = 0; i < NumFiltered(); i++) {
      // refill & solve the next deal
      DdsDeal dl(dlBase, mul.arrToSolve[i]);
      dl.Solve(i);

      // pass
      (cumulScore.*sem.onSinglePrimary)(dl.tr.plainScore);

      // may report
      if (!(i & 0xf)) {
         printf(".");
         if (i && !(i & freqMiniReport)) {
            MiniReport(NumFiltered() - i);
         }
      }
   }
}

// unused chunk to cater for unplayable boards -- we change board result on some percentage boards
#ifdef UNPLAYABLE_ONE_OF
bool isDecimated = false;
if (tr.plainScore == config.prim.goal) {
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

