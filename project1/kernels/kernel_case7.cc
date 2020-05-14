#include "../run.h"

void kernel_case7(float (&A)[32][16], float (&B)[16][32]) {
  for (int i = 0; i < 16; ++i){
    for (int j = 0; j < 32; ++j){
      if ((((i < 16 && j < 32) && j < 32) && i < 16)) {
        B[i][j] = A[j][i];
      }
    }
  }
}

