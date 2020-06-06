/************************************************************
 * Walrus project                                        2019
 * Input/output parts
 *
 ************************************************************/
#define  _CRT_SECURE_NO_WARNINGS
#include <conio.h> // _getch()
#include <Windows.h> // GetModuleFileName
#include <chrono>
#include "walrus.h"

void main(int argc, char *argv[])
{
   printf(TITLE_VERSION" started.  ");
   auto start = std::chrono::high_resolution_clock::now().time_since_epoch();

   Walrus walter;
   if (walter.LoadInitialStatistics(namesBase.StartFrom)) {
      // phase 1
      walter.MainScan();
      auto finish = std::chrono::high_resolution_clock::now().time_since_epoch();
      auto delta1 = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

      // phase 2
      start = finish;
      if (walter.AfterMath()) {
         finish = std::chrono::high_resolution_clock::now().time_since_epoch();
         auto delta2 = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
         walter.ReportState("\nFinal result:\n");
         printf("The search took %llu.%llu + an aftermath %llu.%llu sec.\n"
            , delta1 / 1000, (delta1 % 1000) / 100
            , delta2 / 1000, (delta2 % 1000) / 100);
      } else {
         walter.ReportState("\nEnding with:\n");
         printf("The search is done in %llu.%llu sec.\n", delta1 / 1000, (delta1 % 1000) / 100);
      }

   }
   printf("Press any key.\n");
   //sample_main_PlayBin();
   //sample_main_SolveBoard();
   //sample_main_SolveBoard_S1();
   //sample_main_JK_Solve();
   _getch();
}

/*************************************************************
'* Walrus::LoadInitialStatistics()
'*
'* RET : success/fail
'* IN  : fname
'*************************************************************/
bool Walrus::LoadInitialStatistics(const char *fname)
{
   printf("\nNo initial stats needed\n");
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
   char fmtCell[] = "%5u,";
   char tblHat[] =  "    :  let   spade heart  both   club       sum\n";
#else
   char fmtCell[] = "%9u,";
   //char tblHat[] =  "    :       let    spade    heart     both     club             sum\n";
   char tblHat[] =  "    :  HITS COUNTS   :\n";
#endif   

void Walrus::MiniReport(uint toGo)
{
   uint hitsRow[14];
   uint hitsCamp[6];
   for (int j = 0; j < 6; j++) {
      hitsRow[j] = 0;
      hitsCamp[j] = 0;
   }
   printf("\n%s", tblHat);
   for (int i = 0; i < 14; i++) {
      printf("(%2d):  ", i);
      uint sumline = 0;
      for (int j = 0; j < 6; j++) {
         printf(fmtCell, hitsCount[i][j]);
         sumline      += hitsCount[i][j];
         hitsCamp[j]  += hitsCount[i][j];
      }
      printf("%10u\n", sumline);
      hitsRow[i] = sumline;
   }

   uint sumCamps = hitsCamp[0]+ hitsCamp[1]+ hitsCamp[2]+ hitsCamp[3]+ hitsCamp[4];
   if (!sumCamps) {
      sumCamps = 1;
   }

   uint sumRows = hitsRow[0] + hitsRow[1];
   if (!sumRows) {
      sumRows = 1;
   }

   // calc percentages
   float percPartscore = hitsRow[0] * 100.f / sumRows;
   float percGame      = hitsRow[1] * 100.f / sumRows;

   printf("Processed: %u total; %3.1f%% partscore + %3.1f%% game\n",
      sumRows, percPartscore, percGame);
   printf("Averages: ideal = %llu, bidGame = %lld, partscore=%lld\n",
      cumulScore.ideal / sumRows,
      cumulScore.bidGame / sumRows,
      cumulScore.partscore / sumRows);
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

