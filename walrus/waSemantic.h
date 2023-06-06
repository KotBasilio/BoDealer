/************************************************************
 * Walrus semantics                               18.12.2019
 *
 ************************************************************/

// output rows
const uint IO_ROW_OUR_DOWN = 0;
const uint IO_ROW_OUR_MADE = IO_ROW_OUR_DOWN + 1;
const uint IO_ROW_ZEROES = IO_ROW_OUR_DOWN + 2;
const uint ORDER_BASE = 3;
const uint IO_ROW_THEIRS = 10;
const uint IO_ROW_MYFLY = 12;

// output columns
const uint IO_CAMP_OFF = 0;
const uint IO_CAMP_PREFER_SUIT = 1;
const uint IO_CAMP_SAME_NT = 2;
const uint IO_CAMP_MORE_NT = 3;
const uint SKIP_BY_PART = 1;
const uint SKIP_BY_RESP = 2;
const uint SKIP_BY_OPP = 3;
const uint SKIP_BY_DIRECT = SKIP_BY_RESP;
const uint SKIP_BY_SANDWICH = SKIP_BY_OPP;

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
#define COMMAND_FNAME    "in_cmd.txt"
#define OUT_FNAME        "stm%d%d%d%d%d%d%d%d%d%d.txt"
#define PROGRESS_FNAME   "waProgress.txt"
const int FNAME_SIZE = 128 + 64;
struct waFileNames {
    char StartFrom[FNAME_SIZE];
    char Command[FNAME_SIZE];
    char Progress[FNAME_SIZE];
    char Solution[FNAME_SIZE];
    void Build();
};

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
   twlHCP(SplitBits &hand);
   uint s, h, d, c, total;
};
// -- lengths
struct twLengths
{
   twLengths(SplitBits &hand);
   uint s, h, d, c;
};
// -- controls
struct twlControls
{
   twlControls(SplitBits &hand);
   uint s, h, d, c, total;
};

// hits count and others
struct Progress {
   Progress();
   uint hitsCount[HCP_SIZE][CTRL_SIZE];
   uint step, went, margin;
   uint countExtraMarks;
   u64  delta1, delta2;
   void Init(uint _step);
   bool Step();
   void Up(uint idx);
};

// Filters
class WaFilter
{
   Progress *progress;
public:
   WaFilter() : progress(nullptr) {}
   void Bind(class Walrus* _walrus);
   void RejectAll(SplitBits part, SplitBits lho, SplitBits rho);
   uint DepRejectAll(SplitBits &part, uint &camp, SplitBits &lho, SplitBits &rho) { camp = 2; return 1; }

   // One/4 hand tasks:
   uint Spade4(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint Splinter(SplitBits& partner, uint& camp, SplitBits& lho, SplitBits& rho);
   void ClassifyOnScan(SplitBits a, SplitBits b, SplitBits c);

   // Bidding decision one-sided:
   uint R55(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint AugMultiVul(SplitBits& partner, uint& camp, SplitBits& lho, SplitBits& rho);
   uint Aug3NTOnFit(SplitBits& partner, uint& camp, SplitBits& lho, SplitBits& rho);
   uint JanSpadesGame(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint SeptMajors(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint SlamTry(SplitBits &partner, uint &camp, SplitBits &rho, SplitBits &lho);
   uint Dec12_2425(SplitBits& partner, uint& camp, SplitBits& rho, SplitBits& lho);
   uint Sep10_4252(SplitBits& partner, uint& camp, SplitBits& rho, SplitBits& lho);
   uint JanDblThenH(SplitBits& partner, uint& camp, SplitBits& rho, SplitBits& lho);
   uint NovInvitePrecision(SplitBits& partner, uint& camp, SplitBits& rho, SplitBits& lho);
   uint SomeInvite(SplitBits& partner, uint& camp, SplitBits& rho, SplitBits& lho);

   // Bidding decision competitive:
   uint OctWeakGambling(SplitBits& partner, uint& camp, SplitBits& rho, SplitBits& lho);
   uint FitoJuly(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint AugSplitFit(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint NovSlam(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint DecTopHearts(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint NovDbl3NT(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint Dec21Dbl3NT(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint Tricolor(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint TriSunday(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint DecAcceptTo4S(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint FebManyHearts(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint MixedPreventive(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint Mixed2DwM(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);

   // Opening lead:
   uint NovLevk(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint LeadAugVs3H(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint JuneVZ(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint LeadFlat(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint LeadMax5D(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
};

// types of filters
typedef void(WaFilter::* SemFilterOut)(SplitBits a, SplitBits b, SplitBits c);
typedef uint(WaFilter::* DepFilterOut)(SplitBits& part, uint& camp, SplitBits& lho, SplitBits& rho);// deprecated in 3.0

