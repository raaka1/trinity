/*
 * SYSCALL_DEFINE2(setgroups, int, gidsetsize, gid_t __user *, grouplist)
 */
{
	.name = "setgroups",
	.num_args = 2,
	.arg1name = "gidsetsize",
	.arg2name = "grouplist",
	.arg2type = ARG_ADDRESS,
},
