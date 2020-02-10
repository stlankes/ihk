#include <errno.h>
#include <ihklib.h>
#include "util.h"
#include "okng.h"
#include "cpu.h"
#include "params.h"
#include "init_fini.h"

const char *values[] = {
	"before insmod",
	"after insmod",
};

int main(int argc, char **argv)
{
	int ret;
	int i;

	params_getopt(argc, argv);

	/* All of McKernel CPUs */
	struct mems mems_input[2] = { 0 };

	for (i = 1; i < 2; i++) {
		ret = cpus_ls(&mems_input[i]);
		INTERR(ret, "cpus_ls returned %d\n", ret);

		ret = cpus_shift(&mems_input[i], 2);
		INTERR(ret, "cpus_shift returned %d\n", ret);
	}

	int ret_expected_reserve_cpu[] = { -ENOENT, 0 };
	int ret_expected_get_num_reserved_cpus[] = {
		-ENOENT,
		mems_input[1].ncpus
	};
	int ret_expected[] = { -ENOENT, 0 };
	struct mems *mems_expected[] = { NULL, &mems_input[1] };

	/* Activate and check */
	for (i = 0; i < 2; i++) {
		struct mems cpus;

		START("test-case: /dev/mcd0: %s\n", values[i]);

		ret = ihk_reserve_cpu(0, mems_input[i].cpus,
				      mems_input[i].ncpus);
		INTERR(ret != ret_expected_reserve_cpu[i],
		       "ihk_reserve_cpu returned %d\n", ret);

		ret = ihk_get_num_reserved_cpus(0);
		INTERR(ret != ret_expected_get_num_reserved_cpus[i],
		       "ihk_get_num_reserved_cpus returned %d\n", ret);

		if (!mems_expected[i]) {
			ret = cpus_init(&cpus, 1);
			INTERR(ret, "cpus_init returned %d\n", ret);

			ret = ihk_query_cpu(0, cpus.cpus, cpus.ncpus);
			OKNG(ret == ret_expected[i],
			     "return value: %d, expected: %d\n",
			     ret, ret_expected[i]);
		} else {
			cpus.ncpus = ret;

			ret = cpus_init(&cpus, cpus.ncpus);
			INTERR(ret, "cpus_init returned %d\n", ret);

			ret = ihk_query_cpu(0, cpus.cpus, cpus.ncpus);
			OKNG(ret == ret_expected[i],
			     "return value: %d, expected: %d\n",
			     ret, ret_expected[i]);

			ret = mems_compare(&cpus, mems_expected[i]);
			OKNG(ret == 0, "query result matches input\n");

			/* Clean up */
			ret = ihk_release_cpu(0, mems_input[i].cpus,
					      mems_input[i].ncpus);
			INTERR(ret, "ihk_release_cpu returned %d\n", ret);
		}

		/* Precondition */
		if (i == 0) {
			ret = insmod(params.uid, params.gid);
			INTERR(ret == 0, "insmod returned %d\n", ret);
		}
	}

	ret = 0;
 out:
	rmmod(0);
	return ret;
}