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
   void VoidGenScoring(DdsTricks& tr) {}
   void VoidSingleScoring(uint tricks) {}

   // linear-score oriented
   struct LineScorer {
      LineScorer();
      bool Init(s64 &out, const char* code);
      void TargetOut(s64 &out);
      bool IsEmpty() const;
      s64  Get(uint tricks);
      void operator () (uint tricks);
      void FillUpon(s64 *ourBase, const LineScorer& other, const s64 *thatBase);
      int  Goal() const;
      int  Trump() const;
      int  Decl() const;
   private:
      const char* title;
      const s64* linearBase;
      s64* outSum;
   };
   LineScorer  prima, secunda, tertia;
   void DepPrimary     (uint tricks)   { prima  (tricks); }
   void DepSecondary   (uint tricks)   { secunda(tricks); }
   void DepTertiary    (uint tricks)   { tertia (tricks); }
   void Primary        (DdsTricks &tr) { prima  (tr.plainScore); }
   void Secondary      (DdsTricks &tr) { secunda(tr.plainScore); }
   void Tertiary       (DdsTricks &tr) { tertia (tr.plainScore); }
   void BiddingLevel(uint tricks);
   void OpeningLead (DdsTricks &tr);
   void FillSameLinears(const CumulativeScore &other);
};

extern void PrepareLinearScores();
extern const s64* FindLinearScore(const char* code);
