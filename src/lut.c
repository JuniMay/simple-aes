#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

/// Addition(and substraction) under GF[2^8]
uint8_t add(uint8_t a, uint8_t b) {
  return a ^ b;
}

/// Multiplication under GF[2^8]
/// 
/// Reference: https://en.wikipedia.org/wiki/Finite_field_arithmetic
uint8_t mul(uint8_t a, uint8_t b) {
  uint8_t p = 0;
  uint8_t c;
  for (int i = 0; i < 8; i++) {
    // if b has a constant, add a to p
    // not using if statement to against timing attack
    p ^= (-(b & 1)) & a;
    // if a has a x^7, a multiplication causes carry.
    c = (a & 0x80) >> 7;
    // modulo (sub) x^8 + x^4 + x^3 + x + 1, 0b100011011 = 0x11b
    a <<= 1;
    a ^= (-(c & 1)) & 0x11b;
    b >>= 1;
  }
  return p;
}


/// Inverse under GF[2^8]
uint8_t inv(uint8_t a) {
  uint8_t res = 0;

  // brute-force search
  for (size_t i = 0; i <= 0xff; i++) {
    if (mul(a, (uint8_t)i) == 1) {
      res = (uint8_t)i;
    }
  }

  return res;
}

uint8_t sub_byte(uint8_t a) {
  uint8_t b = inv(a);

  uint8_t affine_mat[8] = {
    0x1f, 0x3e, 0x7c, 0xf8, 0xf1, 0xe3, 0xc7, 0x8f
  };
  uint8_t affine_vec = 0x63;

  uint8_t res = 0;

  for (size_t i = 0; i < 8; i++) {
    res ^= (-((b >> i) & 1)) & affine_mat[i];
  }

  res ^= affine_vec;

  return res;
}

/// S-box lookup table.
uint8_t s_lut[256];
/// MixColumns lookup table.
uint8_t mc_lut[2][256];
/// Combined S-box and MixColumns lookup table.
uint8_t lut[3][256];

int main(int argc, char* argv[]) {
  
  for (size_t i = 0; i <= 0xff; i++) {
    s_lut[i] = sub_byte((uint8_t)i);
  }

  for (size_t i = 0; i <= 0xff; i++) {
    mc_lut[0][i] = mul(0x02, (uint8_t)i);
    mc_lut[1][i] = mul(0x03, (uint8_t)i);
  }

  for (size_t i = 0; i <= 0xff; i++) {
    lut[0][i] = s_lut[i];
    lut[1][i] = mc_lut[0][s_lut[i]];
    lut[2][i] = mc_lut[1][s_lut[i]];
  }

  // print and debug
  printf("S-LUT:\n");
  for (size_t i = 0; i <= 0xff; i++) {
    printf("0x%02x, ", s_lut[i]);
    if (i % 16 == 15) {
      printf("\n");
    }
  }

  printf("MC-LUT (mul 0x02):\n");
  for (size_t i = 0; i <= 0xff; i++) {
    printf("0x%02x, ", mc_lut[0][i]);
    if (i % 16 == 15) {
      printf("\n");
    }
  }

  printf("MC-LUT (mul 0x03):\n");
  for (size_t i = 0; i <= 0xff; i++) {
    printf("0x%02x, ", mc_lut[1][i]);
    if (i % 16 == 15) {
      printf("\n");
    }
  }

  printf("LUT:\n");
  printf("mul 0x01:\n");
  for (size_t i = 0; i <= 0xff; i++) {
    printf("0x%02x, ", lut[0][i]);
    if (i % 16 == 15) {
      printf("\n");
    }
  }

  printf("mul 0x02:\n");
  for (size_t i = 0; i <= 0xff; i++) {
    printf("0x%02x, ", lut[1][i]);
    if (i % 16 == 15) {
      printf("\n");
    }
  }

  printf("mul 0x03:\n");
  for (size_t i = 0; i <= 0xff; i++) {
    printf("0x%02x, ", lut[2][i]);
    if (i % 16 == 15) {
      printf("\n");
    }
  }

  return 0;
}