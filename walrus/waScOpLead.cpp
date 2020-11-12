/************************************************************
 * Walrus project                                        2020
 * AI parts
 *
 ************************************************************/
#include <string.h>
#include "walrus.h"
#include "..\dds-develop\include\dll.h"
#include "..\dds-develop\examples\hands.h"

void Walrus::Score_OpLead3NT(DdsTricks &tr)
{
#ifdef SEEK_OPENING_LEAD
   // hits
   HitByScore(tr, 9);

   // cumulative for each lead
   cumulScore.OpLead3NT(cumulScore.ideal, tr.plainScore);
   cumulScore.OpLead3NT(cumulScore.leadS, tr.lead.S);
   cumulScore.OpLead3NT(cumulScore.leadH, tr.lead.H);
   cumulScore.OpLead3NT(cumulScore.leadD, tr.lead.D);
   cumulScore.OpLead3NT(cumulScore.leadC, tr.lead.Ñ);
#endif // SEEK_OPENING_LEAD
}

void Walrus::Score_OpLead3Major(DdsTricks &tr)
{
#ifdef SEEK_OPENING_LEAD
   // hits
   HitByScore(tr, 9);

   // cumulative for each lead
   cumulScore.OpLead3Major(cumulScore.ideal, tr.plainScore);
   cumulScore.OpLead3Major(cumulScore.leadS, tr.lead.S);
   cumulScore.OpLead3Major(cumulScore.leadH, tr.lead.H);
   cumulScore.OpLead3Major(cumulScore.leadD, tr.lead.D);
   cumulScore.OpLead3Major(cumulScore.leadC, tr.lead.Ñ);
#endif // SEEK_OPENING_LEAD
}

void Walrus::CumulativeScore::OpLead3Major(s64 &sum, uint tricks)
{
   if (tricks > 8) {
      sum += 140 + (tricks - 9) * 30;
   } else {
      sum -= 50 * (9 - tricks);
   }
}

void Walrus::CumulativeScore::OpLead3NT(s64 &sum, uint tricks)
{
   if (tricks > 8) {
      sum += 600 + (tricks - 9) * 30;
   } else {
      sum -= 100 * (9 - tricks);
   }
}

void Walrus::CumulativeScore::OpLead5minor(s64 &sum, uint tricks)
{
   if (tricks > 10) {
      sum += 600 + (tricks - 11) * 20;
   } else {
      sum -= 100 * (11 - tricks);
   }
}

void Walrus::Score_OpLead5D(DdsTricks &tr)
{
#ifdef SEEK_OPENING_LEAD
   // hits
   HitByScore(tr, 11);

   // cumulative for ideal and for each lead
   cumulScore.OpLead5minor(cumulScore.ideal, tr.plainScore);
   cumulScore.OpLead5minor(cumulScore.leadS, tr.lead.S);
   cumulScore.OpLead5minor(cumulScore.leadH, tr.lead.H);
   cumulScore.OpLead5minor(cumulScore.leadD, tr.lead.D);
   cumulScore.OpLead5minor(cumulScore.leadC, tr.lead.Ñ);
#endif // SEEK_OPENING_LEAD
}

void Walrus::Score_OpLead5DX(DdsTricks &tr)
{
#ifdef SEEK_OPENING_LEAD
   // hits
   HitByScore(tr, 11);

   // cumulative for ideal and for each lead
   cumulScore.OpLead5mX(cumulScore.ideal, tr.plainScore);
   cumulScore.OpLead5mX(cumulScore.leadS, tr.lead.S);
   cumulScore.OpLead5mX(cumulScore.leadH, tr.lead.H);
   cumulScore.OpLead5mX(cumulScore.leadD, tr.lead.D);
   cumulScore.OpLead5mX(cumulScore.leadC, tr.lead.Ñ);
#endif // SEEK_OPENING_LEAD
}

void Walrus::CumulativeScore::OpLead5mX(s64 &sum, uint tricks)
{
   switch (tricks) {
      case 11: sum += 550; return;
      case 12: sum += 650; return;
      case 13: sum += 750; return;

      case 10: sum -=  100; return;
      case  9: sum -=  300; return;
      case  8: sum -=  500; return;
      case  7: sum -=  800; return;
      case  6: sum -= 1100; return;
      case  5: sum -= 1400; return;
      case  4: sum -= 1700; return;
      case  3: sum -= 2000; return;
      case  2: sum -= 2300; return;
      case  1: sum -= 2600; return;
      case  0: sum -= 2900; return;
   }
}

void Walrus::CumulativeScore::Opp_3MajX(s64 &sum, uint tricks)
{
  switch (tricks) {
    case  9: sum -= 530; return;
    case 10: sum -= 630; return;
    case 11: sum -= 730; return;
    case 12: sum -= 830; return;
    case 13: sum -= 930; return;

    case  8: sum += 100 ; return;
    case  7: sum += 300 ; return;
    case  6: sum += 500 ; return;
    case  5: sum += 800 ; return;
    case  4: sum += 1100; return;
    case  3: sum += 1400; return;
    case  2: sum += 1700; return;
    case  1: sum += 2000; return;
    case  0: sum += 2300; return;
  }
}

void Walrus::CumulativeScore::Opp_3Major(s64 &sum, uint tricks)
{
  switch (tricks) {
    case  9: sum -= 140; return;
    case 10: sum -= 170; return;
    case 11: sum -= 200; return;
    case 12: sum -= 230; return;
    case 13: sum -= 260; return;

    case  8: sum += 50; return;
    case  7: sum += 100; return;
    case  6: sum += 150; return;
    case  5: sum += 200; return;
    case  4: sum += 250; return;
    case  3: sum += 300; return;
    case  2: sum += 350; return;
    case  1: sum += 400; return;
    case  0: sum += 450; return;
  }
}

