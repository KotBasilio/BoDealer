/************************************************************
 * Walrus semantics                               18.12.2019
 *
 ************************************************************/
#include <stdio.h> // printf

typedef unsigned char UCHAR;
typedef unsigned short int u16;
typedef unsigned int uint;
typedef unsigned long long u64;
typedef signed long long s64;

// Semantics: pick one of the list below
#define SEMANTIC_TRICOLOR_STRONG
//#define SEMANTIC_RED55_KINGS_PART_15_16

// -------------------------------------------------------- TRICOLOR_STRONG
#ifdef SEMANTIC_TRICOLOR_STRONG
#define TITLE_VERSION  "Fix 55 responder; 1NT-2d-3d-?? Question: partscore or a game? v2.0"
#define SEEK_BIDDING_DECISION
#endif // SEMANTIC_TRICOLOR_STRONG

// -------------------------------------------------------- RED55_KINGS_PART_15_16
#ifdef SEMANTIC_RED55_KINGS_PART_15_16
	#define TITLE_VERSION  "Fix 55 responder; 1NT-2d-3d-?? Question: partscore or a game? v2.0"
   #define SEEK_BIDDING_DECISION
#endif // SEMANTIC_RED55_KINGS_PART_15_16

// -----------------------------------------------------------------
// --- COMMON
// -----------------------------------------------------------------
const uint SOURCE_CARDS_COUNT = 52;
#define SBITS_LAYOUT_TWELVE

// -----------------------------------------------------------------
// --- WHAT TO SEEK
// -----------------------------------------------------------------

#ifdef SEEK_BIDDING_DECISION
   const uint REMOVED_CARDS_COUNT = 13;
   const uint ACTUAL_CARDS_COUNT = SOURCE_CARDS_COUNT - REMOVED_CARDS_COUNT;
   #define JK_USE_DOUBLE_DUMMY_SOLVER
#endif // SEEK_BIDDING_DECISION

// -----------------------------------------------------------------
// --- BITS LAYOUT VARIANTS
// -----------------------------------------------------------------
#ifdef SBITS_LAYOUT_TWELVE
   // a suit is encoded in 16 bits:
   // top 12 bits are for ranks; low 4 bits are for count
   // the deuce presence is deducted logically
   // NB: low bytes first, never overflow
   union twSuit
   {
      struct
      {
         UCHAR s543_count;
         UCHAR Ato7;
      };
      struct
      {
         UCHAR lo;
         UCHAR hi;
      };
      u16 w;

      u16 Count() { return (w & 0x000F); }
      uint Decrypt();
   private:
      bool HasDeuce();
   };

   #define SBITS_CHARS_LAYOUT             \
      /* sums up to 64 bits */            \
      struct twHand {                     \
         twSuit c;/* clubs */             \
         twSuit d;/* diamonds */          \
         twSuit h;/* hearts */            \
         twSuit s;/* spades */            \
      } w;

   #define SPADS 0x0001000000000000LL
   #define HEART 0x0000000100000000LL
   #define DIAMD 0x0000000000010000LL
   #define CLUBS 0x0000000000000001LL
   #define HIBITS ((SPADS+HEART+DIAMD+CLUBS) << 1)
      
#define SBITS_SEMANTIC_OPS                                              \
      bool IsBlank() { return (card.jo == 0L); }                        \
      bool IsEndIter() { return (CountAll() > 13); }

#endif // SBITS_LAYOUT_TWELVE

