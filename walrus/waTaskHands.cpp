/************************************************************
 * Walrus project                                        2019
 * Initialization parts
 *
 ************************************************************/
#define  _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include "walrus.h"
#include "../dds-develop/src/PBN.h"
#include "../dds-develop/examples/hands.h"
#include HEADER_CURSES
#include <memory.h> // memset

// --------------------------------------------------------------------------------
// input

#ifdef SEM_MAY24_GLADIATOR
   #define INPUT_TRUMPS    SOL_NOTRUMP
   #define INPUT_ON_LEAD   WEST
   //#define INPUT_ON_LEAD   EAST
   char const taskHandPBN[] = "[N:KT965.A64.73.T74]";
   //char const taskHandPBN[] = "[N:5.KJ4.AKQJ73.Q74]";
#endif 

#ifdef SEM_MAY24_TENS_AND_NINES
   char const taskHandPBN[] = "[N:JT6.KJ6.KJT7.J97]";
  // char const taskHandPBN[] = "[N:JT9.KJT.KJT9.JT9]";
#endif 

#ifdef SEM_MAR24_5C_OR_3NT
   #define INPUT_TRUMPS    SOL_CLUBS
   #define INPUT_ON_LEAD   WEST
   char const taskHandPBN[] = "[N:4.A954.KQ3.QJ985.]";
#endif 

#ifdef SEM_MIX_MAR24_INVITE_4S
   #define INPUT_TRUMPS    SOL_SPADES
   #define INPUT_ON_LEAD   WEST
   //char const taskHandPBN[] = "[N:AK97.432.2.AQJ84]";
#endif 

#ifdef SEM_DU_MAR_1C_X_XX_1H_ETC_MAR
   #define INPUT_TRUMPS    SOL_DIAMONDS
   #define INPUT_ON_LEAD   WEST
   char const taskHandPBN[] = "[N:Q.874.AQ9654.653]";
#endif 

#ifdef SEMANTIC_1C_P_1H_2D_2H_MAR
   #define INPUT_TRUMPS    SOL_HEARTS
   #define INPUT_ON_LEAD   WEST
   char const taskHandPBN[] = "[N:83.QJ874.A5.A965]";
#endif 

#ifdef SEMANTIC_1H_1S_2H_MAR
   #define INPUT_TRUMPS    SOL_HEARTS
   #define INPUT_ON_LEAD   WEST
   char const taskHandPBN[] = "[N:QJ53.KJ8.953.AT7]";
#endif 

#ifdef SEMANTIC_INV_AFTER_2H
   #define INPUT_TRUMPS    SOL_SPADES
   #define INPUT_ON_LEAD   WEST
   char const taskHandPBN[] = "[N:9854.K96.Q65.AK8]";
#endif 

#ifdef SEMANTIC_7D_OR_NT_FEB
   #define INPUT_TRUMPS    SOL_DIAMONDS
   #define INPUT_ON_LEAD   WEST
   char const taskHandPBN[] = "[N:A7.AKJ8743.98.AK]";
#endif 

#ifdef SEMANTIC_4M_ON54_FEB_PATTON
   #define INPUT_TRUMPS    SOL_SPADES
   #define INPUT_ON_LEAD   WEST
   char const taskHandPBN[] = "[N:KJT75.T73.K96.K8]";
#endif 

#ifdef SEMANTIC_54M_FEB_PARTIZAN
//#define INPUT_TRUMPS    SOL_HEARTS
//   #define INPUT_TRUMPS    SOL_SPADES
   #define INPUT_TRUMPS    SOL_NOTRUMP
   #define INPUT_ON_LEAD   WEST
   char const taskHandPBN[] = "[N:J532.AQ843.65.A5.]";
#endif 

#ifdef SEMANTIC_TANJA_FEB_LONG_H
   #define INPUT_TRUMPS    SOL_HEARTS
   #define INPUT_ON_LEAD   EAST
   char const taskHandPBN[] = "[N:AT.JT987532.K6.5.]";
#endif 

#ifdef SEMANTIC_BASHA_NOV_NT
   #define INPUT_TRUMPS    SOL_NOTRUMP
   #define INPUT_ON_LEAD   EAST
   char const taskHandPBN[] = "[N:85.A7.A63.KJT853.]";
#endif 

#ifdef SEMANTIC_OCT_INDIAN_5C
   #define INPUT_TRUMPS    SOL_SPADES
   #define INPUT_ON_LEAD   EAST
   //char const taskHandPBN[] = "[N:AQJ.KJT5.A763.95]";
   char const taskHandPBN[] = "[N:QT985.K73.QJ653.]";
#endif 

#ifdef SEMANTIC_JULY_MOROZOV_HAND_EVAL
   #define INPUT_TRUMPS    SOL_NOTRUMP
   #define INPUT_ON_LEAD   WEST
   //char const taskHandPBN[] = "[N:AQJ.KJT5.A763.95]";
   char const taskHandPBN[] = "[N:A95.KJT5.A763.QJ]";
#endif 

#ifdef SEMANTIC_OCT_SEEK_6D
   #define INPUT_TRUMPS    SOL_DIAMONDS
   #define INPUT_ON_LEAD   WEST
   char const taskHandPBN[] = "[N:K5.AQ96.K93.AK43]";
#endif 

#ifdef SEMANTIC_JUN_MINORS_VS_MAJORS
   // origin: Partizan, July 2023
   #define INPUT_TRUMPS    SOL_DIAMONDS
   #define INPUT_ON_LEAD   WEST
   char const taskHandPBN[] = "[N:JT4.Q93.AT63.Q92]";
#endif


#ifdef SEMANTIC_OVERCALL_4C
   // origin: Morozevic, Dec 2023
   #define INPUT_TRUMPS    SOL_CLUBS
   #define INPUT_ON_LEAD   EAST
   char const taskHandPBN[] = "[N:.KJ5.T763.KQ9873]";
#endif // SEMANTIC_OVERCALL_4C

#ifdef FOUR_HANDS_TASK
   char const taskHandPBN[] = "[N:234.567.AKQT.J98]";// just a dummy
   #ifndef INPUT_TRUMPS
      #define INPUT_TRUMPS    SOL_HEARTS
   #endif
   #ifndef INPUT_ON_LEAD
      #define INPUT_ON_LEAD   EAST
   #endif
#endif // FOUR_HANDS_TASK

uint zero_holdings[DDS_HANDS][DDS_SUITS] =
{ // North    East        South       West
   { 0,       0,          0,         0  } , // spades
   { 0,       0,          0,         0  } , // hearts
   { 0,       0,          0,         0  } , // diamonds
   { 0,       0,          0,         0  }   // clubs
};
uint(*input_holdings)[DDS_HANDS][DDS_SUITS] = &zero_holdings;

u64  wa_SuitByDds[DDS_SUITS] = { BO_SPADS, BO_HEART, BO_DIAMD, BO_CLUBS };
uint wa_PosByDds [DDS_SUITS] = {    48,    32,    16,     0 };
void Walrus::WithdrawByInput(void)
{
   // convert to temp
   ddTableDeal pbnDeal;
   if (ConvertFromPBN(config.taskHandPBN, pbnDeal.cards) != 1) {
      printf("\nERROR: Cannot parse PBN: %s\n", config.taskHandPBN);
      config.MarkFail();
      return;
   }

   // store
   for (int h = 0; h < DDS_HANDS; h++) {
      for (int s = 0; s < DDS_SUITS; s++) {
         (*input_holdings)[s][h] = pbnDeal.cards[h][s];
      }
   }

   // usual withdraw
   for (int h = 0; h < DDS_HANDS; h++) {
      for (int s = 0; s < DDS_SUITS; s++) {
         auto hld = (*input_holdings)[s][h];
         WithdrawHolding(hld, wa_PosByDds[s]);
      }
   }

   if (!shuf.AssertDeckSize(SYMM3, config.taskHandPBN)) {
      config.MarkFail();
   }
}

static uint CountBits(uint v)// count bits set in v (32-bit value)
{
   uint c; // store the total here
   static const int S[] = { 1, 2, 4, 8, 16 }; // 2 pow idx
   static const int B[] = { 0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF, 0x0000FFFF };

   c = v - ((v >> 1) & B[0]);
   c = ((c >> S[1]) & B[1]) + (c & B[1]);
   c = ((c >> S[2]) + c) & B[2];
   c = ((c >> S[3]) + c) & B[3];
   c = ((c >> S[4]) + c) & B[4];

   return c;
}

void Semantics::PrepareBaseDeal()
{
   // ptr
   static deal staticDeal;
   dlBase = &staticDeal;

   // common fields
   memset(dlBase, 0, sizeof(*dlBase));
   dlBase->trump = config.prim.trump;
   dlBase->first = config.prim.first;

   // done for 4-hand tasks. We'll fill each hand
   #ifdef FOUR_HANDS_TASK
      return;
   #endif

   uint countCards = 0;
   for (int h = 0; h < DDS_HANDS; h++) {
      for (int s = 0; s < DDS_SUITS; s++) {
         auto holding = (*input_holdings)[s][h];
         dlBase->remainCards[h][s] = holding;
         countCards += CountBits(holding);
      }
   }

   if (countCards != SYMM) {
      printf("\nERROR: Wrong count of cards in hand: %d\n", countCards);
      config.MarkFail();
   }
}

#define K2       2
#define K3       3
#define K4       4
#define K5       5
#define K6       6
#define K7       7
#define K8       8
#define K9       9
#define KT      10
#define KJ      11
#define KQ      12
#define KK      13
#define KA      14

void DdsTricks::Init(futureTricks& fut)
{
   // plainScore is good for any goal
   plainScore = 13 - fut.score[0];

   // the rest is for opening lead
   if (config.postm.Is(WPM_OPENING_LEADS)) {
      for (int i = 0; i < fut.cards; i++) {
         if (fut.suit[i] == SOL_SPADES && fut.rank[i] == CARD_LEAD_SPADE) {
            lead.S = 13 - fut.score[i];
            continue;
         }
         if (fut.suit[i] == SOL_HEARTS && fut.rank[i] == CARD_LEAD_HEARTS) {
            lead.H = 13 - fut.score[i];
            continue;
         }
         if (fut.suit[i] == SOL_DIAMONDS && fut.rank[i] == CARD_LEAD_DIAMD) {
            lead.D = 13 - fut.score[i];
            continue;
         }
         if (fut.suit[i] == SOL_CLUBS && fut.rank[i] == CARD_LEAD_CLUBS) {
            lead.C = 13 - fut.score[i];
            continue;
         }
      }
   }
}
