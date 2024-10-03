/************************************************************
 * Walrus project                                        2020
 * Extra marks
 *
   // an experimental code to enhance AddMarksBySuit()
   #ifdef CALC_CLUB_HITS_EXPERIMENTAL
      static twlHCP lastCalcHcp;
      lastCalcHcp = hcp; // in CalcNSLineHCP
      {
         row = IO_ROW_POSTMORTEM + (config.postm.maxHCP - config.postm.minHCP) * 2;
         if (lastCalcHcp.c > 8) {
            progress.SolvedExtraMark(row, 0);
         } else {
            progress.SolvedExtraMark(row + 1, 0);
         }
      }
   #endif
 ************************************************************/

#define  _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include "waDoubleDeal.h"
#include "../dds-develop/examples/hands.h"

// ret: HCP on NS line
uint CalcNSLineHCP(const deal& dl, uint& ctrl)
{
   // not a complex task, knowing that 
   // RA | RK | RQ | RJ -- are in nearby bits
   // from RJ = 0x0800 to RA = 0x4000
   // so we construct a hand with face cards only
   const auto &cards = dl.remainCards;
   u64 facecards (RA | RK | RQ | RJ);
   SplitBits reducedHand (
      (((cards[SOUTH][SOL_SPADES  ] | cards[NORTH][SOL_SPADES  ]) & facecards) << (1 + 16*3)) |
      (((cards[SOUTH][SOL_HEARTS  ] | cards[NORTH][SOL_HEARTS  ]) & facecards) << (1 + 16*2)) |
      (((cards[SOUTH][SOL_DIAMONDS] | cards[NORTH][SOL_DIAMONDS]) & facecards) << (1 + 16*1)) |
      (((cards[SOUTH][SOL_CLUBS   ] | cards[NORTH][SOL_CLUBS   ]) & facecards) << (1))
   );

   twlControls controls(reducedHand);
   ctrl = controls.total;

   twlHCP hcp(reducedHand);
   return hcp.total;
}

// ret: HCP on seat position
static uint CalcSuitHCP(const deal& cards, uint seat)
{
   const auto& remain = cards.remainCards;
   u64 facecards(RA | RK | RQ | RJ);
   SplitBits reducedHand(
      ((remain[seat][SOL_SPADES]   & facecards) << (1 + 16 * 3)) |
      ((remain[seat][SOL_HEARTS]   & facecards) << (1 + 16 * 2)) |
      ((remain[seat][SOL_DIAMONDS] & facecards) << (1 + 16 * 1)) |
      ((remain[seat][SOL_CLUBS]    & facecards) << (1))
   );
   twlHCP hcp(reducedHand);
   return hcp.arr[config.postmSuit];
}

void Walrus::AddMarksByHCP(DdsTricks& tr, const deal& cards)
{
   // detect row
   uint row = 0, ctrl = 0;
   int hcp = CalcNSLineHCP(cards, ctrl);
   if (config.postm.minControls) {
      if (ctrl < (uint)config.postm.minControls) {
         row = IO_ROW_POSTMORTEM;
      } else {
         row = IO_ROW_POSTMORTEM + (ctrl - config.postm.minControls) * 2;
      }
   } else if (hcp < config.postm.minHCP || config.postm.maxHCP < hcp) {
      return;
   } else {
      row = IO_ROW_POSTMORTEM + (hcp - config.postm.minHCP) * 2;
   }

   // proper row => add a mark in stat
   if (row < IO_ROW_FILTERING - 1) {
      progress.HitByTricks(tr.plainScore, config.prim.goal, row);
   }
}

void Walrus::AddMarksBySuit(DdsTricks& tr, const deal& cards)
{
   // calc
   auto suitHCP = CalcSuitHCP(cards, config.postmHand);
   uint row = IO_ROW_POSTMORTEM + suitHCP*2;

   // proper row => add a mark in stat
   if (row < IO_ROW_FILTERING - 1) {
      progress.HitByTricks(tr.plainScore, config.prim.goal, row);
   }
}

void Walrus::AddMarksByCamp(uint camp, const deal& cards)
{
   // detect row
   uint ctrl = 0;
   int hcp = CalcNSLineHCP(cards, ctrl);
   if (hcp < config.postm.minHCP || config.postm.maxHCP < hcp) {
      return;
   }
   uint row = IO_ROW_POSTMORTEM + (hcp - config.postm.minHCP);

   // proper row => add a mark in stat
   if (row < IO_ROW_FILTERING - 1) {
      progress.SolvedExtraMark(row, camp);
   }
}

void Walrus::AddMarksByOpLeads(DdsTricks& tr, const deal& cards)
{
   uint row = IO_ROW_POSTMORTEM;
   progress.HitByTricks(tr.lead.S, config.prim.goal, row); row += 2;
   progress.HitByTricks(tr.lead.H, config.prim.goal, row); row += 2;
   progress.HitByTricks(tr.lead.D, config.prim.goal, row); row += 2;
   progress.HitByTricks(tr.lead.C, config.prim.goal, row);
}
