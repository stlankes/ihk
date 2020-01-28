#ifndef __INPUT_VECTOR_H_INCLUDED__
#define __INPUT_VECTOR_H_INCLUDED__

#define MAX_NUM_CPUS 272

struct cpus {
	int *cpus;
	int ncpus;
};

int cpus_init(struct cpus *cpus, int ncpus);
int cpus_copy(struct cpus *dst, struct cpus *src);
int cpus_ls(struct cpus *cpus);
int cpus_push(struct cpus *cpus, int id);
int cpus_pop(struct cpus *cpus, int n);
int cpus_shift(struct cpus *cpus, int n);
void cpus_dump(struct cpus *cpus);
int cpus_compare(struct cpus *cpus_result, struct cpus *cpus_expected);
int cpus_check_reserved(struct cpus *expected);

#endif
