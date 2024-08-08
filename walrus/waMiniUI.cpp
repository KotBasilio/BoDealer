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

void WaConfig::Contract::Init(const CumulativeScore::LineScorer& scorer)
{
   trump = scorer.Trump();
   goal = scorer.Goal();
   by = scorer.Decl();
   first = (by + 1) % 4;
}

void WaConfig::SetupSeatsAndTrumps(const CumulativeScore &cs)
{
   // primary
   prim.Init(cs.prima);
   strcpy(declTrump,  s_TrumpNames[prim.trump]);
   strcpy(seatOnLead, s_SeatNames [prim.first]);
   strcpy(declSeat,   s_SeatNames [prim.by]);

   // secondary
   if (!cs.secunda.IsEmpty()) {
      secondary.Init(cs.secunda);
      strcpy(theirTrump, s_TrumpNames[secondary.trump]);
      const char* whos = "Their";
      #ifdef THE_OTHER_IS_OURS
         whos = "A";
      #elif defined(SEEK_BIDDING_LEVEL)
         whos = "Our";
      #endif
      sprintf(secLongName, "%s contract in %s", whos, theirTrump);
   }
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
         // primary:
         // -- just made
         case ' ': irGoal = config.prim.goal; break;
         // -- overtricks
         case '1': irGoal = config.prim.goal + 1; break;
         case '2': irGoal = config.prim.goal + 2; break;
         case '3': irGoal = config.prim.goal + 3; break;
         case '4': irGoal = config.prim.goal + 4; break;
         // -- down some
         case 'q': irGoal = config.prim.goal - 1;  break;
         case 'w': irGoal = config.prim.goal - 2;  break;
         case 'e': irGoal = config.prim.goal - 3;  break;
         case 'r': irGoal = config.prim.goal - 4;  break;
         case 't': irGoal = config.prim.goal - 5;  break;
         case 'y': irGoal = config.prim.goal - 6;  break;
         case 'u': irGoal = config.prim.goal - 7;  break;
         case 'i': irGoal = config.prim.goal - 8;  break;

         // secondary TODO

         // fly comparison
         case '=': irGoal = config.prim.goal; irFly = IO_CAMP_SAME_NT;     break;
         case '[': irGoal = config.prim.goal; irFly = IO_CAMP_PREFER_SUIT; break;
         case ']': irGoal = config.prim.goal; irFly = IO_CAMP_MORE_NT;     break;

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
         owl.Show("\nSeek %d tricks board by %s in %s ", irGoal, config.declSeat, config.declTrump);
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
      irGoal = config.prim.goal;
      owl.Show(" %d tricks board by %s in %s ", irGoal, config.declSeat, config.declTrump);
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
