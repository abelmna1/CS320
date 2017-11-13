

#include<iostream>
#include<fstream>
#include<string>
#include <sstream>
#include <fstream>

using namespace std;
int bimodalCorrect = 0;
int gShareCorrect = 0;

bool fileExists(string fileName){
  const char * aFile = fileName.c_str();
  ifstream infile(aFile);
  return infile.good();
}

int alwaysTaken(int &branches){
    branches++;
    return branches;
}

int alwaysNotTaken(int &branches){
    branches++;
    return branches;
}

int bimodalSingleBit(int &numTaken, unsigned long long &addr, int *array, int size, string behavior){
    //int retVal = 0;
    int index = addr & (size-1);
    if(behavior == "T"){
      if(array[index] == 1){
        numTaken++;
      }
      if(array[index] == 0){
        array[index] = 1;
      }
    }
    else if(behavior == "NT"){
      if(array[index] == 0){
        numTaken++;
      }
      if(array[index] == 1){
        array[index] = 0;
      }
    }
    return 0;
}

int bimodalTwoBit(int &numTaken, unsigned long long &addr, int *array, int size, string behavior){
  //bimodalTwoBit(bimodalCorrect, addr, bimodal, 2048, behavior);
  int retVal = 0;
  int index = addr & (size-1);
  if(behavior == "T"){
    if(array[index] == 3){
      numTaken++;
      retVal = 1;
    }
    else if(array[index] == 2){
      numTaken++;
      array[index] = 3;
      retVal = 1;
    }
    else if(array[index] == 1){
      array[index] = 2;
    }
    else if(array[index] == 0){
      array[index] = 1;
    }
  }
  else if(behavior == "NT"){
    if(array[index] == 3){
      array[index] = 2;
    }
    else if(array[index] == 2){
      array[index] = 1;
    }
    else if(array[index] == 1){
      numTaken++;
      array[index] = 0;
      retVal = 1;
    }
    else if(array[index] == 0){
      numTaken++;
      retVal = 1;
    }
  }
  return retVal;
}

int gSharePredictor(int &correct, unsigned long long &addr, int *array, int *global, int size, string behavior){
    //gSharePredictor(gShareCorrect, addr, gShare, history, 11, behavior);
    int retVal = 0;
    int globalVar = 0;
    int address = addr & 2047;
    for(int i = 0; i < size; i++){
       if(global[i] == 0){
         globalVar |= (0 << i);
       }
       else if(global[i] == 1){
         globalVar |= (1 << i);
       }
    }
    //cout << "globalvar: " << globalVar << endl;
    //cout << "globalVar: " << globalVar << endl;
    //cout << "address: " << address << endl;
    int index = address ^ globalVar;
    //cout << "index: " << index << endl;


    //update previous bits before newest bit
    //cout << "size: " << size << endl;
    for(int i = size - 1; i > 0; i--){
      //cout << "i: " << i << endl;
      global[i] = global[i-1];
    }

    if(behavior == "T"){
      if(array[index] == 3){
        correct++;
        global[0] = 1;
        retVal = 1;
      }
      else if(array[index] == 2){
        correct++;
        array[index] = 3;
        global[0] = 1;
        retVal = 1;
      }
      else if(array[index] == 1){
        array[index] = 2;
        global[0] = 1;
      }
      else if(array[index] == 0){
        array[index] = 1;
        global[0] = 1;
      }
    }
    else if(behavior == "NT"){
      if(array[index] == 3){
        array[index] = 2;
        global[0] = 0;
      }
      else if(array[index] == 2){
        array[index] = 1;
        global[0] = 0;
      }
      else if(array[index] == 1){
        correct++;
        array[index] = 0;
        global[0] = 0;
        retVal = 1;
      }
      else if(array[index] == 0){
        correct++;
        global[0] = 0;
        retVal = 1;
      }
    }
     return retVal;
}

int tournament(int &correct, unsigned long long &addr, int* selector, int* gShare, int* history, int* bimodal, string behavior){
    int size1 = 2048; int size2 = 11;
    int retVal1 = bimodalTwoBit(bimodalCorrect, addr, bimodal, size1, behavior);
    int retVal2 = gSharePredictor(gShareCorrect, addr, gShare, history, size2, behavior);
    int index = addr & 2047;
    if(retVal1 == 1 && retVal2 == 1){
      correct++;
    }
    if(retVal1 == 0 && retVal2 == 1){  //only gShare is correct
        if(selector[index] == 0){
          correct++;
        }
        else if(selector[index] == 1){
          selector[index] = 0;
          correct++;
        }
        else if(selector[index] == 2){
          selector[index] = 1;
        }
        else if(selector[index] == 3){
          selector[index] = 2;
        }
    }
    else if(retVal1 == 1 && retVal2 == 0){   //only bimodal is correct
        if(selector[index] == 0){
          selector[index] = 1;
        }
        else if(selector[index] == 1){
          selector[index] = 2;
        }
        else if(selector[index] == 2){
          selector[index] = 3;
          correct++;
        }
        else if(selector[index] == 3){
          correct++;
        }
    }
    return 0;
}


int main(int argc, char *argv[]) {
  //bimdoal single bit predictor entry tables
  string arg1, arg2;
  fstream file;
  if(argc != 3){
    cout << "3 arguments required" << endl;
    return 1;
  }
  else{
    arg1 = argv[1];
    arg2 = argv[2];
    if(!fileExists(arg1)){
      cout << "input file doesn't exist" << endl;
      return 1;
    }
    if(!fileExists(arg2)){
      file.open("output.txt", fstream::out);
      arg2 = "output.txt";
    }
  }


  int * bimodalSingleBit16 = new int[16];
  int * bimodalSingleBit32 = new int[32];
  int * bimodalSingleBit128 = new int[128];
  int * bimodalSingleBit256 = new int[256];
  int * bimodalSingleBit512 = new int[512];
  int * bimodalSingleBit1024 = new int[1024];
  int * bimodalSingleBit2048 = new int[2048];

  //bimdoal two bit predictor entry tables
  int * bimodalTwoBit16 = new int[16];
  int * bimodalTwoBit32 = new int[32];
  int * bimodalTwoBit128 = new int[128];
  int * bimodalTwoBit256 = new int[256];
  int * bimodalTwoBit512 = new int[512];
  int * bimodalTwoBit1024 = new int[1024];
  int * bimodalTwoBit2048 = new int[2048];

  int * shareTable3 = new int[2048];
  int * shareTable4 = new int[2048];
  int * shareTable5 = new int[2048];
  int * shareTable6 = new int[2048];
  int * shareTable7 = new int[2048];
  int * shareTable8 = new int[2048];
  int * shareTable9 = new int[2048];
  int * shareTable10 = new int[2048];
  int * shareTable11 = new int[2048];

  int * globalHistoryBits3 = new int[3];
  int * globalHistoryBits4 = new int[4];
  int * globalHistoryBits5 = new int[5];
  int * globalHistoryBits6 = new int[6];
  int * globalHistoryBits7 = new int[7];
  int * globalHistoryBits8 = new int[8];
  int * globalHistoryBits9 = new int[9];
  int * globalHistoryBits10 = new int[10];
  int * globalHistoryBits11 = new int[11];

  int * tournyShare = new int[2048];
  int * tournyHistory = new int[11];
  int * tournyBimodal = new int[2048];
  int * selector = new int[2048];



  //single bit bimodal
  for(int i = 0; i < 16; i++){
    bimodalSingleBit16[i] = 1;
    bimodalTwoBit16[i] = 3;
  }
  for(int i = 0; i < 32; i++){
    bimodalSingleBit32[i] = 1;
    bimodalTwoBit32[i] = 3;
  }
  for(int i = 0; i < 128; i++){
    bimodalSingleBit128[i] = 1;
    bimodalTwoBit128[i] = 3;
  }
  for(int i = 0; i < 256; i++){
    bimodalSingleBit256[i] = 1;
    bimodalTwoBit256[i] = 3;
  }
  for(int i = 0; i < 512; i++){
    bimodalSingleBit512[i] = 1;
    bimodalTwoBit512[i] = 3;
  }
  for(int i = 0; i < 1024; i++){
    bimodalSingleBit1024[i] = 1;
    bimodalTwoBit1024[i] = 3;
  }
  for(int i = 0; i < 2048; i++){
    bimodalSingleBit2048[i] = 1;
    bimodalTwoBit2048[i] = 3;
    shareTable3[i] = 3;
    shareTable4[i] = 3;
    shareTable5[i] = 3;
    shareTable6[i] = 3;
    shareTable7[i] = 3;
    shareTable8[i] = 3;
    shareTable9[i] = 3;
    shareTable10[i] = 3;
    shareTable11[i] = 3;
    selector[i] = 0;
    tournyShare[i] = 3;
    tournyBimodal[i] = 3;
  }

  for(int i = 0; i < 3; i++){
    globalHistoryBits3[i] = 0;
  }
  for(int i = 0; i < 4; i++){
    globalHistoryBits4[i] = 0;
  }
  for(int i = 0; i < 5; i++){
    globalHistoryBits5[i] = 0;
  }
  for(int i = 0; i < 6; i++){
    globalHistoryBits6[i] = 0;
  }
  for(int i = 0; i < 7; i++){
    globalHistoryBits7[i] = 0;
  }
  for(int i = 0; i < 8; i++){
    globalHistoryBits8[i] = 0;
  }
  for(int i = 0; i < 9; i++){
    globalHistoryBits9[i] = 0;
  }
  for(int i = 0; i < 10; i++){
    globalHistoryBits10[i] = 0;
  }
  for(int i = 0; i < 11; i++){
    globalHistoryBits11[i] = 0;
    tournyHistory[i] = 0;
  }


  // Temporary variables
  unsigned long long addr;
  string behavior;
  int taken, alwaysTakenBranches, alwaysNotTakenBranches, notTaken, branches, bit;
  taken = branches = alwaysTakenBranches = alwaysNotTakenBranches = notTaken = bit = 0;

  int bimodalSingleTaken16, bimodalSingleTaken32, bimodalSingleTaken128,
  bimodalSingleTaken256, bimodalSingleTaken512, bimodalSingleTaken1024, bimodalSingleTaken2048;

  bimodalSingleTaken16 = bimodalSingleTaken32 = bimodalSingleTaken128 = bimodalSingleTaken256 =
  bimodalSingleTaken512 = bimodalSingleTaken1024 = bimodalSingleTaken2048 = 0;

  int bimodalTwoTaken16, bimodalTwoTaken32, bimodalTwoTaken128,
  bimodalTwoTaken256, bimodalTwoTaken512, bimodalTwoTaken1024, bimodalTwoTaken2048;

  bimodalTwoTaken16 = bimodalTwoTaken32 = bimodalTwoTaken128 = bimodalTwoTaken256 =
  bimodalTwoTaken512 = bimodalTwoTaken1024 = bimodalTwoTaken2048 = 0;

  int shareTaken3, shareTaken4, shareTaken5, shareTaken6, shareTaken7, shareTaken8, shareTaken9,
  shareTaken10, shareTaken11;

  shareTaken3 = shareTaken4 = shareTaken5 = shareTaken6 = shareTaken7 = shareTaken8 = shareTaken9 =
  shareTaken10 = shareTaken11 = 0;

  int tournyCorrect = 0;



  // Open file for reading
  const char * inputFile = arg1.c_str();
  const char * outputFile = arg2.c_str();
  ifstream infile(inputFile);

  // The following loop will read a hexadecimal number and
  // a string each time and then output them
  while(infile >> std::hex >> addr >> behavior) {
    cout<< "ADDR: " << addr << endl;
    //cout << addr;
    if(behavior == "T") {
      alwaysTakenBranches = alwaysTaken(taken);
    }
    else if(behavior == "NT"){
      alwaysNotTakenBranches = alwaysNotTaken(notTaken);
    }
    bimodalSingleBit(bimodalSingleTaken16, addr, bimodalSingleBit16, 16, behavior);
    bimodalSingleBit(bimodalSingleTaken32, addr, bimodalSingleBit32, 32, behavior);
    bimodalSingleBit(bimodalSingleTaken128, addr, bimodalSingleBit128, 128, behavior);
    bimodalSingleBit(bimodalSingleTaken256, addr, bimodalSingleBit256, 256, behavior);
    bimodalSingleBit(bimodalSingleTaken512, addr, bimodalSingleBit512, 512, behavior);
    bimodalSingleBit(bimodalSingleTaken1024, addr, bimodalSingleBit1024, 1024, behavior);
    bimodalSingleBit(bimodalSingleTaken2048, addr, bimodalSingleBit2048, 2048, behavior);

    bimodalTwoBit(bimodalTwoTaken16, addr, bimodalTwoBit16, 16, behavior);
    bimodalTwoBit(bimodalTwoTaken32, addr, bimodalTwoBit32, 32, behavior);
    bimodalTwoBit(bimodalTwoTaken128, addr, bimodalTwoBit128, 128, behavior);
    bimodalTwoBit(bimodalTwoTaken256, addr, bimodalTwoBit256, 256, behavior);
    bimodalTwoBit(bimodalTwoTaken512, addr, bimodalTwoBit512, 512, behavior);
    bimodalTwoBit(bimodalTwoTaken1024, addr, bimodalTwoBit1024, 1024, behavior);
    bimodalTwoBit(bimodalTwoTaken2048, addr, bimodalTwoBit2048, 2048, behavior);

    gSharePredictor(shareTaken3, addr, shareTable3, globalHistoryBits3, 3, behavior);
    gSharePredictor(shareTaken4, addr, shareTable4, globalHistoryBits4, 4, behavior);
    gSharePredictor(shareTaken5, addr, shareTable5, globalHistoryBits5, 5, behavior);
    gSharePredictor(shareTaken6, addr, shareTable6, globalHistoryBits6, 6, behavior);
    gSharePredictor(shareTaken7, addr, shareTable7, globalHistoryBits7, 7, behavior);
    gSharePredictor(shareTaken8, addr, shareTable8, globalHistoryBits8, 8, behavior);
    gSharePredictor(shareTaken9, addr, shareTable9, globalHistoryBits9, 9, behavior);
    gSharePredictor(shareTaken10, addr, shareTable10, globalHistoryBits10, 10, behavior);
    gSharePredictor(shareTaken11, addr, shareTable11, globalHistoryBits11, 11, behavior);

    tournament(tournyCorrect, addr, selector, tournyShare, tournyHistory, tournyBimodal, behavior);
    branches++;
   }
    ofstream outFile(outputFile); //open output file
    if(outFile.is_open()){
      outFile << alwaysTakenBranches << "," << branches << ";" << endl;
      outFile << alwaysNotTakenBranches << "," << branches << ";" << endl;
      outFile << bimodalSingleTaken16 << "," << branches << "; " <<
      bimodalSingleTaken32 << "," << branches << "; " <<
      bimodalSingleTaken128 << "," << branches << "; " <<
      bimodalSingleTaken256 << "," << branches << "; " <<
      bimodalSingleTaken512 << "," << branches << "; " <<
      bimodalSingleTaken1024 << "," << branches << "; " <<
      bimodalSingleTaken2048 << "," << branches << ";" << endl;
      outFile << bimodalTwoTaken16 << "," << branches << "; " <<
      bimodalTwoTaken32 << "," << branches << "; " <<
      bimodalTwoTaken128 << "," << branches << "; " <<
      bimodalTwoTaken256 << "," << branches << "; " <<
      bimodalTwoTaken512 << "," << branches << "; " <<
      bimodalTwoTaken1024 << "," << branches << "; " <<
      bimodalTwoTaken2048 << "," << branches << ";" << endl;
      outFile << shareTaken3 << "," << branches << "; " <<
      shareTaken4 << "," << branches << "; " <<
      shareTaken5 << "," << branches << "; " <<
      shareTaken6 << "," << branches << "; " <<
      shareTaken7 << "," << branches << "; " <<
      shareTaken8 << "," << branches << "; " <<
      shareTaken9 << "," << branches << "; " <<
      shareTaken10 << "," << branches << "; " <<
      shareTaken11 << "," << branches << ";" << endl;
      outFile << tournyCorrect << "," << branches << ";" << endl;
      outFile.close();
    }
    else{
      cout << "Unable to open file" << endl;
      return 1;
    }
    file.close();
    delete[] bimodalSingleBit16;
    delete[] bimodalSingleBit32;
    delete[] bimodalSingleBit128;
    delete[] bimodalSingleBit256;
    delete[] bimodalSingleBit512;
    delete[] bimodalSingleBit1024;
    delete[] bimodalSingleBit2048;

    delete[] bimodalTwoBit16;
    delete[] bimodalTwoBit32;
    delete[] bimodalTwoBit128;
    delete[] bimodalTwoBit256;
    delete[] bimodalTwoBit512;
    delete[] bimodalTwoBit1024;
    delete[] bimodalTwoBit2048;

    delete[] shareTable3;
    delete[] shareTable4;
    delete[] shareTable5;
    delete[] shareTable6;
    delete[] shareTable7;
    delete[] shareTable8;
    delete[] shareTable9;
    delete[] shareTable10;
    delete[] shareTable11;

    delete[] globalHistoryBits3;
    delete[] globalHistoryBits4;
    delete[] globalHistoryBits5;
    delete[] globalHistoryBits6;
    delete[] globalHistoryBits7;
    delete[] globalHistoryBits8;
    delete[] globalHistoryBits9;
    delete[] globalHistoryBits10;
    delete[] globalHistoryBits11;

    delete[] tournyShare;
    delete[] tournyHistory;
    delete[] tournyBimodal;
    delete[] selector;
    return 0;
}
