// -----------------------------------------------------------------
// --- DESCRIPTIONS 4 HAND TASKS
// -------------------------------------------------------- 

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


#ifdef SEMANTIC_ALL4_1H_1S_2H_MAR
   #define TITLE_VERSION  "North is some 11 hcp =4333;\n1h 1s\n2h ??\nQuestion: how often h game makes?"
   #define FOUR_HANDS_TASK
   #define INPUT_TRUMPS    SOL_HEARTS
   #define INPUT_ON_LEAD   WEST
   #define IO_HCP_MIN  22
   #define IO_HCP_MAX  24
#endif

#ifdef SEM_ALL4_1C_P_1H_2D_2H_MAR
	#define TITLE_VERSION  "North is some 11 hcp 2542;\n1c p 1h\n2d 2h p ??\nQuestion: how often h game makes?"
	#define FOUR_HANDS_TASK
	#define IO_HCP_MIN  22
	#define IO_HCP_MAX  24
	#define INPUT_TRUMPS    SOL_HEARTS
	#define INPUT_ON_LEAD   WEST
#endif

