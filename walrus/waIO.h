// config is gonna be here

// file names
#define START_FROM_FNAME "start_from.txt"
#define COMMAND_FNAME    "in_cmd.txt"
#define OUT_FNAME        "stm%d%d%d%d%d%d%d%d%d%d.txt"
#define PROGRESS_FNAME   "waProgress.txt"
const int FNAME_SIZE = 128 + 64;
struct waFileNames {
   char StartFrom[FNAME_SIZE];
   char Command [FNAME_SIZE];
   char Progress[FNAME_SIZE];
   char Solution[FNAME_SIZE];
   void Build();
};

struct MiniUI {
   bool  exitRequested;
   bool  reportRequested;
   bool  firstAutoShow;
   int   minControls;
   int   irGoal, irFly;
   s64   biddingBetterBy;
   int   farCol;
   char  declTrump[10], declSeat[10], seatOnLead[10], theirTrump[10];
   MiniUI();
   void Init(int trump, int first);
   void DisplayBoard(twContext* lay);
   void WaitAnyKey();
   void FillMiniRows();
   void Run();
};


enum WE_REPORT_TYPE {
   WREPORT_NONE = 0,
   WREPORT_HCP,
   WREPORT_CONTROLS,
   WREPORT_SUIT,
};

constexpr uint WA_CONTR_TITLE_LEN = 5;
struct WaConfig {
   waFileNames namesBase;

   char titleOurContract[WA_CONTR_TITLE_LEN];   // kind of "2S", "6NT"
   char titleTheirContract[WA_CONTR_TITLE_LEN]; // kind of "2Sx", "PASS" 

   WaConfig();

   int   primGoal;  // goal tricks in our primary contract
   int   soTrump, soFirst;

   int   otherGoal; // goal tricks either in our secondary contract or in their contract

   int   postmSuit; // for post-mortem

   WE_REPORT_TYPE detailedReportType;

   void ReadStart();
};

extern WaConfig cfgTask;

// Oscar interactions

#define GRIFFINS_CLUB_IS_CLOSING "Shutdown"

struct OscarTheOwl {
   void Show(const char* format, ...);
   void Silent(const char* format, ...);
   void OnProgress(const char* format, ...);
   void OnDone(const char* format, ...);
   void Send(char* msg);
   void Goodbye();
private:
   static const size_t bufferSize = 512;
   static char buffer[bufferSize];
};

extern OscarTheOwl owl;
