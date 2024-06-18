/************************************************************
* Walrus project                                        2019
* Showing progress
*
************************************************************/
#define  _CRT_SECURE_NO_WARNINGS

#include "waCrossPlatform.h"
#include HEADER_SLEEP
#include <string.h>
#include "walrus.h"
#include HEADER_CURSES

Progress *mainProgress;

Progress::Progress()
   : countExtraMarks(0)
   , step(42), went(42), margin(42)
   , delta1(0)
   , delta2(0)
   , isDoneAll(false)
{
}

void Progress::Init(ucell _step)
{
   step = _step;
   went = 0;
   margin = _step * 4;
}

bool Progress::Step()
{
   went += step;
   return (went > margin);
}

void Progress::Up(ucell idx)
{
   went = 0;
   if (idx > margin * 2) {
      margin = step * 9;
   }
}

void Progress::StoreCountToGo(ucell count)
{
   hitsCount[IO_ROW_SELECTED][0] = count;
}

ucell Progress::GetDiscardedBoardsCount()
{
   u64 sum2 = 0;
   for (uint i = IO_ROW_FILTERING; i < IO_ROW_FILTERING + config.countFilters; i++) {
      sum2 += hitsCount[i][1] + hitsCount[i][2] + hitsCount[i][3];
      sum2 += hitsCount[i][4] + hitsCount[i][5] + hitsCount[i][6];
   }
   return sum2;
}

void Walrus::ShowProgress(ucell idx)
{
   // do reports
   if (progress.Step() || ui.reportRequested) {
      MiniReport(NumFiltered() - idx);
      progress.Up(idx);
   } else {
      printf(".");
      RegularBalanceCheck();
   }
}

bool Walrus::RegularBalanceCheck()
{
   static ucell goodBook[HCP_SIZE][CTRL_SIZE];

   // calc bookman
   ucell bookman = mul.countIterations + progress.countExtraMarks;
   for (int i = 0; i < HCP_SIZE; i++) {
      // calc bookman 
      for (int j = 0; j < CTRL_SIZE; j++) {
         auto cell = progress.hitsCount[i][j];
         bookman -= cell;
      }
   }

   // no bookman is great
   if (!bookman) {
      memcpy(goodBook, progress.hitsCount, sizeof(goodBook));
      return true;
   }

   // one more normal case is if (bookman == NumFiltered())
   // we can use it for more paranoid checks
    
   // announce
   owl.Show("\n>>>>>>>>>>> Balance is broken: ");
   if (bookman < mul.countIterations) {
      owl.Show("%llu iterations left no mark\n", bookman);
   } else {
      owl.Show("%llu more marks than expected\n", MAXUINT64 - bookman + 1);
   }

   // go to details
   for (int i = 0; i < HCP_SIZE; i++) {
      for (int j = 0; j < CTRL_SIZE; j++) {
         auto cell = progress.hitsCount[i][j];
         auto good = goodBook[i][j];
         if (cell != good) {
            owl.Show("row %d camp %d: good=%llu bad=%llu\n", i, j, good, cell);
         }
      }
   }

   owl.Show("Debug pls.\n");
   //PLATFORM_GETCH();
   owl.Show("OK whatever...\n");

   return false;
}

// ------------------------------------------------------------------------------------------
const int MAX_CAMPS = 15;
const int MINI_ROWS = 13;
const int PREFIX_LEN = 16;
char miniRowStart[MINI_ROWS][PREFIX_LEN];

// output alignments
char tblHat[]       = "    :  HITS COUNT   :\n";
char fmtCell[]      = "%8llu,";
char fmtCellStr[]   = "%8s,";
char fmtCellFloat[] = "%8.1f,";
//char tblHat[] =  "    :       let    spade    heart     both     club             sum\n";

void MiniUI::FillMiniRows()
{
   // init lines in mini-report
   {
      sprintf(miniRowStart[0], "       (down): ");
      sprintf(miniRowStart[1], "       (make): ");
      sprintf(miniRowStart[2], "       (----): ");
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
   }
   #endif

   #ifdef SHOW_SACRIFICE_RESULTS
   {
      sprintf(miniRowStart[IO_ROW_THEIRS+2], "(bid/refrain): ");
   }
   #endif 

   #ifdef SHOW_OUR_OTHER
   {
      sprintf(miniRowStart[IO_ROW_OUR_DOWN], "  (ctrA down): ");
      sprintf(miniRowStart[IO_ROW_OUR_MADE], "  (ctrA make): ");
      sprintf(miniRowStart[IO_ROW_THEIRS+0], "  (ctrB down): ");
      sprintf(miniRowStart[IO_ROW_THEIRS+1], "  (ctrB make): ");
   }
   #endif

   #ifdef SHOW_MY_FLY_RESULTS
   {
      sprintf(miniRowStart[IO_ROW_THEIRS + 0], "   (3NT down): ");
      sprintf(miniRowStart[IO_ROW_THEIRS + 1], "   (3NT make): ");
      sprintf(miniRowStart[IO_ROW_THEIRS + 2], "less, =, more: ");
   }
   #endif
}

static bool IsRowSkippable(int i)
{
   // fly => biggest result
   #if defined(SHOW_MY_FLY_RESULTS) || defined(SHOW_SACRIFICE_RESULTS)
      return i > IO_ROW_MAGIC_FLY;
   #endif

   // opp res => show theirs
   #if defined(SHOW_OPP_RESULTS) || defined(SHOW_OUR_OTHER)
      return i > IO_ROW_THEIRS + 1;
   #endif

   // only our results => many are skippable
   return i > IO_ROW_OUR_MADE;
}


void Walrus::ShowMiniHits(ucell * hitsRow, ucell * hitsCamp) // OUT: hitsRow[], hitsCamp[]
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
   if (!progress.isDoneAll) {
      printf("\n%s", tblHat);
   }

   // for all rows
   for (int i = 0; i < MINI_ROWS; i++) {
      // maybe we don't need this row already
      bool showRow = !IsRowSkippable(i);

      // ok start printing
      if (showRow) owl.OnDone( miniRowStart[i] );
      char *fmt = fmtCell;

      // calc and print one line
      // -- its body
      u64 sumline = 0;
      int j = 0;
      for (; j < miniCamps; j++) {
         if (showRow) owl.OnDone(fmt, progress.hitsCount[i][j]);
         sumline     += progress.hitsCount[i][j];
         hitsCamp[j] += progress.hitsCount[i][j];
      }
      // -- its sum
      if (showRow) owl.OnDone("%12llu\n", sumline);
      hitsRow[i] = sumline;

      // may add percentages
      #ifdef PERCENTAGES_IN_ANSWER_ROW
      if (i == ANSWER_ROW_IDX) {
         if (!sumline) {
            sumline = 1;
         }
         owl.OnDone("(  %% ):  ");
         for (int j = 0; j < miniCamps; j++) {
            float percent = progress.hitsCount[i][j] * 100.f / sumline;
            owl.OnDone(fmtCellFloat, percent);
         }
         owl.OnDone("\n");
      }
      #endif // PERCENTAGES_IN_ANSWER_ROW
   }
}

static ucell hitsRow[MINI_ROWS];
static ucell hitsCamp[MAX_CAMPS];

void Walrus::MiniReport(ucell toGo)
{
   // small tables
   ShowMiniHits(hitsRow, hitsCamp);

   // signature
   s64 doneOurs   = (s64)(__max( hitsRow[IO_ROW_OUR_DOWN] + hitsRow[IO_ROW_OUR_MADE  ], 1));
   s64 doneTheirs = (s64)( __max(hitsRow[IO_ROW_THEIRS  ] + hitsRow[IO_ROW_THEIRS + 1], 1));
   owl.OnDone("Processed: %lld total. %s is on lead. Goal is %d tricks in %s.\n", doneOurs, ui.seatOnLead, config.primGoal, ui.declTrump);

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
   owl.OnDone("Chance to make = %3.1f%%\n", percMake);
}

void Walrus::ShowBiddingLevel(s64 sumRows)
{
   #if defined(SEEK_BIDDING_LEVEL) || defined(SEEK_DENOMINATION) || defined(FOUR_HANDS_TASK)
      // slam/game/partscore
      if (config.primGoal < 12) {
         owl.OnDone("Averages: ideal = %lld, bidGame = %lld",
            cumulScore.ideal / sumRows,
            cumulScore.bidGame / sumRows);
            #ifdef SHOW_PARTSCORE_STATLINE
               owl.OnDone(", partscore=%lld.   ", cumulScore.bidPartscore / sumRows);
            #else
         owl.OnDone(".   ");
         #endif 
      } else {
         owl.OnDone("Averages: ideal = %lld, bidGame = %lld, slam=%lld; ",
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
         owl.OnDone("Their contract expectation average: %lld.", cumulScore.oppContract / doneTheirs);
      #elif defined(SHOW_OPPS_ON_DOUBLE_ONLY)
         owl.OnDone("Their contract doubled, expectation average: %lld.", cumulScore.oppCtrDoubled / doneTheirs);
      #else
         owl.OnDone("Their contract expectation average: passed = %lld, doubled = %lld.",
            cumulScore.oppContract / doneTheirs,
            cumulScore.oppCtrDoubled / doneTheirs);
      #endif 
         owl.OnDone(" Chance to make = %3.1f%%\n", hitsRow[IO_ROW_THEIRS + 1] * 100.f / doneTheirs);
   #endif 
}

void Walrus::ShowOptionalReports(s64 sumRows, s64 sumOppRows)
{
   // our other contract
   #ifdef SHOW_OUR_OTHER
      owl.OnDone("The other contract: avg = %lld; ", cumulScore.ourOther / sumOppRows);
      s64 sumBid = (s64)progress.hitsCount[IO_ROW_COMPARISON][IO_CAMP_PREFER_TO_BID];
      s64 sumRef = (s64)progress.hitsCount[IO_ROW_COMPARISON][IO_CAMP_REFRAIN_BIDDING] + 
                   (s64)progress.hitsCount[IO_ROW_COMPARISON][IO_CAMP_NO_DIFF];
      s64 totalComparisons = __max(sumBid + sumRef, 1);
      float percBetterBid = sumBid * 100.f / totalComparisons;
      owl.OnDone("makes in %3.1f%% cases; superior in %3.1f%% cases\n", 
         hitsRow[IO_ROW_THEIRS + 1] * 100.f / sumOppRows, percBetterBid);
         // todo ui.primaBetterBy / totalComparisons
   #endif

   // averages for opening lead
   #ifdef SEEK_OPENING_LEAD
      owl.OnDone("Averages: ideal = %lld, lead Spade = %lld, lead Hearts = %lld, lead Diamonds = %lld, lead Clubs = %lld\n",
         cumulScore.ideal / sumRows,
         cumulScore.leadS / sumRows,
         cumulScore.leadH / sumRows,
         cumulScore.leadD / sumRows,
         cumulScore.leadC / sumRows);
   #endif // SEEK_OPENING_LEAD

   // keycards split
   #ifdef SEMANTIC_KEYCARDS_10_12
      float key0 = progress.hitsCount[0][0] * 100.f / sumRows;
      float key1 = progress.hitsCount[0][1] * 100.f / sumRows;
      float key2 = progress.hitsCount[0][2] * 100.f / sumRows;
      float key3 = progress.hitsCount[0][3] * 100.f / sumRows;
      owl.OnDone("Keycards: 0->%3.1f%%  1->%3.1f%%  2->%3.1f%%  3->%3.1f%%\n",
         key0, key1, key2, key3);
   #endif

   // a magic fly
   #ifdef SHOW_MY_FLY_RESULTS
      ucell sumNT =   progress.hitsCount[IO_ROW_MAGIC_FLY][IO_CAMP_MORE_NT] +
                      progress.hitsCount[IO_ROW_MAGIC_FLY][IO_CAMP_SAME_NT];
      ucell sumSuit = progress.hitsCount[IO_ROW_MAGIC_FLY][IO_CAMP_PREFER_SUIT];
      sumRows = __max(sumNT + sumSuit, 1);
      float percBetterNT = sumNT * 100.f / sumRows;
      owl.OnDone("NT is better in: %3.1f%% cases\n", percBetterNT);
   #endif

   // a sacrifice
   #ifdef SHOW_SACRIFICE_RESULTS
      s64 sumBid  = (s64)progress.hitsCount[IO_ROW_SACRIFICE][IO_CAMP_PREFER_TO_BID];
      s64 sumDefd = (s64)progress.hitsCount[IO_ROW_SACRIFICE][IO_CAMP_REFRAIN_BIDDING];
      sumRows = __max(sumBid + sumDefd, 1);
      float percBetterBid = sumBid * 100.f / sumRows;
      owl.OnDone("To bid is better in: %3.1f%% cases\n", percBetterBid);
   #endif

   // list by hcp, controls, etc
   if (ui.reportRequested) {
      ui.reportRequested = false;
      switch (config.detailedReportType)
      {
         case WREPORT_NONE:
            break;
         case WREPORT_HCP:
         case WREPORT_CONTROLS:
            if (config.minControls) {
               ShowDetailedReportControls();
            } else {
               ShowDetailedReportHighcards();
            }
            break;
         case WREPORT_SUIT:
            ShowDetailedReportSuit();
            break;
      }
   }
}
