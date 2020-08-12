/************************************************************
* Walrus project                                        2019
* Semantic parts of the tasks. They all differ by two small parts:
* -- how we init the deck;
* -- how we do the iteration.
*
************************************************************/
#include "walrus.h"

#ifdef SEMANTIC_JULY_AUTO_FITO_PLANKTON

void Walrus::FillSemantic(void)
{
  Orb_FillSem();
  sem.onFilter = &Walrus::FitoJuly_FilterOut;
  sem.onScoring = &Walrus::Score_4Major;
  sem.onOppContract = &Walrus::Score_3MajorDoubled;
}

// OUT: camp
uint Walrus::FitoJuly_FilterOut(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
  const uint ORDER_BASE = 3;
  const uint SKIP_BY_PART = 1;
  const uint SKIP_BY_OPP = 2;
  const uint SKIP_BY_RESP = 3;

  // LHO: fr-nat 1s, pass
  // partner: 2h overcall, pass
  // RHO: fit 2s, balance 3s
  twLengths lenPart(partner);
  if (lenPart.h < 5 || 6 < lenPart.h) {
    camp = SKIP_BY_PART;
    return ORDER_BASE; // bid hearts, pass 3h
  }
  twlHCP hcpPart(partner);
  if (hcpPart.total < 11) {
    bool tenAndSix = (hcpPart.total == 10 && lenPart.h == 6);
    if (!tenAndSix) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // wrong points count
    }
  }

  // opener opp
  twlHCP hcpOpp(rho);
  if (hcpOpp.total < 11) {
    camp = SKIP_BY_OPP;
    return ORDER_BASE + 2; // opener points count
  }
  twLengths lenOpp(rho);
  if (lenOpp.s != 5) {
    camp = SKIP_BY_OPP;
    return ORDER_BASE + 3; // more spades => bid over 3h
  }
  if (hcpOpp.total > 13) {
    if (hcpOpp.total > 15) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 4; // opener points count
    }
    if (lenOpp.c > 1 && lenOpp.d > 1 && lenOpp.h > 1) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 5; // no shortage, i.e.5332, 5422
    }
  }

  // responder opp
  twlHCP hcpResp(lho);
  if (hcpResp.total < 6 || 9 < hcpResp.total) {
    camp = SKIP_BY_RESP;
    return ORDER_BASE + 2; // resp points count
  }
  twLengths lenResp(lho);
  if (lenResp.s < 3) {
    camp = SKIP_BY_RESP;
    return ORDER_BASE + 3; // fit
  }
  if (lenResp.s == 3) {
    bool shortH = lenResp.h < 2;
    bool goodS = hcpResp.s > 5;
    if (!goodS && !shortH) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 4; // balance 3s over 3h
    }
  }

  // part could raise 3h to 4h
  bool hasSide5 = (lenPart.c > 4 || lenPart.d > 4);
  bool hasPoints = (hcpPart.total > 12);
  if (lenPart.h == 5) {
    if (hasSide5 && hasPoints) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 4; // 5-5 and some points
    }
  } else {
    if (hasPoints) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 5; // 6 card and some points
    }
    if (hasSide5) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 6; // enough distribution
    }
  }

  // seems it passes
  return 0;
}
#endif // SEMANTIC_JULY_AUTO_FITO_PLANKTON

#ifdef SEMANTIC_AUG_MULTI_VUL
void Walrus::FillSemantic(void)
{
	Orb_FillSem();
	sem.onFilter = &Walrus::AugMultiVul_FilterOut;
	sem.onScoring = &Walrus::Score_4Major;
}

uint Walrus::AugMultiVul_FilterOut(SplitBits &partner, uint &camp, SplitBits &rho, SplitBits &lho)
{
	const uint ORDER_BASE = 3;
	const uint SKIP_BY_PART = 1;
	const uint SKIP_BY_RESP = 2;
	const uint SKIP_BY_OPP = 3;

	twlHCP hcpPart(partner);
	//if (hcpPart.total < 7 || 8 < hcpPart.total) {// multi min
	if (hcpPart.total != 7) {// multi full min
		camp = SKIP_BY_PART;
		return ORDER_BASE; // wrong points count
	}
	twLengths lenPart(partner);
	if (lenPart.s != 6) {// multi, decline invitation
		camp = SKIP_BY_PART;
		return ORDER_BASE + 1; // bid spades
	}
	if (hcpPart.s < 5) {
		camp = SKIP_BY_PART;
		return ORDER_BASE + 2; // good six
	}

	twlHCP hcpOpp(rho);
	if (hcpOpp.total < 11 || 15 < hcpOpp.total) {
		camp = SKIP_BY_OPP;
		return ORDER_BASE + 2; // wrong points count, bid h
	}

	twLengths lenOpp(rho);
	if (lenOpp.h < 5 || 6 < lenOpp.h) {
		camp = SKIP_BY_RESP;
		return ORDER_BASE + 3; // bid h
	}

	if (lenPart.c < 1 ||
		lenPart.d < 1 ||
		lenPart.h < 1) {
		camp = SKIP_BY_PART;
		return ORDER_BASE + 3; // no voids
	}

	if (lenPart.c > 4 ||
		lenPart.d > 4) {
		camp = SKIP_BY_PART;
		return ORDER_BASE + 3; // no long minors
	}

// 	if (lenPart.c > 4 ||
// 		lenPart.d > 4) {
// 		camp = SKIP_BY_PART;
// 		return ORDER_BASE + 5; // no 5-5
// 	}
// 
// 	twLengths lenResp(rho);
// 	if (lenResp.h < 3) {
// 		camp = SKIP_BY_RESP;
// 		return ORDER_BASE + 5; // dbl
// 	}

	// seems it passes
	return 0;
}
#endif // SEMANTIC_AUG_MULTI_VUL

#ifdef SEMANTIC_AUG_SPLIT_FIT
void Walrus::FillSemantic(void)
{
	Orb_FillSem();
	sem.onFilter = &Walrus::AugSplitFit_FilterOut;
	sem.onScoring = &Walrus::Score_NV_4Major;
}

uint Walrus::AugSplitFit_FilterOut(SplitBits &partner, uint &camp, SplitBits &rho, SplitBits &lho)
{
	const uint ORDER_BASE = 3;
	const uint SKIP_BY_PART = 1;
	const uint SKIP_BY_RESP = 2;
	const uint SKIP_BY_OPP  = 3;

	twlHCP hcpPart(partner);
	if (hcpPart.total < 11 || 13 < hcpPart.total) {// not 14-15
		camp = SKIP_BY_PART;
		return ORDER_BASE; // wrong points count
	}

	twlHCP hcpOpp(lho);
	if (hcpOpp.total < 6 || 11 < hcpOpp.total) {
		camp = SKIP_BY_OPP;
		return ORDER_BASE + 2; // wrong points count
	}

	twLengths lenOpp(lho);
	if (lenOpp.h != 6) {
		camp = SKIP_BY_RESP;
		return ORDER_BASE + 3; // multi
	}

	twLengths lenPart(partner);
	if (lenPart.s < 5 || 6 < lenPart.s) {// not 7-card
		camp = SKIP_BY_PART;
		return ORDER_BASE + 4; // bid spades
	}
	if (lenPart.c > 4 ||
		  lenPart.d > 4) {
		camp = SKIP_BY_PART;
		return ORDER_BASE + 5; // no 5-5
	}

	twLengths lenResp(rho);
	if (lenResp.h < 3) {
		camp = SKIP_BY_RESP;
		return ORDER_BASE + 5; // dbl
	}

	// seems it passes
	return 0;
}

#endif // SEMANTIC_AUG_SPLIT_FIT


#ifdef SEMANTIC_RED55_KINGS_PART_15_16

void Walrus::FillSemantic(void)
{
   Orb_FillSem();
   sem.onFilter = &Walrus::R55_FilterOut;
   sem.onScoring = &Walrus::Score_4Major;
}

// OUT: camp
uint Walrus::R55_FilterOut(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
   // checks are intermixed according to profiler results
   // partner: 15-16 balanced. exactly 4 hearts. exactly 2 diamonds
   const uint ORDER_BASE = 2;
   const uint SKIP_BY_PART = 1;

   twlHCP hcpPart(partner);
   if (hcpPart.total < 15 || 16 < hcpPart.total) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 3; // wrong points count
   }

   twLengths lenPart(partner);
   if (lenPart.h != 3) {//4
      camp = SKIP_BY_PART;
      return ORDER_BASE + 4; // no 4-cards fit
   }

   if (lenPart.d != 3) {//2
      camp = SKIP_BY_PART;
      return ORDER_BASE + 5; // no doubleton diamonds
   }

   if (lenPart.s < 2 ||
      lenPart.c < 2) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 6; // a shortage in NT
   }

   if (lenPart.s > 4 ||
       lenPart.c > 4) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 7; // long black suits
   }

   // seems it passes
   return 0;
}

#endif // SEMANTIC_RED55_KINGS_PART_15_16

#ifdef SEMANTIC_TRICOLOR_STRONG

void Walrus::FillSemantic(void)
{
   Orb_FillSem();
   sem.onFilter = &Walrus::TriSunday_FilterOut; // Tricolor_FilterOut
   sem.onScoring = &Walrus::Score_4Major;
}

// OUT: camp
uint Walrus::Tricolor_FilterOut(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
   const uint ORDER_BASE = 5;
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP  = 2;

   twlHCP hcpPart(partner);
   if (hcpPart.total != 6) {
      camp = SKIP_BY_PART;
      return ORDER_BASE; // wrong points count
   }

   twlHCP hcpResp(rho);
   if (hcpResp.total > 6) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // resp points count
   }
   twlHCP hcpOpp(lho);
   if (hcpOpp.total < 12) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // wrong points count
   }

   twLengths lenOpp(lho);
   if (lenOpp.d < 5 || lenOpp.c < 4) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 3; // suit bid
   }
   if (lenOpp.c == 4) {
      if (hcpOpp.total < 14) {
         camp = SKIP_BY_OPP;
         return ORDER_BASE + 4; // points when 5-4 minors
      }
   }

   twLengths lenPart(partner);
   if (lenPart.h > lenPart.s) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 4; // bid spades
   }
   if (lenPart.h == lenPart.s) {
      if (hcpPart.h > hcpPart.s) {
         camp = SKIP_BY_PART;
         return ORDER_BASE + 5; // bid spades
      }
   }
   if (lenPart.s < 2) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 6; // bid spades
   }


   // seems it passes
   return 0;
}

uint Walrus::TriSunday_FilterOut(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
   const uint ORDER_BASE = 5;
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;

   twlHCP hcpPart(partner);
   if (hcpPart.total != 3) {
      camp = SKIP_BY_PART;
      return ORDER_BASE; // wrong points count
   }

   twLengths lenPart(partner);
   if (lenPart.h != 5) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // bid 2h 
   }

   twlHCP hcpOpp(rho);
   if (hcpOpp.total < 11 || 16 < hcpOpp.total) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // wrong points count
   }

   twLengths lenOpp(rho);
   if (lenOpp.d != 6) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 3; // bid d twice
   }

   twlHCP hcpResp(lho);
   if (hcpResp.total > 8) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 4; // resp points count
   }

   // seems it passes
   return 0;
}
#endif // SEMANTIC_TRICOLOR_STRONG


