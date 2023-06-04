/************************************************************
 * Walrus project                                        2019
 * Multi thread parts
 *
 ************************************************************/
#include "waCrossPlatform.h"
#include <cstring>
#include HEADER_SLEEP  
#include HEADER_THREADS
#include "walrus.h"

extern u64 ChronoRound();

Walrus::Multi::Multi()
   : isRunning(true)
   , nameHlp("main")
   , countIterations(0)
   , countShare(MAX_ITERATION)
   , countSolo(0)
   , maxTasksToSolve(MAX_TASKS_TO_SOLVE)
   , arrToSolve(nullptr)
   , countToSolve(0)
{
}

Walrus::Walrus(Walrus *other, const char *nameH, int ourShare)
{
   // duplicate fully
   memcpy(this, other, sizeof(*this));

   // but take another random seed
   StepAsideRand(100 * 42);

   // get name and appointment
   mul.nameHlp = nameH;
   mul.countShare = ourShare;

   // helpers need 1/3 of max tasks
   const size_t oneK = 1024;
   mul.maxTasksToSolve = MAX_TASKS_TO_SOLVE;
   size_t bsize = mul.maxTasksToSolve * sizeof(DdsTask);
   if (bsize > 250 * oneK) {
      mul.maxTasksToSolve >>= 3;
      mul.maxTasksToSolve *= 3; // that's 3/8 -- about 1/3
   }
}

PFM_THREAD_RETTYPE ProcHelper(void *arg)
{
   Walrus *helper = (Walrus *)(arg);
   printf("%s: %10u done", helper->GetName(), helper->DoTheShare());
   printf("---> %u\n", helper->NumFiltered());

   return PFM_THREAD_RETVAL;
}

void Walrus::LaunchHelpers(Walrus &hA, Walrus &hB)
{
#ifdef SKIP_HELPERS
   hA.mul.countShare = 0;
   hB.mul.countShare = 0;
   hA.mul.isRunning = false;
   hB.mul.isRunning = false;
#else
   PLATFORM_BEGIN_THREAD(ProcHelper, &hA);
   PLATFORM_BEGIN_THREAD(ProcHelper, &hB);
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
      TestSeed();
      hA.TestSeed();
      hB.TestSeed();
      printf("-------------------\n");
   #endif // _DEBUG
#endif // SHOW_EFFORT_SPLIT
}

void Walrus::MainScan(void)
{
   // decide how to split effort as the main thread is faster a bit
   uint effortA = (mul.countShare >> 2)
                + (mul.countShare >> 4)
                + (mul.countShare >> 6)
                + (mul.countShare >> 8);
   uint effortB = effortA;
   mul.countShare -= effortA + effortB;

   // split the effort
   Walrus hA(this, "helperA", effortA);
   Walrus hB(&hA , "helperB", effortB);
   ShowEffortSplit(hA, hB);

   // do the parallel work
   LaunchHelpers(hA, hB);
   DoTheShare();
   Supervise(&hA, &hB);

   // merge all
   mul.countSolo = mul.countIterations;
   MergeResults(&hA);
   MergeResults(&hB);

   // don't work all day! have a dinner break ;-)
   PLATFORM_SLEEP(20);
   printf("   main: %10u done\n", mul.countSolo);

   // perf
   progress.delta1 = ChronoRound();
}

void Walrus::DoIteration()
{
   // shuffle the deck, add a flip-over
   Shuffle();
   (this->*sem.fillFlipover)();

   // body of the scan
   (this->*sem.onScanCenter)();

   // no flip-over beyond this point
   ClearFlipover();
   VerifyCheckSum();

   // done
   mul.countIterations += sem.scanCover;
}


uint Walrus::DoTheShare()
{
   // any last-train init
   (this->*sem.onShareStart)();

   // iterate until stopped
   while (mul.countIterations < mul.countShare) {
      DoIteration();
   }

   // signal
   mul.isRunning = false;
   return mul.countIterations;
}

//------------------------------------------------
// replicate system rand(), strip the code out of thread handling
//
void Walrus::MergeResults(Walrus *other)
{
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

   // copy tasks
   if (mul.arrToSolve && other->mul.arrToSolve && other->mul.countToSolve) {
      if (mul.countToSolve + other->mul.countToSolve <= MAX_TASKS_TO_SOLVE) {
         uint size = other->mul.countToSolve * sizeof(DdsTask);
         memcpy(&mul.arrToSolve[mul.countToSolve], other->mul.arrToSolve, size);
         mul.countToSolve += other->mul.countToSolve;
      } else {
         printf("\nFailed to merge %d from %s\n", other->mul.countToSolve, other->GetName());
      }
   }
}

void Walrus::CoWork(Walrus *slowHelper)
{
   slowHelper->mul.countShare -= sem.scanCover;
   DoIteration();
}

void Walrus::Supervise(Walrus *helperA, Walrus *helperB)
{
   // while it makes any sense
   for (;;) {
      uint cA = helperA->Remains();
      uint cB = helperB->Remains();
      if (cA + cB < SUPERVISE_REASONABLE) {
         return;
      }

      // co-work with a helper that has more work ahead
      Walrus *needy = cA > cB ? helperA : helperB;
      for (int i = SUPERVISE_CHUNK; --i >= 0;) {
         CoWork(needy);
      }
   }
}


