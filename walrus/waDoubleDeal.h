/************************************************************
* Walrus project                                        2023
* Integration with Bo Haglund's double-dummy solver
*
************************************************************/
#include "../dds-develop/include/dll.h"
#include "walrus.h"

// data conversion class
struct DdsDeal
{
   // to pass to DDS
   deal dl;

   // result tricks
   DdsTricks tr;

   // viewing
   static bool needInspect;

   DdsDeal(const deal& dlBase, DdsTask2& task);
   DdsDeal(const deal& dlBase, DdsTask3& task);
   DdsDeal(twContext* lay);
   void Solve(uint handno);
private:
   void ReconstructNorth(int s);
   void ReconstructWest(int s);

   uint DecryptSpades(const SplitBits &bits) { return bits.card.w.s.Decrypt(); }
   uint DecryptHearts(const SplitBits &bits) { return bits.card.w.h.Decrypt(); }
   uint DecryptDiamnd(const SplitBits &bits) { return bits.card.w.d.Decrypt(); }
   uint DecryptClubs (const SplitBits &bits) { return bits.card.w.c.Decrypt(); }

   void FillByFixedNorth(DdsTask2& task);
   void FillByFixedWest (DdsTask2& task);
};

// main entry point to the double-dummy solver
extern int SolveAllBoardsN(boards& bds, solvedBoards& solved); 

