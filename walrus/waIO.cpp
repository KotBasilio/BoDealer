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
   ReportTime();
}

void Walrus::ReportAllLines()
{
   // prepare ui
   UpdateFarColumnUI();
   ui.shownDashes = false;
   ui.indent = 0;

   // for all lines
   int i = 0;
   for (; i < IO_ROW_FILTERING; i++) {
      // know sum
      ucell sumline = 0;
      for (int j = 0; j < CTRL_SIZE; j++) {
         sumline += progress.hitsCount[i][j];
      }

      // skip normal empty lines
      if (ConsiderNormalZeroLine(i, sumline)) {
         continue;
      }

      // show statistics and summary
      DisplayStatNumbers(i, sumline);
      owl.OnProgress("\n");
      RepeatLineWithPercentages(i, sumline);
   }

   // done all => we've seen filters already
   if (progress.isDoneAll) {
      return;
   }

   // show filtering
   owl.OnProgress("%d:    NORTH     EAST    SOUTH     WEST\n", i-1);
   for (; i < HCP_SIZE; i++) {
      // know sum
      ucell sumline = 0;
      for (int j = 0; j < CTRL_SIZE; j++) {
         sumline += progress.hitsCount[i][j];
      }

      // show statistics and summary
      DisplayStatNumbers(i, sumline);
      HandleFilterLine(i);
      owl.OnProgress("\n");
   }
}

void Walrus::UpdateFarColumnUI()
{
   // to trim tailing zeros
   ui.farCol = 1;
   for (int i = 0; i < HCP_SIZE; i++) {
      for (int j = 0; j < CTRL_SIZE; j++) {
         auto cell = progress.hitsCount[i][j];
         if (cell) {
            if (ui.farCol < j) {
               ui.farCol = j;
            }
         }
      }
   }
}

void Walrus::ReportTime()
{
   u64 delta1 = progress.delta1;
   u64 delta2 = progress.delta2;

   if (!delta2) {
      owl.OnDone("The search is done in %llu.%llu sec.\n"
         , delta1 / 1000, (delta1 % 1000) / 100);
      return;
   }

   u64 seconds = delta2 / 1000;
   if (seconds < 100) {
      owl.OnDone("The search took %llu.%llu sec + an aftermath %llu.%llu sec.\n"
         , delta1 / 1000, (delta1 % 1000) / 100
         , seconds, (delta2 % 1000) / 100);
      return;
   }

   u64 minutes = seconds / 60;
   owl.OnDone("The search took %llu.%llu sec + an aftermath %llu min %llu sec.\n"
      , delta1 / 1000, (delta1 % 1000) / 100
      , minutes, seconds - minutes * 60);
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

   return tail;
}

bool Walrus::IsFilterLine(int i)
{
   auto start = IO_ROW_FILTERING;
   auto end = start + sem.vecFilters.size();
   return ( start <= i && i < end );
}

void Walrus::HandleFilterLine(int i)
{
   // ensure the line is filtering
   if (!IsFilterLine(i)) {
      return;
   }

   // access name
   uint fidx = i - IO_ROW_FILTERING;
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

bool Walrus::ConsiderNormalZeroLine(int i, ucell sumline)
{
   // ensure zeroes
   if (sumline) {
      return false;
   }

   // ensure normal
   if (IsFilterLine(i)) {
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
   #ifdef PERCENTAGES_IN_ANSWER_ROW
   if (i < 20) {
      if (!sumline) {
         sumline = 1;
      }
      owl.OnProgress(" %%:  ");
      for (int j = 0; j < ui.farCol; j++) {
         float percent = progress.hitsCount[i][j] * 100.f / sumline;
         owl.OnProgress(fmtCellFloat, percent);
      }
      owl.OnProgress("\n");
   }
   #endif
}

void Walrus::DisplayStatNumbers(int i, ucell sumline)
{
   owl.OnProgress("%02d: ", i);
   for (int j = 0; j <= ui.farCol; j++) {
      auto cell = progress.hitsCount[i][j];
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
   if (sumline) {
      owl.OnProgress("  : %-12llu", sumline);
   }
   ui.shownDashes = false;
}

void Walrus::ShowDetailedReportHighcards()
{
   UpdateFarColumnUI();

   // for mid-rows
   u64 prevSum = 0;
   for (int i = IO_ROW_HCP_START; i < IO_ROW_FILTERING - 1; i++) {
      // calc hcp for this row (row = 3 + (hcp - 21) * 2)
      auto h = (i - IO_ROW_HCP_START) / 2 + IO_HCP_MIN;
      if (h > IO_HCP_MAX) {
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

void Walrus::ShowDetailedReportControls()
{
   UpdateFarColumnUI();

   // for mid-rows
   u64 prevSum = 0;
   for (int i = IO_ROW_HCP_START; i < IO_ROW_FILTERING - 1; i++) {
      // calc ctrl for this row (row = start + (ctr - min) * 2)
      auto ctr = (i - IO_ROW_HCP_START) / 2 + config.minControls;
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
   for (int i = IO_ROW_HCP_START; i < IO_ROW_FILTERING - 1; i++) {
      // calc suit hcp for this row (row = start + (hcp) * 2)
      auto hcp = (i - IO_ROW_HCP_START) / 2;
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
