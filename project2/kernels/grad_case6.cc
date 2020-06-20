#include "../run2.h"

void grad_case6(float (&C)[8][16][3][3], float (&dA)[2][8][5][5], float (&dB)[2][16][7][7]) {
  for (int ii = 0; ii < 2; ++ii){
    for (int jj = 0; jj < 16; ++jj){
      for (int kk = 0; kk < 7; ++kk){
        for (int ll = 0; ll < 7; ++ll){
          dB[ii][jj][kk][ll] = 0;
        }
      }
    }
  }
  for (int b_tmp = 0; b_tmp < 7; ++b_tmp){
    for (int c = 0; c < 16; ++c){
      for (int c_tmp = 0; c_tmp < 7; ++c_tmp){
        for (int k = 0; k < 8; ++k){
          for (int n = 0; n < 2; ++n){
            for (int p = 0; p < 5; ++p){
              for (int q = 0; q < 5; ++q){
                for (int r = 0; r < 3; ++r){
                  for (int s = 0; s < 3; ++s){
                    if (((((((((((n < 2 && k < 8) && p < 5) && q < 5) && c < 16) && (p + r) == b_tmp) && b_tmp < 7) && (q + s) == c_tmp) && c_tmp < 7) && r < 3) && s < 3)) {
                      dB[n][c][b_tmp][c_tmp] = (dB[n][c][b_tmp][c_tmp] + (dA[n][k][p][q] * C[k][c][r][s]));
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

