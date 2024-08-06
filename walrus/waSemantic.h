/************************************************************
 * Walrus semantics                               18.12.2019
 *
 ************************************************************/

 // -- hcp postmortem
#define IO_ROW_HCP_START 7
#define IO_SHIFT_FOR_EXTRA_MARKS  27
#define IO_ROW_FILTERING (3  + IO_SHIFT_FOR_EXTRA_MARKS)
#define IO_ROW_SELECTED  IO_ROW_FILTERING

 // hitsCount[][] array dimensions and distribution 
 // -- rows are big factor (aka hcp, 0 - 40)
 // -- columns are smaller factor (aka controls, 0 - 12)
constexpr int CTRL_SIZE = 12 + 4;// to get 16 as padding
constexpr int IPR_COMPACTION = 16;// see CellByIPR()
constexpr int HCP_SIZE = IO_ROW_FILTERING + IPR_COMPACTION;

// hits count and others
struct Progress {
   friend class Walrus;
   Progress();
   ucell step, went, margin;
   u64  delta1, delta2;
   bool isDoneAll;
   void Init(ucell _step);
   bool Step();
   void Up(ucell idx);
   void StoreCountToGo(ucell count);
   ucell GetDiscardedBoardsCount();
   ucell PeekByIPR(uint ip, uint reason) { return CellByIPR(ip, reason); }

   // operating marks
   void SelectedMark();
   void FilteredOutMark(uint ip, uint reason);
   void FOutExtraMark(uint ip, uint reason);
   void RemoveFOutExtraMark(uint ip, uint reason);
   void SolvedNormalMark(uint row, uint col);
   void SolvedExtraMark(uint row, uint col);
private:
   ucell hitsCount[HCP_SIZE][CTRL_SIZE];
   ucell countExtraMarks;
   ucell& CellByIPR(uint ip, uint reason);
};
extern Progress *mainProgress;

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
   uint ControlsRange(twContext* lay, const uint* par);
   uint KeyCardsRange(twContext* lay, const uint* par);
   // -
   uint LineControlsRange(twContext* lay, const uint* par);
   uint LineAcesRange(twContext* lay, const uint* par);
   uint LineKeyCardsSpade(twContext* lay, const uint* par);
   uint LinePointsRange(twContext* lay, const uint *par);
   // -
   uint PointsSuitLimit(twContext* lay, const uint* par);
   uint PointsSuitAtLeast(twContext* lay, const uint* par);
   uint PointsSuitLessSuit(twContext* lay, const uint* par);
   uint PointsSuitLEqSuit(twContext* lay, const uint* par);
   // -
   uint SpadesNatural(twContext* lay, const uint* par);
   uint HeartsNatural(twContext* lay, const uint* par);
   uint DiamondsNatural(twContext* lay, const uint* par);
   uint NoMajorFit(twContext* lay, const uint *par);
   // -
   uint PenaltyDoubleSuit(twContext* lay, const uint* par);
   uint PenaltyDoubleDiamonds(twContext* lay, const uint* par);
   // -
   uint SpadesLen(twContext* lay, const uint *par);
   uint HeartsLen(twContext* lay, const uint *par);
   uint DiamondsLen(twContext* lay, const uint *par);
   uint ClubsLen(twContext* lay, const uint *par);
   // -
   uint NoOvcOn1LevOpen(twContext* lay, const uint *par);
   uint NoOvercall(twContext* lay, const uint *par);
   uint NoPrecision2C(twContext* lay, const uint *par);
   uint No7Plus(twContext* lay, const uint *par);
   uint No2SuiterAntiSpade(twContext* lay, const uint *par);
   uint No2SuitsAntiHeart(twContext* lay, const uint *par);
   uint No2SuitsMinors(twContext* lay, const uint* par);
   uint TakeoutOfClubs(twContext* lay, const uint* par);
   // -- branching
   uint AnyInListBelow(twContext* lay, const uint *par);
   uint ExcludeCombination(twContext* lay, const uint *par);
   uint EndList(twContext* lay, const uint *par);
private:
   Progress *progress;
   struct Semantics* sem;
   uint LineKeyCardsRange(twContext* lay, const uint* par, u64 kc_mask);
   bool KeyCardsRange(u64 jo, u64 kc_mask, uint from, uint to);
   void ImprintWithinList(uint ip, uint reason, uint last);

   struct semExec {
      uint ip;     // instruction pointer
      uint depth;  // how much are we within nested condition
      void Reset() { ip = 0; depth = 0; }
   } exec;
};

#include "WaSemMicro.h"
