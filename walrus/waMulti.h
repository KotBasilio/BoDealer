/************************************************************
 * WALRUS multi-thread                                   2023
 *
 ************************************************************/
class Walrus;

struct WaMulti {
   WaMulti();
   bool ShowLiveSigns(uint oneCover);
   // main scan part
   bool             isRunning;
   const char *     nameHlp;
   ucell            countIterations, countShare, countSolo, countShowLiveSign;
   // aftermath double-dummy
   uint       maxTasksToSolve;
   DdsTask*   arrToSolve;
   uint       countToSolve;
   // supervision of helpers
   Walrus* hA;
   Walrus* hB;
};

