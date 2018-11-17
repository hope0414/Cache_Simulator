/*
*FILENAME   :   cache1.cc
*DESCRIPTION    :   cache Layer 1 design

*AUTHOR     :       Haopeng Liu
*START DATE     :       9/21/2018
*/
#include "sim_cache.h"
#include <math.h>
#include <iostream>
#include <iomanip>
using namespace std;

Cache2::Cache2(unsigned long int block_size, unsigned long int l2_size, unsigned long int l2_assoc) {
	blockSize = block_size;
	size = l2_size;
	assoc = l2_assoc;

	//counter2 = { 0, 0, 0, 0 , 0, 0, 0};

	/*all addresses are 32 bits*/
	unsigned long int bitAddress = 32;


	/*check if it is a fully-associative cache */
	if (assoc == (size / blockSize))
	{
		numSet = 1;
		bitIndex = 0;
	}
	else {
		numSet = (size / (blockSize * assoc));
		bitIndex = log2(numSet);
	}

	bitOffset = log2(blockSize);
	bitTag = bitAddress - bitOffset - bitIndex;

	cache = new block_params *[numSet];
	for (int i = 0; i < numSet; i++) {
		cache[i] = new block_params[assoc];
		for (int j = 0; j < assoc; j++) {
			cache[i][j].valid = 0;
			cache[i][j].dirty = 0;
		}

	}


	/*initialize LRU*/
	//int lru[numSet][assoc];
	lru = new int *[numSet];
	for (int i = 0; i < numSet; i++) {
		lru[i] = new int[assoc];
		for (int j = 0; j < assoc; j++) {
			lru[i][j] = 0;
		}

	}


}

void Cache2::printCache() {
	unsigned long int tempLRU;
	int tempDirty;
	int tempValid;
	unsigned long int tempTag;
	unsigned long int tempIndex;

	cout << '\n';
	cout << "===== L2 contents =====" << endl;

	//sort every set according to lru: MRU -> LRU
	for(int m = 0; m < numSet; m++){
		for(int  n = assoc - 1; n > 0; n--){
			for(int k = n - 1; k >= 0; k--){
				if(lru[m][n] < lru[m][k]){
					tempLRU = lru[m][n];
					lru[m][n] = lru[m][k];
					lru[m][k] = tempLRU;

					tempTag = cache[m][n].tag;
					tempIndex = cache[m][n].index;
					tempDirty = cache[m][n].dirty;	
					tempValid = cache[m][n].valid;
	
					cache[m][n].tag = cache[m][k].tag;
					cache[m][n].index = cache[m][k].index;
					cache[m][n].dirty = cache[m][k].dirty;
					cache[m][n].valid = cache[m][k].valid;

					cache[m][k].tag = tempTag;
					cache[m][k].index = tempIndex;
					cache[m][k].dirty = tempDirty;
					cache[m][k].valid = tempValid;


				}
			}

		}
	}


	for (int i = 0; i < numSet; i++) {
		cout << "  set " << setw(3) << dec << i << ":   ";
		for (int j = 0; j < assoc; j++) {
			cout << hex << cache[i][j].tag;
			if (cache[i][j].dirty == 1)
				cout  << " D  ";
			else {
				cout << setw(4) << ' ';
			}
		}
		cout << '\n';
	}
//	cout << '\n';
}

int Cache2::getLRU(unsigned long int index) {
	int tmpValue = lru[index][0];
	int tmpWay = 0;
	for (int i = 0; i < assoc - 1; i++) {
		if (tmpValue < lru[index][i + 1]) {
			tmpValue = lru[index][i + 1];
			tmpWay = i + 1;
		}
	}
	return tmpWay;
}



//update LRU
void Cache2::hitUpdateLRU(unsigned long int index, int numWay) {
	int value = 0;
	value = lru[index][numWay];
	lru[index][numWay] = 0;
	for (int i = 0; i < assoc; i++) {
		if (i != numWay && lru[index][i] <= value && cache[index][i].valid) {
			lru[index][i]++;
		}
	}
}

void Cache2::missUpdateLRU(unsigned long int index, int numWay) {
	lru[index][numWay] = 0;
	for (int i = 0; i < assoc; i++) {
		if (i != numWay) {
			lru[index][i]++;
		}
	}
}

//-----------------------Cache L2-----------------------
void Cache2::function(char rw, unsigned long int address)
{
	int hit = 0;
	int foundBlock = 0;
	unsigned long int wayFound;
	unsigned long int mask = 1;
	unsigned long int tagAndIndex = (address >> bitOffset);


	//index value
	if (rw == 'r') {
		if (assoc == (size / blockSize))
		{
			index = 0;
		}
		else {
			mask = (mask << bitIndex) - 1;
			index = mask & tagAndIndex;
		}


		//tag value
		tag = address >> (bitOffset + bitIndex);

		//increase counter
		counter2.readCounter++;

		//search cache;
		for (int i = 0; i < assoc; i++) {
			//hit?
			if ((cache[index][i].valid == 1) && (cache[index][i].tag == tag)) {
				hit = 1;
				wayFound = i;
				break;
			}
			//else hit = 0;

		}
		//update LRU
		if (hit == 1) {
			hitUpdateLRU(index, wayFound);
		}
		else {
			//increase readMissCounter
			counter2.readMissCounter++;
			//finding invalid block
			for (int i = 0; i < assoc; i++) {
				if (cache[index][i].valid == 0) {
					foundBlock = 1;
					wayFound = i;
					break;
				}
				else {
					foundBlock = 0;
				}
			}

			//found invalid block <- requested block
			if (foundBlock == 1) {
				cache[index][wayFound].tag = tag;
				cache[index][wayFound].valid = 1;
				cache[index][wayFound].dirty = 0;
				cache[index][wayFound].index = index;

				//update LRU
				missUpdateLRU(index, wayFound);

			}
			//cache2 is full
			else {	
			
				int blockNum = getLRU(index);
				
				if(cache[index][blockNum].dirty == 1){
					counter2.writeBack++;
				}

				cache[index][blockNum].tag = tag;
				cache[index][blockNum].index = index;
				cache[index][blockNum].dirty = 0;
				cache[index][blockNum].valid = 1;

				missUpdateLRU(index, blockNum);
				
			}
		}
	}
	// wirte request
	else {
		if (assoc == (size / blockSize))
		{
			index = 0;
		}
		else {
			mask = (mask << bitIndex) - 1;
			index = mask & tagAndIndex;
		}


		//tag value
		tag = address >> (bitOffset + bitIndex);

		//increase writeCounter
		counter2.writeCounter++;

		//search cache;
		for (int i = 0; i < assoc; i++) {
			//hit?
			if ((cache[index][i].valid == 1) && (cache[index][i].tag == tag)) {
				hit = 1;
				wayFound = i;
				break;
			}
			//else hit = 0;

		}
		//update LRU
		if (hit == 1) {
			cache[index][wayFound].dirty = 1;
			hitUpdateLRU(index, wayFound);
		}
		else {
			//increase writeMissCounter
			counter2.writeMissCounter++;

			//finding invalid block
			for (int i = 0; i < assoc; i++) {
				if (cache[index][i].valid == 0) {
					foundBlock = 1;
					wayFound = i;
					break;
				}
				else {
					foundBlock = 0;
				}
			}

			//found invalid block <- requested block
			if (foundBlock == 1) {
				cache[index][wayFound].tag = tag;
				cache[index][wayFound].valid = 1;
				cache[index][wayFound].dirty = 1;
				cache[index][wayFound].index = index;

				//update LRU
				missUpdateLRU(index, wayFound);

			}
			//L2 is full
			else {
				int blockNum = getLRU(index);
				if (cache[index][blockNum].dirty == 0) {
					cache[index][blockNum].tag = tag;
					cache[index][blockNum].index = index;
					cache[index][blockNum].dirty = 1;
					cache[index][blockNum].valid = 1;

					missUpdateLRU(index, blockNum);
				}
				else {
					counter2.writeBack++;
					cache[index][blockNum].tag = tag;
					cache[index][blockNum].index = index;
					cache[index][blockNum].dirty = 1;
					cache[index][blockNum].valid = 1;

					missUpdateLRU(index, blockNum);
				}
			}
		}
	}
}

void Cache2::printCounter(){
	//double missRate = double (counter2.readMissCounter) / (counter2.readCounter);
	cout << "  j. number of L2 reads:" << setw(28) << dec << counter2.readCounter << endl;
	cout << "  k. number of L2 read misses:" << setw(22) << dec << counter2.readMissCounter << endl;
	cout << "  l. number of L2 writes:" << setw(27) << dec << counter2.writeCounter << endl;
	cout << "  m. number of L2 write misses:" << setw(21) << dec << counter2.writeMissCounter << endl;
	if( counter2.readMissCounter == 0){
		cout << "  n. L2 miss rate:" << setw(34) << "0.0000" << endl;
	}
	else{	
		double missRate = double (counter2.readMissCounter) / (counter2.readCounter);
		cout << "  n. L2 miss rate:" << setw(34) << fixed << setprecision(4) << missRate << endl;
	}
	cout << "  o. number of writebacks from L2:" << setw(18) << dec << counter2.writeBack << endl; 
}



