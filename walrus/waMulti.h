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
   bool             isRunning, shouldSignOut, shownFirstLiveSign;
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

// Oscar interactions
#define GRIFFINS_CLUB_RUNS       "Lights are on"
#define GRIFFINS_CLUB_IS_CLOSING "Shutdown"

struct OscarTheOwl {
   OscarTheOwl();
   void Show(const char* format, ...);
   void Silent(const char* format, ...);
   void OnProgress(const char* format, ...);
   void OnDone(const char* format, ...);
   void Send(char* msg);
   void Flush();
   void Goodbye();
private:
   static const size_t bufferSize = 512;
   static char buffer[bufferSize];
   static char earlyLine[bufferSize];
};

extern OscarTheOwl owl;
