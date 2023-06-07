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
//char const taskHandPBN[] = "[N:...]";

#ifdef SEMANTIC_FEB_3NT_REBID
char const taskHandPBN[] = "[N:94.A9.AQJ763.KT9]";
#define INPUT_TRUMPS    SOL_NOTRUMP
#define INPUT_ON_LEAD   WEST
#endif 

#ifdef SEMANTIC_JAN_INVITE_MINORS
#define INPUT_AS_PBN 
char const taskHandPBN[] = "[N:AJT4.76.QJT6.A86]";
//char const taskHandPBN[] = "[N:AJ94.76.QJT6.A86]";
//char const taskHandPBN[] = "[N:AJT4.J6.QJT6.A86]";
//char const taskHandPBN[] = "[N:AJT4.76.QJT6.AJ6]";
//char const taskHandPBN[] = "[N:AT42.76.QJT6.AJ6]";
#define INPUT_TRUMPS    SOL_DIAMONDS
#define INPUT_ON_LEAD   WEST
#endif 

#ifdef SEMANTIC_JAN_PETYA_VS_3NT
// origin match at home
#define INPUT_AS_PBN 
char const taskHandPBN[] = "[W:QT83.A72.73.A862]";
#define INPUT_TRUMPS    SOL_NOTRUMP
#define INPUT_ON_LEAD   WEST
#define CARD_LEAD_SPADE   K3
#define CARD_LEAD_HEARTS  KA
#define CARD_LEAD_DIAMD   K7
#define CARD_LEAD_CLUBS   K2
#endif // SEMANTIC_JAN_PETYA_VS_3NT

#ifdef SEMANTIC_NOV_INVITE_PRECISION
#define INPUT_AS_PBN 
char const taskHandPBN[] = "[N:J942.AQJ72.73.62]";
//char const taskHandPBN[] = "[N:AQJ72.J942.73.62]";
#define INPUT_TRUMPS    SOL_SPADES
#define INPUT_ON_LEAD   WEST
#endif 

#ifdef SEMANTIC_OCT_WEAK_GAMBLING
// origin: Partizan club, IMP
#define INPUT_AS_PBN 
//char const taskHandPBN[] = "N:87..J762.AKQ9754]";
char const taskHandPBN[] = "N:8.7.J762.AKQ9754]";
//char const taskHandPBN[] = "N:8.7.J72.AKQ97654]";
//char const taskHandPBN[] = "N:.87.J762.AKQ9754]";
#define INPUT_TRUMPS    SOL_CLUBS
#define INPUT_ON_LEAD   EAST
#endif

#ifdef SEMANTIC_SEPT_INVITE_OR_FG
// origin: Momo's club, max
#define INPUT_AS_PBN 
char const taskHandPBN[] = "[N:AQ72.87.KT872.J9]";
#define INPUT_TRUMPS    SOL_NOTRUMP
#define INPUT_ON_LEAD   WEST
#endif 


#ifdef SEMANTIC_AUG_3NT_ON_44H
#define INPUT_HOLDINGS  max_partizan
uint max_partizan[DDS_HANDS][DDS_SUITS] =
{ // North                         East        South       West
   { RK | RJ | R3 ,                      0,          0,         0  } , // spades
   { R9 | R8 | R6 | R5,                       0,          0,         0  } , // hearts
   { RQ | R7 | R5 ,                 0,          0,         0  } , // diamonds
   { RJ | RT | R7 ,  0,          0,         0  }   // clubs
};
#define INPUT_TRUMPS    SOL_HEARTS
#define INPUT_ON_LEAD   WEST
#endif

#ifdef SEMANTIC_AUG_3NT2_ON_SPADE_FIT
#define INPUT_HOLDINGS  max_gambler
uint max_gambler[DDS_HANDS][DDS_SUITS] =
{ // North                         East        South       West
   { RT | R3 ,                      0,          0,         0  } , // spades
   { RT | R5,                       0,          0,         0  } , // hearts
   { RA | RJ | R8 | RT | R5 ,                 0,          0,         0  } , // diamonds
   { RK | R9 | R6 | R7 ,  0,          0,         0  }   // clubs
};
#define INPUT_TRUMPS    SOL_SPADES
#define INPUT_ON_LEAD   EAST
#endif


#ifdef SEMANTIC_AUG_3NT_ON_SPADE_FIT
// origin: gambler 11.08.2022
#define INPUT_HOLDINGS  max_gambler
uint max_gambler[DDS_HANDS][DDS_SUITS] =
{ // North                         East        South       West
   { R9 | R6 ,                      0,          0,         0  } , // spades
   { RK | RQ | R9 | R5,                       0,          0,         0  } , // hearts
   { RT | R8 | R3 | R2 ,                 0,          0,         0  } , // diamonds
   { RA | R8 | R7 ,  0,          0,         0  }   // clubs
};
#define INPUT_TRUMPS    SOL_SPADES
#define INPUT_ON_LEAD   EAST
#endif

#ifdef SEMANTIC_MIXED_PREVENTIVE_4S
// origin: https://bridgemoscow.ru/tournaments/results/r21pmx/r21pmxd34p.htm
#define INPUT_HOLDINGS  mixed_preventive
uint mixed_preventive[DDS_HANDS][DDS_SUITS] =
{ // North                         East        South       West
   { RT | R9 | R8 | R5 | R3 | R2 ,                      0,          0,         0  } , // spades
   { RT | R9 | R7,                       0,          0,         0  } , // hearts
   { RQ | R3 | R2 ,                 0,          0,         0  } , // diamonds
   { R9 ,  0,          0,         0  }   // clubs
};
#define INPUT_TRUMPS    SOL_SPADES
#define INPUT_ON_LEAD   EAST
#endif // SEMANTIC_MIXED_PREVENTIVE_4S


#ifdef SEMANTIC_MIXED_2D_WITH_MAJOR
// origin: june
#define INPUT_HOLDINGS  mixed_2d_wM
uint mixed_2d_wM[DDS_HANDS][DDS_SUITS] =
{ // North                         East        South       West
   { R9 | R8 | R6 ,          0,          0,         0  } , // spades
   { R9 | R8 | R4 | R2,      0,          0,         0  } , // hearts
   { RK | RJ | R2,           0,          0,         0  } , // diamonds
   { RK | R9 | R5,           0,          0,         0  }   // clubs
};
#define INPUT_TRUMPS    SOL_DIAMONDS
#define INPUT_ON_LEAD   NORTH
#endif // SEMANTIC_MIXED_2D_WITH_MAJOR

#ifdef SEMANTIC_NOV_DBL_ON_3NT
// origin: a match at home
#define INPUT_HOLDINGS  nov_3nt_doubled
#define INPUT_TRUMPS    SOL_NOTRUMP
#define INPUT_ON_LEAD   EAST
uint nov_3nt_doubled[DDS_HANDS][DDS_SUITS] =
{ // North      East        South      West
   { 0,         0,          0,         RJ | R9 | R7 | R5  } , // spades
   { 0,         0,          0,         RQ | R6 | R4  } ,      // hearts
   { 0,         0,          0,         RQ | R8  } ,           // diamonds
   { 0,         0,          0,         RQ | RJ | R7 | R5}     // clubs
};
#endif // SEMANTIC_NOV_DBL_ON_3NT

#ifdef SEMANTIC_DEC21_DBL_ON_3NT
// origin: ligovka Dec 2021
#define INPUT_HOLDINGS  nov_3nt_doubled
#define INPUT_TRUMPS    SOL_NOTRUMP
#define INPUT_ON_LEAD   WEST
uint nov_3nt_doubled[DDS_HANDS][DDS_SUITS] =
{ // North      East        South      West
	{ 0,         0,          0,         RK | RQ | R5  } , // spades
	{ 0,         0,          0,         R8 | R7  } ,      // hearts
	{ 0,         0,          0,         RJ | RT | R8 | R7 | R3  } ,           // diamonds
	{ 0,         0,          0,         RK | RT | R3 }    // clubs
};
#define CARD_LEAD_SPADE   KK
#define CARD_LEAD_HEARTS  K8
#define CARD_LEAD_DIAMD   KJ
#define CARD_LEAD_CLUBS   K3
#endif // SEMANTIC_DEC21_DBL_ON_3NT

#ifdef SEMANTIC_NOV_BID_6C_OR_DBL_4S
// origin: Morozevich https://www.gambler.ru/tour/786003/protocol?id=72526447 brd 11
#define INPUT_HOLDINGS  fito_july
#define INPUT_TRUMPS    SOL_CLUBS
#define INPUT_ON_LEAD   EAST
uint fito_july[DDS_HANDS][DDS_SUITS] =
{ // North                         East        South       West
   { RA | RQ ,                      0,          0,         0  } , // spades
   { RJ | R7,                       0,          0,         0  } , // hearts
   { RA | RJ | R8 ,                 0,          0,         0  } , // diamonds
   { RK | RQ | R8 | R7 | R6 | R4 ,  0,          0,         0  }   // clubs
};
#endif // SEMANTIC_NOV_BID_6C_OR_DBL_4S

#ifdef SEMANTIC_FEB_4711_DILEMMA_ON_4S
// origin: Morozevich vs Nokhaeva
#define INPUT_HOLDINGS  many_hearts
#define INPUT_TRUMPS    SOL_HEARTS
#define INPUT_ON_LEAD   EAST
uint many_hearts[DDS_HANDS][DDS_SUITS] =
{ // North                                  East        South       West
   { RK | RJ | R9 | R6 ,                      0,          0,         0  } , // spades
   { RK | RJ | R9 | R6 | R5  | R4 | R3,       0,          0,         0  } , // hearts
   { RA  ,                                    0,          0,         0  } , // diamonds
   { R7  ,                                    0,          0,         0  }   // clubs
};
#endif // SEMANTIC_FEB_4711_DILEMMA_ON_4S

#ifdef SEMANTIC_DEC_BID_5H_OR_DBL_4S
// origin: Serbin http://spb.bridgesport.ru/spb/Zimn_Cap_20/Cup20r5/d1p.htm
#define INPUT_HOLDINGS  longers_dec
#define INPUT_TRUMPS    SOL_HEARTS
#define INPUT_ON_LEAD   EAST
uint longers_dec[DDS_HANDS][DDS_SUITS] =
{ // North                         East        South       West
   { 0 ,                       0,          0,         0  } , // spades
   { RA | RK | RQ | RJ | RT | R6 | R3, 0,  0,         0  } , // hearts
   { RA | RJ ,                 0,          0,         0  } , // diamonds
   { RK | RT | R8 | R2,        0,          0,         0  }   // clubs
};
#endif // SEMANTIC_DEC_BID_5H_OR_DBL_4S

#ifdef SEMANTIC_NOV_VOIDWOOD
// origin: a match at home
#define INPUT_HOLDINGS  heartsSlamTry
#define INPUT_TRUMPS    SOL_HEARTS
#define INPUT_ON_LEAD   EAST
uint heartsSlamTry[DDS_HANDS][DDS_SUITS] =
{ // North                    East        South       West
   { RA | R9 | R7,             0,          0,         0  } , // spades
   { RA | RQ | RJ | RT | R8 | R5 | R3, 0,  0,         0  } , // hearts
   { RA | R5 | R3 ,            0,          0,         0  } , // diamonds
   { 0,                        0,          0,         0  }   // clubs
};
#endif // SEMANTIC_NOV_VOIDWOOD

#ifdef SEMANTIC_JAN_NT_SLAM_ON_DIAMONDS
// origin: http://spb.bridgesport.ru/spb/XMas21p/XMas21ps1/d21p.php#h
#define INPUT_HOLDINGS  slamOnDiamonds
#define INPUT_TRUMPS    SOL_NOTRUMP
#define INPUT_ON_LEAD   WEST
uint slamOnDiamonds[DDS_HANDS][DDS_SUITS] =
{ // North                         East        South       West
   { RQ | R5,                      0,          0,         0  } , // spades
   { RA | RJ,                      0,          0,         0  } , // hearts
   { RA | RQ | RT | R9 | R7 | R2 , 0,          0,         0  } , // diamonds
   { RK | R3 | R2,                 0,          0,         0  }   // clubs
};
#endif // SEMANTIC_JAN_NT_SLAM_ON_DIAMONDS

#ifdef SEMANTIC_MORO_SLAM
// origin: whatsapp chat
#define INPUT_HOLDINGS  slamOnDiamonds
//#define INPUT_TRUMPS    SOL_NOTRUMP
//#define INPUT_ON_LEAD   WEST
#define INPUT_TRUMPS    SOL_DIAMONDS
#define INPUT_ON_LEAD   EAST
uint slamOnDiamonds[DDS_HANDS][DDS_SUITS] =
{ // North                         East        South       West
   { RA,                      0,          0,         0  } , // spades
   { R8,                      0,          0,         0  } , // hearts
   { RA | RQ | R8 | R9 | R7 | R3 | R2 , 0,          0,         0  } , // diamonds
   { RK | R8 | R5 | R2,                 0,          0,         0  }   // clubs
};
#endif // SEMANTIC_MORO_SLAM

#ifdef SEMANTIC_SEPT_MANTICORA_14_16
// origin: home match
#define INPUT_HOLDINGS  sash_hand
#define INPUT_TRUMPS    SOL_HEARTS
#define INPUT_ON_LEAD   WEST
uint sash_hand[DDS_HANDS][DDS_SUITS] =
{ // North                         East        South       West
   { RQ | RJ | R9 ,                 0,          0,         0  } , // spades
   { RJ | RT | R7 | R6 | R4,        0,  0,         0  } , // hearts
   { RK | R4 | R3 | R2 ,            0,          0,         0  } , // diamonds
   { R8,                            0,          0,         0  }   // clubs
};
#endif

#ifdef SEMANTIC_APR_64_INVITE
// origin: VaBene on Gambler
#define INPUT_HOLDINGS  apr64
#define INPUT_TRUMPS    SOL_NOTRUMP
#define INPUT_ON_LEAD   WEST
uint apr64[DDS_HANDS][DDS_SUITS] =
{ // North                         East        South       West
   { RT | R9 | R7 | R5,            0,          0,         0  } , // spades
   { RQ,                           0,          0,         0  } , // hearts
   { RK | RT | R9 | R8 | R5 | R2 , 0,          0,         0  } , // diamonds
   { RK | R3,                      0,          0,         0  }   // clubs
};
/*
Results
:  HITS COUNT   :
(down):  703259, 653392, 317622, 106627,  25318,   3169,    898,    241,      8,      0,   1810534
(make):  618046, 222393,  22174,    847,      6,      0,      0,      0,      0,      0,    863466
Processed: 2674000 total. West is on lead. Goal is 9 tricks in notrump.
Averages: ideal = 226, bidGame = 64, partscore=81.   Chances: 67.7% down some + 32.3% make
*/
#endif // SEMANTIC_APR_64_INVITE

#ifdef SEMANTIC_NOV_64_AS_TWO_SUITER
// origin: http://spb.bridgesport.ru/spb/DKVkusa/p2011171/d21p.php
#define INPUT_HOLDINGS  nov64_as_two_suiter
#define INPUT_TRUMPS    SOL_SPADES
#define INPUT_ON_LEAD   EAST
uint nov64_as_two_suiter[DDS_HANDS][DDS_SUITS] =
{ // North                    East        South       West
   { RT | R8 | R6 | R5 | R4 | R2, 0,      0,         0  } , // spades
   { R7 | R4,                 0,          0,         0  } , // hearts
   { R9 ,                     0,          0,         0  } , // diamonds
   { RA | RK | RQ | RJ,       0,          0,         0  }   // clubs
};
#endif // SEMANTIC_NOV_64_AS_TWO_SUITER

#ifdef SEMANTIC_DEC_12_2425
// origin: http://spb.bridgesport.ru/spb/Zimn_Cap_20/Cup20r8/d32p.htm
#define INPUT_HOLDINGS  dec_12_2425
#define INPUT_TRUMPS    SOL_NOTRUMP
#define INPUT_ON_LEAD   EAST
uint dec_12_2425[DDS_HANDS][DDS_SUITS] =
{ // North                    East        South       West
   { RJ | R9,                 0,          0,         0  } , // spades
   { RA | RK | R8 | R4,       0,          0,         0  } , // hearts
   { RQ | R3 ,                0,          0,         0  } , // diamonds
   { RQ | R9 | R6 | R3 | R2,  0,          0,         0  }   // clubs
};
uint var_dec_12_2425[DDS_HANDS][DDS_SUITS] =
{ // North                    East        South       West
   { RJ | R9,                 0,          0,         0  } , // spades
   { RQ | R9 | R8 | R4,       0,          0,         0  } , // hearts
   { RQ | R3 ,                0,          0,         0  } , // diamonds
   { RA | RK | R6 | R3 | R2,  0,          0,         0  }   // clubs
};
#endif // SEMANTIC_DEC_12_2425

#ifdef SEMANTIC_SEPT_MAJORS54_18HCP
#define INPUT_HOLDINGS  majors54_18hcp
#define INPUT_TRUMPS    SOL_HEARTS
#define INPUT_ON_LEAD   EAST
uint majors54_18hcp[DDS_HANDS][DDS_SUITS] =
{ // North                    East        South       West
   { RK | RQ | RJ | R6 | R3,   0,          0,         0  } , // spades
   { RA | RK | R7 | R2,        0,          0,         0  } , // hearts
   { R4 ,                      0,          0,         0  } , // diamonds
   { RK | RQ | R5,             0,          0,         0  }   // clubs
};
#endif // SEMANTIC_SEPT_MAJORS54_18HCP

#ifdef SEMANTIC_AUG_MULTI_VUL
#define INPUT_HOLDINGS  multi_vul
#define INPUT_TRUMPS    SOL_SPADES
#define INPUT_ON_LEAD   EAST
uint multi_vul[DDS_HANDS][DDS_SUITS] =
{ // North                    East        South       West
	{ RK | R8,                  0,          0,         0  } , // spades
	{ RA | RJ | R6 | R3,        0,          0,         0  } , // hearts
	{ RK | RT | R6 | R2 ,       0,          0,         0  } , // diamonds
	{ RA | R9 | R6,             0,          0,         0  }   // clubs
};
#endif // SEMANTIC_AUG_MULTI_VUL

#ifdef SEMANTIC_JAN_SPADES_GAME
#define INPUT_HOLDINGS  spades_nv
#define INPUT_TRUMPS    SOL_SPADES
#define INPUT_ON_LEAD   WEST
uint spades_nv[DDS_HANDS][DDS_SUITS] =
{ // North                    East        South       West
   { RK | RQ | RJ | RT | R9 | R8,   0,     0,         0  } , // spades
   { R7 | R5 | R3,                  0,     0,         0  } , // hearts
   { R6  ,                          0,     0,         0  } , // diamonds
   { RQ | R7 | R4,                  0,     0,         0  }   // clubs
};
#endif // SEMANTIC_JAN_SPADES_GAME

#ifdef SEMANTIC_AUG_SPLIT_FIT
#define INPUT_HOLDINGS  holdings_aug_split_fit
#define INPUT_TRUMPS    SOL_SPADES
#define INPUT_ON_LEAD   WEST
uint holdings_aug_split_fit[DDS_HANDS][DDS_SUITS] =
{ // North                    East        South       West
	{ RK | RT | R9 | R4,      0,          0,         0  } , // spades
	{ R6 | R2,                0,          0,         0  } , // hearts
	{ RQ | RT | R8 | R7 | R3 | R2,        0,          0,         0  } , // diamonds
	{ RK                ,     0,          0,         0  }   // clubs
};
#endif // SEMANTIC_AUG_SPLIT_FIT

#ifdef SEMANTIC_DEC_ACCEPT_TO_4S
#define INPUT_HOLDINGS  holdings_dec_compet
#define INPUT_TRUMPS    SOL_SPADES
#define INPUT_ON_LEAD   WEST
uint holdings_dec_compet[DDS_HANDS][DDS_SUITS] =
{ // North                    East        South       West
   { RK | RJ | R8 | R7,      0,          0,         0  } , // spades
   { RK | RJ | R8 | R6,      0,          0,         0  } , // hearts
   { R5,                     0,          0,         0  } , // diamonds
   { RK | RQ | R7 | R3,      0,          0,         0  }   // clubs
};
#endif // SEMANTIC_DEC_ACCEPT_TO_4S

#ifdef SEMANTIC_DEC_JUMP_TO_4S
#define INPUT_HOLDINGS  holdings_dec_compet
#define INPUT_TRUMPS    SOL_SPADES
#define INPUT_ON_LEAD   WEST
uint holdings_dec_compet[DDS_HANDS][DDS_SUITS] =
{ // North                    East        South       West
   { RA | RQ | RT | R2,      0,          0,         0  } , // spades
   { RQ | R9,                0,          0,         0  } , // hearts
   { RQ | RT | R7,           0,          0,         0  } , // diamonds
   { R9 | R8 | R4 | R2,      0,          0,         0  }   // clubs
};
#endif // SEMANTIC_DEC_JUMP_TO_4S

#ifdef SEMANTIC_JULY_AUTO_FITO_PLANKTON
#define INPUT_HOLDINGS  fito_july
#define INPUT_TRUMPS    SOL_HEARTS
#define INPUT_ON_LEAD   WEST
uint fito_july[DDS_HANDS][DDS_SUITS] =
{ // North                    East        South       West
  { RT | R7 | R6 | R2,      0,          0,         0  } , // spades
  { R8 | R4 | R3,           0,          0,         0  } , // hearts
  { RA | R9 ,               0,          0,         0  } , // diamonds
  { RK | RT | R9 | R2 ,     0,          0,         0  }   // clubs
};
#endif // SEMANTIC_JULY_AUTO_FITO_PLANKTON

#ifdef SEMANTIC_JUNE_MAX_5D_LEAD
#define INPUT_HOLDINGS  july_max_5dX_holdings
#define INPUT_TRUMPS    SOL_DIAMONDS
#define INPUT_ON_LEAD   WEST
uint july_max_5dX_holdings[DDS_HANDS][DDS_SUITS] =
{ // North      East        South      West
	{ 0,         0,          0,         R3 | R2  } ,                 // spades
	{ 0,         0,          0,         RA | RQ | RT | R6 | R2  } ,  // hearts
	{ 0,         0,          0,         RA | R7 | R4  } ,            // diamonds
	{ 0,         0,          0,         RK | RQ | R7}                // clubs
};
#define CARD_LEAD_SPADE   K3
#define CARD_LEAD_HEARTS  KA
#define CARD_LEAD_DIAMD   K4
#define CARD_LEAD_CLUBS   KK
#endif // SEMANTIC_JUNE_MAX_5D_LEAD

#ifdef SEMANTIC_AUG_LEAD_VS_3H
// origin match at home
#define INPUT_HOLDINGS  aug_lead_vs_3h
#define INPUT_TRUMPS    SOL_HEARTS
#define INPUT_ON_LEAD   WEST
uint aug_lead_vs_3h[DDS_HANDS][DDS_SUITS] =
{ // North      East        South      West
	{ 0,         0,          0,         RQ | RT  } ,      // spades
	{ 0,         0,          0,         RQ | RT  } ,      // hearts
	{ 0,         0,          0,         RT | R7 | R6 | R5 | R4  } , // diamonds
	{ 0,         0,          0,         RA | R9 | R6 | R3 }          // clubs
};
#define CARD_LEAD_SPADE   KQ
#define CARD_LEAD_HEARTS  KQ
#define CARD_LEAD_DIAMD   K7
#define CARD_LEAD_CLUBS   K3
#endif // SEMANTIC_AUG_LEAD_VS_3H


#ifdef SEMANTIC_JUNE_LEAD_3343
// origin https://www.gambler.ru/tour/762803/protocol?id=71379460&teamid=53269
#define INPUT_HOLDINGS  july_lead4333_holdings
#define INPUT_TRUMPS    SOL_NOTRUMP
#define INPUT_ON_LEAD   WEST
uint july_lead4333_holdings[DDS_HANDS][DDS_SUITS] =
{ // North      East        South      West
	{ 0,         0,          0,         R9 | R8 | R7  } ,      // spades
	{ 0,         0,          0,         R9 | R8 | R7  } ,      // hearts
	{ 0,         0,          0,         RA | RJ | R6 | R5  } , // diamonds
	{ 0,         0,          0,         RJ | R4 | R2}          // clubs
};
#define CARD_LEAD_SPADE   K9
#define CARD_LEAD_HEARTS  K9
#define CARD_LEAD_DIAMD   K6
#define CARD_LEAD_CLUBS   KJ
#endif // SEMANTIC_JUNE_LEAD_3343

#ifdef SEMANTIC_JUNE_ZAKHAROVY_PREC_3NT
// origin match at home
#define INPUT_HOLDINGS  julyVZ_holdings
#define INPUT_TRUMPS    SOL_NOTRUMP
#define INPUT_ON_LEAD   WEST
uint julyVZ_holdings[DDS_HANDS][DDS_SUITS] =
{ // North      East        South      West
   { 0,         0,          0,         RA | RQ | R8  } ,      // spades
   { 0,         0,          0,         RJ | R7 | R4 | R2  } , // hearts
   { 0,         0,          0,         RJ | R9 | R6 | R4  } , // diamonds
   { 0,         0,          0,         R9 | R4  }             // clubs
};
uint julyVZ_Axx_holdings[DDS_HANDS][DDS_SUITS] =
{ // North      East        South      West
   { 0,         0,          0,         RA | R8 | R2  } ,      // spades
   { 0,         0,          0,         RJ | R7 | R4 | R2  } , // hearts
   { 0,         0,          0,         RJ | R9 | R6 | R4  } , // diamonds
   { 0,         0,          0,         R9 | R4  }             // clubs
};
#define CARD_LEAD_SPADE   KA
#define CARD_LEAD_HEARTS  K2
#define CARD_LEAD_DIAMD   K4
#define CARD_LEAD_CLUBS   K9
#endif // SEMANTIC_JUNE_ZAKHAROVY_PREC_3NT

#ifdef SEMANTIC_IMPS_LEAD_LEVKOVICH
// origin http://spb.bridgesport.ru/spb/SpbImp20/p2011281/d19p.php
#define INPUT_HOLDINGS  novLevk_holdings
#define INPUT_TRUMPS    SOL_NOTRUMP
#define INPUT_ON_LEAD   WEST
uint novLevk_holdings[DDS_HANDS][DDS_SUITS] =
{ // North      East        South      West
   { 0,         0,          0,         R8 | R7 | R4  } ,      // spades
   { 0,         0,          0,         RQ | RT | R9 | R7  } , // hearts
   { 0,         0,          0,         RQ | RT | R9 | R7  } , // diamonds
   { 0,         0,          0,         RT | R5  }             // clubs
};
#define CARD_LEAD_SPADE   K8
#define CARD_LEAD_HEARTS  KT
#define CARD_LEAD_DIAMD   KT
#define CARD_LEAD_CLUBS   KT
#endif // SEMANTIC_IMPS_LEAD_LEVKOVICH

#ifdef SEMANTIC_DEC_JAN_DBL_THEN_HEARTS
#define INPUT_HOLDINGS  dbl_then_hearts
#define INPUT_TRUMPS    SOL_HEARTS
#define INPUT_ON_LEAD   EAST
uint dbl_then_hearts[DDS_HANDS][DDS_SUITS] =
{ // North                         East        South       West
   { RK | R8 | R5,                 0,          0,         0  } , // spades
   { RA | RK | RQ | RJ | RT | R5,  0,          0,         0  } , // hearts
   { RA | RQ | R7 | R2,            0,          0,         0  } , // diamonds
   { 0                  ,          0,          0,         0  }   // clubs
};
#endif // SEMANTIC_DEC_JAN_DBL_THEN_HEARTS

#ifdef SEMANTIC_TRICOLOR_STRONG
   #define INPUT_HOLDINGS  tri_sunday
   #define INPUT_TRUMPS    SOL_HEARTS
   #define INPUT_ON_LEAD   WEST
   uint tristrong_holdings[DDS_HANDS][DDS_SUITS] =   // 21 hcp
   { // North                    East        South       West
      { RA | RK | R8 | R5,      0,          0,         0  } , // spades
      { RK | RT | R7 | R5,      0,          0,         0  } , // hearts
      { RA ,                    0,          0,         0  } , // diamonds
      { RA | RK | R8 |  R5 ,    0,          0,         0  }   // clubs
   };
   uint tri19_holdings[DDS_HANDS][DDS_SUITS] =
   { // North                    East        South       West
      { RA | RK | R8 | R5,      0,          0,         0  } , // spades
      { RK | RT | R7 | R5,      0,          0,         0  } , // hearts
      { RA ,                    0,          0,         0  } , // diamonds
      { RA | RJ | R8 | R5 ,     0,          0,         0  }   // clubs
   };
   uint tri18_holdings[DDS_HANDS][DDS_SUITS] =
   { // North                    East        South       West
      { RA | RK | R8 | R5,      0,          0,         0  } , // spades
      { RK | RT | R7 | R5,      0,          0,         0  } , // hearts
      { RA ,                    0,          0,         0  } , // diamonds
      { RA | R9 | R8 | R5 ,     0,          0,         0  }   // clubs
   };
   uint tri16_holdings[DDS_HANDS][DDS_SUITS] =
   { // North                    East        South       West
      { RA | RJ | R8 | R5,      0,          0,         0  } , // spades
      { RK | RT | R7 | R5,      0,          0,         0  } , // hearts
      { RA ,                    0,          0,         0  } , // diamonds
      { RA | R9 | R8 | R5 ,     0,          0,         0  }   // clubs
   };
   uint tri16_c6_holdings[DDS_HANDS][DDS_SUITS] =
   { // North                    East        South       West
      { RA | RK | R8 | R5,      0,          0,         0  } , // spades
      { RK | RJ | R7 | R5,      0,          0,         0  } , // hearts
      { R8 ,                    0,          0,         0  } , // diamonds
      { RA | RJ | R8 | R5 ,     0,          0,         0  }   // clubs
   };
   //AKJ9
   //A764
   //J
   //A1032
   uint tri_sunday[DDS_HANDS][DDS_SUITS] =
   { // North                    East        South       West
      { RA | RK | RJ | R9,      0,          0,         0  } , // spades
      { RA | R7 | R6 | R4,      0,          0,         0  } , // hearts
      { RJ ,                    0,          0,         0  } , // diamonds
      { RA | RT | R3 | R2 ,     0,          0,         0  }   // clubs
   };
#endif // SEMANTIC_RED55_KINGS_PART_15_16

#ifdef SEMANTIC_RED55_KINGS_PART_15_16
   #define INPUT_HOLDINGS  red55_holdings
   #define INPUT_TRUMPS    SOL_HEARTS
   #define INPUT_ON_LEAD   EAST
   uint red55_holdings[DDS_HANDS][DDS_SUITS] =
   { // North                    East        South       West
      { RT | R4,                  0,          0,         0  } , // spades
      { RK | R9 | R7 | R5 | R2,   0,          0,         0  } , // hearts
      { RK | R8 | R7 | R6 | R5,   0,          0,         0  } , // diamonds
      { R6 ,                      0,          0,         0  }   // clubs
   };
#endif // SEMANTIC_RED55_KINGS_PART_15_16

#ifdef STUB_DUMMY_HOLDINGS
   #define INPUT_HOLDINGS  dummy_holdings
   #define INPUT_TRUMPS    SOL_HEARTS
   #define INPUT_ON_LEAD   EAST
   uint dummy_holdings[DDS_HANDS][DDS_SUITS] =
   { // North                    East        South       West
      { RT | R4,                  0,          0,         0  } , // spades
      { RK | R9 | R7 | R5 | R2,   0,          0,         0  } , // hearts
      { RK | R8 | R7 | R6 | R5,   0,          0,         0  } , // diamonds
      { R6 ,                      0,          0,         0  }   // clubs
   };
#endif // SINGLE_HAND_TASK

#ifdef SEMANTIC_SPADE_4_WHEN_1H
   #define INPUT_HOLDINGS  spade_4_holdings
   #define INPUT_TRUMPS    SOL_HEARTS
   #define INPUT_ON_LEAD   EAST
   uint spade_4_holdings[DDS_HANDS][DDS_SUITS] =
   { // North                    East        South       West
      { RA | R7 | R5 | R4,       0,          0,         0  } , // spades
      { RK | R9 ,            0,          0,         0  } , // hearts
      { RK | R6 | R5,            0,          0,         0  } , // diamonds
      { RQ | RJ | R6 | R2,           0,          0,         0  }   // clubs
   };
#endif // SEMANTIC_SPADE_4_WHEN_1H

#ifdef INPUT_AS_PBN
   uint zero_holdings[DDS_HANDS][DDS_SUITS] =
   { // North    East        South       West
      { 0,       0,          0,         0  } , // spades
      { 0,       0,          0,         0  } , // hearts
      { 0,       0,          0,         0  } , // diamonds
      { 0,       0,          0,         0  }   // clubs
   };
   #define INPUT_HOLDINGS  zero_holdings
#endif

uint(*input_holdings)[DDS_HANDS][DDS_SUITS] = &INPUT_HOLDINGS;

u64  wa_SuitByDds[DDS_SUITS] = { SPADS, HEART, DIAMD, CLUBS };
uint wa_PosByDds [DDS_SUITS] = {    48,    32,    16,     0 };
void Walrus::WithdrawByInput(void)
{
   #ifdef INPUT_AS_PBN
   {
      // convert to temp
      ddTableDeal pbnDeal;
      if (ConvertFromPBN(taskHandPBN, pbnDeal.cards) != 1) {
         printf("\nERROR: Cannot parse PBN: %s\n", taskHandPBN);
         return;
      }

      // store
      for (int h = 0; h < DDS_HANDS; h++) {
         for (int s = 0; s < DDS_SUITS; s++) {
            (*input_holdings)[s][h] = pbnDeal.cards[h][s];
         }
      }
   }
   #endif 

   // usual withdraw
   for (int h = 0; h < DDS_HANDS; h++) {
      for (int s = 0; s < DDS_SUITS; s++) {
         auto hld = (*input_holdings)[s][h];
         WithdrawHolding(hld, wa_PosByDds[s]);
      }
   }

   shuf.AssertDeckSize(SYMM3);
}

uint CountBits(uint v)// count bits set in this (32-bit value)
{
   uint c; // store the total here
   static const int S[] = { 1, 2, 4, 8, 16 }; // Magic Binary Numbers
   static const int B[] = { 0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF, 0x0000FFFF };

   c = v - ((v >> 1) & B[0]);
   c = ((c >> S[1]) & B[1]) + (c & B[1]);
   c = ((c >> S[2]) + c) & B[2];
   c = ((c >> S[3]) + c) & B[3];
   c = ((c >> S[4]) + c) & B[4];

   return c;
}

void Walrus::PrepareBaseDeal(deal& dlBase)
{
   dlBase.trump = INPUT_TRUMPS;
   dlBase.first = INPUT_ON_LEAD;

   dlBase.currentTrickSuit[0] = 0;
   dlBase.currentTrickSuit[1] = 0;
   dlBase.currentTrickSuit[2] = 0;

   dlBase.currentTrickRank[0] = 0;
   dlBase.currentTrickRank[1] = 0;
   dlBase.currentTrickRank[2] = 0;

   uint countCards = 0;
   for (int h = 0; h < DDS_HANDS; h++) {
      for (int s = 0; s < DDS_SUITS; s++) {
         auto holding = (*input_holdings)[s][h];
         dlBase.remainCards[h][s] = holding;
         countCards += CountBits(holding);
      }
   }

   if (countCards != SYMM) {
      printf("\nERROR: Wrong count of cards in hand: %d\n", countCards);
   }
}

void DdsTricks::Init(futureTricks& fut)
{
   // plainScore is good for any goal
   plainScore = 13 - fut.score[0];

   // the rest is for opening lead
#ifdef SEEK_OPENING_LEAD
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
         lead.Ñ = 13 - fut.score[i];
         continue;
      }
   }
#endif // SEEK_OPENING_LEAD
}


