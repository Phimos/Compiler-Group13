#include "../run2.h"

void grad_case8(float (&dB)[32], float (&dA)[2][16]) {
  for (int ii = 0; ii < 2; ++ii){
    for (int jj = 0; jj < 16; ++jj){
      dA[ii][jj] = 0;
    }
  }
  for (int h_tmp = 0; h_tmp < 2; ++h_tmp){
    for (int i = 0; i < 32; ++i){
      for (int i_tmp = 0; i_tmp < 16; ++i_tmp){
        if (((((i < 32 && (i / 16) == h_tmp) && h_tmp < 2) && (i % 16) == i_tmp) && i_tmp < 16)) {
          dA[h_tmp][i_tmp] = (dA[h_tmp][i_tmp] + dB[i]);
        }
      }
    }
  }
}

