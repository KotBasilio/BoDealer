/************************************************************
 * Walrus project                                        2020
 * Scorer parts
 *
 ************************************************************/
#define  _CRT_SECURE_NO_WARNINGS
#include <string.h>

#include "walrus.h"
#include "../dds-develop/include/dll.h"
#include "../dds-develop/examples/hands.h"
#include <assert.h>

LineScorer::LineScorer() 
   : linearBase(nullptr)
   , outSum(nullptr) 
   , title("none")
{}

bool LineScorer::IsEmpty() const
{
   return outSum == nullptr;
}

bool LineScorer::HasDouble() const
{
   if (IsEmpty()) {
      return false;
   }

   return title[3]=='X' || 
          title[3]=='R';
}

bool LineScorer::Init(s64 &out, const char* code)
{
   linearBase = FindLinearScore(code);
   if (linearBase) {
      TargetOut(out);
      title = code;
      return true;
   }

   owl.Show("Failed on encoding scorer for code: %s\n", code);
   return false;
}

void LineScorer::TargetOut(s64& out)
{
   outSum = &out;
}

s64 LineScorer::Get(uint tricks)
{
   return linearBase[tricks];
}

void LineScorer::operator()(uint tricks)
{
   *outSum += Get(tricks);
}

void CumulativeScore::FillSameLinears(const CumulativeScore& other)
{
   prima.  FillUpon(&ideal, other.prima,   &other.ideal);
   secunda.FillUpon(&ideal, other.secunda, &other.ideal);
   tertia. FillUpon(&ideal, other.tertia,  &other.ideal);
}

void LineScorer::FillUpon(s64* ourBase, const LineScorer& other, const s64* thatBase)
{
   if (other.IsEmpty()) {
      return;
   }

   // src
   linearBase = other.linearBase;

   // dst
   auto offset = other.outSum - thatBase;
   outSum = ourBase + offset;
}

void CumulativeScore::BiddingLevel(DdsTricks& tr)
{
   // find out ideal
   uint tricks = tr.plainScore;
   auto gainGame = prima.Get(tricks);
   auto gainPartscore = secunda.Get(tricks);
   ideal += gainGame > gainPartscore ? gainGame : gainPartscore;

   // accumulate regular scores
   prima(tricks);
   secunda(tricks);
}

void CumulativeScore::OpeningLead(DdsTricks &tr)
{
   // accumulate regular score
   prima(tr.plainScore);

   // accumulate results of each lead
   leadS += prima.Get(tr.lead.S);
   leadH += prima.Get(tr.lead.H);
   leadD += prima.Get(tr.lead.D);
   leadC += prima.Get(tr.lead.C);
}

int LineScorer::Goal() const
{
   // see FindLinearScore()
   uint level = title[1] - '0';
   return level + 6;
}

int LineScorer::Trump() const
{
   // see FindLinearScore()
   switch (title[2]) {
      case 'N': return SOL_NOTRUMP;
      case 'S': return SOL_SPADES;
      case 'H': return SOL_HEARTS;
      case 'D': return SOL_DIAMONDS;
      case 'C': return SOL_CLUBS;
   }

   DEBUG_UNEXPECTED;
   return SOL_NOTRUMP;
}

int LineScorer::Decl() const
{
   auto place = strstr(title, "BY ");
   if (!place) {
      return SOUTH;
   }

   switch (place[3]) {
      case 'N': return NORTH;
      case 'S': return SOUTH;
      case 'W': return WEST;
      case 'E': return EAST;
   }

   return SOUTH;
}

static const char* s_TrumpNames[] = {
   "spades",
   "hearts",
   "diamonds",
   "clubs",
   "notrump"
};
static const char* s_SeatNames[] = {
   "North",
   "East",
   "South",
   "West"
};

void WaConfig::Contract::Init(const LineScorer& scorer)
{
   trump = scorer.Trump();
   goal = scorer.Goal();
   by = scorer.Decl();
   first = (by + 1) % 4;

   strcpy(txtTrump, s_TrumpNames[trump]);
   strcpy(txtAttacker, s_SeatNames[first]);
   strcpy(txtBy, s_SeatNames[by]);
}

void WaConfig::Contract::CheckTheSetup(const LineScorer& scorer)
{
   // any fail => see ReadPrimaScorer / ReadSecundaScorer
   assert(trump == scorer.Trump());
   assert(goal == scorer.Goal());
   assert(by == scorer.Decl());
   assert(first == (by + 1) % 4);
}

void Walrus::DetectScorerGoals()
{
   // ensure we have a scorer
   if (cumulScore.prima.IsEmpty()) {
      printf("\nERROR: No primary scorer found\n");
      config.MarkFail();
      return;
   }
   config.SetupSeatsAndTrumps(cumulScore);

   // we really test & display scorers, not just a linear values
   // -- primary
   DdsTricks tr;
   char tail[128];
   CumulativeScore zeroes(cumulScore);
      owl.Show("Primary scorer (%s, %d tr):", config.prim.txtTrump, config.prim.goal);
   strcpy(tail, "  / ");
   for (tr.plainScore = 7; tr.plainScore <= 13; tr.plainScore++) {
      cumulScore = zeroes;
      ScoreWithPrimary(tr);
      AddScorerValues(tail);
   }
   owl.Show("%s\n", tail);

   // -- secondary
   bool shouldSkipSecunda = cumulScore.secunda.IsEmpty() || (sem.onPrimaryScoring == &CumulativeScore::BiddingLevel);
   if (!shouldSkipSecunda) {
      owl.Show("Contract-B scorer (%s, %d tr):", config.secondary.txtTrump, config.secondary.goal);
      strcpy(tail, "  / ");
      for (tr.plainScore = 7; tr.plainScore <= 13; tr.plainScore++) {
         cumulScore = zeroes;
         ScoreWithSecondary(tr);
         AddScorerValues(tail);
      }
      owl.Show("%s\n", tail);
   }

   // final cleanup
   cumulScore = zeroes;
}

void Walrus::AddScorerValues(char* tail)
{
   char chunk[20];

   if (config.postm.Is(WPM_OPENING_LEADS)) {
      if (cumulScore.ideal) {
         owl.Show(" %lld", cumulScore.ideal);
         return;
      }
   }

   if (cumulScore.oppContract) {
      owl.Show(" %lld", -cumulScore.oppContract);
      if (cumulScore.oppCtrDoubled) {
         sprintf(chunk, " %lld", -cumulScore.oppCtrDoubled);
         strcat(tail, chunk);
      }
      return;
   }

   if (cumulScore.oppCtrDoubled) {
      owl.Show(" %lld", -cumulScore.oppCtrDoubled);
      return;
   }

   if (cumulScore.bidGame) {
      owl.Show(" %lld", cumulScore.bidGame);
      if (cumulScore.bidPartscore) {
         sprintf(chunk, " %lld", cumulScore.bidPartscore);
         strcat(tail, chunk);
      }
      return;
   }

   if (cumulScore.ourOther) {
      owl.Show(" %lld", cumulScore.ourOther);
      return;
   }

   if (cumulScore.bidPartscore) {
      sprintf(chunk, " %lld", cumulScore.bidPartscore);
      strcat(tail, chunk);
      return;
   }
}

void Semantics::SetOurPrimaryScorer(CumulativeScore& cs, const char* code)
{
   // aim at bidGame in CumulativeScore
   if (!cs.prima.Init(cs.bidGame, code)) {
      MarkFail("Failed to init prima scorer");
      return;
   }

   // ok
   onPrimaryScoring = &CumulativeScore::Primary;
   assert(config.prim.goal == cs.prima.Goal());
   onSinglePrimary = &CumulativeScore::DepPrimary;
}

void Semantics::SetSecondaryScorer(CumulativeScore& cs, s64& target, const char* code)
{
   if (!cs.secunda.Init(target, code)) {
      MarkFail("Failed to init secondary scorer");
      return;
   }

   // ok
   onSecondScoring = &CumulativeScore::Secondary;
   assert(config.secondary.goal == cs.secunda.Goal());
   onSingleSecondary = &CumulativeScore::DepSecondary;
}

void Semantics::SetOurSecondaryScorer(CumulativeScore& cs, const char* code)
{
   // aim at ourOther in CumulativeScore
   if (IsInitOK()) {
      SetSecondaryScorer(cs, cs.ourOther, code);
      if (IsInitFailed()) {
         printf("Semantics ERROR: the task suggests having our second scorer.\n");
      }
   }
}

void Semantics::SetTheirScorer(CumulativeScore& cs, const char* code)
{
   // aim at oppContract in CumulativeScore
   if (IsInitOK()) {
      SetSecondaryScorer(cs, cs.oppContract, code);
      if (IsInitFailed()) {
         printf("Semantics ERROR: the task suggests having their scorer.\n");
      }
      assert(cs.secunda.HasDouble() == config.io.oppsAreDoubled);
      if (config.io.oppsAreDoubled) {
         cs.secunda.TargetOut(cs.oppCtrDoubled);
      }
   }
}

void Semantics::SetBiddingLevelScorer(CumulativeScore& cs)
{
   // prima scorer aims at "bidGame"
   const char* code = config.txt.primaScorerCode;
   SetOurPrimaryScorer(cs, code);

   // make permanent other scorer code
   config.MakeSecondaryScrorerForBiddingLevel();
   SetOurSecondaryScorer(cs, config.txt.secundaScorerCode);

   // secunda scorer aims at "bidPartscore"
   cs.secunda.TargetOut(cs.bidPartscore);
   if (IsInitFailed()) {
      return;
   }

   // allow to compare
   onPrimaryScoring = &CumulativeScore::BiddingLevel;
}

void WaConfig::MakeSecondaryScrorerForBiddingLevel()
{
   char* hedge = txt.secundaScorerCode;
   strcpy(hedge, txt.primaScorerCode);
   hedge[1]--;
   FillShortScorer(hedge, txt.secundaShort);
}

void Semantics::SetOpeningLeadScorer(CumulativeScore& cs, const char* code)
{
   SetOurPrimaryScorer(cs, code);
   cs.prima.TargetOut(cs.ideal);
   if (IsInitFailed()) {
      return;
   }

   // allow to count for all leads
   onPrimaryScoring = &CumulativeScore::OpeningLead;
}

WA_TASK_TYPE WaConfig::DetectOneHandVariant()
{
   // comparative
   if (solve.shouldSolveTwice) {
      if (solve.seekDecisionCompete) {
         return TTYPE_COMPETITIVE_GENERIC;
      }
      return TTYPE_ONE_SIDED_DENOMINATION;
   }

   // TODO: detect opening lead task

   // basic
   return TTYPE_ONE_SIDED_BIDDING_LEVEL;
}
