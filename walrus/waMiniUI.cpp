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
   , advancedStatistics(false)
   , allStatGraphs(false)
   , firstAutoShow(true)
   , irGoal(0)
   , irFly(0)
   , primaBetterBy(0)
   , farCol(HITS_COLUMNS_SIZE)
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
         case 's': 
            allStatGraphs = true; 
            // fall down
         case 'a': 
            advancedStatistics = true; 
            // fall down
         case 'h': 
            reportRequested = true; 
            break;

         // exit
         case 'x':
            exitRequested = true;
            advancedStatistics = true; 
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

void Walrus::SummarizeFiltering()
{
   // ensure have some
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


