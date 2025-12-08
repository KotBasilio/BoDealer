/************************************************************
* Walrus project                                        2023
* Config reader
*
* TODO: https://docs.github.com/en/copilot/how-tos/configure-custom-instructions/add-repository-instructions
* Copilot Chat          Repository-wide instructions (using the .github/copilot-instructions.md file).
* Copilot coding agent  Repository-wide instructions, path-specific instructions, and agent instructions (using AGENTS.md, CLAUDE.md or GEMINI.md files).
* Copilot code review   Repository-wide instructions and path-specific instructions (using .github/instructions/../NAME.instructions.md files).
************************************************************/
#define  _CRT_SECURE_NO_WARNINGS
#include "waCrossPlatform.h"
#include HEADER_SLEEP
#include HEADER_CURSES
#include "walrus.h"

void WaConfig::MarkFail(const char* reason)
{
   isInitSuccess = false;
   auto safeReason = reason ? reason : "..";
   printf("Parsing ERROR: %s.\n", safeReason);
}

void WaConfig::AnnounceTask()
{
   if (IsInitFailed()) {
      return;
   }

   owl.Show("%s : %s", txt.nameTask, txt.titleBrief);
   owl.Show("Fixed hand is %s\n", txt.taskHandPBN);

   if (lens.prim.txtCode[0]) {
      if (lens.secondary.txtCode[0]) {
         owl.Show("Scorers to use are: %s; %s\n", lens.prim.txtCode, lens.secondary.txtCode);
      } else {
         owl.Show("Scorer to use is %s\n", lens.prim.txtCode);
      }
   }
}

bool WaConfig::AllLenses::IsMagicFly()
{
   // are the short scorers for magic fly?
   if (secondary.goal == 9 &&
       secondary.trump == SOL_NOTRUMP &&
       prim.goal == 10 && 
      (prim.trump == SOL_HEARTS || prim.trump == SOL_SPADES)) {
      return true;
   }

   if (prim.goal == 9 &&
       prim.trump == SOL_NOTRUMP &&
       secondary.goal == 10 &&
      (secondary.trump == SOL_HEARTS || secondary.trump == SOL_SPADES)) {
      return true;
   }

   //if ((secundaShort[0] == '3') && 
   //    (secundaShort[1] == 'N')) {
   //   if (    primaShort[0] == '4') {
   //      if ((primaShort[1] == 'H') ||
   //          (primaShort[1] == 'S')) {
   //         return true;
   //      }
   //   }
   //}

   //if ((primaShort[0] == '3') && 
   //    (primaShort[1] == 'N')) {
   //   if (    secundaShort[0] == '4') {
   //      if ((secundaShort[1] == 'H') ||
   //          (secundaShort[1] == 'S')) {
   //         return true;
   //      }
   //   }
   //}

   return false;
}

void WaConfig::DetectTwoScorers()
{
   // both scorers must be present
   if (!lens.prim.txtShort[0]) {
      lens.countLenses = 1;
      return;
   }
   if (!lens.secondary.txtShort[0]) {
      lens.countLenses = 1;
      return;
   }
   lens.countLenses = 2;

   // found both
   solve.shouldSolveTwice = true;

   // are they on the same line?
   if (lens.secondary.IsNSLine()) {
      solve.seekDecisionCompete = false;
      if (lens.IsMagicFly()) {
         io.showMagicFly = true;
      }
   } else {// different lines
      solve.seekDecisionCompete = true;
   }

   // log
   if (io.showMagicFly) {
      //printf("Search for magic fly is detected -- zero IMPs difference.\n");
   }
}

void WaConfig::ResolvePostmortemType(Walrus* walrus)
{
   // failed before
   if (IsInitFailed()) {
      return;
   }

   // always read board 
   postm.hcpFixedHand = walrus->ParsePbnDeal();

   // pm none is ok
   if (postm.Type == WPM_NONE) {
      return;
   }

   // announce; resolve auto 
   printf("Postmortem type: ");
   if (postm.Type == WPM_HCP_SINGLE_SCORER ||
       postm.Type == WPM_A_TO_B) {
      printf("(deprecated; consider using AUTO) ");
   } else if (postm.Type == WPM_AUTO) {
      printf("Auto --> ");
      RecognizePostmType(key.PostmHCP);
      if (!filters.FindHCPRange(SOUTH, postm.minHCP, postm.maxHCP)) {
         printf("Error: missing PointsRange for SOUTH\n");
         MarkFail();
         return;
      }
      postm.minHCP += postm.hcpFixedHand.total;
      postm.maxHCP += postm.hcpFixedHand.total;
   }

   // may detect controls
   if (postm.minHCP) {
      if (postm.minHCP == postm.maxHCP) {
         postm.minControls = (postm.minHCP * 4) / 10 - 6;
      }
   }

   // final types
   bool checkRange = false;
   bool checkLeads = false;
   switch (postm.Type) {
      case WPM_HCP_SINGLE_SCORER:
         if (postm.minControls) {
            postm.Type = WPM_CONTROLS;
            printf("Controls for %d hcp\n", postm.minHCP);
         } else {
            printf("HCP single scorer: %d to %d\n", postm.minHCP, postm.maxHCP);
         }
         strcpy(config.txt.freqTitleFormat, "TRICKS FREQUENCY FOR %d HCP");
         checkRange = true;
         break;

      case WPM_A_TO_B:
         printf("A to B comparator with HCP %d to %d\n", postm.minHCP, postm.maxHCP);
         strcpy(config.txt.freqTitleFormat, "COMPARISON RESULTS FOR %d HCP");
         checkRange = true;
         break;

      case WPM_OPENING_LEADS:// lead task should have lead cards specified
         printf("Opening Leads\n");
         strcpy(config.txt.freqTitleFormat, "TRICKS FREQUENCY FOR %s LEAD");
         checkLeads = true;
         break;

      case WPM_SUIT:
         printf("Suit\n");
         strcpy(config.txt.freqTitleFormat, "TRICKS FREQUENCY FOR HCP in a suit (which?)");
         break;

      default:
         printf("unrecognized pm: %d\n", postm.Type);
         MarkFail();
         break;
   }

   // final checks
   if (checkRange) {
      if ((postm.minHCP < 0) || (postm.maxHCP < 0) || (postm.minHCP > postm.maxHCP)) {
         printf("Error: invalid HCP range in postmortem: min=%d, max=%d\n", postm.minHCP, postm.maxHCP);
         MarkFail();
      }
   }
   if (checkLeads) {
      if (solve.leads.IsEmpty()) {
         printf("Error: '%s' line is missing.\n", key.Leads);
         MarkFail();
      } else {
         owl.Silent("Leads to inspect: %s", txt.taskLeadsPBN);
      }
   }
}

