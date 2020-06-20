#include "../run2.h"

void grad_case4(float (&B)[16][32], float (&C)[32][32], float (&dA)[16][32], float (&dB)[16][32], float (&dC)[32][32]) {
  for (int ii = 0; ii < 16; ++ii){
    for (int jj = 0; jj < 32; ++jj){
      dB[ii][jj] = 0;
    }
  }
  for (int i = 0; i < 16; ++i){
    for (int j = 0; j < 32; ++j){
      for (int k = 0; k < 32; ++k){
        if (((i < 16 && j < 32) && k < 32)) {
          dB[i][k] = (dB[i][k] + (dA[i][j] * C[k][j]));
        }
      }
    }
  }
  for (int ii = 0; ii < 32; ++ii){
    for (int jj = 0; jj < 32; ++jj){
      dC[ii][jj] = 0;
    }
  }
  for (int i = 0; i < 16; ++i){
    for (int j = 0; j < 32; ++j){
      for (int k = 0; k < 32; ++k){
        if (((i < 16 && j < 32) && k < 32)) {
          dC[k][j] = (dC[k][j] + (B[i][k] * dA[i][j]));
        }
      }
    }
  }
}

