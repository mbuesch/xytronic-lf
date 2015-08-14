#include "test.h"
#include "util.h"


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
}

int main(void)
{
	check_util();

	return 0;
}
