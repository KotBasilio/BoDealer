/* ---------------------------------------------------------------- -
   --- DESCRIPTIONS with useful defines:
   #define SEEK_BIDDING_LEVEL
   #define SEEK_MAGIC_FLY
   #define SEEK_DENOMINATION
   #define SEEK_DECISION_BID_OR_DOUBLE
   #define SEEK_SACRIFICE_DECISION

   #define SCORE_THE_OTHER_CONTRACT
   #define THE_OTHER_IS_OURS

   #define SHOW_OPP_RESULTS
   #define SHOW_OPPS_ON_PASS_ONLY
   #define SHOW_OPPS_ON_DOUBLE_ONLY
   #define SHOW_PARTSCORE_STATLINE

   #define FOUR_HANDS_TASK
*/

#ifdef SEM_ONE_SIDED_BIDDING_LEVEL
   #define SEEK_BIDDING_LEVEL
   #define SEM_ORBITING_FIXED_HAND
#endif 

#ifdef SEP_RESPONCE_TO_2C
   #define SEEK_BIDDING_LEVEL
   #define SEM_ORBITING_FIXED_HAND
#endif 

#ifdef SEM_AUG_JAGODINA_INVITE
   #define SEEK_DENOMINATION
   #define SEM_ORBITING_FIXED_HAND
#endif 

#ifdef SEM_MAR24_5C_OR_3NT
   #define TITLE_VERSION  "Fix a 12 hcp 1435;\n2c p 2d\n3c/2nt p ??\nQuestion: how often 3NT is better than 5C?"
   #define SEEK_DENOMINATION
   #define IO_HCP_MIN  23
   #define IO_HCP_MAX  28
   #define OC_TRUMPS       SOL_NOTRUMP
   #define OC_ON_LEAD      EAST
#endif 

#ifdef SEM_MIX_MAR24_INVITE_4S
   #define TITLE_VERSION  "Fix a 14 hcp 4315;\np 1c p 1d\n1h 1s 2h 2s p ??\nQuestion: how often s game makes?"
   #define SEEK_BIDDING_LEVEL
   #define IO_HCP_MIN  21
   #define IO_HCP_MAX  23
   #define CALC_CLUB_HITS_EXPERIMENTAL
#endif 

#ifdef SEMANTIC_1C_P_1H_2D_2H_MAR
   #define TITLE_VERSION  "Fix a 11 hcp 2542;\n1c p 1h\n2d 2h p ??\nQuestion: how often h game makes?"
   #define SEEK_BIDDING_LEVEL
   #define IO_HCP_MIN  22
   #define IO_HCP_MAX  24
#endif

#ifdef SEMANTIC_1H_1S_2H_MAR
   #define TITLE_VERSION  "Fix a 11 hcp 4333;\n1h 1s\n2h ??\nQuestion: how often h game makes?"
   #define SEEK_BIDDING_LEVEL
   #define IO_HCP_MIN  22
   #define IO_HCP_MAX  25
#endif

#ifdef SEMANTIC_INV_AFTER_2H
   #define TITLE_VERSION  "Fix a 12 hcp 4333;\nLHO has opened 2h (6+, 7-10), part 2S, RHO pass.\nQuestion: What's better -- invit or a game?"
   #define SEEK_BIDDING_LEVEL
   #define IO_HCP_MIN  22
   #define IO_HCP_MAX  25
#endif

#ifdef SEMANTIC_7D_OR_NT_FEB
   #define TITLE_VERSION  "Fix a 19 hcp 2722;\nPart has opened diamonds, shown 6-4 with spaeds, has AKQ in diamonds.\nQuestion: What's better -- 7D or 7NT?"
   #define SEEK_DENOMINATION
   #define IO_HCP_MIN  30
   #define IO_HCP_MAX  32
#endif

#ifdef SEMANTIC_4M_ON54_FEB_PATTON
   #define TITLE_VERSION  "Fix a 10 hcp 5332;\n1NT(10-13) 2h\n2NT ??\nQuestion: How often 4s makes? Task"
   #define SEEK_BIDDING_LEVEL
   #define IO_HCP_MIN  22
   #define IO_HCP_MAX  23
#endif

#ifdef SEMANTIC_54M_FEB_PARTIZAN
   #define TITLE_VERSION  "Fix a 11 hcp 4522;\n1NT(10-13) 2d\n2h ??\nQuestion: Is the hand worth invit? Task"
   #define SEEK_BIDDING_LEVEL
   #define IO_HCP_MIN  23
   #define IO_HCP_MAX  24
#endif

#ifdef SEMANTIC_TANJA_FEB_LONG_H
   #define TITLE_VERSION  "Fix a 8 hcp 2821;\n1s 1NT(?!) 2d ??\nQuestion: How often 4h makes? Task"
   #define SEEK_BIDDING_LEVEL
   #define IO_HCP_MIN  19
   #define IO_HCP_MAX  20
#endif

#ifdef SEMANTIC_BASHA_NOV_NT
   #define TITLE_VERSION  "Fix a 19 hcp 2434;\n1d 1h 1NT 2d 2s\nQuestion: How often 6d makes? Task"
   #define SEEK_BIDDING_LEVEL
   #define IO_HCP_MIN  22
   #define IO_HCP_MAX  25
#endif

#ifdef SEMANTIC_OCT_SEEK_6D
   #define TITLE_VERSION  "Fix a 19 hcp 2434;\n1d 1h 1NT 2d 2s\nQuestion: How often 6d makes? Task"
   #define SEEK_BIDDING_LEVEL
   #define IO_HCP_MIN  29
   #define IO_HCP_MAX  35
#endif 

#ifdef SEMANTIC_JULY_MOROZOV_HAND_EVAL
   #define TITLE_VERSION  "Morozov's lecture hand evaluation in NT. Task"
   #define SEEK_BIDDING_LEVEL
#endif 

