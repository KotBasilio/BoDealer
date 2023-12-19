/************************************************************
 * Walrus semantics                               18.12.2019
 *
 ************************************************************/

#include <vector>

// output rows: we down, we make, a blank line, they down, they make, comparison
const uint IO_ROW_OUR_DOWN = 0;
const uint IO_ROW_OUR_MADE = IO_ROW_OUR_DOWN + 1;
const uint IO_ROW_ZEROES = IO_ROW_OUR_DOWN + 2;
const uint IO_ROW_THEIRS = IO_ROW_ZEROES + 1;
const uint IO_ROW_MAGIC_FLY = IO_ROW_THEIRS + 2;
const uint IO_ROW_SACRIFICE = IO_ROW_MAGIC_FLY;
const uint ORDER_BASE = 7;

// output columns
const uint IO_CAMP_OFF = 0;
const uint IO_CAMP_PREFER_SUIT = 1;
const uint IO_CAMP_SAME_NT = 2;
const uint IO_CAMP_MORE_NT = 3;
const uint IO_CAMP_PREFER_TO_BID = 1;
const uint IO_CAMP_REFRAIN_BIDDING = 3;

// hitsCount[][]; distribution 
// -- rows are big factor (aka hcp, 0 - 40)
// -- columns are smaller factor (aka controls, 0 - 12)
const int HCP_SIZE = 40 + 1;// to address 0 - 40
const int CTRL_SIZE = 12 + 4;// to get 16 as padding

// hits count and others
struct Progress {
   Progress();
   ucell hitsCount[HCP_SIZE][CTRL_SIZE];
   ucell step, went, margin;
   ucell countExtraMarks;
   u64  delta1, delta2;
   void Init(ucell _step);
   bool Step();
   void Up(ucell idx);
   void StoreCountToGo(ucell count);
};

// Filters
class WaFilter
{
public:
   WaFilter() : progress(nullptr), sem(nullptr) {}
   void Bind(class Walrus* _walrus);
   bool ScanOut(twContext* lay);

   // Micro filters
   uint RejectAll(twContext* lay, const uint *par) { return 2; }
   uint OKNum(twContext* lay, const uint *par);
   uint ExactShape(twContext* lay, const uint* par);
   uint ModelShape(twContext* lay, const uint *par);
   uint PointsRange(twContext* lay, const uint *par);
   uint PointsLimit(twContext* lay, const uint *par);
   uint PointsAtLeast(twContext* lay, const uint *par);
   uint KeyCardsRange(twContext* lay, const uint* par);
   // -
   uint ControlsRange(twContext* lay, const uint *par);
   uint LineControlsRange(twContext* lay, const uint* par);
   uint LineAcesRange(twContext* lay, const uint* par);
   uint LineKeyCardsSpade(twContext* lay, const uint* par);
   uint LinePointsRange(twContext* lay, const uint *par);
   // -
   uint ClubPointsLimit(twContext* lay, const uint *par);
   uint HeartPointsLimit(twContext* lay, const uint *par);
   uint SpadePointsLimit(twContext* lay, const uint *par);
   uint DiamPointsLimit(twContext* lay, const uint *par);
   // -
   uint SpadesNatural(twContext* lay, const uint* par);
   uint NoMajorFit(twContext* lay, const uint *par);
   // -
   uint SpadesLen(twContext* lay, const uint *par);
   uint HeartsLen(twContext* lay, const uint *par);
   uint DiamondsLen(twContext* lay, const uint *par);
   uint ClubsLen(twContext* lay, const uint *par);
   // -
   uint NoOvcOn1LevOpen(twContext* lay, const uint *par);
   uint NoOvercall(twContext* lay, const uint *par);
   uint No7Plus(twContext* lay, const uint *par);
   uint No2SuiterAntiSpade(twContext* lay, const uint *par);
   uint No2SuitsAntiHeart(twContext* lay, const uint *par);
   uint No2SuitsMinors(twContext* lay, const uint* par);
   uint TakeoutOfClubs(twContext* lay, const uint* par);
   // -- branching
   uint AnyInListBelow(twContext* lay, const uint *par);
   uint EndList(twContext* lay, const uint *par);
private:
   Progress *progress;
   struct Semantics* sem;
   uint LineKeyCardsRange(twContext* lay, const uint* par, u64 kc_mask);
   bool KeyCardsRange(u64 jo, u64 kc_mask, uint from, uint to);

   struct semExec {
      uint ip;     // instruction pointer
      uint depth;  // how much are we within nested condition
      void Reset() { ip = 0; depth = 0; }
   } exec;
};

#include "WaSemMicro.h"
