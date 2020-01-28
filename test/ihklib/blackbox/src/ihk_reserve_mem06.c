#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <ihklib.h>
#include "util.h"
#include "okng.h"
#include "cpu.h"
#include "params.h"
#include "init_fini.h"

int main(int argc, char **argv)
{
	int ret;
	int i;
	
	params_getopt(argc, argv);

	/* Parse additional options */
	int opt;

	while ((opt = getopt(argc, argv, "ir")) != -1) {
		switch (opt) {
		case 'i':
			/* Precondition */
			ret = insmod(params.uid, params.gid);
			INTERR(ret != 0, "insmod returned %d\n", ret);
			exit(0);
			break;
		case 'r':
			/* Clean up */
			ret = rmmod(1);
			INTERR(ret != 0, "rmmod returned %d\n", ret);
			exit(0);
			break;
		default: /* '?' */
			printf("unknown option %c\n", optopt);
			exit(1);
		}
	}

	const char *messages[] =
		{
		 "non-root",
		};
	
	struct mems cpu_inputs[1] = { 0 };

	/* Both Linux and McKernel cpus */
	for (i = 0; i < 1; i++) { 
		ret = cpus_ls(&cpu_inputs[i]);
		INTERR(ret, "cpus_ls returned %d\n", ret);
	}

	/* Spare two cpus for Linux */
	for (i = 0; i < 1; i++) { 
		ret = cpus_shift(&cpu_inputs[i], 2);
		INTERR(ret, "cpus_shift returned %d\n", ret);
	}

	int ret_expected[] = { -EACCES };
	
	struct mems *cpus_expected[] = 
		{
		 NULL, /* don't care */
		};
	
	/* Activate and check */
	for (i = 0; i < 1; i++) {
		START("test-case: user privilege: %s\n", messages[i]);

		ret = ihk_reserve_mem(0, cpu_inputs[i].mem_chunks, cpu_inputs[i].num_mem_chunks);
		OKNG(ret == ret_expected[i],
		     "return value: %d, expected: %d\n",
		     ret, ret_expected[i]);

		if (cpus_expected[i]) {
			ret = cpus_check_reserved(cpus_expected[i]);
			OKNG(ret == 0, "reserved as expected\n");
			
			/* Clean up */
			ret = ihk_release_mem(0, cpu_inputs[i].mem_chunks,
					      cpu_inputs[i].num_mem_chunks);
			INTERR(ret != 0, "ihk_release_mem returned %d\n", ret);
		}
	}

	ret = 0;
 out:
	return ret;
}
