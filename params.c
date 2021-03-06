#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "bdevs.h"
#include "child.h"
#include "log.h"
#include "net.h"
#include "params.h"
#include "protocols.h"
#include "random.h"
#include "syscall.h"
#include "tables.h"
#include "taint.h"
#include "trinity.h"	// progname

bool debug = FALSE;

bool do_specific_syscall = FALSE;
bool do_exclude_syscall = FALSE;

bool do_32_arch = TRUE;
bool do_64_arch = TRUE;

unsigned int specific_proto = 0;
unsigned int user_specified_children = 0;

bool do_specific_proto = FALSE;
bool no_protos[TRINITY_PF_MAX];

bool dopause = FALSE;
bool show_syscall_list = FALSE;
bool show_ioctl_list = FALSE;
unsigned char quiet_level = 0;
bool verbose = FALSE;
bool monochrome = FALSE;
bool dangerous = FALSE;
bool dropprivs = FALSE;
bool logging = TRUE;
bool do_syslog = FALSE;
bool random_selection = FALSE;
unsigned int random_selection_num;
bool no_files = FALSE;

bool user_set_seed = FALSE;

unsigned char desired_group = GROUP_NONE;

char *specific_proto_optarg = NULL;

char *victim_path = NULL;

unsigned int kernel_taint_mask = 0xFFFFFFFF;
bool kernel_taint_param_occured = FALSE;

static void usage(void)
{
	outputerr("%s\n", progname);
	outputerr(" --arch, -a: selects syscalls for the specified architecture (32 or 64). Both by default.\n");
	outputerr(" --bdev, -b <node>:  Add /dev node to list of block devices to use for destructive tests..\n");
	outputerr(" --children,-C: specify number of child processes\n");
	outputerr(" --debug,-D: enable debug\n");
	outputerr(" --dropprivs, -X: if run as root, switch to nobody [EXPERIMENTAL]\n");
	outputerr(" --exclude,-x: don't call a specific syscall\n");
	outputerr(" --group,-g: only run syscalls from a certain group (So far just 'vm').\n");
	outputerr(" --ioctls,-I: list all ioctls.\n");
	outputerr(" --kernel_taint, -T: controls which kernel taint flags should be considered, for more details refer to README file. \n");
	outputerr(" --list,-L: list all syscalls known on this architecture.\n");
	outputerr(" --logging,-l: (off=disable logging).\n");
	outputerr(" --monochrome,-m: don't output ANSI codes\n");
	outputerr(" --no_files,-n: Only pass sockets as fd's, not files\n");
	outputerr(" --proto,-P: specify specific network protocol for sockets.\n");
	outputerr(" --no_proto,-E: specify network protocols to be excluded from testing.\n");
	outputerr(" --quiet,-q: less output.\n");
	outputerr(" --random,-r#: pick N syscalls at random and just fuzz those\n");
	outputerr(" --syslog,-S: log important info to syslog. (useful if syslog is remote)\n");
	outputerr(" --verbose,-v: increase output verbosity.\n");
	outputerr(" --victims,-V: path to victim files.\n");
	outputerr("\n");
	outputerr(" -c#,@: target specific syscall (takes syscall name as parameter and optionally 32 or 64 as bit-width. Default:both).\n");
	outputerr(" -N#: do # syscalls then exit.\n");
	outputerr(" -p:  pause after syscall.\n");
	outputerr(" -s#: use # as random seed.\n");
	exit(EXIT_SUCCESS);
}

static const char paramstr[] = "a:b:c:C:dDg:hIl:LN:mnP:E:pqr:s:T:SV:vx:X";

static const struct option longopts[] = {
	{ "arch", required_argument, NULL, 'a' },
	{ "bdev", required_argument, NULL, 'b' },
	{ "children", required_argument, NULL, 'C' },
	{ "dangerous", no_argument, NULL, 'd' },
	{ "dropprivs", no_argument, NULL, 'X'},
	{ "debug", no_argument, NULL, 'D' },
	{ "exclude", required_argument, NULL, 'x' },
	{ "group", required_argument, NULL, 'g' },
	{ "kernel_taint", required_argument, NULL, 'T' },
	{ "help", no_argument, NULL, 'h' },
	{ "list", no_argument, NULL, 'L' },
	{ "ioctls", no_argument, NULL, 'I' },
	{ "logging", required_argument, NULL, 'l' },
	{ "monochrome", no_argument, NULL, 'm' },
	{ "no_files", no_argument, NULL, 'n' },
	{ "no_proto", required_argument, NULL, 'E' },
	{ "proto", required_argument, NULL, 'P' },
	{ "quiet", no_argument, NULL, 'q' },
	{ "random", required_argument, NULL, 'r' },
	{ "syslog", no_argument, NULL, 'S' },
	{ "verbose", no_argument, NULL, 'v' },
	{ "victims", required_argument, NULL, 'V' },
	{ NULL, 0, NULL, 0 } };

void parse_args(int argc, char *argv[])
{
	int opt;

	while ((opt = getopt_long(argc, argv, paramstr, longopts, NULL)) != -1) {
		switch (opt) {
		default:
			if (opt == '?')
				exit(EXIT_FAILURE);
			else
				outputstd("opt:%c\n", opt);
			return;

		case '\0':
			return;

		case 'b':
			init_bdev_list();
			process_bdev_param(optarg);
			dump_bdev_list();
			outputstd("--bdev doesn't do anything useful yet.\n");
			exit(EXIT_SUCCESS);

		case 'c':
			/* syscalls are all disabled at this point. enable the syscall we care about. */
			do_specific_syscall = TRUE;
			toggle_syscall(optarg, TRUE);
			break;

		case 'a':
			/* One of the architectures selected*/
			do_32_arch = FALSE;
			do_64_arch = FALSE;
			if (strcmp(optarg, "64") == 0) {
				do_32_arch = FALSE;
				do_64_arch = TRUE;
			} else if (strcmp(optarg, "32") == 0) {
				do_32_arch = TRUE;
				do_64_arch = FALSE;
			} else {
				outputstd("can't parse %s\n", optarg);
				exit(EXIT_FAILURE);
			}
			break;

		case 'C':
			user_specified_children = strtoll(optarg, NULL, 10);
			max_children = user_specified_children;

			if (max_children == 0) {
				outputerr("zero children ? WAT?\n");
				exit(EXIT_FAILURE);
			}
			break;

		case 'd':
			dangerous = TRUE;
			break;

		case 'D':
			debug = TRUE;
			break;

		case 'g':
			if (!strcmp(optarg, "vm"))
				desired_group = GROUP_VM;
			if (!strcmp(optarg, "vfs"))
				desired_group = GROUP_VFS;
			break;

		/* Show help */
		case 'h':
			usage();
			exit(EXIT_SUCCESS);

		case 'I':
			show_ioctl_list = TRUE;
			break;

		case 'l':
			if (!strcmp(optarg, "off"))
				logging = FALSE;
			break;

		case 'L':
			show_syscall_list = TRUE;
			break;

		case 'm':
			monochrome = TRUE;
			memset(&ANSI_RED, 0, 1);
			memset(&ANSI_GREEN, 0, 1);
			memset(&ANSI_YELLOW, 0, 1);
			memset(&ANSI_BLUE, 0, 1);
			memset(&ANSI_MAGENTA, 0, 1);
			memset(&ANSI_CYAN, 0, 1);
			memset(&ANSI_WHITE, 0, 1);
			memset(&ANSI_RESET, 0, 1);
			break;

		case 'n':
			no_files = TRUE;
			break;

		/* Set number of syscalls to do */
		case 'N':
			syscalls_todo = strtoll(optarg, NULL, 10);
			break;

		/* Pause after each syscall */
		case 'p':
			dopause = TRUE;
			break;

		case 'P':
			do_specific_proto = TRUE;
			specific_proto = strtol(optarg, NULL, 10);
			specific_proto_optarg = optarg;
			break;

		case 'E':
			parse_exclude_protos(optarg);
			break;

		case 'q':
			quiet_level++;
			break;

		case 'r':
			if (do_exclude_syscall == TRUE) {
				outputerr("-r needs to be before any -x options.\n");
				exit(EXIT_FAILURE);
			}
			random_selection = TRUE;
			random_selection_num = strtol(optarg, NULL, 10);
			break;

		/* Set seed */
		case 's':
			seed = strtol(optarg, NULL, 10);
			user_set_seed = TRUE;
			break;


		case 'S':
			do_syslog = TRUE;
			break;

		case 'T':
			//Load mask for kernel taint flags.
			process_taint_arg(optarg);
			if (kernel_taint_mask != 0xFFFFFFFF)
				outputstd("Custom kernel taint mask has been specified: 0x%08x (%d).\n",
					kernel_taint_mask, kernel_taint_mask);
			break;

		case 'v':
			verbose = TRUE;
			break;

		case 'V':
			if (victim_path == NULL) {
				victim_path = strdup(optarg);
				break;
			} else {
				outputstd("Sorry, only one victim path right now.\n");
				exit(EXIT_FAILURE);
			}
			//FIXME: Later, allow for multiple victim files
			break;

		case 'x':
			do_exclude_syscall = TRUE;
			toggle_syscall(optarg, FALSE);
			break;

		case 'X':
			dropprivs = TRUE;
			break;
		}
	}
	if (quiet_level > MAX_LOGLEVEL)
		quiet_level = MAX_LOGLEVEL;

	quiet_level = MAX_LOGLEVEL - quiet_level;

	output(1, "Done parsing arguments.\n");
}
