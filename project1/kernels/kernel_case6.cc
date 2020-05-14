#include "../run.h"

void kernel_case6(float (&B)[2][16][7][7], float (&C)[8][16][3][3], float (&A)[2][8][5][5]) {
  for (int c = 0; c < 16; ++c){
    for (int k = 0; k < 8; ++k){
      for (int n = 0; n < 2; ++n){
        for (int p = 0; p < 5; ++p){
          for (int q = 0; q < 5; ++q){
            for (int r = 0; r < 3; ++r){
              for (int s = 0; s < 3; ++s){
                if ((((((((((((((((n < 2 && k < 8) && p < 5) && q < 5) && n < 2) && k < 8) && p < 5) && q < 5) && n < 2) && c < 16) && (p + r) < 7) && (q + s) < 7) && k < 8) && c < 16) && r < 3) && s < 3)) {
                  A[n][k][p][q] = (A[n][k][p][q] + (B[n][c][(p + r)][(q + s)] * C[k][c][r][s]));
                }
              }
            }
          }
        }
      }
    }
  }
}

