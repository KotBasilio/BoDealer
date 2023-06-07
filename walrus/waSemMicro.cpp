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

uint WaFilter::MicPoints(twContext* lay, const uint* par)
{
   return uint();
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

