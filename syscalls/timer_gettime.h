/*
 * SYSCALL_DEFINE2(timer_gettime, timer_t, timer_id, struct itimerspec __user *, setting)
 */
{
	.name = "timer_gettime",
	.num_args = 2,
	.arg1name = "timer_id",
	.arg2name = "setting",
	.arg2type = ARG_ADDRESS,
},
