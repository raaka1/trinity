/*
 * SYSCALL_DEFINE1(mq_unlink, const char __user *, u_name)
 */
{
	.name = "mq_unlink",
	.num_args = 1,
	.arg1name = "u_name",
	.arg1type = ARG_ADDRESS,
},
