/************************************************************
 * Walrus project                                        2020
 * AI parts
 *
 ************************************************************/

#define  _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include "../dds-develop/include/dll.h"
#include "../dds-develop/examples/hands.h"
#include "walrus.h"
#include HEADER_CURSES

#define  DBG_SHOW_BOARD_ON_CONSTRUCTION

#ifndef IO_ROW_SELECTED
   #define IO_ROW_SELECTED 3
#endif 

extern int SolveAllBoardsN(boards& bds, solvedBoards& solved);

struct DdsDeal 
{
   // to pass to DDS
   deal dl;

   // result tricks
   DdsTricks tr;

   // viewing
   static bool needInspect;

   DdsDeal(const deal &dlBase, DdsTask2 &task);
   DdsDeal(const deal &dlBase, DdsTask3 &task);
   DdsDeal(twContext* lay);
   void Solve(uint handno);
private:
   void ReconstructNorth(int s)
   {
      dl.remainCards[NORTH][s] = 
      dl.remainCards[EAST ][s] ^
      dl.remainCards[WEST ][s] ^
      dl.remainCards[SOUTH][s] ^ RFULL;
   }

   void ReconstructWest(int s)
   {
      dl.remainCards[WEST ][s] =
      dl.remainCards[EAST ][s] ^
      dl.remainCards[NORTH][s] ^
      dl.remainCards[SOUTH][s] ^ RFULL;
   }

   uint DecryptSpades(SplitBits bits) { return bits.card.w.s.Decrypt(); }
   uint DecryptHearts(SplitBits bits) { return bits.card.w.h.Decrypt(); }
   uint DecryptDiamnd(SplitBits bits) { return bits.card.w.d.Decrypt(); }
   uint DecryptClubs (SplitBits bits) { return bits.card.w.c.Decrypt(); }
};

bool DdsDeal::needInspect = true;

void HandleErrorDDS(deal &cards, int res)
{
   char line[80];
   sprintf(line, "Problem hand on solve: leads %s, trumps: %s\n", haPlayerToStr(cards.first), haTrumpToStr(cards.trump));
   PrintHand(line, cards);
   ErrorMessage(res, line);
   printf("DDS error: %s\n", line);
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

void Walrus::DisplayBoard(twContext* lay)
{
   DdsDeal wad(lay);

   PrintHand("A board: \n", wad.dl);
   PLATFORM_GETCH();
}

void Walrus::SolveOneByOne(deal &dlBase)
{
#ifdef _DEBUG
   uint freqMiniReport = 0x3f;
#else
   uint freqMiniReport = 0xff;
#endif // _DEBUG

   for (uint i = 0; i < mul.countToSolve ; i++) {
      // refill & solve the next deal
      DdsDeal dl(dlBase, mul.arrToSolve[i]);
      dl.Solve(i);

      // pass
      (this->*sem.onScoring)(dl.tr);

      // may report
      if (!(i & 0xf)) {
         printf(".");
         if (i && !(i & freqMiniReport)) {
            MiniReport(mul.countToSolve - i);
         }
      }
   } // boards

}

Progress::Progress()
   : countExtraMarks(0)
   , delta1(0)
   , delta2(0)
{
}

void Progress::Init(ucell _step)
{
   step = _step;
   went = 0;
   margin = _step * 4;
}

bool Progress::Step()
{
   went += step;
   return (went > margin);
}

void Progress::Up(ucell idx)
{
   went = 0;
   if (idx > margin * 2) {
      margin = step * 9;
   }
}

void Walrus::ShowProgress(ucell idx)
{
   // do reports
   if (progress.Step() || ui.reportRequested) {
      MiniReport(mul.countToSolve - idx);
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

   // exactly 1550 => 3NT doubled
   if (cumulScore.ideal == 1550) {
      return 9;
   }

   // 920 etc => at least a small slam
   if (cumulScore.ideal > 900) {
      return 12;
   } 

   // 890 is NV 4M doubled +2
   if (cumulScore.ideal == 890) {
      return 10;
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
          cumulScore.bidGame == 510 ||
          cumulScore.bidGame == 440) {
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
#ifdef SCORE_THE_OTHER_CONTRACT
   strcpy(ui.theirTrump, s_TrumpNames[OC_TRUMPS]);
#endif

   // decl is anti-ccw from leader
   int declSeat = (first + 3) % 4;
   strcpy(ui.declSeat, s_SeatNames[declSeat]);
}

void Walrus::MiniUI::Run()
{
   // see interrogation command
   if (PLATFORM_KBHIT()) {
      irFly = IO_CAMP_OFF;

      auto inchar = PLATFORM_GETCH();
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

         // comparison
         #ifdef SEEK_MAGIC_FLY
            case '=': irGoal = irBase; irFly = IO_CAMP_SAME_NT; break;
            case '[': irGoal = irBase; irFly = IO_CAMP_PREFER_SUIT; break;
            case ']': irGoal = irBase; irFly = IO_CAMP_MORE_NT; break;
         #endif 

         // report hits
         case 'h': 
            reportRequested = true; 
            break;

         // exit
         case 'x':
            exitRequested = true;
            break;
      }

      if (irGoal) {
         printf("\nSeek %d tricks board by %s in %s ", irGoal, declSeat, declTrump);
         switch (irFly) {
            case IO_CAMP_MORE_NT:
               printf("where NT gives more tricks ");
               break;
            case IO_CAMP_SAME_NT:
               printf("where NT gives the same number of tricks ");
               break;
            case IO_CAMP_PREFER_SUIT:
               printf("where NT gives less tricks ");
               break;
         }
         printf("... ");
      } else if (!(exitRequested || reportRequested)) {
         printf("\nCommand '%c' is ignored...", inchar);
      }
   }

   // auto-command
   if (firstAutoShow && !irGoal) {
      irGoal = irBase;
      printf(" %d tricks board by %s in %s ", irGoal, declSeat, declTrump);
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
   for (; i+step < mul.countToSolve ; i+=step ) {
      // main work
      SolveOneChunk(dlBase, bo, i, step);

      // always be ready to show progress
      progress.hitsCount[IO_ROW_SELECTED][0] = mul.countToSolve - i - step;
      ShowProgress(i);

      // fast exit
      if (ui.exitRequested) {
         return;
      }
   }

   // tail work
   if ( i < mul.countToSolve ) {
      step = mul.countToSolve - i;
      SolveOneChunk(dlBase, bo, i, step);
   }
   progress.hitsCount[IO_ROW_SELECTED][0] = 0;
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

bool Walrus::HandleSolvedBoard(DdsTricks &tr, deal &cards)
{
   // cater for unplayable boards -- we change board result on some percentage boards
   #ifdef UNPLAYABLE_ONE_OF
      bool isDecimated = false;
      if (tr.plainScore == ui.irBase) {
         static int cycleCatering = UNPLAYABLE_ONE_OF;
         if (0 == --cycleCatering) {
            tr.plainScore--;
            isDecimated = true;
            cycleCatering = UNPLAYABLE_ONE_OF;
         }
      }
   #endif // UNPLAYABLE_ONE_OF

   // add to basic statistics
   (this->*sem.onScoring)(tr);

   // results are a fake => forget the board asap
   #ifdef UNPLAYABLE_ONE_OF
      if (isDecimated) {
         return false;
      }
   #endif

   // some detailed postmortem is possible
   (this->*sem.onPostmortem)(tr, cards);

   return true;
}

void Walrus::SolveOneChunk(deal& dlBase, boards& bo, uint ofs, uint step)
{
   bo.noOfBoards = (int)step;

   // refill & copy all deals
   for (int i = 0; i < bo.noOfBoards; i++) {
      DdsDeal dl(dlBase, mul.arrToSolve[ofs + i]);
      bo.deals[i] = dl.dl;
      bo.target[i] = -1;
      bo.solutions[i] = PARAM_SOLUTIONS_DDS;
      bo.mode[i] = 0;
   }

   // solve in parallel
   solvedBoards solved;
   int res = SolveAllBoardsN(bo, solved);
   HandleDDSFail(res);

   // read user commands
   ui.Run();

   // relay
   HandleSolvedChunk(bo, solved);
}

void Walrus::HandleSolvedChunk(boards& bo, solvedBoards& solved)
{
   // pass to statistics
   for (int handno = 0; handno < solved.noOfBoards; handno++) {
      futureTricks &fut = solved.solvedBoard[handno];
      DdsTricks tr; tr.Init(fut);
      deal& cards(bo.deals[handno]);
      if (HandleSolvedBoard(tr, cards)) {
         Orb_Interrogate(tr, cards, fut);
      }
   }

   // ensure we do something else with the same set of boards
   #ifdef SOLVE_TWICE_HANDLED_CHUNK
   {
      // solve second time
      solvedBoards twice;
      for (int i = 0; i < bo.noOfBoards; i++) {
         bo.deals[i].trump = TWICE_TRUMPS;
         bo.deals[i].first = TWICE_ON_LEAD_CHUNK;
      }
      int res = SolveAllBoardsN(bo, twice);
      HandleDDSFail(res);

      // score their contract or seek magic fly 
      for (int handno = 0; handno < twice.noOfBoards; handno++) {
         DdsTricks trTw;
         trTw.Init(twice.solvedBoard[handno]);
         (this->*sem.onSolvedTwice)(trTw);

         #ifdef SEEK_MAGIC_FLY
            DdsTricks tr; tr.Init(solved.solvedBoard[handno]);
            NoticeMagicFly(tr.plainScore, trTw.plainScore);
         #endif // SEEK_MAGIC_FLY

         #ifdef THE_OTHER_IS_OURS
            // DdsTricks tr; tr.Init(solved.solvedBoard[handno]);
            // CountComboScore(tr.plainScore, trTw.plainScore);
         #endif // THE_OTHER_IS_OURS
      }
   }
   #endif
}

void Walrus::ShowLiveSigns()
{
   if (mul.ShowLiveSigns(sem.scanCover)) {
      // show accumulation progress
      uint acc = Gathered() + mul.hA->Gathered() + mul.hB->Gathered();
      printf("%d", acc / 1000);

      // consider extension of the search unless we're 95% close
      if (mul.countIterations + ADDITION_STEP_ITERATIONS > mul.countShare &&
         acc < (AIM_TASKS_COUNT * 95) / 100) {
         mul.countShare         += ADDITION_STEP_ITERATIONS;
         mul.hA->mul.countShare += ADDITION_STEP_ITERATIONS;
         mul.hB->mul.countShare += ADDITION_STEP_ITERATIONS;
         printf(",");
      } else {
         printf(".");
      }

      // allow interruption
      if (PLATFORM_KBHIT()) {
         auto inchar = PLATFORM_GETCH();
         printf("X");
         mul.hA->mul.countShare = mul.hA->mul.countIterations + 1000;
         mul.hB->mul.countShare = mul.hB->mul.countIterations + 1000;
         mul.countShare = mul.countIterations;
      }
   }
}
