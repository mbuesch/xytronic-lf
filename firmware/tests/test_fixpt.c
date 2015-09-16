#include "test.h"
#include "fixpt.h"
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
}

int main(void)
{
	check_common();

	return 0;
}
