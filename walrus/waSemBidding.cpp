/************************************************************
* Walrus project                                        2019
* Semantic parts of the tasks. They all differ by two small parts:
* -- how we init the deck;
* -- how we do the iteration.
*
************************************************************/
#include "walrus.h"

#ifdef SEMANTIC_MIXED_2D_WITH_MAJOR
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::Mixed2DwM;
   sem.onScoring = &Walrus::Score_2m;
}

// OUT: camp
uint WaFilter::Mixed2DwM(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;
   const uint SKIP_BY_RESP = 3;

   // part : 15-17 
   twlHCP hcpPart(partner);
   if (hcpPart.total < 15 || 17 < hcpPart.total) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 3; // wrong points count
   }

   twLengths lenPart(partner);

   if (lenPart.d < 2) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 5; // a shortage in NT 
   }

   if (lenPart.s < 2 ||
      lenPart.h < 2 ||
      lenPart.c < 2) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 6; // a shortage in NT
   }

   if (lenPart.s > 4 ||
      lenPart.h > 4 ||
      lenPart.d > 4 ||
      lenPart.c > 4) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 7; // long black suits
   }

   // seems it passes
   // LHO: pass, 2d
   twlHCP hcpDecl(lho);
   if (hcpDecl.total > 11) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // wrong points count
   }
   twLengths lenDecl(lho);
   if (lenDecl.s != 4 &&
      lenDecl.h != 4) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; 
   }
   if (lenDecl.d < 5 ||
      lenDecl.d > 6) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 3;
   }
   if (lenDecl.s > 4 ||
      lenDecl.h > 4) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 4; 
   }

   // RHO: pass
   twlHCP hcpResp(rho);
   if (hcpResp.total > 10) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 1;
   }
   twLengths lenResp(rho);
   if (lenResp.s > 4 ||
      lenResp.h > 4) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 4;
   }
   if (lenResp.c > 5 ||
      lenResp.d > 5) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 5;
   }

   // seems it passes
   return 0;
}
#endif // SEMANTIC_MIXED_2D_WITH_MAJOR


#ifdef SEMANTIC_NOV_DBL_ON_3NT
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::NovDbl3NT;
   sem.onScoring = &Walrus::Score_Doubled3NT;
}

// OUT: camp
uint WaFilter::NovDbl3NT(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;
   const uint SKIP_BY_RESP = 3;

   // LHO: 1c, 17-21 nt, jump to 3NT
   twlHCP hcpDecl(lho);
   if (hcpDecl.total < 20 || 21 < hcpDecl.total) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // wrong points count
   }
   twLengths lenDecl(lho);
//    if (lenDecl.s > 3) {
//       camp = SKIP_BY_OPP;
//       return ORDER_BASE + 2; // spade fit
//    }
   if (lenDecl.s != 1) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // Basha exactly 1444
   }
   if (lenDecl.h != 4 ||
      lenDecl.d != 4 ||
      lenDecl.c != 4) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 3;// Basha exactly 1444
   }

//    if (lenDecl.h > 4 ||
//       lenDecl.d > 5 ||
//       lenDecl.c > 7) {
//       camp = SKIP_BY_OPP;
//       return ORDER_BASE + 3;// kind of NT
//    }
//    if (lenDecl.h > 4 ||
//       lenDecl.d > 5) {
//       camp = SKIP_BY_OPP;
//       return ORDER_BASE + 3;// kind of NT + Manticora Gambling
//    }
   if (lenDecl.h < 2 ||
      lenDecl.d < 2 ||
      lenDecl.c < 2) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 4;// kind of NT
   }

   // partner: passss
   twLengths lenPart(partner);
   if (lenPart.s > 6 ||
      lenPart.h > 6 ||
      lenPart.d > 6 ||
      lenPart.c > 6) {
      camp = SKIP_BY_PART;
      return ORDER_BASE; // no 7+ suits
   }
   twlHCP hcpPart(partner);
   if (hcpPart.total > 11) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // no overcall
   }
   if (hcpPart.total > 9) {
      if (lenPart.s > 4 || lenPart.h > 4) {
         camp = SKIP_BY_PART;
         return ORDER_BASE + 2; // no overcall
      }
   }
   if (hcpPart.total > 6) {
      if (lenPart.h > 4) {
         if (lenPart.c > 4 ||
            lenPart.d > 4) {
            camp = SKIP_BY_PART;
            return ORDER_BASE + 3; // Michaels
         }
      }
      if (lenPart.c > 4 && lenPart.d > 4) {
         camp = SKIP_BY_PART;
         return ORDER_BASE + 4; // Michaels
      }
      if (lenPart.h > 5 && hcpPart.h > 5 || 
          lenPart.d > 5 && hcpPart.d > 5 || 
          lenPart.c > 5 && hcpPart.c > 5 ) {
         camp = SKIP_BY_PART;
         return ORDER_BASE + 5; // no overcall
      }
   }

   // RHO: neg, trf to spades, pass on 3NT
   twlHCP hcpResp(rho);
   if (hcpResp.total > 6) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 1;
   }
   twLengths lenResp(rho);
   if (lenResp.s != 5) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 1; // fit to 3s
   }
   if (lenResp.h > 4 ||
      lenResp.d > 4 ||
      lenResp.c > 4) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 3;
   }
//    if (lenResp.h > 5 ||
//       lenResp.d > 5 ||
//       lenResp.c > 5) {
//       camp = SKIP_BY_RESP;
//       return ORDER_BASE + 3;// Basha allow 5-5 resp
//    }

   // no 5-5-3-0 resp
//    if (lenResp.h > 5 &&
//       lenResp.d < 1 ||
//       lenResp.c < 1) {
//       camp = SKIP_BY_RESP;
//       return ORDER_BASE + 4;
//    }
//    if (lenResp.d > 5 &&
//       lenResp.h < 1 ||
//       lenResp.c < 1) {
//       camp = SKIP_BY_RESP;
//       return ORDER_BASE + 4;
//    }
//    if (lenResp.c > 5 &&
//       lenResp.d < 1 ||
//       lenResp.h < 1) {
//       camp = SKIP_BY_RESP;
//       return ORDER_BASE + 4;
//    }

   // seems it passes
   return 0;
}
#endif // SEMANTIC_NOV_DBL_ON_3NT

#ifdef SEMANTIC_NOV_BID_6C_OR_DBL_4S
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::NovSlam;
   sem.onScoring = &Walrus::Score_NV6Minor;
   sem.onSolvedTwice = &Walrus::Score_Opp4MajorDoubled;
}

// OUT: camp
uint WaFilter::NovSlam(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;
   const uint SKIP_BY_RESP = 3;

   // LHO: 1s, raise to 3s
   // partner: X, X
   // RHO: jumps to 3s
   twLengths lenOpp(rho);
   if (lenOpp.s < 5 || 6 < lenOpp.s) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE; // 5-6 spades
   }
   twLengths lenResp(lho);
   if (lenResp.s != 4) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 1; // fit to 3s
   }

   twLengths lenPart(partner);
   if (lenPart.h > 5 || 
       lenPart.d > 5 || 
       lenPart.c > 5) {
      camp = SKIP_BY_PART;
      return ORDER_BASE; // no 6+ suits
   }
   twlHCP hcpPart(partner);
   //if (hcpPart.total < 13) {
   //if (hcpPart.total < 12) {
   if (hcpPart.total < 10 || 11 < hcpPart.total) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // wrong points count
   }

   // opener opp
   twlHCP hcpOpp(rho);
   if (hcpOpp.total < 7) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // opener points count
   }
   // responder opp
   twlHCP hcpResp(lho);
   if (hcpResp.total < 3 || 6 < hcpResp.total) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 2; // resp points count
   }

   // exclude Michaels
   if (lenOpp.h > 4 ||
       lenOpp.d > 4 ||
       lenOpp.c > 4) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2;
   }

   // request a shortage when 5 spades
   if (lenOpp.s == 5) {
      if (lenOpp.h > 1 &&
          lenOpp.d > 1 &&
          lenOpp.c > 1) {
         camp = SKIP_BY_OPP;
         return ORDER_BASE + 3; 
      }
   }

   // exclude side 6+ in responder
   if (lenResp.h > 5 ||
       lenResp.d > 5 ||
       lenResp.c > 5) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 3;
   }


   // seems it passes
   return 0;
}
#endif // SEMANTIC_NOV_BID_6C_OR_DBL_4S

#ifdef SEMANTIC_JULY_AUTO_FITO_PLANKTON

void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::FitoJuly;
   sem.onScoring = &Walrus::Score_4Major;
   sem.onSolvedTwice = &Walrus::Score_Opp3MajorDoubled;
}

// OUT: camp
uint WaFilter::FitoJuly(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
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
	Orb_DepFillSem();
	sem.onDepFilter = &WaFilter::AugMultiVul;
	sem.onScoring = &Walrus::Score_4Major;
}

uint WaFilter::AugMultiVul(SplitBits &partner, uint &camp, SplitBits &rho, SplitBits &lho)
{
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
		return ORDER_BASE + 4; // no long minors
	}

	// seems it passes
	return 0;
}
#endif // SEMANTIC_AUG_MULTI_VUL

#ifdef SEMANTIC_AUG_SPLIT_FIT
void Walrus::FillSemantic(void)
{
	Orb_DepFillSem();
	sem.onDepFilter = &WaFilter::AugSplitFit;
	sem.onScoring = &Walrus::Score_NV_4Major;
}

uint WaFilter::AugSplitFit(SplitBits &partner, uint &camp, SplitBits &rho, SplitBits &lho)
{
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
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::R55;
   sem.onScoring = &Walrus::Score_4Major;
}

// OUT: camp
uint WaFilter::R55(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
   // checks are intermixed according to profiler results
   // partner: 15-16 balanced. exactly 4 hearts. exactly 2 diamonds
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
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::TriSunday; // Tricolor
   sem.onScoring = &Walrus::Score_4Major;
}

// OUT: camp
uint WaFilter::Tricolor(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
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

uint WaFilter::TriSunday(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
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

#ifdef SEMANTIC_SEPT_MAJORS54_18HCP
void Walrus::FillSemantic(void)
{
	Orb_DepFillSem();
	sem.onDepFilter = &WaFilter::SeptMajors;
	sem.onScoring = &Walrus::Score_4Major;
}

uint WaFilter::SeptMajors(SplitBits &partner, uint &camp, SplitBits &rho, SplitBits &lho)
{
	const uint SKIP_BY_PART = 1;
	const uint SKIP_BY_RESP = 2;
	const uint SKIP_BY_OPP = 3;

	twlHCP hcpPart(partner);
	if (hcpPart.total < 4 || 6 < hcpPart.total) {// neg max
		camp = SKIP_BY_PART;
		return ORDER_BASE; // wrong points count
	}
	twLengths lenPart(partner);
	if (lenPart.s > 2) {
		camp = SKIP_BY_PART;
		return ORDER_BASE + 1; // spade fit
	}
	if (lenPart.h != 4) {
		camp = SKIP_BY_PART;
		return ORDER_BASE + 2; // five => bid to game
	}

	if (lenPart.c > 5 || lenPart.d > 5) {
		if (hcpPart.total > 4) {
			camp = SKIP_BY_PART;
			return ORDER_BASE + 3; // 5+, 6-4 distribution => bid to game
		}
	}

	if (hcpPart.total == 6) {
		if (lenPart.c < 2 || lenPart.d < 2 || lenPart.s < 2) {
			camp = SKIP_BY_PART;
			return ORDER_BASE + 4; // full max, singleton and a fit => bid to game
		}
		if (partner.card.jo && ANY_ACE) {
			camp = SKIP_BY_PART;
			return ORDER_BASE + 5; // full max, any ace
		}
	}


	// seems it passes
	return 0;
}
#endif // SEMANTIC_SEPT_MAJORS54_18HCP

#ifdef SEMANTIC_NOV_VOIDWOOD
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::SlamTry;
   sem.onScoring = &Walrus::Score_NV6Major;
}

uint WaFilter::SlamTry(SplitBits &partner, uint &camp, SplitBits &rho, SplitBits &lho)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_RHO = 2;
   const uint SKIP_BY_LHO = 3;
   const u64 kc_mask        = 0x8000C00080008000LL;
   const u64 ace_clubs_mask = 0x0000000000008000LL;

   twlHCP hcpPart(partner);
   if (hcpPart.total < 11 || 13 < hcpPart.total) {// opener
      camp = SKIP_BY_PART;
      return ORDER_BASE; // wrong points count
   }
   if (partner.card.jo & kc_mask) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // no keycards on part
   }
   twLengths lenPart(partner);
   if (lenPart.s > 4 ||
       lenPart.h > 4 ||
       lenPart.c > 5 ||
       lenPart.d > 5) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2; // bid notrump
   }
   if (lenPart.s < 2 ||
       lenPart.h < 2 ||
       lenPart.c < 2 ||
       lenPart.d < 2) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 3; // bid notrump
   }

   twLengths lenRHO(rho);
   if (lenRHO.s > 6 ||
       lenRHO.c > 6 ||
       lenRHO.d > 6) {
      camp = SKIP_BY_RHO;
      return ORDER_BASE + 2; // may preempt
   }

   twLengths lenLHO(lho);
   if (lenLHO.s > 6 ||
       lenLHO.c > 6 ||
       lenLHO.d > 6) {
      camp = SKIP_BY_LHO;
      return ORDER_BASE + 2; // may preempt
   }

   if (rho.card.jo & ace_clubs_mask) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // ace of clubs with another guy pls
   }

   // seems the board passes
   return 0;
}
#endif // SEMANTIC_NOV_VOIDWOOD

#ifdef SEMANTIC_NOV_64_AS_TWO_SUITER
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::SeptMajors;
   sem.onScoring = &Walrus::Score_4Major;
}

uint WaFilter::SeptMajors(SplitBits &partner, uint &camp, SplitBits &rho, SplitBits &lho)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_RESP = 2;
   const uint SKIP_BY_OPP = 3;

   twlHCP hcpPart(partner);
   if (hcpPart.total < 4 || 11 < hcpPart.total) {// a raise to 3s
      camp = SKIP_BY_PART;
      return ORDER_BASE; // wrong points count
   }
   twLengths lenPart(partner);
   if (lenPart.s != 4) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // spade fit
   }

   // direct opp
   twlHCP hcpDopp(lho);
   if (hcpDopp.total > 12) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE; // would overcall
   }
   if (hcpDopp.total > 9) {
      twLengths lenDopp(lho);
      if (lenDopp.h > 5 ||
          lenDopp.d > 5 ||
          lenDopp.c > 5) {
         camp = SKIP_BY_RESP;
         return ORDER_BASE + 1; // would overcall
      }
   }

   // sandwich opp
   twlHCP hcpSand(rho);
   if (hcpSand.total > 13) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE; // would overcall
   }
   if (hcpSand.total > 10) {
      twLengths lenSand(rho);
      if (lenSand.h > 5 ||
          lenSand.d > 5 ||
          lenSand.c > 5) {
         camp = SKIP_BY_OPP;
         return ORDER_BASE + 2; // would overcall
      }
   }

   // seems it passes
   return 0;
}
#endif // SEMANTIC_NOV_64_AS_TWO_SUITER

#ifdef SEMANTIC_DEC_12_2425
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::Dec12_2425;
   sem.onScoring = &Walrus::Score_3NT;
}

uint WaFilter::Dec12_2425(SplitBits &partner, uint &camp, SplitBits &rho, SplitBits &lho)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_RESP = 2;
   const uint SKIP_BY_OPP = 3;

   twlHCP hcpPart(partner);
   //if (hcpPart.total < 11 || 12 < hcpPart.total) {// normal opener min
   if (hcpPart.total != 13) {// normal opener min
      camp = SKIP_BY_PART;
      return ORDER_BASE; // wrong points count
   }
   twLengths lenPart(partner);
   if (lenPart.s != 5) {// opened 1s
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // bid spades
   }
   if (lenPart.h > 4 ||
       lenPart.d > 4 ||
       lenPart.c > 4) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2;// any 5-5
   }

   twlHCP hcpOpp(rho);
   if (hcpOpp.total > 12) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // wrong points count, would overcall
   }

   twLengths lenR(rho);
   if (lenR.h > 6 ||
       lenR.d > 6 ||
       lenR.c > 6) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 3;// no 7+ suits
   }

   twLengths lenL(rho);
   if (lenL.h > 6 ||
       lenL.d > 6 ||
       lenL.c > 6) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE;// no 7+ suits
   }

   // seems it passes
   return 0;
}
#endif // SEMANTIC_DEC_12_2425

#ifdef SEMANTIC_DEC_BID_5H_OR_DBL_4S
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::DecTopHearts;
   sem.onScoring = &Walrus::Score_NV_5Major;
   sem.onSolvedTwice = &Walrus::Score_Opp4MajorDoubled;
}

// OUT: camp
uint WaFilter::DecTopHearts(SplitBits &partner, uint &camp, SplitBits &advancer, SplitBits &firstOpp)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;
   const uint SKIP_BY_RESP = 3;

   // LHO: 1s, raise to 4s
   twLengths lenOpp(firstOpp);
   if (lenOpp.s < 5 || 6 < lenOpp.s) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE; // 5-6 spades
   }
   twlHCP hcpOpp(firstOpp);
   if (hcpOpp.total < 11 || 16 < hcpOpp.total) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // opener points count
   }
   if (lenOpp.s == 5) {
      if (lenOpp.h > 1) {
         camp = SKIP_BY_OPP;
         return ORDER_BASE + 3;// request a heart shortage when =5 spades
      }
   }

   // partner: pass, X
   twlHCP hcpPart(partner);
   if (hcpPart.total < 2 || 7 < hcpPart.total) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // too few points count
   }
   twLengths lenPart(partner);
   if (lenPart.h > 2) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2; // would bid 5h
   }

   // RHO: fits with 2s
   twLengths lenAdv(advancer);
   if (lenAdv.s < 3 || 4 < lenAdv.s) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 1; // fit to 2s
   }
   twlHCP hcpAdv(advancer);
   if (hcpAdv.total < 3 || 9 < hcpAdv.total) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 2; // resp points count
   }
   if (lenAdv.h > 5 ||
      lenAdv.d > 5 ||
      lenAdv.c > 5) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 3;// adv would jump
   }

   // seems it passes
   return 0;
}
#endif // SEMANTIC_DEC_BID_5H_OR_DBL_4S

#ifdef SEMANTIC_DEC_ACCEPT_TO_4S
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::DecAcceptTo4S;
   sem.onScoring = &Walrus::Score_NV_4Major;
}

uint WaFilter::DecAcceptTo4S(SplitBits &partner, uint &camp, SplitBits &advancer, SplitBits &firstOpp)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;
   const uint SKIP_BY_RESP = 3;

   // LHO: pass, raise to 3d
   twlHCP hcpOpp(firstOpp);
   if (hcpOpp.total < 3 || 9 < hcpOpp.total) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // opener points count
   }
   twLengths lenOpp(firstOpp);
   if (lenOpp.d < 4 || 5 < lenOpp.d) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE; // 4-5 diam
   }

   // partner: pass, 3s
   twlHCP hcpPart(partner);
   if (hcpPart.total < 7 || 10 < hcpPart.total) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // too few points count
   }
   twLengths lenPart(partner);
   if (lenPart.s < 4 || 5 < lenPart.s) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2; // bid 3s
   }
   if (5 == lenPart.s) {
      if (8 < hcpPart.total) {
         camp = SKIP_BY_PART;
         return ORDER_BASE + 3; // would open 8-17
      }
   }
   if (lenPart.h > lenPart.s) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 4; // would bid 3h
   }

   // RHO: 1d, pass
   twLengths lenAdv(advancer);
   if (lenAdv.d < 4 || 5 < lenAdv.d) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 1; // 1d
   }
   twlHCP hcpAdv(advancer);
   if (hcpAdv.total < 10 || 15 < hcpAdv.total) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 2; // points count
   }
   if (lenAdv.h > 4 || lenAdv.s > 4 || lenAdv.c > 5) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 3; // no 5M, no prec
   }
   if (lenAdv.d == 4) {
      if (lenAdv.h > 1 && lenAdv.s > 1 && lenAdv.c > 1) {
         camp = SKIP_BY_RESP;
         return ORDER_BASE + 4;// unbalanced
      }
   }

   // seems it passes
   return 0;
}
#endif // SEMANTIC_DEC_ACCEPT_TO_4S

#ifdef SEMANTIC_DEC_JUMP_TO_4S
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::DecAcceptTo4S;
   sem.onScoring = &Walrus::Score_NV_4Major;
}

uint WaFilter::DecAcceptTo4S(SplitBits &partner, uint &camp, SplitBits &firstOpp, SplitBits &advancer)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;
   const uint SKIP_BY_RESP = 3;

   // LHO: pass, raise to 3d
   twlHCP hcpOpp(firstOpp);
   if (hcpOpp.total < 3 || 9 < hcpOpp.total) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // opener points count
   }
   twLengths lenOpp(firstOpp);
   if (lenOpp.d < 4 || 5 < lenOpp.d) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE; // 4-5 diam
   }

   // partner: X
   twlHCP hcpPart(partner);
   auto workingHCP = hcpPart.total - hcpPart.d;
   if (hcpPart.total < 12 || 13 < hcpPart.total) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // too few points count
   }
   twLengths lenPart(partner);
   if (lenPart.d > 2) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2; // opps raise d
   }
   if (lenPart.h > 4 || lenPart.s > 4) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 3; // would bid 1M
   }
   if (lenPart.h + lenPart.s < 7) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 4; // at least 4-3 M
   }
   if (lenPart.c > 5) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 5; // would bid 1M
   }

   // RHO: 1d, pass
   twLengths lenAdv(advancer);
   if (lenAdv.d < 4 || 5 < lenAdv.d) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 1; // 1d
   }
   twlHCP hcpAdv(advancer);
   if (hcpAdv.total < 10 || 15 < hcpAdv.total) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 2; // points count
   }
   if (lenAdv.h > 4 || lenAdv.s > 4 || lenAdv.c > 5) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 3; // no 5M, no prec
   }
   if (lenAdv.d == 4) {
      if (lenAdv.h > 1 && lenAdv.s > 1 && lenAdv.c > 1) {
         camp = SKIP_BY_RESP;
         return ORDER_BASE + 4;// unbalanced
      }
   }

   // seems it passes
   return 0;
}
#endif // SEMANTIC_DEC_JUMP_TO_4S

#ifdef SEMANTIC_JAN_NT_SLAM_ON_DIAMONDS
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::SlamTry;
   sem.onScoring = &Walrus::Score_NV6NoTrump;
   //sem.onScoring = &Walrus::Score_3NT;
}

uint WaFilter::SlamTry(SplitBits &partner, uint &camp, SplitBits &rho, SplitBits &lho)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;
   const uint SKIP_BY_RESP = 3;

   // partner: 1NT
   twlHCP hcpPart(partner);
   //if (hcpPart.total < 15 || 17 < hcpPart.total) {
   //if (hcpPart.total < 14 || 16 < hcpPart.total) {
   if (hcpPart.total < 14 || 14 < hcpPart.total) {
      camp = SKIP_BY_PART;
      return ORDER_BASE; // NT
   }
   twLengths lenPart(partner);
   if (lenPart.s > 4 ||
      lenPart.h > 4 ||
      lenPart.d > 4 ||
      lenPart.c > 5) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // kind of NT
   }
   if (lenPart.s < 2 ||
      lenPart.h < 2 ||
      lenPart.d < 2 ||
      lenPart.c < 2) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2;// kind of NT
   }
   if (lenPart.s == 2) {
      if (hcpPart.s < 2) {
         camp = SKIP_BY_PART;
         return ORDER_BASE + 3;// stop spades
      }
   }

   // RHO, LHO: passs
   twLengths lenOpp(rho);
   if (lenOpp.s > 6 ||
      lenOpp.h > 6 ||
      lenOpp.d > 6 ||
      lenOpp.c > 6) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE; // no 7+
   }
   twLengths lenResp(lho);
   if (lenResp.s > 6 ||
      lenResp.h > 6 ||
      lenResp.d > 6 ||
      lenResp.c > 6) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // no 7+
   }

   // two suiters
   if (lenResp.s >= 5) {
      if (lenResp.h > 5 ||
         lenResp.c > 5) {
         camp = SKIP_BY_OPP;
         return ORDER_BASE + 2; // no two-suiters
      }
   } else if (lenResp.h + lenResp.c > 9) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // no two-suiters
   }
   if (lenOpp.s >= 5) {
      if (lenOpp.h > 5 ||
         lenOpp.c > 5) {
         camp = SKIP_BY_RESP;
         return ORDER_BASE + 2; // no two-suiters
      }
   } else if (lenOpp.h + lenOpp.c > 9) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 2; // no two-suiters
   }

   // seems it passes
   return 0;
}

#endif // SEMANTIC_JAN_NT_SLAM_ON_DIAMONDS

#ifdef SEMANTIC_DEC_JAN_DBL_THEN_HEARTS
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::JanDblThenH;
   sem.onScoring = &Walrus::Score_4Major;
}

uint WaFilter::JanDblThenH(SplitBits &partner, uint &camp, SplitBits &firstOpp, SplitBits &resp)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_RESP = 2;
   const uint SKIP_BY_OPP = 3;

   // first opp: precision
   twlHCP hcpOpp(firstOpp);
   if (hcpOpp.total < 11 || 16 < hcpOpp.total) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE; // opener points count
   }
   twLengths lenOpp(firstOpp);
   if (lenOpp.c < 5) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // too few clubs
   }
   if (lenOpp.c < 6) {
      camp = SKIP_BY_OPP;
      if (lenOpp.h != 4 || lenOpp.s != 4) {
         return ORDER_BASE + 2; // have 4 in a major
      }
   }
   if (lenOpp.h > 4 || lenOpp.s > 4) {
      return ORDER_BASE + 3; // no 5 in a major
   }
   if (lenOpp.c > 7) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 4; // have 4 in a major
   }

   twlHCP hcpResp(resp);
   twLengths lenResp(resp);
   if (lenResp.c < 3) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 1; // fit to 3c
   }
   if (lenResp.c == 3 && hcpResp.total < 6) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 2; // fit-3 at least 6 hcp
   }
   if (lenResp.c == 4 && hcpResp.total < 4) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 3; // fit-4 at least 4 hcp
   }

   twlHCP hcpPart(partner);
   if (hcpPart.total > 6) {// part hcp range 0..
      camp = SKIP_BY_PART;
      return ORDER_BASE; // wrong points count
   }

   // seems it passes
   return 0;
}
#endif // SEMANTIC_DEC_JAN_DBL_THEN_HEARTS

#ifdef SEMANTIC_FEB_4711_DILEMMA_ON_4S
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::FebManyHearts;
   sem.onScoring = &Walrus::Score_5Major;
   sem.onSolvedTwice = &Walrus::Score_Opp4MajorDoubled;
}

// OUT: camp
uint WaFilter::FebManyHearts(SplitBits &partner, uint &camp, SplitBits &doubler, SplitBits &advancer)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_ADVANCER = 2;
   const uint SKIP_BY_DOUBLER = 3;

   // LHO: pass, 4s
   // partner: 2h, pass
   // RHO: X, pass
   twLengths lenOpp(advancer);
   if (lenOpp.s < 4) {
      camp = SKIP_BY_ADVANCER;
      return ORDER_BASE; // 4-5
   }
   twLengths lenResp(doubler);
   if (lenResp.s != 4) {
      camp = SKIP_BY_DOUBLER;
      return ORDER_BASE ; // X
   }

   twLengths lenPart(partner);
   if (lenPart.h < 3 || 4 < lenPart.h) {
      camp = SKIP_BY_PART;
      return ORDER_BASE; // fit
   }
   twlHCP hcpPart(partner);
   if (hcpPart.total < 3 || 9 < hcpPart.total) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // wrong points count
   }
   if (hcpPart.total < 6 && lenPart.h < 4) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2; // wrong points count
   }

   // hcp doubler
   twlHCP hcpResp(doubler);
   if (hcpResp.total < 11) {
      camp = SKIP_BY_DOUBLER;
      return ORDER_BASE + 1; // resp points count
   }

   // no suit better than spades in opp
   if (lenOpp.c > lenOpp.s ||
       lenOpp.d > lenOpp.s) {
      camp = SKIP_BY_ADVANCER;
      return ORDER_BASE + 1; // 
   }

   // doubler has no 6+ suits
   if (lenResp.d > 5 || lenResp.c > 5) {
      camp = SKIP_BY_DOUBLER;
      return ORDER_BASE + 2; // X
   }

   // 5+ spades => no 9+ pts
   if (lenOpp.s > 4) {
      twlHCP hcpOpp(advancer);
      if (hcpOpp.total > 8) {
         camp = SKIP_BY_ADVANCER;
         return ORDER_BASE + 2; // 
      }
   }

   // thin filter: we want part to have both void in spades and 4+ hearts
   // if (lenPart.s > 0 || lenPart.h < 4) {
   //    camp = SKIP_BY_PART;
   //    return ORDER_BASE + 4; // thin
   // }

   // clever part: when both void in spades and 4+ hearts => bid 5h
   if (lenPart.s == 0 || lenPart.h > 3) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 3; 
   }

   // seems it passes
   return 0;
}
#endif // SEMANTIC_FEB_4711_DILEMMA_ON_4S

#ifdef SEMANTIC_MIXED_PREVENTIVE_4S
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::MixedPreventive;
   sem.onScoring = &Walrus::Score_NV_Doubled4Major;
   sem.onSolvedTwice = &Walrus::Score_Opp4Major;
}

// OUT: camp
uint WaFilter::MixedPreventive(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;
   const uint SKIP_BY_RESP = 3;

   // LHO: double on 2h
   // partner: 1NT, 2s
   // RHO: double on 1NT, 3h
   twlHCP hcpPart(partner);
   if (hcpPart.total < 15 || 17 < hcpPart.total) {
      camp = SKIP_BY_PART;
      return ORDER_BASE; // wrong points count
   }
   twLengths lenPart(partner);
   if (lenPart.s > 4 ||
      lenPart.h > 4 ||
      lenPart.d > 5 ||
      lenPart.c > 5) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // kind of NT
   }
   if (lenPart.s < 2 ||
      lenPart.h < 2 ||
      lenPart.d < 2 ||
      lenPart.c < 2) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2;// kind of NT
   }

   // doubler opp
   twlHCP hcpDoubler(lho);
   if (hcpDoubler.total < 15) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // opener points count
   }
   twLengths lenDoubler(lho);
   if (lenDoubler.s > 5 ||
       lenDoubler.h > 5) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // would multi 2d
   }
   if (lenDoubler.h < 3) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 3; // did bid 3h
   }

   // advancer
   twLengths lenOpp(rho);
   if (lenOpp.h < 4) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE; // doubled 2h
   }
   twlHCP hcpAdv(rho);
   if (lenOpp.h < 5 && hcpAdv.h < 2) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 1; // doubled 2h
   }

   if (lenPart.s < 3) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 4; // support 3-4 sp
   }
//    if (lenPart.s < 4) {
//       camp = SKIP_BY_PART;
//       return ORDER_BASE + 5; // support 4 sp
//    }

   // seems it passes
   return 0;
}
#endif // SEMANTIC_MIXED_PREVENTIVE_4S

#ifdef SEMANTIC_APR_64_INVITE
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::SlamTry;
   sem.onScoring = &Walrus::Score_3NT;
}

uint WaFilter::SlamTry(SplitBits &partner, uint &camp, SplitBits &rho, SplitBits &lho)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;
   const uint SKIP_BY_RESP = 3;

   // partner: 1NT
   twlHCP hcpPart(partner);
   if (hcpPart.total < 14 || 16 < hcpPart.total) {
      camp = SKIP_BY_PART;
      return ORDER_BASE; // NT
   }
   twLengths lenPart(partner);
   if (lenPart.s > 3 ||
      lenPart.h > 4 ||
      lenPart.d > 5 ||
      lenPart.c > 5) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // wrong points count
   }
   if (lenPart.s < 2 ||
      lenPart.h < 2 ||
      lenPart.d < 2 || // 3
      lenPart.c < 2) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2;// kind of NT
   }
//    if (lenPart.d == 3 &&  hcpPart.d < 1) {
//       camp = SKIP_BY_PART;
//       return ORDER_BASE + 3;// want fit
//    }

   // RHO, LHO: passs
   twLengths lenOpp(rho);
   if (lenOpp.s > 6 ||
      lenOpp.h > 6 ||
      lenOpp.d > 6 ||
      lenOpp.c > 6) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE; // no 7+
   }
   twLengths lenResp(lho);
   if (lenResp.s > 6 ||
      lenResp.h > 6 ||
      lenResp.d > 6 ||
      lenResp.c > 6) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // no 7+
   }

   // two suiters
   if (lenResp.s >= 5) {
      if (lenResp.h > 5 ||
         lenResp.c > 5) {
         camp = SKIP_BY_OPP;
         return ORDER_BASE + 2; // no two-suiters
      }
   } else if (lenResp.h + lenResp.c > 9) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // no two-suiters
   }
   if (lenOpp.s >= 5) {
      if (lenOpp.h > 5 ||
         lenOpp.c > 5) {
         camp = SKIP_BY_RESP;
         return ORDER_BASE + 2; // no two-suiters
      }
   } else if (lenOpp.h + lenOpp.c > 9) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 2; // no two-suiters
   }

   // seems it passes
   return 0;
}

#endif // SEMANTIC_APR_64_INVITE

#ifdef SEMANTIC_SEPT_MANTICORA_14_16
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::SlamTry;
   sem.onScoring = &Walrus::Score_NV_4Major;
}

uint WaFilter::SlamTry(SplitBits &partner, uint &camp, SplitBits &rho, SplitBits &lho)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;
   const uint SKIP_BY_RESP = 3;

   // partner: 1NT (14-16)
   twlHCP hcpPart(partner);
   if (hcpPart.total < 14 || 16 < hcpPart.total) {
      camp = SKIP_BY_PART;
      return ORDER_BASE; // NT
   }
   twLengths lenPart(partner);
   if (lenPart.s != 2 ||
      lenPart.h > 4 ||
      lenPart.d > 4 ||
      lenPart.c > 4) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // wrong points count
   }
   if (lenPart.s < 2 ||
      lenPart.h < 2 ||
      lenPart.d < 2 || // 3
      lenPart.c < 2) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2;// kind of NT
   }
   if (lenPart.h == 3) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 4;// fit
      if (hcpPart.h < 5) {
         camp = SKIP_BY_PART;
         return ORDER_BASE + 3;// fit
      }
      if (hcpPart.total < 15) {
         camp = SKIP_BY_PART;
         return ORDER_BASE + 4;// fit
      }
   }
   //    if (lenPart.d == 3 &&  hcpPart.d < 1) {
   //       camp = SKIP_BY_PART;
   //       return ORDER_BASE + 3;// want fit
   //    }

   // RHO, LHO: pass
   twLengths lenOpp(rho);
   if (lenOpp.s !=5 ||
      lenOpp.h > 6 ||
      lenOpp.d > 6 ||
      lenOpp.c > 6) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE; // no 7+
   }
   twLengths lenResp(lho);
   if (lenResp.s != 3 ||
      lenResp.h > 6 ||
      lenResp.d > 6 ||
      lenResp.c > 6) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // no 7+
   }
   twlHCP hcpOpp(rho);
   if (hcpOpp.total < 9) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // no 7+
   }
   twlHCP hcpResp(lho);
   if (hcpResp.total > 8) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 3; // no 7+
   }

   // two suiters
//    if (lenResp.s >= 5) {
//       if (lenResp.h > 5 ||
//          lenResp.c > 5) {
//          camp = SKIP_BY_OPP;
//          return ORDER_BASE + 2; // no two-suiters
//       }
//    } else if (lenResp.h + lenResp.c > 9) {
//       camp = SKIP_BY_OPP;
//       return ORDER_BASE + 2; // no two-suiters
//    }
//    if (lenOpp.s >= 5) {
//       if (lenOpp.h > 5 ||
//          lenOpp.c > 5) {
//          camp = SKIP_BY_RESP;
//          return ORDER_BASE + 2; // no two-suiters
//       }
//    } else if (lenOpp.h + lenOpp.c > 9) {
//       camp = SKIP_BY_RESP;
//       return ORDER_BASE + 2; // no two-suiters
//    }

   // seems it passes
   return 0;
}
#endif 

#ifdef SEMANTIC_DEC21_DBL_ON_3NT
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::Dec21Dbl3NT;
   sem.onScoring = &Walrus::Score_OpLead3NTX;
}

// OUT: camp
uint WaFilter::Dec21Dbl3NT(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;
   const uint SKIP_BY_RESP = 3;

   // LHO: 1h, 2h, raise to 3NT
   twlHCP hcpDummy(lho);
   if (hcpDummy.total < 8 || 10 < hcpDummy.total) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 1;
   }
   twLengths lenDummy(lho);
   if (lenDummy.s > 4 ||
      lenDummy.h > 6 ||
      lenDummy.d > 4 ||
      lenDummy.c > 4) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 2;
   }
   if (lenDummy.s < 2 ||
      lenDummy.h < 5 ||
      lenDummy.d < 2 ||
      lenDummy.c < 2) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 3;// no singletons, 5-6 h
   }


#ifdef OPINION_BASHA
   // always 6h, only AQ or AK there
   if (lenDummy.h != 6 || hcpDummy.h < 6) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 4;
   }
   if (hcpDummy.total > 9) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 5;
   }
   twlControls ctrDummy(lho);
   if (ctrDummy.h < 2) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 4;
   }
#else // end OPINION
   if (lenDummy.h == 5) {
      // =5h => 10 hcp
      if (hcpDummy.total < 10) {
         camp = SKIP_BY_RESP;
         return ORDER_BASE + 4;
      }
   } else if (hcpDummy.total > 9) {
      // =6h => 8-9 hcp
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 5;
   }
#endif

   // partner: passss
   twLengths lenPart(partner);
   if (lenPart.s > 6 ||
      lenPart.h > 6 ||
      lenPart.d > 6 ||
      lenPart.c > 6) {
      camp = SKIP_BY_PART;
      return ORDER_BASE; // no 7+ suits
   }
   twlHCP hcpPart(partner);
   if (hcpPart.total > 11) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // no overcall
   }
   if (hcpPart.total > 7) {
      if (lenPart.s > 4) {
         camp = SKIP_BY_PART;
         return ORDER_BASE + 2; // no overcall
      }
   }
   if (hcpPart.total > 6) {
      if (lenPart.s > 4) {
         if (lenPart.c > 4 ||
            lenPart.d > 4) {
            camp = SKIP_BY_PART;
            return ORDER_BASE + 3; // Michaels
         }
      }
      if (lenPart.c > 4 && lenPart.d > 4) {
         camp = SKIP_BY_PART;
         return ORDER_BASE + 4; // Michaels
      }
      if (lenPart.s > 5 && hcpPart.s > 5 ||
         lenPart.d > 5 && hcpPart.d > 5 ||
         lenPart.c > 5 && hcpPart.c > 5) {
         camp = SKIP_BY_PART;
         return ORDER_BASE + 5; // no overcall
      }
   }

   // RHO: 1c, 1NT, 2NT
   twlHCP hcpDecl(rho);
   if (hcpDecl.total < 13 || 14 < hcpDecl.total) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1;
   }
   twLengths lenDecl(rho);
   if (lenDecl.s > 4 ||
      lenDecl.d > 5 ||
      lenDecl.c > 5) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 3;// kind of NT, allow 4 spades
   }
   if (lenDecl.s < 2 ||
      lenDecl.d < 2 ||
      lenDecl.c < 2) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 4;// kind of NT
   }
   if (lenDecl.s == 4) {
      if (hcpDecl.s > 1) {
         camp = SKIP_BY_OPP;
         return ORDER_BASE + 5;// allow only weak 4s
      }
   }

#ifdef OPINION_BASHA
   // always three aces, always 3h
   if (lenDecl.h != 3) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2;
   }
   twlControls ctrDecl(rho);
   if (ctrDecl.s < 2 ||
       ctrDecl.d < 2 ||
       ctrDecl.c < 2) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 5;
   }
#else // end OPINION
   if (lenDecl.h != 2) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2;
   }
   if (hcpDecl.h < 2) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 5;
   }
#endif

   // seems it passes
   return 0;
}
#endif // SEMANTIC_DEC21_DBL_ON_3NT

#ifdef SEMANTIC_JAN_SPADES_GAME
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::JanSpadesGame;
   sem.onScoring = &Walrus::Score_NV_4Major;
}

uint WaFilter::JanSpadesGame(SplitBits &partner, uint &camp, SplitBits &rho, SplitBits &lho)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_RESP = 2;
   const uint SKIP_BY_OPP = 3;

   twlHCP hcpPart(partner);
   twLengths lenPart(partner);
#ifdef ACCEPT_INVITE
   if (hcpPart.total < 15 || 16 < hcpPart.total) {// 1c 14-16
      camp = SKIP_BY_PART;
      return ORDER_BASE; // 
   }
   if ((lenPart.s < 4) && (hcpPart.total == 15)) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 3;// 
   }
#else // ACCEPT_INVITE
   if (hcpPart.total < 14 || 15 < hcpPart.total) {// 1c 14-16
      camp = SKIP_BY_PART;
      return ORDER_BASE; // wrong points count
   }
   if ((lenPart.s == 4) && (hcpPart.total == 15)) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 3;// would accept invitation
   }
#endif // ACCEPT_INVITE
   if (lenPart.s > 4 ||
      lenPart.h > 4 ||
      lenPart.d > 5 ||
      lenPart.c > 5) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // kind of NT
   }
   if (lenPart.s < 2 ||
      lenPart.h < 2 ||
      lenPart.d < 2 ||
      lenPart.c < 2) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2;// kind of NT
   }

   twlHCP hcpOpp(rho);
   if (hcpOpp.total < 8 || 15 < hcpOpp.total) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // wrong points count, bid h
   }
   twLengths lenOpp(rho);
   if (lenOpp.s > 4 ||
      lenOpp.d > 5 ||
      lenOpp.c > 5) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // allow only 5-5 with a minor
   }
   if (lenOpp.h < 5 || 6 < lenOpp.h) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 3; // bid 1h
   }
   if (lenOpp.h == 6 && hcpOpp.total < 11) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 4; // would bid 2h
   }

   twlHCP hcpResp(lho);
   twLengths lenResp(lho);
   if (lenResp.h > 3 && hcpResp.total > 2) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE; // wrong points count, bid h
   }
   if (lenResp.s > 6 ||
      lenResp.d > 6 ||
      lenResp.c > 6) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // no 7+ suits
   }

   // seems it passes
   return 0;
}
#endif // SEMANTIC_JAN_SPADES_GAME

#ifdef SEMANTIC_MORO_SLAM
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::SlamTry;
   //sem.onScoring = &Walrus::Score_NV_5Minor;
   sem.onScoring = &Walrus::Score_NV6Minor;
   //sem.onScoring = &Walrus::Score_3NT;
}

uint WaFilter::SlamTry(SplitBits &partner, uint &camp, SplitBits &rho, SplitBits &lho)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;
   const uint SKIP_BY_RESP = 3;

   // partner: 1NT
   twlHCP hcpPart(partner);
   //if (hcpPart.total < 15 || 17 < hcpPart.total) {
   //if (hcpPart.total < 14 || 16 < hcpPart.total) {
   if (hcpPart.total < 9 || 12 < hcpPart.total) {
      camp = SKIP_BY_PART;
      return ORDER_BASE; // NT
   }
   twLengths lenPart(partner);
   if (lenPart.s < 4 || 5 < lenPart.s ||
      lenPart.h > 4 ||
      lenPart.c > 4) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // no 5-5, no 6+s
   }
   if (lenPart.d > 4 && hcpPart.total > 10 ) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2;// try on diamonds
   }
   if (lenPart.s == 4 && lenPart.h == 4) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 4;// would 1h, not 1s
   }
   twlControls ctrlPart(partner);
   bool hasAceAndKD = (ctrlPart.d > 0 && (ctrlPart.h > 1 || ctrlPart.c > 1));
   bool hasTwoAces = (ctrlPart.h > 1 && ctrlPart.c > 1);
   if (hasAceAndKD || hasTwoAces) {
      // ok 
   } else  {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 3;// wrong keycards
   }

   // RHO, LHO: passs
   twLengths lenOpp(rho);
   if (lenOpp.s > 6 ||
      lenOpp.h > 6 ||
      lenOpp.d > 6 ||
      lenOpp.c > 6) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE; // no 7+
   }
   twLengths lenResp(lho);
   if (lenResp.s > 6 ||
      lenResp.h > 6 ||
      lenResp.d > 6 ||
      lenResp.c > 6) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // no 7+
   }

   // two suiters
   if (lenResp.s >= 5) {
      if (lenResp.h > 5 ||
         lenResp.c > 5) {
         camp = SKIP_BY_OPP;
         return ORDER_BASE + 2; // no two-suiters
      }
   } else if (lenResp.h + lenResp.c > 9) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // no two-suiters
   }
   if (lenOpp.s >= 5) {
      if (lenOpp.h > 5 ||
         lenOpp.c > 5) {
         camp = SKIP_BY_RESP;
         return ORDER_BASE + 2; // no two-suiters
      }
   } else if (lenOpp.h + lenOpp.c > 9) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 2; // no two-suiters
   }

   // seems it passes
   return 0;
}

#endif // SEMANTIC_MORO_SLAM

#ifdef SEEK_MAGIC_FLY
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::Aug3NTOnFit;
   sem.onScoring = &Walrus::Score_4Major;
}
#endif

#ifdef SEMANTIC_AUG_3NT_ON_SPADE_FIT
uint WaFilter::Aug3NTOnFit(SplitBits& partner, uint& camp, SplitBits& rho, SplitBits& lho)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;
   const uint SKIP_BY_RESP = 3;

   // partner: 1c-1s-2s
   twlHCP hcpPart(partner);
   //if (hcpPart.total < 17 || 18 < hcpPart.total) {
   if (hcpPart.total != 18) {
      camp = SKIP_BY_PART;
      return ORDER_BASE; // min strong on S
   }
   twLengths lenPart(partner);
   if (lenPart.s < 6 || 7 < lenPart.s ||
      lenPart.h > 3 ||
      lenPart.d > 3 ||
      lenPart.c > 3) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // no other longer
   }

   // RHO, LHO: passs
   twLengths lenOpp(rho);
   if (lenOpp.s > 6 ||
      lenOpp.h > 6 ||
      lenOpp.d > 6 ||
      lenOpp.c > 6) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE; // no 7+
   }
   twLengths lenResp(lho);
   if (lenResp.s > 6 ||
      lenResp.h > 6 ||
      lenResp.d > 6 ||
      lenResp.c > 6) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // no 7+
   }

   // two suiters
   if (lenResp.s >= 5) {
      if (lenResp.h > 5 ||
         lenResp.c > 5) {
         camp = SKIP_BY_OPP;
         return ORDER_BASE + 2; // no two-suiters
      }
   }
   else if (lenResp.h + lenResp.c > 9) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // no two-suiters
   }
   if (lenOpp.s >= 5) {
      if (lenOpp.h > 5 ||
         lenOpp.c > 5) {
         camp = SKIP_BY_RESP;
         return ORDER_BASE + 2; // no two-suiters
      }
   }
   else if (lenOpp.h + lenOpp.c > 9) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 2; // no two-suiters
   }

   // seems it passes
   return 0;
}
#endif // SEMANTIC_AUG_3NT_ON_SPADE_FIT

#ifdef SEMANTIC_AUG_3NT2_ON_SPADE_FIT
uint WaFilter::Aug3NTOnFit(SplitBits& partner, uint& camp, SplitBits& rho, SplitBits& lho)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;
   const uint SKIP_BY_RESP = 3;

   // partner: 1c-1s-2s
   twlHCP hcpPart(partner);
   //if (hcpPart.total < 17 || 18 < hcpPart.total) {
   if (hcpPart.total != 20) {
      camp = SKIP_BY_PART;
      return ORDER_BASE; // min strong on S
   }
   twLengths lenPart(partner);
   if (lenPart.s < 6 || 7 < lenPart.s ||
      lenPart.h > 3 ||
      lenPart.d > 3 ||
      lenPart.c > 3) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // no other longer
   }
   if (lenPart.h < 2 ||
      lenPart.c < 2) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2; // no c/h singleton
   }

   // RHO, LHO: passs
   twLengths lenOpp(rho);
   if (lenOpp.s > 6 ||
      lenOpp.h > 6 ||
      lenOpp.d > 6 ||
      lenOpp.c > 6) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE; // no 7+
   }
   twLengths lenResp(lho);
   if (lenResp.s > 6 ||
      lenResp.h > 6 ||
      lenResp.d > 6 ||
      lenResp.c > 6) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // no 7+
   }

   // two suiters
   if (lenResp.s >= 5) {
      if (lenResp.h > 5 ||
         lenResp.c > 5) {
         camp = SKIP_BY_OPP;
         return ORDER_BASE + 2; // no two-suiters
      }
   }
   else if (lenResp.h + lenResp.c > 9) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // no two-suiters
   }
   if (lenOpp.s >= 5) {
      if (lenOpp.h > 5 ||
         lenOpp.c > 5) {
         camp = SKIP_BY_RESP;
         return ORDER_BASE + 2; // no two-suiters
      }
   }
   else if (lenOpp.h + lenOpp.c > 9) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 2; // no two-suiters
   }

   // seems it passes
   return 0;
}
#endif // SEMANTIC_AUG_3NT2_ON_SPADE_FIT

#ifdef SEMANTIC_AUG_3NT_ON_44H
uint WaFilter::Aug3NTOnFit(SplitBits& partner, uint& camp, SplitBits& rho, SplitBits& lho)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;
   const uint SKIP_BY_RESP = 3;

   // partner: 2NT
   twlHCP hcpPart(partner);
   if (hcpPart.total < 20 || 21 < hcpPart.total) {
   //if (hcpPart.total < 18 || 19 < hcpPart.total) {
   //if (hcpPart.total != 19) {
      camp = SKIP_BY_PART;
      return ORDER_BASE; // 2NT opening
   }
   twLengths lenPart(partner);
   if (lenPart.s < 2 || 4 < lenPart.s ||
      lenPart.h != 4 ||
      lenPart.d < 2 || 4 < lenPart.d ||
      lenPart.c < 2 || 4 < lenPart.c) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // no other longer
   }

   // RHO, LHO: passs
   twLengths lenOpp(rho);
   if (lenOpp.s > 6 ||
      lenOpp.h > 6 ||
      lenOpp.d > 6 ||
      lenOpp.c > 6) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE; // no 7+
   }
   twLengths lenResp(lho);
   if (lenResp.s > 6 ||
      lenResp.h > 6 ||
      lenResp.d > 6 ||
      lenResp.c > 6) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // no 7+
   }

   // two suiters 65
   if (lenResp.s >= 5) {
      if (lenResp.d > 5 ||
         lenResp.c > 5) {
         camp = SKIP_BY_OPP;
         return ORDER_BASE + 2; // no 65 two-suiters
      }
   }
   else if (lenResp.d + lenResp.c > 10) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // no 65 two-suiters
   }
   if (lenOpp.s >= 5) {
      if (lenOpp.d > 5 ||
         lenOpp.c > 5) {
         camp = SKIP_BY_RESP;
         return ORDER_BASE + 2; // no 65 two-suiters
      }
   }
   else if (lenOpp.d + lenOpp.c > 10) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 2; // no 65 two-suiters
   }

   // seems it passes
   return 0;
}
#endif // SEMANTIC_AUG_3NT_ON_44H

#ifdef SEMANTIC_SEPT_INVITE_OR_FG
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::Sep10_4252;
   sem.onScoring = &Walrus::Score_3NT;
}

uint WaFilter::Sep10_4252(SplitBits& partner, uint& camp, SplitBits& rho, SplitBits& lho)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_RESP = 2;
   const uint SKIP_BY_OPP = 3;

   twlHCP hcpPart(partner);
   if (hcpPart.total < 14 || 15 < hcpPart.total) {// 1c(nv) opener min
      camp = SKIP_BY_PART;
      return ORDER_BASE; // wrong points count
   }
   if (hcpPart.total == 15 && hcpPart.d > 2) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1;
   }
   twLengths lenPart(partner);
   if (lenPart.s < 2 || 3 < lenPart.s ||
      lenPart.h < 2 || 4 < lenPart.h ||
      lenPart.d < 2 || 5 < lenPart.d ||
      lenPart.c < 2 || 5 < lenPart.c) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2; // no other longer
   }

   twlHCP hcpOpp(rho);
   if (hcpOpp.total > 12) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // wrong points count, would overcall
   }

   twLengths lenR(rho);
   if (lenR.h > 6 ||
      lenR.d > 6 ||
      lenR.c > 6) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 3;// no 7+ suits
   }

   twLengths lenL(rho);
   if (lenL.h > 6 ||
      lenL.d > 6 ||
      lenL.c > 6) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE;// no 7+ suits
   }

   // two suiters 65
   if (lenR.s >= 5) {
      if (lenR.d > 5 ||
         lenR.c > 5) {
         camp = SKIP_BY_OPP;
         return ORDER_BASE + 2; // no 65 two-suiters
      }
   }
   else if (lenR.d + lenR.c > 10) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // no 65 two-suiters
   }
   if (lenL.s >= 5) {
      if (lenL.d > 5 ||
         lenL.c > 5) {
         camp = SKIP_BY_RESP;
         return ORDER_BASE + 2; // no 65 two-suiters
      }
   }
   else if (lenL.d + lenL.c > 10) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 2; // no 65 two-suiters
   }

   // seems it passes
   return 0;
}
#endif 

#ifdef SEMANTIC_OCT_WEAK_GAMBLING
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::OctWeakGambling;
   sem.onScoring = &Walrus::Score_NV_5Minor;
   sem.onSolvedTwice = &Walrus::Score_Opp3Major;
}

uint WaFilter::OctWeakGambling(SplitBits& partner, uint& camp, SplitBits& rho, SplitBits& lho)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_RESP = 2;
   const uint SKIP_BY_OPP = 3;

   twlHCP hcpPart(partner);
   if (hcpPart.total < 11 || 12 < hcpPart.total) {// normal invitation
      camp = SKIP_BY_PART;
      return ORDER_BASE; // wrong points count
   }
   twLengths lenPart(partner);
   if (4 < lenPart.s ||
      lenPart.h < 6 || 7 < lenPart.h ||
      4 < lenPart.d ||
      3 < lenPart.c) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2; // no other longer
   }

   twLengths lenOpp(lho);
   if (lenOpp.h > 6 ||
      lenOpp.d > 6 ||
      lenOpp.s > 3) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 3;// no 7+ suits, no 4+ support spades
   }

   twlHCP hcpDir(lho);
   if (hcpDir.total > 11) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // wrong points count, would overcall
   }

   twlHCP hcpSand(rho);
   if (hcpSand.total < 10) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 2; // wrong points count, would overcall
   }


   twLengths lenSand(rho);
   if (lenSand.s < 6 || 7 < lenSand.s ||
      4 < lenSand.d) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE;// overcalled 3s
   }

   // overcall on 5s only with more strength
//    if (lenSand.s == 5) {
//       if (hcpSand.total < 12 ||
//           3 < lenSand.h ||
//           4 < lenSand.d ) {
//       }
//       camp = SKIP_BY_RESP;
//       return ORDER_BASE + 1;
//    }

   // seems it passes
   return 0;
}
#endif // SEMANTIC_OCT_WEAK_GAMBLING

#ifdef SEMANTIC_NOV_INVITE_PRECISION
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::NovInvitePrecision;
   sem.onScoring = &Walrus::Score_4Major;
}

uint WaFilter::NovInvitePrecision(SplitBits& partner, uint& camp, SplitBits& rho, SplitBits& lho)
{
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_RESP = 2;
   const uint SKIP_BY_OPP = 3;

   twlHCP hcpPart(partner);
   if (hcpPart.total < 14 || 15 < hcpPart.total) {// max of 2c
      camp = SKIP_BY_PART;
      return ORDER_BASE; // wrong points count
   }
   twLengths lenPart(partner);
   if (4 != lenPart.s ||
      3 < lenPart.h ||
      //4 < lenPart.d ||
      5 != lenPart.c) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2; // 5-4 spades
   }

   twLengths lenOpp(lho);
   if (lenOpp.h > 6 ||
      lenOpp.d > 6 ) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 3;// no 7+ suits
   }

   twlHCP hcpDir(lho);
   if (hcpDir.total > 11 && lenOpp.c < 3) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // wrong points count, would overcall
   }
   if (hcpDir.total > 13) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE; // wrong points count, would overcall
   }

   twlHCP hcpSand(rho);
   if (hcpSand.total > 12) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE + 2; // wrong points count, would overcall
   }


   twLengths lenSand(rho);
   if (lenSand.h > 6 || 
       lenSand.d > 6) {
      camp = SKIP_BY_RESP;
      return ORDER_BASE;// no 7+ suits
   }

   // seems it passes
   return 0;
}

#endif // SEMANTIC_NOV_INVITE_PRECISION


#ifdef SEMANTIC_JAN_INVITE_MINORS
void Walrus::FillSemantic(void)
{
   Orb_DepFillSem();
   sem.onDepFilter = &WaFilter::SomeInvite;
   sem.onScoring = &Walrus::Score_5Minor;
   sem.onSolvedTwice = &Walrus::Score_Opp3NT;
}

uint WaFilter::SomeInvite(SplitBits& partner, uint& camp, SplitBits& direct, SplitBits& sand)
{
   twlHCP hcpPart(partner);
   if (hcpPart.total < 11 || 13 < hcpPart.total) {// min of 1d-1s-2c
      camp = SKIP_BY_PART;
      return ORDER_BASE; // wrong points count
   }
   twLengths lenPart(partner);
   if (lenPart.d + lenPart.c < 9) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // 1d-1s-2c
   }
   if (2 < lenPart.s ||
      4 < lenPart.h ||
      5 < lenPart.d ||
      5 < lenPart.c) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2; // 1d-1s-2c
   }
   twlControls ctrPart(partner);
   if (hcpPart.total > 11) {
      if (ctrPart.total > 4) {
         camp = SKIP_BY_PART;
         return ORDER_BASE + 4; // 12+ hcp 5+ ctrl
      }
      if (lenPart.c == 5 && lenPart.d == 5) {
         if (ctrPart.total > 3) {
            camp = SKIP_BY_PART;
            return ORDER_BASE + 3; // 5-5 and 12+ hcp 4+ctrl
         }
      }
   }

   twLengths lenDir(direct);
   twlHCP hcpDir(direct);
   if (hcpDir.total > 11 && lenDir.d < 3) {
      camp = SKIP_BY_DIRECT;
      return ORDER_BASE; // would overcall
   }
   if (hcpDir.total > 5) {
      if (lenDir.h > 5 ||
         lenDir.s > 5) {
         camp = SKIP_BY_DIRECT;
         return ORDER_BASE + 1;// no preempt
      }
      if (lenDir.h + lenDir.s > 9) {
         camp = SKIP_BY_DIRECT;
         return ORDER_BASE + 2;// michaels
      }
   }

   if (hcpDir.h > 4 && lenDir.h > 5) {
      camp = SKIP_BY_DIRECT;
      return ORDER_BASE + 3;// h preempt
   }
   if (hcpDir.s > 4 && lenDir.s > 5) {
      camp = SKIP_BY_DIRECT;
      return ORDER_BASE + 3;// s preempt
   }

   if (hcpDir.total > 8) {
      if (lenDir.h > 4 ||
         lenDir.s > 4) {
         camp = SKIP_BY_DIRECT;
         return ORDER_BASE + 4; // would overcall
      }
   }

   twLengths lenSand(sand);
   if (lenSand.h > 6 ||
      lenSand.c > 6) {
      camp = SKIP_BY_SANDWICH;
      return ORDER_BASE;// no 7+ suits
   }

   twlHCP hcpSand(sand);
   if (hcpSand.total > 5) {
      if (lenSand.h > 5) {
         camp = SKIP_BY_SANDWICH;
         return ORDER_BASE + 1; // wrong points count, would overcall
      }
   }

   if (hcpSand.total > 11) {
      if (lenSand.h > 4) {
         camp = SKIP_BY_SANDWICH;
         return ORDER_BASE + 2; // wrong points count, would overcall
      }
   }

   // detect stopper
   bool hasStopper = 
      (hcpPart.h > 3) ||
      (hcpPart.h > 2 && lenPart.h > 1) || 
      (hcpPart.h > 1 && lenPart.h > 2) ||
      (hcpPart.h > 0 && lenPart.h > 3);

   if (!hasStopper) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 6; // lucky coin
   }

   // seems it passes
   return 0;
}

#endif // SEMANTIC_JAN_INVITE_MINORS

// ------------------------------------------------------------------------
// 3.0 micro-filters
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


