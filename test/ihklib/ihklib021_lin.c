/**
 * \file ihklib021_lin.c
 *  License details are found in the file LICENSE.
 * \brief
 *  Test ihk_os_get_assigned_cpus()
 * \author Masamichi Takagi  <masamichi.takagi@riken.jp> \par
 * Copyright (C) 2018  Masamichi Takagi
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ihklib.h>
#include <mckernel/ihklib_rusage.h>
#include <sys/types.h>
#include <errno.h>
#include <pwd.h>
#include "util.h"

static char prefix[256];

int main(int argc, char **argv)
{
	int ret = 0, status, ret_internal;
	FILE *fp, *fp1, *fp2;
	char buf[65536], buf1[65536], buf2[65536];
	size_t nread;

	char cmd[1024];
	char fn[256];
	char kargs[256];
	char logname[256], *envstr, *dup, *line, *groups;

	int cpus[4];
	int num_cpus;

	struct ihk_mem_chunk mem_chunks[4];
	int num_mem_chunks;

	int indices[2];
	int num_os_instances;

	ssize_t kmsg_size;

	struct ihk_ikc_cpu_map ikc_map[2];

	int num_numa_nodes;
	long memfree[4];

	int num_pgsizes;
	long pgsizes[3];

	struct mckernel_rusage rusage;

	char *home, *retstr;
	struct passwd *pwd;

	home = getenv("HOME");
	CHKANDJUMP(home == NULL, -1, "getenv");
	sprintf(prefix, "%s/project/os/install", home);
	printf("%s\n", prefix);

	fp = popen("logname", "r");
	nread = fread(logname, 1, sizeof(logname), fp);
	CHKANDJUMP(nread == 0, -1, "fread");
	retstr = strrchr(logname, '\n');
	if (retstr) {
		*retstr = 0;
	}
	printf("logname=%s\n", logname);

	envstr = getenv("GROUPS");
	CHKANDJUMP(envstr == NULL, -1, "groups");
	groups = strdup(envstr);
	retstr = strrchr(groups, '\n');
	if (retstr) {
		*retstr = 0;
	}
	printf("groups=%s\n", groups);

	if (geteuid() != 0) {
		printf("Execute as a root\n");
	}

	/* Expected to succeed */

	sprintf(cmd, "insmod %s/kmod/ihk.ko", prefix);
	status = system(cmd);
	CHKANDJUMP(WEXITSTATUS(status) != 0, -1, "system");

	sprintf(cmd, "insmod %s/kmod/ihk-smp-x86_64.ko "
			"ihk_start_irq=240 ihk_ikc_irq_core=0", prefix);
	status = system(cmd);
	CHKANDJUMP(WEXITSTATUS(status) != 0, -1, "system");

	sprintf(cmd, "chown %s:%s /dev/mcd*\n", logname, groups);
	printf("%s\n", cmd);
	status = system(cmd);
	CHKANDJUMP(WEXITSTATUS(status) != 0, -1, "system");

	sprintf(cmd, "insmod %s/kmod/mcctrl.ko", prefix);
	status = system(cmd);
	CHKANDJUMP(WEXITSTATUS(status) != 0, -1, "system");

	// reserve cpu
	cpus[0] = 1;
	cpus[1] = 2;
	num_cpus = 2;
	ret = ihk_reserve_cpu(0, cpus, num_cpus);
	OKNG(ret == 0, "ihk_reserve_cpu 1,2 succeeded\n");

	// get # of reserved cpus
	num_cpus = ihk_get_num_reserved_cpus(0);
	OKNG(num_cpus == 2, "ihk_get_num_reserved_cpu returned 2\n");

	// get reserved cpus. Note that cpu# is sorted in ihk.
	ret = ihk_query_cpu(0, cpus, num_cpus);
	OKNG(ret == 0 &&
		 cpus[0] == 1 &&
		 cpus[1] == 2, "ihk_query_cpu returned 1,2\n");

	// release cpu
	cpus[0] = 1;
	cpus[1] = 2;
	num_cpus = 2;
	ret = ihk_release_cpu(0, cpus, num_cpus);
	OKNG(ret == 0, "ihk_release_cpu 1,2 succeeded\n");

	// get # of reserved cpus
	num_cpus = ihk_get_num_reserved_cpus(0);
	OKNG(num_cpus == 0, "ihk_get_num_reserved_cpu returned 0\n");

	// reserve cpu
	cpus[0] = 2;
	cpus[1] = 3;
	num_cpus = 2;
	ret = ihk_reserve_cpu(0, cpus, num_cpus);
	OKNG(ret == 0, "ihk_reserve_cpu 2,3 succeeded\n");

	// get # of reserved cpus
	num_cpus = ihk_get_num_reserved_cpus(0);
	OKNG(num_cpus == 2, "ihk_get_num_reserved_cpu returned 2\n");

	// get reserved cpus. Note that cpu# is sorted in ihk.
	ret = ihk_query_cpu(0, cpus, num_cpus);
	OKNG(ret == 0 &&
		 cpus[0] == 2 &&
		 cpus[1] == 3, "ihk_query_cpu returned 2,3\n");

	// release cpu
	cpus[0] = 2;
	cpus[1] = 3;
	num_cpus = 2;
	ret = ihk_release_cpu(0, cpus, num_cpus);
	OKNG(ret == 0, "ihk_release_cpu 2,3 succeeded\n");

	// get # of reserved cpus
	num_cpus = ihk_get_num_reserved_cpus(0);
	OKNG(num_cpus == 0, "ihk_get_num_reserved_cpu returned 0\n");

	// reserve cpu
	cpus[0] = 3;
	cpus[1] = 1;
	num_cpus = 2;
	ret = ihk_reserve_cpu(0, cpus, num_cpus);
	OKNG(ret == 0, "ihk_reserve_cpu 3,1 succeeded\n");

	// get # of reserved cpus
	num_cpus = ihk_get_num_reserved_cpus(0);
	OKNG(num_cpus == 2, "ihk_get_num_reserved_cpu returned 2\n");

	// get reserved cpus. Note that cpu# is sorted in ihk.
	ret = ihk_query_cpu(0, cpus, num_cpus);
	OKNG(ret == 0 &&
		 cpus[0] == 1 &&
		 cpus[1] == 3, "ihk_query_cpu returned 1,3\n");

	// release cpu
	cpus[0] = 3;
	num_cpus = 1;
	ret = ihk_release_cpu(0, cpus, num_cpus);
	OKNG(ret == 0, "ihk_release_cpu 3 succeeded\n");

	// get # of reserved cpus
	num_cpus = ihk_get_num_reserved_cpus(0);
	OKNG(num_cpus == 1, "ihk_get_num_reserved_cpu returned 1\n");

	// get reserved cpus
	ret = ihk_query_cpu(0, cpus, num_cpus);
	OKNG(ret == 0 &&
		 cpus[0] == 1, "ihk_query_cpu returned 1\n");

	// get # of reserverd cpus (error handling)
	ret = ihk_get_num_reserved_cpus(1);
	OKNG(ret == -ENOENT,
	     "ihk_get_num_reserved_cpus returned -ENOENT as expected\n");

	// get reserverd cpus (error handling)
	ret = ihk_query_cpu(0, cpus, 1ULL<<30);
	OKNG(ret == -EINVAL,
	     "ihk_query_cpu returned -EINVAL as expected\n");

	// get reserverd cpus (error handling)
	ret = ihk_query_cpu(1, cpus, num_cpus);
	OKNG(ret == -ENOENT,
	     "ihk_query_cpu returned -ENOENT as expected\n");

	// get reserved cpus (error handling)
	ret = ihk_query_cpu(0, cpus, num_cpus + 1);
	OKNG(ret == -EINVAL,
	     "ihk_query_cpu returned -EINVAL as expected\n");

	// seteuid bin
	CHKANDJUMP(geteuid(), -1, "not a superuser");

	CHKANDJUMP(!(pwd = getpwnam("bin")), -1,
		"getpwnam failed: %s\n", strerror(errno));

	CHKANDJUMP(seteuid(pwd->pw_uid), -1,
		"seteuid failed: %s\n", strerror(errno));

	// get # of reserved cpus (error handling)
	ret = ihk_get_num_reserved_cpus(0);
	OKNG(ret == -EACCES,
	     "ihk_get_num_reserved_cpus returned -EACCES as expected\n");

	// get reserved cpus (error handling)
	ret = ihk_query_cpu(0, cpus, num_cpus);
	OKNG(ret == -EACCES,
	     "ihk_query_cpu returned -EACCES as expected\n");

	// setuid root
	CHKANDJUMP(seteuid(0), -1, "seteuid failed: %s\n", strerror(errno));

	// rmmod modules
	sprintf(cmd, "rmmod %s/kmod/mcctrl.ko", prefix);
	status = system(cmd);
	CHKANDJUMP(WEXITSTATUS(status) != 0, -1, "system");

	sprintf(cmd, "rmmod %s/kmod/ihk-smp-x86_64.ko", prefix);
	status = system(cmd);
	CHKANDJUMP(WEXITSTATUS(status) != 0, -1, "system");

	sprintf(cmd, "rmmod %s/kmod/ihk.ko", prefix);
	status = system(cmd);
	CHKANDJUMP(WEXITSTATUS(status) != 0, -1, "system");

	printf("All tests finished\n");
	ret = 0;
 fn_fail:
	return ret;
}
