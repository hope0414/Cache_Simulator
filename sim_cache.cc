/*
*FILENAME   :   sim_cache.cc
*DESCRIPTION    :   cache design

*AUTHOR     :       Haopeng Liu        
*START DATE     :       9/15/2018
*/

#include <stdio.h>
#include <stdlib.h>
#include "sim_cache.h"
#include <iomanip>
#include <math.h>
using namespace std;

/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim_cache 32 8192 4 7 262144 8 gcc_trace.txt
    argc = 8
    argv[0] = "sim_cache"
    argv[1] = "32"
    argv[2] = "8192"
    ... and so on
*/
int main(int argc, char* argv[])
{
	FILE *FP;               // File handler
	char *trace_file;       // Variable that holds trace file name;
	cache_params params;    // look at sim_cache.h header file for the the definition of struct cache_params
	char rw;                // variable holds read/write type read from input file. The array size is 2 because it holds 'r' or 'w' and '\0'. Make sure to adapt in future projects
	unsigned long int addr; // Variable holds the address read from input file

	if (argc != 8)           // Checks if correct number of inputs have been given. Throw error and exit if wrong
	{
		printf("Error: Expected inputs:7 Given inputs:%d\n", argc - 1);
		exit(EXIT_FAILURE);
	}

	// strtoul() converts char* to unsigned long. It is included in <stdlib.h>
	params.block_size = strtoul(argv[1], NULL, 10);
	params.l1_size = strtoul(argv[2], NULL, 10);
	params.l1_assoc = strtoul(argv[3], NULL, 10);
	params.vc_num_blocks = strtoul(argv[4], NULL, 10);
	params.l2_size = strtoul(argv[5], NULL, 10);
	params.l2_assoc = strtoul(argv[6], NULL, 10);
	trace_file = argv[7];

	//enable signals for VC and L2
	int vcEnable = 0;
	int l2Enable = 0;

	if (params.vc_num_blocks != 0)
		vcEnable = 1;                               //when VC is enabled

	if (params.l2_size != 0)
		l2Enable = 1;

	// Open trace_file in read mode
	FP = fopen(trace_file, "r");
	if (FP == NULL)
	{
		// Throw error and exit if fopen() failed
		printf("Error: Unable to open file %s\n", trace_file);
		exit(EXIT_FAILURE);
	}

	// Print params
	printf("===== Simulator configuration =====\n"
		"  BLOCKSIZE:%20lu\n"
		"  L1_SIZE:%22lu\n"
		"  L1_ASSOC:%21lu\n"
		"  VC_NUM_BLOCKS:%16lu\n"
		"  L2_SIZE:%22lu\n"
		"  L2_ASSOC:%21lu\n"
		"  trace_file:%19s\n", params.block_size, params.l1_size, params.l1_assoc, params.vc_num_blocks, params.l2_size, params.l2_assoc, trace_file);

	char str[2];

	//create L1 with BLOCKSIZE, L1_SIZE, L1_ASSOC, VC_NUM_BLOCKS and L2Enable
	Cache1 L1(params.block_size, params.l1_size, params.l1_assoc, params.vc_num_blocks, l2Enable);

	//create L2 with b,lock_size, L2_SIZE, L2_ASSOC
	Cache2 L2(params.block_size, params.l2_size, params.l2_assoc);

	while (fscanf(FP, "%s %lx", str, &addr) != EOF)
	{
		rw = str[0];
		//if (rw == 'r')
		//	printf("%s %lx\n", "read", addr);           // Print and test if file is read correctly
		//else if (rw == 'w')
		//	printf("%s %lx\n", "write", addr);          // Print and test if file is read correctly
		                     //when L2 is enabled
				
		L1.function(rw, addr, L2);

		
	}

	L1.printCache();

	if (vcEnable == 1) {
		L1.printVC();
	}

	if (l2Enable == 1) {
		L2.printCache();
	}

	L1.printCounter();
	L2.printCounter();


	//double missRate;
	//missRate = double (L2.counter2.readMissCounter / L2.counter2.readCounter);

	//printf("n. L2 miss rate:");
	//printf("%34.4f\n", missRate); 
	

	//cout << "n. L2 miss rate:" << setw(34) << fixed << setprecision(4) << missRate << endl;
	//cout << "o. number of writebacks from L2:" << setw(18) << L2.counter2.writeBack << endl; 
	
	if( l2Enable == 0 ){
		printf("  p. total memory traffic:%26d", (L1.counter1.readMissCounter + L1.counter1.writeMissCounter + L1.counter1.writeBack - L1.counter1.swaps));
		printf("\n");

	}
	else{
		printf("  p. total memory traffic:%26d", (L2.counter2.readMissCounter + L2.counter2.writeMissCounter + L2.counter2.writeBack - L2.counter2.swaps));
		printf("\n");

	}


	return 0;
}
