/******************************************************************************
 * Micro-filters are a kind of byte-code programming   07.06.2023
 *
 ******************************************************************************/

#define MAX_MICRO_PARAMS 4

typedef uint (WaFilter::* MicroFunc)(twContext* lay, const uint *par);

struct MicroFilter {
   MicroFunc func;
   uint  params[MAX_MICRO_PARAMS];
   MicroFilter() : func(nullptr) {}
   MicroFilter(MicroFunc f, uint p0 = 0, uint p1 = 0, uint p2 = 0, uint p3 = 0);
};

// common reasons for a filter
const uint SKIP_BY_PART = 1;
const uint SKIP_BY_RESP = 2;
const uint SKIP_BY_OPP = 3;
const uint SKIP_BY_DIRECT = SKIP_BY_RESP;
const uint SKIP_BY_SANDWICH = SKIP_BY_OPP;

// macros for adding filters
#define ADD_VOID_FILTER(NAME)                   sem.vecFilters.push_back( MicroFilter(&WaFilter::NAME)                 )
#define ADD_1PAR_FILTER(NAME, P0)               sem.vecFilters.push_back( MicroFilter(&WaFilter::NAME, P0)             )
#define ADD_2PAR_FILTER(NAME, P0, P2)           sem.vecFilters.push_back( MicroFilter(&WaFilter::NAME, P0, P2)         )
#define ADD_3PAR_FILTER(NAME, P0, P2, P3)       sem.vecFilters.push_back( MicroFilter(&WaFilter::NAME, P0, P2, P3)     )
#define ADD_4PAR_FILTER(NAME, P0, P2, P3, P4)   sem.vecFilters.push_back( MicroFilter(&WaFilter::NAME, P0, P2, P3, P4) )

