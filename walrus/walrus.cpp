#include "walrus.h"

#include <chrono>

bool Walrus::AfterMath()
{
   // no op => back off
   if (sem.onAfterMath == (&Walrus::NOP)) {
      return false;
   }

   // relay
   (this->*sem.onAfterMath)();
   return true;
}

Walrus::CumulativeScore::CumulativeScore()
   : ideal(0L)
   , bidGame(0L)
   , partscore(0L)
{
}
