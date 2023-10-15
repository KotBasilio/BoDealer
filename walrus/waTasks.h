/************************************************************
 * Walrus project -- tasks definitions only        29.11.2020
 * --- TASKS, pick only one define of the list below
 * Only micro-filters versions -- 3.0 and later
 * See previous tasks in archive 2.0 branch
 ************************************************************/

// Bidding decision one-sided:
//#define SEMANTIC_JULY_MOROZOV_HAND_EVAL
#define SEMANTIC_OCT_SEEK_6D

// Bidding decision competitive:
//#define SEMANTIC_JUN_MINORS_VS_MAJORS

// Opening lead:

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
