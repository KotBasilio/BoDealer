/************************************************************
 * Walrus BITS LAYOUT VARIANTS
 // each suit is encoded in 16 bits:
 // -- top 12 bits are for ranks; 
 // -- low 4 bits are for count
 // the deuce presence is deducted logically
 // NB: low bytes first, so never overflow
 ************************************************************/

#define SBITS_LAYOUT_TWELVE

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

