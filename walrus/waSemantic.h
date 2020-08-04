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
// -- tasks for bidding decision:
//#define SEMANTIC_JULY_AUTO_FITO_PLANKTON
//#define SEMANTIC_JUNE_GAMBLE_OR_5C
//#define SEMANTIC_TRICOLOR_STRONG
//#define SEMANTIC_RED55_KINGS_PART_15_16
// -- tasks for opening lead:
//#define SEMANTIC_JUNE_MAX_5D_LEAD
//#define SEMANTIC_JUNE_ZAKHAROVY_PREC_3NT
//#define SEMANTIC_JUNE_LEAD_3343
#define SEMANTIC_AUG_LEAD_VS_3H
// -- tasks for one hand:
//#define SEMANTIC_KEYCARDS_10_12

// -------------------------------------------------------- 
#ifdef SEMANTIC_AUG_LEAD_VS_3H
#define TITLE_VERSION  "Fix a hand with an ace;\n....s\nQuestion: what to lead?\nVer 2.0 "
#define SEEK_OPENING_LEAD
#endif 

// -------------------------------------------------------- 
#ifdef SEMANTIC_KEYCARDS_10_12
#define TITLE_VERSION  "Abstract 10-12 with 4+ major;\nQuestion: how keycards are distributed? Ver 2.0 "
#define SINGLE_HAND_TASK
#endif 

// -------------------------------------------------------- 
#ifdef SEMANTIC_JULY_AUTO_FITO_PLANKTON
#define TITLE_VERSION  "Fix 7 hcp hand;\n(1s)-2h-(2s)-3h\n(p)-p-(3s)-??\nQuestion: pass/dbl or a game? Ver 2.0 "
#define SEEK_BIDDING_LEVEL
#define SCORE_OPP_CONTRACT
#define OC_TRUMPS       SOL_SPADES
#define OC_ON_LEAD      SOUTH
#endif 

// -------------------------------------------------------- JUNE_MAX_5D_LEAD
#ifdef SEMANTIC_JUNE_MAX_5D_LEAD
#define TITLE_VERSION  "Fix a strong hand;\n(p)-p-(1d)-1h\n(3d)-3h-(p)-4h\n(p)-p-(5d)-X-all pass\nQuestion: what to lead?\nVer 2.0 "
#define SEEK_OPENING_LEAD
#endif 

// -------------------------------------------------------- JUNE_LEAD_3343
#ifdef SEMANTIC_JUNE_LEAD_3343
#define TITLE_VERSION  "Fix a weak hand;\np-(p)-p-(1s)\np-(1NT)-p-(3NT)-all pass\nQuestion: what to lead?\nVer 2.0 "
#define SEEK_OPENING_LEAD
#endif 

// -------------------------------------------------------- JUNE_GAMBLE_OR_5C
#ifdef SEMANTIC_JUNE_GAMBLE_OR_5C
   #define TITLE_VERSION  "Fix 3307 fourth hand;\n(2h)-p-(p)-??\nQuestion: 3NT, partscore or 5C? Ver 2.0 "
   #define SEEK_DENOMINATION
#endif 

// -------------------------------------------------------- JUNE_ZAKHAROVY_PREC_3NT
#ifdef SEMANTIC_JUNE_ZAKHAROVY_PREC_3NT
   #define TITLE_VERSION  "Fix a weak hand;\n(2c)-p-(3NT)-all pass\nQuestion: what to lead?\nVer 2.0 "
   #define SEEK_OPENING_LEAD
#endif 

// -------------------------------------------------------- TRICOLOR_STRONG
#ifdef SEMANTIC_TRICOLOR_STRONG
   #define TITLE_VERSION  "Fix tricolor opening;\np-(p)-1c-(1d)\np-(p)-dbl-(2d)\n2h-(p)-??\nQuestion: partscore or a game? Ver 2.0 "
   #define SEEK_BIDDING_LEVEL
#endif 

// -------------------------------------------------------- RED55_KINGS_PART_15_16
#ifdef SEMANTIC_RED55_KINGS_PART_15_16
	#define TITLE_VERSION  "Fix 55 responder; 1NT-2d-3d-?? Question: partscore or a game? v2.0"
   #define SEEK_BIDDING_LEVEL
#endif 

// -----------------------------------------------------------------
// --- COMMON
// -----------------------------------------------------------------
const uint SOURCE_CARDS_COUNT = 52;
#define SBITS_LAYOUT_TWELVE

// -----------------------------------------------------------------
// --- WHAT TO SEEK
// DOC: solutions parameter
// 1 -- Find the maximum number of tricks for the side to play. Return only one of the optimum cards and its score.
// 2 -- Find the maximum number of tricks for the side to play. Return all optimum cards and their scores.
// 3 -- Return all cards that can be legally played, with their scores in descending order.
// -----------------------------------------------------------------

#ifdef SEEK_BIDDING_LEVEL
   const uint REMOVED_CARDS_COUNT = 13;
   const uint ACTUAL_CARDS_COUNT = SOURCE_CARDS_COUNT - REMOVED_CARDS_COUNT;
   #define PARAM_SOLUTIONS_DDS   1
   #define FIXED_HAND_NORTH
#endif // SEEK_BIDDING_LEVEL

#ifdef SEEK_OPENING_LEAD
   const uint REMOVED_CARDS_COUNT = 13;
   const uint ACTUAL_CARDS_COUNT = SOURCE_CARDS_COUNT - REMOVED_CARDS_COUNT;
   #define PARAM_SOLUTIONS_DDS   3
   #define FIXED_HAND_WEST
#endif // SEEK_BIDDING_LEVEL

#ifdef SINGLE_HAND_TASK
   const uint REMOVED_CARDS_COUNT = 0;
   const uint ACTUAL_CARDS_COUNT = SOURCE_CARDS_COUNT - REMOVED_CARDS_COUNT;
   #define PARAM_SOLUTIONS_DDS   1
#endif // SINGLE_HAND_TASK

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

