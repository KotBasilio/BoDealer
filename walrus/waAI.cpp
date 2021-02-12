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

int Walrus::PokeScorerForTricks()
{
   // take 13 tricks 
   DdsTricks tr;
   tr.plainScore = 13;
   (this->*sem.onScoring)(tr);

   // not a game => some partscore
   if (cumulScore.ideal < 300) {
      return 9;
   } 
   
   // 920 etc => at least a small slam
   if (cumulScore.ideal > 900) {
      return 12;
   } 

   // take 9 and analyze sum
   tr.plainScore = 9;
   (this->*sem.onScoring)(tr);

   // made two games => seems playing 3NT
   if (cumulScore.ideal > 1200) {
      return 9;
   } 

   // made one game => seems playing 4M
   if (cumulScore.ideal > 500) {
      // maybe 5M?
      if (cumulScore.bidGame < 420 ||
          cumulScore.bidGame == 510) {
         return 11;
      }
      return 10;
   } 
   
   // so far one case 450 for 5dX+2(750) and 5dX-2(-300)
   return 11;
}

static const char *s_TrumpNames[] = {
   "spades",
   "hearts",
   "diamonds",
   "clubs",
   "notrump"
};
static const char *s_SeatNames[] = {
   "North",
   "East",
   "South",
   "West"
};

void Walrus::InitMiniUI(int trump, int first)
{
   // how many tricks is the base?
   ui.irBase = PokeScorerForTricks();

   // that poking has left some marks in stats
   CleanupStats();

   // fill names
   strcpy(ui.declTrump, s_TrumpNames[trump]);
   strcpy(ui.seatOnLead, s_SeatNames[first]);
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

      if (irGoal) {
         printf("\nSeek %d tricks board...", irGoal);
      } else if (!exitRequested) {
         printf("\nCommand '%c' is ignored...", inchar);
      }
   }

   // auto-command
   if (firstAutoShow && !irGoal) {
      irGoal = irBase;
      printf(" %d tricks board ", irGoal);
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

void Walrus::HandleDDSFail(int res)
{
  if (res == RETURN_NO_FAULT) {
    return;
  }

  char line[80];
  ErrorMessage(res, line);
  printf("DDS error: %s\n", line);
  _getch();
}

void Walrus::HandleSolvedBoard(DdsTricks &tr, deal &cards, futureTricks &fut)
{
   // cater for unplayable boards
   bool intactScore = true;
   #ifdef UNPLAYABLE_ONE_OF
   if (tr.plainScore == ui.irBase) {
      static int cycleCatering = UNPLAYABLE_ONE_OF;
      if (0 == --cycleCatering) {
         tr.plainScore--;
         intactScore = false;
         cycleCatering = UNPLAYABLE_ONE_OF;
      }
   }
   #endif // UNPLAYABLE_ONE_OF

   // score and add to statistic
   (this->*sem.onScoring)(tr);

   // intact => run mini-UI to show boards on console
   if (intactScore) {
      Orb_Interrogate(tr, cards, fut);
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
   HandleDDSFail(res);

   // handle all solved
   ui.Run();
   for (int handno = 0; handno < solved.noOfBoards; handno++) {
      futureTricks &fut = solved.solvedBoard[handno];
      DdsTricks tr; tr.Init(fut);
      HandleSolvedBoard(tr, bo.deals[handno], fut);
   }

   // may score their contract
   #ifdef SCORE_OPP_CONTRACT
   {
     for (int i = 0; i < bo.noOfBoards; i++) {
       bo.deals[i].trump = OC_TRUMPS;
       bo.deals[i].first = OC_ON_LEAD;
     }
     int res = SolveAllBoardsN(bo, solved);
     HandleDDSFail(res);
     for (int handno = 0; handno < solved.noOfBoards; handno++) {
       DdsTricks tr;
       tr.Init(solved.solvedBoard[handno]);
       (this->*sem.onOppContract)(tr);
     }
   }
   #endif // SCORE_OPP_CONTRACT
}

