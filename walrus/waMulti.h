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
   uint NumFiltered() const { return countToSolve; }

   // main scan part
   bool             isRunning, shouldSignOut;
   const char *     nameHlp;
   ucell            countIterations, countShare, countSolo, countShowLiveSign;
   // aftermath double-dummy
   uint       maxTasksToSolve;
   WaTask*    arrToSolve;
   uint       countToSolve;
   // supervision of helpers
   Walrus* hA;
   Walrus* hB;
};

// co-working on filtering
const uint COWORK_CHUNK = 10;
const uint COWORK_REASONABLE = COWORK_CHUNK * ACTUAL_CARDS_COUNT * 2;
