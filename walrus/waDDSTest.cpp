#define  _CRT_SECURE_NO_WARNINGS
// Test program for the SolveBoard function.
// Uses the hands pre-set in hands.cpp.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../dds-develop/include/dll.h"
#include "../dds-develop/examples/hands.h"
#include "walrus.h"
#include HEADER_CURSES

// Hands in binary: (or similar)
//               54
//               JT
//               AJT987
//               T87
//
// s KQ32                      T98
// h A932                      Q876
// d 2                         K43
// c Q932                      J54
//               AJ76
//               K54
//               Q65
//               AK6
//
// The first index is suit, the last index is hand.
unsigned int tds_holdings[4][4] =
{ // North                 East             South          West
   { R5|R4|R8,             RT|R9,           RA|RJ|R7|R6,   RK|RQ|R3|R2  } , // spades
   { RJ|RT,                RQ|R8|R7|R6,     RK|R5|R4,      RA|R9|R3|R2  } , // hearts
   { RA|RJ|RT|R9|R8|R7,    RK|R4,           RQ|R6|R5|R3,   R2           } , // diamonds
   { RT|R8,                RJ|R5|R4|R6|R7,  RA|RK,         RQ|R9|R3|R2  }   // clubs
};

void sample_main_JK_Solve()
{
   SetMaxThreads(0);

   deal dl;
   futureTricks fut1; // solutions == 1

   int target;
   int solutions;
   int mode;
   int threadIndex = 0;
   int res;
   char line[80];

   int handno = 1;
   dl.trump = SOL_NOTRUMP;
   dl.first = WEST;

   dl.currentTrickSuit[0] = 0;
   dl.currentTrickSuit[1] = 0;
   dl.currentTrickSuit[2] = 0;

   dl.currentTrickRank[0] = 0;
   dl.currentTrickRank[1] = 0;
   dl.currentTrickRank[2] = 0;

   for (int h = 0; h < DDS_HANDS; h++)
      for (int s = 0; s < DDS_SUITS; s++)
         dl.remainCards[h][s] = tds_holdings[s][h];

   sprintf(line, "SolveBoard, hand %d: leads %s, trumps: %s\n", handno,
      haPlayerToStr(dl.first), haTrumpToStr(dl.trump) );
   PrintHand(line, dl);

   target = -1;
   solutions = 3;
   mode = 0;
   res = SolveBoard(dl, target, solutions, mode, &fut1, threadIndex);

   if (res != RETURN_NO_FAULT)
   {
      ErrorMessage(res, line);
      printf("DDS error: %s\n", line);
   }

   PrintFut("", &fut1);
}

void sample_main_SolveBoard_S1()
{
   SetMaxThreads(0);

   deal dl;
   futureTricks fut1; // solutions == 1

   int target;
   int solutions;
   int mode;
   int threadIndex = 0;
   int res;
   char line[80];

   for (int handno = 1; handno < 2 /*3*/; handno++)
   {
      dl.trump = trump[handno];
      dl.first = first[handno];

      dl.currentTrickSuit[0] = 0;
      dl.currentTrickSuit[1] = 0;
      dl.currentTrickSuit[2] = 0;

      dl.currentTrickRank[0] = 0;
      dl.currentTrickRank[1] = 0;
      dl.currentTrickRank[2] = 0;

      for (int h = 0; h < DDS_HANDS; h++)
         for (int s = 0; s < DDS_SUITS; s++)
            dl.remainCards[h][s] = holdings[handno][s][h];

      sprintf(line, "SolveBoard, hand %d: leads %s, trumps: %s\n", handno,
         haPlayerToStr(dl.first), haTrumpToStr(dl.trump) );
      PrintHand(line, dl);

      target = -1;
      solutions = 1;
      mode = 0;
      res = SolveBoard(dl, target, solutions, mode, &fut1, threadIndex);

      if (res != RETURN_NO_FAULT)
      {
         ErrorMessage(res, line);
         printf("DDS error: %s\n", line);
      }

      PrintFut("", &fut1);
      PLATFORM_GETCH();

      // W.E.T.
      dl.first = NORTH;
      sprintf(line, "SolveBoard, hand %d: leads %s, trumps: %s\n", handno,
         haPlayerToStr(dl.first), haTrumpToStr(dl.trump) );
      PrintHand(line, dl);

      target = -1;
      solutions = 1;
      mode = 0;
      res = SolveBoard(dl, target, solutions, mode, &fut1, threadIndex);

      if (res != RETURN_NO_FAULT)
      {
         ErrorMessage(res, line);
         printf("DDS error: %s\n", line);
      }

      PrintFut("", &fut1);
   }
}

void sample_main_SolveBoard()
{
   SetMaxThreads(0);

   deal dl;
   futureTricks fut2, // solutions == 2
      fut3; // solutions == 3

   int target;
   int solutions;
   int mode;
   int threadIndex = 0;
   int res;
   char line[80];
   bool match2;
   bool match3;

   for (int handno = 0; handno < 3; handno++)
   {
      dl.trump = trump[handno];
      dl.first = first[handno];

      dl.currentTrickSuit[0] = 0;
      dl.currentTrickSuit[1] = 0;
      dl.currentTrickSuit[2] = 0;

      dl.currentTrickRank[0] = 0;
      dl.currentTrickRank[1] = 0;
      dl.currentTrickRank[2] = 0;

      for (int h = 0; h < DDS_HANDS; h++)
         for (int s = 0; s < DDS_SUITS; s++)
            dl.remainCards[h][s] = holdings[handno][s][h];

      target = -1;
      solutions = 3;
      mode = 0;
      res = SolveBoard(dl, target, solutions, mode, &fut3, threadIndex);

      if (res != RETURN_NO_FAULT)
      {
         ErrorMessage(res, line);
         printf("DDS error: %s\n", line);
      }

      // auto-control vs expected results
      match3 = CompareFut(&fut3, handno, solutions);

      solutions = 2;
      res = SolveBoard(dl, target, solutions, mode, &fut2, threadIndex);
      if (res != RETURN_NO_FAULT)
      {
         ErrorMessage(res, line);
         printf("DDS error: %s\n", line);
      }

      // auto-control vs expected results
      match2 = CompareFut(&fut2, handno, solutions);

      sprintf(line,
         "SolveBoard, hand %d: solutions 3 %s, solutions 2 %s\n",
         handno + 1,
         (match3 ? "OK" : "ERROR"),
         (match2 ? "OK" : "ERROR"));

      PrintHand(line, dl);

      sprintf(line, "solutions == 3 leads %s, trumps: %s\n",  haPlayerToStr(dl.first), haTrumpToStr(dl.trump) );
      PrintFut(line, &fut3);
      sprintf(line, "solutions == 2 leads %s, trumps: %s\n",  haPlayerToStr(dl.first), haTrumpToStr(dl.trump) );
      PrintFut(line, &fut2);
   }
}

extern unsigned char dcardSuit[5], dcardRank[16];


void sample_main_PlayBin()
{
   SetMaxThreads(0);

   DDSInfo info;
   GetDDSInfo(&info);

   deal dl;
   playTraceBin DDplay;
   solvedPlay solved;

   int threadIndex = 0, res;
   char line[80];
   bool match;

   for (int handno = 0; handno < 3; handno++)
   {
      dl.trump = trump[handno];
      dl.first = first[handno];

      dl.currentTrickSuit[0] = 0;
      dl.currentTrickSuit[1] = 0;
      dl.currentTrickSuit[2] = 0;

      dl.currentTrickRank[0] = 0;
      dl.currentTrickRank[1] = 0;
      dl.currentTrickRank[2] = 0;

      for (int h = 0; h < DDS_HANDS; h++)
         for (int s = 0; s < DDS_SUITS; s++)
            dl.remainCards[h][s] = holdings[handno][s][h];

      DDplay.number = playNo[handno];
      for (int i = 0; i < playNo[handno]; i++)
      {
         DDplay.suit[i] = playSuit[handno][i];
         DDplay.rank[i] = playRank[handno][i];
      }

      res = AnalysePlayBin(dl, DDplay, &solved, threadIndex);

      if (res != RETURN_NO_FAULT)
      {
         ErrorMessage(res, line);
         printf("DDS error: %s\n", line);
      }

      match = ComparePlay(&solved, handno);

      sprintf(line, "AnalysePlayBin, hand %d: %s\n",
         handno + 1, (match ? "OK" : "ERROR"));

      PrintHand(line, dl);

      PrintBinPlay(&DDplay, &solved);
   }

   GetDDSInfo(&info);
}

void DoSelfTests()
{
   //sample_main_PlayBin();
   //sample_main_SolveBoard();
   //sample_main_SolveBoard_S1();
   //sample_main_JK_Solve();
}
