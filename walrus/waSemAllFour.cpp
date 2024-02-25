/************************************************************
* Walrus project                                        2024
* Semantic parts for 4-hand tasks:
* -- tasks only 
*
************************************************************/

#include "walrus.h"

#ifdef SEMANTIC_SPLINTER_SHAPE
void Walrus::GrabSplinterVariant(twContext* lay)
{
   const auto &hcp_N(lay[NORTH].hcp);
   const auto &len_S(lay[SOUTH].len);
   const auto &ctr_S(lay[SOUTH].ctrl);

   // having the Ac
   progress.hitsCount[hcp_N.total][(ctr_S.c > 1) ? 1 : 3]++;

   // length of clubs
   progress.hitsCount[0][len_S.c]++;

   // balance
   progress.countExtraMarks += 2;
}

void Walrus::FillSemantic(void)
{
   FourHandsFillSem();
   sem.onScoring = &Walrus::Score_NV6Major;
   sem.onPostmortem = &Walrus::PostmortemHCP;
   sem.vecFilters.clear();
   ADD_4PAR_FILTER( NORTH, ExactShape, 4, 4, 4, 1);
   ADD_2PAR_FILTER( SOUTH, SpadesLen, 5, 6);
   ADD_2PAR_FILTER( SOUTH, PointsRange, 11, 16);
   ADD_3PAR_FILTER( NORTH, LinePointsRange, SOUTH, IO_HCP_MIN, IO_HCP_MAX);
   ADD_3PAR_FILTER( NORTH, LineControlsRange, SOUTH, 8, 8);
   ADD_3PAR_FILTER( NORTH, LineKeyCardsSpade, SOUTH, 4, 5);
   ADD_1PAR_FILTER( NORTH, ClubPointsLimit, 1);
   ADD_0PAR_FILTER( WEST,  NoOvercall );
   ADD_0PAR_FILTER( SOUTH, SpadesNatural );
   ADD_0PAR_FILTER( EAST,  No7Plus );
   ADD_0PAR_FILTER( WEST,  No2SuiterAntiSpade );

   // used previously
   // ADD_1PAR_FILTER( NORTH, PointsAtLeast, 10);
   // ADD_2PAR_FILTER( SOUTH, ControlsRange, 4, 10);
   // ADD_2PAR_FILTER( NORTH, ControlsRange, 4, 10);
   // sem.onBoardAdded = &Walrus::DisplayBoard;
   // sem.onBoardAdded = &Walrus::GrabSplinterVariant;
}
#endif // SEMANTIC_SPLINTER_SHAPE

#ifdef SEMANTIC_STANDARD_3NT
void Walrus::FillSemantic(void)
{
   FourHandsFillSem();
   sem.onScoring = &Walrus::Score_3NT;
   sem.onPostmortem = &Walrus::PostmortemHCP;
   sem.vecFilters.clear();
   ADD_4PAR_FILTER(NORTH, ModelShape, 3, 3, 3, 4);
   ADD_4PAR_FILTER(SOUTH, ModelShape, 3, 3, 4, 3);
   ADD_3PAR_FILTER(NORTH, LinePointsRange, SOUTH, 23, 32);
   ADD_1PAR_FILTER(NORTH, PointsAtLeast, 10);
   ADD_1PAR_FILTER(SOUTH, PointsAtLeast, 10);
   ADD_1PAR_FILTER(NORTH, NoMajorFit, SOUTH);

   //sem.onBoardAdded = &Walrus::DisplayBoard;
   //ADD_3PAR_FILTER(NORTH, LinePointsRange, SOUTH, IO_HCP_MIN, IO_HCP_MAX);
}
#endif

#ifdef SEMANTIC_STANDARD_6NT
void Walrus::FillSemantic(void)
{
   FourHandsFillSem();
   sem.onScoring = &Walrus::Score_NV6NoTrump;
   sem.onPostmortem = &Walrus::PostmortemHCP;
   sem.vecFilters.clear();
   ADD_4PAR_FILTER(NORTH, ModelShape, 3, 3, 3, 4);
   ADD_4PAR_FILTER(SOUTH, ModelShape, 3, 3, 4, 3);
   ADD_3PAR_FILTER(NORTH, LinePointsRange, SOUTH, IO_HCP_MIN, IO_HCP_MAX);
   ADD_2PAR_FILTER(SOUTH, DiamondsLen, 3, 4);
   ADD_2PAR_FILTER(NORTH, ClubsLen, 3, 4);
   ADD_1PAR_FILTER(NORTH, NoMajorFit, SOUTH);
   ADD_1PAR_FILTER(NORTH, PointsAtLeast, 10);
   ADD_1PAR_FILTER(SOUTH, PointsAtLeast, 10);
   ADD_3PAR_FILTER(NORTH, LineAcesRange, SOUTH, 3, 4);

   //sem.onBoardAdded = &Walrus::DisplayBoard;
   //ADD_2PAR_FILTER(SOUTH, DiamondsLen, 5, 5);
}
#endif

#ifdef SEMANTIC_CONFIG_BASED
void Walrus::FillSemantic(void)
{
   FourHandsFillSem();
   sem.onScoring = &Walrus::Score_3NT;
   sem.onPostmortem = &Walrus::PostmortemHCP;
   sem.vecFilters.clear();
   ADD_4PAR_FILTER(NORTH, ModelShape, 3, 3, 3, 4);
   ADD_4PAR_FILTER(SOUTH, ModelShape, 3, 3, 4, 3);
   ADD_3PAR_FILTER(NORTH, LinePointsRange, SOUTH, IO_HCP_MIN, IO_HCP_MAX);
   //ADD_2PAR_FILTER(SOUTH, DiamondsLen, 3, 4);
   //ADD_2PAR_FILTER(NORTH, ClubsLen, 3, 4);
   ADD_1PAR_FILTER(NORTH, PointsAtLeast, 10);
   ADD_1PAR_FILTER(SOUTH, PointsAtLeast, 10);
   ADD_1PAR_FILTER(NORTH, NoMajorFit, SOUTH);

   //sem.onBoardAdded = &Walrus::DisplayBoard;
   ADD_2PAR_FILTER(SOUTH, DiamondsLen, 5, 5);
}
#endif

#ifdef SEMANTIC_1NT_OR_SIGN_OFF
void Walrus::FillSemantic(void)
{
   FourHandsFillSem();
   //sem.onScoring = &Walrus::Score_1NT;
   sem.onScoring = &Walrus::Score_2M;
   sem.onScoring = &Walrus::Score_4Major;
   sem.onScoring = &Walrus::Score_3NT;
   sem.onPostmortem = &Walrus::PostmortemHCP;
   sem.vecFilters.clear();
   ADD_4PAR_FILTER(NORTH, ExactShape, 5, 3, 3, 2);
   //ADD_2PAR_FILTER(NORTH, PointsRange, 9, 9);
   ADD_2PAR_FILTER(NORTH, PointsRange, 10, 12);
   //ADD_2PAR_FILTER(NORTH, ControlsRange, 4, 6);
   //ADD_2PAR_FILTER(NORTH, ControlsRange, 3, 3);
   ADD_2PAR_FILTER(SOUTH, PointsRange, 11, 13);
   ADD_2PAR_FILTER(SOUTH, SpadesLen, 2, 3);
   ADD_0PAR_FILTER(WEST, NoOvcOn1LevOpen);
   ADD_0PAR_FILTER(EAST, NoOvercall);
   ADD_2PAR_FILTER(SOUTH, HeartsLen, 2, 4);
   ADD_2PAR_FILTER(SOUTH, DiamondsLen, 2, 5);
   ADD_2PAR_FILTER(SOUTH, ClubsLen, 2, 5);

   //sem.onBoardAdded = &Walrus::DisplayBoard;
}
#endif

#ifdef SEMANTIC_4M_ON54_BIG
void Walrus::FillSemantic(void)
{
   FourHandsFillSem();
   sem.onScoring = &Walrus::Score_NV_4Major;
   //sem.onScoring = &Walrus::Score_4Major;
   sem.onPostmortem = &Walrus::PostmortemHCP;
   //sem.onPostmortem = &Walrus::PostmortemSuit;
   sem.vecFilters.clear();
   ADD_4PAR_FILTER(SOUTH, ExactShape, 4, 4, 3, 2);

   //ADD_4PAR_FILTER(NORTH, ExactShape, 5, 3, 3, 2);
   ADD_0PAR_FILTER(NORTH, AnyInListBelow);
      ADD_4PAR_FILTER(NORTH, ExactShape, 5, 4, 2, 2);
      ADD_4PAR_FILTER(NORTH, ExactShape, 5, 2, 4, 2);
      //ADD_4PAR_FILTER(NORTH, ExactShape, 5, 2, 2, 4);
   ADD_0PAR_FILTER(NORTH, EndList);

   ADD_2PAR_FILTER(SOUTH, PointsRange, 12, 13);
   ADD_2PAR_FILTER(NORTH, PointsRange, 10, 12);
   ADD_3PAR_FILTER(NORTH, LinePointsRange, SOUTH, 23, 25);
//    ADD_0PAR_FILTER(SOUTH, AnyInListBelow);
//       ADD_4PAR_FILTER(SOUTH, ExactShape, 4, 2, 4, 3);
//       ADD_4PAR_FILTER(SOUTH, ExactShape, 4, 2, 3, 4);
//    ADD_0PAR_FILTER(SOUTH, EndList);
   ADD_0PAR_FILTER(EAST, NoOvercall);
   ADD_0PAR_FILTER(WEST, NoOvercall);

   //ADD_2PAR_FILTER(NORTH, PointsSuitLessSuit, HRT, DMD);
   //ADD_2PAR_FILTER(NORTH, PointsSuitLEqSuit, DMD, HRT);

   config.postmSuit = SOL_HEARTS;
   //config.detailedReportType = WREPORT_SUIT;

   //sem.onBoardAdded = &Walrus::DisplayBoard;
}
#endif

