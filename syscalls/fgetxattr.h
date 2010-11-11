/*
 * SYSCALL_DEFINE4(fgetxattr, int, fd, const char __user *, name,
	 void __user *, value, size_t, size)
 */
{
	.name = "fgetxattr",
	.num_args = 4,
	.arg1name = "fd",
	.arg1type = ARG_FD,
	.arg2name = "name",
	.arg2type = ARG_ADDRESS,
	.arg3name = "value",
	.arg3type = ARG_ADDRESS,
	.arg4name = "size",
	.arg4type = ARG_LEN,
},
