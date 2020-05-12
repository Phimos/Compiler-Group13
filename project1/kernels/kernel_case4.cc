#include "../run.h"

void kernel_case4(float (&B)[16][32], float (&C)[32][32], float (&A)[16][32]) {
  for (int i = 0; i < 16; ++i){
    for (int j = 0; j < 32; ++j){
      for (int k = 0; k < 32; ++k){
        A[i][j] = A[i][j] + B[i][k] * C[k][j];
      }
    }
  }
}

