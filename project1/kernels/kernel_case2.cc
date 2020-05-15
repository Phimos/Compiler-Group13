#include "../run.h"

void kernel_case2(float (&A)[32][16]) {
  for (int i = 0; i < 32; ++i){
    for (int j = 0; j < 16; ++j){
      if ((((i < 32 && j < 16) && i < 32) && j < 16)) {
        A[i][j] = (A[i][j] + 1);
      }
    }
  }
}

