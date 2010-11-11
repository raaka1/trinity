/*
 * SYSCALL_DEFINE2(sethostname, char __user *, name, int, len)
 */
{
	.name = "sethostname",
	.num_args = 2,
	.flags = CAPABILITY_CHECK,
	.arg1name = "name",
	.arg1type = ARG_ADDRESS,
	.arg2name = "len",
	.arg2type = ARG_LEN,
},
