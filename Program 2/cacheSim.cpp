#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

bool fileExists(string fileName){
  const char * aFile = fileName.c_str();
  ifstream infile(aFile);
  return infile.good();
}

typedef struct directEntry{
  int validBit;
  int tag;

}directEntry;

typedef struct setValue{
  int tag;
  int used;
  int validBit;
}setValue;

typedef struct setEntry2{
 setValue lines[2];
}setEntry2;

typedef struct setEntry4{
 setValue lines[4];
}setEntry4;

typedef struct setEntry8{
 setValue lines[8];
}setEntry8;

typedef struct setEntry16{
 setValue lines[16];
}setEntry16;

typedef struct fullEntryLRU{
    int address;
    int validBit;
    int used;
}fullEntryLRU;

typedef struct fullEntryHotCold{
    int address;
    int validBit;
}fullEntryHotCold;

void directMapCache(directEntry * cache, string str, int size, int& hits, int& accesses){
    char command = str[0];
    int convert = 0;
    string address = str.substr(2, str.length()-1);
    stringstream ss;
    ss << hex << address;
    ss >> convert;
    //cout << "converted int value: " << convert << endl;
    convert /= 32;
    int index = convert % size;
    int newTag = convert / size;
    //cout << "orig address: " << str << endl;
    //cout << "address: " << convert << endl;
    //cout << "index: " << index << endl;
    //cout << "tag: " << newTag << " -> ";

    if(cache[index].tag == newTag && cache[index].validBit == 1){
       // cout << "hit" << endl;
        hits++;
        accesses++;
    }
    else {
	/*cout << "[";
	for(int i = 0; i < 31; i++){
		cout << cache[i].tag << ", ";
	}
	cout << cache[31].tag;
	cout << " ]\n";
    	cout << cache[index].tag << " != " << newTag << endl;
        */
        accesses++;
        cache[index].validBit = 1;
        cache[index].tag = newTag;
    }
	//cout << "hits: " << hits << endl;

}

void setAssoCache2(setEntry2 * cache, string str, int size, int& hits, int& accesses, char in = 'L'){
    char command = str[0];
    int convert = 0;
    int i;
    string address = str.substr(2, str.length()-1);
    stringstream ss;
    ss << hex << address;
    ss >> convert;
    convert /= 32;
	if(in == 'P'){
		convert++;
	}
    int numSets = 256;
	int index = 0;
    //cout << "address: " << convert << endl;
	index = (convert%256);
    //cout << "index: " << index << endl;
    int tagFound = 0;
    int emptyIndex = -1;
    int min = cache[index].lines[0].used;
    int lruIndex = -1;

    //cout << "size: " << size << endl;
    int newTag = convert / size;
	accesses++;


	for(i = 0; i < 2; i++){
		cache[index].lines[i].used++;
	}


	    for(i = 0; i < 2; i++){
        if(newTag == cache[index].lines[i].tag && cache[index].lines[i].validBit == 1){  //tag found within line of set
		if(in != 'P'){
			hits++;
		}
		//cout << "hit on index " << index << " place " << i << endl;
            cache[index].lines[i].used = 0;
            //accesses++;
		return;
        } else if(cache[index].lines[i].validBit == 0){   //checks for empty slot within line of set
            //miss = 1;
            cache[index].lines[i].tag = newTag;
           cache[index].lines[i].validBit = 1;
	            cache[index].lines[i].used = 0;
		return;
        }
    }
    /*if(tagFound != 1){   //tag wasn't found within line of set
        if(emptyIndex > -1){  //empty slot found
           cache[index].lines[emptyIndex].tag = newTag;
           cache[index].lines[emptyIndex].validBit = 1;
          // cache[index].lines[emptyIndex].used++;
        }
        else if(emptyIndex == -1){   //line needs to be evicted
            for(i = 0; i < 2; i++){
                if(min > cache[index].lines[i].used){
                    min = cache[index].lines[i].used;
                    lruIndex = i;
                }
            }
            cache[index].lines[lruIndex].tag = newTag;
            cache[index].lines[lruIndex].validBit = 1;
            //cache[index].lines[lruIndex].used++;
        }
        //accesses++;
    }*/
	int min_index = 0;
       // miss = 1;
	for(i = 0; i < 2; i++){
		if(cache[index].lines[i].used > min){
			min = cache[index].lines[i].used;
			min_index = i;
		}
	}
	cache[index].lines[min_index].used = 0;
	cache[index].lines[min_index].tag = newTag;
	cache[index].lines[min_index].validBit = 1;
}

void setAssoCache4(setEntry4 * cache, string str, int size, int& hits, int& accesses, char in = 'L'){
    char command = str[0];
    int convert = 0;
    int i;
    string address = str.substr(2, str.length()-1);
    stringstream ss;
    ss << hex << address;
    ss >> convert;
    convert /= 32;
    int numSets = 128;
    if(in == 'P'){
        convert++;
    }
    //cout << "address: " << convert << endl;
    int index = (convert%128);
    //cout << "index: " << index << endl;
    int tagFound = 0;
    int emptyIndex = -1;
    int min = cache[index].lines[0].used;
    int lruIndex = -1;

    //cout << "size: " << size << endl;
    int newTag = convert / size;
	accesses++;


	for(i = 0; i < 4; i++){
		cache[index].lines[i].used++;
	}


	    for(i = 0; i < 4; i++){
        if(newTag == cache[index].lines[i].tag && cache[index].lines[i].validBit == 1){  //tag found within line of set
              if(in != 'P'){
                 hits++;
	      }
            //cout << "hit on index " << index << " place " << i << endl;
            cache[index].lines[i].used = 0;
            //accesses++;
		return;
        } else if(cache[index].lines[i].validBit == 0){   //checks for empty slot within line of set
            cache[index].lines[i].tag = newTag;
           cache[index].lines[i].validBit = 1;
	            cache[index].lines[i].used = 0;
		return;
        }
    }
	int min_index = 0;

	for(i = 0; i < 4; i++){
		if(cache[index].lines[i].used > min){
			min = cache[index].lines[i].used;
			min_index = i;
		}
	}
	cache[index].lines[min_index].used = 0;
	cache[index].lines[min_index].tag = newTag;
	cache[index].lines[min_index].validBit = 1;


}

void setAssoCache8(setEntry8 * cache, string str, int size, int& hits, int& accesses, char in = 'L'){
    char command = str[0];
    int convert = 0;
    int i;
    string address = str.substr(2, str.length()-1);
    stringstream ss;
    ss << hex << address;
    ss >> convert;
    convert /= 32;
    int numSets = 64;
    //cout << "address: " << convert << endl;
    if(in == 'P'){
        convert++;
    }
    int index = (convert%64);
    //cout << "index: " << index << endl;
    int tagFound = 0;
    int emptyIndex = -1;
    int min = cache[index].lines[0].used;
    int lruIndex = -1;

    //cout << "size: " << size << endl;
    int newTag = convert / size;
	accesses++;


	for(i = 0; i < 8; i++){
		cache[index].lines[i].used++;
	}


	    for(i = 0; i < 8; i++){
        if(newTag == cache[index].lines[i].tag && cache[index].lines[i].validBit == 1){  //tag found within line of set
             if(in != 'P'){
                hits++;
             }
		//cout << "hit on index " << index << " place " << i << endl;
            cache[index].lines[i].used = 0;
            //accesses++;
		return;
        } else if(cache[index].lines[i].validBit == 0){   //checks for empty slot within line of set
            cache[index].lines[i].tag = newTag;
           cache[index].lines[i].validBit = 1;
	            cache[index].lines[i].used = 0;
		return;
        }
    }
	int min_index = 0;

	for(i = 0; i < 8; i++){
		if(cache[index].lines[i].used > min){
			min = cache[index].lines[i].used;
			min_index = i;
		}
	}
	cache[index].lines[min_index].used = 0;
	cache[index].lines[min_index].tag = newTag;
	cache[index].lines[min_index].validBit = 1;


}

void setAssoCache16(setEntry16 * cache, string str, int size, int& hits, int& accesses, char in = 'L'){
    char command = str[0];
    int convert = 0;
    int i;
    string address = str.substr(2, str.length()-1);
    stringstream ss;
    ss << hex << address;
    ss >> convert;
    convert /= 32;
    int numSets = 32;
    if(in == 'P'){
        convert++;
    }
    //cout << "address: " << convert << endl;
    int index = (convert%32);
    //cout << "index: " << index << endl;
    int tagFound = 0;
    int emptyIndex = -1;
    int min = cache[index].lines[0].used;
    int lruIndex = -1;

    //cout << "size: " << size << endl;
    int newTag = convert / size;
	accesses++;


	for(i = 0; i < 16; i++){
		cache[index].lines[i].used++;
	}


	    for(i = 0; i < 16; i++){
        if(newTag == cache[index].lines[i].tag && cache[index].lines[i].validBit == 1){  //tag found within line of set
             if(in != 'P'){
                 hits++;
             }
		//cout << "hit on index " << index << " place " << i << endl;
            cache[index].lines[i].used = 0;
            //accesses++;
		return;
        } else if(cache[index].lines[i].validBit == 0){   //checks for empty slot within line of set
            cache[index].lines[i].tag = newTag;
           cache[index].lines[i].validBit = 1;
	            cache[index].lines[i].used = 0;
		return;
        }
    }
	int min_index = 0;

	for(i = 0; i < 16; i++){
		if(cache[index].lines[i].used > min){
			min = cache[index].lines[i].used;
			min_index = i;
		}
	}
	cache[index].lines[min_index].used = 0;
	cache[index].lines[min_index].tag = newTag;
	cache[index].lines[min_index].validBit = 1;


}

void fullyAssoLRU(fullEntryLRU * cache, string str, int& hits, int&accesses){
    char command = str[0];
    int convert = 0;
    int i,j;
    string address = str.substr(2, str.length()-1);
    stringstream ss;
    ss << hex << address;
    ss >> convert;
    convert /= 32;
    accesses++;
    int max = cache[0].used;
    for(i = 0; i < 512; i++){
        cache[i].used++;
    }
    for(i = 0; i < 512; i++){
        if(cache[i].address == convert && cache[i].validBit == 1){
            hits++;
            cache[i].used = 0;
            return;
        }
    }
    for(i = 0; i < 512; i++){        //find available spot in cache
        if(cache[i].validBit == 0){
            cache[i].address = convert;
            cache[i].validBit = 1;
            cache[i].used = 0;
            return;
        }
    }

    int min_index = 0;            //no available space in cache
    for(i = 0; i < 512; i++){
        if(max < cache[i].used){
            max = cache[i].used;
            min_index = i;
        }
    }
    cache[min_index].used = 0;
    cache[min_index].address = convert;
    cache[min_index].validBit = 1;
}

void fullyAssoHotCold(fullEntryHotCold * cache, string str, int * tree, int& hits, int& accesses){
    char command = str[0];
    int convert = 0;
    int found = 0;
    int i, j, k;
    int foundIndex = -1;
    string address = str.substr(2, str.length()-1);
    stringstream ss;
    ss << hex << address;
    ss >> convert;
    convert /= 32;
    for(i = 0; i < 512; i++){  //check if value is in cache
        if(convert == cache[i].address && cache[i].validBit == 1){
            hits++;
            foundIndex = i;
            found = 1;
            break;
        }
    }
    if(found == 1){  //update hot-cold tree
        int cacheIndex = -1;
        if(foundIndex % 2 == 0){  //in an even index in cache
            cacheIndex = 0;
        }
        if(foundIndex % 2 != 0){   //in an odd index in cache
            cacheIndex = 1;
        }
        foundIndex /= 2;   //convert index to find parent
        foundIndex += 255;
        j = foundIndex;
        if(cacheIndex == 0){   //most LRU is right child
            tree[j] = 1;
        }
        else if(cacheIndex == 1){   //most LRU is left child
            tree[j] = 0;
        }
        while(j != 0){
            if(j % 2 != 0){   //odd index
               if(tree[(j-1)/2] == 0){    //most LRU is from child's path - incorrect
                   tree[(j-1)/2] = 1;
               }
               j = (j-1)/2; //set to parent
            }
            else if(j % 2 == 0){ //even index
               if(tree[(j/2)-1] == 1){      //most LRU is from child's path --> incorrect
                   tree[(j/2)-1] = 0;
               }
               j = (j/2)-1; //set to parent
            }
        }

    }
    else if(found == 0){   //value not found in cache
        j = 0;
        for(k = 0; k < 8; k++){
            if(tree[j] == 0){  //visit left side
                tree[j] = 1;
                j = (2*j)+1;
                //tree[j] = 1;
            }
            else if(tree[j] == 1){   //visit right side
                 tree[j] = 0;
                j = 2*(j+1);
                //tree[j] = 0;
            }
        }
        int lruValue = tree[j];
        int index = j;
        j -= 255;
        if(lruValue == 0){   //visit left child
            cache[2*j].address = convert;
            cache[2*j].validBit = 1;
            tree[index] = 1;
        }
        else if(lruValue == 1){   //visit right child
            cache[(2*j)+1].address = convert;
            cache[(2*j)+1].validBit = 1;
            tree[index] = 0;
        }

    }
    accesses++;
}

void writeMiss2(setEntry2 * cache, string str, int size, int& hits, int& accesses){
    char command = str[0];
    int convert = 0;
    int i;
    string address = str.substr(2, str.length()-1);
    stringstream ss;
    ss << hex << address;
    ss >> convert;
    convert /= 32;
    int numSets = 256;
    //cout << "address: " << convert << endl;
    int index = (convert%256);
    //cout << "index: " << index << endl;
    int tagFound = 0;
    int emptyIndex = -1;
    int min = cache[index].lines[0].used;
    int lruIndex = -1;

    //cout << "size: " << size << endl;
    int newTag = convert / size;
	accesses++;


	for(i = 0; i < 2; i++){
		cache[index].lines[i].used++;
	}


	    for(i = 0; i < 2; i++){
        if(newTag == cache[index].lines[i].tag && cache[index].lines[i].validBit == 1){  //tag found within line of set
            hits++;
		//cout << "hit on index " << index << " place " << i << endl;
            cache[index].lines[i].used = 0;
            //accesses++;
		return;
        } else if(cache[index].lines[i].validBit == 0){   //checks for empty slot within line of set
            if(command != 'S'){   //don't write to cache on STORE-miss
                cache[index].lines[i].tag = newTag;
                cache[index].lines[i].validBit = 1;
	        cache[index].lines[i].used = 0;
            }
		return;
        }
    }
	int min_index = 0;
      if(command != 'S'){      //only store value in cache during load instruction
	for(i = 0; i < 2; i++){
		if(cache[index].lines[i].used > min){
			min = cache[index].lines[i].used;
			min_index = i;
		}
	}
	cache[index].lines[min_index].used = 0;
	cache[index].lines[min_index].tag = newTag;
	cache[index].lines[min_index].validBit = 1;
      }
}

void writeMiss4(setEntry4 * cache, string str, int size, int& hits, int& accesses){
    char command = str[0];
    int convert = 0;
    int i;
    string address = str.substr(2, str.length()-1);
    stringstream ss;
    ss << hex << address;
    ss >> convert;
    convert /= 32;
    int numSets = 128;
    //cout << "address: " << convert << endl;
    int index = (convert%128);
    //cout << "index: " << index << endl;
    int tagFound = 0;
    int emptyIndex = -1;
    int min = cache[index].lines[0].used;
    int lruIndex = -1;

    //cout << "size: " << size << endl;
    int newTag = convert / size;
	accesses++;


	for(i = 0; i < 4; i++){
		cache[index].lines[i].used++;
	}


	    for(i = 0; i < 4; i++){
        if(newTag == cache[index].lines[i].tag && cache[index].lines[i].validBit == 1){  //tag found within line of set
            hits++;
		//cout << "hit on index " << index << " place " << i << endl;
            cache[index].lines[i].used = 0;
            //accesses++;
		return;
        } else if(cache[index].lines[i].validBit == 0){   //checks for empty slot within line of set
            if(command != 'S'){   //don't write to cache on STORE-miss
                cache[index].lines[i].tag = newTag;
                cache[index].lines[i].validBit = 1;
	        cache[index].lines[i].used = 0;
            }
		return;
        }
    }
	int min_index = 0;
      if(command != 'S'){      //only store value in cache during load instruction
	for(i = 0; i < 4; i++){
		if(cache[index].lines[i].used > min){
			min = cache[index].lines[i].used;
			min_index = i;
		}
	}
	cache[index].lines[min_index].used = 0;
	cache[index].lines[min_index].tag = newTag;
	cache[index].lines[min_index].validBit = 1;
      }
}

void writeMiss8(setEntry8 * cache, string str, int size, int& hits, int& accesses){
    char command = str[0];
    int convert = 0;
    int i;
    string address = str.substr(2, str.length()-1);
    stringstream ss;
    ss << hex << address;
    ss >> convert;
    convert /= 32;
    int numSets = 64;
    //cout << "address: " << convert << endl;
    int index = (convert%64);
    //cout << "index: " << index << endl;
    int tagFound = 0;
    int emptyIndex = -1;
    int min = cache[index].lines[0].used;
    int lruIndex = -1;

    //cout << "size: " << size << endl;
    int newTag = convert / size;
	accesses++;


	for(i = 0; i < 8; i++){
		cache[index].lines[i].used++;
	}


	    for(i = 0; i < 8; i++){
        if(newTag == cache[index].lines[i].tag && cache[index].lines[i].validBit == 1){  //tag found within line of set
            hits++;
		//cout << "hit on index " << index << " place " << i << endl;
            cache[index].lines[i].used = 0;
            //accesses++;
		return;
        } else if(cache[index].lines[i].validBit == 0){   //checks for empty slot within line of set
            if(command != 'S'){   //don't write to cache on STORE-miss
                cache[index].lines[i].tag = newTag;
                cache[index].lines[i].validBit = 1;
	        cache[index].lines[i].used = 0;
            }
		return;
        }
    }
	int min_index = 0;
      if(command != 'S'){      //only store value in cache during load instruction
	for(i = 0; i < 8; i++){
		if(cache[index].lines[i].used > min){
			min = cache[index].lines[i].used;
			min_index = i;
		}
	}
	cache[index].lines[min_index].used = 0;
	cache[index].lines[min_index].tag = newTag;
	cache[index].lines[min_index].validBit = 1;
      }
}

void writeMiss16(setEntry16 * cache, string str, int size, int& hits, int& accesses){
    char command = str[0];
    int convert = 0;
    int i;
    string address = str.substr(2, str.length()-1);
    stringstream ss;
    ss << hex << address;
    ss >> convert;
    convert /= 32;
    int numSets = 32;
    //cout << "address: " << convert << endl;
    int index = (convert%32);
    //cout << "index: " << index << endl;
    int tagFound = 0;
    int emptyIndex = -1;
    int min = cache[index].lines[0].used;
    int lruIndex = -1;

    //cout << "size: " << size << endl;
    int newTag = convert / size;
	accesses++;


	for(i = 0; i < 16; i++){
		cache[index].lines[i].used++;
	}


	    for(i = 0; i < 16; i++){
        if(newTag == cache[index].lines[i].tag && cache[index].lines[i].validBit == 1){  //tag found within line of set
            hits++;
		//cout << "hit on index " << index << " place " << i << endl;
            cache[index].lines[i].used = 0;
            //accesses++;
		return;
        } else if(cache[index].lines[i].validBit == 0){   //checks for empty slot within line of set
            if(command != 'S'){   //don't write to cache on STORE-miss
                cache[index].lines[i].tag = newTag;
                cache[index].lines[i].validBit = 1;
	        cache[index].lines[i].used = 0;
            }
		return;
        }
    }
	int min_index = 0;
      if(command != 'S'){      //only store value in cache during load instruction
	for(i = 0; i < 16; i++){
		if(cache[index].lines[i].used > min){
			min = cache[index].lines[i].used;
			min_index = i;
		}
	}
	cache[index].lines[min_index].used = 0;
	cache[index].lines[min_index].tag = newTag;
	cache[index].lines[min_index].validBit = 1;
      }
}

int main(int argc, char ** argv){
    FILE *fp;
    FILE *ofp;
    int i, j;
    string file1;
    string arg2;
    string line;
    fstream file;

    int hitsDirect1 = 0;
    int totalDirect1 = 0;
    int hitsDirect2 = 0;
    int totalDirect2 = 0;
    int hitsDirect3 = 0;
    int totalDirect3 = 0;
    int hitsDirect4 = 0;
    int totalDirect4 = 0;

    int hitsSet1 = 0;
    int totalSet1 = 0;
    int hitsSet2 = 0;
    int totalSet2 = 0;
    int hitsSet3 = 0;
    int totalSet3 = 0;
    int hitsSet4 = 0;
    int totalSet4 = 0;

    int hitsFull1 = 0;
    int hitsFull2 = 0;
    int totalFull1 = 0;
    int totalFull2 = 0;

    int hitsMiss1 = 0;
    int totalMiss1 = 0;
    int hitsMiss2 = 0;
    int totalMiss2 = 0;
    int hitsMiss3 = 0;
    int totalMiss3 = 0;
    int hitsMiss4 = 0;
    int totalMiss4 = 0;

    int preFetchHits1 = 0;
    int preFetchTotal1 = 0;
    int preFetchHits2 = 0;
    int preFetchHits3 = 0;
    int preFetchHits4 = 0;
    int preFetchHits5 = 0;
    int preFetchHits6 = 0;
    int preFetchHits7 = 0;
    int preFetchHits8 = 0;

    int totalAccesses = 0;

    if(argc != 3){
        fprintf(stderr, "%s\n", "3 arguments required");
        exit(1);
    }

    arg2 = argv[2];
    if(!fileExists(arg2)){
       file.open("output.txt", fstream::out);
        arg2 = "output.txt";
    }
    /** CREATE DIRECT CACHES ***/
    directEntry directTable1[32];
    //directEntry * directTable1 = new directEntry[32];
    for(i = 0; i < 32; i++){
        directTable1[i].validBit = 0;
        directTable1[i].tag = -1;
    }

    directEntry * directTable2 = new directEntry[128];
    for(i = 0; i < 128; i++){
        directTable2[i].validBit = 0;
        directTable2[i].tag = -1;
    }

    directEntry * directTable3 = new directEntry[512];
    for(i = 0; i < 512; i++){
        directTable3[i].validBit = 0;
        directTable3[i].tag = -1;
    }

    directEntry * directTable4 = new directEntry[1024];
    for(i = 0; i < 1024; i++){
        directTable4[i].validBit = 0;
        directTable4[i].tag = -1;
    }

   /*** **************** ***/

   /** CREATE SET-ASSOCIATIVE CACHES ***/
   setEntry2 setTable1[256];
   for(i = 0; i < 256; i++){
       setTable1[i].lines[0].tag = -1;
       setTable1[i].lines[0].used = setTable1[i].lines[0].validBit = 0;
       setTable1[i].lines[1].tag = -1;
       setTable1[i].lines[1].used = setTable1[i].lines[0].validBit = 0;
   }

   setEntry4 setTable2[128];
   for(i = 0; i < 128; i++){
       for(j = 0; j < 4; j++){
           setTable2[i].lines[j].tag = -1;
           setTable2[i].lines[j].used = setTable2[i].lines[j].validBit = 0;
       }
   }

   setEntry8 setTable3[64];
   for(i = 0; i < 64; i++){
       for(j = 0; j < 8; j++){
           setTable3[i].lines[j].tag = -1;
           setTable3[i].lines[j].used = setTable3[i].lines[j].validBit = 0;
       }
   }

   setEntry16 setTable4[32];
   for(i = 0; i < 32; i++){
       for(j = 0; j < 16; j++){
           setTable4[i].lines[j].tag = -1;
           setTable4[i].lines[j].used = setTable4[i].lines[j].validBit = 0;
       }
   }

  /***********************************************/

  /***** FULLY ASSOCIATIVE CACHES ******/
  int tree[511];
  fullEntryHotCold fullTable1[512];
  fullEntryLRU fullTable2[512];
  for(i = 0; i < 512; i++){
      fullTable1[i].address = fullTable2[i].address = -1;
      fullTable1[i].validBit = fullTable2[i].validBit = 0;
      fullTable2[i].used = 0;
      if(i < 511){
          tree[i] = 0;
      }
  }
  /******************************************/

  /*** WRITE-MISS ASSOCIATIVE CACHES ****/
  setEntry2 writeMissTable1[256];
  for(i = 0; i < 256; i++){
       writeMissTable1[i].lines[0].tag = -1;
       writeMissTable1[i].lines[0].used = writeMissTable1[i].lines[0].validBit = 0;
       writeMissTable1[i].lines[1].tag = -1;
       writeMissTable1[i].lines[1].used = writeMissTable1[i].lines[0].validBit = 0;
  }

  setEntry4 writeMissTable2[128];
  for(i = 0; i < 128; i++){
       for(j = 0; j < 4; j++){
          writeMissTable2[i].lines[j].tag = -1;
          writeMissTable2[i].lines[j].used = writeMissTable2[i].lines[j].validBit = 0;
       }
  }

  setEntry8 writeMissTable3[64];
  for(i = 0; i < 64; i++){
      for(j = 0; j < 8; j++){
          writeMissTable3[i].lines[j].tag = -1;
          writeMissTable3[i].lines[j].used = writeMissTable3[i].lines[j].validBit = 0;
      }
  }

  setEntry16 writeMissTable4[32];
  for(i = 0; i < 32; i++){
      for(j = 0; j < 16; j++){
          writeMissTable4[i].lines[j].tag = -1;
          writeMissTable4[i].lines[j].used = writeMissTable4[i].lines[j].validBit = 0;
      }
  }
  /******************************/

  /***** PRE-FETCH ASSOCIATIVE CACHES *****/
  setEntry2 preFetchTable1[256];
  for(i = 0; i < 256; i++){
       preFetchTable1[i].lines[0].tag = -1;
       preFetchTable1[i].lines[0].used = writeMissTable1[i].lines[0].validBit = 0;
       preFetchTable1[i].lines[1].tag = -1;
       preFetchTable1[i].lines[1].used = preFetchTable1[i].lines[0].validBit = 0;
  }

  setEntry4 preFetchTable2[128];
  for(i = 0; i < 128; i++){
       for(j = 0; j < 4; j++){
          preFetchTable2[i].lines[j].tag = -1;
          preFetchTable2[i].lines[j].used = preFetchTable2[i].lines[j].validBit = 0;
       }
  }

  setEntry8 preFetchTable3[64];
  for(i = 0; i < 64; i++){
      for(j = 0; j < 8; j++){
         preFetchTable3[i].lines[j].tag = -1;
         preFetchTable3[i].lines[j].used = preFetchTable3[i].lines[j].validBit = 0;
      }
  }

  setEntry16 preFetchTable4[32];
  for(i = 0; i < 32; i++){
      for(j = 0; j < 16; j++){
          preFetchTable4[i].lines[j].tag = -1;
          preFetchTable4[i].lines[j].used = preFetchTable4[i].lines[j].validBit = 0;
      }
  }

   setEntry2 preFetchTable5[256];
  for(i = 0; i < 256; i++){
       preFetchTable5[i].lines[0].tag = -1;
       preFetchTable5[i].lines[0].used = preFetchTable5[i].lines[0].validBit = 0;
       preFetchTable5[i].lines[1].tag = -1;
       preFetchTable5[i].lines[1].used = preFetchTable5[i].lines[0].validBit = 0;
  }

  setEntry4 preFetchTable6[128];
  for(i = 0; i < 128; i++){
       for(j = 0; j < 4; j++){
          preFetchTable6[i].lines[j].tag = -1;
          preFetchTable6[i].lines[j].used = preFetchTable6[i].lines[j].validBit = 0;
       }
  }

  setEntry8 preFetchTable7[64];
  for(i = 0; i < 64; i++){
      for(j = 0; j < 8; j++){
         preFetchTable7[i].lines[j].tag = -1;
         preFetchTable7[i].lines[j].used = preFetchTable7[i].lines[j].validBit = 0;
      }
  }

  setEntry16 preFetchTable8[32];
  for(i = 0; i < 32; i++){
      for(j = 0; j < 16; j++){
          preFetchTable8[i].lines[j].tag = -1;
          preFetchTable8[i].lines[j].used = preFetchTable8[i].lines[j].validBit = 0;
      }
  }




 /**************************************************/
    file1 = argv[1];
    const char * inputFile = file1.c_str();
    ifstream myFile(inputFile);

    if(myFile.is_open()){
        while(getline(myFile, line)){
            int preFetchNeeded5; int preFetchNeeded6; int preFetchNeeded7;
            int preFetchNeeded8;

            directMapCache(directTable1, line, 32, hitsDirect1, totalDirect1);
            directMapCache(directTable2, line, 128, hitsDirect2, totalDirect2);
            directMapCache(directTable3, line, 512, hitsDirect3, totalDirect3);
            directMapCache(directTable4, line, 1024, hitsDirect4, totalDirect4);

            setAssoCache2(setTable1, line, 256, hitsSet1, totalSet1);
            setAssoCache4(setTable2, line, 128, hitsSet2, totalSet2);
            setAssoCache8(setTable3, line, 64, hitsSet3, totalSet3);
            setAssoCache16(setTable4, line, 32, hitsSet4, totalSet4);

            fullyAssoHotCold(fullTable1, line, tree, hitsFull1, totalFull1);
            fullyAssoLRU(fullTable2, line, hitsFull2, totalFull2);

            writeMiss2(writeMissTable1, line, 256, hitsMiss1, totalMiss1);
            writeMiss4(writeMissTable2, line, 128, hitsMiss2, totalMiss2);
    	    writeMiss8(writeMissTable3, line, 64, hitsMiss3, totalMiss3);
            writeMiss16(writeMissTable4, line, 32, hitsMiss4, totalMiss4);

            setAssoCache2(preFetchTable1, line, 256, preFetchHits1, preFetchTotal1);
	    setAssoCache2(preFetchTable1, line, 256, preFetchHits1, preFetchTotal1, 'P');
            setAssoCache4(preFetchTable2, line, 128, preFetchHits2, preFetchTotal1);
	    setAssoCache4(preFetchTable2, line, 128, preFetchHits2, preFetchTotal1, 'P');
            setAssoCache8(preFetchTable3, line, 64,  preFetchHits3, preFetchTotal1);
	    setAssoCache8(preFetchTable3, line, 64,  preFetchHits3, preFetchTotal1, 'P');
            setAssoCache16(preFetchTable4, line, 32, preFetchHits4, preFetchTotal1);
	    setAssoCache16(preFetchTable4, line, 32, preFetchHits4, preFetchTotal1, 'P');

            preFetchNeeded5 = preFetchHits5;
            setAssoCache2(preFetchTable5, line, 256, preFetchHits5, preFetchTotal1);
            if(preFetchNeeded5 == preFetchHits5){
                setAssoCache2(preFetchTable5, line, 256, preFetchHits5, preFetchTotal1, 'P');
            }

            preFetchNeeded6 = preFetchHits6;
            setAssoCache4(preFetchTable6, line, 128, preFetchHits6, preFetchTotal1);
            if(preFetchNeeded6 == preFetchHits6){
                setAssoCache4(preFetchTable6, line, 128, preFetchHits6, preFetchTotal1, 'P');
            }

            preFetchNeeded7 = preFetchHits7;
            setAssoCache8(preFetchTable7, line, 64, preFetchHits7, preFetchTotal1);
            if(preFetchNeeded7 == preFetchHits7){
                setAssoCache8(preFetchTable7, line, 64, preFetchHits7, preFetchTotal1, 'P');
            }

            preFetchNeeded8 = preFetchHits8;
            setAssoCache16(preFetchTable8, line, 32, preFetchHits8, preFetchTotal1);
            if(preFetchNeeded8 == preFetchHits8){
                setAssoCache16(preFetchTable8, line, 32, preFetchHits8, preFetchTotal1, 'P');
            }



            totalAccesses++;
            //setAssoCache4(setTable2, line, 128, hitsSet2, totalSet2);
            //setAssoCache8(setTable3, line, 64, hitsSet3, totalSet3);
            //setAssoCache16(setTable4, line, 32, hitsSet4, totalSet4);
        }
    }
    myFile.close();
    /******* PART 1 *******/
    /*cout << hitsDirect1 << "," << totalDirect1 << endl;
    cout << hitsDirect2 << "," << totalDirect2 << endl;
    cout << hitsDirect3 << "," << totalDirect3 << endl;
    cout << hitsDirect4 << "," << totalDirect4 << endl;*/

    /******* PART 2 *******/
   /* cout << hitsSet1 << "," << totalSet1 << endl;
    cout << hitsSet2 << "," << totalSet2 << endl;
    cout << hitsSet3 << "," << totalSet3 << endl;
    cout << hitsSet4 << "," << totalSet4 << endl; */

    /******* PART 3 *******/
    /*cout << hitsFull1 << "," << totalFull1 << endl;
    cout << hitsFull2 << "," << totalFull2 << endl; */

    /******* PART 4 ******/
    /*cout << hitsMiss1 << "," << totalMiss1 << endl;
    cout << hitsMiss2 << "," << totalMiss2 << endl;
    cout << hitsMiss3 << "," << totalMiss3 << endl;
    cout << hitsMiss4 << "," << totalMiss4 << endl;*/

    /******* PART 5 ******/

    /*cout << preFetchHits1 << "," << preFetchTotal1 << endl;
    cout << preFetchHits2 << "," << preFetchTotal1 << endl;
    cout << preFetchHits3 << "," << preFetchTotal1 << endl;
    cout << preFetchHits4 << "," << preFetchTotal1 << endl;*/

    /******* PART 6 *******/

    /*cout << preFetchHits5 << "," << preFetchTotal1 << endl;
    cout << preFetchHits6 << "," << preFetchTotal1 << endl;
    cout << preFetchHits7 << "," << preFetchTotal1 << endl;
    cout << preFetchHits8 << "," << preFetchTotal1 << endl;*/
    const char * outputFile = arg2.c_str();
    ofstream outFile(outputFile);
    if(outFile.is_open()){
        outFile << hitsDirect1 << "," << totalAccesses << "; " <<
        hitsDirect2 << "," << totalAccesses << "; " <<
        hitsDirect3 << "," << totalAccesses << "; " <<
        hitsDirect4 << "," << totalAccesses << ";" << endl;

        outFile << hitsSet1 << "," << totalAccesses << "; " <<
        hitsSet2 << "," << totalAccesses << "; " <<
        hitsSet3 << "," << totalAccesses << "; " <<
        hitsSet4 << "," << totalAccesses << ";" << endl;

        outFile << hitsFull2 << "," << totalAccesses << ";" << endl;
        outFile << hitsFull1 << "," << totalAccesses << ";" << endl;

        outFile << hitsMiss1 << "," << totalAccesses << "; " <<
        hitsMiss2 << "," << totalAccesses << "; " <<
        hitsMiss3 << "," << totalAccesses << "; " <<
        hitsMiss4 << "," << totalAccesses << ";" << endl;

        outFile << preFetchHits1 << "," << totalAccesses << "; " <<
        preFetchHits2 << "," << totalAccesses << "; " <<
        preFetchHits3 << "," << totalAccesses << "; " <<
        preFetchHits4 << "," << totalAccesses << ";" << endl;

        outFile << preFetchHits5 << "," << totalAccesses << "; " <<
        preFetchHits6 << "," << totalAccesses << "; " <<
        preFetchHits7 << "," << totalAccesses << "; " <<
        preFetchHits8 << "," << totalAccesses << ";" << endl;
        outFile.close();
    }
    else{
      cout << "unable to open file" << endl;
      return 1;
    }




    return 0;
}
