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

constexpr uint WA_CONTR_TITLE_LEN = 5;
struct WaConfig {
   waFileNames namesBase;

   char titleOurContract[WA_CONTR_TITLE_LEN];   // kind of "2S", "6NT"
   char titleTheirContract[WA_CONTR_TITLE_LEN]; // kind of "2Sx", "PASS" 

   WaConfig();

   void ReadStart();
};

extern WaConfig cfgTask;
