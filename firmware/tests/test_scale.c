#include "test.h"
#include "scale.c"

#include "util.c"
#include "fixpt.c"


static void test_scale(void)
{
	fixpt_t ret;

	ret = scale(5, -10, 10,
		    float_to_fixpt(-10.0f), float_to_fixpt(10.0f));
	CHECK(ret == float_to_fixpt(5.0f));

	ret = scale(5, 10, -10,
		    float_to_fixpt(10.0f), float_to_fixpt(-10.0f));
	CHECK(ret == float_to_fixpt(5.0f));

	ret = scale(5, -10, 10,
		    float_to_fixpt(-1.0f), float_to_fixpt(1.0f));
	CHECK(ret == float_to_fixpt(0.5f));

	ret = scale(5, 10, -10,
		    float_to_fixpt(1.0f), float_to_fixpt(-1.0f));
	CHECK(ret == float_to_fixpt(0.5f));

	ret = scale(-1, 10, -10,
		    float_to_fixpt(1.0f), float_to_fixpt(-1.0f));
	CHECK(ret == float_to_fixpt(-0.1f));

	ret = scale(1, 10, -10,
		    float_to_fixpt(1.0f), float_to_fixpt(-1.0f));
	CHECK(ret == float_to_fixpt(0.1f));

	ret = scale(1000, 0, 2000,
		    float_to_fixpt(0.0f), float_to_fixpt(10.0f));
	CHECK(ret == float_to_fixpt(5.0f));

	ret = scale(1000, 0, 2000,
		    float_to_fixpt(0.0f), float_to_fixpt(500.0f));
	CHECK(ret == float_to_fixpt(250.0f));

	ret = scale(500, 0, 2000,
		    float_to_fixpt(0.0f), float_to_fixpt(500.0f));
	CHECK(ret == float_to_fixpt(125.0f));

	ret = scale(1000, 0, 2000,
		    float_to_fixpt(500.0f), float_to_fixpt(0.0f));
	CHECK(ret == float_to_fixpt(250.0f));

	ret = scale(500, 0, 2000,
		    float_to_fixpt(500.0f), float_to_fixpt(0.0f));
	CHECK(ret == float_to_fixpt(375.0f));

	ret = scale(128, -10000, 10000,
		    float_to_fixpt(-10.0f), float_to_fixpt(10.0f));
	CHECK(ret == float_to_fixpt(0.128f));

	ret = scale(-128, -10000, 10000,
		    float_to_fixpt(-10.0f), float_to_fixpt(10.0f));
	CHECK(ret == float_to_fixpt(-0.128f));

	ret = scale(0, 0, 0x3FF,
		    float_to_fixpt(0.0f), float_to_fixpt(480.0f));
	CHECK(ret == float_to_fixpt(0.0f));

	ret = scale(0x3FF, 0, 0x3FF,
		    float_to_fixpt(0.0f), float_to_fixpt(480.0f));
	CHECK(ret == float_to_fixpt(480.0f));

	ret = scale(0x100, 0, 0x3FF,
		    float_to_fixpt(0.0f), float_to_fixpt(480.0f));
	CHECK(ret >= float_to_fixpt(120.1f) &&
	      ret <= float_to_fixpt(120.2f));

	ret = scale(0x200, 0, 0x3FF,
		    float_to_fixpt(0.0f), float_to_fixpt(480.0f));
	CHECK(ret >= float_to_fixpt(240.2f) &&
	      ret <= float_to_fixpt(240.3f));

	ret = scale(0x300, 0, 0x3FF,
		    float_to_fixpt(0.0f), float_to_fixpt(480.0f));
	CHECK(ret >= float_to_fixpt(360.3f) &&
	      ret <= float_to_fixpt(360.4f));

	ret = scale(0, 0, 0x3FF,
		    float_to_fixpt(0.0f), float_to_fixpt(5.0f));
	CHECK(ret == float_to_fixpt(0.0f));

	ret = scale(0x3FF, 0, 0x3FF,
		    float_to_fixpt(0.0f), float_to_fixpt(5.0f));
	CHECK(ret == float_to_fixpt(5.0f));

	ret = scale(0x100, 0, 0x3FF,
		    float_to_fixpt(0.0f), float_to_fixpt(5.0f));
	CHECK(ret >= float_to_fixpt(1.245f) &&
	      ret <= float_to_fixpt(1.255f));

	ret = scale(0x200, 0, 0x3FF,
		    float_to_fixpt(0.0f), float_to_fixpt(5.0f));
	CHECK(ret >= float_to_fixpt(2.495f) &&
	      ret <= float_to_fixpt(2.505f));

	ret = scale(0x300, 0, 0x3FF,
		    float_to_fixpt(0.0f), float_to_fixpt(5.0f));
	CHECK(ret >= float_to_fixpt(3.745f) &&
	      ret <= float_to_fixpt(3.755f));
}

static void test_unscale(void)
{
	int16_t ret;

	ret = unscale(float_to_fixpt(5.0f), float_to_fixpt(-10.0f), float_to_fixpt(10.0f),
		      -10, 10);
	CHECK(ret == 5);

	ret = unscale(float_to_fixpt(5.0f), float_to_fixpt(10.0f), float_to_fixpt(-10.0f),
		      10, -10);
	CHECK(ret == 5);

	ret = unscale(float_to_fixpt(250.0f), float_to_fixpt(0.0f), float_to_fixpt(500.0f),
		      0, 10);
	CHECK(ret == 5);

	ret = unscale(float_to_fixpt(100.0f), float_to_fixpt(0.0f), float_to_fixpt(200.0f),
		      0, 500);
	CHECK(ret == 250);

	ret = unscale(float_to_fixpt(50.0f), float_to_fixpt(0.0f), float_to_fixpt(200.0f),
		      0, 500);
	CHECK(ret == 125);

	ret = unscale(float_to_fixpt(100.0f), float_to_fixpt(0.0f), float_to_fixpt(200.0f),
		      500, 0);
	CHECK(ret == 250);

	ret = unscale(float_to_fixpt(50.0f), float_to_fixpt(0.0f), float_to_fixpt(200.0f),
		      500, 0);
	CHECK(ret == 375);

	ret = unscale(float_to_fixpt(12.8f), float_to_fixpt(-100.0f), float_to_fixpt(100.0f),
		      -2000, 2000);
	CHECK(ret == 256);

	ret = unscale(float_to_fixpt(-12.8f), float_to_fixpt(-100.0f), float_to_fixpt(100.0f),
		      -2000, 2000);
	CHECK(ret == -256);

	ret = unscale(float_to_fixpt(0.0f), float_to_fixpt(0.0f), float_to_fixpt(480.0f),
		      0, 0x3FF);
	CHECK(ret == 0);

	ret = unscale(float_to_fixpt(480.0f), float_to_fixpt(0.0f), float_to_fixpt(480.0f),
		      0, 0x3FF);
	CHECK(ret == 0x3FF);

	ret = unscale(float_to_fixpt(120.15f), float_to_fixpt(0.0f), float_to_fixpt(480.0f),
		      0, 0x3FF);
	CHECK(ret == 0x100);

	ret = unscale(float_to_fixpt(240.25f), float_to_fixpt(0.0f), float_to_fixpt(480.0f),
		      0, 0x3FF);
	CHECK(ret == 0x200);

	ret = unscale(float_to_fixpt(360.35f), float_to_fixpt(0.0f), float_to_fixpt(480.0f),
		      0, 0x3FF);
	CHECK(ret == 0x300);

	ret = unscale(float_to_fixpt(0.0f), float_to_fixpt(0.0f), float_to_fixpt(5.0f),
		      0, 0x3FF);
	CHECK(ret == 0);

	ret = unscale(float_to_fixpt(5.0f), float_to_fixpt(0.0f), float_to_fixpt(5.0f),
		      0, 0x3FF);
	CHECK(ret == 0x3FF);

	ret = unscale(float_to_fixpt(1.25f), float_to_fixpt(0.0f), float_to_fixpt(5.0f),
		      0, 0x3FF);
	CHECK(ret == 0x100);

	ret = unscale(float_to_fixpt(2.5f), float_to_fixpt(0.0f), float_to_fixpt(5.0f),
		      0, 0x3FF);
	CHECK(ret == 0x200);

/* FIXME
	ret = unscale(float_to_fixpt(3.75f), float_to_fixpt(0.0f), float_to_fixpt(5.0f),
		      0, 0x3FF);
	CHECK(ret == 0x300);
*/

	ret = unscale(float_to_fixpt(5.0f), float_to_fixpt(0.0f), float_to_fixpt(5.0f),
		      0, 0x1FFF);
	CHECK(ret == 0x1FFF);

	ret = unscale(float_to_fixpt(2.5f), float_to_fixpt(0.0f), float_to_fixpt(5.0f),
		      0, 0x1FFF);
	CHECK(ret == 0x1000);

	ret = unscale(float_to_fixpt(0.0f), float_to_fixpt(0.0f), float_to_fixpt(5.0f),
		      0, 0x1FFF);
	CHECK(ret == 0x0);

	ret = unscale(float_to_fixpt(5.0f), float_to_fixpt(0.0f), float_to_fixpt(5.0f),
		      0x1FFF, 0);
	CHECK(ret == 0x0);

	ret = unscale(float_to_fixpt(2.5f), float_to_fixpt(0.0f), float_to_fixpt(5.0f),
		      0x1FFF, 0);
	CHECK(ret == 0x1000);

	ret = unscale(float_to_fixpt(0.0f), float_to_fixpt(0.0f), float_to_fixpt(5.0f),
		      0x1FFF, 0);
	CHECK(ret == 0x1FFF);
}

static void test_scale_unscale(void)
{
	fixpt_t fix;
	int16_t raw;

	fix = scale(100, -1000, 1000,
		    float_to_fixpt(-10.0f), float_to_fixpt(10.0f));
	raw = unscale(fix, float_to_fixpt(-10.0f), float_to_fixpt(10.0f),
		      -1000, 1000);
	CHECK(raw == 100);

	fix = scale(-100, -1000, 1000,
		    float_to_fixpt(-10.0f), float_to_fixpt(10.0f));
	raw = unscale(fix, float_to_fixpt(-10.0f), float_to_fixpt(10.0f),
		      -1000, 1000);
	CHECK(raw == -100);

	raw = unscale(float_to_fixpt(0.1f), float_to_fixpt(-10.0f), float_to_fixpt(10.0f),
		      -1000, 1000);
	fix = scale(raw, -1000, 1000,
		    float_to_fixpt(-10.0f), float_to_fixpt(10.0f));
	CHECK(fix == float_to_fixpt(0.1f));

	raw = unscale(float_to_fixpt(-0.1f), float_to_fixpt(-10.0f), float_to_fixpt(10.0f),
		      -1000, 1000);
	fix = scale(raw, -1000, 1000,
		    float_to_fixpt(-10.0f), float_to_fixpt(10.0f));
	CHECK(fix == float_to_fixpt(-0.1f));
}

static void test_rescale(void)
{
	fixpt_t ret;

	ret = rescale(float_to_fixpt(5.0f), float_to_fixpt(-10.0f), float_to_fixpt(10.0f),
		      float_to_fixpt(-10.0f), float_to_fixpt(10.0f));
	CHECK(ret == float_to_fixpt(5.0f));

	ret = rescale(float_to_fixpt(5.0f), float_to_fixpt(10.0f), float_to_fixpt(-10.0f),
		      float_to_fixpt(10.0f), float_to_fixpt(-10.0f));
	CHECK(ret == float_to_fixpt(5.0f));

	ret = rescale(float_to_fixpt(5.0f), float_to_fixpt(-10.0f), float_to_fixpt(10.0f),
		      float_to_fixpt(-1.0f), float_to_fixpt(1.0f));
	CHECK(ret == float_to_fixpt(0.5f));

	ret = rescale(float_to_fixpt(5.0f), float_to_fixpt(10.0f), float_to_fixpt(-10.0f),
		      float_to_fixpt(1.0f), float_to_fixpt(-1.0f));
	CHECK(ret == float_to_fixpt(0.5f));

	ret = rescale(float_to_fixpt(-1.0f), float_to_fixpt(10.0f), float_to_fixpt(-10.0f),
		      float_to_fixpt(1.0f), float_to_fixpt(-1.0f));
	CHECK(ret == float_to_fixpt(-0.1f));

	ret = rescale(float_to_fixpt(1.0f), float_to_fixpt(10.0f), float_to_fixpt(-10.0f),
		      float_to_fixpt(1.0f), float_to_fixpt(-1.0f));
	CHECK(ret == float_to_fixpt(0.1f));

	ret = rescale(float_to_fixpt(100.0f), float_to_fixpt(0.0f), float_to_fixpt(200.0f),
		      float_to_fixpt(0.0f), float_to_fixpt(10.0f));
	CHECK(ret == float_to_fixpt(5.0f));

	ret = rescale(float_to_fixpt(100.0f), float_to_fixpt(0.0f), float_to_fixpt(200.0f),
		      float_to_fixpt(0.0f), float_to_fixpt(500.0f));
	CHECK(ret == float_to_fixpt(250.0f));

	ret = rescale(float_to_fixpt(50.0f), float_to_fixpt(0.0f), float_to_fixpt(200.0f),
		      float_to_fixpt(0.0f), float_to_fixpt(500.0f));
	CHECK(ret == float_to_fixpt(125.0f));

	ret = rescale(float_to_fixpt(100.0f), float_to_fixpt(0.0f), float_to_fixpt(200.0f),
		      float_to_fixpt(500.0f), float_to_fixpt(0.0f));
	CHECK(ret == float_to_fixpt(250.0f));

	ret = rescale(float_to_fixpt(50.0f), float_to_fixpt(0.0f), float_to_fixpt(200.0f),
		      float_to_fixpt(500.0f), float_to_fixpt(0.0f));
	CHECK(ret == float_to_fixpt(375.0f));

	ret = rescale(float_to_fixpt(12.8f), float_to_fixpt(-100.0f), float_to_fixpt(100.0f),
		      float_to_fixpt(-10.0f), float_to_fixpt(10.0f));
	CHECK(ret == float_to_fixpt(1.28f));

	ret = rescale(float_to_fixpt(-12.8f), float_to_fixpt(-100.0f), float_to_fixpt(100.0f),
		      float_to_fixpt(-10.0f), float_to_fixpt(10.0f));
	CHECK(ret == float_to_fixpt(-1.28f));

	ret = rescale(int_to_fixpt(0), int_to_fixpt(0), int_to_fixpt(0x3FF),
		      float_to_fixpt(0.0f), float_to_fixpt(480.0f));
	CHECK(ret == float_to_fixpt(0.0f));

	ret = rescale(int_to_fixpt(0x3FF), int_to_fixpt(0), int_to_fixpt(0x3FF),
		      float_to_fixpt(0.0f), float_to_fixpt(480.0f));
	CHECK(ret == float_to_fixpt(480.0f));

	ret = rescale(int_to_fixpt(0x100), int_to_fixpt(0), int_to_fixpt(0x3FF),
		      float_to_fixpt(0.0f), float_to_fixpt(480.0f));
	CHECK(ret >= float_to_fixpt(120.1f) &&
	      ret <= float_to_fixpt(120.2f));

	ret = rescale(int_to_fixpt(0x200), int_to_fixpt(0), int_to_fixpt(0x3FF),
		      float_to_fixpt(0.0f), float_to_fixpt(480.0f));
	CHECK(ret >= float_to_fixpt(240.2f) &&
	      ret <= float_to_fixpt(240.3f));

	ret = rescale(int_to_fixpt(0x300), int_to_fixpt(0), int_to_fixpt(0x3FF),
		      float_to_fixpt(0.0f), float_to_fixpt(480.0f));
	CHECK(ret >= float_to_fixpt(360.3f) &&
	      ret <= float_to_fixpt(360.4f));

	ret = rescale(int_to_fixpt(0), int_to_fixpt(0), int_to_fixpt(0x3FF),
		      float_to_fixpt(0.0f), float_to_fixpt(5.0f));
	CHECK(ret == float_to_fixpt(0.0f));

	ret = rescale(int_to_fixpt(0x3FF), int_to_fixpt(0), int_to_fixpt(0x3FF),
		      float_to_fixpt(0.0f), float_to_fixpt(5.0f));
	CHECK(ret == float_to_fixpt(5.0f));

	ret = rescale(int_to_fixpt(0x100), int_to_fixpt(0), int_to_fixpt(0x3FF),
		      float_to_fixpt(0.0f), float_to_fixpt(5.0f));
	CHECK(ret >= float_to_fixpt(1.245f) &&
	      ret <= float_to_fixpt(1.255f));

	ret = rescale(int_to_fixpt(0x200), int_to_fixpt(0), int_to_fixpt(0x3FF),
		      float_to_fixpt(0.0f), float_to_fixpt(5.0f));
	CHECK(ret >= float_to_fixpt(2.495f) &&
	      ret <= float_to_fixpt(2.505f));

	ret = rescale(int_to_fixpt(0x300), int_to_fixpt(0), int_to_fixpt(0x3FF),
		      float_to_fixpt(0.0f), float_to_fixpt(5.0f));
	CHECK(ret >= float_to_fixpt(3.745f) &&
	      ret <= float_to_fixpt(3.755f));
}

int main(void)
{
	test_scale();
	test_unscale();
	test_scale_unscale();
	test_rescale();

	return 0;
}
