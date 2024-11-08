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
   for (uint i = IO_ROW_FILTERING; i < HITS_LINES_SIZE; i++) {
      for (uint j = 0; j < HITS_COLUMNS_SIZE; j++) {
         sum2 += hitsCount[i][j];
      }
   }
   sum2 -= hitsCount[IO_ROW_SELECTED][0];

   return sum2;
}

void Walrus::ShowProgress(ucell idx)
{
   RegularBalanceCheck();

   // do reports
   if (progress.Step() || ui.reportRequested || ui.exitRequested) {
      MiniReport(NumFiltered() - idx);
      progress.Up(idx);
   } else {
      printf(".");
   }
}

bool Walrus::RegularBalanceCheck()
{
   static ucell goodBook[HITS_LINES_SIZE][HITS_COLUMNS_SIZE];

   // calc bookman
   ucell bookman = mul.countIterations + progress.countExtraMarks;
   for (int i = 0; i < HITS_LINES_SIZE; i++) {
      // calc bookman 
      for (int j = 0; j < HITS_COLUMNS_SIZE; j++) {
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
   for (int i = 0; i < HITS_LINES_SIZE; i++) {
      for (int j = 0; j < HITS_COLUMNS_SIZE; j++) {
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
const int PREFIX_LEN = 18;
char miniRowStart[MINI_ROWS][PREFIX_LEN];

// output alignments
char fmtCell[]      = "%8llu,";
char fmtCellStr[]   = "%8s,";
char fmtStatCell[]  = "%8.0lf,";
char fmtCellFloat[] = "%8.1f,";
char fmtCellShortPercent[] = "%8.0f%%";
char fmtCellDouble[] = "%-.2lf";

// hats
char tblLeads[] = "    :       let    spade    heart     both     club             sum\n";
char tblHat[]   = "    :  HITS COUNT   :\n";

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
}

void MiniUI::AdaptMiniRows(Walrus* wal)
{
   // may rewrite some lines
   if (config.io.showOurOther) {
      sprintf(miniRowStart[IO_ROW_OUR_DOWN],   "(ctrA:%s down): ", config.txt.primaShort);
      sprintf(miniRowStart[IO_ROW_OUR_MADE],   "(ctrA:%s make): ", config.txt.primaShort);
      sprintf(miniRowStart[IO_ROW_OUR_MADE+1], "        (----): ");
      sprintf(miniRowStart[IO_ROW_THEIRS+0],   "(ctrB:%s down): ", config.txt.secundaShort);
      sprintf(miniRowStart[IO_ROW_THEIRS+1],   "(ctrB:%s make): ", config.txt.secundaShort);
      sprintf(miniRowStart[IO_ROW_COMPARISON], "  (A, same, B): ");
   } else if (config.io.showMagicFly) {
      sprintf(miniRowStart[IO_ROW_THEIRS + 0], "   (3NT down): ");
      sprintf(miniRowStart[IO_ROW_THEIRS + 1], "   (3NT make): ");
      sprintf(miniRowStart[IO_ROW_COMPARISON], "less, =, more: ");
   } else if (config.io.showOppResults) {
      sprintf(miniRowStart[IO_ROW_OUR_DOWN], "    (we down): ");
      sprintf(miniRowStart[IO_ROW_OUR_MADE], "    (we make): ");
      sprintf(miniRowStart[IO_ROW_THEIRS+0], "   (opp down): ");
      sprintf(miniRowStart[IO_ROW_THEIRS+1], "   (opp make): ");
   }

   // often we compare contracts
   if (config.io.seekDecisionCompete) {
      sprintf(miniRowStart[IO_ROW_COMPARISON], "(bid/refrain): ");
   }
}

static bool IsRowSkippable(int i)
{
   // opp res => show theirs
   if (config.io.DisplayingOtherContract()) {
      return i > IO_ROW_COMPARISON;
   }

   // fly => include that row
   if (config.io.showMagicFly) {
      return i > IO_ROW_MAGIC_FLY;
   }

   // only our results => shortest result
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
         if (!config.io.showOppResults || (progress.hitsCount[IO_ROW_THEIRS][miniCamps - 1] == 0)) {
            break;
         }
      }
   }

   // hat
   if (!progress.isDoneAll) {
      if (config.postm.Is(WPM_OPENING_LEADS)) {
         printf("\n%s", tblHat);// TODO -- use the other hat. And what do we store for leads?
      } else {
         printf("\n%s", tblHat);
      }
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
      if (config.io.showPercentages && (i == config.io.rowPercentage)) {
         if (!sumline) {
            sumline = 1;
         }
         owl.OnDone("    (      %%): ");
         for (int j = 0; j < miniCamps; j++) {
            float percent = progress.hitsCount[i][j] * 100.f / sumline;
            owl.OnDone(fmtCellShortPercent, percent);
         }
         owl.OnDone("\n");
      }
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
   owl.OnDone("Processed: %lld total. %s is on lead. Goal is %d tricks in %s.\n", doneOurs, config.prim.txtAttacker, config.prim.goal, config.prim.txtTrump);

   // other stuff
   ShowBiddingLevel(doneOurs);
   ShowPercentages(doneOurs);
   ShowTheirScore(doneTheirs);
   ShowOptionalReports(doneOurs, doneTheirs);
   if (toGo) {
      printf("Yet more %llu to go:", toGo);
   }
}

void Walrus::ShowBiddingLevel(s64 sumRows)
{
   // game/partscore
   owl.OnDone("Averages: ");
   if (cumulScore.ideal) {
      owl.OnDone("ideal = % lld, ", cumulScore.ideal / sumRows);
   }
   owl.OnDone("%s = %lld", config.txt.primaShort, cumulScore.bidGame / sumRows);
   if (cumulScore.bidPartscore) {
      owl.OnDone(", partscore=%lld", cumulScore.bidPartscore / sumRows);
   }
   owl.OnDone(".   ");
}

void Walrus::ShowPercentages(s64 sumRows)
{
   float percGoDown = hitsRow[IO_ROW_OUR_DOWN] * 100.f / sumRows;
   float percMake = hitsRow[IO_ROW_OUR_MADE] * 100.f / sumRows;
   owl.OnDone("Chance to make = %3.1f%%. \n", percMake);
}

void Walrus::ShowTheirScore(s64 doneTheirs)
{
   if (config.io.showOppResults) {
      if (config.io.oppsOnlyPassed) {
         owl.OnDone("Their contract expectation average: %lld.", cumulScore.oppContract / doneTheirs);
      } else if (config.io.oppsOnlyDoubled) {
         owl.OnDone("Their contract doubled, expectation average: %lld.", cumulScore.oppCtrDoubled / doneTheirs);
      } else {
         owl.OnDone("Their contract expectation average: passed = %lld, doubled = %lld.",
            cumulScore.oppContract / doneTheirs,
            cumulScore.oppCtrDoubled / doneTheirs);
      }
      owl.OnDone(" Chance to make = %3.1f%%\n", hitsRow[IO_ROW_THEIRS + 1] * 100.f / doneTheirs);
   }
}

void Walrus::ShowOptionalReports(s64 sumRows, s64 sumOppRows)
{
   // averages for opening lead
   if (config.postm.Is(WPM_OPENING_LEADS)) {
      owl.OnDone("Averages: ideal = %lld, lead Spade = %lld, lead Hearts = %lld, lead Diamonds = %lld, lead Clubs = %lld\n",
         cumulScore.ideal / sumRows,
         cumulScore.leadS / sumRows,
         cumulScore.leadH / sumRows,
         cumulScore.leadD / sumRows,
         cumulScore.leadC / sumRows);
   }

   s64 sumBid      = (s64)progress.hitsCount[IO_ROW_COMPARISON][IO_CAMP_PREFER_TO_BID];
   s64 sumSame     = (s64)progress.hitsCount[IO_ROW_COMPARISON][IO_CAMP_NO_DIFF];
   s64 sumRefrain  = (s64)progress.hitsCount[IO_ROW_COMPARISON][IO_CAMP_REFRAIN_BIDDING];
   s64 totalComparisons = __max(sumBid + sumSame + sumRefrain, 1);
   float posto = 100.f / totalComparisons;

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
   if (config.io.showMagicFly) {
      ucell sumNT = progress.hitsCount[IO_ROW_MAGIC_FLY][IO_CAMP_MORE_NT] +
         progress.hitsCount[IO_ROW_MAGIC_FLY][IO_CAMP_SAME_NT];
      ucell sumSuit = progress.hitsCount[IO_ROW_MAGIC_FLY][IO_CAMP_PREFER_SUIT];
      sumRows = __max(sumNT + sumSuit, 1);
      float percBetterNT = sumNT * 100.f / sumRows;
      owl.OnDone("NT is better in: %3.1f%% cases\n", percBetterNT);
   }

   // a bid/refrain decision
   if (config.io.seekDecisionCompete) {
      owl.OnDone("Comparison: favor bidding %3.1f%%; same %3.1f%%; favor defending %3.1f%%\n", 
         sumBid * posto, sumSame * posto, sumRefrain * posto
      );
   }

   // our other contract
   if (config.io.showOurOther) {
      owl.OnDone("%s: avg = %lld; makes in %3.1f%% cases\n",
         config.txt.secundaShort,
         cumulScore.ourOther / sumOppRows,
         hitsRow[IO_ROW_THEIRS + 1] * 100.f / sumOppRows
      );
      owl.OnDone("Comparison: favor %s %3.1f%%; same %3.1f%%; favor %s %3.1f%%\n",
         config.txt.primaShort, sumBid * posto,
         sumSame * posto,
         config.txt.secundaShort, sumRefrain * posto
      );
   }

   // huge match
   if (config.io.showHugeMatch) {
      owl.OnDone("A huge match: %+lld IMPs; about %3.0f IMPs/hub\n",
         ui.primaBetterBy, ui.primaBetterBy * posto
      );
   }

   // list by hcp, controls, etc
   if (ui.reportRequested) {
      ui.reportRequested = false;
      switch (config.postm.Type)
      {
         case WPM_NONE:
            break;
         case WPM_OPENING_LEADS:
            ShowDetailedReportOpLeads();
            break;
         case WPM_HCP_SINGLE_SCORER:
         case WPM_CONTROLS:
            if (config.postm.minControls) {
               ShowDetailedReportControls();
            } else {
               ShowDetailedReportHighcards();
            }
            break;
         case WPM_COMPARISON_WITH_HCP:
            ShowDetailedReportCompWithHcp();
            break;
         case WPM_SUIT:
            ShowDetailedReportSuit();
            break;
      }
      ui.advancedStatistics = false;
      ui.allStatGraphs = false;
   }
}

const char* Progress::TimeToReadable(u64 ms)
{
   static char displayBuf[32];

   u64 seconds = ms / 1000;
   if (seconds < 100) {
      sprintf_s(displayBuf, sizeof(displayBuf), "%llu.%llu sec", seconds, (ms % 1000) / 100);
      return displayBuf;
   } 

   u64 minutes = seconds / 60;
   seconds -= minutes * 60;
   if (minutes < 60) {
      sprintf_s(displayBuf, sizeof(displayBuf), "%llu min %llu sec", minutes, seconds);
      return displayBuf;
   } 

   u64 hours = minutes / 60;
   minutes -= hours * 60;
   sprintf_s(displayBuf, sizeof(displayBuf), "%llu h %llu min", hours, minutes);
   return displayBuf;
}

// --------------------------------------------------------------------
// A progress as adding marks on filtering

void Progress::SelectedMark()
{
   hitsCount[IO_ROW_SELECTED][0]++;
}

void Progress::FilteredOutMark(uint ip, uint reason)
{
   CellByIPR(ip, reason)++;
}

void Progress::FOutExtraMark(uint ip, uint reason)
{
   CellByIPR(ip, reason)++;
   countExtraMarks++;
}

void Progress::RemoveFOutExtraMark(uint ip, uint reason)
{
   CellByIPR(ip, reason)--;
   countExtraMarks--;
}

ucell& Progress::CellByIPR(uint ip, uint reason)
{
   // we use 4 columns (N,E,S,W) of IPR_COMPACTION records
   // then (N,E,S,W) more columns similarly three times, 
   // to get total HITS_COLUMNS_SIZE columns
   // so we get a total of IPR_COMPACTION * 4
   auto lowIP = ip & 0x0F;
   auto IPdiv16 = ip >> 4;
   auto startReason = IPdiv16 << 2;
   return hitsCount[IO_ROW_FILTERING + lowIP][startReason + reason - 1];

   // an old, flat version left for debug: all is linear, IPR_COMPACTION is not used
   //return hitsCount[IO_ROW_FILTERING + ip][reason - 1];
}

// --------------------------------------------------------------------
// A progress as adding marks on solving

void Progress::SolvedExtraMark(uint row, uint col)
{
   hitsCount[row][col]++;
   countExtraMarks++;
}

void Progress::AddImps(uint row, uint col, ucell imps)
{
   hitsCount[row][col] += imps;
   countExtraMarks += imps;
}

void Progress::HitByTricks(uint amount, uint made, uint row, bool isExtraMark)
{
   // get place
   uint camp = 0;
   if (amount >= made) {
      row++;
      camp = amount - made;
   } else {
      camp = made - amount - 1;
   }

   // apply
   hitsCount[row][camp]++;
   if (isExtraMark) {
      countExtraMarks++;
   }
}



