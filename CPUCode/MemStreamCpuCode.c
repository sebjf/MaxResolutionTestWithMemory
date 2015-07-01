#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int main(void)
{
	const int size = 384;
	int sizeBytes = size * sizeof(uint32_t);
	uint32_t *x = malloc(sizeBytes);
	uint32_t *y = malloc(sizeBytes);
	uint32_t *s = malloc(sizeBytes);
	int scalar = 3;

	// TODO Generate input data
	for(int i=0; i<size; ++i) {
		x[i] = random() % 100;
		y[i] = random() % 100;
	}
	
	max_file_t *maxfile = MemStream_init();
	max_engine_t *engine = max_load(maxfile, "*");

	printf("Writing to LMem.\n");
	max_actions_t* act = max_actions_init(maxfile, "writeLMem");
	max_set_param_uint64t(act, "address", 0);
	max_set_param_uint64t(act, "nbytes", sizeBytes);
	max_queue_input(act, "cpu_to_lmem", x, size * sizeof(uint32_t));
	//max_ignore_kernel("MaxResolutionTest");
	max_run(engine, act);



	printf("Running on DFE.\n");
	act = max_actions_init(maxfile, "default");
	max_set_param_uint64t(act, "N", size);
	max_set_param_uint64t(act, "A", scalar);

	max_set_uint64t(act,"MaxVideoSignalKernel","HSyncPolarity",1);
		max_set_uint64t(act,"MaxVideoSignalKernel","VSyncPolarity",1);

		max_set_uint64t(act,"MaxResolutionTestKernel","pattern",0);
		max_set_uint64t(act,"MaxResolutionTestKernel","r",127);
		max_set_uint64t(act,"MaxResolutionTestKernel","g",60);
		max_set_uint64t(act,"MaxResolutionTestKernel","b",0);


	max_queue_input(act, "y", y, size * sizeof(uint32_t));
	max_queue_output(act, "s", s, size * sizeof(uint32_t));
	max_run(engine, act);
	max_unload(engine);
	
	// TODO Use result data
	for(int i = 0; i < size; ++i)
		if (s[i] != x[i] + y[i] + scalar)
			return 1;

	printf("Done.\n");
	return 0;
}
