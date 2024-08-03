//
// Scoring experiments
//

#define  _CRT_SECURE_NO_WARNINGS
#include <string.h>

typedef unsigned int uint;
typedef signed long long s64;

constexpr uint SCO_ACTIONS = 3; // Pass, Dbl, ReDbl
constexpr uint SCO_LEVELS  = 7; // 1..7
constexpr uint SCO_DENOMS  = 3; // m, M, NT
constexpr uint SCO_TRICKS  = 14;// 0..13
constexpr uint SCO_VUL     = 2; // nv, V
constexpr uint SIZE_LINEAR_SCORES = SCO_ACTIONS * SCO_LEVELS * SCO_DENOMS * SCO_VUL * SCO_TRICKS ;
s64  gLinearScores[SIZE_LINEAR_SCORES + SCO_TRICKS];

static void MinorPartscore(s64*& cur, int level)
{
   int goal = 6 + level;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      if (tricks >= goal) {
         *cur++ = (tricks - 6) * 20 + 50;
      } else {
         *cur++ = -50 * (goal - tricks);
      }
   }
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      if (tricks >= goal) {
         *cur++ = *(cur - SCO_TRICKS);
      } else {
         *cur++ = *(cur - SCO_TRICKS) * 2;
      }
   }
}

static void MajorPartscore(s64*& cur, int level)
{
   int goal = 6 + level;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      if (tricks >= goal) {
         *cur++ = (tricks - 6) * 30 + 50;
      } else {
         *cur++ = -50 * (goal - tricks);
      }
   }
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      if (tricks >= goal) {
         *cur++ = *(cur - SCO_TRICKS);
      } else {
         *cur++ = *(cur - SCO_TRICKS) * 2;
      }
   }
}

static void NTPartscore(s64*& cur, int level)
{
   int goal = 6 + level;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      if (tricks >= goal) {
         *cur++ = (tricks - 6) * 30 + 60;
      } else {
         *cur++ = -50 * (goal - tricks);
      }
   }
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      if (tricks >= goal) {
         *cur++ = *(cur - SCO_TRICKS);
      } else {
         *cur++ = *(cur - SCO_TRICKS) * 2;
      }
   }
}

static void NTGame(s64*& cur, int level)
{
   NTPartscore(cur, level);
   cur -= SCO_TRICKS * 2;

   int goal = 6 + level;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      *cur++ += (tricks >= goal) ? 300 - 50 : 0;
   }
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      *cur++ += (tricks >= goal) ? 500 - 50 : 0;
   }
}

static void MajorGame(s64*& cur, int level)
{
   MajorPartscore(cur, level);
   cur -= SCO_TRICKS * 2;

   int goal = 6 + level;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      *cur++ += (tricks >= goal) ? 300 - 50 : 0;
   }
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      *cur++ += (tricks >= goal) ? 500 - 50 : 0;
   }
}

static void MinorGame(s64*& cur, int level)
{
   MinorPartscore(cur, level);
   cur -= SCO_TRICKS * 2;

   int goal = 6 + level;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      *cur++ += (tricks >= goal) ? 300 - 50 : 0;
   }
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      *cur++ += (tricks >= goal) ? 500 - 50 : 0;
   }
}

static void MinorSmall(s64*& cur, int level)
{
   MinorGame(cur, level);
   cur -= SCO_TRICKS * 2;

   int goal = 6 + level;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      *cur++ += (tricks >= goal) ? 500 : 0;
   }
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      *cur++ += (tricks >= goal) ? 750 : 0;
   }
}

static void MajorSmall(s64*& cur, int level)
{
   MajorGame(cur, level);
   cur -= SCO_TRICKS * 2;

   int goal = 6 + level;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      *cur++ += (tricks >= goal) ? 500 : 0;
   }
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      *cur++ += (tricks >= goal) ? 750 : 0;
   }
}

static void NTSmall(s64*& cur, int level)
{
   NTGame(cur, level);
   cur -= SCO_TRICKS * 2;

   int goal = 6 + level;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      *cur++ += (tricks >= goal) ? 500 : 0;
   }
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      *cur++ += (tricks >= goal) ? 750 : 0;
   }
}

static void MinorGrand(s64*& cur, int level)
{
   MinorSmall(cur, level);
   cur -= SCO_TRICKS * 2;

   int goal = 6 + level;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      *cur++ += (tricks >= goal) ? 500 : 0;
   }
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      *cur++ += (tricks >= goal) ? 750 : 0;
   }
}

static void MajorGrand(s64*& cur, int level)
{
   MajorSmall(cur, level);
   cur -= SCO_TRICKS * 2;

   int goal = 6 + level;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      *cur++ += (tricks >= goal) ? 500 : 0;
   }
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      *cur++ += (tricks >= goal) ? 750 : 0;
   }
}

static void NTGrand(s64*& cur, int level)
{
   NTSmall(cur, level);
   cur -= SCO_TRICKS * 2;

   int goal = 6 + level;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      *cur++ += (tricks >= goal) ? 500 : 0;
   }
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++) {
      *cur++ += (tricks >= goal) ? 750 : 0;
   }
}

static s64 DoubledScore[] = 
{ 
   0,     0,
   100,   200,
   300,   500,
   500,   800,
   800,   1100,
   1100,  1400,
   1400,  1700,
   1700,  2000,
   2000,  2300,
   2300,  2600,
   2600,  2900,
   2900,  3200,
   3200,  3600,
   3600,  3900
};

static void XMinor(s64*& cur, s64*& passed, int level)
{
   int goal = 6 + level;
   int gameAdd = (3 <= level && level <= 4) ? 300 - 50 : 0;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, passed++) {
      if (tricks >= goal) {
         *cur = *passed * 2 + 60 * (tricks-goal) + gameAdd;
      } else {
         *cur = - DoubledScore[ 2 * (goal - tricks) ];
      }
   }
   gameAdd = (3 <= level && level <= 4) ? 200 : 0;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, passed++) {
      if (tricks >= goal) {
         *cur = *(cur - SCO_TRICKS) + 100 * (tricks - goal) + gameAdd;
      } else {
         *cur = - DoubledScore[2 * (goal - tricks) + 1];
      }
   }
}

static void XMajor(s64*& cur, s64*& passed, int level)
{
   int goal = 6 + level;
   int gameAdd = (2 <= level && level <= 3) ? 300 - 50 : 0;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, passed++) {
      if (tricks >= goal) {
         *cur = *passed * 2 + 40 * (tricks - goal) + gameAdd;
      } else {
         *cur = -DoubledScore[2 * (goal - tricks)];
      }
   }
   gameAdd = (2 <= level && level <= 3) ? 200 : 0;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, passed++) {
      if (tricks >= goal) {
         *cur = *(cur - SCO_TRICKS) + 100 * (tricks - goal) + gameAdd;
      } else {
         *cur = -DoubledScore[2 * (goal - tricks) + 1];
      }
   }
}

static void XNT(s64*& cur, s64*& passed, int level)
{
   int goal = 6 + level;
   int gameAdd = (2 == level) ? 300 - 50 : 0;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, passed++) {
      if (tricks >= goal) {
         *cur = *passed * 2 + 40 * (tricks - goal) + gameAdd;
      } else {
         *cur = -DoubledScore[2 * (goal - tricks)];
      }
   }
   gameAdd = (2 == level) ? 200 : 0;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, passed++) {
      if (tricks >= goal) {
         *cur = *(cur - SCO_TRICKS) + 100 * (tricks - goal) + gameAdd;
      } else {
         *cur = -DoubledScore[2 * (goal - tricks) + 1];
      }
   }
}

static void XNTMGame(s64*& cur, s64*& passed, int level)
{
   int goal = 6 + level;
   int gameAdd = -250;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, passed++) {
      if (tricks >= goal) {
         *cur = *passed * 2 + 40 * (tricks - goal) + gameAdd;
      } else {
         *cur = -DoubledScore[2 * (goal - tricks)];
      }
   }
   gameAdd = 200;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, passed++) {
      if (tricks >= goal) {
         *cur = *(cur - SCO_TRICKS) + 100 * (tricks - goal) + gameAdd;
      } else {
         *cur = -DoubledScore[2 * (goal - tricks) + 1];
      }
   }
}

static void XMiGame(s64*& cur, s64*& passed, int level)
{
   int goal = 6 + level;
   int gameAdd = -250;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, passed++) {
      if (tricks >= goal) {
         *cur = *passed * 2 + 60 * (tricks - goal) + gameAdd;
      } else {
         *cur = -DoubledScore[2 * (goal - tricks)];
      }
   }
   gameAdd = 200;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, passed++) {
      if (tricks >= goal) {
         *cur = *(cur - SCO_TRICKS) + 100 * (tricks - goal) + gameAdd;
      } else {
         *cur = -DoubledScore[2 * (goal - tricks) + 1];
      }
   }
}

static void XMinorSlam(s64*& cur, s64*& passed, int level)
{
   int goal = 6 + level;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, passed++) {
      if (tricks >= goal) {
         *cur = *passed + 20 * (tricks - 6) + 60 * (tricks - goal) + 50;
      } else {
         *cur = -DoubledScore[2 * (goal - tricks)];
      }
   }
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, passed++) {
      if (tricks >= goal) {
         *cur = *passed + 20 * (tricks - 6) + 160 * (tricks - goal) + 50;
      } else {
         *cur = -DoubledScore[2 * (goal - tricks) + 1];
      }
   }
}

static void XMNTSlam(s64*& cur, s64*& passed, int level, int add)
{
   int goal = 6 + level;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, passed++) {
      if (tricks >= goal) {
         *cur = *passed + 30 * (tricks - 6) + 40 * (tricks - goal) + add;
      } else {
         *cur = -DoubledScore[2 * (goal - tricks)];
      }
   }
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, passed++) {
      if (tricks >= goal) {
         *cur = *passed + 30 * (tricks - 6) + 140 * (tricks - goal) + add;
      } else {
         *cur = -DoubledScore[2 * (goal - tricks) + 1];
      }
   }
}

static void XXMinor(s64*& cur, s64*& doubled, int level)
{
   int goal = 6 + level;
   int gameAdd = (2 == level) ? 200 : (level > 2) ? -300 : -50;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, doubled++) {
      if (tricks >= goal) {
         *cur = *doubled * 2 + gameAdd;
      } else {
         *cur = *doubled * 2;
      }
   }
   gameAdd = (level >= 2) ? 200 : 0;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, doubled++) {
      if (tricks >= goal) {
         *cur = *(cur - SCO_TRICKS) + 200 * (tricks - goal) + gameAdd;
      } else {
         *cur = *doubled * 2;
      }
   }
}

static void XXMNT(s64*& cur, s64*& doubled, int level)
{
   int goal = 6 + level;
   int gameAdd = (1 == level) ? 200 : -300;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, doubled++) {
      if (tricks >= goal) {
         *cur = *doubled * 2 + gameAdd;
      } else {
         *cur = *doubled * 2;
      }
   }
   gameAdd = 200;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, doubled++) {
      if (tricks >= goal) {
         *cur = *(cur - SCO_TRICKS) + 200 * (tricks - goal) + gameAdd;
      } else {
         *cur = *doubled * 2;
      }
   }
}

static void XXMinorSlam(s64*& cur, s64*& doubled, int level)
{
   int goal = 6 + level;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, doubled++) {
      if (tricks >= goal) {
         *cur = *doubled + 40 * (tricks - 6) + 60 * (tricks - goal) + 50;
      } else {
         *cur = *doubled * 2;
      }
   }
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, doubled++) {
      if (tricks >= goal) {
         *cur = *doubled + 40 * (tricks - 6) + 160 * (tricks - goal) + 50;
      } else {
         *cur = *doubled * 2;
      }
   }
}

static void XXMNTSlam(s64*& cur, s64*& doubled, int level, int add)
{
   int goal = 6 + level;
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, doubled++) {
      if (tricks >= goal) {
         *cur = *doubled + 60 * (tricks - 6) + 40 * (tricks - goal) + add;
      } else {
         *cur = *doubled * 2;
      }
   }
   for (int tricks = 0; tricks < SCO_TRICKS; tricks++, cur++, doubled++) {
      if (tricks >= goal) {
         *cur = *doubled + 60 * (tricks - 6) + 140 * (tricks - goal) + add;
      } else {
         *cur = *doubled * 2;
      }
   }
}

void PrepareLinearScores()
{
   // on pass
   s64* cur = gLinearScores;
   MinorPartscore(cur, 1);
   MajorPartscore(cur, 1);
   NTPartscore   (cur, 1);
   MinorPartscore(cur, 2);
   MajorPartscore(cur, 2);
   NTPartscore   (cur, 2);
   MinorPartscore(cur, 3);
   MajorPartscore(cur, 3);
   NTGame        (cur, 3);
   MinorPartscore(cur, 4);
   MajorGame     (cur, 4);
   NTGame        (cur, 4);
   MinorGame     (cur, 5);
   MajorGame     (cur, 5);
   NTGame        (cur, 5);
   MinorSmall    (cur, 6);
   MajorSmall    (cur, 6);
   NTSmall       (cur, 6);
   MinorGrand    (cur, 7);
   MajorGrand    (cur, 7);
   NTGrand       (cur, 7);

   // on DBL
   s64* passed = gLinearScores;
   XMinor     (cur, passed, 1);
   XMajor     (cur, passed, 1);
   XNT        (cur, passed, 1);
   XMinor     (cur, passed, 2);
   XMajor     (cur, passed, 2);
   XNT        (cur, passed, 2);
   XMinor     (cur, passed, 3);
   XMajor     (cur, passed, 3);
   XNTMGame   (cur, passed, 3);
   XMinor     (cur, passed, 4);
   XNTMGame   (cur, passed, 4);
   XNTMGame   (cur, passed, 4);
   XMiGame    (cur, passed, 5);
   XNTMGame   (cur, passed, 5);
   XNTMGame   (cur, passed, 5);
   XMinorSlam (cur, passed, 6);
   XMNTSlam   (cur, passed, 6, 50);
   XMNTSlam   (cur, passed, 6, 60);
   XMinorSlam (cur, passed, 7);
   XMNTSlam   (cur, passed, 7, 50);
   XMNTSlam   (cur, passed, 7, 60);

   // on ReDBL
   s64* doubled = passed;
   XXMinor    (cur, doubled, 1);
   XXMNT      (cur, doubled, 1);
   XXMNT      (cur, doubled, 1);
   XXMinor    (cur, doubled, 2);
   XXMNT      (cur, doubled, 2);
   XXMNT      (cur, doubled, 2);
   XXMinor    (cur, doubled, 3);
   XXMNT      (cur, doubled, 3);
   XXMNT      (cur, doubled, 3);
   XXMinor    (cur, doubled, 4);
   XXMNT      (cur, doubled, 4);
   XXMNT      (cur, doubled, 4);
   XXMinor    (cur, doubled, 5);
   XXMNT      (cur, doubled, 5);
   XXMNT      (cur, doubled, 5);
   XXMinorSlam(cur, doubled, 6);
   XXMNTSlam  (cur, doubled, 6, 50);
   XXMNTSlam  (cur, doubled, 6, 70);
   XXMinorSlam(cur, doubled, 7);
   XXMNTSlam  (cur, doubled, 7, 50);
   XXMNTSlam  (cur, doubled, 7, 70);

   // tail for debug
   gLinearScores[SIZE_LINEAR_SCORES] = 42;
   gLinearScores[SIZE_LINEAR_SCORES + 1] = 0;
   gLinearScores[SIZE_LINEAR_SCORES + 2] = 42;
}

// accept code in format like:
// {VN}[level]{CDHSN}{X R}
// Examples:
// V2S  -- vulnerable, 2 spades
// N1CX -- nv, 1 club doubled
// N3NR -- nv, 3 NT redoubled
const s64* FindLinearScore(const char* code)
{
   if (strlen(code) < 3) {
      return nullptr;
   }

   uint vul = 0;
   switch (code[0]) {
      case 'V': vul++;
      case 'N': break;
      default : return nullptr;
   } 

   uint level = code[1] - '0';
   if (level < 1 || 7 < level) {
      return nullptr;
   }
   level--;

   uint denom = 0;
   switch (code[2]) {
      case 'N': denom++;
      case 'S':
      case 'H': denom++;
      case 'D':
      case 'C': break;
      default : return nullptr;
   }

   uint action = 0;
   switch (code[3]) {
      case 'R': action++;
      case 'X': action++;
      case ' ':
      case 0  : break;
      default : return nullptr;
   }

   uint shift = 
      vul      * SCO_TRICKS                                       // vul/nv scores within 1c
      + denom  * SCO_TRICKS * SCO_VUL                             // all denominations within 1st level
      + level  * SCO_TRICKS * SCO_VUL * SCO_DENOMS                // all levels within one action
      + action * SCO_TRICKS * SCO_VUL * SCO_DENOMS * SCO_LEVELS;  // all actions

   return gLinearScores + shift;
}