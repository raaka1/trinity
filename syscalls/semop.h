/*
 * SYSCALL_DEFINE3(semop, int, semid, struct sembuf __user *, tsops, unsigned, nsops)
 */
{
	.name = "semop",
	.num_args = 3,
	.arg1name = "semid",
	.arg2name = "tsops",
	.arg2type = ARG_ADDRESS,
	.arg3name = "nsops",
},
