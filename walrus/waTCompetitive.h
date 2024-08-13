/* ---------------------------------------------------------------- -
   --- DESCRIPTIONS with useful defines:
   #define SEEK_BIDDING_LEVEL
   #define SEEK_MAGIC_FLY
   #define SEEK_DENOMINATION
   #define SEEK_DECISION_BID_OR_DOUBLE
   #define SEEK_SACRIFICE_DECISION

   #define SCORE_THE_OTHER_CONTRACT
   #define OC_TRUMPS       SOL_NOTRUMP
   #define OC_ON_LEAD      WEST
   #define THE_OTHER_IS_OURS

   #define SHOW_OPPS_ON_PASS_ONLY
   #define SHOW_OPPS_ON_DOUBLE_ONLY
   #define SHOW_PARTSCORE_STATLINE

   #define FOUR_HANDS_TASK
*/
#pragma once

#ifdef SEM_AUG_BLITZ_VS_DIMULYA
   #define SEEK_DECISION_BID_OR_DOUBLE
   #define SEM_ORBITING_FIXED_HAND
#endif 

#ifdef SEM_AUG_TRICOLOR_OVER_OPP_3S
   #define SEEK_SACRIFICE_DECISION
   #define SEM_ORBITING_FIXED_HAND
#endif 

#ifdef SEM_AUG_PREEMPTIVE_ON_PART_1H
   #define SEEK_SACRIFICE_DECISION
   #define SEM_ORBITING_FIXED_HAND
#endif 

#ifdef SEM_DU_MAR_1C_X_XX_1H_ETC_MAR
	#define TITLE_VERSION  "Fix 1363 8hcp, all vul;\n  1c (X) XX 1h\n2d (2h) 3d (3h)\n X p ??\nQuestion: pass or 4d?"
	#define SEEK_DECISION_BID_OR_DOUBLE
	#define OC_TRUMPS       SOL_HEARTS
	#define OC_ON_LEAD      SOUTH
	#define SHOW_OPPS_ON_DOUBLE_ONLY
#endif 

#ifdef SEMANTIC_OCT_INDIAN_5C
   #define TITLE_VERSION  "Fix 5530 9hcp, all vul;\n  1c (2c) 2s two suiter spade and a red suit (4c)\n  4s (pass) pass (5c)\n  dbl pass or 5s? Task "
   #define SEEK_DECISION_BID_OR_DOUBLE
   #define SCORE_THE_OTHER_CONTRACT
   #define OC_TRUMPS       SOL_CLUBS
   #define OC_ON_LEAD      NORTH
   #define SHOW_OPPS_ON_DOUBLE_ONLY
#endif 

#ifdef SEMANTIC_JUN_MINORS_VS_MAJORS
   #define TITLE_VERSION  "Fix 3343 9hcp, fav vul;\npass-(1s)-2d-(2h)\n3d-(3h)-4c-(4h)-??\nQuestion: dbl, pass or 5c? Task "
   #define SEEK_SACRIFICE_DECISION
   #define SHOW_OPPS_ON_PASS_ONLY
   #define OC_TRUMPS       SOL_HEARTS
   #define OC_ON_LEAD      NORTH
#endif


#ifdef SEMANTIC_OVERCALL_4C
   #define TITLE_VERSION  "Fix 0346 9hcp, unfav vul;\n(1s)-pass-(3s inv)-??\nQuestion: pass or 4c? Task "
   #define SEEK_SACRIFICE_DECISION
   #define SHOW_OPPS_ON_PASS_ONLY
   #define OC_TRUMPS       SOL_SPADES
   #define OC_ON_LEAD      NORTH
#endif // SEMANTIC_OVERCALL_4C

