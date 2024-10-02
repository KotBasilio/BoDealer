//
// For Masha
//

#define  _CRT_SECURE_NO_WARNINGS
#include "walrus.h"
#include "Oscar.h"
#include HEADER_CURSES

void TrainingTask();
void TrainingTask()
{
   int R = 0, A = 1000;
   printf("We will search a min of a sequence. End with a negative number.\n");

   do {
      printf("Enter next number --->");
      auto howMuch = scanf("%d", &R);

      if (howMuch > 0) {
         if (R >= 0) {
            if (R < A) {
               A = R;
               printf("Accepted: %d. A min has changed\n", R);
            } else {
               printf("Accepted: %d. A min stays\n", R);
            }
         }
      } else {
         char garbage[128];
         scanf("%s", garbage);
         printf("Cannot read '%s'. A min stays\n", garbage);
      }

   } while (R >= 0);

   printf("End of the task. Minimum is  %d\n", A);

}