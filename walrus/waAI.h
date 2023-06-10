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
struct DdsTask2 
{
   SplitBits partner;
   SplitBits rho;

   DdsTask2() {}
   void Init(SplitBits& a, SplitBits& b) { partner = a; rho = b; }
};

// version with 3 hands stored
struct DdsTask3
{
   SplitBits north, east, south;

   DdsTask3() {}
   void Init(SplitBits& a, SplitBits& b) { DEBUG_UNEXPECTED; }
   void Init(twContext* lay);
};

#ifdef FOUR_HANDS_TASK
   typedef DdsTask3 DdsTask;
#else
   typedef DdsTask2 DdsTask;
#endif

#define SOL_SPADES   0
#define SOL_HEARTS   1
#define SOL_DIAMONDS 2
#define SOL_CLUBS    3
#define SOL_NOTRUMP  4

#define R2     0x0004
#define R3     0x0008
#define R4     0x0010
#define R5     0x0020
#define R6     0x0040
#define R7     0x0080
#define R8     0x0100
#define R9     0x0200
#define RT     0x0400
#define RJ     0x0800
#define RQ     0x1000
#define RK     0x2000
#define RA     0x4000
#define RFULL  0x7ffc

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
uint WaCalcHCP(const deal& dl);

