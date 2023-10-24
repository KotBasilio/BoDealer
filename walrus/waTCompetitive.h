/* ---------------------------------------------------------------- -
   --- DESCRIPTIONS with useful defines:
   #define SEEK_BIDDING_LEVEL
   #define SEEK_MAGIC_FLY
   #define SEEK_DENOMINATION
   #define SEEK_DECISION_OVER_DOUBLE
   #define SEEK_OPENING_LEAD
   #define SEEK_SACRIFICE_DECISION
   #define UNPLAYABLE_ONE_OF  6

   #define SCORE_THE_OTHER_CONTRACT
   #define OC_TRUMPS       SOL_NOTRUMP
   #define OC_ON_LEAD      WEST
   #define THE_OTHER_IS_OURS

   #define SHOW_OPPS_ON_PASS_ONLY
   #define SHOW_OPPS_ON_DOUBLE_ONLY
   #define SHOW_PARTSCORE_STATLINE
   #define SHOW_SACRIFICE_RESULTS

   #define FOUR_HANDS_TASK
*/

#ifdef SEMANTIC_OCT_INDIAN_5C
   #define TITLE_VERSION  "Fix 5530 9hcp, all vul;\n: ... pass or 5s? Task "
   #define SEEK_BIDDING_LEVEL
   #define IO_SHOW_MINI_FILTERING
   #define SCORE_THE_OTHER_CONTRACT
   #define OC_TRUMPS       SOL_CLUBS
   #define OC_ON_LEAD      NORTH
   #define SHOW_OPPS_ON_DOUBLE_ONLY
#endif 

#ifdef SEMANTIC_JUN_MINORS_VS_MAJORS
   #define TITLE_VERSION  "Fix 3343 9hcp, fav vul;\npass-(1s)-2d-(2h)\n3d-(3h)-4c-(4h)-??\nQuestion: dbl, pass or 5c? Task "
   #define SEEK_SACRIFICE_DECISION
   #define IO_SHOW_MINI_FILTERING
   #define SHOW_OPPS_ON_PASS_ONLY
   #define OC_TRUMPS       SOL_HEARTS
   #define OC_ON_LEAD      NORTH
#endif

