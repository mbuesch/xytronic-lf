#define PIDVAL_SIZE	16
#define PIDVAL_SHIFT	5
#define PIDFLOAT_SIZE	32

#include "test.h"
#include "pid.c"


static void check_common(void)
{
	struct pid pid;
	pidval_t ret;

	/* Check conversion functions */

	CHECK(float_to_pidval(1.0) == int_to_pidval(1));
	CHECK(float_to_pidval(5.0) == int_to_pidval(5));
	CHECK(float_to_pidval(10.0) == int_to_pidval(10));
	CHECK(float_to_pidval(1023.0) == int_to_pidval(1023));
	CHECK(float_to_pidval(-1.0) == int_to_pidval(-1));
	CHECK(float_to_pidval(-99.0) == int_to_pidval(-99));
	CHECK(float_to_pidval(-1023.0) == int_to_pidval(-1023));
	CHECK(pidval_to_int(int_to_pidval(6)) == 6);
	CHECK(pidval_to_float(float_to_pidval(6.0)) == 6.0);
	CHECK(pidval_to_float(float_to_pidval(6.5)) == 6.5);
	CHECK(pidval_to_float(float_to_pidval(6.5)) != 6.0);
	CHECK(pidval_to_float(float_to_pidval(-6.0)) == -6.0);

	/* Check get/set setpoint */

	memset(&pid, 0xFF, sizeof(pid));
	pid_init(&pid,
		 float_to_pidval(1.0),		// kp
		 float_to_pidval(0.0),		// ki
		 float_to_pidval(0.0),		// kd
		 float_to_pidval(15.0));	// lim
	pid_set_setpoint(&pid, int_to_pidval(10));
	CHECK(pid_get_setpoint(&pid) == float_to_pidval(10.0));
}

static void check_P(void)
{
	struct pid pid;
	pidval_t ret;

	/* Check P-term with KP=1 */

	memset(&pid, 0xFF, sizeof(pid));
	pid_init(&pid,
		 float_to_pidval(1.0),		// kp
		 float_to_pidval(0.0),		// ki
		 float_to_pidval(0.0),		// kd
		 float_to_pidval(15.0));	// lim
	pid_set_setpoint(&pid, int_to_pidval(10));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(1.0));	// r
	CHECK(ret == float_to_pidval(9.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(2.0));	// r
	CHECK(ret == float_to_pidval(8.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(5.0));	// r
	CHECK(ret == float_to_pidval(5.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(5.0));	// r
	CHECK(ret == float_to_pidval(5.0));

	/* Check P-term with KP=3 */

	memset(&pid, 0xFF, sizeof(pid));
	pid_init(&pid,
		 float_to_pidval(3.0),		// kp
		 float_to_pidval(0.0),		// ki
		 float_to_pidval(0.0),		// kd
		 float_to_pidval(15.0));	// lim
	pid_set_setpoint(&pid, int_to_pidval(10));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(1.0));	// r
	CHECK(ret == float_to_pidval(15.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.5),	// dt
		      float_to_pidval(2.0));	// r
	CHECK(ret == float_to_pidval(15.0));
	ret = pid_run(&pid,
		      float_to_pidval(2.0),	// dt
		      float_to_pidval(5.0));	// r
	CHECK(ret == float_to_pidval(15.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(7.0));	// r
	CHECK(ret == float_to_pidval(9.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(7.5));	// r
	CHECK(ret == float_to_pidval(7.5));

	/* Check P-term with KP=2.5 */

	memset(&pid, 0xFF, sizeof(pid));
	pid_init(&pid,
		 float_to_pidval(2.5),		// kp
		 float_to_pidval(0.0),		// ki
		 float_to_pidval(0.0),		// kd
		 float_to_pidval(15.0));	// lim
	pid_set_setpoint(&pid, int_to_pidval(10));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(1.0));	// r
	CHECK(ret == float_to_pidval(15.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(2.0));	// r
	CHECK(ret == float_to_pidval(15.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(5.0));	// r
	CHECK(ret == float_to_pidval(12.5));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(7.0));	// r
	CHECK(ret == float_to_pidval(7.5));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(7.5));	// r
	CHECK(ret == float_to_pidval(6.25));

	/* Check P-term with KP=-2.5 */

	memset(&pid, 0xFF, sizeof(pid));
	pid_init(&pid,
		 float_to_pidval(-2.5),		// kp
		 float_to_pidval(0.0),		// ki
		 float_to_pidval(0.0),		// kd
		 float_to_pidval(15.0));	// lim
	pid_set_setpoint(&pid, int_to_pidval(10));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(1.0));	// r
	CHECK(ret == float_to_pidval(-15.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(2.0));	// r
	CHECK(ret == float_to_pidval(-15.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(5.0));	// r
	CHECK(ret == float_to_pidval(-12.5));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(7.0));	// r
	CHECK(ret == float_to_pidval(-7.5));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(7.5));	// r
	CHECK(ret == float_to_pidval(-6.25));
}

static void check_I(void)
{
	struct pid pid;
	pidval_t ret;

	/* Check I-term with KI=1 */

	memset(&pid, 0xFF, sizeof(pid));
	pid_init(&pid,
		 float_to_pidval(0.0),		// kp
		 float_to_pidval(1.0),		// ki
		 float_to_pidval(0.0),		// kd
		 float_to_pidval(15.0));	// lim
	pid_set_setpoint(&pid, int_to_pidval(10));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(9.0));	// r
	CHECK(ret == float_to_pidval(1.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(9.0));	// r
	CHECK(ret == float_to_pidval(2.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(9.0));	// r
	CHECK(ret == float_to_pidval(3.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(10.0));	// r
	CHECK(ret == float_to_pidval(3.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(11.0));	// r
	CHECK(ret == float_to_pidval(2.0));
	ret = pid_run(&pid,
		      float_to_pidval(3.0),	// dt
		      float_to_pidval(11.0));	// r
	CHECK(ret == float_to_pidval(-1.0));
	ret = pid_run(&pid,
		      float_to_pidval(0.5),	// dt
		      float_to_pidval(11.0));	// r
	CHECK(ret == float_to_pidval(-1.5));
	ret = pid_run(&pid,
		      float_to_pidval(0.5),	// dt
		      float_to_pidval(11.0));	// r
	CHECK(ret == float_to_pidval(-2.0));

	/* Check I-term with KI=-1 */

	memset(&pid, 0xFF, sizeof(pid));
	pid_init(&pid,
		 float_to_pidval(0.0),		// kp
		 float_to_pidval(-1.0),		// ki
		 float_to_pidval(0.0),		// kd
		 float_to_pidval(15.0));	// lim
	pid_set_setpoint(&pid, int_to_pidval(10));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(9.0));	// r
	CHECK(ret == float_to_pidval(-1.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(9.0));	// r
	CHECK(ret == float_to_pidval(-2.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(9.0));	// r
	CHECK(ret == float_to_pidval(-3.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(10.0));	// r
	CHECK(ret == float_to_pidval(-3.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(11.0));	// r
	CHECK(ret == float_to_pidval(-2.0));
	ret = pid_run(&pid,
		      float_to_pidval(3.0),	// dt
		      float_to_pidval(11.0));	// r
	CHECK(ret == float_to_pidval(1.0));
	ret = pid_run(&pid,
		      float_to_pidval(0.5),	// dt
		      float_to_pidval(11.0));	// r
	CHECK(ret == float_to_pidval(1.5));
	ret = pid_run(&pid,
		      float_to_pidval(0.5),	// dt
		      float_to_pidval(11.0));	// r
	CHECK(ret == float_to_pidval(2.0));

	/* Check I-term with KI=1.5 */

	memset(&pid, 0xFF, sizeof(pid));
	pid_init(&pid,
		 float_to_pidval(0.0),		// kp
		 float_to_pidval(1.5),		// ki
		 float_to_pidval(0.0),		// kd
		 float_to_pidval(15.0));	// lim
	pid_set_setpoint(&pid, int_to_pidval(10));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(9.0));	// r
	CHECK(ret == float_to_pidval(1.5));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(9.0));	// r
	CHECK(ret == float_to_pidval(3.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(9.0));	// r
	CHECK(ret == float_to_pidval(4.5));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(10.0));	// r
	CHECK(ret == float_to_pidval(4.5));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(11.0));	// r
	CHECK(ret == float_to_pidval(3.0));
	ret = pid_run(&pid,
		      float_to_pidval(3.0),	// dt
		      float_to_pidval(11.0));	// r
	CHECK(ret == float_to_pidval(-1.5));
	ret = pid_run(&pid,
		      float_to_pidval(0.5),	// dt
		      float_to_pidval(11.0));	// r
	CHECK(ret == float_to_pidval(-2.25));
	ret = pid_run(&pid,
		      float_to_pidval(0.5),	// dt
		      float_to_pidval(11.0));	// r
	CHECK(ret == float_to_pidval(-3.0));
}

static void check_D(void)
{
	struct pid pid;
	pidval_t ret;

	/* Check D-term with KD=1 */

	memset(&pid, 0xFF, sizeof(pid));
	pid_init(&pid,
		 float_to_pidval(0.0),		// kp
		 float_to_pidval(0.0),		// ki
		 float_to_pidval(1.0),		// kd
		 float_to_pidval(15.0));	// lim
	pid_set_setpoint(&pid, int_to_pidval(10));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(9.0));	// r
	CHECK(ret == float_to_pidval(1.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(8.0));	// r
	CHECK(ret == float_to_pidval(1.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(7.0));	// r
	CHECK(ret == float_to_pidval(1.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(5.0));	// r
	CHECK(ret == float_to_pidval(2.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(5.0));	// r
	CHECK(ret == float_to_pidval(0.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(7.0));	// r
	CHECK(ret == float_to_pidval(-2.0));

	/* Check D-term with KD=-1 */

	memset(&pid, 0xFF, sizeof(pid));
	pid_init(&pid,
		 float_to_pidval(0.0),		// kp
		 float_to_pidval(0.0),		// ki
		 float_to_pidval(-1.0),		// kd
		 float_to_pidval(15.0));	// lim
	pid_set_setpoint(&pid, int_to_pidval(10));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(9.0));	// r
	CHECK(ret == float_to_pidval(-1.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(8.0));	// r
	CHECK(ret == float_to_pidval(-1.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(7.0));	// r
	CHECK(ret == float_to_pidval(-1.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(5.0));	// r
	CHECK(ret == float_to_pidval(-2.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(5.0));	// r
	CHECK(ret == float_to_pidval(0.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(7.0));	// r
	CHECK(ret == float_to_pidval(2.0));


	/* Check D-term with KD=1.5 */

	memset(&pid, 0xFF, sizeof(pid));
	pid_init(&pid,
		 float_to_pidval(0.0),		// kp
		 float_to_pidval(0.0),		// ki
		 float_to_pidval(1.5),		// kd
		 float_to_pidval(15.0));	// lim
	pid_set_setpoint(&pid, int_to_pidval(10));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(9.0));	// r
	CHECK(ret == float_to_pidval(1.5));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(8.0));	// r
	CHECK(ret == float_to_pidval(1.5));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(7.0));	// r
	CHECK(ret == float_to_pidval(1.5));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(5.0));	// r
	CHECK(ret == float_to_pidval(3.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(5.0));	// r
	CHECK(ret == float_to_pidval(0.0));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(7.0));	// r
	CHECK(ret == float_to_pidval(-3.0));
}

static void check_PID(void)
{
	struct pid pid;
	pidval_t ret;

	/* Check PID with KP=1.5, KI=1.5, KD=1.5 */

	memset(&pid, 0xFF, sizeof(pid));
	pid_init(&pid,
		 float_to_pidval(1.5),		// kp
		 float_to_pidval(1.5),		// ki
		 float_to_pidval(1.5),		// kd
		 float_to_pidval(15.0));	// lim
	pid_set_setpoint(&pid, int_to_pidval(10));
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(9.0));	// r
	CHECK(ret == float_to_pidval(4.5));	// (integr=1.5)
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(8.0));	// r
	CHECK(ret == float_to_pidval(9.0));	// (integr=4.5)
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(8.0));	// r
	CHECK(ret == float_to_pidval(10.5));	// (integr=7.5)
	ret = pid_run(&pid,
		      float_to_pidval(1.0),	// dt
		      float_to_pidval(9.0));	// r
	CHECK(ret == float_to_pidval(9.0));	// (integr=9.0)
}

int main(void)
{
	check_common();
	check_P();
	check_I();
	check_D();
	check_PID();

	return 0;
}
