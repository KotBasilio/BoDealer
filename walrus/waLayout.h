/************************************************************
 * Walrus BITS LAYOUT VARIANTS
 // each suit is encoded in 16 bits:
 // -- top 12 bits are for ranks; 
 // -- low 4 bits are for count
 // the deuce presence is deducted logically
 // NB: low bytes first, so never overflow
 ************************************************************/

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

#define SPADS   0x0001000000000000LL
#define HEART   0x0000000100000000LL
#define DIAMD   0x0000000000010000LL
#define CLUBS   0x0000000000000001LL
#define ANY_ACE 0x8000800080008000LL

#define SBITS_CHARS_LAYOUT             \
   /* sums up to 64 bits */            \
   struct twHand {                     \
      twSuit c;/* clubs */             \
      twSuit d;/* diamonds */          \
      twSuit h;/* hearts */            \
      twSuit s;/* spades */            \
   } w;

// split bits card to operate super-fast
struct SplitBits {
   union SBUnion {// low bytes first, so never overflow
      SBITS_CHARS_LAYOUT;
      u64 jo;
   };
   SBUnion card;

   SplitBits()                       { card.jo = 0L; }
   explicit SplitBits(u64 jo)        { card.jo = jo; }
   SplitBits(const SplitBits &other) { card.jo = other.card.jo; }
   SplitBits(uint hld, uint waPos);
   SplitBits(const SplitBits &a, const SplitBits &b, const SplitBits &c);
   bool operator != (const SplitBits &other) const { return card.jo != other.card.jo; }
   u16 CountAll();
   bool IsBlank() { return (card.jo == 0L); }
   u16 IsEndIter() { return (CountAll() & (u16)(0x10)); }
};
inline u16 SplitBits::CountAll()
{
   return card.w.c.Count() + 
          card.w.d.Count() + 
          card.w.h.Count() + 
          card.w.s.Count();
}
extern SplitBits sbBlank;

// all cards + flip over 12 cards
const uint FLIP_OVER_SIZE = 12;
const uint FLIP_OVER_START_IDX = ACTUAL_CARDS_COUNT;
const uint DECK_ARR_SIZE = 64;// random does work to this range
const uint RIDX_SIZE = 4;
const uint SUPERVISE_CHUNK = 10;
const uint SUPERVISE_REASONABLE = SUPERVISE_CHUNK * ACTUAL_CARDS_COUNT * 2;

