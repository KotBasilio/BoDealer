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

