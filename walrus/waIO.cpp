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
      sprintf(miniRowStart[ 0], "      (down): ");
      sprintf(miniRowStart[ 1], "      (make): ");
      sprintf(miniRowStart[ 2], "      (----): ");
   }
   for (int i = 3; i < MINI_ROWS; i++) {
      sprintf(miniRowStart[i], "      (%4d): ", i);
   }

   // may rewrite some lines
   #ifdef SHOW_OPP_RESULTS
   {
      sprintf(miniRowStart[IO_ROW_OUR_DOWN], "   (we down): ");
      sprintf(miniRowStart[IO_ROW_OUR_MADE], "   (we make): ");
      sprintf(miniRowStart[IO_ROW_THEIRS+0], "  (opp down): ");
      sprintf(miniRowStart[IO_ROW_THEIRS+1], "  (opp make): ");
      sprintf(miniRowStart[IO_ROW_THEIRS+2], "  (--------): ");
   }
   #endif

   #ifdef SHOW_MY_FLY_RESULTS
   {
      sprintf(miniRowStart[IO_ROW_THEIRS + 0], "  (3NT down): ");
      sprintf(miniRowStart[IO_ROW_THEIRS + 1], "  (3NT make): ");
      sprintf(miniRowStart[IO_ROW_THEIRS + 2], "  (NT  suit): ");
   }
   #endif // SHOW_MY_FLY_RESULTS

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
   #if defined(SHOW_OPP_RESULTS) || defined (SHOW_MY_FLY_RESULTS)
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
      if (progress.hitsCount[IO_ROW_OUR_DOWN][miniCamps - 1] == 0) {
         #ifdef SHOW_OPP_RESULTS
         if (progress.hitsCount[IO_ROW_THEIRS][miniCamps - 1] == 0) // intended incomplete
         #endif
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
         if (showRow) printf(fmtCell, progress.hitsCount[i][j]);
         sumline     += progress.hitsCount[i][j];
         hitsCamp[j] += progress.hitsCount[i][j];
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
               float percent = progress.hitsCount[i][j] * 100.f / sumline;
               printf(fmtCellFloat, percent);
            }
            printf("\n");
         }
      #endif // PERCENTAGES_IN_ANSWER_ROW
   }
}

void Walrus::MiniReport(uint toGo)
{
   if (mul.countToSolve && (toGo == mul.countToSolve)) {
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

   printf("Chances: %3.1f%% down some + %3.1f%% make\n", percGoDown, percMake);

#ifdef SHOW_OPP_RESULTS
   uint sumOppRows = __max(hitsRow[IO_ROW_THEIRS] + hitsRow[IO_ROW_THEIRS + 1], 1);
   #ifdef SHOW_OPPS_ON_PASS
      printf("Their contract expectation average: passed = %lld, doubled = %lld.",
         cumulScore.oppContract / sumOppRows, 
         cumulScore.oppCtrDoubled / sumOppRows);
   #else
      printf("Their contract doubled, expectation average: %lld.",
         cumulScore.oppCtrDoubled / sumOppRows);
   #endif // SHOW_OPPS_ON_PASS
   printf(" Chance to make = %3.1f%%\n", hitsRow[IO_ROW_THEIRS + 1] * 100.f / sumOppRows);
#endif 

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
   uint sumNT = progress.hitsCount[IO_ROW_MYFLY][IO_CAMP_PREFER_NT];
   uint sumSuit = progress.hitsCount[IO_ROW_MYFLY][IO_CAMP_PREFER_SUIT];
   sumRows = __max(sumNT + sumSuit, 1);
   float percBetterNT = sumNT * 100.f / sumRows;
   printf("NT is better in: %3.1f%% cases\n", percBetterNT);
#endif // SHOW_MY_FLY_RESULTS

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
   uint bookman = mul.countIterations + progress.countOppContractMarks;

   OUT_BIG_TABLE("%s", header);
   for (int i = 0; i < HCP_SIZE; i++) {
      uint sumline = 0;
      for (int j = 0; j < CTRL_SIZE; j++) {
         OUT_BIG_TABLE(fmtCell, progress.hitsCount[i][j]);
         bookman -= progress.hitsCount[i][j];
         sumline += progress.hitsCount[i][j];
      }
      OUT_BIG_TABLE("%10u\n", sumline);
   }
   printf("\n\nTotal iterations = %u, balance ", mul.countIterations);
   if (bookman) {
      printf("is broken: ");
      if (bookman < mul.countIterations) {
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

