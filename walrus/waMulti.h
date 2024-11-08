/************************************************************
 * WALRUS multi-thread                                   2023
 *
 ************************************************************/
class Walrus;

struct WaMulti {
   WaMulti();
   void NOP() {}
   void Setup(const char *nameH, ucell ourShare);
   void AllocFilteredTasksBuf();
   void ShowLiveSigns(uint oneCover);
   void SaveThreeHands(twContext* lay);
   void StopHelpersSuddenly();
   bool IsRunning(void) const  { return isRunning; }
   uint NumFiltered() const    { return countToSolve; }
   ucell CloudSize() const     { return countShare; }
   ucell Remains() const       { return (countIterations < CloudSize()) ? CloudSize() - countIterations : 0; }
   const char *SizeToReadable(const size_t bsize);

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
constexpr uint COWORK_CHUNK = 10;
constexpr uint COWORK_REASONABLE = COWORK_CHUNK * ACTUAL_CARDS_COUNT * 2;
constexpr uint MAX_TASKS_TO_SOLVE = 200 * 1000;

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
