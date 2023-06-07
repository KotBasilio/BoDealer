/************************************************************
 * Walrus semantics                               18.12.2019
 *
 ************************************************************/

#include <vector>

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
// -- permuted for filtering
union twPermutedContexts {
   struct {
      twContext xA, xB, xC;
   };
   twContext lay[9];
   twPermutedContexts(const SplitBits& a, const SplitBits& b, const SplitBits& c);
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
public:
   WaFilter() : progress(nullptr) {}
   void Bind(class Walrus* _walrus);
   uint DepRejectAll(SplitBits &part, uint &camp, SplitBits &lho, SplitBits &rho) { camp = 2; return 1; }

   // ver 2.0 Filters
   // -- One hand tasks:
   uint Spade4(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   // -- Bidding decision one-sided:
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
   // -- Bidding decision competitive:
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
   // -- Opening lead:
   uint NovLevk(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint LeadAugVs3H(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint JuneVZ(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint LeadFlat(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);
   uint LeadMax5D(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho);

   // Micro filters
   uint RejectAll(twContext* lay, const uint *par) { return 2; }
   uint OK100(twContext* lay, const uint *par);
   uint OKNum(twContext* lay, const uint *par);
   uint MicPoints(twContext* lay, const uint *par);
   uint ExactShape(twContext* lay, const uint *par);
private:
   Progress *progress;
};

#include "WaSemMicro.h"

// a class to rule task logic. fill them on init. 
// then values are constant through all solving
typedef void (Walrus::* SemFuncType)();
typedef void (Shuffler::* ShufflerFunc)();
typedef void (Walrus::* SemScoring)(DdsTricks &tr);
typedef void (Walrus::* SemOnBoardAdded)(twContext* lay);
typedef uint (WaFilter::* DepFilterOut)(SplitBits& part, uint& camp, SplitBits& lho, SplitBits& rho);// deprecated since 3.0
struct Semantics {
   SemFuncType              onInit;
   SemFuncType              onShareStart;
   SemFuncType              onScanCenter;
   ShufflerFunc             fillFlipover;
   std::vector<MicroFilter> vecFilters;
   SemOnBoardAdded          onBoardAdded;
   SemScoring               onScoring;
   SemScoring               onSolvedTwice;
   SemFuncType              onAfterMath;
   DepFilterOut             onDepFilter;
   uint scanCover; // how much iterations covers one scan
   Semantics();
};

extern Semantics semShared;

