/************************************************************
 * Walrus project                                        2020
 * AI parts
 *
 ************************************************************/

#define  _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include "waDoubleDeal.h"
#include "../dds-develop/examples/hands.h"
#include HEADER_C_LEGACY

#define CMD_GOAL_JUST_MADE  ' '
#define CMD_GOAL_PLUS_1     '1'
#define CMD_GOAL_PLUS_2     '2'
#define CMD_GOAL_PLUS_3     '3'
#define CMD_GOAL_PLUS_4     '4'
#define CMD_GOAL_MINUS_1    'q'
#define CMD_GOAL_MINUS_2    'w'
#define CMD_GOAL_MINUS_3    'e'
#define CMD_GOAL_MINUS_4    'r'
#define CMD_GOAL_MINUS_5    't'
#define CMD_GOAL_MINUS_6    'y'
#define CMD_GOAL_MINUS_7    'u'
#define CMD_GOAL_MINUS_8    'i'
#define CMD_FLY_SAME_NT     '='
#define CMD_FLY_PREFER_SUIT '['
#define CMD_FLY_MORE_NT     ']'
#define CMD_REPORT_ALL      's'
#define CMD_REPORT_ADVANCED 'a'
#define CMD_REPORT_ONLY     'z'
#define CMD_EXIT            'x'

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
}

void WaConfig::SetupSeatsAndTrumps(const CumulativeScore &cs)
{
   // primary
   lens.prim.CheckTheSetup(cs.prima);

   // secondary
   if (!cs.secunda.IsEmpty()) {
      lens.secondary.CheckTheSetup(cs.secunda);
      const char* whos = solve.seekDecisionCompete ? "Their" : "A";
      if (solve.taskType == TTYPE_ONE_SIDED_BIDDING_LEVEL) {
         whos = "Our";
      }
      sprintf(txt.secLongName, "%s contract in %s", whos, lens.secondary.txtTrump);
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
      RecognizeCommands(inchar);

      // check whether we've handled this key
      if (irGoal) {
         owl.Show("\nSeek %d tricks board by %s in %s ", irGoal, config.lens.prim.txtBy, config.lens.prim.txtTrump);
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
      irGoal = config.lens.prim.goal;
      owl.Show(" %d tricks board by %s in %s ", irGoal, config.lens.prim.txtBy, config.lens.prim.txtTrump);
   }
}

void MiniUI::RecognizeCommands(int inchar)
{
   switch (inchar) {
      // primary scorer:
      // -- just made
      case CMD_GOAL_JUST_MADE: irGoal = config.lens.prim.goal; break;
      // -- overtricks
      case CMD_GOAL_PLUS_1: irGoal = config.lens.prim.goal + 1; break;
      case CMD_GOAL_PLUS_2: irGoal = config.lens.prim.goal + 2; break;
      case CMD_GOAL_PLUS_3: irGoal = config.lens.prim.goal + 3; break;
      case CMD_GOAL_PLUS_4: irGoal = config.lens.prim.goal + 4; break;
      // -- down some
      case CMD_GOAL_MINUS_1: irGoal = config.lens.prim.goal - 1;  break;
      case CMD_GOAL_MINUS_2: irGoal = config.lens.prim.goal - 2;  break;
      case CMD_GOAL_MINUS_3: irGoal = config.lens.prim.goal - 3;  break;
      case CMD_GOAL_MINUS_4: irGoal = config.lens.prim.goal - 4;  break;
      case CMD_GOAL_MINUS_5: irGoal = config.lens.prim.goal - 5;  break;
      case CMD_GOAL_MINUS_6: irGoal = config.lens.prim.goal - 6;  break;
      case CMD_GOAL_MINUS_7: irGoal = config.lens.prim.goal - 7;  break;
      case CMD_GOAL_MINUS_8: irGoal = config.lens.prim.goal - 8;  break;

      // secondary TODO

      // fly comparison
      case CMD_FLY_SAME_NT: irGoal = config.lens.prim.goal; irFly = IO_CAMP_SAME_NT;     break;
      case CMD_FLY_PREFER_SUIT: irGoal = config.lens.prim.goal; irFly = IO_CAMP_PREFER_SUIT; break;
      case CMD_FLY_MORE_NT: irGoal = config.lens.prim.goal; irFly = IO_CAMP_MORE_NT;     break;

      // report hits
      case CMD_REPORT_ALL: // report + all graphs
         allStatGraphs = true;
         // no break
      case CMD_REPORT_ADVANCED: // report + one graph
         advancedStatistics = true;
         // no break
      case CMD_REPORT_ONLY: // just a report
         reportRequested = true;
         break;

      // exit
      case CMD_EXIT:
         exitRequested = true;
         owl.Show("\n");
         break;
   }
}
