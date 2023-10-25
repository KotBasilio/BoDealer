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

//#define  DBG_SHOW_BOARD_ON_CONSTRUCTION

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

uint WaCalcHCP(const deal& dl, uint& ctrl)
{
   // not a complex task, knowing that
   // #define RJ     0x0800
   // #define RQ     0x1000
   // #define RK     0x2000
   // #define RA     0x4000
   const auto &cards = dl.remainCards;
   u64 facecards (RA | RK | RQ | RJ);
   SplitBits reducedHand (
      (((cards[SOUTH][SOL_SPADES  ] | cards[NORTH][SOL_SPADES  ]) & facecards) << (1 + 16*3)) |
      (((cards[SOUTH][SOL_HEARTS  ] | cards[NORTH][SOL_HEARTS  ]) & facecards) << (1 + 16*2)) |
      (((cards[SOUTH][SOL_DIAMONDS] | cards[NORTH][SOL_DIAMONDS]) & facecards) << (1 + 16*1)) |
      (((cards[SOUTH][SOL_CLUBS   ] | cards[NORTH][SOL_CLUBS   ]) & facecards) << (1))
   );
   twlHCP hcp(reducedHand);
   twlControls onRed(reducedHand);
   ctrl = onRed.total;

   return hcp.total;
}

void Walrus::PostmortemHCP(DdsTricks& tr, deal& cards)
{
   // detect row and column
   uint row = 0;
   uint ctrl;
   auto hcp = WaCalcHCP(cards, ctrl);
   if (ui.minControls) {
      if (ctrl < (uint)ui.minControls) {
         row = IO_ROW_HCP_START;
      }
      else {
         row = IO_ROW_HCP_START + (ctrl - ui.minControls) * 2;
      }
   }
   else if (hcp < IO_HCP_MIN || IO_HCP_MAX < hcp) {
      return;
   }
   else {
      row = IO_ROW_HCP_START + (hcp - IO_HCP_MIN) * 2;
   }

   //  proper row => add a mark in stat
   if (row < IO_ROW_FILTERING - 1) {
      HitByScore(tr, ui.irBase, row);
      progress.countExtraMarks++;
   }
}


DdsDeal::DdsDeal(const deal &dlBase, DdsTask2 &task)
{
   memcpy(&dl, &dlBase, sizeof(dl));

#ifdef FIXED_HAND_NORTH
   // decrypt all cards
   dl.remainCards[EAST ][SOL_SPADES  ] = DecryptSpades(task.rho);
   dl.remainCards[EAST ][SOL_HEARTS  ] = DecryptHearts(task.rho);
   dl.remainCards[EAST ][SOL_DIAMONDS] = DecryptDiamnd(task.rho);
   dl.remainCards[EAST ][SOL_CLUBS   ] = DecryptClubs (task.rho);
   dl.remainCards[SOUTH][SOL_SPADES  ] = DecryptSpades(task.partner);
   dl.remainCards[SOUTH][SOL_HEARTS  ] = DecryptHearts(task.partner);
   dl.remainCards[SOUTH][SOL_DIAMONDS] = DecryptDiamnd(task.partner);
   dl.remainCards[SOUTH][SOL_CLUBS   ] = DecryptClubs (task.partner);

   // reconstruct 4th hand
   ReconstructWest(SOL_SPADES);
   ReconstructWest(SOL_HEARTS);
   ReconstructWest(SOL_DIAMONDS);
   ReconstructWest(SOL_CLUBS);
#endif // FIXED_HAND_NORTH

#ifdef FIXED_HAND_WEST
   // decrypt all cards
   dl.remainCards[SOUTH][SOL_SPADES  ] = DecryptSpades(task.rho);
   dl.remainCards[SOUTH][SOL_HEARTS  ] = DecryptHearts(task.rho);
   dl.remainCards[SOUTH][SOL_DIAMONDS] = DecryptDiamnd(task.rho);
   dl.remainCards[SOUTH][SOL_CLUBS   ] = DecryptClubs (task.rho);
   dl.remainCards[EAST ][SOL_SPADES  ] = DecryptSpades(task.partner);
   dl.remainCards[EAST ][SOL_HEARTS  ] = DecryptHearts(task.partner);
   dl.remainCards[EAST ][SOL_DIAMONDS] = DecryptDiamnd(task.partner);
   dl.remainCards[EAST ][SOL_CLUBS   ] = DecryptClubs (task.partner);

   // reconstruct 4th hand
   ReconstructNorth(SOL_SPADES);
   ReconstructNorth(SOL_HEARTS);
   ReconstructNorth(SOL_DIAMONDS);
   ReconstructNorth(SOL_CLUBS);
#endif // FIXED_HAND_WEST

   // debug
   #ifdef DBG_SHOW_BOARD_ON_CONSTRUCTION
      PrintHand("A board: \n", dl);
      PLATFORM_GETCH();
   #endif 
}

DdsDeal::DdsDeal(const deal& dlBase, DdsTask3& task)
{
   memcpy(&dl, &dlBase, sizeof(dl));

   // decrypt all cards
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
}

void DdsDeal::Solve(uint handno)
{
   futureTricks fut;
   int target = -1;
   int solutions = PARAM_SOLUTIONS_DDS;
   int mode = 0;
   int threadIndex = 0;

   int res = SolveBoard(dl, target, solutions, mode, &fut, threadIndex);
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
