/************************************************************
 * Walrus project                                        2020
 * AI parts
 *
 ************************************************************/
#include <string.h>

#include "walrus.h"
#include "../dds-develop/include/dll.h"
#include "../dds-develop/examples/hands.h"

void CumulativeScore::Our4M(uint tricks)
{
   // "always game" strategy
   switch (tricks) {
      case 1:  bidGame -= 900; break;
      case 2:  bidGame -= 800; break;
      case 3:  bidGame -= 700; break;
      case 4:  bidGame -= 600; break;
      case 5:  bidGame -= 500; break;
      case 6:  bidGame -= 400; break;
      case 7:  bidGame -= 300; break;
      case 8:  bidGame -= 200; break;
      case 9:  bidGame -= 100; break;
      case 10: bidGame += 620; break;
      case 11: bidGame += 650; break;
      case 12: bidGame += 680; break;
      case 13: bidGame += 710; break;
   }

   // "always partscore" strategy
   int partdelta = tricks > 6 ? 
      50 + (tricks - 6) * 30 : 
      -100 * (7 - tricks);
   partscore += partdelta;

   // "ideal"
   if (tricks < 10) {
      ideal += partdelta;
   } else {
      ideal += 620 + (tricks - 10) * 30;
   }
}

void CumulativeScore::OurNV4M(uint tricks)
{
   // "always game" strategy
   switch (tricks) {
      case 1:  bidGame -= 450; break;
      case 2:  bidGame -= 400; break;
      case 3:  bidGame -= 350; break;
      case 4:  bidGame -= 300; break;
      case 5:  bidGame -= 250; break;
      case 6:  bidGame -= 200; break;
      case 7:  bidGame -= 150; break;
      case 8:  bidGame -= 100; break;
      case 9:  bidGame -= 50;  break;
      case 10: bidGame += 420; break;
      case 11: bidGame += 450; break;
      case 12: bidGame += 480; break;
      case 13: bidGame += 510; break;
   }

   // "always partscore" strategy
   int partdelta = tricks > 6 ?
      50 + (tricks - 6) * 30 :
      -50 * (7 - tricks);
   partscore += partdelta;

   // "ideal"
   if (tricks < 10) {
      ideal += partdelta;
   } else {
      ideal += 420 + (tricks - 10) * 30;
   }
}

void CumulativeScore::OurNV4MX(uint tricks)
{
   // "always game" strategy
   switch (tricks) {
      case 1:  bidGame -= 2300; break;
      case 2:  bidGame -= 2000; break;
      case 3:  bidGame -= 1700; break;
      case 4:  bidGame -= 1400; break;
      case 5:  bidGame -= 1100; break;
      case 6:  bidGame -= 800;  break;
      case 7:  bidGame -= 500;  break;
      case 8:  bidGame -= 300;  break;
      case 9:  bidGame -= 100;  break;
      case 10: bidGame += 590;  break;
      case 11: bidGame += 690;  break;
      case 12: bidGame += 790;  break;
      case 13: bidGame += 890;  break;
   }

   // "always partscore" strategy
   int partdelta = tricks > 6 ?
      50 + (tricks - 6) * 30 :
      -50 * (7 - tricks);
   partscore += partdelta;

   // "ideal"
   if (tricks < 10) {
      ideal += partdelta;
   } else {
      ideal += 590 + (tricks - 10) * 100;
   }
}

void CumulativeScore::Our5M(uint tricks)
{
   // "always game" strategy
   switch (tricks) {
      case 2:  bidGame -= 900; break;
      case 3:  bidGame -= 800; break;
      case 4:  bidGame -= 700; break;
      case 5:  bidGame -= 600; break;
      case 6:  bidGame -= 500; break;
      case 7:  bidGame -= 400; break;
      case 8:  bidGame -= 300; break;
      case 9:  bidGame -= 200; break;
      case 10: bidGame -= 100; break;
      case 11: bidGame += 650; break;
      case 12: bidGame += 680; break;
      case 13: bidGame += 710; break;
   }

   // "always partscore" strategy
   int partdelta = tricks > 6 ? 
      50 + (tricks - 6) * 30 : 
      -100 * (7 - tricks);
   partscore += partdelta;

   // "ideal"
   if (tricks < 11) {
      ideal += partdelta;
   } else {
      ideal += 620 + (tricks - 10) * 30;
   }
}

void CumulativeScore::Our4minor(uint tricks)
{
   // both strategies
   switch (tricks) {
      case 2:  bidGame -= 800; partscore -= 800; break;
      case 3:  bidGame -= 700; partscore -= 700; break;
      case 4:  bidGame -= 600; partscore -= 600; break;
      case 5:  bidGame -= 500; partscore -= 500; break;
      case 6:  bidGame -= 400; partscore -= 400; break;
      case 7:  bidGame -= 300; partscore -= 300; break;
      case 8:  bidGame -= 200; partscore -= 200; break;
      case 9:  bidGame -= 100; partscore -= 100; break;
      case 10: bidGame += 130; partscore += 130; break;
      case 11: bidGame += 150; partscore += 150; break;
      case 12: bidGame += 170; partscore += 170; break;
      case 13: bidGame += 190; partscore += 190; break;
   }

   // "ideal"
   ideal += 50;
}

void CumulativeScore::Our5minor(uint tricks)
{
   // "always game" strategy
   switch (tricks) {
      case 2:  bidGame -= 900; break;
      case 3:  bidGame -= 800; break;
      case 4:  bidGame -= 700; break;
      case 5:  bidGame -= 600; break;
      case 6:  bidGame -= 500; break;
      case 7:  bidGame -= 400; break;
      case 8:  bidGame -= 300; break;
      case 9:  bidGame -= 200; break;
      case 10: bidGame -= 100; break;
      case 11: bidGame += 600; break;
      case 12: bidGame += 620; break;
      case 13: bidGame += 640; break;
   }

   // "always partscore" strategy
   int partdelta = 0;
   switch (tricks) {
      case 2:  partdelta -= 700; break;
      case 3:  partdelta -= 600; break;
      case 4:  partdelta -= 500; break;
      case 5:  partdelta -= 400; break;
      case 6:  partdelta -= 300; break;
      case 7:  partdelta -= 200; break;
      case 8:  partdelta -= 100; break;
      case 9:  partdelta += 110; break;
      case 10: partdelta += 130; break;
      case 11: partdelta += 150; break;
      case 12: partdelta += 170; break;
      case 13: partdelta += 190; break;
   }
   partscore += partdelta;

   // "ideal"
   if (tricks < 10) {
      ideal += partdelta;
   } else {
      ideal += 600 + (tricks - 11) * 20;
   }
}

void CumulativeScore::OurNV5minor(uint tricks)
{
   // "always game" strategy
   switch (tricks) {
      case 2:  bidGame -= 450; break;
      case 3:  bidGame -= 400; break;
      case 4:  bidGame -= 350; break;
      case 5:  bidGame -= 300; break;
      case 6:  bidGame -= 250; break;
      case 7:  bidGame -= 200; break;
      case 8:  bidGame -= 150; break;
      case 9:  bidGame -= 100; break;
      case 10: bidGame -= 50; break;
      case 11: bidGame += 400; break;
      case 12: bidGame += 420; break;
      case 13: bidGame += 440; break;
   }

   // "always partscore" strategy
   int partdelta = 0;
   switch (tricks) {
      case 2:  partdelta -= 350; break;
      case 3:  partdelta -= 300; break;
      case 4:  partdelta -= 250; break;
      case 5:  partdelta -= 200; break;
      case 6:  partdelta -= 150; break;
      case 7:  partdelta -= 100; break;
      case 8:  partdelta -= 50 ; break;
      case 9:  partdelta += 110; break;
      case 10: partdelta += 130; break;
      case 11: partdelta += 150; break;
      case 12: partdelta += 170; break;
      case 13: partdelta += 190; break;
   }
   partscore += partdelta;

   // "ideal"
   if (tricks < 10) {
      ideal += partdelta;
   } else {
      ideal += 400 + (tricks - 11) * 20;
   }
}

void CumulativeScore::OurNV5mX(uint tricks)
{
   // "always game" strategy
   switch (tricks) {
      case 2:  bidGame -= 2300; break;
      case 3:  bidGame -= 2000; break;
      case 4:  bidGame -= 1700; break;
      case 5:  bidGame -= 1400; break;
      case 6:  bidGame -= 1100; break;
      case 7:  bidGame -= 800;  break;
      case 8:  bidGame -= 500;  break;
      case 9:  bidGame -= 300;  break;
      case 10: bidGame -= 100;  break;
      case 11: bidGame += 550;  break;
      case 12: bidGame += 650;  break;
      case 13: bidGame += 750;  break;
   }

   // "always partscore" strategy
   int partdelta = tricks > 6 ?
      50 + (tricks - 6) * 20 :
      -50 * (7 - tricks);
   partscore += partdelta;

   // "ideal"
   if (tricks < 11) {
      ideal += partdelta;
   } else {
      ideal += 550 + (tricks - 11) * 100;
   }
}

void CumulativeScore::OurNV5M(uint tricks)
{
   // "always game" strategy
   switch (tricks) {
      case 2:  bidGame -= 450; break;
      case 3:  bidGame -= 400; break;
      case 4:  bidGame -= 350; break;
      case 5:  bidGame -= 300; break;
      case 6:  bidGame -= 250; break;
      case 7:  bidGame -= 200; break;
      case 8:  bidGame -= 150; break;
      case 9:  bidGame -= 100; break;
      case 10: bidGame -= 50 ; break;
      case 11: bidGame += 450; break;
      case 12: bidGame += 480; break;
      case 13: bidGame += 510; break;
   }

   // "always partscore" strategy -- not applicable
   int partdelta = 0;
   partscore += partdelta;

   // "ideal"
   if (tricks < 10) {
      ideal += partdelta;
   } else {
      ideal += 450 + (tricks - 11) * 30;
   }
}

void CumulativeScore::Our3NT(uint tricks)
{
   // "always game" strategy
   switch (tricks) {
      case 2:  bidGame -= 700; break;
      case 3:  bidGame -= 600; break;
      case 4:  bidGame -= 500; break;
      case 5:  bidGame -= 400; break;
      case 6:  bidGame -= 300; break;
      case 7:  bidGame -= 200; break;
      case 8:  bidGame -= 100; break;
      case 9:  bidGame += 600; break;
      case 10: bidGame += 630; break;
      case 11: bidGame += 660; break;
      case 12: bidGame += 690; break;
      case 13: bidGame += 720; break;
   }

   // "always partscore" strategy
   int partdelta = tricks > 6 ? 
      90 + (tricks - 7) * 30 : 
      -100 * (7 - tricks);
   partscore += partdelta;

   // "ideal"
   if (tricks < 9) {
      ideal += partdelta;
   } else {
      ideal += 600 + (tricks - 9) * 30;
   }
}

void CumulativeScore::Our1NT(uint tricks)
{
   // "always partscore" strategy
   int partdelta = tricks > 6 ? 
      90 + (tricks - 7) * 30 : 
      -100 * (7 - tricks);
   partscore += partdelta;
   bidGame += partdelta;
   ideal += partdelta;
}

void CumulativeScore::Our2M(uint tricks)
{
   // "always partscore" strategy
   int partdelta = tricks > 7 ? 
      80 + (tricks - 7) * 30 : 
      -100 * (8 - tricks);
   partscore += partdelta;
   bidGame += partdelta;
   ideal += partdelta;
}

void CumulativeScore::OurNV6m(uint tricks)
{
   // "always slam" strategy
   switch (tricks) {
      case 2:  bidSlam -= 450; break;
      case 3:  bidSlam -= 350; break;
      case 4:  bidSlam -= 400; break;
      case 5:  bidSlam -= 350; break;
      case 6:  bidSlam -= 300; break;
      case 7:  bidSlam -= 250; break;
      case 8:  bidSlam -= 200; break;
      case 9:  bidSlam -= 150; break;
      case 10: bidSlam -= 100; break;
      case 11: bidSlam -= 50;  break;
      case 12: bidSlam += 920; break;
      case 13: bidSlam += 940; break;
   }

   // "always game" strategy
   int gameDelta = tricks > 10 ? 
      300 + (tricks - 6) * 20 : 
      -50 * (11 - tricks);
   bidGame += gameDelta;

   // "ideal"
   if (tricks < 12) {
      ideal += gameDelta;
   } else {
      ideal += 920 + (tricks - 12) * 20;
   }
}

void CumulativeScore::OurNV7m(uint tricks)
{
   // "always slam" strategy
   switch (tricks) {
      case 2:  bidSlam -= 500; break;
      case 3:  bidSlam -= 450; break;
      case 4:  bidSlam -= 350; break;
      case 5:  bidSlam -= 400; break;
      case 6:  bidSlam -= 350; break;
      case 7:  bidSlam -= 300; break;
      case 8:  bidSlam -= 250; break;
      case 9:  bidSlam -= 200; break;
      case 10: bidSlam -= 150; break;
      case 11: bidSlam -= 100; break;
      case 12: bidSlam -= 50;  break;
      case 13: bidSlam += 1440; break;
   }

   // "always game" strategy
   int gameDelta = tricks > 11 ?
      500 + 300 + (tricks - 6) * 20 :
      -50 * (12 - tricks);
   bidGame += gameDelta;

   // "ideal"
   if (tricks < 13) {
      ideal += gameDelta;
   } else {
      ideal += 1440;
   }
}

void CumulativeScore::OurNV7NT(uint tricks)
{
   // "always slam" strategy
   switch (tricks) {
      case 2:  bidSlam -= 500; break;
      case 3:  bidSlam -= 450; break;
      case 4:  bidSlam -= 350; break;
      case 5:  bidSlam -= 400; break;
      case 6:  bidSlam -= 350; break;
      case 7:  bidSlam -= 300; break;
      case 8:  bidSlam -= 250; break;
      case 9:  bidSlam -= 200; break;
      case 10: bidSlam -= 150; break;
      case 11: bidSlam -= 100; break;
      case 12: bidSlam -= 50;  break;
      case 13: bidSlam += 1520; break;
   }

   // "always game" strategy
   int gameDelta = tricks > 11 ?
      500 + 300 + (tricks - 6) * 30 + 10 :
      -50 * (12 - tricks);
   bidGame += gameDelta;

   // "ideal"
   if (tricks < 13) {
      ideal += gameDelta;
   } else {
      ideal += 1520;
   }
}

void CumulativeScore::OurNV6Maj(uint tricks)
{
   // "always slam" strategy
   switch (tricks) {
      case 2:  bidSlam -=  450; break;
      case 3:  bidSlam -=  350; break;
      case 4:  bidSlam -=  400; break;
      case 5:  bidSlam -=  350; break;
      case 6:  bidSlam -=  300; break;
      case 7:  bidSlam -=  250; break;
      case 8:  bidSlam -=  200; break;
      case 9:  bidSlam -=  150; break;
      case 10: bidSlam -=  100; break;
      case 11: bidSlam -=  50;  break;
      case 12: bidSlam +=  980; break;
      case 13: bidSlam += 1010; break;
   }

   // "always game" strategy
   int gameDelta = tricks > 10 ?
      300 + (tricks - 6) * 30 :
      -50 * (11 - tricks);
   bidGame += gameDelta;

   // "ideal"
   if (tricks < 12) {
      ideal += gameDelta;
   } else {
      ideal += 980 + (tricks - 12) * 30;
   }
}

void CumulativeScore::Our6_No(uint tricks)
{
   // "always slam" strategy
   switch (tricks) {
      case 2:  bidSlam -= 1000; break;
      case 3:  bidSlam -= 900; break;
      case 4:  bidSlam -= 800; break;
      case 5:  bidSlam -= 700; break;
      case 6:  bidSlam -= 600; break;
      case 7:  bidSlam -= 500; break;
      case 8:  bidSlam -= 400; break;
      case 9:  bidSlam -= 300; break;
      case 10: bidSlam -= 200; break;
      case 11: bidSlam -= 100;  break;
      case 12: bidSlam += 1440; break;
      case 13: bidSlam += 1470; break;
   }

   // "always game" strategy
   int gameDelta = tricks > 8 ?
      600 + (tricks - 9) * 30 :
      -100 * (9 - tricks);
   bidGame += gameDelta;

   // "ideal"
   if (tricks < 12) {
      ideal += gameDelta;
   } else {
      ideal += 1440 + (tricks - 12) * 30;
   }
}

void CumulativeScore::OurNV6_No(uint tricks)
{
   // "always slam" strategy
   switch (tricks) {
      case 2:  bidSlam -= 450; break;
      case 3:  bidSlam -= 350; break;
      case 4:  bidSlam -= 400; break;
      case 5:  bidSlam -= 350; break;
      case 6:  bidSlam -= 300; break;
      case 7:  bidSlam -= 250; break;
      case 8:  bidSlam -= 200; break;
      case 9:  bidSlam -= 150; break;
      case 10: bidSlam -= 100; break;
      case 11: bidSlam -= 50;  break;
      case 12: bidSlam += 990; break;
      case 13: bidSlam += 1020; break;
   }

   // "always game" strategy
   int gameDelta = tricks > 8 ?
      400 + (tricks - 9) * 30 :
      -50 * (9 - tricks);
   bidGame += gameDelta;

   // "ideal"
   if (tricks < 12) {
      ideal += gameDelta;
   } else {
      ideal += 990 + (tricks - 12) * 30;
   }
}


