#include "../run2.h"

void grad_case9(float (&dB)[4][6], float (&dA)[4]) {
  for (int ii = 0; ii < 4; ++ii){
    dA[ii] = 0;
  }
  for (int i = 0; i < 4; ++i){
    for (int j = 0; j < 6; ++j){
      if ((i < 4 && j < 6)) {
        dA[i] = (dA[i] + dB[i][j]);
      }
    }
  }
}

