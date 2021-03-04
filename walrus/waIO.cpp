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

#ifdef _DEBUG
char fmtCell[] = "%6u,";
char fmtCellFloat[] = "%6.1f,";
char tblHat[] = "    :  HITS COUNT   :\n";
#else
char fmtCell[] = "%7u,";
char fmtCellFloat[] = "%7.1f,";
//char tblHat[] =  "    :       let    spade    heart     both     club             sum\n";
char tblHat[] = "    :  HITS COUNT   :\n";
#endif   

const int MAX_CAMPS = 15;
const int MINI_ROWS = 13;

const int PREFIX_LEN = 16;
char miniRowStart[MINI_ROWS][PREFIX_LEN];

/*************************************************************
'* Walrus::LoadInitialStatistics()
'*
'* RET : success/fail
'* IN  : fname
'*************************************************************/
bool Walrus::LoadInitialStatistics(const char *fname)
{
   //printf("\nNo initial stats needed\n");

   // init lines in mini-report
   {
      sprintf(miniRowStart[ 0], "     (down): ");
      sprintf(miniRowStart[ 1], "     (make): ");
      sprintf(miniRowStart[ 2], "     (----): ");
   }
   for (int i = 3; i < MINI_ROWS; i++) {
      sprintf(miniRowStart[i], "     (%4d): ", i);
   }
   #ifdef SHOW_OPP_RESULTS
   {
      sprintf(miniRowStart[ 0], "  (we down): ");
      sprintf(miniRowStart[ 1], "  (we make): ");
      sprintf(miniRowStart[10], "  (op down): ");
      sprintf(miniRowStart[11], "  (op make): ");
      sprintf(miniRowStart[12], "  (-------): ");
      }
   #endif

   return true;
}

void Walrus::BuildFileNames(void)
{
   // make path
   char *buf = namesBase.StartFrom;
   size_t size = sizeof(namesBase.StartFrom);
   #ifdef WIN_DETECT_PATH
      int rl = GetModuleFileName(NULL, buf, (DWORD)size);
      int slashToDel = 2;
      for (int i = rl; --i >= 0;) {
         if (buf[i] == '\\') {
            if (--slashToDel == 0) {
               break;
            }
         }
         buf[i] = 0;
      }
   #else
      buf[0] = 0;
   #endif // WIN_DETECT_PATH

   // duplicate
   memcpy(namesBase.Command, buf, size);
   memcpy(namesBase.Progress, buf, size);
   memcpy(namesBase.Solution, buf, size);

   // make real names
   strcat(namesBase.StartFrom, START_FROM_FNAME);
   strcat(namesBase.Command, COMMAND_FNAME);
   strcat(namesBase.Progress, PROGRESS_FNAME);
   strcat(namesBase.Solution, OUT_FNAME);
}

static bool IsRowSkippable(int i)
{
   // opp res => only middle is skippable
   #ifdef SHOW_OPP_RESULTS
      return IO_ROW_OUR_MADE + 1 < i && i < IO_ROW_THEIRS;
   #endif

   // only our results => many are skippable
   return i > IO_ROW_OUR_MADE;
}

// OUT: hitsRow[], hitsCamp[]
void Walrus::CalcHitsForMiniReport(uint * hitsRow, uint * hitsCamp)
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
      if (hitsCount[IO_ROW_OUR_DOWN][miniCamps - 1] == 0) {
         #ifdef SHOW_OPP_RESULTS
         if (hitsCount[IO_ROW_THEIRS][miniCamps - 1] == 0) // intended incomplete
         #endif // SHOW_OPP_RESULTS
         break;
      }
   }

   // print all rows
   printf("\n%s", tblHat);
   for (int i = 0; i < MINI_ROWS; i++) {
      // maybe we don't need this row already
      bool showRow = true;
      if ( IsRowSkippable(i) ) {
         uint solvedCount = hitsRow[IO_ROW_OUR_DOWN] + hitsRow[IO_ROW_OUR_MADE];
         if (solvedCount > 900) {
            showRow = false;
         }
      }

      // ok start printing
      if (showRow) printf( miniRowStart[i] );

      uint sumline = 0;
      int j = 0;
      for (; j < miniCamps; j++) {
         if (showRow) printf(fmtCell, hitsCount[i][j]);
         sumline     += hitsCount[i][j];
         hitsCamp[j] += hitsCount[i][j];
      }

      if (showRow) printf("%10u\n", sumline);
      hitsRow[i] = sumline;

      #ifdef PERCENTAGES_IN_ANSWER_ROW
         if (i == ANSWER_ROW_IDX) {
            if (!sumline) {
               sumline = 1;
            }
            printf("(  %% ):  ");
            for (int j = 0; j < miniCamps; j++) {
               float percent = hitsCount[i][j] * 100.f / sumline;
               printf(fmtCellFloat, percent);
            }
            printf("\n");
         }
      #endif // PERCENTAGES_IN_ANSWER_ROW
   }
}

void Walrus::MiniReport(uint toGo)
{
   if (countToSolve && (toGo == countToSolve)) {
      printf("Solving started:");
      return;
   }

   // small tables
   uint hitsRow[MINI_ROWS];
   uint hitsCamp[MAX_CAMPS];
   CalcHitsForMiniReport(hitsRow, hitsCamp);

   // percentages
   uint sumCamps = __max( hitsCamp[0]+ hitsCamp[1]+ hitsCamp[2]+ hitsCamp[3]+ hitsCamp[4], 1);
   uint sumRows  = __max( hitsRow[IO_ROW_OUR_DOWN] + hitsRow[IO_ROW_OUR_MADE], 1);
   float percGoDown = hitsRow[IO_ROW_OUR_DOWN] * 100.f / sumRows;
   float percMake = hitsRow[IO_ROW_OUR_MADE] * 100.f / sumRows;
   printf("Processed: %u total. %s is on lead. Goal is %d tricks in %s.\n", sumRows, ui.seatOnLead, ui.irBase, ui.declTrump);

#ifdef SEEK_BIDDING_LEVEL
   // slam/game/partscore
   if (ui.irBase < 12) {
   #ifdef SHOW_OPP_RESULTS
      printf("Averages: ideal = %lld, bidGame = %lld.   ",
         cumulScore.ideal / sumRows,
         cumulScore.bidGame / sumRows);
   #else
      printf("Averages: ideal = %lld, bidGame = %lld, partscore=%lld\n",
         cumulScore.ideal / sumRows,
         cumulScore.bidGame / sumRows,
         cumulScore.partscore / sumRows);
   #endif 
   } else {
      printf("Averages: ideal = %lld, bidGame = %lld, slam=%lld\n",
         cumulScore.ideal / sumRows,
         cumulScore.bidGame / sumRows,
         cumulScore.bidSlam / sumRows);
   }
#endif // SEEK_BIDDING_LEVEL

   printf("Chances: %3.1f%% down some + %3.1f%% make\n", percGoDown, percMake);

#ifdef SHOW_OPP_RESULTS
   // printf("Their contract expectation average: passed = %lld, doubled = %lld\n",
   //    cumulScore.oppContract / sumRows, 
   //    cumulScore.oppCtrDoubled / sumRows);
   uint sumOppRows = __max(hitsRow[IO_ROW_THEIRS] + hitsRow[IO_ROW_THEIRS + 1], 1);
   printf("Their contract doubled, expectation average: %lld. Chance to make = %3.1f%%\n", 
      cumulScore.oppCtrDoubled / sumRows,
      hitsRow[IO_ROW_THEIRS + 1] * 100.f / sumRows);
#endif // SCORE_OPP_CONTRACT

#ifdef SEEK_OPENING_LEAD
   printf("Averages: ideal = %lld, lead Spade = %lld, lead Hearts = %lld, lead Diamonds = %lld, lead Clubs = %lld\n",
      cumulScore.ideal / sumRows,
      cumulScore.leadS / sumRows,
      cumulScore.leadH / sumRows,
      cumulScore.leadD / sumRows,
      cumulScore.leadC / sumRows);
#endif // SEEK_OPENING_LEAD

#ifdef SEMANTIC_KEYCARDS_10_12
   float key0 = hitsCount[0][0] * 100.f / sumRows;
   float key1 = hitsCount[0][1] * 100.f / sumRows;
   float key2 = hitsCount[0][2] * 100.f / sumRows;
   float key3 = hitsCount[0][3] * 100.f / sumRows;
   printf("Keycards: 0->%3.1f%%  1->%3.1f%%  2->%3.1f%%  3->%3.1f%%\n",
      key0, key1, key2, key3);
#endif // SEMANTIC_KEYCARDS_10_12

   if (toGo) {
      printf("Yet more %u to go:", toGo);
   }

}

#ifdef IO_NEED_FULL_TABLE
   #define OUT_BIG_TABLE(fmt, par)   printf(fmt, par)
#else
   #define OUT_BIG_TABLE(fmt, par)   
#endif // IO_NEED_FULL_TABLE

void Walrus::ReportState(char *header)
{
   uint bookman = countIterations + countOppContractMarks;

   OUT_BIG_TABLE("%s", header);
   for (int i = 0; i < HCP_SIZE; i++) {
      uint sumline = 0;
      for (int j = 0; j < CTRL_SIZE; j++) {
         OUT_BIG_TABLE(fmtCell, hitsCount[i][j]);
         bookman -= hitsCount[i][j];
         sumline += hitsCount[i][j];
      }
      OUT_BIG_TABLE("%10u\n", sumline);
   }
   printf("\n\nTotal iterations = %u, balance ", countIterations);
   if (bookman) {
      printf("is broken: ");
      if (bookman < countIterations) {
         printf("%u iterations left no mark\n", bookman);
      } else {
         printf("%u more marks than expected\n", MAXUINT32 - bookman + 1);
      }
   } else {
      printf("is fine\n");
   }

   printf("\n--------------------------------\n%s", header);
   MiniReport(0);
}

