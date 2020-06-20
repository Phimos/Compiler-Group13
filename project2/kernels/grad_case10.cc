#include "../run2.h"

void grad_case10(float (&dA)[8][8], float (&dB)[10][8]) {
  for (int ii = 0; ii < 10; ++ii){
    for (int jj = 0; jj < 8; ++jj){
      dB[ii][jj] = 0;
    }
  }
  for (int i = 0; i < 8; ++i){
    for (int j = 0; j < 8; ++j){
      for (int n_tmp = 0; n_tmp < 10; ++n_tmp){
        for (int o_tmp = 0; o_tmp < 10; ++o_tmp){
          if (((((((i < 8 && j < 8) && i < 10) && (i + 1) == n_tmp) && n_tmp < 10) && (i + 2) == o_tmp) && o_tmp < 10)) {
            dB[i][j] = (dB[i][j] + (dA[i][j] / 3));
          }
        }
      }
    }
  }
  for (int i = 0; i < 8; ++i){
    for (int j = 0; j < 8; ++j){
      for (int n_tmp = 0; n_tmp < 10; ++n_tmp){
        for (int o_tmp = 0; o_tmp < 10; ++o_tmp){
          if (((((((i < 8 && j < 8) && i < 10) && (i + 1) == n_tmp) && n_tmp < 10) && (i + 2) == o_tmp) && o_tmp < 10)) {
            dB[n_tmp][j] = (dB[n_tmp][j] + (dA[i][j] / 3));
          }
        }
      }
    }
  }
  for (int i = 0; i < 8; ++i){
    for (int j = 0; j < 8; ++j){
      for (int n_tmp = 0; n_tmp < 10; ++n_tmp){
        for (int o_tmp = 0; o_tmp < 10; ++o_tmp){
          if (((((((i < 8 && j < 8) && i < 10) && (i + 1) == n_tmp) && n_tmp < 10) && (i + 2) == o_tmp) && o_tmp < 10)) {
            dB[o_tmp][j] = (dB[o_tmp][j] + (dA[i][j] / 3));
          }
        }
      }
    }
  }
}

