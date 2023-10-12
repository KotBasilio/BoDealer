/******************************************************************************
 * Micro-filters are a kind of byte-code programming   07.06.2023
 *
 ******************************************************************************/

#define MAX_MICRO_PARAMS 5

typedef uint (WaFilter::* MicroFunc)(twContext* lay, const uint *par);

struct MicroFilter {
   MicroFunc func;
   uint  params[MAX_MICRO_PARAMS];
   char  name[32];
   MicroFilter() : func(nullptr) { params[0] = 0; name[0] = 0; }
   MicroFilter(MicroFunc f, const char *_name, uint p0 = 0, uint p1 = 0, uint p2 = 0, uint p3 = 0, uint p4 = 0);
};

// common reasons for a filter
const uint SKIP_BY_PART = 1;
const uint SKIP_BY_RESP = 2;
const uint SKIP_BY_OPP = 3;
const uint SKIP_BY_DIRECT = SKIP_BY_RESP;
const uint SKIP_BY_SANDWICH = SKIP_BY_OPP;

// macros for adding filters
#define ADD_VOID_FILTER(NAME)                         sem.vecFilters.push_back( MicroFilter(&WaFilter::NAME, #NAME)                        )
#define ADD_0PAR_FILTER(HAND, NAME)                   sem.vecFilters.push_back( MicroFilter(&WaFilter::NAME, #NAME, HAND)                  )
#define ADD_1PAR_FILTER(HAND, NAME, P2)               sem.vecFilters.push_back( MicroFilter(&WaFilter::NAME, #NAME, HAND, P2)              )
#define ADD_2PAR_FILTER(HAND, NAME, P2, P3)           sem.vecFilters.push_back( MicroFilter(&WaFilter::NAME, #NAME, HAND, P2, P3)          )
#define ADD_3PAR_FILTER(HAND, NAME, P2, P3, P4)       sem.vecFilters.push_back( MicroFilter(&WaFilter::NAME, #NAME, HAND, P2, P3, P4)      )
#define ADD_4PAR_FILTER(HAND, NAME, P2, P3, P4, P5)   sem.vecFilters.push_back( MicroFilter(&WaFilter::NAME, #NAME, HAND, P2, P3, P4, P5)  )

// standard macros for accessing
#define ACCESS_MICPAR_HCP            \
   auto seat = par[0];               \
   const auto &hcp(lay[seat].hcp)

#define ACCESS_MICPAR_LEN            \
   auto seat = par[0];               \
   const auto &len(lay[seat].len)

#define ACCESS_MICPAR_CTRL           \
   auto seat = par[0];               \
   const auto &ctrl(lay[seat].ctrl)

#define ACCESS_MICPAR_JO             \
   auto seat = par[0];               \
   u64 jo(lay[seat].hand.card.jo)

#define ACCESS_MICPARS_HL            \
   auto seat = par[0];               \
   const auto &hcp(lay[seat].hcp);   \
   const auto &len(lay[seat].len)


// a class to rule task logic. fill them on init. 
// then values are constant through all solving
typedef void (Walrus::* SemFuncType)();
typedef void (Shuffler::* ShufflerFunc)();
typedef void (Walrus::* SemScoring)(DdsTricks &tr);
typedef void (Walrus::* SemPostMortem)(DdsTricks& tr, deal& cards);
typedef void (Walrus::* SemOnBoardAdded)(twContext* lay);
typedef uint (WaFilter::* DepFilterOut)(SplitBits& part, uint& camp, SplitBits& lho, SplitBits& rho);// deprecated since 3.0
struct Semantics {
   SemFuncType              onInit;
   SemFuncType              onShareStart;
   SemFuncType              onScanCenter;
   ShufflerFunc             fillFlipover;
   std::vector<MicroFilter> vecFilters;
   SemOnBoardAdded          onBoardAdded;
   SemScoring               onScoring;
   SemPostMortem            onPostmortem;
   SemScoring               onSolvedTwice;
   SemFuncType              onAfterMath;
   DepFilterOut             onDepFilter;
   uint scanCover; // how much iterations covers one scan
   Semantics();
   void MiniLink();
};

extern Semantics semShared;

