/************************************************************
 * Walrus project -- tasks definitions only        29.11.2020
 * --- TASKS, pick only one define of the list below
 * Only micro-filters versions -- 3.0 and later
 * See previous tasks in archive 2.0 branch
 ************************************************************/

// Bidding decision one-sided:
//#define SEMANTIC_JULY_MOROZOV_HAND_EVAL
//#define SEMANTIC_OCT_SEEK_6D
//#define SEMANTIC_BASHA_NOV_NT
//#define SEMANTIC_TANJA_FEB_LONG_H
//#define SEMANTIC_54M_FEB_PARTIZAN
//#define SEMANTIC_4M_ON54_FEB_PATTON
//#define SEMANTIC_7D_OR_NT_FEB
//#define SEMANTIC_1H_1S_2H_MAR
//#define SEMANTIC_1C_P_1H_2D_2H_MAR
//#define SEM_MAR24_5C_OR_3NT
//#define SEM_MAY24_TENS_AND_NINES
//#define SEM_MAY24_GLADIATOR
#define SEM_MAY24_TRHEE_SUITED_BALANCE

// Bidding decision competitive:
//#define SEMANTIC_JUN_MINORS_VS_MAJORS
//#define SEMANTIC_OCT_INDIAN_5C
//#define SEMANTIC_OVERCALL_4C
//#define SEMANTIC_INV_AFTER_2H
//#define SEM_DU_MAR_1C_X_XX_1H_ETC_MAR
//#define SEM_MIX_MAR24_INVITE_4S

// Opening lead:

// One hand tasks:
//#define SEMANTIC_KEYCARDS_10_12
//#define SEMANTIC_SPADE_4_WHEN_1H

// 4 hands tasks:
//#define SEMANTIC_SPLINTER_SHAPE
//#define SEMANTIC_STANDARD_3NT
//#define SEMANTIC_STANDARD_6NT
//#define SEMANTIC_CONFIG_BASED
//#define SEMANTIC_1NT_OR_SIGN_OFF
//#define SEMANTIC_4M_ON54_BIG
//#define SEMANTIC_ALL4_1H_1S_2H_MAR
//#define SEM_ALL4_1C_P_1H_2D_2H_MAR

// -----------------------------------------------------------------
// task descriptions
#include "waTOneSided.h"
#include "waTCompetitive.h"
#include "waTLead.h"
#include "waTFour.h"

// other parameters for solving
#include "waDerived.h"
//#define DBG_VIEW_ON_ADD
