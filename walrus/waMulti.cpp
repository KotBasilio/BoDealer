/************************************************************
 * Walrus project                                        2019
 * Multi thread parts
 *
 ************************************************************/
#include "waCrossPlatform.h"
#include HEADER_SLEEP  
#include HEADER_THREADS
#include "walrus.h"
#include HEADER_CURSES

//#define SHOW_EFFORT_SPLIT

extern u64 ChronoRound();

void PaintWalrus();

WaMulti::WaMulti()
   : isRunning(true)
   , shouldSignOut(false)
   , shownFirstLiveSign(false)
   , nameHlp("main")
   , countIterations(0)
   , countShare(MAX_ITERATION)
   , countSolo(0)
   , maxTasksToSolve(MAX_TASKS_TO_SOLVE)
   , arrToSolve(nullptr)
   , countToSolve(0)
   , countShowLiveSign(0)
   , hA(nullptr)
   , hB(nullptr)
{
}

// Constructing a helper
Walrus::Walrus(Walrus *other, const char *nameH, ucell ourShare) : sem(semShared)
{
   // duplicate fully
   memcpy(this, other, sizeof(*this));

   // rebind
   filter.Bind(this);

   // but take another random seed
   shuf.StepAsideRand(100 * 42);

   // setup multi for reduced amount of tasks
   mul.Setup(nameH, ourShare);
}

void Walrus::ScanAsHelper()
{
   printf("\n%s: %10llu done ", GetName(), DoTheShare());
   printf("---> %lu ", NumFiltered());
}

PFM_THREAD_RETTYPE ProcHelper(void *arg)
{
   Walrus *bro = (Walrus *)(arg);
   bro->ScanAsHelper();

   return PFM_THREAD_RETVAL;
}

void Walrus::LaunchHelpers()
{
   // may work without helpers
   #ifdef SKIP_HELPERS
      mul.hA = mul.hB = nullptr;
      return;
   #endif

   // decide how to split effort. Take in account that the main thread is faster a bit
   ucell effortA = (mul.countShare >> 2)
                 + (mul.countShare >> 4)
                 + (mul.countShare >> 6)
                 + (mul.countShare >> 8);
   ucell effortB = effortA;
   mul.countShare -= effortA + effortB;

   // split the effort
   mul.hA = new Walrus(this  , "helperA", effortA);
   mul.hB = new Walrus(mul.hA, "helperB", effortB);
   ShowEffortSplit(*mul.hA, *mul.hB);

   // threads
   PLATFORM_BEGIN_THREAD(ProcHelper, mul.hA);
   PLATFORM_BEGIN_THREAD(ProcHelper, mul.hB);
   mul.countShowLiveSign = ADDITION_STEP_ITERATIONS;
}

void Walrus::ClearHelpers()
{
   delete mul.hA;
   delete mul.hB;
   mul.hA = mul.hB = nullptr;
}

void Walrus::ShowEffortSplit(Walrus &hA, Walrus &hB)
{
#ifdef SHOW_EFFORT_SPLIT
   printf("Aiming : %10llu =\n"
      "   main: %10llu +\n"
      "%s: %10llu +\n%s: %10llu\n-------------------\n"
      , MAX_ITERATION, mul.countShare
      , hA.GetName(), hA.mul.countShare
      , hB.GetName(), hB.mul.countShare);

   // monitor random in debug
   #ifdef _DEBUG
      shuf.TestSeed(GetName());
      hA.shuf.TestSeed(hA.GetName());
      hB.shuf.TestSeed(hB.GetName());
      printf("-------------------\n");
   #endif // _DEBUG
#endif // SHOW_EFFORT_SPLIT
}

void Walrus::ScanFixedTask(void)
{
   // setup the parallel work
   LaunchHelpers();

   // start ours
   DoTheShare();
   Supervise();

   // merge all
   mul.countSolo = mul.countIterations;
   MergeResults(mul.hA);
   MergeResults(mul.hB);

   // don't work all day! have a dinner break ;-)
   RegularBalanceCheck();
   PLATFORM_SLEEP(20);
   printf("\n   main: %-10llu done\n", mul.countSolo);
   ClearHelpers();

   // perf
   progress.searchTime = ChronoRound();
}

void Walrus::ScanStray(void)
{
   // give main effort to helper, Corgy
   ucell effortA = (mul.countShare >> 1)
                 + (mul.countShare >> 2);
   mul.countShare -= effortA;
   mul.hA = new Walrus(this, "Corgy", effortA);

   // let Corgy go

   // find some amount of boards to start

   // pass to Corgy remained effort
}

void Walrus::EndStray(void)
{
   if (mul.hA) {
      delete mul.hA;
      mul.hA = nullptr;
   }
}

void Walrus::DoIteration()
{
   // shuffle the deck, add a flip-over
   shuf.Shuffle();
   (shuf.*sem.fillFlipover)();

   // body of the scan
   (this->*sem.onScanCenter)();

   // no flip-over beyond this point
   shuf.ClearFlipover();
   shuf.VerifyCheckSum();

   // done the chunk
   mul.countIterations += sem.scanCover;
}

ucell Walrus::DoTheShare()
{
   shuf.VerifyCheckSum();

   // any last-train init
   (mul.*sem.onShareStart)();

   // iterate until stopped
   do {
      // normal work
      while (mul.countIterations < mul.countShare) {
         if (mul.shouldSignOut) {
            mul.countShare = mul.countIterations;
         } else {
            DoIteration();
            //RegularBalanceCheck();
            mul.ShowLiveSigns(sem.scanCover);
         }
      }

      // watch for helper done his share natural
      if (mul.hA || mul.shouldSignOut) {
      } else { // may decide to work more
         if (NumFiltered() < (config.solve.aimTaskCount * 32) / 100) {
            while (mul.countIterations >= mul.countShare) {
               mul.countShare += ADDITION_STEP_ITERATIONS;
            }
         }
      }
   } while (mul.countIterations < mul.countShare);
   RegularBalanceCheck();

   // signal
   mul.isRunning = false;
   return mul.countIterations;
}

//------------------------------------------------
void Walrus::MergeResults(Walrus *other)
{
   if (!other) {
      return;
   }

   // mark
   //printf((other == mul.hA) ? "+A," : "+B,");

   // helper is busy => do some of its work
   while (other->mul.IsRunning()) {
      CoWork(other);
      mul.countSolo += sem.scanCover;
   }

   // sum up
   for (int i = 0; i < HITS_LINES_SIZE; i++) {
      for (int j = 0; j < HITS_COLUMNS_SIZE; j++) {
         progress.hitsCount[i][j] += other->progress.hitsCount[i][j];
      }
   }
   mul.countIterations += other->mul.countIterations;
   progress.countExtraMarks += other->progress.countExtraMarks;

   // copy tasks
   if (mul.arrToSolve && other->mul.arrToSolve && other->NumFiltered()) {
      if (NumFiltered() + other->NumFiltered() <= MAX_TASKS_TO_SOLVE) {
         size_t size = other->NumFiltered() * sizeof(WaTask);
         memcpy(&mul.arrToSolve[NumFiltered()], other->mul.arrToSolve, size);
         mul.countToSolve += other->NumFiltered();
      } else {
         printf("\nFailed to merge %d from %s\n", other->NumFiltered(), other->GetName());
      }
   }
}

void Walrus::CoWork(Walrus *slowHelper)
{
   // byte a bit, but not the last chunk
   if (slowHelper->mul.countShare > sem.scanCover) {
      slowHelper->mul.countShare -= sem.scanCover;
   }

   // work it here
   DoIteration();
}

void Walrus::Supervise()
{
   Walrus* helperA = mul.hA;
   Walrus* helperB = mul.hB;
   if (!helperA) {
      return;
   }

   // while it makes any sense
   for (int count_cowork = 0; ; count_cowork++) {
      ucell cA = helperA->mul.Remains();
      ucell cB = helperB->mul.Remains();
      if (cA + cB < COWORK_REASONABLE) {
         return;
      }

      // co-work with a helper that has more work ahead
      Walrus *needy = cA > cB ? helperA : helperB;
      for (int i = COWORK_CHUNK; --i >= 0;) {
         CoWork(needy);
      }

      if (count_cowork > 100) {
         mul.StopHelpersSuddenly();
         return;
      }
   }
}

void WaMulti::ShowLiveSigns(uint oneCover)
{
   const uint LIVE_SIGN = ADDITION_STEP_ITERATIONS * 64 / 70;

   if (!hA) {
      #ifdef SKIP_HELPERS
         if ( NumFiltered() > config.solve.aimTaskCount) {
            printf("found.");
            countShare = countIterations;
         }
      #endif 
      return;
   }

   if (!isRunning) {
      return;
   }

   // got enough => sign out to stop
   uint acc = NumFiltered() + hA->NumFiltered() + hB->NumFiltered();
   if (acc > config.solve.aimTaskCount) {
      printf("found.");
      countShare = countIterations;
      StopHelpersSuddenly();
      return;
   }

   // wait / reset
   if (countShowLiveSign > oneCover) {
      countShowLiveSign -= oneCover;
      return;
   }
   countShowLiveSign = LIVE_SIGN;

   // first sign is a title
   if (!shownFirstLiveSign) {
      printf("A progress in finding %dK boards: ", config.solve.aimTaskCount / 1000);
      shownFirstLiveSign = true;
   }

   // show accumulation progress
   printf("%d", acc / 1000);

   // consider extension of the search unless we're 98% close
   if (countIterations + ADDITION_STEP_ITERATIONS > countShare &&
      acc < (config.solve.aimTaskCount * 98) / 100) {
      countShare += ADDITION_STEP_ITERATIONS;
      hA->mul.countShare += ADDITION_STEP_ITERATIONS;
      hB->mul.countShare += ADDITION_STEP_ITERATIONS;
      printf(",");
   } else {
      printf(".");
   }

   // allow interruption
   if (PLATFORM_KBHIT()) {
      auto inchar = PLATFORM_GETCH();
      StopHelpersSuddenly();
      countShare = countIterations;
   }
}

void WaMulti::StopHelpersSuddenly()
{
   printf("X");
   if (hA->mul.isRunning) {
      hA->mul.shouldSignOut = true;
   }
   if (hB->mul.isRunning) {
      hB->mul.shouldSignOut = true;
   }
}

const char* WaMulti::SizeToReadable(const size_t bsize)
{
   static char displayBuf[10];
   const size_t oneK = 1024;
   const size_t oneM = 1024 * oneK;
   if (bsize > oneM) {
      sprintf_s(displayBuf, sizeof(displayBuf), "%lluM", bsize / oneM);
   } else {
      sprintf_s(displayBuf, sizeof(displayBuf), "%lluK", bsize / oneK);
   }
   return displayBuf;
}

