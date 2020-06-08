/************************************************************
 * Walrus project                                        2020
 * AI parts
 *
 ************************************************************/

#define  _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include "..\dds-develop\include\dll.h"
#include "..\dds-develop\examples\hands.h"
#include <conio.h>
#include "walrus.h"

extern int SolveAllBoardsN(boards& bds, solvedBoards& solved);

void DdsTask::Init(SplitBits &part, SplitBits &opp)
{
   partner.Init(part);
   rho.Init(opp);
}

void DdsTask::DTUnion::Init(SplitBits &sb)
{
   hand = sb;
}

uint CountBits(uint v)// count bits set in this (32-bit value)
{
   uint c; // store the total here
   static const int S[] = { 1, 2, 4, 8, 16 }; // Magic Binary Numbers
   static const int B[] = { 0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF, 0x0000FFFF };

   c = v - ((v >> 1) & B[0]);
   c = ((c >> S[1]) & B[1]) + (c & B[1]);
   c = ((c >> S[2]) + c) & B[2];
   c = ((c >> S[3]) + c) & B[3];
   c = ((c >> S[4]) + c) & B[4];

   return c;
}

struct DdsDeal 
{
   // to pass to DDS
   deal dl;

   // result tricks
   DdsTricks tr;

   // viewing
   static bool needInspect;

   DdsDeal(const deal &dlBase, DdsPack &task);
private:

   void ReconsctuctNorth(int s)
   {
      dl.remainCards[NORTH][s] = 
      dl.remainCards[EAST ][s] ^
      dl.remainCards[WEST ][s] ^
      dl.remainCards[SOUTH][s] ^ RFULL;
   }

   void ReconsctuctWest(int s)
   {
      dl.remainCards[WEST ][s] =
      dl.remainCards[EAST ][s] ^
      dl.remainCards[NORTH][s] ^
      dl.remainCards[SOUTH][s] ^ RFULL;
   }

   uint DecryptSpades(DdsTask::DTUnion bits) { return bits.hand.card.w.s.Decrypt(); }
   uint DecryptHearts(DdsTask::DTUnion bits) { return bits.hand.card.w.h.Decrypt(); }
   uint DecryptDiamnd(DdsTask::DTUnion bits) { return bits.hand.card.w.d.Decrypt(); }
   uint DecryptClubs (DdsTask::DTUnion bits) { return bits.hand.card.w.c.Decrypt(); }

public:

   void Solve(uint handno)
   {
      char line[80];
      futureTricks fut;
      int target = -1;
      int solutions = PARAM_SOLUTIONS_DDS;  
      int mode = 0;
      int threadIndex = 0;

      int res = SolveBoard(dl, target, solutions, mode, &fut, threadIndex);
      if (res != RETURN_NO_FAULT) {
         sprintf(line, "Problem hand on solve, #%d: leads %s, trumps: %s\n", handno,
            haPlayerToStr(dl.first), haTrumpToStr(dl.trump) );
         PrintHand(line, dl.remainCards);
         ErrorMessage(res, line);
         printf("DDS error: %s\n", line);
         _getch();
      }

      // inspect
      if (needInspect) {
         PrintHand("ONE-BY-ONE solving\n", dl.remainCards);
         PrintFut("", &fut);
         auto inchar = _getch();
         if (inchar != ' ') {
            needInspect = false;
         }
      }

      // fill tricks
      tr.Init(fut);
   }
};

bool DdsDeal::needInspect = true;

//#define  DBG_VIEW_BOARD_ON_CONSTRUCTION

DdsDeal::DdsDeal(const deal &dlBase, DdsPack &t)
{
   memcpy(&dl, &dlBase, sizeof(dl));

#ifdef FIXED_HAND_NORTH
   // decrypt all cards
   dl.remainCards[EAST ][SOL_SPADES  ] = DecryptSpades(t.task.rho);
   dl.remainCards[EAST ][SOL_HEARTS  ] = DecryptHearts(t.task.rho);
   dl.remainCards[EAST ][SOL_DIAMONDS] = DecryptDiamnd(t.task.rho);
   dl.remainCards[EAST ][SOL_CLUBS   ] = DecryptClubs(t.task.rho);
   dl.remainCards[SOUTH][SOL_SPADES  ] = DecryptSpades(t.task.partner);
   dl.remainCards[SOUTH][SOL_HEARTS  ] = DecryptHearts(t.task.partner);
   dl.remainCards[SOUTH][SOL_DIAMONDS] = DecryptDiamnd(t.task.partner);
   dl.remainCards[SOUTH][SOL_CLUBS   ] = DecryptClubs(t.task.partner);

   // reconstruct 4th hand
   ReconsctuctWest(SOL_SPADES);
   ReconsctuctWest(SOL_HEARTS);
   ReconsctuctWest(SOL_DIAMONDS);
   ReconsctuctWest(SOL_CLUBS);
#endif // FIXED_HAND_NORTH

#ifdef FIXED_HAND_WEST
   // decrypt all cards
   dl.remainCards[SOUTH][SOL_SPADES  ] = DecryptSpades(t.task.rho);
   dl.remainCards[SOUTH][SOL_HEARTS  ] = DecryptHearts(t.task.rho);
   dl.remainCards[SOUTH][SOL_DIAMONDS] = DecryptDiamnd(t.task.rho);
   dl.remainCards[SOUTH][SOL_CLUBS   ] = DecryptClubs (t.task.rho);
   dl.remainCards[EAST ][SOL_SPADES  ] = DecryptSpades(t.task.partner);
   dl.remainCards[EAST ][SOL_HEARTS  ] = DecryptHearts(t.task.partner);
   dl.remainCards[EAST ][SOL_DIAMONDS] = DecryptDiamnd(t.task.partner);
   dl.remainCards[EAST ][SOL_CLUBS   ] = DecryptClubs (t.task.partner);

   // reconstruct 4th hand
   ReconsctuctNorth(SOL_SPADES);
   ReconsctuctNorth(SOL_HEARTS);
   ReconsctuctNorth(SOL_DIAMONDS);
   ReconsctuctNorth(SOL_CLUBS);
#endif // FIXED_HAND_WEST

   // debug
   #ifdef DBG_VIEW_BOARD_ON_CONSTRUCTION
      char line[80];
      sprintf(line, "A board: \n");
      PrintHand(line, dl.remainCards);
      _getch();
   #endif 
}

void Walrus::SolveOneByOne(deal &dlBase)
{
#ifdef _DEBUG
   uint freqMiniReport = 0x3f;
#else
   uint freqMiniReport = 0xff;
#endif // _DEBUG

   for (uint i = 0; i < countToSolve ; i++) {
      // refill & solve the next deal
      DdsDeal dl(dlBase, arrToSolve[i]);
      dl.Solve(i);

      // pass
      (this->*sem.onScoring)(dl.tr);

      // may report
      if (!(i & 0xf)) {
         printf(".");
         if (i && !(i & freqMiniReport)) {
            MiniReport(countToSolve - i);
         }
      }
   } // boards

}

void Walrus::Progress::Init(uint _step)
{
   step = _step;
   went = 0;
   margin = _step * 4;
}

bool Walrus::Progress::Step()
{
   went += step;
   return (went > margin);
}

void Walrus::Progress::Up(uint idx)
{
   went = 0;
   if (idx > margin * 2) {
      margin = step * 9;
   }
}

void Walrus::ShowProgress(uint idx)
{
   // do reports
   if (progress.Step()) {
      MiniReport(countToSolve - idx);
      progress.Up(idx);
   } else {
      printf(".");
   }
}

void Walrus::InitMiniUI()
{
   CumulativeScore zeroes;

   // take 13 tricks then 9
   DdsTricks tr;
   tr.plainScore = 13;
   (this->*sem.onScoring)(tr);
   tr.plainScore = 9;
   (this->*sem.onScoring)(tr);

   // analyze
   if (cumulScore.ideal > 1200) {// 1200+ => seems playing 3NT
      ui.irBase = 9;
   } else {
      ui.irBase = 10;
   }

   // reset results
   cumulScore = zeroes;
}

void Walrus::MiniUI::Run()
{
   // see interrogation command
   if (_kbhit()) {
      auto inchar = _getch();
      switch (inchar) {
         // just made
         case ' ': irGoal = irBase; break;

         // overs
         case '1': irGoal = irBase + 1; break;
         case '2': irGoal = irBase + 2; break;
         case '3': irGoal = irBase + 3; break;
         case '4': irGoal = irBase + 4; break;

         // down some
         case 'q': irGoal = irBase - 1;  break;
         case 'w': irGoal = irBase - 2;  break;
         case 'e': irGoal = irBase - 3;  break;
         case 'r': irGoal = irBase - 4;  break;
         case 't': irGoal = irBase - 5;  break;
         case 'y': irGoal = irBase - 6;  break;
         case 'u': irGoal = irBase - 7;  break;
         case 'i': irGoal = irBase - 8;  break;

         // exit
         case 'x':
            exitRequested = true;
            break;
      }
      printf("\nSeek %d tricks board...", irGoal);
   }
}


void Walrus::SolveInChunks(deal &dlBase)
{
   boards bo;
#ifdef _DEBUG
   uint step = 20;
#else
   uint step = 100;
#endif // _DEBUG

   // do big chunks, then a tail
   uint i = 0;
   progress.Init(step);
   for (; i+step < countToSolve ; i+=step ) {
      // main work
      SolveOneChunk(dlBase, bo, i, step);
      ShowProgress(i);

      // keep balance on abort
      if (ui.exitRequested) {
         hitsCount[3][0] = countToSolve - i - step;
         i += countToSolve;
      }
   }
   if ( i < countToSolve ) {
      step = countToSolve - i;
      SolveOneChunk(dlBase, bo, i, step);
   }
}

void Walrus::SolveOneChunk(deal &dlBase, boards &bo, uint ofs, uint step)
{
   bo.noOfBoards = (int)step;

   // refill & copy all deals
   for (int i = 0; i < bo.noOfBoards ; i++) {
      DdsDeal dl(dlBase, arrToSolve[ofs + i]);
      bo.deals[i] = dl.dl;
      bo.target[i] = -1;
      bo.solutions[i] = PARAM_SOLUTIONS_DDS;
      bo.mode[i] = 0;
   }

   // solve in parallel
   solvedBoards solved;
   int res = SolveAllBoardsN(bo, solved);
   if (res != RETURN_NO_FAULT) {
      char line[80];
      ErrorMessage(res, line);
      printf("DDS error: %s\n", line);
      _getch();
   }

   // account all, may show on console
   ui.Run();
   for (int handno = 0; handno < solved.noOfBoards; handno++) {
      DdsTricks tr;
      tr.Init(solved.solvedBoard[handno]);

      (this->*sem.onScoring)(tr);
      Orb_Interrogate(ui.irGoal, tr, bo.deals[handno], solved.solvedBoard[handno]);
   }

}

