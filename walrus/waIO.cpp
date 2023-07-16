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

// output alignments
char tblHat[]        = "    :  HITS COUNT   :\n";
char tblFiltering[]  = "  (FILTERING)       PARTNER       DIRECT     SANDWICH        TOTAL\n";
char fmtFiltering[]  = "%12llu,";
char fmtCell[]       = "%8llu,";
char fmtCellStr[]    = "%8s,";
char fmtCellFloat[]  = "%8.1f,";
//char tblHat[] =  "    :       let    spade    heart     both     club             sum\n";

const int MAX_CAMPS = 15;
const int MINI_ROWS = 13;

const int PREFIX_LEN = 16;
char miniRowStart[MINI_ROWS][PREFIX_LEN];

Walrus::MiniUI::MiniUI()
   : exitRequested(false)
   , reportRequested(false)
   , firstAutoShow(true)
   , minControls(0)
   , irGoal(0)
   , irBase(0)
   , irFly(0)
   , farCol(CTRL_SIZE)
{
   // init lines in mini-report
   {
      sprintf(miniRowStart[ 0], "       (down): ");
      sprintf(miniRowStart[ 1], "       (make): ");
      sprintf(miniRowStart[ 2], "       (----): ");
   }
   for (int i = 3; i < MINI_ROWS; i++) {
      sprintf(miniRowStart[i], "       (%4d): ", i);
   }

   // may rewrite some lines
   #ifdef SHOW_OPP_RESULTS
   {
      sprintf(miniRowStart[IO_ROW_OUR_DOWN], "    (we down): ");
      sprintf(miniRowStart[IO_ROW_OUR_MADE], "    (we make): ");
      sprintf(miniRowStart[IO_ROW_THEIRS+0], "   (opp down): ");
      sprintf(miniRowStart[IO_ROW_THEIRS+1], "   (opp make): ");
      sprintf(miniRowStart[IO_ROW_THEIRS+2], "   (--------): ");
   }
   #endif
   #ifdef SHOW_OUR_OTHER
   {
      sprintf(miniRowStart[IO_ROW_OUR_DOWN], "    (5d down): ");
      sprintf(miniRowStart[IO_ROW_OUR_MADE], "    (5d make): ");
      sprintf(miniRowStart[IO_ROW_THEIRS+0], "   (3NT down): ");
      sprintf(miniRowStart[IO_ROW_THEIRS+1], "   (3NT make): ");
      sprintf(miniRowStart[IO_ROW_THEIRS+2], "   (--------): ");
   }
   #endif

   #ifdef SHOW_MY_FLY_RESULTS
   {
      sprintf(miniRowStart[IO_ROW_THEIRS + 0], "   (3NT down): ");
      sprintf(miniRowStart[IO_ROW_THEIRS + 1], "   (3NT make): ");
      sprintf(miniRowStart[IO_ROW_THEIRS + 2], "less, =, more: ");
   }
   #endif // SHOW_MY_FLY_RESULTS

   #ifdef IO_DISPLAY_CONTROLS_SPLIT
      minControls = (IO_HCP_MIN * 4) / 10 - 6;
   #endif
}

static bool IsRowSkippable(int i)
{
   // opp res => only middle is skippable
   #if defined(SHOW_OPP_RESULTS) || defined (SHOW_MY_FLY_RESULTS) || defined(SHOW_OUR_OTHER)
      return IO_ROW_OUR_MADE + 1 < i && i < IO_ROW_THEIRS;
   #endif

   // only our results => many are skippable
   return i > IO_ROW_OUR_MADE;
}

// OUT: hitsRow[], hitsCamp[]
void Walrus::ReportDepFilteringResults()
{
   // hat
   printf(tblFiltering);
   int miniCamps = 4;

   // for all rows
   bool showFiltering = false;
   for (int i = IO_ROW_OUR_MADE+2; i < IO_ROW_THEIRS; i++) {
      // ok start printing
      printf(miniRowStart[i]);

      // calc and print one line
      // -- its body
      ucell sumline = 0;
      for (int j = 1; j < miniCamps; j++) {
         printf(fmtFiltering, progress.hitsCount[i][j]);
         sumline += progress.hitsCount[i][j];
      }
      // -- its sum
      printf("%12llu\n", sumline);
   }

   // repeat in case it scrolls
   printf(tblFiltering);
}

void Walrus::ReportMiniFilteringResults()
{
   ReportState("");
}

// OUT: hitsRow[], hitsCamp[]
void Walrus::CalcHitsForMiniReport(ucell * hitsRow, ucell * hitsCamp)
{
   // zero hit sums
   for (int i = 0; i < MINI_ROWS; i++) {
      hitsRow[i] = 0;
   }
   for (int j = 0; j < MAX_CAMPS; j++) {
      hitsCamp[j] = 0;
   }

   // detect optimal camps
   auto miniCamps = MAX_CAMPS / 2;
   for (; miniCamps < MAX_CAMPS; miniCamps++) {
      if (progress.hitsCount[IO_ROW_OUR_DOWN][miniCamps - 1] == 0) {
         #ifdef SHOW_OPP_RESULTS
         if (progress.hitsCount[IO_ROW_THEIRS][miniCamps - 1] == 0) // intended incomplete
         #endif
            break;
      }
   }

   // hat
   printf("\n%s", tblHat);

   // for all rows
   for (int i = 0; i < MINI_ROWS; i++) {
      // maybe we don't need this row already
      bool showRow = !IsRowSkippable(i);

      // ok start printing
      if (showRow) printf( miniRowStart[i] );
      char *fmt = fmtCell;

      // calc and print one line
      // -- its body
      u64 sumline = 0;
      int j = 0;
      for (; j < miniCamps; j++) {
         if (showRow) printf(fmt, progress.hitsCount[i][j]);
         sumline     += progress.hitsCount[i][j];
         hitsCamp[j] += progress.hitsCount[i][j];
      }
      // -- its sum
      if (showRow) printf("%12llu\n", sumline);
      hitsRow[i] = sumline;

      // may add percentages
      #ifdef PERCENTAGES_IN_ANSWER_ROW
         if (i == ANSWER_ROW_IDX) {
            if (!sumline) {
               sumline = 1;
            }
            printf("(  %% ):  ");
            for (int j = 0; j < miniCamps; j++) {
               float percent = progress.hitsCount[i][j] * 100.f / sumline;
               printf(fmtCellFloat, percent);
            }
            printf("\n");
         }
      #endif // PERCENTAGES_IN_ANSWER_ROW
   }
}

static ucell hitsRow[MINI_ROWS];
static ucell hitsCamp[MAX_CAMPS];

void Walrus::MiniReport(ucell toGo)
{
   // small tables
   CalcHitsForMiniReport(hitsRow, hitsCamp);

   // signature
   s64 doneOurs   = (s64)(__max( hitsRow[IO_ROW_OUR_DOWN] + hitsRow[IO_ROW_OUR_MADE  ], 1));
   s64 doneTheirs = (s64)( __max(hitsRow[IO_ROW_THEIRS  ] + hitsRow[IO_ROW_THEIRS + 1], 1));
   printf("Processed: %lld total. %s is on lead. Goal is %d tricks in %s.\n", doneOurs, ui.seatOnLead, ui.irBase, ui.declTrump);

   // other stuff
   ShowBiddingLevel(doneOurs);
   ShowPercentages(doneOurs);
   ShowTheirScore(doneTheirs);
   ShowOptionalReports(doneOurs, doneTheirs);
   if (toGo) {
      printf("Yet more %llu to go:", toGo);
   }
}

void Walrus::ShowPercentages(s64 sumRows)
{
   float percGoDown = hitsRow[IO_ROW_OUR_DOWN] * 100.f / sumRows;
   float percMake = hitsRow[IO_ROW_OUR_MADE] * 100.f / sumRows;
   //printf("Chances: %3.1f%% down some + %3.1f%% make\n", percGoDown, percMake);
   printf("Chance to make = %3.1f%%\n", percMake);
}

void Walrus::ShowBiddingLevel(s64 sumRows)
{
   #if defined(SEEK_BIDDING_LEVEL) || defined(FOUR_HANDS_TASK)
      // slam/game/partscore
      if (ui.irBase < 12) {
         printf("Averages: ideal = %lld, bidGame = %lld",
            cumulScore.ideal / sumRows,
            cumulScore.bidGame / sumRows);
            #ifdef SHOW_PARTSCORE_STATLINE
               printf(", partscore=%lld.   ", cumulScore.partscore / sumRows);
            #else
            printf(".   ");
         #endif 
      } else {
         printf("Averages: ideal = %lld, bidGame = %lld, slam=%lld\n",
            cumulScore.ideal / sumRows,
            cumulScore.bidGame / sumRows,
            cumulScore.bidSlam / sumRows);
      }
   #endif // SEEK_BIDDING_LEVEL
}

void Walrus::ShowTheirScore(s64 doneTheirs)
{
   #ifdef SHOW_OPP_RESULTS
      #ifdef SHOW_OPPS_ON_PASS_ONLY
         printf("Their contract expectation average: %lld.", cumulScore.oppContract / doneTheirs);
      #elif defined(SHOW_OPPS_ON_DOUBLE_ONLY)
         printf("Their contract doubled, expectation average: %lld.", cumulScore.oppCtrDoubled / doneTheirs);
      #else
         printf("Their contract expectation average: passed = %lld, doubled = %lld.",
            cumulScore.oppContract / doneTheirs,
            cumulScore.oppCtrDoubled / doneTheirs);
      #endif 
      printf(" Chance to make = %3.1f%%\n", hitsRow[IO_ROW_THEIRS + 1] * 100.f / doneTheirs);
   #endif 
}

void Walrus::ShowOptionalReports(s64 sumRows, s64 sumOppRows)
{
#ifdef SHOW_OUR_OTHER
   printf("The other contract expectation average = %lld.", cumulScore.ourOther / sumOppRows);
   printf(" Chance to make = %3.1f%%\n", hitsRow[IO_ROW_THEIRS + 1] * 100.f / sumOppRows);
   //printf("Combo-score average for our two contracts = %lld.\n", cumulScore.ourCombo / sumOppRows);
#endif // SHOW_OUR_OTHER

#ifdef SEEK_OPENING_LEAD
   printf("Averages: ideal = %lld, lead Spade = %lld, lead Hearts = %lld, lead Diamonds = %lld, lead Clubs = %lld\n",
      cumulScore.ideal / sumRows,
      cumulScore.leadS / sumRows,
      cumulScore.leadH / sumRows,
      cumulScore.leadD / sumRows,
      cumulScore.leadC / sumRows);
#endif // SEEK_OPENING_LEAD

#ifdef SEMANTIC_KEYCARDS_10_12
   float key0 = progress.hitsCount[0][0] * 100.f / sumRows;
   float key1 = progress.hitsCount[0][1] * 100.f / sumRows;
   float key2 = progress.hitsCount[0][2] * 100.f / sumRows;
   float key3 = progress.hitsCount[0][3] * 100.f / sumRows;
   printf("Keycards: 0->%3.1f%%  1->%3.1f%%  2->%3.1f%%  3->%3.1f%%\n",
      key0, key1, key2, key3);
#endif // SEMANTIC_KEYCARDS_10_12

   // magic fly
#ifdef SHOW_MY_FLY_RESULTS
   ucell sumNT = progress.hitsCount[IO_ROW_MYFLY][IO_CAMP_MORE_NT] +
      progress.hitsCount[IO_ROW_MYFLY][IO_CAMP_SAME_NT];
   ucell sumSuit = progress.hitsCount[IO_ROW_MYFLY][IO_CAMP_PREFER_SUIT];
   sumRows = __max(sumNT + sumSuit, 1);
   float percBetterNT = sumNT * 100.f / sumRows;
   printf("NT is better in: %3.1f%% cases\n", percBetterNT);
#endif // SHOW_MY_FLY_RESULTS

#ifdef IO_DETAILED_REPORT_ON_END
   if (ui.reportRequested) {
      ui.reportRequested = false;
      if (ui.minControls) {
         ShowDetailedReportControls();
      } else {
         ShowDetailedReportHighcards();
      }
   }
#endif
}

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

#ifdef IO_NEED_FULL_TABLE
   #define OUT_BIG_TABLE(fmt, par)   printf(fmt, par)
#else
   #define OUT_BIG_TABLE(fmt, par)   
#endif // IO_NEED_FULL_TABLE

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

   OUT_BIG_TABLE("    : %-14llu\n", sumline);
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

