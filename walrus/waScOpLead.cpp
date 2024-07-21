/************************************************************
 * Walrus project                                        2020
 * AI parts
 *
 ************************************************************/
#include <string.h>
#include "walrus.h"
#include "../dds-develop/include/dll.h"
#include "../dds-develop/examples/hands.h"

void Walrus::Score_OpLead3NT(DdsTricks &tr)
{
#ifdef SEEK_OPENING_LEAD
   // cumulative for each lead
   cumulScore.OpLead3NT(cumulScore.ideal, tr.plainScore);
   cumulScore.OpLead3NT(cumulScore.leadS, tr.lead.S);
   cumulScore.OpLead3NT(cumulScore.leadH, tr.lead.H);
   cumulScore.OpLead3NT(cumulScore.leadD, tr.lead.D);
   cumulScore.OpLead3NT(cumulScore.leadC, tr.lead.N);
#endif // SEEK_OPENING_LEAD
}

void Walrus::Score_OpLead3NTX(DdsTricks &tr)
{
#ifdef SEEK_OPENING_LEAD
   // cumulative for each lead
   cumulScore.OpLead3NTX(cumulScore.ideal, tr.plainScore);
   cumulScore.OpLead3NTX(cumulScore.leadS, tr.lead.S);
   cumulScore.OpLead3NTX(cumulScore.leadH, tr.lead.H);
   cumulScore.OpLead3NTX(cumulScore.leadD, tr.lead.D);
   cumulScore.OpLead3NTX(cumulScore.leadC, tr.lead.N);
#endif // SEEK_OPENING_LEAD
}

void Walrus::Score_OpLead3Major(DdsTricks &tr)
{
#ifdef SEEK_OPENING_LEAD
   // cumulative for each lead
   cumulScore.OpLead3Major(cumulScore.ideal, tr.plainScore);
   cumulScore.OpLead3Major(cumulScore.leadS, tr.lead.S);
   cumulScore.OpLead3Major(cumulScore.leadH, tr.lead.H);
   cumulScore.OpLead3Major(cumulScore.leadD, tr.lead.D);
   cumulScore.OpLead3Major(cumulScore.leadC, tr.lead.N);
#endif // SEEK_OPENING_LEAD
}

void CumulativeScore::OpLead3Major(s64 &sum, uint tricks)
{
   if (tricks > 8) {
      sum += 140 + (tricks - 9) * 30;
   } else {
      sum -= 50 * (9 - tricks);
   }
}

void CumulativeScore::OpLead4Major(s64& sum, uint tricks)
{
   if (tricks > 9) {
      sum += 620 + (tricks - 10) * 30;
   } else {
      sum -= 100 * (10 - tricks);
   }
}

void CumulativeScore::OpLead3NT(s64 &sum, uint tricks)
{
   if (tricks > 8) {
      sum += 600 + (tricks - 9) * 30;
   } else {
      sum -= 100 * (9 - tricks);
   }
}

void CumulativeScore::OpLead3NTX(s64 &sum, uint tricks)
{
   switch (tricks) {
      case  9: sum += 750;  return;
      case 10: sum += 950;  return;
      case 11: sum += 1150; return;
      case 12: sum += 1350; return;
      case 13: sum += 1550; return;

      case  8: sum -= 200;  return;
      case  7: sum -= 500;  return;
      case  6: sum -= 800;  return;
      case  5: sum -= 1100; return;
      case  4: sum -= 1400; return;
      case  3: sum -= 1700; return;
      case  2: sum -= 2000; return;
      case  1: sum -= 2300; return;
      case  0: sum -= 2600; return;
   }
}

void CumulativeScore::OpLead5minor(s64 &sum, uint tricks)
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
   // cumulative for ideal and for each lead
   cumulScore.OpLead5minor(cumulScore.ideal, tr.plainScore);
   cumulScore.OpLead5minor(cumulScore.leadS, tr.lead.S);
   cumulScore.OpLead5minor(cumulScore.leadH, tr.lead.H);
   cumulScore.OpLead5minor(cumulScore.leadD, tr.lead.D);
   cumulScore.OpLead5minor(cumulScore.leadC, tr.lead.N);
#endif // SEEK_OPENING_LEAD
}

void Walrus::Score_OpLead5DX(DdsTricks &tr)
{
#ifdef SEEK_OPENING_LEAD
   // cumulative for ideal and for each lead
   cumulScore.OpLead5mX(cumulScore.ideal, tr.plainScore);
   cumulScore.OpLead5mX(cumulScore.leadS, tr.lead.S);
   cumulScore.OpLead5mX(cumulScore.leadH, tr.lead.H);
   cumulScore.OpLead5mX(cumulScore.leadD, tr.lead.D);
   cumulScore.OpLead5mX(cumulScore.leadC, tr.lead.N);
#endif // SEEK_OPENING_LEAD
}

void CumulativeScore::OpLead5mX(s64 &sum, uint tricks)
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

void CumulativeScore::Opp_3MajX(s64 &sum, uint tricks)
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

void CumulativeScore::Opp_4MajX(s64& sum, uint tricks)
{
   switch (tricks) {
      case 10: sum -= 790; return;
      case 11: sum -= 990; return;
      case 12: sum -= 1190; return;
      case 13: sum -= 1390; return;

      case  9: sum += 200; return;
      case  8: sum += 500; return;
      case  7: sum += 800; return;
      case  6: sum += 1100; return;
      case  5: sum += 1400; return;
      case  4: sum += 1700; return;
      case  3: sum += 2000; return;
      case  2: sum += 2300; return;
      case  1: sum += 2600; return;
      case  0: sum += 2900; return;
   }
}

void CumulativeScore::Opp_NV_4MajX(s64 &sum, uint tricks)
{
   switch (tricks) {
      case 10: sum -= 590; return;
      case 11: sum -= 690; return;
      case 12: sum -= 790; return;
      case 13: sum -= 890; return;

      case  9: sum += 100; return;
      case  8: sum += 300; return;
      case  7: sum += 500; return;
      case  6: sum += 800; return;
      case  5: sum += 1100; return;
      case  4: sum += 1400; return;
      case  3: sum += 1700; return;
      case  2: sum += 2000; return;
      case  1: sum += 2300; return;
      case  0: sum += 2600; return;
   }
}

void CumulativeScore::Opp_4M(s64 &sum, uint tricks)
{
   switch (tricks) {
      case 10: sum -= 620; return;
      case 11: sum -= 650; return;
      case 12: sum -= 680; return;
      case 13: sum -= 710; return;

      case  9: sum += 100; return;
      case  8: sum += 200; return;
      case  7: sum += 300; return;
      case  6: sum += 400; return;
      case  5: sum += 500; return;
      case  4: sum += 600; return;
      case  3: sum += 700; return;
      case  2: sum += 800; return;
      case  1: sum += 900; return;
      case  0: sum += 1000; return;
   }
}

void CumulativeScore::Opp_3Major(s64 &sum, uint tricks)
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

void CumulativeScore::Opp_3NT(s64 &sum, uint tricks)
{
   switch (tricks) {
      case  9: sum -= 600; return;
      case 10: sum -= 630; return;
      case 11: sum -= 660; return;
      case 12: sum -= 690; return;
      case 13: sum -= 720; return;

      case  8: sum += 100; return;
      case  7: sum += 200; return;
      case  6: sum += 300; return;
      case  5: sum += 400; return;
      case  4: sum += 500; return;
      case  3: sum += 600; return;
      case  2: sum += 700; return;
      case  1: sum += 800; return;
      case  0: sum += 900; return;
   }
}

void CumulativeScore::Opp_5minor(s64& sum, uint tricks)
{
   switch (tricks) {
      case 11: sum -=  600; return;
      case 12: sum -=  620; return;
      case 13: sum -=  640; return;
                       
      case 10: sum +=  100; return;
      case  9: sum +=  200; return;
      case  8: sum +=  300; return;
      case  7: sum +=  400; return;
      case  6: sum +=  500; return;
      case  5: sum +=  600; return;
      case  4: sum +=  700; return;
      case  3: sum +=  800; return;
      case  2: sum +=  900; return;
      case  1: sum += 1000; return;
      case  0: sum += 1100; return;
   }
}

void CumulativeScore::Opp_5minorX(s64& sum, uint tricks)
{
   switch (tricks) {
      case 11: sum -=  750; return;
      case 12: sum -=  950; return;
      case 13: sum -=  1150; return;
                       
      case 10: sum += 200;  return;
      case  9: sum += 500;  return;
      case  8: sum += 800;  return;
      case  7: sum += 1100; return;
      case  6: sum += 1400; return;
      case  5: sum += 1700; return;
      case  4: sum += 2000; return;
      case  3: sum += 2300; return;
      case  2: sum += 2600; return;
      case  1: sum += 2900; return;
      case  0: sum += 3200; return;
   }
}

void CumulativeScore::Opp_3NTX(s64 &sum, uint tricks)
{
   switch (tricks) {
      case  9: sum -=  750; return;
      case 10: sum -=  950; return;
      case 11: sum -= 1150; return;
      case 12: sum -= 1350; return;
      case 13: sum -= 1550; return;

      case  8: sum +=  200; return;
      case  7: sum +=  500; return;
      case  6: sum +=  800; return;
      case  5: sum += 1100; return;
      case  4: sum += 1400; return;
      case  3: sum += 1700; return;
      case  2: sum += 2000; return;
      case  1: sum += 2300; return;
      case  0: sum += 2600; return;
   }
}

void CumulativeScore::Opp_2m(s64 &sum, uint tricks)
{
   switch (tricks) {
      case  8: sum -=  90; return;
      case  9: sum -= 110; return;
      case 10: sum -= 130; return;
      case 11: sum -= 150; return;
      case 12: sum -= 170; return;
      case 13: sum -= 190; return;

      case  7: sum += 100; return;
      case  6: sum += 200; return;
      case  5: sum += 300; return;
      case  4: sum += 400; return;
      case  3: sum += 500; return;
      case  2: sum += 600; return;
      case  1: sum += 700; return;
      case  0: sum += 800; return;
   }
}

void CumulativeScore::Opp_2mX(s64 &sum, uint tricks)
{
   switch (tricks) {
      case  8: sum -= 180; return;
      case  9: sum -= 380; return;
      case 10: sum -= 580; return;
      case 11: sum -= 780; return;
      case 12: sum -= 980; return;
      case 13: sum -= 1180; return;

      case  7: sum +=  200; return;
      case  6: sum +=  500; return;
      case  5: sum +=  800; return;
      case  4: sum += 1100; return;
      case  3: sum += 1400; return;
      case  2: sum += 1700; return;
      case  1: sum += 2000; return;
      case  0: sum += 2300; return;
   }
}



