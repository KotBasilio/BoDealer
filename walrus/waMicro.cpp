#include "walrus.h"

#define MIC_PASSED 0
#define MIC_BLOCK (seat + 1)

MicroFilter::MicroFilter(MicroFunc f, const char *_name, uint p0, uint p1, uint p2, uint p3, uint p4)
   : func(f)
{
   params[0] = p0;
   params[1] = p1;
   params[2] = p2;
   params[3] = p3;
   params[4] = p4;
   strcpy_s(name, _name);
}

uint WaFilter::OKNum(twContext* lay, const uint* par)
{
   static uint co = 0;
   return co++ < par[0];
}

uint WaFilter::ExactShape(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_LEN;

   // match 3 lengths -- no need to check 4th
   if (len.s == par[1] &&
       len.h == par[2] &&
       len.d == par[3]) {
      return MIC_PASSED;
   }

   // fail
   return MIC_BLOCK;
}

inline bool AddCloseDist(uint from, uint to, uint &dist)
{
   if (from < to - 1 || to + 1 < from) {
      return false;
   }

   if (from == to) {
      return true;
   }

   dist++;
   return true;
}


uint WaFilter::ModelShape(twContext* lay, const uint* par)
{
   #define PASS_DIST(SUIT, IDX) if (!AddCloseDist(len.SUIT, par[IDX], dist)) { return MIC_BLOCK; }
   ACCESS_MICPAR_LEN;
   uint dist = 0;

   // match each length
   PASS_DIST(s, 1);
   PASS_DIST(h, 2);
   PASS_DIST(d, 3);
   PASS_DIST(c, 4);
   if (dist < 3) {
      return MIC_PASSED;
   }

   // fail
   return MIC_BLOCK;
}

uint WaFilter::SpadesLen(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_LEN;

   // require in range
   if (par[1] <= len.s && len.s <= par[2]) {
      return MIC_PASSED;
   }
   return MIC_BLOCK;
}

uint WaFilter::HeartsLen(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_LEN;

   // require in range
   if (par[1] <= len.h && len.h <= par[2]) {
      return MIC_PASSED;
   }
   return MIC_BLOCK;
}

uint WaFilter::DiamondsLen(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_LEN;

   // require in range
   if (par[1] <= len.d && len.d <= par[2]) {
      return MIC_PASSED;
   }
   return MIC_BLOCK;
}

uint WaFilter::ClubsLen(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_LEN;

   // require in range
   if (par[1] <= len.c && len.c <= par[2]) {
      return MIC_PASSED;
   }
   return MIC_BLOCK;
}

uint WaFilter::PointsRange(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_HCP;

   // require in range
   if (par[1] <= hcp.total && hcp.total <= par[2]) {
      return MIC_PASSED;
   }
   return MIC_BLOCK;
}

uint WaFilter::ControlsRange(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_CTRL;

   // require in range
   if (par[1] <= ctrl.total && ctrl.total <= par[2]) {
      return MIC_PASSED;
   }
   return MIC_BLOCK;
}

uint WaFilter::LineControlsRange(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_CTRL;
   auto seatPart = par[1];
   const auto& ctrlPart(lay[seatPart].ctrl);

   // require in range
   auto sum = ctrl.total + ctrlPart.total;
   if (par[2] <= sum && sum <= par[3]) {
      return MIC_PASSED;
   }

   return MIC_BLOCK;
}

static u64 sKingBit[] =
{
   0x4000000000000000LL, // SOL_SPADES
   0x0000400000000000LL, // SOL_HEARTS  
   0x0000000040000000LL, // SOL_DIAMONDS
   0x0000000000004000LL, // SOL_CLUBS
   0LL                   // SOL_NOTRUMP
};

uint WaFilter::KeyCardsRange(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_JO;

   // which king do we need
   auto sol_suit = par[1];
   u64 kc_mask = ANY_ACE | sKingBit[sol_suit];

   // relay
   return KeyCardsRange(jo, kc_mask, par[2], par[3]) ? MIC_PASSED : MIC_BLOCK;
}


uint WaFilter::LineKeyCardsSpade(twContext* lay, const uint* par)
{// obsolete and early version. see more generic
   auto seat     = par[0];
   auto seatPart = par[1];

   // which king do we need
   u64 kc_mask = ANY_ACE | sKingBit[SOL_SPADES];

   // combine the line
   u64 line = lay[seat    ].hand.card.jo +
              lay[seatPart].hand.card.jo;

   // get keycards
   u64 keycards = line & kc_mask;

   // sum bits
   auto x =  (keycards & 0xF000000000000000LL) >> (16*3 + 12 + 2);
   auto y = ((keycards & 0x0000F00000000000LL) >> (16*2 + 12 + 3)) +
            ((keycards & 0x00000000F0000000LL) >> (16   + 12 + 3)) +
            ((keycards & 0x000000000000F000LL) >> (       12 + 3));

   // require in range
   auto sum = (x & 0x1) + ((x & 0x2) >> 1) + y;
   if (par[2] <= sum && sum <= par[3]) {
      return MIC_PASSED;
   }

   return MIC_BLOCK;
}

uint WaFilter::LinePointsRange(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_HCP;
   auto seatPart = par[1];
   const auto& hcpPart(lay[seatPart].hcp);

   // require in range
   auto sum = hcp.total + hcpPart.total;
   if (par[2] <= sum && sum <= par[3]) {
      return MIC_PASSED;
   }

   return MIC_BLOCK;
}

uint WaFilter::PointsLimit(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_HCP;

   // only upper limit
   if (hcp.total <= par[1]) {
      return MIC_PASSED;
   }
   return MIC_BLOCK;
}

uint WaFilter::PointsSuitLimit(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_HCP;

   // only upper limit
   if (hcp.arr[par[1]] <= par[2]) {
      return MIC_PASSED;
   }
   return MIC_BLOCK;
}

uint WaFilter::PointsAtLeast(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_HCP;

   // require enough
   if (par[1] <= hcp.total) {
      return MIC_PASSED;
   }
   return MIC_BLOCK;
}

uint WaFilter::PointsSuitAtLeast(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_HCP;

   // only lower limit
   if (hcp.arr[par[1]] >= par[2]) {
      return MIC_PASSED;
   }
   return MIC_BLOCK;
}

uint WaFilter::SpadesNatural(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_LEN;

   // h is same or shorter, minor can be 1 card longer
   if (len.s >= len.h &&
       len.s >= len.d + 1 &&
       len.s >= len.c + 1) {
      return MIC_PASSED;
   }

   return MIC_BLOCK;
}

uint WaFilter::HeartsNatural(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_LEN;

   // s is shorter, minor can be 1 card longer
   if (len.h > len.s &&
      len.h >= len.d + 1 &&
      len.h >= len.c + 1) {
      return MIC_PASSED;
   }

   return MIC_BLOCK;
}



uint WaFilter::DiamondsNatural(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_LEN;

   // c is same or shorter, majors are shorter
   if (len.d > len.h &&
       len.d > len.s &&
       len.d >= len.c) {
      return MIC_PASSED;
   }

   return MIC_BLOCK;
}

uint WaFilter::NoMajorFit(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_LEN;
   auto seatPart = par[1];
   const auto& lenPart(lay[seatPart].len);

   // require in range
   auto sumS = len.s + lenPart.s;
   if (sumS < 8) {
      auto sumH = len.h + lenPart.h;
      if (sumH < 8) {
         return MIC_PASSED;
      }
   }

   return MIC_BLOCK;
}

// "no overcall on 1st level opening"
uint WaFilter::NoOvcOn1LevOpen(twContext* lay, const uint* par)
{
   ACCESS_MICPARS_HL;

   // many points => fail
   if (hcp.total >= 12) {
      return MIC_BLOCK;
   }

   // no 6 with 10+pts
   if (hcp.total >= 10) {
      if (len.s > 5 || len.h > 5 || len.d > 5 || len.c > 5) {
         return MIC_BLOCK;
      }
   } else if (hcp.total >= 7) {// no half-preempts (6M with 7+pts)
      if (len.s > 5 || len.h > 5) {
         return MIC_BLOCK;
      }
   }

   // relay
   return No7Plus(lay, par);
}


uint WaFilter::NoOvercall(twContext* lay, const uint* par)
{
   ACCESS_MICPARS_HL;

   // many points => fail
   if (hcp.total >= 12) {
      return MIC_BLOCK;
   }

   // no 6 with 10+pts
   if (hcp.total >= 10) {
      if (len.s > 5 || len.h > 5 || len.d > 5 || len.c > 5) {
         return MIC_BLOCK;
      }
   }

   // relay
   return No7Plus(lay, par);
}

uint WaFilter::NoPrecision2C(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_LEN;

   if (len.c > 5) {
      return MIC_BLOCK;
   }
   if (len.c == 5 && len.h > 3 || len.s > 3) {
      return MIC_BLOCK;
   }

   // ok
   return MIC_PASSED;
}

uint WaFilter::No7Plus(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_LEN;

   // no 7+suit
   if (len.s > 6 || len.h > 6 || len.d > 6 || len.c > 6) {
      return MIC_BLOCK;
   }

   // ok
   return MIC_PASSED;
}

uint WaFilter::No2SuiterAntiSpade(twContext* lay, const uint* par)
{
   ACCESS_MICPARS_HL;

   // exclude Michaels
   if (hcp.total > 6) {
      if (len.h > 4) {
         if (len.c > 4 || len.d > 4) {
            return MIC_BLOCK;
         }
      }
   }

   // relay
   return No2SuitsMinors(lay, par);
}

uint WaFilter::No2SuitsAntiHeart(twContext* lay, const uint* par)
{
   ACCESS_MICPARS_HL;

   // exclude Michaels
   if (hcp.total > 6) {
      if (len.s > 4) {
         if (len.c > 4 || len.d > 4) {
            return MIC_BLOCK;
         }
      }
   }

   // relay
   return No2SuitsMinors(lay, par);
}

uint WaFilter::No2SuitsMinors(twContext* lay, const uint* par)
{
   ACCESS_MICPARS_HL;

   if (hcp.total > 6) {
      if (len.c > 4 && len.d > 4) {
         return MIC_BLOCK;
      }
   }

   // ok
   return MIC_PASSED;
}

uint WaFilter::TakeoutOfClubs(twContext* lay, const uint* par)
{
   ACCESS_MICPARS_HL;

   if (hcp.total < 11) {
      return MIC_BLOCK;
   }

   if (len.s > 4 || len.h > 4 || len.d > 4) {
      return MIC_BLOCK;
   }

   if (len.s < 3|| len.h < 3 || len.d < 3) {
      return MIC_BLOCK;
   }

   if (len.c > 3) {
      return MIC_BLOCK;
   }

   return MIC_PASSED;
}

uint WaFilter::LineAcesRange(twContext* lay, const uint* par)
{
   // aces only
   u64 kc_mask = ANY_ACE;
   return LineKeyCardsRange(lay, par, kc_mask);
}

uint WaFilter::LineKeyCardsRange(twContext* lay, const uint* par, u64 kc_mask)
{
   auto seat     = par[0];
   auto seatPart = par[1];

   // combine the line keycards
   u64 line = lay[seat    ].hand.card.jo +
              lay[seatPart].hand.card.jo;

   // relay
   return KeyCardsRange(line, kc_mask, par[2], par[3]) ? MIC_PASSED : MIC_BLOCK;
}

bool WaFilter::KeyCardsRange(u64 jo, u64 kc_mask, uint from, uint to)
{
   // combine the keycards
   u64 keycards = (jo & kc_mask) >> (12 + 2);

   // sum bits
   auto keysum =  (keycards & 0x0001000100010001LL) + 
                 ((keycards & 0x0002000200020002LL) >> 1);
   auto sum = ((keysum & 0x000F000000000000LL) >> (16*3)) +
              ((keysum & 0x0000000F00000000LL) >> (16*2)) +
              ((keysum & 0x00000000000F0000LL) >> (16  )) +
              ((keysum & 0x000000000000000FLL));

   // require in range
   return (from <= sum && sum <= to);
}


bool WaFilter::ScanOut(twContext* lay)
{
   // run all micro-filters on this 4-hands layout
   // and mark reason why we skip this board
   exec.Reset();
   for (auto &ip = exec.ip; ip < sem->vecFilters.size(); ip++) {
      const auto& mic = sem->vecFilters[ip];
      if (auto reason = (this->*mic.func)(lay, mic.params)) {
         progress->FilteredOutMark(ip, reason);
         return true;
      } 
   }

   // not scanned out
   return false;
}

void WaFilter::ImprintWithinList(uint ip, uint reason, uint last)
{
   progress->FOutExtraMark(ip, reason);
   if (ip >= last) {
      progress->RemoveFOutExtraMark(ip, reason);
   }
}

uint WaFilter::AnyInListBelow(twContext* lay, const uint* par)
{
   // init
   uint reason = MIC_PASSED;
   exec.depth++;
   auto &ip = exec.ip;
   auto last = par[1] - 1;

   // for all filters in this block
   for (ip++; ip <= last; ip++) {
      // try current filter
      const auto& mic = sem->vecFilters[ip];
      reason = (this->*mic.func)(lay, mic.params);

      // when the filter rejects this layout
      if (reason) {
         // we just imprint that rejection in the main table
         // continue checks since some other filter can accept this layout
         ImprintWithinList(ip, reason, last);
         continue;
      } 
      
      // current filter says ok, so entire block is accepted
      exec.depth--;
      ip = last;
   }

   // pass or fail by the last checked filter
   return reason;
}

uint WaFilter::ExcludeCombination(twContext* lay, const uint* par)
{
   // same as AnyInListBelow(), but all filters must fail
   uint reason = MIC_PASSED;
   auto backup = exec.ip;
   auto &ip = exec.ip;
   auto last = par[1] - 1;

   // for all filters in this block
   for (ip++; ip <= last; ip++) {
      // try current filter
      const auto& mic = sem->vecFilters[ip];
      reason = (this->*mic.func)(lay, mic.params);

      // rejected => entire block is accepted
      if (reason) {
         ImprintWithinList(ip, reason, last + 2);
         ip = last + 1;
         return MIC_PASSED;
      }

      // continue checks since some other filter can reject this layout
   }

   // all filters accepted the layout => we exclude it
   ip = backup;
   auto seat = par[0];
   return MIC_BLOCK;
}

uint WaFilter::EndList(twContext* lay, const uint* par)
{
   // should not really execute this filter
   DEBUG_UNEXPECTED;
   return MIC_PASSED;
}

uint WaFilter::PointsSuitLessSuit(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_HCP;

   if (hcp.arr[par[1]] < hcp.arr[par[2]]) {
      return MIC_PASSED;
   }

   return MIC_BLOCK;
}

uint WaFilter::PointsSuitLEqSuit(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_HCP;

   if (hcp.arr[par[1]] <= hcp.arr[par[2]]) {
      return MIC_PASSED;
   }

   return MIC_BLOCK;
}

uint WaFilter::PenaltyDoubleSuit(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_LEN;
   DEBUG_UNEXPECTED;
   return MIC_BLOCK;
}

uint WaFilter::PenaltyDoubleDiamonds(twContext* lay, const uint* par)
{
   ACCESS_MICPARS_HL;

   if (len.d <= 2) {
      return MIC_BLOCK;
   }

   if (len.d == 3) {
      if (hcp.d < 8) {
         return MIC_BLOCK;
      }
   }

   if (len.d == 4) {
      if (hcp.d < 5) {
         return MIC_BLOCK;
      }
   }

   return MIC_PASSED;
}

