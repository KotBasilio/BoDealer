#include "walrus.h"

#define MIC_PASSED 0
#define MIC_BLOCK (seat + 1)

MicroFilter::MicroFilter(MicroFunc f, uint p0, uint p1, uint p2, uint p3, uint p4)
   : func(f)
{
   params[0] = p0;
   params[1] = p1;
   params[2] = p2;
   params[3] = p3;
   params[4] = p4;
}

uint WaFilter::OKNum(twContext* lay, const uint* par)
{
   static uint co = 0;
   return co++ < par[0];
}

uint WaFilter::ExactShape(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_LEN;

   // match each length
   if (len.s == par[1] &&
       len.h == par[2] &&
       len.d == par[3] &&
       len.c == par[4]) {
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

uint WaFilter::DiamLen(twContext* lay, const uint* par)
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

uint WaFilter::LineKeyCardsSpade(twContext* lay, const uint* par)
{
   auto seat     = par[0];
   auto seatPart = par[1];

   // which king do we need
   u64 king    = 0x4000000000000000LL;
   u64 kc_mask = 0x8000800080008000LL | king;

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

uint WaFilter::PointsAtLeast(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_HCP;

   // require enough
   if (par[1] <= hcp.total) {
      return MIC_PASSED;
   }
   return MIC_BLOCK;
}

uint WaFilter::ClubPointsLimit(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_HCP;

   // only upper limit
   if (hcp.c <= par[1]) {
      return MIC_PASSED;
   }
   return MIC_BLOCK;
}

uint WaFilter::HeartPointsLimit(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_HCP;

   // only upper limit
   if (hcp.h <= par[1]) {
      return MIC_PASSED;
   }
   return MIC_BLOCK;
}

uint WaFilter::SpadePointsLimit(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_HCP;

   // only upper limit
   if (hcp.s <= par[1]) {
      return MIC_PASSED;
   }
   return MIC_BLOCK;
}

uint WaFilter::DiamPointsLimit(twContext* lay, const uint* par)
{
   ACCESS_MICPAR_HCP;

   // only upper limit
   if (hcp.d <= par[1]) {
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

uint WaFilter::LineAcesRange(twContext* lay, const uint* par)
{
   // aces only
   u64 kc_mask = 0x8000800080008000LL;
   return LineKeyCardsRange(lay, par, kc_mask);
}

uint WaFilter::LineKeyCardsRange(twContext* lay, const uint* par, u64 kc_mask)
{
   auto seat     = par[0];
   auto seatPart = par[1];

   // combine the line keycards
   u64 line = lay[seat    ].hand.card.jo +
              lay[seatPart].hand.card.jo;
   u64 keycards = (line & kc_mask) >> (12 + 2);

   // sum bits
   auto keysum =  (keycards & 0x0001000100010001LL) + 
                 ((keycards & 0x0002000200020002LL) >> 1);
   auto sum = ((keysum & 0x000F000000000000LL) >> (16*3)) +
              ((keysum & 0x0000000F00000000LL) >> (16*2)) +
              ((keysum & 0x00000000000F0000LL) >> (16  )) +
              ((keysum & 0x000000000000000FLL));

   // require in range
   if (par[2] <= sum && sum <= par[3]) {
      return MIC_PASSED;
   }

   return MIC_BLOCK;
}

