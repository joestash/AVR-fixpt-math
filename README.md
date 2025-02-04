# AVR-fixpt-math
A small/fast fixed-point math library optimized for 8-bit AVR/Arduboy.
The header-only library is in [fixpt.h](src/fixpt.h).
The Arduboy test/benchmark app is in [main.cpp](src/main.cpp).

All math functions have been exhaustively tested over their argument domains.

For sin/cos/atan2, angles represent multiples of pi, rather than radians.
This allows phase wrapping for free and special values to be exact.

#
Min/max error (ULP) and typical performance (clock cycles) measured on Atmega32u4.
NOTE: due to the lack of barrel shifter, performance will vary with magnitude of inputs.

![](images/benchmark.png)

#
Error plots (ULP) for each function:

![](images/recip.png)
![](images/udivhi.png)
![](images/rsqrt.png)
![](images/log2.png)
![](images/exp2.png)
![](images/sincos.png)
![](images/atan2.png)
