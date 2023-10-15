/************************************************************
* Walrus project                                        2019
* Semantic parts of the tasks. They all differ by two small parts:
* -- how we init the deck;
* -- how we do the iteration.
*
************************************************************/
#include "walrus.h"

#ifdef SEMANTIC_JUN_MINORS_VS_MAJORS
void Walrus::FillSemantic(void)
{
   OrbNorthFillSem();
   //sem.onBoardAdded = &Walrus::DisplayBoard;
   sem.onScoring = &Walrus::Score_NV_Doubled5Minor;
   sem.onSolvedTwice = &Walrus::Score_Opp4Major;
   sem.vecFilters.clear();
   ADD_2PAR_FILTER( SOUTH, PointsRange,   11, 16);
   ADD_2PAR_FILTER( EAST,  PointsRange,   11, 16);
   ADD_1PAR_FILTER( WEST,  PointsAtLeast, 7);
   ADD_2PAR_FILTER( EAST,  SpadesLen,   5, 6);
   ADD_2PAR_FILTER( WEST,  HeartsLen,   5, 7);
   ADD_2PAR_FILTER( EAST,  HeartsLen,   3, 4);
   ADD_2PAR_FILTER( SOUTH, DiamondsLen, 5, 6);
   ADD_2PAR_FILTER( WEST,  SpadesLen,   0, 2);
   ADD_2PAR_FILTER( SOUTH, ClubsLen,    4, 5);
   ADD_0PAR_FILTER( SOUTH, AnyInListBelow );
      ADD_2PAR_FILTER( SOUTH, DiamondsLen, 6, 6);
      ADD_2PAR_FILTER( SOUTH, ClubsLen,    5, 5);
   ADD_0PAR_FILTER( SOUTH, EndList );
   ADD_0PAR_FILTER( EAST,  AnyInListBelow );
      ADD_1PAR_FILTER( EAST,  PointsAtLeast, 15);
      ADD_2PAR_FILTER( EAST,  SpadesLen,   6, 6);
      ADD_2PAR_FILTER( EAST,  HeartsLen,   4, 4);
   ADD_0PAR_FILTER( EAST,  EndList );
}
#endif // SEMANTIC_JUN_MINORS_VS_MAJORS


