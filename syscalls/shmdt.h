/*
 * SYSCALL_DEFINE1(shmdt, char __user *, shmaddr)
 */
{
	.name = "shmdt",
	.num_args = 1,
	.arg1name = "shmaddr",
	.arg1type = ARG_ADDRESS,
},
