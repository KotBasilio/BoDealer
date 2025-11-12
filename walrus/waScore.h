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
   void Init(const struct futureTricks &fut);
};

constexpr size_t WA_MAX_LENSES = 6;

struct LineScorer {
   LineScorer();
   bool Init(s64& out, const char* code);
   void TargetOut(s64& out);
   bool IsEmpty() const;
   bool HasDouble() const;
   s64  Get(uint tricks);
   void operator () (uint tricks);
   int  Goal() const;
   int  Trump() const;
   int  Decl() const;
private:
   const char* title;
   const s64* linearBase;
   s64* outSum;
   void FillUpon(s64* ourBase, const LineScorer& other, const s64* thatBase);
};

struct CumulativeScore {
   CumulativeScore();
   s64    ideal;
   s64    bidPartscore, bidGame;
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

   // linear-score oriented
   union {
      struct {
         LineScorer  prima, secunda;
      };
      struct {
         LineScorer  allScorers[WA_MAX_LENSES];
      };
   };
   uint idxVariator = 1; // secondary

   void Primary        (DdsTricks& tr) { prima  (tr.plainScore); }
   void Secondary      (DdsTricks &tr) { secunda(tr.plainScore); }
   void Variator       (DdsTricks &tr) { allScorers[idxVariator](tr.plainScore); }
   s64  GetFromVariator(uint tricks)   { return allScorers[idxVariator].Get(tricks); }
   void BiddingLevel   (DdsTricks &tr);
   void OpeningLead    (DdsTricks &tr);

   // deprecated
   void DepPrimary     (uint tricks)   { prima  (tricks); } 
   void DepSecondary   (uint tricks)   { secunda(tricks); }

   // UX/UI
   void ShowValues(char* tail);
};

extern void PrepareLinearScores();
extern const s64* FindLinearScore(const char* code);
