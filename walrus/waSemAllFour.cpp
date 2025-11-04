/************************************************************
* Walrus project                                        2024
* Semantic parts for 4-hand tasks:
* -- tasks only 
*
************************************************************/

#include "walrus.h"
#include <assert.h>

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
   sem.onSinglePrimary = &Walrus::Score_NV6Major;
   sem.onMarkAfterSolve = &Walrus::AddMarksByHCP;
   sem.vecFilters.clear();
   ADD_4PAR_FILTER( NORTH, ExactShape, 4, 4, 4, 1);
   ADD_2PAR_FILTER( SOUTH, SpadesLen, 5, 6);
   ADD_2PAR_FILTER( SOUTH, PointsRange, 11, 16);
   ADD_3PAR_FILTER( NORTH, LinePointsRange, SOUTH, config.postm.minHCP, config.postm.maxHCP);
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
   // sem.onBoardAdded = &Walrus::GrabSplinterVariant;
}
#endif // SEMANTIC_SPLINTER_SHAPE

#ifdef SEMANTIC_STANDARD_3NT
void Walrus::FillSemantic(void)
{
   FourHandsFillSem();
   sem.onSinglePrimary = &Walrus::Score_3NT;
   sem.onMarkAfterSolve = &Walrus::AddMarksByHCP;
   sem.vecFilters.clear();
   ADD_4PAR_FILTER(NORTH, ModelShape, 3, 3, 3, 4);
   ADD_4PAR_FILTER(SOUTH, ModelShape, 3, 3, 4, 3);
   ADD_3PAR_FILTER(NORTH, LinePointsRange, SOUTH, 23, 32);
   ADD_1PAR_FILTER(NORTH, PointsAtLeast, 10);
   ADD_1PAR_FILTER(SOUTH, PointsAtLeast, 10);
   ADD_1PAR_FILTER(NORTH, NoMajorFit, SOUTH);

   //ADD_3PAR_FILTER(NORTH, LinePointsRange, SOUTH, config.postm.minHCP, config.postm.maxHCP);
}
#endif

#ifdef SEMANTIC_STANDARD_6NT
void Walrus::FillSemantic(void)
{
   FourHandsFillSem();
   sem.onSinglePrimary = &Walrus::Score_NV6NoTrump;
   sem.onMarkAfterSolve = &Walrus::AddMarksByHCP;
   sem.vecFilters.clear();
   ADD_4PAR_FILTER(NORTH, ModelShape, 3, 3, 3, 4);
   ADD_4PAR_FILTER(SOUTH, ModelShape, 3, 3, 4, 3);
   ADD_3PAR_FILTER(NORTH, LinePointsRange, SOUTH, config.postm.minHCP, config.postm.maxHCP);
   ADD_2PAR_FILTER(SOUTH, DiamondsLen, 3, 4);
   ADD_2PAR_FILTER(NORTH, ClubsLen, 3, 4);
   ADD_1PAR_FILTER(NORTH, NoMajorFit, SOUTH);
   ADD_1PAR_FILTER(NORTH, PointsAtLeast, 10);
   ADD_1PAR_FILTER(SOUTH, PointsAtLeast, 10);
   ADD_3PAR_FILTER(NORTH, LineAcesRange, SOUTH, 3, 4);

   //ADD_2PAR_FILTER(SOUTH, DiamondsLen, 5, 5);
}
#endif

#ifdef SEMANTIC_CONFIG_BASED
void Walrus::FillSemantic(void)
{
   FourHandsFillSem();
   sem.onSinglePrimary = &Walrus::Score_3NT;
   sem.onMarkAfterSolve = &Walrus::AddMarksByHCP;
   sem.vecFilters.clear();
   ADD_4PAR_FILTER(NORTH, ModelShape, 3, 3, 3, 4);
   ADD_4PAR_FILTER(SOUTH, ModelShape, 3, 3, 4, 3);
   ADD_3PAR_FILTER(NORTH, LinePointsRange, SOUTH, config.postm.minHCP, config.postm.maxHCP);
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
   //sem.onSinglePrimary = &Walrus::Score_1NT;
   sem.onSinglePrimary = &Walrus::Score_2M;
   sem.onSinglePrimary = &Walrus::Score_4Major;
   sem.onSinglePrimary = &Walrus::Score_3NT;
   sem.onMarkAfterSolve = &Walrus::AddMarksByHCP;
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

}
#endif

#ifdef SEMANTIC_4M_ON54_BIG
void Walrus::FillSemantic(void)
{
   FourHandsFillSem();
   sem.onSinglePrimary = &Walrus::Score_NV_4Major;
   //sem.onSinglePrimary = &Walrus::Score_4Major;
   sem.onMarkAfterSolve = &Walrus::AddMarksByHCP;
   //sem.onMarkAfterSolve = &Walrus::AddMarksBySuit;
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
   //config.postm.Type = WPM_SUIT;

}
#endif

#ifdef SEMANTIC_ALL4_1H_1S_2H_MAR
void Walrus::FillSemantic(void)
{
   FourHandsFillSem();

   sem.onSinglePrimary = &CumulativeScore::Our4M;
   sem.onMarkAfterSolve = &Walrus::AddMarksByHCP;
   sem.onMarkAfterSolve = &Walrus::AddMarksBySuit;
   sem.vecFilters.clear();

   ADD_4PAR_FILTER(NORTH, ExactShape, 4, 2, 4, 3);
   ADD_2PAR_FILTER(NORTH, PointsRange, 11, 11);
   //ADD_2PAR_FILTER(NORTH, PointsSuitLimit, DMD, 1);

   ADD_2PAR_FILTER(SOUTH, PointsRange, 11, 11);
   ADD_0PAR_FILTER(SOUTH, AnyInListBelow);
      ADD_4PAR_FILTER(SOUTH, ExactShape, 3, 6, 1, 3);
      ADD_4PAR_FILTER(SOUTH, ExactShape, 2, 6, 1, 4);
      ADD_4PAR_FILTER(SOUTH, ExactShape, 3, 6, 0, 4);
   ADD_0PAR_FILTER(SOUTH, EndList);
   //ADD_2PAR_FILTER(SOUTH, SpadesLen, 0, 4);
   //ADD_2PAR_FILTER(SOUTH, HeartsLen, 6, 6);
   //ADD_2PAR_FILTER(SOUTH, DiamondsLen, 0, 4);
   //ADD_2PAR_FILTER(SOUTH, ClubsLen, 0, 4);

   ADD_2PAR_FILTER(NORTH, ControlsRange, 3, 5);
   ADD_0PAR_FILTER(WEST, NoOvercall);
   ADD_0PAR_FILTER(EAST, NoOvercall);

   //ADD_0PAR_FILTER(SOUTH, ExcludeCombination);
   //   ADD_2PAR_FILTER(SOUTH, SpadesLen, 2, 3);
   //   ADD_2PAR_FILTER(SOUTH, DiamondsLen, 2, 3);
   //   ADD_2PAR_FILTER(SOUTH, ClubsLen, 2, 3);
   //ADD_0PAR_FILTER(SOUTH, EndList);

   config.postmSuit = SOL_DIAMONDS;
   config.postm.Type = WPM_SUIT;
}
#endif

#ifdef SEM_ALL4_1C_P_1H_2D_2H_MAR
void Walrus::FillSemantic(void)
{
    FourHandsFillSem();

    sem.onSinglePrimary = &CumulativeScore::Our4M;
    sem.onMarkAfterSolve = &Walrus::AddMarksByHCP;
    sem.vecFilters.clear();

    // prototype [N:83.QJ874.A5.A965]
    ADD_4PAR_FILTER(NORTH, ExactShape, 2, 5, 2, 4);
    ADD_2PAR_FILTER(NORTH, PointsRange, 11, 11);
    ADD_2PAR_FILTER(NORTH, ControlsRange, 3, 5);

    //ADD_2PAR_FILTER(SOUTH, PointsRange, 12, 13);
    //ADD_0PAR_FILTER(SOUTH, AnyInListBelow);
    //    ADD_2PAR_FILTER(SOUTH, PointsRange, 13, 13);
    //    ADD_2PAR_FILTER(SOUTH, ControlsRange, 4, 6);
    //ADD_0PAR_FILTER(SOUTH, EndList);

    ADD_2PAR_FILTER(SOUTH, PointsRange, 11, 12);
    ADD_0PAR_FILTER(SOUTH, AnyInListBelow);
        ADD_2PAR_FILTER(SOUTH, PointsRange, 11, 11);
        ADD_2PAR_FILTER(SOUTH, ControlsRange, 0, 3);
    ADD_0PAR_FILTER(SOUTH, EndList);

    //ADD_4PAR_FILTER(SOUTH, ExactShape, 3, 4, 3, 3);
     ADD_0PAR_FILTER(SOUTH, AnyInListBelow);
        ADD_4PAR_FILTER(SOUTH, ExactShape, 3, 4, 4, 2);
        ADD_4PAR_FILTER(SOUTH, ExactShape, 4, 4, 3, 2);
     ADD_0PAR_FILTER(SOUTH, EndList);
 //    ADD_0PAR_FILTER(SOUTH, AnyInListBelow);
 //       ADD_4PAR_FILTER(SOUTH, ExactShape, 2, 4, 4, 3);
 //       ADD_4PAR_FILTER(SOUTH, ExactShape, 2, 4, 3, 4);
 //    ADD_0PAR_FILTER(SOUTH, EndList);
    //ADD_0PAR_FILTER(SOUTH, AnyInListBelow);
    //    ADD_4PAR_FILTER(SOUTH, ExactShape, 4, 4, 2, 3);
    //    ADD_4PAR_FILTER(SOUTH, ExactShape, 3, 4, 2, 4);
    //ADD_0PAR_FILTER(SOUTH, EndList);

    //ADD_2PAR_FILTER(SOUTH, HeartsLen, 4, 4);
    //ADD_2PAR_FILTER(SOUTH, SpadesLen, 2, 4);
    //ADD_2PAR_FILTER(SOUTH, DiamondsLen, 2, 4);
    //ADD_2PAR_FILTER(SOUTH, ClubsLen, 2, 4);

    ADD_0PAR_FILTER(WEST, NoOvercall);

    ADD_2PAR_FILTER(EAST, PointsRange, 10, 16);
    ADD_2PAR_FILTER(EAST, DiamondsLen, 5, 7);
    ADD_0PAR_FILTER(EAST, DiamondsNatural);

    //config.postmSuit = SOL_DIAMONDS;
    //config.postm.Type = WPM_SUIT;
}
#endif 

#ifdef FOUR_HANDS_TASK
void Walrus::FillSemantic(void)
{
   FourHandsFillSem();
}
#else
void Walrus::FillSemantic(void)
{
   assert(0);
}
#endif // FOUR_HANDS_TASK

