/************************************************************
 * WALRUS multi-thread                                   2023
 *
 ************************************************************/
class Walrus;

struct WaMulti {
   WaMulti();
   void ShowLiveSigns(uint oneCover);
   void SaveThreeHands(twContext* lay);
   void StopHelpersSuddenly();
   uint Gathered() const { return countToSolve; }

   // main scan part
   bool             isRunning, shouldSignOut;
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

