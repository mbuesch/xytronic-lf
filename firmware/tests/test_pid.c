#include "test.h"
#include "pid.c"

int main(void)
{
	struct pid pid;
	pidval_t ret;


	memset(&pid, 0xFF, sizeof(pid));
	pid_init(&pid,
		 FLOAT_TO_PIDVAL(1.0),		// kp
		 FLOAT_TO_PIDVAL(0.0),		// ki
		 FLOAT_TO_PIDVAL(0.0),		// kd
		 FLOAT_TO_PIDVAL(15.0));	// lim
	pid_set_setpoint(&pid, INT_TO_PIDVAL(10));
	CHECK(pid_get_setpoint(&pid) == FLOAT_TO_PIDVAL(10.0));


	memset(&pid, 0xFF, sizeof(pid));
	pid_init(&pid,
		 FLOAT_TO_PIDVAL(1.0),		// kp
		 FLOAT_TO_PIDVAL(0.0),		// ki
		 FLOAT_TO_PIDVAL(0.0),		// kd
		 FLOAT_TO_PIDVAL(15.0));	// lim
	pid_set_setpoint(&pid, INT_TO_PIDVAL(10));
	ret = pid_run(&pid,
		      FLOAT_TO_PIDVAL(1.0),	// dt
		      FLOAT_TO_PIDVAL(5.0));	// r
//	CHECK(ret == 99);
	//TODO

	return 0;
}
