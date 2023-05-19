#ifndef FILTER_DEF_H_
#define FILTER_DEF_H_

const int MASK_N = 9;
const int MASK_X = 3;
const int MASK_Y = 3;

// Filter inner transport addresses
// Used between blocking_transport() & do_filter()
const int FILTER_R_ADDR = 0x00000000;
const int FILTER_RESULT_ADDR = 0x00000004;
const int FILTER_CHECK_ADDR = 0x00000008;

union word {
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};

const float mean_mask[MASK_Y][MASK_X] = {{0.1, 0.1, 0.1}, {0.1, 0.2, 0.1}, {0.1, 0.1, 0.1}};

#define CLOCK_PERIOD 10

#endif
