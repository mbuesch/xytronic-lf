#include "test.h"
#include "scale.c"


static void test_scale(void)
{
	fixpt_t ret;

	ret = scale(5, -10, 10,
		    float_to_fixpt(-10.0), float_to_fixpt(10.0));
	CHECK(ret == float_to_fixpt(5.0));

	ret = scale(5, 10, -10,
		    float_to_fixpt(10.0), float_to_fixpt(-10.0));
	CHECK(ret == float_to_fixpt(5.0));

	ret = scale(5, -10, 10,
		    float_to_fixpt(-1.0), float_to_fixpt(1.0));
	CHECK(ret == float_to_fixpt(0.5));

	ret = scale(5, 10, -10,
		    float_to_fixpt(1.0), float_to_fixpt(-1.0));
	CHECK(ret == float_to_fixpt(0.5));

	ret = scale(-1, 10, -10,
		    float_to_fixpt(1.0), float_to_fixpt(-1.0));
	CHECK(ret == float_to_fixpt(-0.1));

	ret = scale(1, 10, -10,
		    float_to_fixpt(1.0), float_to_fixpt(-1.0));
	CHECK(ret == float_to_fixpt(0.1));

	ret = scale(1000, 0, 2000,
		    float_to_fixpt(0.0), float_to_fixpt(10.0));
	CHECK(ret == float_to_fixpt(5.0));

	ret = scale(1000, 0, 2000,
		    float_to_fixpt(0.0), float_to_fixpt(500.0));
	CHECK(ret == float_to_fixpt(250.0));

	ret = scale(500, 0, 2000,
		    float_to_fixpt(0.0), float_to_fixpt(500.0));
	CHECK(ret == float_to_fixpt(125.0));

	ret = scale(1000, 0, 2000,
		    float_to_fixpt(500.0), float_to_fixpt(0.0));
	CHECK(ret == float_to_fixpt(250.0));

	ret = scale(500, 0, 2000,
		    float_to_fixpt(500.0), float_to_fixpt(0.0));
	CHECK(ret == float_to_fixpt(375.0));

	ret = scale(128, -10000, 10000,
		    float_to_fixpt(-10.0), float_to_fixpt(10.0));
	CHECK(ret == float_to_fixpt(0.128));

	ret = scale(-128, -10000, 10000,
		    float_to_fixpt(-10.0), float_to_fixpt(10.0));
	CHECK(ret == float_to_fixpt(-0.128));

	ret = scale(0, 0, 0x3FF,
		    float_to_fixpt(0.0), float_to_fixpt(480.0));
	CHECK(ret == float_to_fixpt(0.0));

	ret = scale(0x3FF, 0, 0x3FF,
		    float_to_fixpt(0.0), float_to_fixpt(480.0));
	CHECK(ret == float_to_fixpt(480.0));

	ret = scale(0x100, 0, 0x3FF,
		    float_to_fixpt(0.0), float_to_fixpt(480.0));
	CHECK(ret >= float_to_fixpt(120.1) &&
	      ret <= float_to_fixpt(120.2));

	ret = scale(0x200, 0, 0x3FF,
		    float_to_fixpt(0.0), float_to_fixpt(480.0));
	CHECK(ret >= float_to_fixpt(240.2) &&
	      ret <= float_to_fixpt(240.3));

	ret = scale(0x300, 0, 0x3FF,
		    float_to_fixpt(0.0), float_to_fixpt(480.0));
	CHECK(ret >= float_to_fixpt(360.3) &&
	      ret <= float_to_fixpt(360.4));

	ret = scale(0, 0, 0x3FF,
		    float_to_fixpt(0.0), float_to_fixpt(5.0));
	CHECK(ret == float_to_fixpt(0.0));

	ret = scale(0x3FF, 0, 0x3FF,
		    float_to_fixpt(0.0), float_to_fixpt(5.0));
	CHECK(ret == float_to_fixpt(5.0));

	ret = scale(0x100, 0, 0x3FF,
		    float_to_fixpt(0.0), float_to_fixpt(5.0));
	CHECK(ret >= float_to_fixpt(1.245) &&
	      ret <= float_to_fixpt(1.255));

	ret = scale(0x200, 0, 0x3FF,
		    float_to_fixpt(0.0), float_to_fixpt(5.0));
	CHECK(ret >= float_to_fixpt(2.495) &&
	      ret <= float_to_fixpt(2.505));

	ret = scale(0x300, 0, 0x3FF,
		    float_to_fixpt(0.0), float_to_fixpt(5.0));
	CHECK(ret >= float_to_fixpt(3.745) &&
	      ret <= float_to_fixpt(3.755));
}

static void test_unscale(void)
{
	int16_t ret;

	ret = unscale(float_to_fixpt(5.0), float_to_fixpt(-10.0), float_to_fixpt(10.0),
		      -10, 10);
	CHECK(ret == 5);

	ret = unscale(float_to_fixpt(5.0), float_to_fixpt(10.0), float_to_fixpt(-10.0),
		      10, -10);
	CHECK(ret == 5);

	ret = unscale(float_to_fixpt(250.0), float_to_fixpt(0.0), float_to_fixpt(500.0),
		      0, 10);
	CHECK(ret == 5);

	ret = unscale(float_to_fixpt(100.0), float_to_fixpt(0.0), float_to_fixpt(200.0),
		      0, 500);
	CHECK(ret == 250);

	ret = unscale(float_to_fixpt(50.0), float_to_fixpt(0.0), float_to_fixpt(200.0),
		      0, 500);
	CHECK(ret == 125);

	ret = unscale(float_to_fixpt(100.0), float_to_fixpt(0.0), float_to_fixpt(200.0),
		      500, 0);
	CHECK(ret == 250);

	ret = unscale(float_to_fixpt(50.0), float_to_fixpt(0.0), float_to_fixpt(200.0),
		      500, 0);
	CHECK(ret == 375);

	ret = unscale(float_to_fixpt(12.8), float_to_fixpt(-100.0), float_to_fixpt(100.0),
		      -2000, 2000);
	CHECK(ret == 256);

	ret = unscale(float_to_fixpt(-12.8), float_to_fixpt(-100.0), float_to_fixpt(100.0),
		      -2000, 2000);
	CHECK(ret == -256);

	ret = unscale(float_to_fixpt(0.0), float_to_fixpt(0.0), float_to_fixpt(480.0),
		      0, 0x3FF);
	CHECK(ret == 0);

	ret = unscale(float_to_fixpt(480.0), float_to_fixpt(0.0), float_to_fixpt(480.0),
		      0, 0x3FF);
	CHECK(ret == 0x3FF);

	ret = unscale(float_to_fixpt(120.15), float_to_fixpt(0.0), float_to_fixpt(480.0),
		      0, 0x3FF);
	CHECK(ret == 0x100);

	ret = unscale(float_to_fixpt(240.25), float_to_fixpt(0.0), float_to_fixpt(480.0),
		      0, 0x3FF);
	CHECK(ret == 0x200);

	ret = unscale(float_to_fixpt(360.35), float_to_fixpt(0.0), float_to_fixpt(480.0),
		      0, 0x3FF);
	CHECK(ret == 0x300);

	ret = unscale(float_to_fixpt(0.0), float_to_fixpt(0.0), float_to_fixpt(5.0),
		      0, 0x3FF);
	CHECK(ret == 0);

	ret = unscale(float_to_fixpt(5.0), float_to_fixpt(0.0), float_to_fixpt(5.0),
		      0, 0x3FF);
	CHECK(ret == 0x3FF);

	ret = unscale(float_to_fixpt(1.25), float_to_fixpt(0.0), float_to_fixpt(5.0),
		      0, 0x3FF);
	CHECK(ret == 0x100);

	ret = unscale(float_to_fixpt(2.5), float_to_fixpt(0.0), float_to_fixpt(5.0),
		      0, 0x3FF);
	CHECK(ret == 0x200);

/* FIXME
	ret = unscale(float_to_fixpt(3.75), float_to_fixpt(0.0), float_to_fixpt(5.0),
		      0, 0x3FF);
	CHECK(ret == 0x300);
*/
}

static void test_scale_unscale(void)
{
	fixpt_t fix;
	int16_t raw;

	fix = scale(100, -1000, 1000,
		    float_to_fixpt(-10.0), float_to_fixpt(10.0));
	raw = unscale(fix, float_to_fixpt(-10.0), float_to_fixpt(10.0),
		      -1000, 1000);
	CHECK(raw == 100);

	fix = scale(-100, -1000, 1000,
		    float_to_fixpt(-10.0), float_to_fixpt(10.0));
	raw = unscale(fix, float_to_fixpt(-10.0), float_to_fixpt(10.0),
		      -1000, 1000);
	CHECK(raw == -100);

	raw = unscale(float_to_fixpt(0.1), float_to_fixpt(-10.0), float_to_fixpt(10.0),
		      -1000, 1000);
	fix = scale(raw, -1000, 1000,
		    float_to_fixpt(-10.0), float_to_fixpt(10.0));
	CHECK(fix == float_to_fixpt(0.1));

	raw = unscale(float_to_fixpt(-0.1), float_to_fixpt(-10.0), float_to_fixpt(10.0),
		      -1000, 1000);
	fix = scale(raw, -1000, 1000,
		    float_to_fixpt(-10.0), float_to_fixpt(10.0));
	CHECK(fix == float_to_fixpt(-0.1));
}

int main(void)
{
	test_scale();
	test_unscale();
	test_scale_unscale();

	return 0;
}
