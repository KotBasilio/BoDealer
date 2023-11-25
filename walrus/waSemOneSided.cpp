/************************************************************
* Walrus project                                        2019
* Semantic parts of the tasks. They all differ by two small parts:
* -- how we init the deck;
* -- how we do the iteration.
*
************************************************************/
#include "walrus.h"

#ifdef SEMANTIC_JULY_MOROZOV_HAND_EVAL
void Walrus::FillSemantic(void)
{
   OrbNorthFillSem();
   //sem.onBoardAdded = &Walrus::DisplayBoard;
   //sem.onScoring = &Walrus::Score_3NT;
   sem.onScoring = &Walrus::Score_6NT;
   sem.vecFilters.clear();
   //ADD_2PAR_FILTER( SOUTH, PointsRange, 15, 17);
   ADD_2PAR_FILTER( SOUTH, PointsRange, 16, 17);
   ADD_2PAR_FILTER( SOUTH, SpadesLen,   2, 3);
   ADD_2PAR_FILTER( SOUTH, HeartsLen,   2, 3);
   ADD_2PAR_FILTER( SOUTH, DiamondsLen, 2, 5);
   ADD_2PAR_FILTER( SOUTH, ClubsLen,    2, 5);
}
#endif // SEMANTIC_JULY_MOROZOV_HAND_EVAL

#ifdef SEMANTIC_OCT_SEEK_6D
void Walrus::FillSemantic(void)
{
   OrbNorthFillSem();
   //sem.onBoardAdded = &Walrus::DisplayBoard;
   sem.onScoring = &Walrus::Score_NV6Minor;
   sem.onPostmortem = &Walrus::PostmortemHCP;
   sem.vecFilters.clear();
   ADD_4PAR_FILTER(SOUTH, ExactShape, 3, 2, 5, 3);
   ADD_2PAR_FILTER(SOUTH, PointsRange, 10, 10);
   ADD_0PAR_FILTER(EAST, NoOvercall);
   ADD_0PAR_FILTER(WEST, NoOvercall);
   ADD_3PAR_FILTER(SOUTH, KeyCardsRange, DMD, 1, 2);

   //ADD_3PAR_FILTER(SOUTH, KeyCardsRange, DMD, 2, 2);
}
#endif // SEMANTIC_OCT_SEEK_6D

#ifdef SEMANTIC_BASHA_NOV_NT
void Walrus::FillSemantic(void)
{
   OrbNorthFillSem();
   //sem.onBoardAdded = &Walrus::DisplayBoard;
   sem.onScoring = &Walrus::Score_3NT;
   sem.onPostmortem = &Walrus::PostmortemHCP;
   sem.vecFilters.clear();
   ADD_2PAR_FILTER(SOUTH, PointsRange, 10, 11);
   ADD_0PAR_FILTER(EAST, TakeoutOfClubs);
   ADD_2PAR_FILTER(SOUTH, SpadesLen, 2, 3);
   ADD_2PAR_FILTER(SOUTH, HeartsLen, 2, 3);
   ADD_2PAR_FILTER(SOUTH, DiamondsLen, 2, 5);
   ADD_2PAR_FILTER(SOUTH, ClubsLen, 3, 4);
   ADD_0PAR_FILTER(WEST, NoOvercall);

   //ADD_3PAR_FILTER(SOUTH, KeyCardsRange, DMD, 2, 2);
}
#endif // SEMANTIC_BASHA_NOV_NT
