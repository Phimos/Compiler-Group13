#include "../run.h"

void kernel_case2(float (&B)[5][5], float (&C)[7], float (&A)[5][5]) {
  for (int i = 0; i < 5; ++i){
    for (int j = 0; j < 5; ++j){
      if (((((i < 5 && j < 5) && i < 5) && j < 5) && (i + j) < 7)) {
        A[i][j] = (B[i][j] + C[(i + j)]);
      }
    }
  }
}

