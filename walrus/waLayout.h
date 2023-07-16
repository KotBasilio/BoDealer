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
      UCHAR s543_count;         // four  bits for (6543), then four bits for count
      UCHAR Ato7;               // eight bits for (AKQJT987)
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
   void AsComplement(u64 jo);
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

// twelve-layout lets counting some parameters in parallel, then queried
// -- high-card points
struct twlHCP 
{
   twlHCP() {}
   twlHCP(const SplitBits &hand);
   uint s, h, d, c, total;
};
// -- lengths
struct twLengths
{
   twLengths() {}
   twLengths(const SplitBits &hand);
   uint s, h, d, c;
};
// -- controls
struct twlControls
{
   twlControls() {}
   twlControls(const SplitBits &hand);
   uint s, h, d, c, total;
};
// -- all combined together
struct twContext {
   SplitBits   hand;
   twLengths   len;
   twlHCP      hcp;
   twlControls ctrl;
   twContext() : hand(0) {}
   twContext(const SplitBits& h): hand(h), len(h), hcp(h), ctrl(h) {}
};
// -- permuted for filtering with one hand fixed
#define SIZE_PERMUTE_PATTERN 10
union twPermutedContexts {
   struct {
      twContext xA, xB, xC;
   };
   twContext lay[SIZE_PERMUTE_PATTERN];
   twPermutedContexts(const SplitBits& a, const SplitBits& b, const SplitBits& c);
   twPermutedContexts(const SplitBits& a, const SplitBits& b, const SplitBits& c, uint hand);
};
// -- permuted for filtering, full transposition
union twPermutedFullFlip {
   twPermutedContexts p6;
   twContext lay[SIZE_PERMUTE_PATTERN * 4];
   twPermutedFullFlip(const SplitBits& a, const SplitBits& b, const SplitBits& c);
   void LayPattern(uint dest, uint iNewD);
};


