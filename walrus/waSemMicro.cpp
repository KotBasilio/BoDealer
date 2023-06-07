#include "walrus.h"

const uint MIC_PASSED = 0;

MicroFilter::MicroFilter(MicroFunc f, uint p0, uint p1, uint p2, uint p3, uint p4)
   : func(f)
{
   params[0] = p0;
   params[1] = p1;
   params[2] = p2;
   params[3] = p3;
   params[4] = p4;
}

uint WaFilter::OK100(twContext* lay, const uint* par)
{
   static uint co = 0;
   return co++ < 100;
}

uint WaFilter::OKNum(twContext* lay, const uint* par)
{
   static uint co = 0;
   return co++ < par[0];
}

uint WaFilter::ExactShape(twContext* lay, const uint* par)
{
   auto seat = par[0];
   const auto &len(lay[seat].len);

   // match each length
   if (len.s == par[1] &&
       len.h == par[2] &&
       len.d == par[3] &&
       len.c == par[4]) {
      return MIC_PASSED;
   }

   // fail
   return seat + 1;
}

uint WaFilter::SpadesLen(twContext* lay, const uint* par)
{
   auto seat = par[0];
   const auto &len(lay[seat].len);

   // require in range
   if (par[1] <= len.s && len.s <= par[2]) {
      return MIC_PASSED;
   }
   return seat + 1;
}

uint WaFilter::HeartsLen(twContext* lay, const uint* par)
{
   auto seat = par[0];
   const auto &len(lay[seat].len);

   // require in range
   if (par[1] <= len.h && len.h <= par[2]) {
      return MIC_PASSED;
   }
   return seat + 1;
}

uint WaFilter::DiamLen(twContext* lay, const uint* par)
{
   auto seat = par[0];
   const auto &len(lay[seat].len);

   // require in range
   if (par[1] <= len.d && len.d <= par[2]) {
      return MIC_PASSED;
   }
   return seat + 1;
}

uint WaFilter::ClubsLen(twContext* lay, const uint* par)
{
   auto seat = par[0];
   const auto &len(lay[seat].len);

   // require in range
   if (par[1] <= len.c && len.c <= par[2]) {
      return MIC_PASSED;
   }
   return seat + 1;
}

uint WaFilter::PointsRange(twContext* lay, const uint* par)
{
   auto seat = par[0];
   const auto &hcp(lay[seat].hcp);

   // require in range
   if (par[1] <= hcp.total && hcp.total <= par[2]) {
      return MIC_PASSED;
   }
   return seat + 1;
}

uint WaFilter::PointsLimit(twContext* lay, const uint* par)
{
   auto seat = par[0];
   const auto &hcp(lay[seat].hcp);

   // only upper limit
   if (hcp.total <= par[1]) {
      return MIC_PASSED;
   }
   return seat + 1;
}

uint WaFilter::PointsAtLeast(twContext* lay, const uint* par)
{
   auto seat = par[0];
   const auto &hcp(lay[seat].hcp);

   // require enough
   if (par[1] <= hcp.total) {
      return MIC_PASSED;
   }
   return seat + 1;
}

uint WaFilter::SpadesNatural(twContext* lay, const uint* par)
{
   auto seat = par[0];
   const auto &len(lay[seat].len);

   // h is same or shorter, minor can be 1 card longer
   if (len.s >= len.h &&
       len.s >= len.d + 1 &&
       len.s >= len.c + 1) {
      return MIC_PASSED;
   }

   return seat + 1;
}

uint WaFilter::NoOvercall(twContext* lay, const uint* par)
{
   auto seat = par[0];
   const auto &hcp(lay[seat].hcp);
   const auto &len(lay[seat].len);

   // many points => fail
   if (hcp.total >= 12) {
      return seat + 1;
   }

   // no 7+suit
   if (len.s > 6 || len.h > 6 || len.d > 6 || len.c > 6) {
      return seat + 1;
   }

   // no 6 with 10+pts
   if (hcp.total >= 10) {
      if (len.s > 5 || len.h > 5 || len.d > 5 || len.c > 5) {
         return seat + 1;
      }
   }

   // todo: exclude some michaels

   // ok
   return MIC_PASSED;
}

