/************************************************************
 * Walrus project                                        2019
 * Input/output parts
 *
 ************************************************************/
#define  _CRT_SECURE_NO_WARNINGS

#include "waCrossPlatform.h"
#include HEADER_SLEEP
#include <string.h>
#include "walrus.h"
#include HEADER_CURSES

extern char fmtCell[];
extern char fmtCellStr[];
extern char fmtCellFloat[];
extern char fmtCellShortPercent[];

void Walrus::ReportState()
{
   // tell final report from intermediate
   if (!mul.countToSolve) {
      owl.Show("\nEnding with:\n");
   }

   // normal big table
   ReportAllLines();

   // verbose bookman
   printf("Total iterations = %llu, balance %s\n", mul.countIterations, RegularBalanceCheck() ? "is fine" : "is broken");

   // tailing report
   if (progress.isDoneAll) {
      owl.OnProgress("\n--------------------------------\n");
      ui.reportRequested = true;
      MiniReport(0);
   }
   ReportTimeMem();
}

void Walrus::MiniReport(ucell toGo)
{
   // small tables
   progress.ShowMiniHits();

   // signature
   s64 doneOurs = progress.UpdateDoneStats();
   owl.OnDone("Processed: %lld total. %s is on lead. Goal is %d tricks in %s.\n", 
      doneOurs, config.prim.txtAttacker, config.prim.goal, config.prim.txtTrump);

   // other stuff
   ShowBiddingLevel(doneOurs);
   ShowPercentages(doneOurs);
   ShowTheirScore(progress.doneTheirs);
   ShowOptionalReports(doneOurs, progress.doneTheirs);
   if (toGo) {
      printf("Yet more %llu to go:", toGo);
   }
}

void Walrus::ReportAllLines()
{
   if (progress.isDoneAll) {
      owl.OnProgress("\n");
   }

   // prepare ui
   UpdateFarColumnUI();
   ui.shownDashes = false;
   ui.indent = 0;

   // for all lines
   int i = 0;
   for (; i < IO_ROW_FILTERING; i++) {
      // know sum
      ucell sumline = 0;
      for (int j = 0; j < HITS_COLUMNS_SIZE; j++) {
         sumline += progress.hitsCount[i][j];
      }

      // skip normal empty lines
      if (ConsiderDashOutLine(sumline)) {
         continue;
      }

      // show statistics and summary
      DisplaySolvingStats(i, sumline);
      owl.OnProgress("\n");
      RepeatLineWithPercentages(i, sumline);
   }

   // done all => we've seen filters already
   if (progress.isDoneAll) {
      return;
   }

   // show filtering
   owl.OnProgress("%d:    NORTH     EAST    SOUTH     WEST\n", i-1);
   for (uint ip = 0; ip < sem.vecFilters.size(); ip++) {
      DisplayFilterNumbers(ip);
      HandleFilterLine(ip);
   }
}

void Walrus::UpdateFarColumnUI()
{
   // to trim tailing zeros
   ui.farCol = 1;
   for (int i = 0; i < IO_ROW_FILTERING; i++) {
      for (int j = 0; j < HITS_COLUMNS_SIZE; j++) {
         auto cell = progress.hitsCount[i][j];
         if (cell) {
            if (ui.farCol < j) {
               ui.farCol = j;
            }
         }
      }
   }
}

void Walrus::ReportTimeMem()
{
   // a search usually takes seconds
   if (!progress.solveTime) {
      size_t bsize = NumFiltered() * sizeof(WaTask);
      owl.OnDone("The search is done in %s. Memory consumed: %s.\n", 
         progress.ReadableSearchTime(), mul.SizeToReadable(bsize)
      );
      return;
   }

   // solving is a matter of minutes
   progress.ShowSolvingTime();
}

constexpr int SIGN_INDENT_PLUS_A = 'IynA';
constexpr int SIGN_INDENT_PLUS_B = 'lcxE';
constexpr int SIGN_INDENT_MINUS = 'LdnE';
static bool DetectKeyword(char* name, int key)
{
   return key == *(int *)(name);
}

static char *IndentName(const char *start, const char* name, int indent)
{
   static char tail[40];

   strcpy(tail, start);
   for (int d = 0; d < indent; d++) {
      strcat(tail, "  ");
   }
   strcat(tail, name);
   strcat(tail, "\n");

   return tail;
}

void Walrus::HandleFilterLine(uint fidx)
{
   // access name
   char* name = sem.vecFilters[fidx].name;

   // indent changing lines
   if (sem.IsClosingBracket(fidx)) {
      ui.indent--;
   }
   int effective = ui.indent;
   if (sem.IsOpeningBracket(fidx)) {
      ui.indent++;
   }

   // out with effective indent
   owl.OnProgress(IndentName("  ", name, effective));
}

bool Walrus::ConsiderDashOutLine(ucell sumline)
{
   // ensure zeroes
   if (sumline) {
      return false;
   }

   // dash once
   if (!ui.shownDashes) {
      owl.OnProgress("----\n");
      ui.shownDashes = true;
   }

   // handled 
   return true;
}

void Walrus::RepeatLineWithPercentages(int i, ucell sumline)
{
   // percentages are optional
   if (config.io.showPercentages && i < 20) {
      if (!sumline) {
         sumline = 1;
      }
      owl.OnProgress(" %%: ");
      for (int j = 0; j < ui.farCol; j++) {
         float percent = progress.hitsCount[i][j] * 100.f / sumline;
         owl.OnProgress(fmtCellShortPercent, percent);
      }
      owl.OnProgress("\n");
   }
}

void Walrus::DisplayStatCell(ucell cell)
{
   if (cell <= 1000000) {
      owl.OnProgress(fmtCell, cell);
   } else if (cell <= 10000000) {
      owl.OnProgress(fmtCellStr, ">MLN");
   } else if (cell <= 100000000) {
      owl.OnProgress(fmtCellStr, ">XM");
   } else if (cell <= 1000000000) {
      owl.OnProgress(fmtCellStr, ">XXM");
   } else if (cell <= 10000000000LL) {
      owl.OnProgress(fmtCellStr, ">MLRD");
   } else {
      owl.OnProgress(fmtCellStr, ">XR");
   }
}

void Walrus::DisplaySolvingStats(int i, ucell sumline)
{
   owl.OnProgress("%02d: ", i);
   for (int j = 0; j <= ui.farCol; j++) {
      auto cell = progress.hitsCount[i][j];
      DisplayStatCell(cell);
   }
   owl.OnProgress("  : %-12llu", sumline);
   ui.shownDashes = false;
}

void Walrus::DisplayFilterNumbers(uint ip)
{
   int i = IO_ROW_FILTERING + ip;
   owl.OnProgress("%02d: ", i);
   for (uint reason = NORTH; reason <= WEST; ) {
      DisplayStatCell(progress.PeekByIPR(ip, ++reason));
   }
   owl.OnProgress("  : ");
}

void Walrus::ShowDetailedReportHighcards()
{
   UpdateFarColumnUI();
   owl.Silent("\nA split of board results by HCP from %d to %d:\n", config.postm.minHCP, config.postm.maxHCP);

   // for mid-rows
   u64 prevSum = 0;
   for (int i = IO_ROW_POSTMORTEM; i < IO_ROW_FILTERING - 1; i++) {
      // calc hcp for this row (row = 3 + (hcp - 21) * 2)
      int h = (i - IO_ROW_POSTMORTEM) / 2 + config.postm.minHCP;
      if (h > config.postm.maxHCP) {
         break;
      }

      // ok start printing
      bool down = (bool)(i & 1);
      owl.Silent(down ? "(p %2d down): " : "(p %2d make): ", h);

      // calc and print one line
      // -- its body
      u64 sumline = 0;
      int j = 0;
      for (; j <= ui.farCol; j++) {
         owl.Silent(fmtCell, progress.hitsCount[i][j]);
         sumline += progress.hitsCount[i][j];
      }
      // -- its sum
      owl.Silent("   : ");
      owl.Silent(fmtCell, sumline);
      // -- percentage
      if (sumline && !down) {
         owl.Silent("  --> %2llu %%", sumline * 100 / (sumline + prevSum));
      }
      owl.Silent("\n");

      prevSum = sumline;

      if (!down) {
         ShowAdvancedStatistics(i);
      }
   }

   //PLATFORM_GETCH();
}

void Walrus::ShowDetailedReportCompWithHcp()
{
   owl.Silent(
      "\nA split of comparison results by HCP from %d to %d:"
      "\n              go A     same     go B\n", 
      config.postm.minHCP, config.postm.maxHCP
   );

   // for mid-rows
   for (int i = IO_ROW_POSTMORTEM; i < IO_ROW_FILTERING - 1; i++) {
      // calc hcp for this row (row = 3 + (hcp - 21) * 2)
      int h = (i - IO_ROW_POSTMORTEM) + config.postm.minHCP;
      if (h > config.postm.maxHCP) {
         break;
      }

      // ok start printing
      owl.Silent("(hcp %2d): ", h);

      // calc and print one line
      // -- its body
      u64 sumline = 0;
      for (int j = IO_CAMP_PREFER_PRIMA; j <= IO_CAMP_PREFER_SECUNDA; j++) {
         owl.Silent(fmtCell, progress.hitsCount[i][j]);
         sumline += progress.hitsCount[i][j];
      }
      // -- its sum
      owl.Silent("   : ");
      owl.Silent(fmtCell, sumline);
      // -- percentage
      if (sumline) {
         owl.Silent("  --> %2llu%% to %2llu%%", 
            progress.hitsCount[i][IO_CAMP_PREFER_PRIMA] * 100 / sumline,
            progress.hitsCount[i][IO_CAMP_PREFER_SECUNDA] * 100 / sumline
         );
         auto primaBetterBy = (s64)(progress.hitsCount[i][IO_CAMP_IMPS_PRIMA]) - 
                              (s64)(progress.hitsCount[i][IO_CAMP_IMPS_SECUNDA]);
         float posto = 100.f / sumline;
         owl.Silent(", avg 100 boards give %-3.0f IMPs", primaBetterBy * posto);
      }
      owl.Silent("\n");
   }
}

void Walrus::ShowDetailedReportControls()
{
   UpdateFarColumnUI();
   owl.Silent("\nA split of board results by CONTROLS from %d and up:\n", config.postm.minControls);

   // for mid-rows
   u64 prevSum = 0;
   for (int i = IO_ROW_POSTMORTEM; i < IO_ROW_FILTERING - 1; i++) {
      // calc ctrl for this row (row = start + (ctr - min) * 2)
      auto ctr = (i - IO_ROW_POSTMORTEM) / 2 + config.postm.minControls;
      if (ctr > 12) {
         break;
      }

      // ok start printing
      bool down = (bool)(i & 1);
      owl.Silent(down ? "(c %2d down): " : "(c %2d make): ", ctr);

      // calc and print one line
      // -- its body
      u64 sumline = 0;
      int j = 0;
      for (; j <= ui.farCol; j++) {
         owl.Silent(fmtCell, progress.hitsCount[i][j]);
         sumline += progress.hitsCount[i][j];
      }
      // -- its sum
      owl.Silent("   : ");
      owl.Silent(fmtCell, sumline);
      // -- percentage
      if (sumline && !down) {
         owl.Silent("  --> %2llu %%", sumline * 100 / (sumline + prevSum));
      }
      owl.Silent("\n");

      prevSum = sumline;

      if (down) {
         ShowAdvancedStatistics(i);
      }
   }
}

void Walrus::ShowDetailedReportSuit()
{
   UpdateFarColumnUI();

   // for mid-rows
   u64 prevSum = 0;
   for (int i = IO_ROW_POSTMORTEM; i < IO_ROW_FILTERING - 1; i++) {
      // calc suit hcp for this row (row = start + (hcp) * 2)
      auto hcp = (i - IO_ROW_POSTMORTEM) / 2;
      if (hcp > 10) {
         break;
      }

      // ok start printing
      bool down = (bool)(i & 1);
      owl.Silent(down ? "(p %2d down): " : "(p %2d make): ", hcp);

      // calc and print one line
      // -- its body
      u64 sumline = 0;
      int j = 0;
      for (; j <= ui.farCol; j++) {
         owl.Silent(fmtCell, progress.hitsCount[i][j]);
         sumline += progress.hitsCount[i][j];
      }
      // -- its sum
      owl.Silent("   : ");
      owl.Silent(fmtCell, sumline);
      // -- percentage
      if (sumline && !down) {
         owl.Silent("  --> %2llu %%", sumline * 100 / (sumline + prevSum));
      }
      owl.Silent("\n");

      prevSum = sumline;
   }
}


static const char *s_LeadNames[] = {
   "spade lead",
   "heart lead",
   "diamd lead",
   " club lead"
};

void Walrus::ShowDetailedReportOpLeads()
{
   UpdateFarColumnUI();
   owl.Silent("\nA split of board results by opening leads:\n");

   // for mid-rows
   u64 prevSum = 0;
   for (int i = IO_ROW_POSTMORTEM; i < IO_ROW_FILTERING - 1; i++) {
      // calc hcp for this row (row = 3 + (hcp - 21) * 2)
      int suit = (i - IO_ROW_POSTMORTEM) / 2;
      if (suit > SOL_CLUBS) {
         break;
      }

      // ok start printing
      bool down = (bool)(i & 1);
      owl.Silent(down ? "(%10s down): " : "(%10s make): ", s_LeadNames[suit]);

      // calc and print one line
      // -- its body
      u64 sumline = 0;
      int j = 0;
      for (; j <= ui.farCol; j++) {
         owl.Silent(fmtCell, progress.hitsCount[i][j]);
         sumline += progress.hitsCount[i][j];
      }
      // -- its sum
      owl.Silent("   : ");
      owl.Silent(fmtCell, sumline);
      // -- percentage
      if (sumline && !down) {
         owl.Silent("  --> %2llu %%", sumline * 100 / (sumline + prevSum));
      }
      owl.Silent("\n");

      prevSum = sumline;

      if (!down) {
         ShowAdvancedStatistics(i);
      }
   }
}

