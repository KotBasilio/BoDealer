/************************************************************
 * Walrus project                                        2024
 * Statitics parts. 
 * With a help of https://chatgpt.com/share/4e3599b7-762b-45c1-8014-1fb58af2dd3b
 *
 ************************************************************/
#define  _CRT_SECURE_NO_WARNINGS

#include "waCrossPlatform.h"
#include HEADER_SLEEP
#include <string.h>
#include "walrus.h"
#include <cmath>
#include "waStatistics.h"
#include HEADER_CURSES

extern char fmtCellFloat[];
extern char fmtCellDouble[];
extern char fmtStatCell[];
extern char fmtCellStr[];

constexpr int MAX_SIZE = 14;  // 0 to 13 tricks

// >>> GPT code start
// Static array to hold frequencies, initialized to zeros
static double FREQ[MAX_SIZE] = {0};

// Function to calculate the total count of frequencies
double calculateTotalCount() {
   double totalCount = 0;

   for (int i = 0; i < MAX_SIZE; ++i) {
      totalCount += FREQ[i];
   }

   return totalCount;
}

// Function to calculate the mean
double calculateMean(double totalCount) {
   double sum = 0;

   for (int i = 0; i < MAX_SIZE; ++i) {
      sum += i * FREQ[i];
   }

   // Prevent division by zero by checking if totalCount is very small
   if (std::fabs(totalCount) < 1e-9) return 0;  // Adjust epsilon as needed
   return sum / totalCount;
}

// Function to calculate the standard deviation
double calculateStandardDeviation(double totalCount, double mean) {
   double sumSquaredDifferences = 0;

   for (int i = 0; i < MAX_SIZE; ++i) {
      sumSquaredDifferences += FREQ[i] * std::pow(i - mean, 2);
   }

   // Prevent division by zero by checking if totalCount is very small
   if (std::fabs(totalCount) < 1e-9) return 0;  // Adjust epsilon as needed
   double variance = sumSquaredDifferences / totalCount;
   return std::sqrt(variance);
}

// Function to calculate kurtosis
double calculateKurtosis(double totalCount, double mean, double sigma) {
   double sumFourthDifferences = 0;

   for (int i = 0; i < MAX_SIZE; ++i) {
      sumFourthDifferences += FREQ[i] * std::pow((i - mean) / sigma, 4);
   }

   // Prevent division by zero by checking if totalCount is very small
   if (std::fabs(totalCount) < 1e-9) return 0;  // Adjust epsilon as needed
   double kurtosis = sumFourthDifferences / totalCount - 3;  // Excess kurtosis
   return kurtosis;
}

// Function to calculate the confidence interval for the mean
void calculateConfidenceInterval(double mean, double sigma, double totalCount, double& lowerBound, double& upperBound) {
   // Choose the critical value corresponding to 90% confidence level (Z-distribution)
   double z = 1.645;  // For 90% confidence level

   // Calculate the standard error of the mean
   double standardError = sigma / std::sqrt(totalCount);

   // Calculate the margin of error
   double marginOfError = z * standardError;

   // Calculate the confidence interval bounds
   lowerBound = mean - marginOfError;
   upperBound = mean + marginOfError;
}

/*  MEANING OF THE CHARASTERISTICS:
* 
* Statistical Measure   Scientific Meaning
* ========================================
* Mean                  The arithmetic mean represents the average value of a dataset. 
*                       It is calculated as the sum of all values divided by the number of values.
* 
* Standard Deviation	   The standard deviation measures the amount of variation or dispersion 
* (Sigma)               of a dataset from its mean. It quantifies the spread of data points around the mean.
*
*                       Data Distribution: In a normal (Gaussian) distribution, the standard deviation can be related 
*                       to the quantiles through the empirical rule, also known as the 68-95-99.7 rule. 
*                       This rule states that:
*                       -- Approximately 68% of the data lies within one standard deviation of the mean (𝜇±𝜎).
*                       -- Approximately 95% lies within two standard deviations (μ±2σ).
*                       -- Approximately 99.7% lies within three standard deviations (μ±3σ).
* 
* Kurtosis              Kurtosis measures the "tailedness" of the probability distribution of a dataset. 
*                       It indicates the extent to which data points in the tails of the distribution 
*                       differ from the norm (normal Gaussian distribution). 
*                       Higher kurtosis indicates heavier tails and potentially more extreme values. 
*                       Negative kurtosis indicates lighter tails than a normal distribution.
* 
*                       Key values:
*                       Kurtosis = 0: Indicates the distribution has tails similar to a normal distribution (mesokurtic).
*                       Kurtosis > 0: Indicates heavier tails than a normal distribution (leptokurtic), suggesting more extreme values.
*                       Kurtosis < 0: Indicates lighter tails than a normal distribution (platykurtic), suggesting fewer extreme values.
* 
*                       In practical terms, a platykurtic distribution does have a more peaked shape compared to a normal distribution. 
*                       This occurs because there are fewer values in the tails of the distribution, concentrating more values around the mean. 
*                       This results in a lower kurtosis value (<0), indicating lighter tails compared to a normal distribution. 
* 
* Confidence Interval   A confidence interval is a type of estimate computed from the statistics of the observed data. 
*                       It provides a range of plausible values for an unknown population parameter (such as the mean or proportion), 
*                       along with a level of confidence that the parameter lies within that range.
*                       For example, if we calculate a 95% confidence interval for the mean of a dataset, 
*                       it means that if we were to take multiple samples and compute confidence intervals 
*                       for each sample, approximately 95% of those intervals would contain the true population mean.
*                       Confidence intervals are essential in statistical inference as they provide a measure 
*                       of uncertainty around an estimated parameter, allowing us to make informed decisions based on sample data.
*/

// >>> GPT code end

static bool IsSmall(double x)
{
   return x < 1e-9;
}


static void ClearFreqs()
{
   for (int i = 0; i < MAX_SIZE; ++i) {
      FREQ[i] = 0;
   }
}

extern char viscr[DDS_HAND_LINES][DDS_FULL_LINE];
extern void ClearViScreen();
extern void SilentViScreen(int count, char scr[][DDS_FULL_LINE]);
constexpr int REL_GRAPH_LINES = 10;
constexpr int REL_GRAPH_SCREEN = DDS_HAND_LINES;

static void DisplayRawData()
{
   // prepare v-screen
   ClearViScreen();
   int off2 = 28;
   sprintf(viscr[0] + off2, "RELATIVE GRAPH");
   auto line = REL_GRAPH_SCREEN - 1;
   viscr[line][0] = 0;
   for (int i = 0; i < MAX_SIZE; ++i) {
      auto loc = viscr[line] + i * 5;
      sprintf(loc, " %02d    ", i);
   }
   line--;

   // get max
   auto top = FREQ[0];
   for (int i = 1; i < MAX_SIZE; ++i) {
      top = max(top, FREQ[i]);
   }
   if (IsSmall(top)) {
      return;
   }

   // display columns
   auto step = top / 10.f;
   for (int i = 0; i < MAX_SIZE; ++i) {
      auto val = FREQ[i];
      if (IsSmall(val)) {
         continue;
      }
      for (int j = line; j > 0; j--, val -= step) {
         auto loc = viscr[j] + i * 5;
         if (val > 1e-9) {
            loc[0] = loc[1] = loc[2] = loc[3] = '*';
         }
      }
   }

   // out
   SilentViScreen(REL_GRAPH_SCREEN, viscr);
}

static void CalcAndDisplayStatistics(char *title)
{
   // calc all stats
   double totalCount = calculateTotalCount();
   if (IsSmall(totalCount)) {
      return;
   }
   double mean = calculateMean(totalCount);
   double sigma = calculateStandardDeviation(totalCount, mean);
   double kurtosis = calculateKurtosis(totalCount, mean, sigma);
   double lowerBound, upperBound;
   calculateConfidenceInterval(mean, sigma, totalCount, lowerBound, upperBound);

   // out
   owl.Silent("%s: Mean = ", title);
   owl.Silent(fmtCellDouble, mean);
   owl.Silent(" +- ");
   owl.Silent(fmtCellDouble, sigma);
   owl.Silent(" --> [");
   owl.Silent(fmtCellDouble, mean - sigma);
   owl.Silent(", ");
   owl.Silent(fmtCellDouble, mean + sigma);
   //owl.Silent(" Conf.Interval(90%): [");
   //owl.Silent(fmtCellDouble, lowerBound);
   //owl.Silent(", ");
   //owl.Silent(fmtCellDouble, upperBound);
   //owl.Silent("] kurtosis=");

   // deliberate on kurtosis
   owl.Silent("], a shape is ");
   if (kurtosis > 4.f) {
      owl.Silent("a confident peak (");
      owl.Silent(fmtCellDouble, kurtosis);
      owl.Silent(")");
   } else if (kurtosis > 3.f) {
      owl.Silent("a very sharp-peak");
   } else if (kurtosis > 2.f) {
      owl.Silent("a sharp-peak");
   } else if (kurtosis > 1.f) {
      owl.Silent("peaky");
   } else if (kurtosis > .5f) {
      owl.Silent("Gauss++");
   } else if (kurtosis > 0.1f) {
      owl.Silent("Gauss+");
   } else if (kurtosis > -0.1f) {
      owl.Silent("Gauss-like");
   } else if (kurtosis > -0.5f) {
      owl.Silent("approaching Gauss");
   } else if (kurtosis > -1.f) {
      owl.Silent("Fudzi-like");
   } else {
      owl.Silent("uncertain");
   }

   owl.Silent("\n\n");
}

void Walrus::AddSetContracts(int idx)
{
   for (int j = 0; j <= ui.farCol; j++) {
      auto tricks = config.primGoal - 1 - j;
      if (tricks >= 0) {
         FREQ[tricks] += progress.hitsCount[idx][j];
      }
   }
}

void Walrus::AddMadeContracts(int idx)
{
   for (int j = 0; j <= ui.farCol; j++) {
      auto tricks = config.primGoal + j;
      if (tricks < MAX_SIZE) {
         FREQ[tricks] += progress.hitsCount[idx][j];
      }
   }
}

void Walrus::AddOverallStats(int idx)
{
   for (int i = IO_ROW_HCP_START; i <= idx; i++) {
      bool down = (bool)(i & 1);
      if (down) {
         AddSetContracts(i);
      } else {
         AddMadeContracts(i);
      }
   }
}

void Walrus::DisplayGraphStatData(int idx)
{

   // ok start printing
   //bool down = (bool)(i & 1);
   //owl.Silent(down ? "(p %2d down): " : "(p %2d make): ", h);

   // print setting line
   double prevSum = 0;
   owl.Silent("( all down): ");
   for (int j = 0; j <= ui.farCol; j++) {
      auto tricks = config.primGoal - 1 - j;
      if (tricks >= 0) {
         owl.Silent(fmtStatCell, FREQ[tricks]);
         prevSum += FREQ[tricks];
      } else {
         owl.Silent(fmtCellStr, "  ");
      }
   }
   owl.Silent("   : ");
   owl.Silent(fmtStatCell, prevSum);
   owl.Silent("\n");

   // print making line
   double sumline = 0;
   owl.Silent("( all make): ");
   for (int j = 0; j <= ui.farCol; j++) {
      auto tricks = config.primGoal + j;
      if (tricks < MAX_SIZE) {
         owl.Silent(fmtStatCell, FREQ[tricks]);
         sumline += FREQ[tricks];
      } else {
         owl.Silent(fmtCellStr, "  ");
      }
   }
   owl.Silent("   : ");
   owl.Silent(fmtStatCell, sumline);

   // add percentage
   owl.Silent("  --> %2.0lf %%", sumline * 100 / (sumline + prevSum));
   owl.Silent("\n\n");

   DisplayRawData();
}



void Walrus::ShowAdvancedStatistics(int idx)
{
   if (!ui.advancedStatistics) {
      return;
   }

   // we get two lines that represent frequences in walrus-format
   //   idx for making contract
   //   idx-1 for going down
   // convert them to frequences
   ClearFreqs();
   AddMadeContracts(idx);
   AddSetContracts(idx - 1);
   if (ui.allStatGraphs) {
      DisplayRawData();
   }
   CalcAndDisplayStatistics("Adv stats");

   // not the last line => done
   auto h = (idx - IO_ROW_HCP_START) / 2 + IO_HCP_MIN;
   if (h < IO_HCP_MAX) {
      return;
   }

   // it's time to display overall
   ClearFreqs();
   AddOverallStats(idx);
   DisplayGraphStatData(idx);
   CalcAndDisplayStatistics("Overall adv stats");
}

