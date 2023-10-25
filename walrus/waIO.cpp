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
      printf("The search is done in %llu.%llu sec.\n"
         , delta1 / 1000, (delta1 % 1000) / 100);
      return;
   }

   u64 seconds = delta2 / 1000;
   if (seconds < 100) {
      printf("The search took %llu.%llu sec + an aftermath %llu.%llu sec.\n"
         , delta1 / 1000, (delta1 % 1000) / 100
         , seconds, (delta2 % 1000) / 100);
      return;
   }

   u64 minutes = seconds / 60;
   printf("The search took %llu.%llu sec + an aftermath %llu min %llu sec.\n"
      , delta1 / 1000, (delta1 % 1000) / 100
      , minutes, seconds - minutes * 60);
}

void Walrus::ReportState()
{
   if (progress.delta2 > 0) {
      ReportState("\nFinal result:\n");
   } else {
      ReportState("\nEnding with:\n");
   }
}

#if defined(IO_NEED_FULL_TABLE) || defined (IO_SHOW_MINI_FILTERING)
   #define OUT_BIG_TABLE(fmt, par)   printf(fmt, par)
#else
   #define OUT_BIG_TABLE(fmt, par)   
#endif

void Walrus::ReportState(char* header)
{
   u64 delta1 = progress.delta1;
   u64 delta2 = progress.delta2;
   DetectFarColumn();

   // always act as if printing big table as we do bookman calc
   OUT_BIG_TABLE("%s", header);
   ucell bookman = mul.countIterations + progress.countExtraMarks;
   for (int i = 0; i < HCP_SIZE; i++) {
      ucell sumline = 0;
      for (int j = 0; j < CTRL_SIZE; j++) {
         auto cell = progress.hitsCount[i][j];
         bookman -= cell;
         sumline += cell;
      }
      ReportLine(sumline, i);
   }

   // self-control
   printf("Total iterations = %llu, balance ", mul.countIterations);
   if (bookman) {
      printf("is broken: ");
      if (bookman < mul.countIterations) {
         printf("%llu iterations left no mark\n", bookman);
      } else {
         printf("%llu more marks than expected\n", MAXUINT64 - bookman + 1);
      }
   } else {
      printf("is fine\n");
   }

   // tailing report
   if (delta2 > 0) {
      printf("\n--------------------------------\n");
      ui.reportRequested = true;
      MiniReport(0);
   }
   ReportTime(delta1, delta2);
}

void Walrus::DetectFarColumn()
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

void Walrus::ReportLine(ucell sumline, int i)
{
   static bool shownDashes = false;

   // skip lines filled with zeros
   if (!sumline) {
      if (!shownDashes) {
         OUT_BIG_TABLE("%s", "----\n");
         shownDashes = true;
      }
      return;
   }

   OUT_BIG_TABLE("%02d: ", i);
   for (int j = 0; j <= ui.farCol; j++) {
      auto cell = progress.hitsCount[i][j];
      if        (cell <= 1000000) {
         OUT_BIG_TABLE(fmtCell, cell);
      } else if (cell <= 10000000) {
         OUT_BIG_TABLE(fmtCellStr, ">MLN");
      } else if (cell <= 100000000) {
         OUT_BIG_TABLE(fmtCellStr, ">XM");
      } else if (cell <= 1000000000) {
         OUT_BIG_TABLE(fmtCellStr, ">XXM");
      } else if (cell <= 10000000000LL) {
         OUT_BIG_TABLE(fmtCellStr, ">MLRD");
      } else {
         OUT_BIG_TABLE(fmtCellStr, ">XR");
      }
   }

   OUT_BIG_TABLE("    : %-14llu", sumline);
   if (i >= IO_ROW_FILTERING) {
      uint fidx = i - IO_ROW_FILTERING;
      if (fidx < sem.vecFilters.size()) {
         OUT_BIG_TABLE("  %s", sem.vecFilters[fidx].name);
      }
   }
   OUT_BIG_TABLE("%s", "\n");
   shownDashes = false;

   // may add percentages
   #ifdef PERCENTAGES_IN_ANSWER_ROW
   if (i < 20) {
      if (!sumline) {
         sumline = 1;
      }
      printf(" %%:  ");
      for (int j = 0; j < ui.farCol; j++) {
         float percent = progress.hitsCount[i][j] * 100.f / sumline;
         printf(fmtCellFloat, percent);
      }
      printf("\n");
   }
   #endif
}

void Walrus::ShowDetailedReportHighcards()
{
   DetectFarColumn();

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
      printf(down ? "(p %2d down): " : "(p %2d make): ", h);

      // calc and print one line
      // -- its body
      u64 sumline = 0;
      int j = 0;
      for (; j <= ui.farCol; j++) {
         printf(fmtCell, progress.hitsCount[i][j]);
         sumline += progress.hitsCount[i][j];
      }
      // -- its sum
      printf("   : ");
      printf(fmtCell, sumline);
      // -- percentage
      if (sumline && !down) {
         printf("  --> %2llu %%", sumline * 100 / (sumline + prevSum));
      }
      printf("\n");

      prevSum = sumline;
   }

   //PLATFORM_GETCH();
}

void Walrus::ShowDetailedReportControls()
{
   DetectFarColumn();

   // for mid-rows
   u64 prevSum = 0;
   for (int i = IO_ROW_HCP_START; i < IO_ROW_FILTERING - 1; i++) {
      // calc ctrl for this row (row = start + (ctr - min) * 2)
      auto ctr = (i - IO_ROW_HCP_START) / 2 + ui.minControls;
      if (ctr > 12) {
         break;
      }

      // ok start printing
      bool down = (bool)(i & 1);
      printf(down ? "(c %2d down): " : "(c %2d make): ", ctr);

      // calc and print one line
      // -- its body
      u64 sumline = 0;
      int j = 0;
      for (; j <= ui.farCol; j++) {
         printf(fmtCell, progress.hitsCount[i][j]);
         sumline += progress.hitsCount[i][j];
      }
      // -- its sum
      printf("   : ");
      printf(fmtCell, sumline);
      // -- percentage
      if (sumline && !down) {
         printf("  --> %2llu %%", sumline * 100 / (sumline + prevSum));
      }
      printf("\n");

      prevSum = sumline;
   }
}

