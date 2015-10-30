#include "test.h"
#include "pid.c"
#include "fixpt.c"


static void check_common(void)
{
	struct pid pid;
	struct pid_k_set k;

	/* Check get/set setpoint */

	memset(&pid, 0xFF, sizeof(pid));
	memset(&k, 0xFF, sizeof(k));
	k.kp = float_to_fixpt(1.0);
	k.ki = float_to_fixpt(0.0);
	k.kd = float_to_fixpt(0.0);
	k.d_decay_div = float_to_fixpt(1.0);
	pid_init(&pid, &k,
		 float_to_fixpt(-15.0),		// -lim
		 float_to_fixpt(15.0));		// +lim
	pid_set_setpoint(&pid, int_to_fixpt(10));
	CHECK(pid_get_setpoint(&pid) == float_to_fixpt(10.0));
}

static void check_P(void)
{
	struct pid pid;
	struct pid_k_set k;
	fixpt_t ret;

	/* Check P-term with KP=1 */

	memset(&pid, 0xFF, sizeof(pid));
	memset(&k, 0xFF, sizeof(k));
	k.kp = float_to_fixpt(1.0);
	k.ki = float_to_fixpt(0.0);
	k.kd = float_to_fixpt(0.0);
	k.d_decay_div = float_to_fixpt(1.0);
	pid_init(&pid, &k,
		 float_to_fixpt(-15.0),		// -lim
		 float_to_fixpt(15.0));		// +lim
	pid_set_setpoint(&pid, int_to_fixpt(10));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(1.0));	// r
	CHECK(ret == float_to_fixpt(9.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(2.0));	// r
	CHECK(ret == float_to_fixpt(8.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(5.0));	// r
	CHECK(ret == float_to_fixpt(5.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(5.0));	// r
	CHECK(ret == float_to_fixpt(5.0));

	/* Check P-term with KP=3 */

	memset(&pid, 0xFF, sizeof(pid));
	memset(&k, 0xFF, sizeof(k));
	k.kp = float_to_fixpt(3.0);
	k.ki = float_to_fixpt(0.0);
	k.kd = float_to_fixpt(0.0);
	k.d_decay_div = float_to_fixpt(1.0);
	pid_init(&pid, &k,
		 float_to_fixpt(-15.0),		// -lim
		 float_to_fixpt(15.0));		// +lim
	pid_set_setpoint(&pid, int_to_fixpt(10));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(1.0));	// r
	CHECK(ret == float_to_fixpt(15.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.5),	// dt
		      float_to_fixpt(2.0));	// r
	CHECK(ret == float_to_fixpt(15.0));
	ret = pid_run(&pid,
		      float_to_fixpt(2.0),	// dt
		      float_to_fixpt(5.0));	// r
	CHECK(ret == float_to_fixpt(15.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(7.0));	// r
	CHECK(ret == float_to_fixpt(9.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(7.5));	// r
	CHECK(ret == float_to_fixpt(7.5));

	/* Check P-term with KP=2.5 */

	memset(&pid, 0xFF, sizeof(pid));
	memset(&k, 0xFF, sizeof(k));
	k.kp = float_to_fixpt(2.5);
	k.ki = float_to_fixpt(0.0);
	k.kd = float_to_fixpt(0.0);
	k.d_decay_div = float_to_fixpt(1.0);
	pid_init(&pid, &k,
		 float_to_fixpt(-15.0),		// -lim
		 float_to_fixpt(15.0));		// +lim
	pid_set_setpoint(&pid, int_to_fixpt(10));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(1.0));	// r
	CHECK(ret == float_to_fixpt(15.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(2.0));	// r
	CHECK(ret == float_to_fixpt(15.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(5.0));	// r
	CHECK(ret == float_to_fixpt(12.5));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(7.0));	// r
	CHECK(ret == float_to_fixpt(7.5));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(7.5));	// r
	CHECK(ret == float_to_fixpt(6.25));

	/* Check P-term with KP=-2.5 */

	memset(&pid, 0xFF, sizeof(pid));
	memset(&k, 0xFF, sizeof(k));
	k.kp = float_to_fixpt(-2.5);
	k.ki = float_to_fixpt(0.0);
	k.kd = float_to_fixpt(0.0);
	k.d_decay_div = float_to_fixpt(1.0);
	pid_init(&pid, &k,
		 float_to_fixpt(-15.0),		// -lim
		 float_to_fixpt(15.0));		// +lim
	pid_set_setpoint(&pid, int_to_fixpt(10));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(1.0));	// r
	CHECK(ret == float_to_fixpt(-15.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(2.0));	// r
	CHECK(ret == float_to_fixpt(-15.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(5.0));	// r
	CHECK(ret == float_to_fixpt(-12.5));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(7.0));	// r
	CHECK(ret == float_to_fixpt(-7.5));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(7.5));	// r
	CHECK(ret == float_to_fixpt(-6.25));
}

static void check_I(void)
{
	struct pid pid;
	struct pid_k_set k;
	fixpt_t ret;

	/* Check I-term with KI=1 */

	memset(&pid, 0xFF, sizeof(pid));
	memset(&k, 0xFF, sizeof(k));
	k.kp = float_to_fixpt(0.0);
	k.ki = float_to_fixpt(1.0);
	k.kd = float_to_fixpt(0.0);
	k.d_decay_div = float_to_fixpt(1.0);
	pid_init(&pid, &k,
		 float_to_fixpt(-15.0),		// -lim
		 float_to_fixpt(15.0));		// +lim
	pid_set_setpoint(&pid, int_to_fixpt(10));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(9.0));	// r
	CHECK(ret == float_to_fixpt(1.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(9.0));	// r
	CHECK(ret == float_to_fixpt(2.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(9.0));	// r
	CHECK(ret == float_to_fixpt(3.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(10.0));	// r
	CHECK(ret == float_to_fixpt(3.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(11.0));	// r
	CHECK(ret == float_to_fixpt(2.0));
	ret = pid_run(&pid,
		      float_to_fixpt(3.0),	// dt
		      float_to_fixpt(11.0));	// r
	CHECK(ret == float_to_fixpt(-1.0));
	ret = pid_run(&pid,
		      float_to_fixpt(0.5),	// dt
		      float_to_fixpt(11.0));	// r
	CHECK(ret == float_to_fixpt(-1.5));
	ret = pid_run(&pid,
		      float_to_fixpt(0.5),	// dt
		      float_to_fixpt(11.0));	// r
	CHECK(ret == float_to_fixpt(-2.0));

	/* Check I-term with KI=-1 */

	memset(&pid, 0xFF, sizeof(pid));
	memset(&k, 0xFF, sizeof(k));
	k.kp = float_to_fixpt(0.0);
	k.ki = float_to_fixpt(-1.0);
	k.kd = float_to_fixpt(0.0);
	k.d_decay_div = float_to_fixpt(1.0);
	pid_init(&pid, &k,
		 float_to_fixpt(-15.0),		// -lim
		 float_to_fixpt(15.0));		// +lim
	pid_set_setpoint(&pid, int_to_fixpt(10));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(9.0));	// r
	CHECK(ret == float_to_fixpt(-1.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(9.0));	// r
	CHECK(ret == float_to_fixpt(-2.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(9.0));	// r
	CHECK(ret == float_to_fixpt(-3.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(10.0));	// r
	CHECK(ret == float_to_fixpt(-3.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(11.0));	// r
	CHECK(ret == float_to_fixpt(-2.0));
	ret = pid_run(&pid,
		      float_to_fixpt(3.0),	// dt
		      float_to_fixpt(11.0));	// r
	CHECK(ret == float_to_fixpt(1.0));
	ret = pid_run(&pid,
		      float_to_fixpt(0.5),	// dt
		      float_to_fixpt(11.0));	// r
	CHECK(ret == float_to_fixpt(1.5));
	ret = pid_run(&pid,
		      float_to_fixpt(0.5),	// dt
		      float_to_fixpt(11.0));	// r
	CHECK(ret == float_to_fixpt(2.0));

	/* Check I-term with KI=1.5 */

	memset(&pid, 0xFF, sizeof(pid));
	memset(&k, 0xFF, sizeof(k));
	k.kp = float_to_fixpt(0.0);
	k.ki = float_to_fixpt(1.5);
	k.kd = float_to_fixpt(0.0);
	k.d_decay_div = float_to_fixpt(1.0);
	pid_init(&pid, &k,
		 float_to_fixpt(-15.0),		// -lim
		 float_to_fixpt(15.0));		// +lim
	pid_set_setpoint(&pid, int_to_fixpt(10));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(9.0));	// r
	CHECK(ret == float_to_fixpt(1.5));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(9.0));	// r
	CHECK(ret == float_to_fixpt(3.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(9.0));	// r
	CHECK(ret == float_to_fixpt(4.5));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(10.0));	// r
	CHECK(ret == float_to_fixpt(4.5));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(11.0));	// r
	CHECK(ret == float_to_fixpt(3.0));
	ret = pid_run(&pid,
		      float_to_fixpt(3.0),	// dt
		      float_to_fixpt(11.0));	// r
	CHECK(ret == float_to_fixpt(-1.5));
	ret = pid_run(&pid,
		      float_to_fixpt(0.5),	// dt
		      float_to_fixpt(11.0));	// r
	CHECK(ret == float_to_fixpt(-2.25));
	ret = pid_run(&pid,
		      float_to_fixpt(0.5),	// dt
		      float_to_fixpt(11.0));	// r
	CHECK(ret == float_to_fixpt(-3.0));
}

static void check_D(void)
{
	struct pid pid;
	struct pid_k_set k;
	fixpt_t ret;

	/* Check D-term with KD=1 */

	memset(&pid, 0xFF, sizeof(pid));
	memset(&k, 0xFF, sizeof(k));
	k.kp = float_to_fixpt(0.0);
	k.ki = float_to_fixpt(0.0);
	k.kd = float_to_fixpt(1.0);
	k.d_decay_div = float_to_fixpt(1.0);
	pid_init(&pid, &k,
		 float_to_fixpt(-15.0),		// -lim
		 float_to_fixpt(15.0));		// +lim
	pid_set_setpoint(&pid, int_to_fixpt(10));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(9.0));	// r
	CHECK(ret == float_to_fixpt(1.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(8.0));	// r
	CHECK(ret == float_to_fixpt(1.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(7.0));	// r
	CHECK(ret == float_to_fixpt(1.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(5.0));	// r
	CHECK(ret == float_to_fixpt(2.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(5.0));	// r
	CHECK(ret == float_to_fixpt(0.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(7.0));	// r
	CHECK(ret == float_to_fixpt(-2.0));

	/* Check D-term with KD=-1 */

	memset(&pid, 0xFF, sizeof(pid));
	memset(&k, 0xFF, sizeof(k));
	k.kp = float_to_fixpt(0.0);
	k.ki = float_to_fixpt(0.0);
	k.kd = float_to_fixpt(-1.0);
	k.d_decay_div = float_to_fixpt(1.0);
	pid_init(&pid, &k,
		 float_to_fixpt(-15.0),		// -lim
		 float_to_fixpt(15.0));		// +lim
	pid_set_setpoint(&pid, int_to_fixpt(10));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(9.0));	// r
	CHECK(ret == float_to_fixpt(-1.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(8.0));	// r
	CHECK(ret == float_to_fixpt(-1.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(7.0));	// r
	CHECK(ret == float_to_fixpt(-1.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(5.0));	// r
	CHECK(ret == float_to_fixpt(-2.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(5.0));	// r
	CHECK(ret == float_to_fixpt(0.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(7.0));	// r
	CHECK(ret == float_to_fixpt(2.0));


	/* Check D-term with KD=1.5 */

	memset(&pid, 0xFF, sizeof(pid));
	memset(&k, 0xFF, sizeof(k));
	k.kp = float_to_fixpt(0.0);
	k.ki = float_to_fixpt(0.0);
	k.kd = float_to_fixpt(1.5);
	k.d_decay_div = float_to_fixpt(1.0);
	pid_init(&pid, &k,
		 float_to_fixpt(-15.0),		// -lim
		 float_to_fixpt(15.0));		// +lim
	pid_set_setpoint(&pid, int_to_fixpt(10));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(9.0));	// r
	CHECK(ret == float_to_fixpt(1.5));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(8.0));	// r
	CHECK(ret == float_to_fixpt(1.5));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(7.0));	// r
	CHECK(ret == float_to_fixpt(1.5));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(5.0));	// r
	CHECK(ret == float_to_fixpt(3.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(5.0));	// r
	CHECK(ret == float_to_fixpt(0.0));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(7.0));	// r
	CHECK(ret == float_to_fixpt(-3.0));
}

static void check_PID(void)
{
	struct pid pid;
	struct pid_k_set k;
	fixpt_t ret;

	/* Check PID with KP=1.5, KI=1.5, KD=1.5 */

	memset(&pid, 0xFF, sizeof(pid));
	memset(&k, 0xFF, sizeof(k));
	k.kp = float_to_fixpt(1.5);
	k.ki = float_to_fixpt(1.5);
	k.kd = float_to_fixpt(1.5);
	k.d_decay_div = float_to_fixpt(1.0);
	pid_init(&pid, &k,
		 float_to_fixpt(-15.0),		// -lim
		 float_to_fixpt(15.0));		// +lim
	pid_set_setpoint(&pid, int_to_fixpt(10));
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(9.0));	// r
	CHECK(ret == float_to_fixpt(4.5));	// (integr=1.5)
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(8.0));	// r
	CHECK(ret == float_to_fixpt(9.0));	// (integr=4.5)
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(8.0));	// r
	CHECK(ret == float_to_fixpt(10.5));	// (integr=7.5)
	ret = pid_run(&pid,
		      float_to_fixpt(1.0),	// dt
		      float_to_fixpt(9.0));	// r
	CHECK(ret == float_to_fixpt(9.0));	// (integr=9.0)
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
