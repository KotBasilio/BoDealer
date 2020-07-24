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
  sem.onFilter = &Walrus::TriFitoJuly_FilterOut;
  sem.onScoring = &Walrus::Score_4Major;
  sem.onOppContract = &Walrus::Score_3MajorDoubled;
}

// OUT: camp
uint Walrus::TriFitoJuly_FilterOut(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
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

#ifdef SEMANTIC_JUNE_MAX_5D_LEAD
void Walrus::FillSemantic(void)
{
	Orb_FillSem();
	sem.onFilter = &Walrus::LeadMax5D_FilterOut;
	sem.onScoring = &Walrus::Score_OpLead5DX;
}

uint Walrus::LeadMax5D_FilterOut(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
	const uint ORDER_BASE = 3;
	const uint SKIP_BY_PART = 1;
	const uint SKIP_BY_OPP = 2;
	const uint SKIP_BY_DECL = 3;

	// LHO: fr-nat pass, raise D, pass
	// partner: pass, fit 3h
	// RHO: fr-nat 1D, pass on 3h, sacrifice 5D
	twlHCP hcpDecl(rho);
	if (hcpDecl.total < 10 || 17 < hcpDecl.total) {
		camp = SKIP_BY_DECL;
		return ORDER_BASE; // wrong points count
	}
	twLengths lenDecl(rho);
	if (lenDecl.d < 4) {
		camp = SKIP_BY_DECL;
		return ORDER_BASE + 1; 
	}
	if (lenDecl.h > 2 || lenDecl.s > 4) {
		camp = SKIP_BY_DECL;
		return ORDER_BASE + 2;
	}
	if (lenDecl.h == 2 && lenDecl.d == 4) {
		camp = SKIP_BY_DECL;
		return ORDER_BASE + 3;
	}
	if (lenDecl.s == 4 && hcpDecl.s > 4) {
		camp = SKIP_BY_DECL;
		return ORDER_BASE + 4;
	}

	twlHCP hcpOpp(lho);
	if (hcpOpp.total < 2 || 9 < hcpOpp.total) {
		camp = SKIP_BY_OPP;
		return ORDER_BASE + 2; // wrong points count
	}
	twLengths lenOpp(lho);
	if (lenOpp.d < 4 || 5 < lenOpp.d) {
		camp = SKIP_BY_OPP;
		return ORDER_BASE + 3;
	}
	if (lenOpp.s > 4 && hcpOpp.total > 7) {
		camp = SKIP_BY_OPP;
		return ORDER_BASE + 4;
	}
	if (6 < lenOpp.c) {
		camp = SKIP_BY_OPP;
		return ORDER_BASE + 5;
	}
	if (7 < hcpOpp.total && 2 < hcpOpp.h) {
		camp = SKIP_BY_OPP;
		return ORDER_BASE + 6; // 6-8 and stopper
	}

	twlHCP hcpPart(partner);
	if (hcpPart.total < 6 || 11 < hcpPart.total) {
		camp = SKIP_BY_PART;
		return ORDER_BASE + 3; // might do something
	} 

	twLengths lenPart(partner);
	if (lenPart.h < 3 || 4 < lenPart.h) {
		camp = SKIP_BY_PART;
		return ORDER_BASE + 4; // not 3h
	}

	if (hcpPart.total < 8 && lenPart.h < 4) {
		camp = SKIP_BY_PART;
		return ORDER_BASE + 5; // too weak fit
	}

	if (5 < lenPart.s) {
		camp = SKIP_BY_PART;
		return ORDER_BASE + 6; // multi
	}

	if (6 < lenPart.c) {
		camp = SKIP_BY_PART;
		return ORDER_BASE + 7; // club preempt
	}

	// seems it passes
	return 0;
}

#endif // SEMANTIC_JUNE_MAX_5D_LEAD

#ifdef SEMANTIC_JUNE_LEAD_3343
void Walrus::FillSemantic(void)
{
   Orb_FillSem();
   sem.onFilter = &Walrus::LeadFlat_FilterOut;
   sem.onScoring = &Walrus::Score_OpLead3NT;
}

uint Walrus::LeadFlat_FilterOut(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
   const uint ORDER_BASE = 5;
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;
   const uint SKIP_BY_DECL = 3;

   // LHO: fr-nat 1s, raise to 3NT
   // partner: pass
   // RHO: 1NT
   twlHCP hcpDecl(rho);
   if (hcpDecl.total < 5 || 9 < hcpDecl.total) {
      camp = SKIP_BY_DECL;
      return ORDER_BASE; // wrong points count
   }
   twlHCP hcpOpp(lho);
   if (hcpOpp.total < 16 || 20 < hcpOpp.total) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // wrong points count
   }

   twLengths lenOpp(lho);
   if (lenOpp.s < 5 || 6 < lenOpp.s) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // wrong spades count
   }

   twLengths lenDecl(rho);
   if (lenDecl.s > 2) {
      camp = SKIP_BY_DECL;
      return ORDER_BASE + 3; // didn't fit
   }

   twLengths lenPart(partner);
   if (lenPart.s > 6 ||
      lenPart.h > 6 ||
      lenPart.d > 6) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 3; // might preempt
   }

   if (lenOpp.s == 6) {
      if (lenOpp.h > 3 || lenOpp.d > 3 || lenOpp.c > 3) {
         camp = SKIP_BY_OPP;
         return ORDER_BASE + 4; // 6-4 
      }
   } else {
      if (lenOpp.h > 4 || lenOpp.d > 4 || lenOpp.c > 4) {
         camp = SKIP_BY_OPP;
         return ORDER_BASE + 4; // 5-5 
      }
   }

   twlHCP hcpPart(partner);
   if (hcpPart.total >= 12) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 5; // might do something
   } else if (hcpPart.total >= 10) {
      if (lenPart.c > 4 ||
         lenPart.h > 4 ||
         lenPart.d > 4) {
         camp = SKIP_BY_PART;
         return ORDER_BASE + 6; // might overcall
      }
   }

   // seems it passes
   return 0;
}


#endif // SEMANTIC_JUNE_LEAD_3343

#ifdef SEMANTIC_JUNE_ZAKHAROVY_PREC_3NT
void Walrus::FillSemantic(void)
{
   Orb_FillSem();
   sem.onFilter = &Walrus::JuneVZ_FilterOut;
   sem.onScoring = &Walrus::Score_OpLead3NT;
}

uint Walrus::JuneVZ_FilterOut(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
   const uint ORDER_BASE = 5;
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;
   const uint SKIP_BY_DECL = 3;

   // LHO: precision 2c
   // partner: pass
   // RHO: 3NT
   twlHCP hcpDecl(rho);
   if (hcpDecl.total != 13) {
      camp = SKIP_BY_DECL;
      return ORDER_BASE; // wrong points count
   }
   twlHCP hcpOpp(lho);
   if (hcpOpp.total < 11 || 15 < hcpOpp.total) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // wrong points count
   }

   twLengths lenOpp(lho);
   if (lenOpp.c < 5 || 6 < lenOpp.c) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // wrong clubs count
   }

   twLengths lenDecl(rho);
   if (lenDecl.h > 3 || lenDecl.s > 3) {
      camp = SKIP_BY_DECL;
      return ORDER_BASE + 3; // didn't ask 2d
   }

   twLengths lenPart(partner);
   if (lenPart.s > 6 ||
      lenPart.h > 6 ||
      lenPart.d > 6) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 3; // may preempt
   }

   if (lenOpp.h > 4 || lenOpp.s > 4) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 4; // 5 card majors
   }

   if (lenDecl.c > 5) {
      camp = SKIP_BY_DECL;
      return ORDER_BASE + 6; // would seek 5c
   }

   if (lenOpp.c == 5) {
      if (lenOpp.h < 4 && lenOpp.s < 4) {
         camp = SKIP_BY_OPP;
         return ORDER_BASE + 5; // precision 54
      }
   }

   twlHCP hcpPart(partner);
   if (hcpPart.total >= 9) {
      if (lenPart.s > 5 ||
         lenPart.h > 5 ||
         lenPart.d > 5) {
         camp = SKIP_BY_PART;
         return ORDER_BASE + 6; // may overcall
      }
   }

   // seems it passes
   return 0;
}


#endif // SEMANTIC_JUNE_ZAKHAROVY_PREC_3NT

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


