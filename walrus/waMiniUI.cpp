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

Walrus::MiniUI::MiniUI()
   : exitRequested(false)
   , reportRequested(false)
   , firstAutoShow(true)
   , minControls(0)
   , irGoal(0)
   , irFly(0)
   , biddingBetterBy(0)
   , farCol(CTRL_SIZE)
{
   FillMiniRows();

   #ifdef IO_DISPLAY_CONTROLS_SPLIT
      minControls = (IO_HCP_MIN * 4) / 10 - 6;
   #endif
}

void Walrus::InitMiniUI(int trump, int first)
{
   // fill names
   strcpy(ui.declTrump, s_TrumpNames[trump]);
   strcpy(ui.seatOnLead, s_SeatNames[first]);
   if (cfgTask.otherGoal > 0) {
      strcpy(ui.theirTrump, s_TrumpNames[OC_TRUMPS]);
   }

   // decl is anti-ccw from leader
   int declSeat = (first + 3) % 4;
   strcpy(ui.declSeat, s_SeatNames[declSeat]);
}

void Walrus::MiniUI::DisplayBoard(twContext* lay)
{
   DdsDeal wad(lay);

   PrintHand("A board: \n", wad.dl);
   PLATFORM_GETCH();
}

void Walrus::MiniUI::WaitAnyKey()
{
   printf("Any key to continue...");
   PLATFORM_GETCH();
   printf("ok");
}

int Walrus::PokeScorerForTricks()
{
   // take 13 tricks 
   DdsTricks tr;
   tr.plainScore = 13;
   (this->*sem.onScoring)(tr);

   // not a game => some partscore
   if (cumulScore.ideal < 300) {
      if (cumulScore.partscore == 190) {
         return 10; // minor, 4m for example
      }
      return 9; // major, 3M for example
   }

   // exactly 1550 => 3NT doubled
   if (cumulScore.ideal == 1550) {
      return 9;
   }

   // grand slams
   if (cumulScore.ideal >= 1440) {
      return 13;
   }

   // 920 etc => at least a small slam
   if (cumulScore.ideal > 900) {
      return 12;
   } 

   // 890 is NV 4M doubled +3
   if (cumulScore.ideal == 890) {
      return 10;
   }

   // 750 is NV 5m doubled +2
   if (cumulScore.ideal == 750) {
      return 11;
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

int Walrus::PokeOtherScorer()
{
   #ifdef SEEK_MAGIC_FLY
      return 9;
   #endif

   // take 13 tricks 
   DdsTricks tr;
   tr.plainScore = 13;
   (this->*sem.onSolvedTwice)(tr);

   switch (cumulScore.oppContract) {
      case -260: return 9;  // 3M
      case -510: return 10; // 4M NV
      case -640: return 11; // 5m VUL
      case -710: return 10; // 4M VUL
      case -720: return 9;  // 3NT VUL
   }

   switch (cumulScore.oppCtrDoubled) {
      case -930: return 9;   // 3MX
      case -690: return 10;  // 4M NV
      case -890: return 10;  // 4M VUL
      case -1150: return 11; // 5mX
   }

   return 0;
}

void Walrus::MiniUI::Run()
{
   // see interrogation command
   if (PLATFORM_KBHIT()) {
      irFly = IO_CAMP_OFF;

      auto inchar = PLATFORM_GETCH();
      switch (inchar) {
         // just made
         case ' ': irGoal = cfgTask.primGoal; break;

         // overtricks
         case '1': irGoal = cfgTask.primGoal + 1; break;
         case '2': irGoal = cfgTask.primGoal + 2; break;
         case '3': irGoal = cfgTask.primGoal + 3; break;
         case '4': irGoal = cfgTask.primGoal + 4; break;

         // down some
         case 'q': irGoal = cfgTask.primGoal - 1;  break;
         case 'w': irGoal = cfgTask.primGoal - 2;  break;
         case 'e': irGoal = cfgTask.primGoal - 3;  break;
         case 'r': irGoal = cfgTask.primGoal - 4;  break;
         case 't': irGoal = cfgTask.primGoal - 5;  break;
         case 'y': irGoal = cfgTask.primGoal - 6;  break;
         case 'u': irGoal = cfgTask.primGoal - 7;  break;
         case 'i': irGoal = cfgTask.primGoal - 8;  break;

         // comparison
         #ifdef SEEK_MAGIC_FLY
            case '=': irGoal = cfgTask.primGoal; irFly = IO_CAMP_SAME_NT; break;
            case '[': irGoal = cfgTask.primGoal; irFly = IO_CAMP_PREFER_SUIT; break;
            case ']': irGoal = cfgTask.primGoal; irFly = IO_CAMP_MORE_NT; break;
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
      irGoal = cfgTask.primGoal;
      printf(" %d tricks board by %s in %s ", irGoal, declSeat, declTrump);
   }
}

void Walrus::AnnounceSolving()
{
   // how much filtered out
   u64 sum = 0;
   for (int i = 0; i < HCP_SIZE; i++) {
      sum += progress.hitsCount[i][1] + progress.hitsCount[i][2] + progress.hitsCount[i][3];
      sum += progress.hitsCount[i][4] + progress.hitsCount[i][5] + progress.hitsCount[i][6];
   }

   // show filtration results
   if (mul.countToSolve) {
      printf("Passing %u for double-dummy inspection: roughly each 1 of %llu; %llu skipped\n"
         , mul.countToSolve, sum / mul.countToSolve, sum);
      ReportState("");
      //PLATFORM_GETCH();
      printf("Solving started: ");
   }
}

