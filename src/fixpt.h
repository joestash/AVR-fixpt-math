// Copyright 2024 Ken Cooke
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdint.h>

#define FORCEINLINE __attribute__((always_inline)) inline

#define MUL32(a, b)   ((int32_t)(int16_t)(a) * (int32_t)(int16_t)(b))
#define UMUL32(a, b)  ((uint32_t)(uint16_t)(a) * (uint32_t)(uint16_t)(b))
#define UDIVHI(n, d)  ((uint16_t)(((uint32_t)((uint16_t)(n)) << 16) / (uint16_t)(d)))

namespace fixpt {

constexpr uint16_t recip_tab[128] PROGMEM = {
    0xffff, 0xfe03, 0xfc0f, 0xfa23, 0xf83e, 0xf660, 0xf489, 0xf2b9,
    0xf0f0, 0xef2e, 0xed73, 0xebbd, 0xea0e, 0xe865, 0xe6c2, 0xe525,
    0xe38e, 0xe1fc, 0xe070, 0xdee9, 0xdd67, 0xdbeb, 0xda74, 0xd901,
    0xd794, 0xd62b, 0xd4c7, 0xd368, 0xd20d, 0xd0b6, 0xcf64, 0xce16,
    0xcccc, 0xcb87, 0xca45, 0xc907, 0xc7ce, 0xc698, 0xc565, 0xc437,
    0xc30c, 0xc1e4, 0xc0c0, 0xbfa0, 0xbe82, 0xbd69, 0xbc52, 0xbb3e,
    0xba2e, 0xb921, 0xb817, 0xb70f, 0xb60b, 0xb509, 0xb40b, 0xb30f,
    0xb216, 0xb11f, 0xb02c, 0xaf3a, 0xae4c, 0xad60, 0xac76, 0xab8f,
    0xaaaa, 0xa9c8, 0xa8e8, 0xa80a, 0xa72f, 0xa655, 0xa57e, 0xa4a9,
    0xa3d7, 0xa306, 0xa237, 0xa16b, 0xa0a0, 0x9fd8, 0x9f11, 0x9e4c,
    0x9d89, 0x9cc8, 0x9c09, 0x9b4c, 0x9a90, 0x99d7, 0x991f, 0x9868,
    0x97b4, 0x9701, 0x964f, 0x95a0, 0x94f2, 0x9445, 0x939a, 0x92f1,
    0x9249, 0x91a2, 0x90fd, 0x905a, 0x8fb8, 0x8f17, 0x8e78, 0x8dda,
    0x8d3d, 0x8ca2, 0x8c08, 0x8b70, 0x8ad8, 0x8a42, 0x89ae, 0x891a,
    0x8888, 0x87f7, 0x8767, 0x86d9, 0x864b, 0x85bf, 0x8534, 0x84a9,
    0x8421, 0x8399, 0x8312, 0x828c, 0x8208, 0x8184, 0x8102, 0x8080,
};

constexpr uint8_t rsqrt_tab[96] PROGMEM = {
    0xfe, 0xfa, 0xf7, 0xf3, 0xf0, 0xec, 0xe9, 0xe6, 0xe4, 0xe1, 0xde, 0xdc,
    0xd9, 0xd7, 0xd4, 0xd2, 0xd0, 0xce, 0xcc, 0xca, 0xc8, 0xc6, 0xc4, 0xc2,
    0xc1, 0xbf, 0xbd, 0xbc, 0xba, 0xb9, 0xb7, 0xb6, 0xb4, 0xb3, 0xb2, 0xb0,
    0xaf, 0xae, 0xac, 0xab, 0xaa, 0xa9, 0xa8, 0xa7, 0xa6, 0xa5, 0xa3, 0xa2,
    0xa1, 0xa0, 0x9f, 0x9e, 0x9e, 0x9d, 0x9c, 0x9b, 0x9a, 0x99, 0x98, 0x97,
    0x97, 0x96, 0x95, 0x94, 0x93, 0x93, 0x92, 0x91, 0x90, 0x90, 0x8f, 0x8e,
    0x8e, 0x8d, 0x8c, 0x8c, 0x8b, 0x8a, 0x8a, 0x89, 0x89, 0x88, 0x87, 0x87,
    0x86, 0x86, 0x85, 0x84, 0x84, 0x83, 0x83, 0x82, 0x82, 0x81, 0x81, 0x80,
};

#ifdef __AVR__
FORCEINLINE int16_t MULHI(int16_t a, int16_t b) {
    int16_t r;
    asm volatile(
        "clr    r26         \n\t"

        "mul    %A1, %A2    \n\t"
        "mov    r27, r1     \n\t"

        "muls   %B1, %B2    \n\t"
        "movw   %A0, r0     \n\t"

        "mulsu  %B2, %A1    \n\t"
        "sbc    %B0, r26    \n\t"
        "add    r27, r0     \n\t"
        "adc    %A0, r1     \n\t"
        "adc    %B0, r26    \n\t"

        "mulsu  %B1, %A2    \n\t"
        "sbc    %B0, r26    \n\t"
        "add    r27, r0     \n\t"
        "adc    %A0, r1     \n\t"
        "adc    %B0, r26    \n\t"

        "clr    r1          \n\t"

        : "=&r" (r)
        : "a" (a),
          "a" (b)
        : "r26", "r27"
    );
    return r;
}
#else
#define MULHI(a, b) (int16_t)(((int32_t)(int16_t)(a) * (int32_t)(int16_t)(b)) >> 16)
#endif

#ifdef __AVR__
FORCEINLINE uint16_t UMULHI(uint16_t a, uint16_t b) {
    uint16_t r;
    asm volatile(
        "clr    r26         \n\t"

        "mul    %A1, %A2    \n\t"
        "mov    r27, r1     \n\t"

        "mul    %B1, %B2    \n\t"
        "movw   %A0, r0     \n\t"

        "mul    %B2, %A1    \n\t"
        "add    r27, r0     \n\t"
        "adc    %A0, r1     \n\t"
        "adc    %B0, r26    \n\t"

        "mul    %B1, %A2    \n\t"
        "add    r27, r0     \n\t"
        "adc    %A0, r1     \n\t"
        "adc    %B0, r26    \n\t"

        "clr    r1          \n\t"

        : "=&r" (r)
        : "a" (a),
          "a" (b)
        : "r26", "r27"
    );
    return r;
}
#else
#define UMULHI(a, b) (uint16_t)(((uint32_t)(uint16_t)(a) * (uint32_t)(uint16_t)(b)) >> 16)
#endif

// returns e = CLZ(x) and x <<= e
FORCEINLINE uint8_t norm(uint16_t& x) {
    uint8_t e = 0;
    if (x < 0x0100) { e += 8; x <<= 8; }
    if (x < 0x1000) { e += 4; x <<= 4; }
    if (x < 0x4000) { e += 2; x <<= 2; }
    if (x < 0x8000) { e += 1; x <<= 1; }
    return e;
}

// for x!=0 returns 1/x in Q16
// for x==0 returns 0xffff
//
// |error| <= 1.0 ULP
FORCEINLINE uint16_t recip(uint16_t x) {

    if (x == 0) return 0xffff;

    // normalize
    uint8_t e = norm(x);

    // r = 0x7fffff / x, where x = 8 MSBs
    uint8_t idx = (x >> 8) - 128;
    uint16_t r = pgm_read_word(&recip_tab[idx]);

    // undo normalize
    return r >> (15 - e);
}

// Computes 1/sqrt(x) for x=[0,0xffff] in Q0
// x > 0 returns 1/sqrt(x) in Q16
// x = 0 returns 0xffff
//
// |error| <= 3.0 ULP
FORCEINLINE uint16_t rsqrt(uint16_t x) {

    if (x <= 1) return 0xffff;

    // normalize
    uint8_t e = norm(x);
    if (e & 0x1) x >>= 1;
    e &= 0xfe;  // e=[0,14], even

    // initial estimate
    uint8_t idx = (x >> 9) - 32;
    uint8_t t = pgm_read_byte(&rsqrt_tab[idx]); // Q8

    // Newton-Raphson iteration
    uint16_t s = t * t;
    uint16_t r = 0xc000 - UMULHI(x, s); // Q16
    r = UMUL32(r, t << 1) >> 8;

    // undo normalize
    e = 7 - (e >> 1);
    return r >> e;  // Q16
}

// Computes unsigned 16-bit fixed-point division
// n < d returns (n<<16)/d in Q16
// n >= d returns 0xffff (overflow)
// d = 0 returns 0xffff (divide-by-zero)
//
// |error| <= [-8.0,0] ULP
FORCEINLINE uint16_t udivhi(uint16_t n, uint16_t d) {

    if (n >= d) return 0xffff;  // overflow or divide-by-zero

    // normalize
    uint8_t e = norm(d);
    n <<= e;

    // reciprocal estimate
    uint8_t idx = (d >> 8) - 128;
    uint16_t r = pgm_read_word(&recip_tab[idx]);

    // Newton-Raphson iteration
    d = ~UMULHI(d, r);
    d = UMULHI(d, r) << 1;

    // multiply with n, to produce quotient
    uint16_t q = UMULHI(d, n);
    return q << 1;  // Q16
}

// Computes log2(x) for x=[0,1] in Q16
// x > 0 returns log2(x) as [-16,0] in Q11
// x = 0 returns -16.0 in Q11
//
// |error| < 2.5 ULP
FORCEINLINE int16_t log2(uint16_t x) {

    if (x == 0) return 0x8000;

    // split into e and x - 1.0
    uint8_t e = norm(x);
    x &= 0x7fff;                // Q15

    // log2(1+x) polynomial approx for x=[0,1]
    int16_t f = 10435;          // Q16
    f = MULHI(f, x) - 19074;    // Q15
    f = MULHI(f, x) + 23312;    // Q14
    f = MULHI(f, x);            // Q13

    // reconstruct from integer and fractional parts
    f = (~e << 11) + ((f + 2) >> 2);
    return f;
}

// Computes exp2(x) for x=[-16,0] in Q11
// x <= 0 returns exp2(x) as [0,1] in Q16
// x > 0 returns 1.0 in Q16
//
// |error| < 5.9 ULP
FORCEINLINE uint16_t exp2(int16_t x) {

    if (x >= 0) return 0xffff;

    // split into e and x
    uint8_t e = ~(x >> 11);
    x = (x << 4) & 0x7fff;      // Q15

    // exp2(x) polynomial approx for x=[0,1]
    uint16_t f = 20774;         // Q18
    f = UMULHI(f, x) + 29474;   // Q17
    f = UMULHI(f, x) + 45606;   // Q16
    f = UMULHI(f, x) + 32768;   // Q15

    // reconstruct from integer and fractional parts
    return f >> e;
}

// Computes sin(pi*x) and cos(pi*x) for signed x=[-1,1] in Q15
//                                 or unsigned x=[ 0,2] in Q15
// |error| < 2.3 ULP
FORCEINLINE void sincospi(int16_t x, int16_t& s, int16_t& c) {

    // reduce to quadrant, and x in [-0.25,0.25]
    uint16_t q = (x + 0x2000) & 0xc000;     // quadrant [0,3] in 2 MSBs
    x <<= 2;                                // normalize to Q15
    if (x == -32768) x++;                   // saturate to avoid x*x overflow

    int16_t xx = MULHI(x, x) << 1;  // Q15

    // sin approx, as odd polynomial
    s = 322;                    // Q17
    s = MULHI(s, xx) - 5291;    // Q16
    s = MULHI(s, xx) + 25736;   // Q15
    s = MULHI(s, x);            // Q14
    s = (s << 1) + 1;           // Q15

    // cos approx, as even polynomial
    c = 2039;                   // Q17
    c = MULHI(c, xx) - 20209;   // Q16
    c = MULHI(c, xx) + 32767;   // Q15

    // reconstruct using quadrant
    if (q & 0x8000) {
        s = -s;
        c = -c;
    }
    if (q & 0x4000) {
        int16_t t = s;
        s = c;
        c = -t;
    }
}

// Computes atan2(y,x)/pi, with result in Q15
// atan2pi(0,0) returns 0.25 (pi/4) in Q15
//
// |error| < 3.75 ULP
FORCEINLINE int16_t atan2pi(int16_t y, int16_t x) {

    // reduce to first octant
    uint16_t ay = y < 0 ? -(uint16_t)y : y;
    uint16_t ax = x < 0 ? -(uint16_t)x : x;
    int16_t r = udivhi(min(ay, ax), max(ay, ax)) >> 1;  // Q15

    // atan(r) polynomial approx
    int16_t s = 22915;          // Q19
    s = MULHI(s, r) - 28217;    // Q18
    s = MULHI(s, r) - 696;      // Q17
    s = MULHI(s, r) + 20922;    // Q16
    s = MULHI(s, r);            // Q15

    // reconstruct using octant
    if (ay > ax) s = 0x4000 - s;
    if (x < 0) s = 0x8000 - s;
    if (y < 0) s = -s;
    return s;
}

} // namespace fixpt
