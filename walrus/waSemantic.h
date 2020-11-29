/************************************************************
 * Walrus semantics                               18.12.2019
 *
 ************************************************************/
#include <stdio.h> // printf

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

   #define SPADS   0x0001000000000000LL
   #define HEART   0x0000000100000000LL
   #define DIAMD   0x0000000000010000LL
   #define CLUBS   0x0000000000000001LL
   #define HIBITS  ((SPADS+HEART+DIAMD+CLUBS) << 1)
   #define ANY_ACE 0x8000800080008000LL

#define SBITS_SEMANTIC_OPS                                              \
      bool IsBlank() { return (card.jo == 0L); }                        \
      bool IsEndIter() { return (CountAll() > 13); }

#endif // SBITS_LAYOUT_TWELVE

// all cards + flip over 12 cards
const uint FLIP_OVER_SIZE = 12;
const uint FLIP_OVER_START_IDX = ACTUAL_CARDS_COUNT;
const uint DECK_ARR_SIZE = 64;// random does work to this range
const uint RIDX_SIZE = 4;
const uint SUPERVISE_CHUNK = 10;
const uint SUPERVISE_REASONABLE = SUPERVISE_CHUNK * ACTUAL_CARDS_COUNT * 2;

// hitsCount[][]; distribution 
// -- rows are big factor (aka hcp, 0 - 40)
// -- columns are smaller factor (aka controls, 0 - 12)
const int HCP_SIZE = 40 + 1;// to address 0 - 40
const int CTRL_SIZE = 12 + 4;// to get 16 as padding

// file names
#define START_FROM_FNAME "start_from.txt"
#define INDICES_FNAME  "Indices.txt"
#define OUT_FNAME      "stm%d%d%d%d%d%d%d%d%d%d.txt"
#define PROGRESS_FNAME "w09progress.txt"
const int FNAME_SIZE = 128 + 64;
static struct tFileNames {
    char StartFrom[FNAME_SIZE];
    char Indices[FNAME_SIZE];
    char Progress[FNAME_SIZE];
    char Solution[FNAME_SIZE];
} namesBase;

// split bits card to operate super-fast
struct SplitBits {
    union SBUnion {// low bytes first, so never overflow
        SBITS_CHARS_LAYOUT;
        u64 jo;
    };
    SBUnion card;

    SplitBits()                  { card.jo = 0L; }
    explicit SplitBits(u64 jo)   { card.jo = jo; }
    SplitBits(uint hld, uint waPos);
    u16 CountAll();
    SBITS_SEMANTIC_OPS
};
inline u16 SplitBits::CountAll()
{
   return card.w.c.Count() + 
          card.w.d.Count() + 
          card.w.h.Count() + 
          card.w.s.Count();
}
extern SplitBits sbBlank;

// SA = "Semi-AI"
typedef SplitBits saCard;
typedef SplitBits saHand;

// twelve-layout lets counting some parameters in parallel, then queried
// -- high-card points
struct twlHCP 
{
   twlHCP(SplitBits &hand);
   uint s, h, d, c, total;
};
// -- lengths
struct twLengths
{
   twLengths(SplitBits &hand);
   uint s, h, d, c;
};
