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
# define FIXPTBIG_SIZE	16
typedef int8_t fixpt_t;
typedef int16_t fixpt_big_t;
#elif FIXPT_SIZE == 16
# define FIXPTBIG_SIZE	24
typedef int16_t fixpt_t;
typedef int24_t fixpt_big_t;
#elif FIXPT_SIZE == 24
# define FIXPTBIG_SIZE	32
typedef int24_t fixpt_t;
typedef int32_t fixpt_big_t;
#elif FIXPT_SIZE == 32
# define FIXPTBIG_SIZE	64
typedef int32_t fixpt_t;
typedef int64_t fixpt_big_t;
#else
# error "Invalid FIXPT_SIZE"
#endif

#define FIXPT_MAX	((fixpt_t)((1ULL << (FIXPT_SIZE - 1U)) - 1U))
#define FIXPT_MIN	((fixpt_t)(-FIXPT_MAX))
#define FIXPTBIG_MAX	((fixpt_t)((1ULL << (FIXPTBIG_SIZE - 1U)) - 1U))
#define FIXPTBIG_MIN	((fixpt_t)(-FIXPTBIG_MAX))


#if FIXPTFLOAT_SIZE == 32
typedef float fixptfloat_t;
#elif FIXPTFLOAT_SIZE == 64
typedef double fixptfloat_t;
#else
# error "Invalid FIXPTFLOAT_SIZE"
#endif


fixpt_big_t fixpt_inflate(fixpt_t a);

fixpt_t fixpt_deflate(fixpt_big_t a);

#define FLOAT_TO_FIXPT_BIG(f)	((fixpt_big_t)(						\
		((f) < 0.0) ?								\
			(fixpt_big_t)(((f) * (fixptfloat_t)(1L << FIXPT_SHIFT)) - 0.5)	\
		:									\
			(fixpt_big_t)(((f) * (fixptfloat_t)(1L << FIXPT_SHIFT)) + 0.5)	\
	))

#define FLOAT_TO_FIXPT(f)	((fixpt_t)FLOAT_TO_FIXPT_BIG(f))

static alwaysinline fixpt_big_t float_to_fixpt_big(fixptfloat_t f)
{
	return (fixpt_big_t)FLOAT_TO_FIXPT_BIG(f);
}

static alwaysinline fixpt_t float_to_fixpt(fixptfloat_t f)
{
	return fixpt_deflate(float_to_fixpt_big(f));
}

static alwaysinline fixptfloat_t fixpt_big_to_float(fixpt_big_t p)
{
	return (fixptfloat_t)p / (fixptfloat_t)(1L << FIXPT_SHIFT);
}

static alwaysinline fixptfloat_t fixpt_to_float(fixpt_t p)
{
	return fixpt_big_to_float(fixpt_inflate(p));
}

#define INT_TO_FIXPT_BIG(i)	((fixpt_big_t)((int32_t)(i) << FIXPT_SHIFT))
#define INT_TO_FIXPT(i)		((fixpt_t)INT_TO_FIXPT_BIG(i))

fixpt_big_t _do_int32_to_fixpt_big(int32_t i);
#define int_to_fixpt_big(i)	choose_expr(is_constant(i),		\
					    INT_TO_FIXPT_BIG(i),	\
					    _do_int32_to_fixpt_big(i))

fixpt_t _do_int32_to_fixpt(int32_t i);
#define int_to_fixpt(i)		choose_expr(is_constant(i),		\
					    INT_TO_FIXPT(i),		\
					    _do_int32_to_fixpt(i))

int32_t fixpt_big_to_int(fixpt_big_t p);

int32_t fixpt_to_int(fixpt_t p);

/* Get the integer part of a fixpt_t.
 */
int32_t fixpt_get_int_part(fixpt_t p);

/* Get the decimal fractional part of a fixpt_t.
 */
uint32_t fixpt_get_dec_fract(fixpt_t p, uint8_t nr_digits);

/* Calculate: a + b
 */
fixpt_big_t fixpt_big_add(fixpt_big_t a, fixpt_big_t b);

/* Calculate: a - b
 */
fixpt_big_t fixpt_big_sub(fixpt_big_t a, fixpt_big_t b);

/* Calculate: a * b
 */
fixpt_big_t fixpt_big_mul(fixpt_big_t a, fixpt_big_t b);

/* Calculate: a / b
 */
fixpt_big_t fixpt_big_div(fixpt_big_t a, fixpt_big_t b);

/* Calculate: (a * b) / c
 */
fixpt_big_t fixpt_big_mul_div(fixpt_big_t a, fixpt_big_t b, fixpt_big_t c);

/* Calculate: a + b
 */
fixpt_t fixpt_add(fixpt_t a, fixpt_t b);

/* Calculate: a - b
 */
fixpt_t fixpt_sub(fixpt_t a, fixpt_t b);

/* Calculate: a * b
 */
fixpt_t fixpt_mul(fixpt_t a, fixpt_t b);

/* Calculate: a / b
 */
fixpt_t fixpt_div(fixpt_t a, fixpt_t b);

/* Calculate: (a * b) / c
 */
fixpt_t fixpt_mul_div(fixpt_t a, fixpt_t b, fixpt_t c);

/* Calculate: -a
 */
fixpt_t fixpt_neg(fixpt_t a);

/* Calculate: Absolute value of a
 */
fixpt_t fixpt_abs(fixpt_t a);

/* Calculate: a + b, limited to lo_lim, hi_lim
 */
fixpt_t fixpt_add_limited(fixpt_t a, fixpt_t b,
			  fixpt_t lo_lim, fixpt_t hi_lim);

/* Limit a to lo_lim, hi_lim */
fixpt_t fixpt_clamp(fixpt_t a,
		    fixpt_t lo_lim, fixpt_t hi_lim);

#endif /* FIXEDPOINT_H_ */
