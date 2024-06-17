/************************************************************
 * Walrus AI                                       27.12.2019
 *
 ************************************************************/

#ifdef _DEBUG
   #define DEBUG_UNEXPECTED printf("\nwtf?")
#else
   #define DEBUG_UNEXPECTED
#endif // _DEBUG

// Double-dummy solver integration
void sample_main_SolveBoard();
void sample_main_SolveBoard_S1();
void sample_main_PlayBin();
void sample_main_JK_Solve();

#define NORTH    0
#define EAST     1
#define SOUTH    2
#define WEST     3

// version when we need to store only two hands
// It's usefult for:
// -- bidding decisions. Then a fixed hand is North
// -- opening lead decisions. Then a fixed hand is West
struct WaTask2 
{
   SplitBits partner;
   SplitBits oneOpp;

   CodedTricks tricks;

   WaTask2() {}
   void Init(SplitBits& a, SplitBits& b) { partner = a; oneOpp = b; }
   void Init(twContext* lay);
};

// version with 3 hands stored
struct WaTask3
{
   SplitBits north, east, south;

   WaTask3() {}
   void Init(SplitBits& a, SplitBits& b) { DEBUG_UNEXPECTED; }
   void Init(twContext* lay);
};

#ifdef FOUR_HANDS_TASK
   typedef WaTask3 WaTask;
#else
   typedef WaTask2 WaTask;
#endif

#define SOL_SPADES   0
#define SOL_HEARTS   1
#define SOL_DIAMONDS 2
#define SOL_CLUBS    3
#define SOL_NOTRUMP  4

#define SPD SOL_SPADES
#define HRT SOL_HEARTS
#define DMD SOL_DIAMONDS
#define CLB SOL_CLUBS

#define K2       2
#define K3       3
#define K4       4
#define K5       5
#define K6       6
#define K7       7
#define K8       8
#define K9       9
#define KT      10
#define KJ      11
#define KQ      12
#define KK      13
#define KA      14

struct DdsTricks
{
   // number of tricks in basic contract
   uint plainScore; 

   // tricks for each opening lead
   #ifdef SEEK_OPENING_LEAD
   struct Lead
   {
      uint S, H, D, Ñ;
      Lead() { S = H = D = Ñ = 13; }
   } lead;
   #endif // SEEK_OPENING_LEAD

   DdsTricks() : plainScore(0) {}
   void Init(struct futureTricks &fut);
};

void HandleErrorDDS(struct deal &dl, int res);

