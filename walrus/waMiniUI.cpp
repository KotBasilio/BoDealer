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

MiniUI::MiniUI()
   : exitRequested(false)
   , reportRequested(false)
   , firstAutoShow(true)
   , irGoal(0)
   , irFly(0)
   , primaBetterBy(0)
   , farCol(CTRL_SIZE)
{
   FillMiniRows();
}

void WaConfig::SetupOtherContract()
{
   if (secGoal > 0) {
      strcpy(theirTrump, s_TrumpNames[OC_TRUMPS]);
      const char* whos = "Their";
      #ifdef THE_OTHER_IS_OURS
         whos = "A";
      #elif defined(SEEK_BIDDING_LEVEL)
         whos = "Our";
      #endif
      sprintf(secLongName, "%s contract in %s", whos, theirTrump);
   }
}

void MiniUI::Init(int trump, int first)
{
   // fill names
   strcpy(declTrump, s_TrumpNames[trump]);
   strcpy(seatOnLead, s_SeatNames[first]);

   // declarer is anti-ccw from leader
   int ds = (first + 3) % 4;
   strcpy(declSeat, s_SeatNames[ds]);
}

void MiniUI::DisplayBoard(twContext* lay)
{
   DdsDeal wad(lay);

   PrintHand("A board: \n", wad.dl);
   PLATFORM_GETCH();
}

void MiniUI::WaitAnyKey()
{
   printf("Any key to continue...");
   PLATFORM_GETCH();
   printf("ok");
}

int Walrus::PokeScorerForTricks()
{
   // take 13 tricks 
   uint plainScore = 13;
   (cumulScore.*sem.onScoring)(plainScore);

   // not a game => some partscore
   if (cumulScore.ideal < 300) {
      if (cumulScore.bidPartscore == 270) {// NT, 1NT for example
         return 7; 
      }

      if (cumulScore.bidPartscore == 190) {// a minor, 4m for example
         return 10; 
      }

      // a major, tell 3M from 2M
      plainScore = 8;
      (cumulScore.*sem.onScoring)(plainScore);
      if (cumulScore.bidPartscore == 370) {
         return 8; 
      }

      return 9; 
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
   plainScore = 9;
   (cumulScore.*sem.onScoring)(plainScore);

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
   cumulScore.oppContract = 0;
   cumulScore.oppCtrDoubled = 0;
   cumulScore.bidSlam = 0;

   // take 13 tricks 
   DdsTricks tr;
   tr.plainScore = 13;
   (cumulScore.*sem.onSolvedTwice)(tr.plainScore);

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

   switch (cumulScore.ourOther) {
      case 520: // 3NT NV
      case 720: // 3NT VUL
         return 9;  
      case 1510: // 7M  NV
      case 1520: // 7NT NV
      case 2210: // 7M  V
      case 2220: // 7NT V
         return 13;
   }

   return 0;
}

void MiniUI::Run()
{
   // see interrogation command
   if (PLATFORM_KBHIT()) {
      irFly = IO_CAMP_OFF;

      auto inchar = PLATFORM_GETCH();
      switch (inchar) {
         // just made
         case ' ': irGoal = config.primGoal; break;

         // overtricks
         case '1': irGoal = config.primGoal + 1; break;
         case '2': irGoal = config.primGoal + 2; break;
         case '3': irGoal = config.primGoal + 3; break;
         case '4': irGoal = config.primGoal + 4; break;

         // down some
         case 'q': irGoal = config.primGoal - 1;  break;
         case 'w': irGoal = config.primGoal - 2;  break;
         case 'e': irGoal = config.primGoal - 3;  break;
         case 'r': irGoal = config.primGoal - 4;  break;
         case 't': irGoal = config.primGoal - 5;  break;
         case 'y': irGoal = config.primGoal - 6;  break;
         case 'u': irGoal = config.primGoal - 7;  break;
         case 'i': irGoal = config.primGoal - 8;  break;

         // comparison
         #ifdef SEEK_MAGIC_FLY
            case '=': irGoal = config.primGoal; irFly = IO_CAMP_SAME_NT; break;
            case '[': irGoal = config.primGoal; irFly = IO_CAMP_PREFER_SUIT; break;
            case ']': irGoal = config.primGoal; irFly = IO_CAMP_MORE_NT; break;
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

      // check whether we've handled this key
      if (irGoal) {
         owl.Show("\nSeek %d tricks board by %s in %s ", irGoal, declSeat, declTrump);
         switch (irFly) {
            case IO_CAMP_MORE_NT:
               owl.Show("where NT gives more tricks ");
               break;
            case IO_CAMP_SAME_NT:
               owl.Show("where NT gives the same number of tricks ");
               break;
            case IO_CAMP_PREFER_SUIT:
               owl.Show("where NT gives less tricks ");
               break;
         }
         owl.Show("... ");
      } else if (exitRequested || reportRequested) {
         // silent ok
      } else {
         printf("\nCommand '%c' is ignored...", inchar);
      }
   }

   // auto-command
   if (firstAutoShow && !irGoal) {
      irGoal = config.primGoal;
      owl.Show(" %d tricks board by %s in %s ", irGoal, declSeat, declTrump);
   }
}

void Walrus::AnnounceSolving()
{
   if (!NumFiltered()) {
      return;
   }

   // show filtration results
   ucell discarded = progress.GetDiscardedBoardsCount();
   printf("Passing %u for double-dummy inspection. %llu boards are skipped. A pick rate is 1 to %llu\n"
      , NumFiltered(), discarded, discarded / NumFiltered());
   ReportState();
   RegularBalanceCheck();

   printf("Solving started: ");
   //PLATFORM_GETCH();
}


