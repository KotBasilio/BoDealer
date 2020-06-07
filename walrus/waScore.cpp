/************************************************************
 * Walrus project                                        2020
 * AI parts
 *
 ************************************************************/
#include <string.h>
#include "walrus.h"
#include "..\dds-develop\include\dll.h"
#include "..\dds-develop\examples\hands.h"

void DdsTricks::Init(futureTricks &fut)
{
   plainScore = 13 - fut.score[0];
}

void Walrus::Red_UpdateCumulScores(DdsTricks &tr)
{
   // "always game"
   uint ideal = tr.plainScore;
   switch (ideal) {
      case 2:  cumulScore.bidGame -= 800; break;
      case 3:  cumulScore.bidGame -= 700; break;
      case 4:  cumulScore.bidGame -= 600; break;
      case 5:  cumulScore.bidGame -= 500; break;
      case 6:  cumulScore.bidGame -= 400; break;
      case 7:  cumulScore.bidGame -= 300; break;
      case 8:  cumulScore.bidGame -= 200; break;
      case 9:  cumulScore.bidGame -= 100; break;
      case 10: cumulScore.bidGame += 620; break;
      case 11: cumulScore.bidGame += 650; break;
      case 12: cumulScore.bidGame += 680; break;
      case 13: cumulScore.bidGame += 710; break;
   }

   // "always partscore"
   int partdelta = ideal > 6 ? 50 + (ideal - 6) * 30 : -50;
   cumulScore.partscore += partdelta;

   // "ideal"
   if (ideal < 10) {
      cumulScore.ideal += partdelta;
   } else {
      cumulScore.ideal += 620 + (ideal-10)*30;
   }

}

void Walrus::Red_UpateHitsByTricks(DdsTricks &tr)
{
   uint row = 0;
   uint camp = Red_Reclassify(tr.plainScore, row);
   hitsCount[row][camp]++;
}



