#include "../run2.h"

void grad_case3(float (&B)[16][16], float (&dC)[4][16], float (&dA)[4][16]) {
  for (int ii = 0; ii < 4; ++ii){
    for (int jj = 0; jj < 16; ++jj){
      dA[ii][jj] = 0;
    }
  }
  for (int i = 0; i < 4; ++i){
    for (int j = 0; j < 16; ++j){
      for (int k = 0; k < 16; ++k){
        if (((i < 4 && j < 16) && k < 16)) {
          dA[i][k] = (dA[i][k] + (dC[i][j] * B[k][j]));
        }
      }
    }
  }
}

