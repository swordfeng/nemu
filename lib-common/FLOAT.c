#include "FLOAT.h"

typedef unsigned int uint32_t;
typedef long long int64_t;

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	return ((int64_t)a * b) >> 16;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	int sign = 1;
	if (a < 0) {
		sign *= -1;
		a = -a;
	}
	if (b < 0) {
		sign *= -1;
		b = -b;
	}
	int res = a / b;
	a = a % b;
	for (int i = 0; i < 16; i++) {
		a <<= 1;
		res <<= 1;
		if (a >= b) {
			a -= b;
			res++;
		}
	}
	return res * sign;
}

FLOAT f2F(float a) {
	uint32_t af = *(uint32_t *)&a;
	uint32_t sign = af >> 31;
	int exp = (af >> 23) & 0xff;
	uint32_t sig = af & 0x7fffff;
	if (exp != 0) sig += 1 << 23;
	exp -= 150;
	if (exp < -16) sig >>= -16 - exp;
	if (exp > -16) sig <<= exp + 16;
	return sign == 0 ? sig : -sig;
}

FLOAT Fabs(FLOAT a) {
	return a >= 0 ? a : -a;
}

FLOAT sqrt(FLOAT x) {
	FLOAT dt, t = int2F(2);

	do {
		dt = F_div_int((F_div_F(x, t) - t), 2);
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

FLOAT pow(FLOAT x, FLOAT y) {
	/* we only compute x^0.333 */
	FLOAT t2, dt, t = int2F(2);

	do {
		t2 = F_mul_F(t, t);
		dt = (F_div_F(x, t2) - t) / 3;
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}
