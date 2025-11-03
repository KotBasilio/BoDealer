/************************************************************
 // Walrus BITS LAYOUT VARIANTS
 // each suit is encoded in 4 hexadecimal numbers, i.e. two chars, 16 bits:
 // -- top 12 bits are for ranks, except a deuce; 
 // -- low 4 bits are for count
 // -- the deuce presence is deducted logically
 // NB1: we combine cards by adding and subtracting card.jo, so it goes super-fast and parallel
 // NB2: low bytes first, so we never get an overflow
 ************************************************************/
#pragma once

// DDS layout -- suitable for fast conversion from Walrus format. see twSuit::Decrypt()
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

union twSuit
{
   struct
   {
      UCHAR s543_count;         // four  bits for cards (6543), then four bits for count
      UCHAR Ato7;               // eight bits for cards (AKQJT987)
   };
   struct
   {
      UCHAR lo;
      UCHAR hi;
   };
   u16 w;

   u16 Count() const { return (w & 0x000F); }
   uint Decrypt() const {
      uint holding = (w & 0xFFF0) >> 1;
      if (HasDeuce()) {
         holding |= R2;
      }
      return holding;
   }

private:
   bool HasDeuce() const {
      // count bits for 12 cards -- their mask is 0xFFF0
      uint y = (w & 0x5550) + ((w & 0xAAA0) >> 1);
      uint z = (y & 0x3333) + ((y & 0xCCCC) >> 2);
      y      = (z & 0x0F0F) + ((z & 0xF0F0) >> 4);
      z      = (y & 0x00FF) + ((y & 0xFF00) >> 8);

      // count is greater => deuce is present
      return Count() > z;
   }
};

union SBUnion {
   struct twHand {/* sums up to 64 bits */
      twSuit c;/* clubs */   
      twSuit d;/* diamonds */
      twSuit h;/* hearts */ 
      twSuit s;/* spades */ 
   } w;
   u64 jo;
};

// split bits cards and/or hands
struct SplitBits {
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

#define BO_SPADS   0x0001000000000000LL
#define BO_HEART   0x0000000100000000LL
#define BO_DIAMD   0x0000000000010000LL
#define BO_CLUBS   0x0000000000000001LL
#define ANY_ACE    0x8000800080008000LL

// twelve-layout lets counting some parameters in parallel, then queried
// -- high-card points
struct twlHCP {
   twlHCP() {} // intentional, for perf
   twlHCP(const SplitBits &hand);
   union {
      struct {
         uint s, h, d, c, total;
      };
      uint arr[5];
   };
   void Zero() { s = h = d = c = total = 0; }
};
// -- lengths
struct twLengths {
   twLengths() {}
   twLengths(const SplitBits &hand);
   union {
      struct {
         uint s, h, d, c;
      };
      uint arr[4];
   };
};
// -- controls
struct twlControls {
   twlControls() {}
   twlControls(const SplitBits &hand);
   union {
      struct {
         uint s, h, d, c, total;
      };
      uint arr[5];
   };
};


// then we combine all 3 characteristics together to make a context for permutation
struct twContext {
   SplitBits   hand;
   twLengths   len;
   twlHCP      hcp;
   twlControls ctrl;
   twContext() : hand(0) {}
   twContext(const SplitBits& h): hand(h), len(h), hcp(h), ctrl(h) {}
};

// now we combine permuted context for filtering with one hand fixed
constexpr uint SIZE_PERMUTE_PATTERN = 10;
union twPermutedContexts {
   struct {
      twContext xA, xB, xC;
   };
   twContext lay[SIZE_PERMUTE_PATTERN];
   twPermutedContexts(const SplitBits& a, const SplitBits& b, const SplitBits& c);
   twPermutedContexts(const SplitBits& a, const SplitBits& b, const SplitBits& c, uint hand);
};

// and finally we combine them for full transposition used in 4-hands scan
union twPermutedFullFlip {
   twPermutedContexts p6;
   twContext lay[SIZE_PERMUTE_PATTERN * 4];
   twPermutedFullFlip(const SplitBits& a, const SplitBits& b, const SplitBits& c);
   void LayPattern(uint dest, uint iNewD);
};

