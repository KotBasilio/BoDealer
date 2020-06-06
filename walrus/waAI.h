/************************************************************
 * Walrus AI                                       27.12.2019
 *
 ************************************************************/

// SA = "Semi-AI"
typedef SplitBits saCard;
typedef SplitBits saHand;

#ifdef _DEBUG
   #define DEBUG_UNEXPECTED printf("\nwtf?")
#else
   #define DEBUG_UNEXPECTED
#endif // _DEBUG

const int COUNT_AI_CARDS = 16;

class SaDeck
{
public:
   saCard  arr[COUNT_AI_CARDS];

   SaDeck();
};

// Double-dummy solver integration
void sample_main_SolveBoard();
void sample_main_SolveBoard_S1();
void sample_main_PlayBin();
void sample_main_JK_Solve();

struct DdsTask 
{
   union DTUnion {
      SplitBits hand;
      DTUnion() {}
      void Init(SplitBits &sb);
   };
   DTUnion partner;
   DTUnion rho;

   void Init(SplitBits &part, SplitBits &nt);
};

union DdsPack 
{
   DdsTask  task;
   struct {
      u64 jo1;
      u64 jo2;
   };
   DdsPack() {}
};

#define NORTH    0
#define EAST     1
#define SOUTH    2
#define WEST     3

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


