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
   #define IO_NEED_FULL_TABLE
   #define FOUR_HANDS_TASK
   #define STUB_DUMMY_HOLDINGS
   #define INPUT_TRUMPS    SOL_SPADES
   #define IO_SHIFT_FOR_EXTRA_MARKS  21
   #define IO_ROW_SELECTED  (11 + IO_SHIFT_FOR_EXTRA_MARKS)
   #define IO_ROW_FILTERING (3  + IO_SHIFT_FOR_EXTRA_MARKS)
   //#define PERCENTAGES_IN_ANSWER_ROW
#endif // SEMANTIC_SPLINTER_SHAPE

