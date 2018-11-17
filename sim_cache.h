#ifndef SIM_CACHE_H
#define SIM_CACHE_H

typedef struct cache_params{
    unsigned long int block_size;
    unsigned long int l1_size;
    unsigned long int l1_assoc;
    unsigned long int vc_num_blocks;
    unsigned long int l2_size;
    unsigned long int l2_assoc;
}cache_params;

// Put additional data structures here as per your requirement
typedef struct block_params{
	int valid;	
	//valid = 1: valid; valid = 0: invalid;
	//set 0 so don't need to initialize cache. 
	int dirty;
	unsigned long int tag;
	unsigned long int index;
}block_params;

typedef struct counter {
	int readCounter = 0;
	int writeCounter = 0;
	int readMissCounter = 0;
	int writeMissCounter = 0;
	int swaps = 0;
	int writeBack = 0;
	int swapRequest = 0;


}counter;

class Cache2 {
public:
	unsigned long int blockSize;
	unsigned long int size;
	unsigned long int assoc;

	unsigned long int address;
	unsigned long int index;
	unsigned long int tag;

	int numSet; // how many sets in total
	int bitIndex;
	int bitOffset;
	int bitTag;

	//double missRate;


	//counter
	counter counter2;


	//build constructor
	Cache2(unsigned long int, unsigned long int, unsigned long int);

	//initialize cache
	block_params **cache;
	//void initCache1();
	//read & write
	void function(char rw, unsigned long int address);

	//LRU
	int **lru;
	//void initLRU();
	void missUpdateLRU(unsigned long int index, int numWay);
	void hitUpdateLRU(unsigned long int index, int numWay);

	void printCache();

	int getLRU(unsigned long int index);	//return Way#
	void printCounter();
};

class Cache1 {
public:
	unsigned long int blockSize;
	unsigned long int size;
	unsigned long int assoc;
	unsigned long int vc_blocks;
	int enable;

	unsigned long int address;
	unsigned long int index;
	unsigned long int tag;


	int numSet; // how many sets in total
	int bitIndex;
	int bitOffset;
	int bitTag;
	int bitTagVC;

	//double swapRequestRate;
	//double missRate;


	//counter
	counter counter1;


	//build constructor
	Cache1(unsigned long int, unsigned long int, unsigned long int, unsigned long int, int);
	
	//initialize cache
	block_params **cache;
	//void initCache1();
	//read & write
	void function(char rw, unsigned long int address, Cache2 &L2);

	//LRU
	int **lru;
	//void initLRU();
	void hitUpdateLRU(unsigned long int index , int numWay);
	void missUpdateLRU(unsigned long int index, int nummWay);

	//Vitim Cache
	block_params *vc;
	int *vc_lru;
	//void initVC_LRU();
	void hitUpdateVC_LRU(int numWay);
	void missUpdateVC_LRU(int numWay);
	void printCache();
	int getLRU(unsigned long int index);	//return Way#
	int getVC_LRU();
	void printVC();
	void printCounter();

};



#endif