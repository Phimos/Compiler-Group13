#include "../run2.h"

void grad_case2(float (&A)[4][16], float (&dB)[4][16], float (&dA)[4][16]) {
  for (int ii = 0; ii < 4; ++ii){
    for (int jj = 0; jj < 16; ++jj){
      dA[ii][jj] = 0;
    }
  }
  for (int i = 0; i < 4; ++i){
    for (int j = 0; j < 16; ++j){
      if ((i < 4 && j < 16)) {
        dA[i][j] = (dA[i][j] + (dB[i][j] * A[i][j]));
      }
    }
  }
  for (int i = 0; i < 4; ++i){
    for (int j = 0; j < 16; ++j){
      if ((i < 4 && j < 16)) {
        dA[i][j] = (dA[i][j] + (A[i][j] * dB[i][j]));
      }
    }
  }
}

