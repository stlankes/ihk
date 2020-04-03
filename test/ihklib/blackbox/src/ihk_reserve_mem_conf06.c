#include <limits.h>
#include <errno.h>
#include <ihklib.h>
#include "util.h"
#include "okng.h"
#include "mem.h"
#include "params.h"
#include "linux.h"

const char param[] = "user privilege";
const char *values[] = {
	"non-root"
};

int main(int argc, char **argv)
{
	int ret;
	int i;
	int opt;

	params_getopt(argc, argv);

	int mem_conf_keys[1] = {
		IHK_RESERVE_MEM_MAX_SIZE_RATIO_ALL,
	};

	int mem_conf_values[1] = {
		95
	};

	int ret_expected[1] = {
		-EACCES,
	};

	while ((opt = getopt(argc, argv, "ir")) != -1) {
		switch (opt) {
		case 'i':
			ret = linux_insmod(0);
			INTERR(ret, "linux_insmod returned %d\n", ret);
			exit(0);
			break;
		case 'r':
			ret = linux_rmmod(1);
			INTERR(ret, "rmmod returned %d\n", ret);
			exit(0);
			break;
		default: /* '?' */
			printf("unknown option %c\n", optopt);
			exit(1);
		}
	}

	/* Activate and check */
	for (i = 0; i < 1; i++) {
		START("test-case: %s: %s\n", param, values[i]);

		ret = ihk_reserve_mem_conf(0, mem_conf_keys[i],
					   &mem_conf_values[i]);
		OKNG(ret == ret_expected[i],
		     "return value: %d, expected: %d\n",
		     ret, ret_expected[i]);
	}

	ret = 0;
 out:
	return ret;
}
