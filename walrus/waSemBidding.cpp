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

#ifdef SEMANTIC_OCT_INDIAN_5C
void Walrus::FillSemantic(void)
{
   OrbNorthFillSem();
   //sem.onBoardAdded = &Walrus::DisplayBoard;
   sem.onScoring = &Walrus::Score_5Major;
   sem.onSolvedTwice = &Walrus::Score_Opp5Minor;
   sem.vecFilters.clear();
   ADD_2PAR_FILTER(SOUTH, PointsRange, 17, 19);
   ADD_2PAR_FILTER(SOUTH, SpadesLen, 3, 4);

   ADD_2PAR_FILTER(WEST, PointsRange, 9, 16);
   ADD_2PAR_FILTER(WEST, ClubsLen, 5, 6);

   ADD_2PAR_FILTER(EAST, PointsRange, 0, 11);
   ADD_2PAR_FILTER(EAST, ClubsLen, 4, 5);

   ADD_0PAR_FILTER(EAST,  AnyInListBelow );
      ADD_2PAR_FILTER(EAST, ClubsLen, 5, 5);
      ADD_2PAR_FILTER(EAST,  SpadesLen,  0, 1);
      ADD_2PAR_FILTER(EAST, HeartsLen, 0, 1);
      ADD_2PAR_FILTER(EAST, DiamondsLen, 0, 1);
   ADD_0PAR_FILTER(EAST, EndList);

//    ADD_0PAR_FILTER(SOUTH,  AnyInListBelow );
//       ADD_1PAR_FILTER( SOUTH,  PointsAtLeast, 15);
//       ADD_2PAR_FILTER( SOUTH,  SpadesLen,   6, 6);
//    ADD_0PAR_FILTER(SOUTH,  EndList );
}
#endif // SEMANTIC_OCT_INDIAN_5C

#ifdef SEMANTIC_OVERCALL_4C
void Walrus::FillSemantic(void)
{
   OrbNorthFillSem();
   //sem.onBoardAdded = &Walrus::DisplayBoard;
   sem.onScoring = &Walrus::Score_4Minor;
   sem.onSolvedTwice = &Walrus::Score_Opp4Major;
   sem.vecFilters.clear();
   ADD_2PAR_FILTER(EAST,  PointsRange, 12, 13);
   ADD_2PAR_FILTER(WEST,  PointsRange, 9, 11);
   ADD_2PAR_FILTER(WEST,  SpadesLen, 3, 4);
   ADD_2PAR_FILTER(EAST,  SpadesLen, 6, 6);
   ADD_0PAR_FILTER(EAST,  SpadesNatural);
   ADD_2PAR_FILTER(WEST,  HeartsLen, 1, 4);
   ADD_0PAR_FILTER(SOUTH, NoOvercall);
   ADD_2PAR_FILTER(WEST,  DiamondsLen, 0, 5);
   ADD_2PAR_FILTER(WEST,  ClubsLen, 0, 5);
}
#endif // SEMANTIC_OVERCALL_4C


