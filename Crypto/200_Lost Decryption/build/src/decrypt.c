#include <stdint.h>
#include <string.h>

const int R = 14;
#define k1 10449751217102183504UL

union data128 {
  uint64_t a[2];
  unsigned char d[16];
};
unsigned char sb(unsigned char b) {
  unsigned char r = b;
  if(b >= 128) {
    r = r * 101;
    r ^= sb(b - 128);
    return r;
  }else if(r < 16) {
    if(r == 0) return 10;
    if(r == 1) return 9;
    if(r == 2) return 3;
    if(r == 4) return 12;
    if(r == 8) return 14;
    if(r == 3) return 15;
    if(r == 6) return 13;
    if(r == 9) return 11;
    if(r == 12) return 6;
    if(r == 5) return 8;
    if(r == 10) return 7;
    if(r == 15) return 5;
    if(r == 7) return 1;
    if(r == 14) return 4;
    if(r == 11) return 2;
    return 0;
  }else {
    return sb(r - 16) + sb(r - 8) + r;
  }
}
uint64_t box(uint64_t x, uint64_t k) {

  for(int i = 0; i < 5; i++) {
    x ^= x * 2305843009213693967UL;
    x += k;
    x ^= x << 3;
    k = k * 18056251572273141347UL + 3402824464142017303UL;
    x ^= x + 7984905489176406801UL;
    x ^= k;
    x ^= x >> 17 & 13225883552121561891UL;
    x -= k;
    for(int j = 0; j < 8; j++) {
       x ^= k >> j;
       k ^= x << j;
    }
    uint64_t tk = k;
    while(tk & 1 == 0 && tk > 0) {
      tk >>= 1;
    }
    
    if(tk > 0) x *= tk;
    for(int j = 0; j < 8; j++) {
      x ^= sb((k >> (j * 8)) & 255) << (j * 8);
    }
  }
  return x;
}

// 16byte data
// 16byte key
void decrypt(unsigned char* data, unsigned char *key) {
  union data128 m;
  union data128 k;
  memcpy(&m, data, 16);
  memcpy(&k, key, 16);

  uint64_t rkey[R];
  for(int r = 0; r < R; r++) {
    rkey[r] = k.a[1];
    k.a[1] = box(k.a[1], k1);
    if(k.a[1] != k.a[0]) {
      k.a[0] ^= k.a[1];
      k.a[1] ^= k.a[0];
      k.a[0] ^= k.a[1];
    }
  }
  for(int r = 0; r < R; r++) {
    m.a[0] ^= box(m.a[1], rkey[R - r - 1]);
    if(m.a[0] != m.a[1]) {
      m.a[0] ^= m.a[1];
      m.a[1] ^= m.a[0];
      m.a[0] ^= m.a[1];
    }
  }
  memcpy(data, &m, 16);
}
