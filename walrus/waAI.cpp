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

   // fork performance 
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

   // relay
   HandleSolvedChunk(bo, solved);

   // sometimes we solve the same chunk in a different suit and/or declared
   (this->*sem.solveSecondTime)(bo, solved);
}

void Walrus::HandleSolvedChunk(boards& bo, solvedBoards& solved)
{
   // read user commands
   ui.Run();

   // pass to statistics and/or UI
   for (int handno = 0; handno < solved.noOfBoards; handno++) {
      futureTricks& fut = solved.solvedBoard[handno];
      DdsTricks tr; tr.Init(fut);
      deal& cards(bo.deals[handno]);
      if (HandleSolvedBoard(tr, cards)) {
         Orb_Interrogate(tr, cards, fut);
      }
   }
}

// RET: true -- normal solved board
// false -- was decimated, not for UI
bool Walrus::HandleSolvedBoard(DdsTricks &tr, deal &cards)
{
   // cater for unplayable boards -- we change board result on some percentage boards
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
   #endif // UNPLAYABLE_ONE_OF

   // pass to basic statistics
   HitByScore(tr, ui.irBase);
   (this->*sem.onScoring)(tr);

   // results are a fake => forget the board asap
   #ifdef UNPLAYABLE_ONE_OF
      if (isDecimated) {
         return false;
      }
   #endif

   // some detailed postmortem is possible
   (this->*sem.onPostmortem)(tr, cards);

   return true;
}

void Walrus::SolveSecondTime(boards& bo, solvedBoards& chunk)
{
   // overwrite trumps and lead
   for (int i = 0; i < bo.noOfBoards; i++) {
      bo.deals[i].trump = TWICE_TRUMPS;
      bo.deals[i].first = TWICE_ON_LEAD_CHUNK;
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
   }
}

void Walrus::SolveOneByOne(deal& dlBase)
{
   #ifdef _DEBUG
      uint freqMiniReport = 0x3f;
   #else
      uint freqMiniReport = 0xff;
   #endif // _DEBUG

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
   } // boards

}

