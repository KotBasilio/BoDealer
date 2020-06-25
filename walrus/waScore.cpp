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
   // plainScore is good for any goal
   plainScore = 13 - fut.score[0];

   // the rest is for opening lead @_@ a better abstraction needed
   #ifdef SEEK_OPENING_LEAD
   for (int i = 0; i < fut.cards; i++) {
      if (fut.suit[i] == SOL_SPADES && fut.rank[i] == K9) {
         lead.S = 13 - fut.score[i];
         continue;
      }
      if (fut.suit[i] == SOL_HEARTS && fut.rank[i] == K9) {
         lead.H = 13 - fut.score[i]; 
         continue;
      }
      if (fut.suit[i] == SOL_DIAMONDS && fut.rank[i] == K6) {
         lead.D = 13 - fut.score[i]; 
         continue;
      }
      if (fut.suit[i] == SOL_CLUBS && fut.rank[i] == KJ) {
         lead.Ñ = 13 - fut.score[i];
         continue;
      }
   }
   #endif // SEEK_OPENING_LEAD
}

void Walrus::Score_Cumul4M(DdsTricks &tr)
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
   int partdelta = ideal > 6 ? 50 + (ideal - 6) * 30 : -100;
   cumulScore.partscore += partdelta;

   // "ideal"
   if (ideal < 10) {
      cumulScore.ideal += partdelta;
   } else {
      cumulScore.ideal += 620 + (ideal-10)*30;
   }
}

void Walrus::Score_Cumul3NT(DdsTricks &tr)
{
   // "always game"
   uint ideal = tr.plainScore;
   switch (ideal) {
      case 2:  cumulScore.bidGame -= 700; break;
      case 3:  cumulScore.bidGame -= 600; break;
      case 4:  cumulScore.bidGame -= 500; break;
      case 5:  cumulScore.bidGame -= 400; break;
      case 6:  cumulScore.bidGame -= 300; break;
      case 7:  cumulScore.bidGame -= 200; break;
      case 8:  cumulScore.bidGame -= 100; break;
      case 9:  cumulScore.bidGame += 600; break;
      case 10: cumulScore.bidGame += 630; break;
      case 11: cumulScore.bidGame += 660; break;
      case 12: cumulScore.bidGame += 690; break;
      case 13: cumulScore.bidGame += 720; break;
   }

   // "always partscore"
   int partdelta = ideal > 6 ? 90 + (ideal - 7) * 30 : -100;
   cumulScore.partscore += partdelta;

   // "ideal"
   if (ideal < 9) {
      cumulScore.ideal += partdelta;
   } else {
      cumulScore.ideal += 600 + (ideal - 9) * 30;
   }
}

// OUT: camp
void Walrus::Score_4Major(DdsTricks &tr)
{
   // hits
   uint row = 0, camp = 0;
   if (tr.plainScore > 9) {
      row = 1;
      camp = 1 + tr.plainScore - 9;
   } else {
      camp = 1 + 10 - tr.plainScore;
   }
   hitsCount[row][camp]++;

   // cumul
   Score_Cumul4M(tr);
}

void Walrus::Score_3NT(DdsTricks &tr)
{
   // hits
   uint row = 0, camp = 0;
   if (tr.plainScore > 8) {
      row = 1;
      camp = 1 + tr.plainScore - 8;
   } else {
      camp = 1 + 9 - tr.plainScore;
   }
   hitsCount[row][camp]++;

   // cumul
   Score_Cumul3NT(tr);
}

void Walrus::Score_OpLead3NT(DdsTricks &tr)
{
#ifdef SEEK_OPENING_LEAD
   // hits
   uint row = 0, camp = 0;
   if (tr.plainScore > 8) {
      row = 1;
      camp = tr.plainScore - 9;
      cumulScore.ideal += 600 + (tr.plainScore - 9) * 30;
   } else {
      camp = 9 - tr.plainScore - 1;
      cumulScore.ideal -= 100 * (9 - tr.plainScore);
   }
   hitsCount[row][camp]++;

   // cumulative for each lead
   cumulScore.OpLead3NT(cumulScore.leadS, tr.lead.S);
   cumulScore.OpLead3NT(cumulScore.leadH, tr.lead.H);
   cumulScore.OpLead3NT(cumulScore.leadD, tr.lead.D);
   cumulScore.OpLead3NT(cumulScore.leadC, tr.lead.Ñ);
#endif // SEEK_OPENING_LEAD
}

void Walrus::CumulativeScore::OpLead3NT(s64 &sum, uint tricks)
{
   if (tricks > 8) {
      sum += 600 + (tricks - 9) * 30;
   } else {
      sum -= 100 * (9 - tricks);
   }
}


