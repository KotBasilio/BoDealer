/************************************************************
 * Walrus project                                        2019
 * Multi thread parts
 *
 ************************************************************/
//#include <Windows.h> // Sleep
//#include <process.h> // _beginthread
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include "walrus.h"

#define SUCCESS               0

Walrus::Walrus(Walrus *other, const char *nameH, int ourShare)
{
   // duplicate fully
   memcpy(this, other, sizeof(*this));

   // but take another random seed
   StepAsideRand(100 * 42);

   // get name and appointment
   nameHlp = nameH;
   countShare = ourShare;

   // helpers need 1/3 of max tasks
   const size_t oneK = 1024;
   maxTasksToSolve = MAX_TASKS_TO_SOLVE;
   size_t bsize = maxTasksToSolve * sizeof(DdsTask);
   if (bsize > 250 * oneK) {
      maxTasksToSolve >>= 3;
      maxTasksToSolve *= 3; // that's 3/8 -- about 1/3
   }
}

void* ProcHelper(void *arg)
{
   Walrus *helper = (Walrus *)(arg);
   printf("%s: %10u done", helper->GetName(), helper->DoTheShare());
   printf("---> %u\n", helper->NumFiltered());

   return SUCCESS;
}

void Walrus::LaunchHelpers(Walrus &hA, Walrus &hB)
{
#ifdef SKIP_HELPERS
   hA.countShare = 0;
   hB.countShare = 0;
   hA.isRunning = false;
   hB.isRunning = false;
#else
    pthread_t thread1;
    pthread_t thread2;
    pthread_create(&thread1, NULL, ProcHelper, (void*)&hA );
    pthread_create(&thread2, NULL, ProcHelper, (void*)&hB );
   //_beginthread(ProcHelper, 0, &hA);
   //_beginthread(ProcHelper, 0, &hB);
#endif // SKIP_HELPERS
}


void Walrus::MainScan(void)
{
   // decide how to split effort as the main thread is faster a bit
   uint effortA = (countShare >> 2)
                + (countShare >> 4)
                + (countShare >> 6)
                + (countShare >> 8);
   uint effortB = effortA;
   countShare -= effortA + effortB;

   // split the effort
   Walrus hA(this, "helperA", effortA);
   Walrus hB(&hA , "helperB", effortB);
   printf("Aiming : %10u =\n"
          "   main: %10u +\n"
          "%s: %10u +\n%s: %10u\n-------------------\n"
      , MAX_ITERATION, countShare
      , hA.GetName(), hA.countShare
      , hB.GetName(), hB.countShare);

   // monitor random in debug
   #ifdef _DEBUG
      TestSeed();
      hA.TestSeed();
      hB.TestSeed();
      printf("-------------------\n");
   #endif // _DEBUG

   // do the parallel work
   LaunchHelpers(hA, hB);
   DoTheShare();
   Supervise(&hA, &hB);

   // merge all
   countSolo = countIterations;
   MergeResults(&hA);
   MergeResults(&hB);

   // don't work all day! have a dinner break ;-)
   sleep(20);
   printf("   main: %10u done\n", countSolo);
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
   countIterations += sem.scanCover;
}


uint Walrus::DoTheShare()
{
   // any last-train init
   (this->*sem.onShareStart)();

   // iterate until stopped
   while (countIterations < countShare) {
      DoIteration();
   }

   // signal
   isRunning = false;
   return countIterations;
}

//------------------------------------------------
// replicate system rand(), strip the code out of thread handling
//
void Walrus::MergeResults(Walrus *other)
{
   // helper is busy => do some of its work
   while (other->IsRunning()) {
      CoWork(other);
      countSolo += sem.scanCover;
   }

   // sum up
   for (int i = 0; i < HCP_SIZE; i++) {
      for (int j = 0; j < CTRL_SIZE; j++) {
         hitsCount[i][j] += other->hitsCount[i][j];
      }
   }
   countIterations += other->countIterations;

   // copy tasks
   if (arrToSolve && other->arrToSolve && other->countToSolve) {
      if (countToSolve + other->countToSolve <= MAX_TASKS_TO_SOLVE) {
         uint size = other->countToSolve * sizeof(DdsTask);
         memcpy(&arrToSolve[countToSolve], other->arrToSolve, size);
         countToSolve += other->countToSolve;
      } else {
         printf("\nFailed to merge %d from %s\n", other->countToSolve, other->GetName());
      }
   }
}

void Walrus::CoWork(Walrus *slowHelper)
{
   slowHelper->countShare -= sem.scanCover;
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


