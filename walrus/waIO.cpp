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

static void ReportTime(u64 delta1, u64 delta2)
{
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

void Walrus::ReportState(char* header)
{
   if (!header) {
      owl.Show("\nEnding with:\n");
   } else {
      printf(header);
   }

   ReportAllLines();

   // verbose bookman
   printf("Total iterations = %llu, balance %s\n", mul.countIterations, RegularBalanceCheck() ? "is fine" : "is broken");

   // tailing report
   if (progress.isDoneAll) {
      owl.OnProgress("\n--------------------------------\n");
      ui.reportRequested = true;
      MiniReport(0);
   }
   ReportTime(progress.delta1, progress.delta2);
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

constexpr int SIGN_INDENT_PLUS_A = 'IynA';
constexpr int SIGN_INDENT_PLUS_B = 'lcxE';
constexpr int SIGN_INDENT_MINUS = 'LdnE';

bool Walrus::HandleFilterLine(int i, ucell sumline, int& indent)
{
   // normal line => may write dashes
   if (i < IO_ROW_FILTERING || IO_ROW_FILTERING + sem.vecFilters.size() <= i) {
      if (!sumline) {
         owl.OnProgress("----\n");
      }
      return true;
   }

   // access name
   uint fidx = i - IO_ROW_FILTERING;
   char* name = sem.vecFilters[fidx].name;

   // filled filter line - show with indent
   if (sumline) {
      owl.OnProgress(IndentName("  ", name, indent));
      if (DetectKeyword(name, SIGN_INDENT_PLUS_B)) {
         indent++;
      }
      return false;
   }

   // empty line with filter -- may change indent
   if (DetectKeyword(name, SIGN_INDENT_MINUS)) {
      indent--;
      return false;
   }
   if (DetectKeyword(name, SIGN_INDENT_PLUS_A) || DetectKeyword(name, SIGN_INDENT_PLUS_B)) {
      indent++;
   }

   // out neutral or indent-adding
   owl.OnProgress("%02d:\t\t\t\t\t\t\t\t%s\n", i, IndentName(" ", name, indent));
   return true;
}


void Walrus::ReportAllLines()
{
   UpdateFarColumnUI();

   bool shownDashes = false;
   int  indent = 0;
   for (int i = 0; i < HCP_SIZE; i++) {
      // calc bookman 
      ucell sumline = 0;
      for (int j = 0; j < CTRL_SIZE; j++) {
         sumline += progress.hitsCount[i][j];
      }

      // skip lines filled with zeros
      if (!sumline) {
         if (!shownDashes) {
            shownDashes = HandleFilterLine(i, sumline, indent);
         }
         continue;
      }

      // show numbers in line
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

      // show summary
      owl.OnProgress("  : %-12llu", sumline);
      HandleFilterLine(i, sumline, indent);
      owl.OnProgress("\n");
      shownDashes = false;

      // may add percentages
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
