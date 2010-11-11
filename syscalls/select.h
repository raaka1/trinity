/*
 * SYSCALL_DEFINE5(select, int, n, fd_set __user *, inp, fd_set __user *, outp,
	fd_set __user *, exp, struct timeval __user *, tvp)
 */
{
	.name = "select",
	.num_args = 5,
	.flags = AVOID_SYSCALL,
	.arg1name = "n",
	.arg2name = "inp",
	.arg2type = ARG_ADDRESS,
	.arg3name = "outp",
	.arg3type = ARG_ADDRESS,
	.arg4name = "exp",
	.arg4type = ARG_ADDRESS,
	.arg5name = "tvp",
	.arg5type = ARG_ADDRESS,
},
