/************************************************************
 * Walrus project                                        2020
 * AI parts
 *
 ************************************************************/
#include "waDoubleDeal.h"

void Walrus::SolveSavedTasks()
{
   // say
   AnnounceSolving();

   // finalize preparations
   deal dlBase;
   PrepareBaseDeal(dlBase);
   InitMiniUI(dlBase.trump, dlBase.first);
   SetMaxThreads(0);
   progress.StoreCountToGo(0);

   // solve in some manner
   #ifdef SOLVE_ONE_BY_ONE
      SolveOneByOne(dlBase);
   #else
      SolveInChunks(dlBase);
   #endif
}

void Walrus::SolveInChunks(deal &dlBase)
{
   // do big chunks
   boards bo;
   uint step = WALRUS_CHUNK_SIZE;
   progress.Init(step);
   uint chunkStart = 0;
   for (; chunkStart+step < mul.countToSolve ; chunkStart+=step ) {
      // main work
      SolveOneChunk(dlBase, bo, chunkStart, step);

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
      SolveOneChunk(dlBase, bo, chunkStart, step);
   }
   progress.StoreCountToGo(0);
}

void Walrus::SolveOneChunk(deal& dlBase, boards& bo, uint ofs, uint step)
{
   bo.noOfBoards = (int)step;

   // refill & copy all deals
   for (int i = 0; i < bo.noOfBoards; i++) {
      DdsDeal dl(dlBase, mul.arrToSolve[ofs + i]);
      bo.deals[i] = dl.dl;
      bo.target[i] = -1;
      bo.solutions[i] = PARAM_SOLUTIONS_DDS;
      bo.mode[i] = 0;
   }

   // solve in parallel
   solvedBoards solved;
   int res = SolveAllBoardsN(bo, solved);
   HandleDDSFail(res);

   // read user commands
   ui.Run();

   // relay the chunk
   HandleSolvedChunk(bo, solved);

   // sometimes we solve the same chunk in a different suit and/or declared
   (this->*sem.solveSecondTime)(bo, solved);
}

void Walrus::HandleSolvedChunk(boards& bo, solvedBoards& solved)
{
   // pass to statistics and/or UI
   for (int handno = 0; handno < solved.noOfBoards; handno++) {
      futureTricks& fut = solved.solvedBoard[handno];
      DdsTricks tr; tr.Init(fut);
      deal& cards(bo.deals[handno]);

      // pass to basic statistics
      HitByScore(tr, ui.irBase);
      (this->*sem.onScoring)(tr);

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
   solvedBoards twice;
   int res = SolveAllBoardsN(bo, twice);
   HandleDDSFail(res);

   // score the other contract and maybe compare them
   for (int handno = 0; handno < twice.noOfBoards; handno++) {
      DdsTricks trSecond;
      trSecond.Init(twice.solvedBoard[handno]);

      // pass to basic statistics
      HitByScore(trSecond, ui.otherGoal, IO_ROW_THEIRS);
      (this->*sem.onSolvedTwice)(trSecond);
      progress.countExtraMarks++;

      // pass to comparison
      DdsTricks trFirst;
      trFirst.Init(chunk.solvedBoard[handno]);
      (this->*sem.onCompareContracts)(trFirst.plainScore, trSecond.plainScore);

      // may monitor TNT -- TODO
      deal& cards(bo.deals[handno]);
      //(this->*sem.onMonitorTNT)(cards, trFirst.plainScore + trSecond.plainScore);

      // may debug each board
      // cards.trump = ui.soTrump;
      // cards.first = ui.soFirst;
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
      (this->*sem.onScoring)(dl.tr);

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
if (tr.plainScore == ui.irBase) {
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

