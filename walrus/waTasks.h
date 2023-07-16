/************************************************************
 * Walrus project -- tasks definitions only        29.11.2020
 * --- TASKS, pick only one define of the list below
 ************************************************************/

// Bidding decision one-sided:
//#define SEMANTIC_FEB_3NT_REBID
//#define SEMANTIC_JAN_INVITE_MINORS
//#define SEMANTIC_NOV_INVITE_PRECISION
//#define SEMANTIC_APR_64_INVITE
//#define SEMANTIC_AUG_3NT_ON_SPADE_FIT
//#define SEMANTIC_AUG_3NT2_ON_SPADE_FIT
//#define SEMANTIC_AUG_3NT_ON_44H
//#define SEMANTIC_AUG_MULTI_VUL
//#define SEMANTIC_DEC_12_2425
//#define SEMANTIC_SEPT_INVITE_OR_FG
//#define SEMANTIC_IMPS_ACCEPT_3NT_ON_SPADE_FIT
//#define SEMANTIC_JAN_NT_SLAM_ON_DIAMONDS
//#define SEMANTIC_JAN_SPADES_GAME
//#define SEMANTIC_MORO_SLAM
//#define SEMANTIC_NOV_64_AS_TWO_SUITER
//#define SEMANTIC_NOV_VOIDWOOD
//#define SEMANTIC_RED55_KINGS_PART_15_16
//#define SEMANTIC_SEPT_MAJORS54_18HCP
//#define SEMANTIC_SEPT_MANTICORA_14_16

// Bidding decision competitive:
#define SEMANTIC_JUN_MINORS_VS_MAJORS
//#define SEMANTIC_OCT_WEAK_GAMBLING
//#define SEMANTIC_AUG_SPLIT_FIT
//#define SEMANTIC_DEC_ACCEPT_TO_4S
//#define SEMANTIC_DEC_BID_5H_OR_DBL_4S
//#define SEMANTIC_DEC_JAN_DBL_THEN_HEARTS
//#define SEMANTIC_DEC_JUMP_TO_4S
//#define SEMANTIC_FEB_4711_DILEMMA_ON_4S
//#define SEMANTIC_JULY_AUTO_FITO_PLANKTON
//#define SEMANTIC_JUNE_GAMBLE_OR_5C
//#define SEMANTIC_MIXED_2D_WITH_MAJOR
//#define SEMANTIC_MIXED_PREVENTIVE_4S
//#define SEMANTIC_NOV_BID_6C_OR_DBL_4S
//#define SEMANTIC_NOV_DBL_ON_3NT
//#define SEMANTIC_DEC21_DBL_ON_3NT
//#define SEMANTIC_TRICOLOR_STRONG

// Opening lead:
//#define SEMANTIC_JAN_PETYA_VS_3NT
//#define SEMANTIC_AUG_LEAD_VS_3H
//#define SEMANTIC_IMPS_LEAD_LEVKOVICH
//#define SEMANTIC_JUNE_LEAD_3343
//#define SEMANTIC_JUNE_MAX_5D_LEAD
//#define SEMANTIC_JUNE_ZAKHAROVY_PREC_3NT
//#define SEMANTIC_MIXED_PREVENTIVE_4S

// One hand tasks:
//#define SEMANTIC_KEYCARDS_10_12
//#define SEMANTIC_SPADE_4_WHEN_1H

// 4 hands tasks:
//#define SEMANTIC_SPLINTER_SHAPE
//#define SEMANTIC_STANDARD_3NT
//#define SEMANTIC_STANDARD_6NT
//#define SEMANTIC_CONFIG_BASED

// -----------------------------------------------------------------
// task descriptions
#include "waTOneSided.h"
#include "waTCompetitive.h"
#include "waTLead.h"

// other parameters for solving
#include "waDerived.h"
