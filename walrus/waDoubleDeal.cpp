/************************************************************
 * Walrus project                                        2020
 * AI parts
 *
 ************************************************************/

#define  _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include "waDoubleDeal.h"
#include "../dds-develop/examples/hands.h"
#include HEADER_CURSES

//#define  DBG_VIEW_ON_RECONSTRUCTION

bool DdsDeal::needInspect = true;

void DdsDeal::ReconstructNorth(int s)
{
   dl.remainCards[NORTH][s] = 
   dl.remainCards[EAST ][s] ^
   dl.remainCards[WEST ][s] ^
   dl.remainCards[SOUTH][s] ^ RFULL;
}

void DdsDeal::ReconstructWest(int s)
{
   dl.remainCards[WEST ][s] =
   dl.remainCards[EAST ][s] ^
   dl.remainCards[NORTH][s] ^
   dl.remainCards[SOUTH][s] ^ RFULL;
}

void HandleErrorDDS(deal &cards, int res)
{
   char line[80];
   sprintf(line, "Problem hand on solve: leads %s, trumps: %s\n", haPlayerToStr(cards.first), haTrumpToStr(cards.trump));
   PrintHand(line, cards);
   ErrorMessage(res, line);
   printf("DDS error: %s\n", line);
}

void Walrus::HandleDDSFail(int res)
{
   if (res == RETURN_NO_FAULT) {
      return;
   }

   char line[80];
   ErrorMessage(res, line);
   printf("DDS error: %s\n", line);
   PLATFORM_GETCH();
}

void DdsDeal::FillByFixedWest(WaTask2 & task)
{
   // decrypt all cards
   dl.remainCards[SOUTH][SOL_SPADES  ] = DecryptSpades(task.partner);
   dl.remainCards[SOUTH][SOL_HEARTS  ] = DecryptHearts(task.partner);
   dl.remainCards[SOUTH][SOL_DIAMONDS] = DecryptDiamnd(task.partner);
   dl.remainCards[SOUTH][SOL_CLUBS   ] = DecryptClubs (task.partner);
   dl.remainCards[EAST ][SOL_SPADES  ] = DecryptSpades(task.oneOpp);
   dl.remainCards[EAST ][SOL_HEARTS  ] = DecryptHearts(task.oneOpp);
   dl.remainCards[EAST ][SOL_DIAMONDS] = DecryptDiamnd(task.oneOpp);
   dl.remainCards[EAST ][SOL_CLUBS   ] = DecryptClubs (task.oneOpp);

   // reconstruct 4th hand
   ReconstructNorth(SOL_SPADES);
   ReconstructNorth(SOL_HEARTS);
   ReconstructNorth(SOL_DIAMONDS);
   ReconstructNorth(SOL_CLUBS);
}

void DdsDeal::FillByFixedNorth(WaTask2& task)
{
   // decrypt all cards
   dl.remainCards[EAST ][SOL_SPADES  ] = DecryptSpades(task.oneOpp);
   dl.remainCards[EAST ][SOL_HEARTS  ] = DecryptHearts(task.oneOpp);
   dl.remainCards[EAST ][SOL_DIAMONDS] = DecryptDiamnd(task.oneOpp);
   dl.remainCards[EAST ][SOL_CLUBS   ] = DecryptClubs (task.oneOpp);
   dl.remainCards[SOUTH][SOL_SPADES  ] = DecryptSpades(task.partner);
   dl.remainCards[SOUTH][SOL_HEARTS  ] = DecryptHearts(task.partner);
   dl.remainCards[SOUTH][SOL_DIAMONDS] = DecryptDiamnd(task.partner);
   dl.remainCards[SOUTH][SOL_CLUBS   ] = DecryptClubs (task.partner);

   // reconstruct 4th hand
   ReconstructWest(SOL_SPADES);
   ReconstructWest(SOL_HEARTS);
   ReconstructWest(SOL_DIAMONDS);
   ReconstructWest(SOL_CLUBS);
}

#ifdef FIXED_HAND_NORTH
   #define FILL_BY_TASK_WITH_FIXED_HAND  FillByFixedNorth(task)
#elif defined( FIXED_HAND_WEST )
   #define FILL_BY_TASK_WITH_FIXED_HAND  FillByFixedWest(task)
#else
   #define FILL_BY_TASK_WITH_FIXED_HAND  DEBUG_UNEXPECTED
#endif 

DdsDeal::DdsDeal(const deal &dlBase, WaTask2 &task)
{
   // base deal contains a denomination to solve and 
   memcpy(&dl, &dlBase, sizeof(dl));

   // task-2 contains 2 hands, enough for reconstruction
   FILL_BY_TASK_WITH_FIXED_HAND;

   // may see
   if (config.dbg.viewBoardOnReconstruct) {
      PrintHand("A board: \n", dl);
      PLATFORM_GETCH();
   }
}

DdsDeal::DdsDeal(const deal& dlBase, WaTask3& task)
{
   // base deal contains a denomination to solve and 
   memcpy(&dl, &dlBase, sizeof(dl));

   // task-3 contains 3 hands. We reconstruct the last one.
   dl.remainCards[NORTH][SOL_SPADES  ] = DecryptSpades(task.north);
   dl.remainCards[NORTH][SOL_HEARTS  ] = DecryptHearts(task.north);
   dl.remainCards[NORTH][SOL_DIAMONDS] = DecryptDiamnd(task.north);
   dl.remainCards[NORTH][SOL_CLUBS   ] = DecryptClubs (task.north);
   dl.remainCards[EAST ][SOL_SPADES  ] = DecryptSpades(task.east);
   dl.remainCards[EAST ][SOL_HEARTS  ] = DecryptHearts(task.east);
   dl.remainCards[EAST ][SOL_DIAMONDS] = DecryptDiamnd(task.east);
   dl.remainCards[EAST ][SOL_CLUBS   ] = DecryptClubs (task.east);
   dl.remainCards[SOUTH][SOL_SPADES  ] = DecryptSpades(task.south);
   dl.remainCards[SOUTH][SOL_HEARTS  ] = DecryptHearts(task.south);
   dl.remainCards[SOUTH][SOL_DIAMONDS] = DecryptDiamnd(task.south);
   dl.remainCards[SOUTH][SOL_CLUBS   ] = DecryptClubs (task.south);
   ReconstructWest(SOL_SPADES);
   ReconstructWest(SOL_HEARTS);
   ReconstructWest(SOL_DIAMONDS);
   ReconstructWest(SOL_CLUBS);

   // may see
   if (config.dbg.viewBoardOnReconstruct) {
      PrintHand("A board: \n", dl);
      PLATFORM_GETCH();
   }
}

void DdsDeal::Solve(uint handno)
{
   futureTricks fut;
   int target = -1;
   int mode = 0;
   int threadIndex = 0;

   int res = SolveBoard(dl, target, config.ddsSolutions, mode, &fut, threadIndex);
   if (res != RETURN_NO_FAULT) {
      HandleErrorDDS(dl, res);
      PLATFORM_GETCH();
   }

   // inspect
   if (needInspect) {
      PrintHand("ONE-BY-ONE solving\n", dl);
      PrintFut("", &fut);
      auto inchar = PLATFORM_GETCH();
      if (inchar != ' ') {
         needInspect = false;
      }
   }

   // fill tricks
   tr.Init(fut);
}

DdsDeal::DdsDeal(twContext* lay)
{
   dl.trump = SOL_NOTRUMP;
   dl.first = WEST;

   dl.currentTrickSuit[0] = 0;
   dl.currentTrickSuit[1] = 0;
   dl.currentTrickSuit[2] = 0;

   dl.currentTrickRank[0] = 0;
   dl.currentTrickRank[1] = 0;
   dl.currentTrickRank[2] = 0;

   for (int h = 0; h < DDS_HANDS; h++) {
      for (int s = 0; s < DDS_SUITS; s++) {
         dl.remainCards[h][s] = 0;
      }
   }

   // decrypt all cards
   dl.remainCards[NORTH][SOL_SPADES  ] = DecryptSpades(lay[NORTH].hand);
   dl.remainCards[NORTH][SOL_HEARTS  ] = DecryptHearts(lay[NORTH].hand);
   dl.remainCards[NORTH][SOL_DIAMONDS] = DecryptDiamnd(lay[NORTH].hand);
   dl.remainCards[NORTH][SOL_CLUBS   ] = DecryptClubs (lay[NORTH].hand);
   dl.remainCards[EAST ][SOL_SPADES  ] = DecryptSpades(lay[EAST ].hand);
   dl.remainCards[EAST ][SOL_HEARTS  ] = DecryptHearts(lay[EAST ].hand);
   dl.remainCards[EAST ][SOL_DIAMONDS] = DecryptDiamnd(lay[EAST ].hand);
   dl.remainCards[EAST ][SOL_CLUBS   ] = DecryptClubs (lay[EAST ].hand);
   dl.remainCards[SOUTH][SOL_SPADES  ] = DecryptSpades(lay[SOUTH].hand);
   dl.remainCards[SOUTH][SOL_HEARTS  ] = DecryptHearts(lay[SOUTH].hand);
   dl.remainCards[SOUTH][SOL_DIAMONDS] = DecryptDiamnd(lay[SOUTH].hand);
   dl.remainCards[SOUTH][SOL_CLUBS   ] = DecryptClubs (lay[SOUTH].hand);
   ReconstructWest(SOL_SPADES);
   ReconstructWest(SOL_HEARTS);
   ReconstructWest(SOL_DIAMONDS);
   ReconstructWest(SOL_CLUBS);
}

