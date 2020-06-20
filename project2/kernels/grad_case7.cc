#include "../run2.h"

void grad_case7(float (&dB)[16][32], float (&dA)[32][16]) {
  for (int ii = 0; ii < 32; ++ii){
    for (int jj = 0; jj < 16; ++jj){
      dA[ii][jj] = 0;
    }
  }
  for (int i = 0; i < 16; ++i){
    for (int j = 0; j < 32; ++j){
      if ((i < 16 && j < 32)) {
        dA[j][i] = (dA[j][i] + dB[i][j]);
      }
    }
  }
}

