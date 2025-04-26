#include <iostream>
#include <string>
#include <fstream>
#include <cryptopp/osrng.h>
#include <cryptopp/integer.h>
#include <cryptopp/nbtheory.h>
#include <cryptopp/sha.h>      // For SHA256
#include <cryptopp/hex.h>      // For HexEncoder
#include <cryptopp/filters.h>  // For StringSource and HashFilter
#include <cryptopp/secblock.h> // For SecByteBlock
#include <random>
#include <set>
#include <ctime>

using namespace std;
using namespace CryptoPP;


int main(int argc, char **argv) {
	size_t n = 100; // accuracy vs. cost
	clock_t startTime, endTime;
    	double elapsed_time;
    	double avgTimeTaken, totalTimeTaken = 0.0;

	 
    	
    	for(size_t i = 0; i < n; i++){
    		
    		startTime = clock();
    		// CODE BLOCK STARTS HERE
    		
    		
    		
    		
    		// CODE BLOCK ENDS HERE
    		endTime = clock();
    		elapsed_time = static_cast<double>(endTime6 - startTime6)/CLOCKS_PER_SEC*1000;
    		totalTimeTaken = (totalTimeTaken + elapsed_time);
    		
    	}
    	
    	avgTimeTaken = (totalTimeTaken / n);
    	
	cout << "Execution Cost = " << avgTimeTaken << " ms" << endl;
	
	return 0;
}
