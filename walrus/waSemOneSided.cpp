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
#endif

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
#endif

#ifdef SEMANTIC_TANJA_FEB_LONG_H
void Walrus::FillSemantic(void)
{
   OrbNorthFillSem();
   //sem.onBoardAdded = &Walrus::DisplayBoard;
   sem.onScoring = &Walrus::Score_4Major;
   sem.onPostmortem = &Walrus::PostmortemHCP;
   sem.vecFilters.clear();
   ADD_2PAR_FILTER(SOUTH, PointsRange, 11, 12);
   ADD_0PAR_FILTER(EAST, NoOvercall);
   ADD_0PAR_FILTER(WEST, NoOvercall);
   ADD_2PAR_FILTER(SOUTH, SpadesLen, 5, 5);
   ADD_2PAR_FILTER(SOUTH, DiamondsLen, 4, 5);
   ADD_2PAR_FILTER(SOUTH, ClubsLen, 0, 3);
}
#endif

#ifdef SEMANTIC_54M_FEB_PARTIZAN
void Walrus::FillSemantic(void)
{
   OrbNorthFillSem();
   //sem.onBoardAdded = &Walrus::DisplayBoard;
   //sem.onScoring = &Walrus::Score_4Major;
   sem.onScoring = &Walrus::Score_3NT;
   sem.onPostmortem = &Walrus::PostmortemHCP;
   sem.vecFilters.clear();
   ADD_2PAR_FILTER(SOUTH, PointsRange, 12, 13);
   ADD_0PAR_FILTER(EAST, NoOvercall);
   ADD_0PAR_FILTER(WEST, NoOvercall);
   ADD_2PAR_FILTER(SOUTH, SpadesLen, 2, 3);
   ADD_2PAR_FILTER(SOUTH, HeartsLen, 2, 2);
   ADD_2PAR_FILTER(SOUTH, DiamondsLen, 2, 5);
   ADD_2PAR_FILTER(SOUTH, ClubsLen, 2, 5);
}
#endif

#ifdef SEMANTIC_4M_ON54_FEB_PATTON
void Walrus::FillSemantic(void)
{
   OrbNorthFillSem();
   //sem.onBoardAdded = &Walrus::DisplayBoard;
   sem.onScoring = &CumulativeScore::Our4M;
   sem.onPostmortem = &Walrus::PostmortemHCP;
   sem.vecFilters.clear();
   ADD_2PAR_FILTER(SOUTH, PointsRange, 12, 13);
   ADD_0PAR_FILTER(EAST, NoOvercall);
   ADD_0PAR_FILTER(WEST, NoOvercall);
   ADD_2PAR_FILTER(SOUTH, ControlsRange, 3, 6);

   //ADD_0PAR_FILTER(SOUTH, AnyInListBelow);
   //   ADD_4PAR_FILTER(SOUTH, ExactShape, 4, 4, 2, 3);
   //   ADD_4PAR_FILTER(SOUTH, ExactShape, 4, 3, 2, 4);
   //ADD_0PAR_FILTER(SOUTH, EndList);

   ADD_2PAR_FILTER(SOUTH, SpadesLen,   4, 4);
   ADD_2PAR_FILTER(SOUTH, HeartsLen,   2, 4);
   ADD_2PAR_FILTER(SOUTH, DiamondsLen, 2, 4);
   ADD_2PAR_FILTER(SOUTH, ClubsLen,    2, 4);

}
#endif

#ifdef SEMANTIC_7D_OR_NT_FEB
void Walrus::FillSemantic(void)
{
   OrbNorthFillSem();
   //sem.onBoardAdded = &Walrus::DisplayBoard;
   sem.onScoring = &CumulativeScore::OurNV7m;
   sem.onSolvedTwice = &CumulativeScore::Score_NV7NT;
   sem.onCompareContracts = &Walrus::CompareSlams;
   sem.onPostmortem = &Walrus::PostmortemHCP;
   sem.vecFilters.clear();
   ADD_2PAR_FILTER(SOUTH, PointsRange, 11, 13);
   ADD_2PAR_FILTER(SOUTH, DiamondsLen, 6, 7);
   ADD_2PAR_FILTER(SOUTH, PointsSuitAtLeast, DMD, 9);
   ADD_2PAR_FILTER(SOUTH, SpadesLen, 4, 4);
   ADD_2PAR_FILTER(SOUTH, HeartsLen, 0, 2);
   ADD_0PAR_FILTER(EAST, NoOvercall);
   ADD_0PAR_FILTER(WEST, NoOvercall);
}
#endif

#ifdef SEMANTIC_INV_AFTER_2H
void Walrus::FillSemantic(void)
{
   OrbNorthFillSem();
   //sem.onBoardAdded = &Walrus::DisplayBoard;
   sem.onScoring = &CumulativeScore::Our4M;
   sem.onPostmortem = &Walrus::PostmortemHCP;
   sem.vecFilters.clear();
   ADD_2PAR_FILTER(SOUTH, PointsRange, 10, 13);
   ADD_2PAR_FILTER(SOUTH, SpadesLen, 5, 6);

   ADD_0PAR_FILTER(WEST, NoOvercall);
   ADD_2PAR_FILTER(WEST, HeartsLen, 0, 2);

   ADD_2PAR_FILTER(EAST, HeartsLen, 6, 6);
   ADD_2PAR_FILTER(EAST, PointsRange, 7, 10);
   ADD_2PAR_FILTER(EAST, PointsSuitAtLeast, HRT, 3);

   //ADD_0PAR_FILTER(SOUTH, AnyInListBelow);
   //   ADD_2PAR_FILTER(SOUTH, SpadesLen, 6, 6);
   //   ADD_2PAR_FILTER(SOUTH, PointsRange, 12, 13);
   //ADD_0PAR_FILTER(SOUTH, EndList);

   ADD_0PAR_FILTER(SOUTH, SpadesNatural);
   ADD_0PAR_FILTER(SOUTH, ExcludeCombination);
      ADD_2PAR_FILTER(SOUTH, SpadesLen, 5, 5);
      ADD_2PAR_FILTER(SOUTH, PointsRange, 10, 11);
   ADD_0PAR_FILTER(SOUTH, EndList);
   ADD_0PAR_FILTER(SOUTH, ExcludeCombination);
      ADD_2PAR_FILTER(SOUTH, SpadesLen, 6, 6);
      ADD_2PAR_FILTER(SOUTH, PointsRange, 13, 13);
   ADD_0PAR_FILTER(SOUTH, EndList);

}
#endif 


#ifdef SEMANTIC_1H_1S_2H_MAR
void Walrus::FillSemantic(void)
{
   OrbNorthFillSem();
   //sem.onBoardAdded = &Walrus::DisplayBoard;
   sem.onScoring = &CumulativeScore::Our4M;
   sem.onPostmortem = &Walrus::PostmortemHCP;
   sem.vecFilters.clear();
   ADD_2PAR_FILTER(SOUTH, PointsRange, 11, 12);
   ADD_2PAR_FILTER(SOUTH, SpadesLen, 0, 4);
   ADD_2PAR_FILTER(SOUTH, HeartsLen, 6, 6);
   ADD_2PAR_FILTER(SOUTH, DiamondsLen, 0, 4);
   ADD_2PAR_FILTER(SOUTH, ClubsLen, 0, 4);

   ADD_0PAR_FILTER(WEST, NoOvercall);
   ADD_0PAR_FILTER(EAST, NoOvercall);

   ADD_0PAR_FILTER(SOUTH, ExcludeCombination);
      ADD_2PAR_FILTER(SOUTH, SpadesLen, 2, 3);
      ADD_2PAR_FILTER(SOUTH, DiamondsLen, 2, 3);
      ADD_2PAR_FILTER(SOUTH, ClubsLen, 2, 3);
   ADD_0PAR_FILTER(SOUTH, EndList);

}
#endif 

#ifdef SEMANTIC_1C_P_1H_2D_2H_MAR
void Walrus::FillSemantic(void)
{
   OrbNorthFillSem();
   //sem.onBoardAdded = &Walrus::DisplayBoard;
   sem.onScoring = &CumulativeScore::Our4M;
   sem.onPostmortem = &Walrus::PostmortemHCP;
   sem.vecFilters.clear();
   ADD_2PAR_FILTER(SOUTH, PointsRange, 11, 12);
   ADD_0PAR_FILTER(SOUTH, AnyInListBelow);
      ADD_2PAR_FILTER(SOUTH, PointsRange, 11, 11);
      ADD_2PAR_FILTER(SOUTH, ControlsRange, 0, 3);
   ADD_0PAR_FILTER(SOUTH, EndList);

   //ADD_4PAR_FILTER(SOUTH, ExactShape, 3, 4, 3, 3);
//    ADD_0PAR_FILTER(SOUTH, AnyInListBelow);
//       ADD_4PAR_FILTER(SOUTH, ExactShape, 3, 4, 4, 2);
//       ADD_4PAR_FILTER(SOUTH, ExactShape, 4, 4, 3, 2);
//    ADD_0PAR_FILTER(SOUTH, EndList);
//    ADD_0PAR_FILTER(SOUTH, AnyInListBelow);
//       ADD_4PAR_FILTER(SOUTH, ExactShape, 2, 4, 4, 3);
//       ADD_4PAR_FILTER(SOUTH, ExactShape, 2, 4, 3, 4);
//    ADD_0PAR_FILTER(SOUTH, EndList);
   ADD_0PAR_FILTER(SOUTH, AnyInListBelow);
      ADD_4PAR_FILTER(SOUTH, ExactShape, 4, 4, 2, 3);
      ADD_4PAR_FILTER(SOUTH, ExactShape, 3, 4, 2, 4);
   ADD_0PAR_FILTER(SOUTH, EndList);

//   ADD_4PAR_FILTER(SOUTH, ExactShape, 3, 4, 3, 3);
//    ADD_2PAR_FILTER(SOUTH, HeartsLen, 4, 4);
//    ADD_2PAR_FILTER(SOUTH, SpadesLen, 2, 4);
//    ADD_2PAR_FILTER(SOUTH, DiamondsLen, 2, 4);
//    ADD_2PAR_FILTER(SOUTH, ClubsLen, 2, 4);

   ADD_0PAR_FILTER(WEST, NoOvercall);

   ADD_2PAR_FILTER(EAST, PointsRange, 10, 16);
   ADD_2PAR_FILTER(EAST, DiamondsLen, 5, 7);
   ADD_0PAR_FILTER(EAST, DiamondsNatural);
}
#endif 


#ifdef SEM_MIX_MAR24_INVITE_4S
void Walrus::FillSemantic(void)
{
   OrbNorthFillSem();
   //sem.onBoardAdded = &Walrus::DisplayBoard;

   sem.onScoring = &CumulativeScore::Our4M;
   sem.onPostmortem = &Walrus::PostmortemHCP;
   sem.vecFilters.clear();

   //"Fix a 14 hcp 4315;\np 1c p 1d\n1h 1s 2h 2s p ??\nQuestion: how often s game makes?"
   ADD_2PAR_FILTER(SOUTH, PointsRange, 7, 8);
   ADD_2PAR_FILTER(WEST,  PointsRange, 7, 11);

   ADD_2PAR_FILTER(WEST,  HeartsLen, 4, 5);
   ADD_2PAR_FILTER(SOUTH, HeartsLen, 2, 2);

   ADD_2PAR_FILTER(SOUTH, SpadesLen, 4, 4);
   ADD_2PAR_FILTER(SOUTH, DiamondsLen, 4, 5);

   ADD_0PAR_FILTER(WEST, HeartsNatural);
   ADD_0PAR_FILTER(EAST, NoOvercall);

   ADD_2PAR_FILTER(EAST, DiamondsLen, 0, 5);
   ADD_0PAR_FILTER(EAST, ExcludeCombination);
      ADD_2PAR_FILTER(EAST, DiamondsLen, 5, 5);
      ADD_2PAR_FILTER(EAST, PointsRange, 10, 12);
   ADD_0PAR_FILTER(EAST, EndList);

   //ADD_2PAR_FILTER(SOUTH, PointsSuitLimit, CLB, 0);

}
#endif 


