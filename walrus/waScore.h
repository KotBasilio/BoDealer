/************************************************************
 * Walrus Scoring                                        2024
 ************************************************************/

struct DdsTricks
{
   // number of tricks in basic contract
   uint plainScore; 

   // tricks for each opening lead
   struct Lead
   {
      uint S, H, D, C;
      Lead() { S = H = D = C = 13; }
   } lead;

   DdsTricks() : plainScore(0) {}
   void Init(struct futureTricks &fut);
};


struct CumulativeScore {
   CumulativeScore();
   s64    ideal;
   s64    bidPartscore, bidGame, bidSlam;
   union {
      struct {
         s64    leadS, leadH, leadD, leadC;
      };
      struct {
         s64    oppContract, oppCtrDoubled;
         s64    ourOther, ourHedging;
      };
   };
   // -- our contracts
   void VoidScoring(DdsTricks& tr) {}
   void VoidDepScoring(uint tricks) {}
   void OurNV6m(uint tricks);
   void OurNV6Maj(uint tricks);
   void OurNV6_No(uint tricks);
   void Our6_No(uint tricks);
   void OurNV7m(uint tricks);
   void Our3NT(uint tricks);
   void Our4M(uint tricks);
   void Our1NT(uint tricks);
   void Our2M(uint tricks);
   void Our4minor(uint tricks);
   void OurNV4M(uint tricks);
   void OurNV4MX(uint tricks);
   void Our5M(uint tricks);
   void OurNV5M(uint tricks);
   void Our5minor(uint tricks);
   void OurNV5minor(uint tricks);
   void OurNV5mX(uint tricks);
   // -- allowing for scorer2
   void Our2_NV7NT(uint tricks);

   // linear-score oriented
   struct Adjustable {
      Adjustable();
      bool Init(s64 &out, const char* code);
      void TargetOut(s64 &out);
      bool IsEmpty() const;
      s64  Get(uint tricks);
      void operator () (uint tricks);
      void FillUpon(s64 *ourBase, const Adjustable& other, const s64 *thatBase);
   private:
      const char* title;
      const s64* linearBase;
      s64* outSum;
   };
   Adjustable  prima, secunda, tertia;
   void DepPrimary     (uint tricks) { prima(tricks); }
   void DepSecondary   (uint tricks) { secunda(tricks); }
   void DepTertiary    (uint tricks) { tertia(tricks); }
   void Primary        (DdsTricks &tr) { DepPrimary  (tr.plainScore); }
   void Secondary      (DdsTricks &tr) { DepSecondary(tr.plainScore); }
   void Tertiary       (DdsTricks &tr) { DepTertiary (tr.plainScore); }
   void BiddingLevel(uint tricks);
   void OpeningLead (DdsTricks &tr);
   void FillSameLinears(const CumulativeScore &other);
};

extern void PrepareLinearScores();
extern const s64* FindLinearScore(const char* code);
