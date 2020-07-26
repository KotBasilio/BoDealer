/************************************************************
 * Walrus project                                        2019
 * Input/output parts
 *
 ************************************************************/
#define  _CRT_SECURE_NO_WARNINGS
#include <Windows.h> // GetModuleFileName
#include "walrus.h"

/*************************************************************
'* Walrus::LoadInitialStatistics()
'*
'* RET : success/fail
'* IN  : fname
'*************************************************************/
bool Walrus::LoadInitialStatistics(const char *fname)
{
   //printf("\nNo initial stats needed\n");
   return true;
}

void Walrus::BuildFileNames(void)
{
   // make path
   char *buf = namesBase.StartFrom;
   size_t size = sizeof(namesBase.StartFrom);
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

   // duplicate
   memcpy(namesBase.Indices, buf, size);
   memcpy(namesBase.Progress, buf, size);
   memcpy(namesBase.Solution, buf, size);

   // make real names
   strcat(namesBase.StartFrom, START_FROM_FNAME);
   strcat(namesBase.Indices, INDICES_FNAME);
   strcat(namesBase.Solution, OUT_FNAME);
   strcat(namesBase.Progress, PROGRESS_FNAME);
}

#ifdef _DEBUG
   char fmtCell[] = "%6u,";
   char tblHat[] =  "    :  let   spade heart  both   club       sum\n";
#else
   char fmtCell[] = "%10u,";
   //char tblHat[] =  "    :       let    spade    heart     both     club             sum\n";
   char tblHat[] =  "    :  HITS COUNTS   :\n";
#endif   

void Walrus::MiniReport(uint toGo)
{
   uint hitsRow[14];
   uint hitsCamp[7];
   for (int j = 0; j < 7; j++) {
      hitsRow[j] = 0;
      hitsCamp[j] = 0;
   }
   printf("\n%s", tblHat);
   for (int i = 0; i < 14; i++) {
      printf("(%2d):  ", i);
      uint sumline = 0;
      for (int j = 0; j < 7; j++) {
         printf(fmtCell, hitsCount[i][j]);
         sumline      += hitsCount[i][j];
         hitsCamp[j]  += hitsCount[i][j];
      }
      printf("%10u\n", sumline);
      hitsRow[i] = sumline;
   }

   if (countToSolve && (toGo == countToSolve)) {
      printf("Solving started:");
      return;
   }

   uint sumCamps = hitsCamp[0]+ hitsCamp[1]+ hitsCamp[2]+ hitsCamp[3]+ hitsCamp[4];
   if (!sumCamps) {
      sumCamps = 1;
   }

   uint sumRows = hitsRow[0] + hitsRow[1];
   if (!sumRows) {
      sumRows = 1;
   }

#ifdef SEEK_BIDDING_LEVEL
   // calc percentages
   float percPartscore = hitsRow[0] * 100.f / sumRows;
   float percGame      = hitsRow[1] * 100.f / sumRows;

   printf("Processed: %u total; %3.1f%% partscore + %3.1f%% game\n",
      sumRows, percPartscore, percGame);
   printf("Averages: ideal = %lld, bidGame = %lld, partscore=%lld\n",
      cumulScore.ideal / sumRows,
      cumulScore.bidGame / sumRows,
      cumulScore.partscore / sumRows);
   #ifdef SCORE_OPP_CONTRACT
     printf("Their contract expectation average: passed = %lld, doubled = %lld\n",
       cumulScore.oppContract / sumRows, 
       cumulScore.oppCtrDoubled / sumRows);
   #endif // SCORE_OPP_CONTRACT
#endif // SEEK_BIDDING_LEVEL

#ifdef SEEK_OPENING_LEAD
   // calc percentages
   float percPartscore = hitsRow[0] * 100.f / sumRows;
   float percGame      = hitsRow[1] * 100.f / sumRows;
   printf("Processed: %u total; %3.1f%% down some + %3.1f%% game\n",
            sumRows, percPartscore, percGame);
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

void Walrus::ReportState(char *header)
{
   uint bookman = countIterations;

   printf("%s", header);
   for (int i = 0; i < HCP_SIZE; i++) {
      uint sumline = 0;
      for (int j = 0; j < CTRL_SIZE; j++) {
         printf(fmtCell, hitsCount[i][j]);
         bookman -= hitsCount[i][j];
         sumline += hitsCount[i][j];
      }
      printf("%10u\n", sumline);
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

