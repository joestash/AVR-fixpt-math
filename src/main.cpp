
#include <Arduboy2.h>
#include <math.h>

#include "fixpt.h"
using namespace fixpt;

Arduboy2 arduboy;

volatile int16_t xi = 0x1234;
volatile int16_t yi = -0x5678;
volatile int16_t zi = 0;

constexpr uint16_t N_BENCH = 65535;
constexpr uint16_t ARG1_OVERHEAD = 12;  // 2*lds, 2*sts, stiw, brne
constexpr uint16_t ARG2_OVERHEAD = 16;  // 4*lds, 2*sts, stiw, brne

#define exp2f(x) (expf((x) * 0.69314718056f))
#define log2f(x) (logf(x) * 1.442695040889f)

void mulhi_test() {

    float minerr = 0, maxerr = 0;
    for (int32_t x = -32768; x <= 32767; x += 257) {
        for (int32_t y = -32768; y <= 32767; y += 257) {

            float ref = (x * y) >> 16;
            float tst = MULHI(x, y);

            float err = tst - ref;
            if (err < minerr) minerr = err;
            if (err > maxerr) maxerr = err;
        }
    }

    uint32_t cycles = micros();
    for (uint16_t i = 0; i < N_BENCH; ++i) {
        zi = MULHI(xi, yi);
    }
    cycles = 16 * (micros() - cycles) / N_BENCH;

    arduboy.print("mulhi   ");
    arduboy.print(minerr, 1); arduboy.print(", ");
    arduboy.print(maxerr, 1); arduboy.print("  ");
    arduboy.println(cycles - ARG2_OVERHEAD);
    arduboy.display();
}

void recip_test() {

    float minerr = 0, maxerr = 0;
    for (uint32_t x = 0; x <= 65535; x++) {

        float ref = (x == 0) ? 65535 : (1ul << 16) / x;   // Q16
        float tst = recip(x);

        float err = tst - ref;
        if (err < minerr) minerr = err;
        if (err > maxerr) maxerr = err;
    }

    uint32_t cycles = micros();
    for (uint16_t i = 0; i < N_BENCH; ++i) {
        zi = recip(xi);
    }
    cycles = 16 * (micros() - cycles) / N_BENCH;

    arduboy.print("recip  ");
    arduboy.print(minerr, 1); arduboy.print(", ");
    arduboy.print(maxerr, 1); arduboy.print("  ");
    arduboy.println(cycles - ARG1_OVERHEAD);
    arduboy.display();
}

void udiv_test() {

    float minerr = 0, maxerr = 0;
    for (uint32_t d = 0; d <= 65535; d++) {
        const uint32_t n = d - 1;

        float ref = (n >= d) ? 65535 : (n << 16) / d; // Q16
        float tst = udivhi(n, d);

        float err = tst - ref;
        if (err < minerr) minerr = err;
        if (err > maxerr) maxerr = err;
    }

    uint32_t cycles = micros();
    for (uint16_t i = 0; i < N_BENCH; ++i) {
        zi = udivhi(xi, yi);
    }
    cycles = 16 * (micros() - cycles) / N_BENCH;

    arduboy.print("udivhi ");
    arduboy.print(minerr, 1); arduboy.print(", ");
    arduboy.print(maxerr, 1); arduboy.print("  ");
    arduboy.println(cycles - ARG2_OVERHEAD);
    arduboy.display();
}

void rsqrt_test() {

    float minerr = 0, maxerr = 0;
    for (uint32_t x = 0; x <= 65535; x++) {

        float ref = (x == 0) ? 65535 : 65536.0f / sqrtf(x); // Q16
        float tst = rsqrt(x);

        float err = tst - ref;
        if (err < minerr) minerr = err;
        if (err > maxerr) maxerr = err;
    }

    uint32_t cycles = micros();
    for (uint16_t i = 0; i < N_BENCH; ++i) {
        zi = rsqrt(xi);
    }
    cycles = 16 * (micros() - cycles) / N_BENCH;

    arduboy.print("rsqrt  ");
    arduboy.print(minerr, 1); arduboy.print(", ");
    arduboy.print(maxerr, 1); arduboy.print("  ");
    arduboy.println(cycles - ARG1_OVERHEAD);
    arduboy.display();
}

void sincos_test() {

    float minerr = 0, maxerr = 0;
    for (int32_t x = -32768; x <= 32767; x++) {

        float ref = sinf(M_PI * x / 32768.0f) * 32768.0f;   // Q15

        int16_t sn, cs;
        sincospi(x, sn, cs);

        float err = sn - ref;
        if (err < minerr) minerr = err;
        if (err > maxerr) maxerr = err;
    }

    uint32_t cycles = micros();
    for (uint16_t i = 0; i < N_BENCH; ++i) {
        sincospi(xi, (int16_t&)yi, (int16_t&)zi);
    }
    cycles = 16 * (micros() - cycles) / N_BENCH;

    arduboy.print("sincos ");
    arduboy.print(minerr, 1); arduboy.print(", ");
    arduboy.print(maxerr, 1); arduboy.print("  ");
    arduboy.println(cycles - ARG1_OVERHEAD);
    arduboy.display();
}

void atan2_test() {

    float minerr = 0, maxerr = 0;
    for (int32_t y = -32768; y <= 32767; y++) {
        const int32_t x = 32767;

        float ref = ((x | y) == 0) ? 0x1fff : atan2f(y / 32768.0f, x / 32768.0f) * 32768.0f / M_PI;    // Q15
        float tst = atan2pi(y, x);

        // unwrap error across branch cut
        float err = tst - ref;
        if (err >= 32768.0f) err -= 65536.0f;
        if (err < -32768.0f) err += 65536.0f;

        if (err < minerr) minerr = err;
        if (err > maxerr) maxerr = err;
    }

    uint32_t cycles = micros();
    for (uint16_t i = 0; i < N_BENCH; ++i) {
        zi = atan2pi(yi, xi);
    }
    cycles = 16 * (micros() - cycles) / N_BENCH;

    arduboy.print("atan2  ");
    arduboy.print(minerr, 1); arduboy.print(", ");
    arduboy.print(maxerr, 1); arduboy.print("  ");
    arduboy.println(cycles - ARG2_OVERHEAD);
    arduboy.display();
}

void log2_test() {

    float minerr = 0, maxerr = 0;
    for (uint32_t x = 0; x <= 65535; x++) {

        float ref = (x == 0) ? -32768.0f : log2f(x / 65536.0f) * 2048.0f; // Q4.11
        float tst = log2(x);

        float err = tst - ref;
        if (err < minerr) minerr = err;
        if (err > maxerr) maxerr = err;
    }

    uint32_t cycles = micros();
    for (uint16_t i = 0; i < N_BENCH; ++i) {
        zi = log2(xi);
    }
    cycles = 16 * (micros() - cycles) / N_BENCH;

    arduboy.print("log2   ");
    arduboy.print(minerr, 1); arduboy.print(", ");
    arduboy.print(maxerr, 1); arduboy.print("  ");
    arduboy.println(cycles - ARG1_OVERHEAD);
    arduboy.display();
}

void exp2_test() {

    float minerr = 0, maxerr = 0;
    for (int32_t x = -32768; x <= 0; x++) {

        float ref = exp2f(x / 2048.0f) * 65536.0f; // Q16
        float tst = exp2(x);

        float err = tst - ref;
        if (err < minerr) minerr = err;
        if (err > maxerr) maxerr = err;
    }

    uint32_t cycles = micros();
    for (uint16_t i = 0; i < N_BENCH; ++i) {
        zi = exp2(yi);
    }
    cycles = 16 * (micros() - cycles) / N_BENCH;

    arduboy.print("exp2   ");
    arduboy.print(minerr, 1); arduboy.print(", ");
    arduboy.print(maxerr, 1); arduboy.print("  ");
    arduboy.println(cycles - ARG1_OVERHEAD);
    arduboy.display();
}

void setup() {
    arduboy.begin();
    arduboy.clear();

    mulhi_test();
    recip_test();
    udiv_test();
    rsqrt_test();
    log2_test();
    exp2_test();
    sincos_test();
    atan2_test();
}

void loop() {}
