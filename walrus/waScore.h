/************************************************************
 * Walrus Scoring                                        2024
 ************************************************************/

struct DdsTricks
{
   // number of tricks in basic contract
   uint plainScore; 

   // tricks for each opening lead
   #ifdef SEEK_OPENING_LEAD
      struct Lead
      {
         uint S, H, D, N;
         Lead() { S = H = D = N = 13; }
      } lead;
   #endif // SEEK_OPENING_LEAD

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
   // -- opening lead
   void OpLead3NT(s64& sum, uint tricks);
   void OpLead3NTX(s64& sum, uint tricks);
   void OpLead3Major(s64& sum, uint tricks);
   void OpLead4Major(s64& sum, uint tricks);
   void OpLead5minor(s64& sum, uint tricks);
   void OpLead5mX(s64& sum, uint tricks);
   // -- opp contract
   void Score_Opp4Major(uint tricks);
   void Score_Opp3Major(uint tricks);
   void Score_Opp5Minor(uint tricks);
   void Score_Opp3NT(uint tricks);
   // -- our contracts
   void VoidScoring(uint tricks) {}
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
      bool IsEmpty();
      s64  Get(uint tricks);
      void operator () (uint tricks);
   private:
      const char* title;
      const s64* linearBase;
      s64* outSum;
   };
   Adjustable  prima, secunda, tertia;
   void Primary     (uint tricks) { prima(tricks); }
   void Secondary   (uint tricks) { secunda(tricks); }
   void Tertiary    (uint tricks) { tertia(tricks); }
   void BiddingLevel(uint tricks);

private:
   void Opp_3MajX(s64& sum, uint tricks);
   void Opp_3Major(s64& sum, uint tricks);
   void Opp_4M(s64& sum, uint tricks);
   void Opp_4MajX(s64& sum, uint tricks);
   void Opp_NV_4MajX(s64& sum, uint tricks);
   void Opp_3NT(s64& sum, uint tricks);
   void Opp_3NTX(s64& sum, uint tricks);
   void Opp_5minor(s64& sum, uint tricks);
   void Opp_5minorX(s64& sum, uint tricks);
   void Opp_2m(s64& sum, uint tricks);
   void Opp_2mX(s64& sum, uint tricks);
   void OurNV7NT(s64& sum, uint tricks);
};

extern void PrepareLinearScores();
extern const s64* FindLinearScore(const char* code);
