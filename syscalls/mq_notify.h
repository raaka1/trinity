/*
 * SYSCALL_DEFINE2(mq_notify, mqd_t, mqdes, const struct sigevent __user *, u_notification)
 */
{
	.name = "mq_notify",
	.num_args = 2,
	.arg1name = "mqdes",
	.arg2name = "u_notification",
	.arg2type = ARG_ADDRESS,
},
