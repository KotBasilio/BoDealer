/************************************************************
* Walrus project                                        2019
* Semantic parts of the tasks. They all differ by two small parts:
* -- how we init the deck;
* -- how we do the iteration.
*
************************************************************/
#include "walrus.h"

#ifdef SEMANTIC_JUNE_MAX_5D_LEAD
void Walrus::FillSemantic(void)
{
	Orb_FillSem();
	sem.onDepFilter = &WaFilter::LeadMax5D;
	sem.onScoring = &Walrus::Score_OpLead5DX;
}

uint WaFilter::LeadMax5D(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
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

#ifdef SEMANTIC_AUG_LEAD_VS_3H
void Walrus::FillSemantic(void)
{
	Orb_FillSem();
	sem.onDepFilter = &WaFilter::LeadAugVs3H;
	sem.onScoring = &Walrus::Score_OpLead3Major;
}

uint WaFilter::LeadAugVs3H(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
   const uint ORDER_BASE = 3;
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;
   const uint SKIP_BY_DECL = 3;

   twlHCP hcpDecl(rho);
   if (hcpDecl.total < 8 || 12 < hcpDecl.total) {
      camp = SKIP_BY_DECL;
      return ORDER_BASE; // wrong points count
   }
   twlHCP hcpResp(lho);
   if (hcpResp.total < 8 || 11 < hcpResp.total) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // wrong points count
   }

   twLengths lenResp(lho);
   twLengths lenDecl(rho);
   if (lenDecl.s < 5 || 6 < lenDecl.s) {
      camp = SKIP_BY_DECL;
      return ORDER_BASE + 2; // wrong spades count
   }
   if (lenDecl.h < 1 || 2 < lenDecl.h) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 3; // no h fit
   }

   twLengths lenPart(partner);
   if (lenPart.s > 6 ||
      lenPart.h > 6 ||
      lenPart.d > 6) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 3; // may preempt
   }

   if (2 < lenResp.s) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 4; // resp has no fit
   }
   if (lenResp.h < 6 || 7 < lenResp.h) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 5; // resp 6-7 hearts
   }
   if (lenResp.c > 4 || lenResp.d > 4) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 6; // resp no side 5+
   }

   // seems it passes
   return 0;
}


#endif // SEMANTIC_AUG_LEAD_VS_3H

#ifdef SEMANTIC_JUNE_LEAD_3343
void Walrus::FillSemantic(void)
{
   Orb_FillSem();
   sem.onDepFilter = &WaFilter::LeadFlat;
   sem.onScoring = &Walrus::Score_OpLead3NT;
}

uint WaFilter::LeadFlat(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
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
   sem.onDepFilter = &WaFilter::JuneVZ;
   sem.onScoring = &Walrus::Score_OpLead3NT;
}

uint WaFilter::JuneVZ(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
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

#ifdef SEMANTIC_IMPS_LEAD_LEVKOVICH
void Walrus::FillSemantic(void)
{
   Orb_FillSem();
   sem.onDepFilter = &WaFilter::NovLevk;
   sem.onScoring = &Walrus::Score_OpLead3NT;
}

uint WaFilter::NovLevk(SplitBits &partner, uint &camp, SplitBits &lho, SplitBits &rho)
{
   const uint ORDER_BASE = 5;
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;
   const uint SKIP_BY_DECL = 3;

   // LHO: preempt 3d
   // partner: pass
   // RHO: 3NT
   twlHCP hcpDecl(rho);
   if (hcpDecl.total < 16 || 19 < hcpDecl.total) {
      camp = SKIP_BY_DECL;
      return ORDER_BASE; // wrong points count
   }
   twlHCP hcpOpp(lho);
   if (hcpOpp.total < 7 || 10 < hcpOpp.total) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // wrong points count
   }

   twLengths lenOpp(lho);
   if (lenOpp.d < 6 || 7 < lenOpp.d) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 2; // wrong clubs count
   }

   twLengths lenDecl(rho);
   if (lenDecl.h > 5 || lenDecl.s > 5) {
      camp = SKIP_BY_DECL;
      return ORDER_BASE + 3; // didn't ask 2d
   }

   twlHCP hcpPart(partner);
   twLengths lenPart(partner);
   if (hcpPart.total > 14) {
      camp = SKIP_BY_PART;
      return ORDER_BASE; // all overcall
   }
   if (hcpPart.total > 11) {
      if (lenPart.s > 4 ||
         lenPart.h > 4 ||
         lenPart.d > 5) {
         camp = SKIP_BY_PART;
         return ORDER_BASE + 3; // may overcall
      }
   }

   // seems it passes
   return 0;
}
#endif // SEMANTIC_IMPS_LEAD_LEVKOVICH

#ifdef SEMANTIC_JAN_PETYA_VS_3NT
void Walrus::FillSemantic(void)
{
   Orb_FillSem();
   sem.onDepFilter = &WaFilter::NovLevk;
   sem.onScoring = &Walrus::Score_OpLead3NT;
}

uint WaFilter::NovLevk(SplitBits& partner, uint& camp, SplitBits& dummy, SplitBits& decl)
{
   const uint ORDER_BASE = 5;
   const uint SKIP_BY_PART = 1;
   const uint SKIP_BY_OPP = 2;
   const uint SKIP_BY_DECL = 3;

   // decl:  1NT
   // dummy: 3NT
   // partner: pass
   twlHCP hcpDecl(decl);
   if (hcpDecl.total < 15 || 17 < hcpDecl.total) {
      camp = SKIP_BY_DECL;
      return ORDER_BASE; // wrong points count
   }
   twlHCP hcpDummy(dummy);
   if (hcpDummy.total < 9 || 11 < hcpDummy.total) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 1; // wrong points count
   }

   twLengths lenDum(dummy);
   if (hcpDummy.total == 9) {// hcp => request 5+ minor
      if ((lenDum.d < 5 || 7 < lenDum.d) &&
          (lenDum.c < 5 || 7 < lenDum.c)) {
         camp = SKIP_BY_OPP;
         return ORDER_BASE + 2; // wrong minor
      }
   }

   // exclude 5+ major
   if (lenDum.h > 4 || 
       lenDum.s > 4) {
      camp = SKIP_BY_OPP;
      return ORDER_BASE + 3;
   }

   // allow =4M only in 4333
   if (lenDum.h == 4) {
      if (lenDum.s < 3 || lenDum.c < 3 || lenDum.d < 3) {
         camp = SKIP_BY_OPP;
         return ORDER_BASE + 4;
      }
   }
   if (lenDum.s == 4) {
      if (lenDum.h < 3 || lenDum.c < 3 || lenDum.d < 3) {
         camp = SKIP_BY_OPP;
         return ORDER_BASE + 5;
      }
   }

   twLengths lenDecl(decl);
   if (lenDecl.s > 4 ||
      lenDecl.h > 4 ||
      lenDecl.d > 5 ||
      lenDecl.c > 5) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 1; // kind of NT
   }
   if (lenDecl.s < 2 ||
      lenDecl.h < 2 ||
      lenDecl.d < 2 ||
      lenDecl.c < 2) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 2;// kind of NT
   }

   twLengths lenPart(partner);
   if (lenPart.s > 7 ||
      lenPart.h > 7 ||
      lenPart.c > 7 ||
      lenPart.d > 7) {
      camp = SKIP_BY_PART;
      return ORDER_BASE + 3; // may overcall
   }

   // seems it passes
   return 0;
}
#endif // SEMANTIC_JAN_PETYA_VS_3NT

