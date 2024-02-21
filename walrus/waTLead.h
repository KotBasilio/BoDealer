// -----------------------------------------------------------------
// --- DESCRIPTIONS
// -------------------------------------------------------- 

#ifdef SEMANTIC_JAN_PETYA_VS_3NT
   #define TITLE_VERSION  "Fix a 10 hcp ;\n 1NT - 3NT\nQuestion: what to lead?\nVer 2.0 "
   #define SEEK_OPENING_LEAD
#endif // SEMANTIC_JAN_PETYA_VS_3NT

// ---
#ifdef SEMANTIC_AUG_LEAD_VS_3H
   #define TITLE_VERSION  "Fix a hand with an ace;\n....s\nQuestion: what to lead?\nVer 2.0 "
   #define SEEK_OPENING_LEAD
#endif 

// ---
#ifdef SEMANTIC_IMPS_LEAD_LEVKOVICH
   #define TITLE_VERSION  "Fix a hand with an 3-4 majors;\n3d-p-3NTs\nQuestion: what to lead?\nVer 2.0 "
   #define SEEK_OPENING_LEAD
   #define PERCENTAGES_IN_ANSWER_ROW
#endif 

#ifdef SEMANTIC_JUNE_MAX_5D_LEAD
   #define TITLE_VERSION  "Fix a strong hand;\n(p)-p-(1d)-1h\n(3d)-3h-(p)-4h\n(p)-p-(5d)-X-all pass\nQuestion: what to lead?\nVer 2.0 "
   #define SEEK_OPENING_LEAD
#endif 

#ifdef SEMANTIC_JUNE_LEAD_3343
   #define TITLE_VERSION  "Fix a weak hand;\np-(p)-p-(1s)\np-(1NT)-p-(3NT)-all pass\nQuestion: what to lead?\nVer 2.0 "
   #define SEEK_OPENING_LEAD
#endif 

#ifdef SEMANTIC_JUNE_ZAKHAROVY_PREC_3NT
   #define TITLE_VERSION  "Fix a weak hand;\n(2c)-p-(3NT)-all pass\nQuestion: what to lead?\nVer 2.0 "
   #define SEEK_OPENING_LEAD
#endif 



/// -----------------------------------------------------------------
/// ONE HAND TASKS
#ifdef SEMANTIC_KEYCARDS_10_12
   #define TITLE_VERSION  "Abstract 10-12 with 4+ major;\nQuestion: how keycards are distributed? Ver 2.0 "
   #define SINGLE_HAND_TASK
   #define PERCENTAGES_IN_ANSWER_ROW
#endif 

// ---
#ifdef SEMANTIC_SPADE_4_WHEN_1H
   #define TITLE_VERSION  "Part opened 1H. We hold ~4333 with 4s;\nQuestion: how often part has 4+ spades? Ver 2.0 "
   #define PARTNER_HAND_TASK
   #define PERCENTAGES_IN_ANSWER_ROW
   #define ANSWER_ROW_IDX  1
   #define IO_NEED_FULL_TABLE
#endif

/// -----------------------------------------------------------------
/// 4 HAND TASKS
#ifdef SEMANTIC_SPLINTER_SHAPE
   //#define TITLE_VERSION  "Only bidding is fixed: 1s pass 4c. Responder holds 4441;\nQuestion: how often opener has Ac, depending on responder shape? Ver 3.0 "
   #define TITLE_VERSION  "Only bidding is fixed: 1s pass 4c. Responder holds 4441; and some other filters\nQuestion: how often a slam makes? Ver 3.0 "
   #define FOUR_HANDS_TASK
   #define INPUT_TRUMPS    SOL_SPADES
   #define IO_HCP_MIN  26
   #define IO_HCP_MAX  30
   //#define PERCENTAGES_IN_ANSWER_ROW
   // #define IO_HCP_MIN  24
   // #define IO_HCP_MAX  27
#endif

#ifdef SEMANTIC_STANDARD_3NT
   #define TITLE_VERSION  "No bidding. 3NT on points. Ver 3.0 "
   #define FOUR_HANDS_TASK
   #define INPUT_TRUMPS    SOL_NOTRUMP
   #define IO_HCP_MIN  23
   #define IO_HCP_MAX  32
#endif

#ifdef SEMANTIC_STANDARD_6NT
   #define TITLE_VERSION  "No bidding. 6NT on points. Ver 3.0 "
   #define FOUR_HANDS_TASK
   #define INPUT_TRUMPS    SOL_NOTRUMP
   #define IO_HCP_MIN  23
   #define IO_HCP_MAX  31
#endif

#ifdef SEMANTIC_CONFIG_BASED
   #define TITLE_VERSION  "Config-based task. Ver 3.0 "
   #define FOUR_HANDS_TASK
   #define INPUT_TRUMPS    SOL_NOTRUMP
#endif

#ifdef SEMANTIC_1NT_OR_SIGN_OFF
   #define TITLE_VERSION  "1c 1s\n1NT ?? Pass or a spade sign-off. Ver 3.0 "
   #define FOUR_HANDS_TASK
   //#define INPUT_TRUMPS    SOL_SPADES
   #define INPUT_TRUMPS    SOL_NOTRUMP
   #define IO_HCP_MIN  19
   #define IO_HCP_MAX  24
#endif

#ifdef SEMANTIC_4M_ON54_BIG
   #define TITLE_VERSION  "Weak nt, 54 fit, 5332 to hearts doubleton 4234/4243."
   #define FOUR_HANDS_TASK
   #define INPUT_TRUMPS    SOL_SPADES
   #define INPUT_ON_LEAD   WEST
   #define IO_HCP_MIN  22
   #define IO_HCP_MAX  25
#endif

