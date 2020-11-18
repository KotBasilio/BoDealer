/************************************************************
* Walrus project                                        2019
* Semantic parts of the tasks. They all differ by two small parts:
* -- how we init the deck;
* -- how we do the iteration.
*
************************************************************/
#include "walrus.h"

#ifdef SEMANTIC_NOV_DBL_ON_3NT
void Walrus::FillSemantic(void)
{
   Orb_FillSem();
   sem.onFilter = &Walrus::NovDbl3NT_FilterOut;
   sem.onScoring = &Walrus::Score_Doubled3NT;
}

// OUT: camp
uint Walrus::NovDbl3NT_FilterOut(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
   const uint ORDER_BASE = 3;
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
   Orb_FillSem();
   sem.onFilter = &Walrus::NovSlam_FilterOut;
   sem.onScoring = &Walrus::Score_NV6Minor;
   sem.onOppContract = &Walrus::Score_Opp4MajorDoubled;
}

// OUT: camp
uint Walrus::NovSlam_FilterOut(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
   const uint ORDER_BASE = 3;
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
   if (hcpPart.total < 13) {
   //if (hcpPart.total < 12) {
   //if (hcpPart.total < 10 || 11 < hcpPart.total) {
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
   Orb_FillSem();
   sem.onFilter = &Walrus::FitoJuly_FilterOut;
   sem.onScoring = &Walrus::Score_4Major;
   sem.onOppContract = &Walrus::Score_Opp3MajorDoubled;
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
		return ORDER_BASE + 4; // no long minors
	}

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

#ifdef SEMANTIC_SEPT_MAJORS54_18HCP
void Walrus::FillSemantic(void)
{
	Orb_FillSem();
	sem.onFilter = &Walrus::SeptMajors_FilterOut;
	sem.onScoring = &Walrus::Score_4Major;
}

uint Walrus::SeptMajors_FilterOut(SplitBits &partner, uint &camp, SplitBits &rho, SplitBits &lho)
{
	const uint ORDER_BASE = 3;
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
   Orb_FillSem();
   sem.onFilter = &Walrus::NovVoidwood_FilterOut;
   sem.onScoring = &Walrus::Score_NV6Major;
}

uint Walrus::NovVoidwood_FilterOut(SplitBits &partner, uint &camp, SplitBits &rho, SplitBits &lho)
{
   const uint ORDER_BASE = 3;
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
   Orb_FillSem();
   sem.onFilter = &Walrus::SeptMajors_FilterOut;
   sem.onScoring = &Walrus::Score_4Major;
}

uint Walrus::SeptMajors_FilterOut(SplitBits &partner, uint &camp, SplitBits &rho, SplitBits &lho)
{
   const uint ORDER_BASE = 3;
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

