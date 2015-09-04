#ifndef FIXEDPOINT_H_
#define FIXEDPOINT_H_

#include "util.h"


#ifndef FIXPT_SIZE
  /* Q10.5 */
# define FIXPT_SIZE		24
# define FIXPT_SHIFT		6
#endif

#ifndef FIXPTFLOAT_SIZE
# define FIXPTFLOAT_SIZE	32
#endif


#if FIXPT_SIZE == 8
typedef int8_t fixpt_t;
typedef int16_t fixpt_big_t;
#elif FIXPT_SIZE == 16
typedef int16_t fixpt_t;
typedef int24_t fixpt_big_t;
#elif FIXPT_SIZE == 24
typedef int24_t fixpt_t;
typedef int32_t fixpt_big_t;
#elif FIXPT_SIZE == 32
typedef int32_t fixpt_t;
typedef int64_t fixpt_big_t;
#else
# error "Invalid FIXPT_SIZE"
#endif

#define FIXPT_MAX	((fixpt_t)((1ULL << (FIXPT_SIZE - 1U)) - 1U))
#define FIXPT_MIN	((fixpt_t)(-FIXPT_MAX))


#if FIXPTFLOAT_SIZE == 32
typedef float fixptfloat_t;
#elif FIXPTFLOAT_SIZE == 64
typedef double fixptfloat_t;
#else
# error "Invalid FIXPTFLOAT_SIZE"
#endif


static inline fixpt_big_t fixpt_inflate(fixpt_t a)
{
	return (fixpt_big_t)a;
}

static inline fixpt_t fixpt_deflate(fixpt_big_t a)
{
	if (a >= FIXPT_MAX)
		return FIXPT_MAX;
	if (a <= FIXPT_MIN)
		return FIXPT_MIN;

	return (fixpt_t)a;
}

static inline fixpt_big_t int_to_fixpt_big(int32_t i)
{
	return (fixpt_big_t)(i * (1L << FIXPT_SHIFT));
}

static inline fixpt_t int_to_fixpt(int32_t i)
{
	return fixpt_deflate(int_to_fixpt_big(i));
}

static inline int32_t fixpt_big_to_int(fixpt_big_t p)
{
#if 1
//FIXME we should be able to optimize that div by doing shift. Compiler won't do.
	if (p < 0)
		return (int32_t)((p - (1L << (FIXPT_SHIFT - 1))) / (1L << FIXPT_SHIFT));
	else
		return (int32_t)((p + (1L << (FIXPT_SHIFT - 1))) / (1L << FIXPT_SHIFT));
#else
	if (p < 0)
		return (int32_t)((p - (1L << (FIXPT_SHIFT - 1))) >> FIXPT_SHIFT);
	else
		return (int32_t)((p + (1L << (FIXPT_SHIFT - 1))) >> FIXPT_SHIFT);
#endif
}

static inline int32_t fixpt_to_int(fixpt_t p)
{
	return fixpt_big_to_int(fixpt_inflate(p));
}

#define FLOAT_TO_FIXPT_BIG(f)	((fixpt_big_t)(						\
		((f) < 0.0) ?								\
			(fixpt_big_t)(((f) * (fixptfloat_t)(1L << FIXPT_SHIFT)) - 0.5)	\
		:									\
			(fixpt_big_t)(((f) * (fixptfloat_t)(1L << FIXPT_SHIFT)) + 0.5)	\
	))

#define FLOAT_TO_FIXPT(f)	((fixpt_t)FLOAT_TO_FIXPT_BIG(f))

static inline fixpt_big_t float_to_fixpt_big(fixptfloat_t f)
{
	return (fixpt_big_t)FLOAT_TO_FIXPT_BIG(f);
}

static inline fixpt_t float_to_fixpt(fixptfloat_t f)
{
	return fixpt_deflate(float_to_fixpt_big(f));
}

static inline fixptfloat_t fixpt_big_to_float(fixpt_big_t p)
{
	return (fixptfloat_t)p / (fixptfloat_t)(1L << FIXPT_SHIFT);
}

static inline fixptfloat_t fixpt_to_float(fixpt_t p)
{
	return fixpt_big_to_float(fixpt_inflate(p));
}

/* Calculate: a + b
 */
static inline fixpt_big_t fixpt_big_add(fixpt_big_t a, fixpt_big_t b)
{
	fixpt_big_t tmp;

	tmp = a + b;

	return tmp;
}

/* Calculate: a - b
 */
static inline fixpt_big_t fixpt_big_sub(fixpt_big_t a, fixpt_big_t b)
{
	fixpt_big_t tmp;

	tmp = a - b;

	return tmp;
}

/* Calculate: a * b
 */
static inline fixpt_big_t fixpt_big_mul(fixpt_big_t a, fixpt_big_t b)
{
	fixpt_big_t tmp;

	/* Multiply */
	tmp = a * b;
	/* Round */
	tmp += (fixpt_big_t)1L << (FIXPT_SHIFT - 1);
	/* Scale */
	tmp >>= FIXPT_SHIFT;

	return tmp;
}

/* Calculate: a / b
 */
static inline fixpt_big_t fixpt_big_div(fixpt_big_t a, fixpt_big_t b)
{
	fixpt_big_t tmp;

	/* Scale */
	tmp = a << FIXPT_SHIFT;
	/* Round */
	if ((tmp >= 0 && b >= 0) || (tmp < 0 && b < 0))
		tmp += b / 2;
	else
		tmp -= b / 2;
	/* Divide */
	tmp /= b;

	return tmp;
}

/* Calculate: (a * b) / c
 */
static inline fixpt_big_t fixpt_big_mul_div(fixpt_big_t a, fixpt_big_t b, fixpt_big_t c)
{
	fixpt_big_t tmp;

	tmp = fixpt_big_mul(a, b);
	tmp = fixpt_big_div(tmp, c);

	return tmp;
}

/* Calculate: a + b
 */
static inline fixpt_t fixpt_add(fixpt_t a, fixpt_t b)
{
	fixpt_big_t tmp;

	tmp = fixpt_big_add(fixpt_inflate(a), fixpt_inflate(b));

	return fixpt_deflate(tmp);
}

/* Calculate: a - b
 */
static inline fixpt_t fixpt_sub(fixpt_t a, fixpt_t b)
{
	fixpt_big_t tmp;

	tmp = fixpt_big_sub(fixpt_inflate(a), fixpt_inflate(b));

	return fixpt_deflate(tmp);
}

/* Calculate: a * b
 */
static inline fixpt_t fixpt_mul(fixpt_t a, fixpt_t b)
{
	fixpt_big_t tmp;

	tmp = fixpt_big_mul(fixpt_inflate(a), fixpt_inflate(b));

	return fixpt_deflate(tmp);
}

/* Calculate: a / b
 */
static inline fixpt_t fixpt_div(fixpt_t a, fixpt_t b)
{
	fixpt_big_t tmp;

	tmp = fixpt_big_div(fixpt_inflate(a), fixpt_inflate(b));

	return fixpt_deflate(tmp);
}

/* Calculate: (a * b) / c
 */
static inline fixpt_t fixpt_mul_div(fixpt_t a, fixpt_t b, fixpt_t c)
{
	fixpt_big_t tmp;

	tmp = fixpt_big_mul_div(fixpt_inflate(a),
				fixpt_inflate(b),
				fixpt_inflate(c));

	return fixpt_deflate(tmp);
}

/* Calculate: -a
 */
static inline fixpt_t fixpt_neg(fixpt_t a)
{
	if (a <= FIXPT_MIN)
		return FIXPT_MAX;

	return (fixpt_t)-a;
}

#endif /* FIXEDPOINT_H_ */
