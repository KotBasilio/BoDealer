/* ---------------------------------------------------------------- -
   --- DESCRIPTIONS with useful defines:
   #define SEEK_BIDDING_LEVEL
   #define SEEK_MAGIC_FLY
   #define SEEK_DENOMINATION
   #define SEEK_DECISION_BID_OR_DOUBLE
   #define SEEK_DECISION_WHETHER_DOUBLE
   #define SEEK_OPENING_LEAD
   #define SEEK_SACRIFICE_DECISION

   #define SCORE_THE_OTHER_CONTRACT
   #define THE_OTHER_IS_OURS
   #define OC_TRUMPS       SOL_NOTRUMP
   #define OC_ON_LEAD      WEST

   #define SHOW_OPP_RESULTS
   #define SHOW_OPPS_ON_PASS_ONLY
   #define SHOW_OPPS_ON_DOUBLE_ONLY
   #define SHOW_PARTSCORE_STATLINE

   #define FOUR_HANDS_TASK
*/

#ifdef SEMANTIC_OCT_SEEK_6D
   #define TITLE_VERSION  "Fix a 19 hcp 2434;\n1d 1h 1NT 2d 2s\nQuestion: How often 6d makes? Task"
   #define SEEK_BIDDING_LEVEL
   #define IO_LAYOUT_3_0
   #define IO_HCP_MIN  29
   #define IO_HCP_MAX  35
#endif 

#ifdef SEMANTIC_JULY_MOROZOV_HAND_EVAL
   #define TITLE_VERSION  "Morozov's lecture hand evaluation in NT. Task"
   #define SEEK_BIDDING_LEVEL
#endif 

