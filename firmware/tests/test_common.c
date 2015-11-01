#include "test.h"
#include "util.h"

#include "util.c"


static void check_util(void)
{
	int ret;


	/* Check: sdiv_round_up() */

	ret = sdiv_round_up(10, 1);
	CHECK(ret == 10);
	ret = sdiv_round_up(10, 5);
	CHECK(ret == 2);
	ret = sdiv_round_up(10, 6);
	CHECK(ret == 2);
	ret = sdiv_round_up(10, 9);
	CHECK(ret == 2);
	ret = sdiv_round_up(10, 10);
	CHECK(ret == 1);
	ret = sdiv_round_up(-10, 1);
	CHECK(ret == -10);
	ret = sdiv_round_up(-10, 5);
	CHECK(ret == -2);
	ret = sdiv_round_up(-10, 6);
	CHECK(ret == -1);
	ret = sdiv_round_up(-10, 9);
	CHECK(ret == -1);
	ret = sdiv_round_up(-10, 10);
	CHECK(ret == -1);
	ret = sdiv_round_up(10, -1);
	CHECK(ret == -10);
	ret = sdiv_round_up(10, -5);
	CHECK(ret == -2);
	ret = sdiv_round_up(10, -6);
	CHECK(ret == -1);
	ret = sdiv_round_up(10, -9);
	CHECK(ret == -1);
	ret = sdiv_round_up(10, -10);
	CHECK(ret == -1);
	ret = sdiv_round_up(-10, -1);
	CHECK(ret == 10);
	ret = sdiv_round_up(-10, -5);
	CHECK(ret == 2);
	ret = sdiv_round_up(-10, -6);
	CHECK(ret == 2);
	ret = sdiv_round_up(-10, -9);
	CHECK(ret == 2);
	ret = sdiv_round_up(-10, -10);
	CHECK(ret == 1);


	/* Check: udiv_round_up() */

	ret = udiv_round_up(10, 1);
	CHECK(ret == 10);
	ret = udiv_round_up(10, 5);
	CHECK(ret == 2);
	ret = udiv_round_up(10, 6);
	CHECK(ret == 2);
	ret = udiv_round_up(10, 9);
	CHECK(ret == 2);
	ret = udiv_round_up(10, 10);
	CHECK(ret == 1);


	/* Check: sdiv_round() */

	ret = sdiv_round(10, 1);
	CHECK(ret == 10);
	ret = sdiv_round(10, 5);
	CHECK(ret == 2);
	ret = sdiv_round(10, 6);
	CHECK(ret == 2);
	ret = sdiv_round(10, 9);
	CHECK(ret == 1);
	ret = sdiv_round(10, 10);
	CHECK(ret == 1);
	ret = sdiv_round(15, 6);
	CHECK(ret == 3);
	ret = sdiv_round(-10, 1);
	CHECK(ret == -10);
	ret = sdiv_round(-10, 5);
	CHECK(ret == -2);
	ret = sdiv_round(-10, 6);
	CHECK(ret == -2);
	ret = sdiv_round(-10, 9);
	CHECK(ret == -1);
	ret = sdiv_round(-10, 10);
	CHECK(ret == -1);
	ret = sdiv_round(-15, 6);
	CHECK(ret == -3);
	ret = sdiv_round(10, -1);
	CHECK(ret == -10);
	ret = sdiv_round(10, -5);
	CHECK(ret == -2);
	ret = sdiv_round(10, -6);
	CHECK(ret == -2);
	ret = sdiv_round(10, -9);
	CHECK(ret == -1);
	ret = sdiv_round(10, -10);
	CHECK(ret == -1);
	ret = sdiv_round(15, -6);
	CHECK(ret == -3);
	ret = sdiv_round(-10, -1);
	CHECK(ret == 10);
	ret = sdiv_round(-10, -5);
	CHECK(ret == 2);
	ret = sdiv_round(-10, -6);
	CHECK(ret == 2);
	ret = sdiv_round(-10, -9);
	CHECK(ret == 1);
	ret = sdiv_round(-10, -10);
	CHECK(ret == 1);
	ret = sdiv_round(-15, -6);
	CHECK(ret == 3);


	/* Check: udiv_round() */

	ret = udiv_round(10, 1);
	CHECK(ret == 10);
	ret = udiv_round(10, 5);
	CHECK(ret == 2);
	ret = udiv_round(10, 6);
	CHECK(ret == 2);
	ret = udiv_round(10, 9);
	CHECK(ret == 1);
	ret = udiv_round(10, 10);
	CHECK(ret == 1);
	ret = udiv_round(15, 6);
	CHECK(ret == 3);


	/* Check: pow_int() */

	CHECK_EQ_II(pow_int(0, 1), 0);
	CHECK_EQ_II(pow_int(10, 0), 1);
	CHECK_EQ_II(pow_int(10, 1), 10);
	CHECK_EQ_II(pow_int(10, 2), 100);
	CHECK_EQ_II(pow_int(10, 3), 1000);
	CHECK_EQ_II(pow_int(10, 4), 10000);
	CHECK_EQ_II(pow_int(10, 5), 100000);
	CHECK_EQ_II(pow_int(-10, 0), 1);
	CHECK_EQ_II(pow_int(-10, 1), -10);
	CHECK_EQ_II(pow_int(-10, 2), 100);
	CHECK_EQ_II(pow_int(-10, 3), -1000);
	CHECK_EQ_II(pow_int(-10, 4), 10000);
	CHECK_EQ_II(pow_int(-10, 5), -100000);
	CHECK_EQ_II(pow_int(3, 0), 1);
	CHECK_EQ_II(pow_int(3, 1), 3);
	CHECK_EQ_II(pow_int(3, 2), 9);
	CHECK_EQ_II(pow_int(3, 3), 27);
	CHECK_EQ_II(pow_int(3, 4), 81);
	CHECK_EQ_II(pow_int(3, 5), 243);
	CHECK_EQ_II(pow_int(-3, 0), 1);
	CHECK_EQ_II(pow_int(-3, 1), -3);
	CHECK_EQ_II(pow_int(-3, 2), 9);
	CHECK_EQ_II(pow_int(-3, 3), -27);
	CHECK_EQ_II(pow_int(-3, 4), 81);
	CHECK_EQ_II(pow_int(-3, 5), -243);
}

int main(void)
{
	check_util();

	return 0;
}
