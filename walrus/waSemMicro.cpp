#include "walrus.h"

MicroFilter::MicroFilter(MicroFunc f, uint p0, uint p1, uint p2, uint p3)
   : func(f)
{
   params[0] = p0;
   params[1] = p1;
   params[2] = p2;
   params[3] = p3;
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

