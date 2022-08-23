// -----------------------------------------------------------------
// --- DESCRIPTIONS
#ifdef SEMANTIC_AUG_3NT_ON_SPADE_FIT
   #define TITLE_VERSION  "Max scoring. Fix a 9 hcp with doubleton fit;\n1c 1h\n1s 1NT\n2s 2NT\n3NT ??\nQuestion: pass 3NT or 4s? Task"
   #define SEEK_MAGIC_FLY
#endif 
#ifdef SEMANTIC_AUG_3NT2_ON_SPADE_FIT
   #define TITLE_VERSION  "Max scoring. Fix a 8 hcp with doubleton fit;\n1c 1dh\n1s 1NT\n2s 2NT(!)\n3d/3s/3NT ??\nQuestion: pass 3NT or 4s? Task"
   #define SEEK_MAGIC_FLY
#endif
#ifdef SEMANTIC_AUG_3NT_ON_44H
   #define TITLE_VERSION  "Max scoring. Fix a 7 hcp with 3433;\n2NT ??\nQuestion: pass 3NT or seek 4h? Task"
   #define SEEK_MAGIC_FLY
#endif

// ---
#ifdef SEMANTIC_DEC_12_2425
   #define TITLE_VERSION  "Fix a 12 hcp with no fit;\n1s 1NT\npass\nQuestion: did we miss the 3NT? Task"
   #define SEEK_BIDDING_LEVEL
#endif 

// ---
#ifdef SEMANTIC_IMPS_ACCEPT_3NT_ON_SPADE_FIT
   #define TITLE_VERSION  "Fix a 8 hcp with split/fit;\n1s 2h(split)\n3s ??\nQuestion: partscore 3s or a game 3NT? Task"
   #define SEEK_DENOMINATION
#endif 

// ---
#ifdef SEMANTIC_NOV_64_AS_TWO_SUITER
   #define TITLE_VERSION  "Fix a 10 hcp 6-4;\n2s 3s\n??\nQuestion: partscore or a game? v2.0"
   #define SEEK_BIDDING_LEVEL
#endif 

// ---
#ifdef SEMANTIC_NOV_VOIDWOOD
   #define TITLE_VERSION  "Fix a 3730 three aces;\n1nt(11-13) 2d\n2h 5c -- opp doubled that\nXX(zero of 5) ??\nQuestion: a game or a slam? v2.0"
   #define SEEK_BIDDING_LEVEL
   #define UNPLAYABLE_ONE_OF  6    // adjust to singleton king offside -- go down in 1/6 of such cases
#endif 

// ---
#ifdef SEMANTIC_APR_64_INVITE
   #define TITLE_VERSION  "Fix a 4162 8 hcp;\n 1NT - ??\nQuestion: a game or a invtie? v2.0"
   #define SEEK_BIDDING_LEVEL
#endif 

// ---
#ifdef SEMANTIC_SEPT_MANTICORA_14_16
   #define TITLE_VERSION  "Fix a Sash hand 7hcp;\n  ??\nQuestion: a game or a partscore? v2.0"
   #define SEEK_BIDDING_LEVEL
#endif 

// ---
#ifdef SEMANTIC_JAN_NT_SLAM_ON_DIAMONDS
   #define TITLE_VERSION  "Fix a 2263 16 hcp;\n 1NT - ??\nQuestion: a game or a slam? v2.0"
   #define SEEK_BIDDING_LEVEL
#endif 

// ---
#ifdef SEMANTIC_MORO_SLAM
   #define TITLE_VERSION  "Fix a 1174 13 hcp;\n 1d - 1s\n3d - 3NT\n??\nQuestion: pass here or a slam? v2.0"
   #define SEEK_BIDDING_LEVEL
#endif 

// ---
#ifdef SEMANTIC_SEPT_MAJORS54_18HCP
   #define TITLE_VERSION  "Fix a 18 hcp;\n1c 1d\n1s 1NT\n2h 3h\nQuestion: partscore or a game? v2.0"
   #define SEEK_BIDDING_LEVEL
#endif 

#ifdef SEMANTIC_JUNE_GAMBLE_OR_5C
   #define TITLE_VERSION  "Fix 3307 fourth hand;\n(2h)-p-(p)-??\nQuestion: 3NT, partscore or 5C? Ver 2.0 "
   #define SEEK_DENOMINATION
#endif 

#ifdef SEMANTIC_RED55_KINGS_PART_15_16
   #define TITLE_VERSION  "Fix 55 responder; 1NT-2d-3d-?? Question: partscore or a game? v2.0"
   #define SEEK_BIDDING_LEVEL
#endif 

#ifdef SEMANTIC_JAN_SPADES_GAME
   #define TITLE_VERSION  "Fix an 8 hcp, part 14-16;\n....s\nQuestion: partscore or a game? v2.0"
   #define SEEK_BIDDING_LEVEL
#endif

#ifdef SEMANTIC_AUG_MULTI_VUL
   #define TITLE_VERSION  "Fix a 15 hcp;\n....s\nQuestion: partscore or a game? v2.0"
   #define SEEK_BIDDING_LEVEL
#endif

