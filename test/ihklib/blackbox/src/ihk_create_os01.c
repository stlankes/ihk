#include <errno.h>
#include <ihklib.h>
#include "util.h"
#include "okng.h"
#include "params.h"
#include "init_fini.h"
#include <unistd.h>

const char param[] = "/dev/mcd0";
const char *values[] = {
	"/dev/mcd0 exists",
	"No /dev/mcd0 exists"
};

int main(int argc, char **argv) 
{
	int ret;
	int i;

	params_getopt(argc, argv);
	
	ret = insmod(params.uid, params.gid);
	INTERR(ret, "insmod returned %d\n", ret);
	
	int ret_expected[2] = { 0, -ENOENT }; 
	int ret_expected_os_instances[2] = { 1, -ENOENT }; 

/*	for (i = 0; i < 2; i++) {
		ret = ihk_create_os(0);
		OKNG(ret == ret_expected[i],
		     "return value: %d, expected: %d\n",
		     ret, ret_expected[i]);
		
		ret = ihk_get_num_os_instances(0);
		OKNG(ret == ret_expected_os_instances[i], 
				"create os instance as expected\n");
		
		if (i == 0) rmmod(0);
	}
*/	

out:
	return ret;
}
