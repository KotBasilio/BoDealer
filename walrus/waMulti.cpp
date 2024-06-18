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

Walrus::Walrus(Walrus *other, const char *nameH, ucell ourShare) : sem(semShared)
{
   // duplicate fully
   memcpy(this, other, sizeof(*this));

   // rebind
   filter.Bind(this);

   // but take another random seed
   shuf.StepAsideRand(100 * 42);

   // get name and appointment
   mul.nameHlp = nameH;
   mul.countShare = ourShare;

   // helpers need 1/3 of max tasks
   const size_t oneK = 1024;
   mul.maxTasksToSolve = MAX_TASKS_TO_SOLVE;
   size_t bsize = mul.maxTasksToSolve * sizeof(WaTask);
   if (bsize > 250 * oneK) {
      mul.maxTasksToSolve >>= 3;
      mul.maxTasksToSolve *= 3; // that's 3/8 -- about 1/3
   }
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

void Walrus::LaunchHelpers(Walrus &hA, Walrus &hB)
{
#ifdef SKIP_HELPERS
   hA.mul.isRunning = false;
   hB.mul.isRunning = false;
#else
   mul.hA = &hA;
   mul.hB = &hB;
   PLATFORM_BEGIN_THREAD(ProcHelper, mul.hA);
   PLATFORM_BEGIN_THREAD(ProcHelper, mul.hB);
   mul.countShowLiveSign = ADDITION_STEP_ITERATIONS;
#endif // SKIP_HELPERS
}

void Walrus::ShowEffortSplit(Walrus &hA, Walrus &hB)
{
#ifdef SHOW_EFFORT_SPLIT
   printf("Aiming : %10u =\n"
      "   main: %10u +\n"
      "%s: %10u +\n%s: %10u\n-------------------\n"
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
   // decide how to split effort as the main thread is faster a bit
   ucell effortA = (mul.countShare >> 2)
                + (mul.countShare >> 4)
                + (mul.countShare >> 6)
                + (mul.countShare >> 8);
   ucell effortB = effortA;
   #ifdef SKIP_HELPERS
      effortB = effortA = 0;
   #endif
   mul.countShare -= effortA + effortB;

   // split the effort
   Walrus hA(this, "helperA", effortA);
   Walrus hB(&hA , "helperB", effortB);
   ShowEffortSplit(hA, hB);

   // setup the parallel work
   LaunchHelpers(hA, hB);

   // start ours
   DoTheShare();
   Supervise();

   // merge all
   mul.countSolo = mul.countIterations;
   MergeResults(mul.hA);
   MergeResults(mul.hB);

   // don't work all day! have a dinner break ;-)
   PLATFORM_SLEEP(20);
   printf("\n   main: %-10llu done\n", mul.countSolo);

   // perf
   progress.delta1 = ChronoRound();
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
   (this->*sem.onShareStart)();

   // iterate until stopped
   do {
      // normal work
      while (mul.countIterations < mul.countShare) {
         if (mul.shouldSignOut) {
            mul.countShare = mul.countIterations;
         } else {
            DoIteration();
            mul.ShowLiveSigns(sem.scanCover);
         }
      }

      // watch for helper done his share natural
      if (mul.hA || mul.shouldSignOut) {
      } else { // may decide to work more
         if (NumFiltered() < (AIM_TASKS_COUNT * 32) / 100) {
            while (mul.countIterations >= mul.countShare) {
               mul.countShare += ADDITION_STEP_ITERATIONS;
            }
         }
      }
   } while (mul.countIterations < mul.countShare);

   // signal
   mul.isRunning = false;
   return mul.countIterations;
}

//------------------------------------------------
// replicate system rand(), strip the code out of thread handling
//
void Walrus::MergeResults(Walrus *other)
{
   if (!other) {
      return;
   }

   // mark
   //printf((other == mul.hA) ? "+A," : "+B,");

   // helper is busy => do some of its work
   while (other->IsRunning()) {
      CoWork(other);
      mul.countSolo += sem.scanCover;
   }

   // sum up
   for (int i = 0; i < HCP_SIZE; i++) {
      for (int j = 0; j < CTRL_SIZE; j++) {
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
      ucell cA = helperA->Remains();
      ucell cB = helperB->Remains();
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
         if ( Gathered() > AIM_TASKS_COUNT) {
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
   if (acc > AIM_TASKS_COUNT) {
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
      printf("A progress in finding %dK boards: ", AIM_TASKS_COUNT / 1000);
      shownFirstLiveSign = true;
   }

   // show accumulation progress
   printf("%d", acc / 1000);

   // consider extension of the search unless we're 98% close
   if (countIterations + ADDITION_STEP_ITERATIONS > countShare &&
      acc < (AIM_TASKS_COUNT * 98) / 100) {
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


