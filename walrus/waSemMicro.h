/******************************************************************************
 * Micro-filters are a kind of byte-code programming               2023
 *
 ******************************************************************************/
#pragma once
#include <vector>

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
   uint StrongNT(twContext* lay, const uint *par);
   uint WeakNT(twContext* lay, const uint *par);
   uint WeakGroup(twContext* lay, const uint *par);
   // -
   uint LinePointsRange(twContext* lay, const uint *par);
   uint LineControlsRange(twContext* lay, const uint* par);
   uint LineAcesRange(twContext* lay, const uint* par);
   uint LineKeyCardsSuit(twContext* lay, const uint* par);
   uint LineSuitAnyCuebid(twContext* lay, const uint *par);
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
   uint PassVul(twContext* lay, const uint* par);
   uint PassNV(twContext* lay, const uint* par);
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

constexpr uint MAX_MICRO_PARAMS = 5;

typedef uint (WaFilter::* MicroFunc)(twContext* lay, const uint *par);

struct MicroFilter {
   MicroFunc func;
   uint  params[MAX_MICRO_PARAMS];
   char  name[48];
   MicroFilter() : func(nullptr) { params[0] = params[1] = params[2] = params[3] = params[4] = 0; name[0] = 0; }
   MicroFilter(MicroFunc f, const char *_name, uint p0 = 0, uint p1 = 0, uint p2 = 0, uint p3 = 0, uint p4 = 0);
};

// common reasons for a filter
constexpr uint SKIP_BY_PART = 1;
constexpr uint SKIP_BY_RESP = 2;
constexpr uint SKIP_BY_OPP = 3;
constexpr uint SKIP_BY_DIRECT = SKIP_BY_RESP;
constexpr uint SKIP_BY_SANDWICH = SKIP_BY_OPP;

// macros for adding filters
#define ADD_VOID_FILTER(NAME)                         sem.vecFilters.push_back( MicroFilter(&WaFilter::NAME, #NAME)                        )
#define ADD_0PAR_FILTER(HAND, NAME)                   sem.vecFilters.push_back( MicroFilter(&WaFilter::NAME, #NAME" "#HAND, HAND)          )
#define ADD_1PAR_FILTER(HAND, NAME, P2)               sem.vecFilters.push_back( MicroFilter(&WaFilter::NAME, #NAME" "#P2" "#HAND, HAND, P2))
#define ADD_2PAR_FILTER(HAND, NAME, P2, P3)           sem.vecFilters.push_back( MicroFilter(&WaFilter::NAME, #NAME" "#P2" "#P3" "#HAND, HAND, P2, P3)          )
#define ADD_3PAR_FILTER(HAND, NAME, P2, P3, P4)       sem.vecFilters.push_back( MicroFilter(&WaFilter::NAME, #NAME" "#P2" "#P3" "#P4" "#HAND, HAND, P2, P3, P4)      )
#define ADD_4PAR_FILTER(HAND, NAME, P2, P3, P4, P5)   sem.vecFilters.push_back( MicroFilter(&WaFilter::NAME, #NAME" "#P2" "#P3" "#P4" "#P5" "#HAND, HAND, P2, P3, P4, P5)  )

// standard macros for accessing
#define ACCESS_MICPAR_HCP            \
   auto seat = par[0];               \
   const auto &hcp(lay[seat].hcp)

#define ACCESS_MICPAR_LEN            \
   auto seat = par[0];               \
   const auto &len(lay[seat].len)

#define ACCESS_MICPAR_CTRL           \
   auto seat = par[0];               \
   const auto &ctrl(lay[seat].ctrl)

#define ACCESS_MICPAR_JO             \
   auto seat = par[0];               \
   u64 jo(lay[seat].hand.card.jo)

#define ACCESS_MICPAR_LINE                \
   auto seat = par[0];                    \
   auto seatPart = par[1];                \
   const auto& ctxOur = lay[seat];        \
   const auto& ctxPart = lay[seatPart]

#define ACCESS_MICPARS_HL            \
   auto seat = par[0];               \
   const auto &hcp(lay[seat].hcp);   \
   const auto &len(lay[seat].len)

#define ACCESS_MICPARS_ALL           \
   auto seat = par[0];               \
   const auto &hcp(lay[seat].hcp);   \
   const auto &len(lay[seat].len);   \
   const auto &ctrl(lay[seat].ctrl)

#define ACCESS_MICPAR_SUIT           \
   auto suit = par[1];
