// -----------------------------------------------------------------
// --- DESCRIPTIONS

#ifdef SEMANTIC_OCT_WEAK_GAMBLING
   #define TITLE_VERSION  "Fix weak Gambling hand 2047;\n2c p 3d(trf) 3s\n??\nQuestion: 4c or 5c? Task"
   #define SEEK_BIDDING_LEVEL
   #define SHOW_PARTSCORE_STATLINE
   #define SCORE_THE_OTHER_CONTRACT
   #define OC_TRUMPS       SOL_SPADES
   #define OC_ON_LEAD      NORTH
#endif

#ifdef SEMANTIC_MIXED_PREVENTIVE_4S
   #define TITLE_VERSION  "Fix some 2 hcp with 6331;\n1NT X 2h(trf) X\n2s(fit 3 or 4) 3h ??\nQuestion: 3s or 4s? Task"
   #define SEEK_BIDDING_LEVEL
   #define SCORE_THE_OTHER_CONTRACT
   #define SHOW_OPPS_ON_PASS_ONLY
   #define OC_TRUMPS       SOL_HEARTS
   #define OC_ON_LEAD      NORTH
#endif 

// ---
#ifdef SEMANTIC_NOV_DBL_ON_3NT
   #define TITLE_VERSION  "Fix some even values 8hcp;\nOpps bid:\np-1c\n1d-1nt(17-21)\n2h 3NT\n\nQuestion: maybe double that? v2.0"
   #define SEEK_DECISION_OVER_DOUBLE
   #define SHOW_OPP_RESULTS
#endif 

#ifdef SEMANTIC_DEC21_DBL_ON_3NT
#define TITLE_VERSION  "Fix some even values 9hcp;\nOpps bid:\n1c-1h\n1nt-2h\n2NT 3NT\n\nQuestion: maybe double that? v2.0"
//#define SEEK_DECISION_OVER_DOUBLE
#define SEEK_OPENING_LEAD
//#define SHOW_OPP_RESULTS
#endif

#ifdef SEMANTIC_MIXED_2D_WITH_MAJOR
   #define TITLE_VERSION  "Fix some even values 7hcp;\nQuestion: maybe pass that? v2.0"
   #define SEEK_BIDDING_LEVEL
   #define SHOW_OPP_RESULTS
#endif 

// ---
#ifdef SEMANTIC_NOV_BID_6C_OR_DBL_4S
   //#define TITLE_VERSION  "Fix 17 hcp hand;\n1c-(1s)-dbl-(3s)\n-3NT-(4s)-dbl-(p)-??\nAssume partner 12+\nQuestion: dbl, 5 clubs / 6 clubs? Ver 2.0 "
   //#define TITLE_VERSION  "Fix 17 hcp hand;\n1c-(1s)-dbl-(3s)\n-3NT-(4s)-dbl-(p)-??\nAssume partner (10-11)\nQuestion: dbl, 5 clubs / 6 clubs? Ver 2.0 "
   #define TITLE_VERSION  "Fix 17 hcp hand;\n1c-(1s)-dbl-(3s)\n-3NT-(4s)-dbl-(p)-??\nAssume partner 13+\nQuestion: dbl, 5 clubs / 6 clubs? Ver 2.0 "
   #define SEEK_BIDDING_LEVEL
   #define SCORE_THE_OTHER_CONTRACT
   #define SHOW_OPPS_ON_DOUBLE_ONLY
   #define OC_TRUMPS       SOL_SPADES
   #define OC_ON_LEAD      SOUTH
#endif 

#ifdef SEMANTIC_FEB_4711_DILEMMA_ON_4S
   #define TITLE_VERSION  "Fix 12 hcp 4711 -- 4 controls;\n1h-(p)-2h-(X)\n4h-(4s)p-(p)-??\nQuestion: double them or bid our 5h? Task "
   #define SEEK_BIDDING_LEVEL
   #define SCORE_THE_OTHER_CONTRACT
   #define SHOW_OPPS_ON_DOUBLE_ONLY
   #define OC_TRUMPS       SOL_SPADES
   #define OC_ON_LEAD      SOUTH
#endif 

// ---
#ifdef SEMANTIC_DEC_BID_5H_OR_DBL_4S
   #define TITLE_VERSION  "Fix heart-top-7 hand;\n1c-(1s)-pass-(3s)\n-4h-(4s)-dbl-(p)-??\nQuestion: dbl or 5h? Ver 2.0 "
   #define SEEK_BIDDING_LEVEL
   #define SCORE_THE_OTHER_CONTRACT
   #define SHOW_OPPS_ON_DOUBLE_ONLY
   #define OC_TRUMPS       SOL_SPADES
   #define OC_ON_LEAD      SOUTH
#endif // SEMANTIC_DEC_BID_5H_OR_DBL_4S


#ifdef SEMANTIC_JULY_AUTO_FITO_PLANKTON
   #define TITLE_VERSION  "Fix 7 hcp hand;\n(1s)-2h-(2s)-3h\n(p)-p-(3s)-??\nQuestion: pass/dbl or a game? Ver 2.0 "
   #define SEEK_BIDDING_LEVEL
   #define SCORE_THE_OTHER_CONTRACT
   #define OC_TRUMPS       SOL_SPADES
   #define OC_ON_LEAD      SOUTH
#endif 

#ifdef SEMANTIC_DEC_JAN_DBL_THEN_HEARTS
   #define TITLE_VERSION  "Fix 19 hcp 3640 -- strong with hearts;\np-(2c)-X-(3c)\np-(p)-??\nQuestion: partscore or a game? Task "
   #define SEEK_BIDDING_LEVEL
#endif 

#ifdef SEMANTIC_TRICOLOR_STRONG
   #define TITLE_VERSION  "Fix tricolor opening;\np-(p)-1c-(1d)\np-(p)-dbl-(2d)\n2h-(p)-??\nQuestion: partscore or a game? Ver 2.0 "
   #define SEEK_BIDDING_LEVEL
#endif 

#ifdef SEMANTIC_AUG_SPLIT_FIT
   #define TITLE_VERSION  "Fix 6-4 responder; (2d)-2s-(dbl)-?? Question: partscore or a game? v2.0"
   #define SEEK_BIDDING_LEVEL
#endif // SEMANTIC_AUG_SPLIT_FIT

#ifdef SEMANTIC_DEC_ACCEPT_TO_4S
   #define TITLE_VERSION  "Fix 4441 13 hcp;\n(p)-p-(1d)-X\n(3d)-3s-(p)-??\nQuestion: partscore or a game? v2.0"
   #define SEEK_BIDDING_LEVEL
#endif // SEMANTIC_DEC_ACCEPT_TO_4S

#ifdef SEMANTIC_DEC_JUMP_TO_4S
   #define TITLE_VERSION  "Fix 4234 10 hcp;\n(p)-p-(1d)-X\n(3d)-??\nQuestion: 3s or 4s? v2.0"
   #define SEEK_BIDDING_LEVEL
#endif // SEMANTIC_DEC_JUMP_TO_4S

