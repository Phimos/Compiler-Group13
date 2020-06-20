#include "../run2.h"

void grad_case5(float (&C)[32][32], float (&D)[4][32], float (&dA)[16][32], float (&dB)[16][32][4]) {
  for (int ii = 0; ii < 16; ++ii){
    for (int jj = 0; jj < 32; ++jj){
      for (int kk = 0; kk < 4; ++kk){
        dB[ii][jj][kk] = 0;
      }
    }
  }
  for (int i = 0; i < 16; ++i){
    for (int j = 0; j < 32; ++j){
      for (int k = 0; k < 32; ++k){
        for (int l = 0; l < 4; ++l){
          if ((((i < 16 && j < 32) && k < 32) && l < 4)) {
            dB[i][k][l] = (dB[i][k][l] + ((dA[i][j] * C[k][j]) * D[l][j]));
          }
        }
      }
    }
  }
}

