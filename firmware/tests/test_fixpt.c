#include "test.h"
#include "fixpt.h"
#include "fixpt.c"


static void check_common(void)
{
	/* Check conversion functions */

	CHECK(float_to_fixpt(1.0) == int_to_fixpt(1));
	CHECK(float_to_fixpt(5.0) == int_to_fixpt(5));
	CHECK(float_to_fixpt(10.0) == int_to_fixpt(10));
	CHECK(float_to_fixpt(1023.0) == int_to_fixpt(1023));
	CHECK(float_to_fixpt(-1.0) == int_to_fixpt(-1));
	CHECK(float_to_fixpt(-99.0) == int_to_fixpt(-99));
	CHECK(float_to_fixpt(-1023.0) == int_to_fixpt(-1023));
	CHECK(fixpt_to_int(int_to_fixpt(6)) == 6);
	CHECK(fixpt_to_float(float_to_fixpt(6.0)) == 6.0);
	CHECK(fixpt_to_float(float_to_fixpt(6.5)) == 6.5);
	CHECK(fixpt_to_float(float_to_fixpt(6.5)) != 6.0);
	CHECK(fixpt_to_float(float_to_fixpt(-6.0)) == -6.0);

	/* Check rounding of conversion functions */

	CHECK(fixpt_to_int(float_to_fixpt(5.4)) == 5);
	CHECK(fixpt_to_int(float_to_fixpt(5.5)) == 6);
	CHECK(fixpt_to_int(float_to_fixpt(5.6)) == 6);
	CHECK(fixpt_to_int(float_to_fixpt(-5.4)) == -5);
	CHECK(fixpt_to_int(float_to_fixpt(-5.5)) == -6);
	CHECK(fixpt_to_int(float_to_fixpt(-5.6)) == -6);
	CHECK(fixpt_to_int(float_to_fixpt(0.4)) == 0);
	CHECK(fixpt_to_int(float_to_fixpt(0.5)) == 1);
	CHECK(fixpt_to_int(float_to_fixpt(0.6)) == 1);
	CHECK(fixpt_to_int(float_to_fixpt(-0.4)) == 0);
	CHECK(fixpt_to_int(float_to_fixpt(-0.5)) == -1);
	CHECK(fixpt_to_int(float_to_fixpt(-0.6)) == -1);
}

int main(void)
{
	check_common();

	return 0;
}
