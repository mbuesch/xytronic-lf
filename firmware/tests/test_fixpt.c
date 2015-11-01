#include "test.h"
#include "fixpt.h"

#include "util.c"
#include "fixpt.c"


static void check_common(void)
{
	/* Check conversion functions */

	CHECK(float_to_fixpt(1.0f) == int_to_fixpt(1));
	CHECK(float_to_fixpt(5.0f) == int_to_fixpt(5));
	CHECK(float_to_fixpt(10.0f) == int_to_fixpt(10));
	CHECK(float_to_fixpt(1023.0f) == int_to_fixpt(1023));
	CHECK(float_to_fixpt(-1.0f) == int_to_fixpt(-1));
	CHECK(float_to_fixpt(-99.0f) == int_to_fixpt(-99));
	CHECK(float_to_fixpt(-1023.0f) == int_to_fixpt(-1023));
	CHECK(fixpt_to_int(int_to_fixpt(6)) == 6);
	CHECK(fixpt_to_float(float_to_fixpt(6.0f)) == 6.0f);
	CHECK(fixpt_to_float(float_to_fixpt(6.5f)) == 6.5f);
	CHECK(fixpt_to_float(float_to_fixpt(6.5f)) != 6.0f);
	CHECK(fixpt_to_float(float_to_fixpt(-6.0f)) == -6.0f);

	/* Check rounding of conversion functions */

	CHECK(fixpt_to_int(float_to_fixpt(5.4f)) == 5);
	CHECK(fixpt_to_int(float_to_fixpt(5.5f)) == 6);
	CHECK(fixpt_to_int(float_to_fixpt(5.6f)) == 6);
	CHECK(fixpt_to_int(float_to_fixpt(-5.4f)) == -5);
	CHECK(fixpt_to_int(float_to_fixpt(-5.5f)) == -6);
	CHECK(fixpt_to_int(float_to_fixpt(-5.6f)) == -6);
	CHECK(fixpt_to_int(float_to_fixpt(0.4f)) == 0);
	CHECK(fixpt_to_int(float_to_fixpt(0.5f)) == 1);
	CHECK(fixpt_to_int(float_to_fixpt(0.6f)) == 1);
	CHECK(fixpt_to_int(float_to_fixpt(-0.4f)) == 0);
	CHECK(fixpt_to_int(float_to_fixpt(-0.5f)) == -1);
	CHECK(fixpt_to_int(float_to_fixpt(-0.6f)) == -1);

	/* Check decimal functions */

	CHECK(fixpt_get_int_part(float_to_fixpt(0.0f)) == 0);
	CHECK(fixpt_get_int_part(float_to_fixpt(1.0f)) == 1);
	CHECK(fixpt_get_int_part(float_to_fixpt(1.5f)) == 1);
	CHECK(fixpt_get_int_part(float_to_fixpt(2.5f)) == 2);
	CHECK(fixpt_get_int_part(float_to_fixpt(-1.0f)) == -1);
	CHECK(fixpt_get_int_part(float_to_fixpt(-1.5f)) == -1);
	CHECK(fixpt_get_int_part(float_to_fixpt(-2.5f)) == -2);

	CHECK(fixpt_get_dec_fract(float_to_fixpt(1.25f), 2) == 25);
	CHECK(fixpt_get_dec_fract(float_to_fixpt(-1.25f), 2) == 25);
	CHECK(fixpt_get_dec_fract(float_to_fixpt(1.25f), 1) == 3);
	CHECK(fixpt_get_dec_fract(float_to_fixpt(1.225f), 1) == 2);
	CHECK(fixpt_get_dec_fract(float_to_fixpt(-1.25f), 1) == 3);
	CHECK(fixpt_get_dec_fract(float_to_fixpt(-1.225f), 1) == 2);
	CHECK(fixpt_get_dec_fract(float_to_fixpt(1.125f), 1) == 1);
	CHECK(fixpt_get_dec_fract(float_to_fixpt(1.125f), 2) == 13);
	CHECK(fixpt_get_dec_fract(float_to_fixpt(1.125f), 3) == 125);
	CHECK(fixpt_get_dec_fract(float_to_fixpt(-1.125f), 1) == 1);
	CHECK(fixpt_get_dec_fract(float_to_fixpt(-1.125f), 2) == 13);
	CHECK(fixpt_get_dec_fract(float_to_fixpt(-1.125f), 3) == 125);
	CHECK(fixpt_get_dec_fract(float_to_fixpt(1.0f), 2) == 0);
	CHECK(fixpt_get_dec_fract(float_to_fixpt(-1.0f), 2) == 0);
	CHECK(fixpt_get_dec_fract(float_to_fixpt(0.0f), 2) == 0);
}

int main(void)
{
	check_common();

	return 0;
}
