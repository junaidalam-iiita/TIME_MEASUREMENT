#include"dataAuditv2.h"


int main(int argc, char **argv) {
	clock_t startTime, endTime;
    	double elapsed_time;
    	double avgTimeTaken, totalTimeTaken = 0.0;
    	
    	
	std::ifstream configFile("Configuration.txt");  
    	if (!configFile.is_open()) {
        	std::cerr << "Failed to open the file." << std::endl;
        	exit(1);
    	}
    
    	ifstream ParamsFile("Params.bin", ios::binary);
    	if (!ParamsFile.is_open()) {
        	cerr << "Unable to open input file: Params.bin" << endl;
        	exit(1);
    	}
    	
    	// Reading the Protocol Configuration Values from the "Configuration.txt" file
	    int configValCount = 8, i= 0;
	    vector<string> Values(configValCount);
	    string val;
	    std::string line;
	    while (std::getline(configFile, line)) {  // Read the file line by line
		size_t pos = line.find('=');     // Find the '=' symbol
		if (pos != std::string::npos) {  // If '=' is found
		    val = line.substr(pos + 1);  // Extract the value after '='
		    // Trim whitespace from the value
		    val.erase(0, val.find_first_not_of(" \t"));
		    val.erase(val.find_last_not_of(" \t") + 1);
		    Values[i++] = val;
		}
	    }
	    
	    // Assigning the retrived configuration values to initialize the global variables
	   
	    SECSIZE = stoi(Values[0]);
	    SECPERBLOCK = stoi(Values[1]);
	    k1 = stoi(Values[2]);
	    k2 = stoi(Values[3]);
	    AUDIT_PERCENT = stod(Values[4]);
	    debug =  stoi(Values[5]);
	    lastProgramDebug =  stoi(Values[6]);
	    timeDebug = stoi(Values[7]);
	    BLOCKSIZE = (SECSIZE*SECPERBLOCK);
	    ENCODESIZE_1 = (k1/8);
	    ENCODESIZE_2 = (k2/8);
	    TAGSIZE = ENCODESIZE_2;
    
    	
    	cout<< "Retrieving Param Values from Param-File... " << endl;
    	GlobalParams params;
    	params.LoadValuesFromFile(ParamsFile);
    	 
    	cout << "p = " << params.p  << endl;
	cout << "g = " << params.g  << endl;
    	cout << "q = " << params.q  << endl;
	
	string input_to_hash, str1, str2;
    	size_t var = 0;
    	size_t hashedLength = SECSIZE;
    	unsigned char * hashedData;
    	hashedData = (unsigned char *) malloc(hashedLength);
    	str1 = "Helloh";
	Integer block;
	
	size_t n = 1000;
	
	for(size_t j = 0; j < n; j++){
			str2 = to_string(var++);;
			input_to_hash = str1 + str2;
			hashData(hashedData, hashedLength, reinterpret_cast<unsigned char*> (&input_to_hash[0]), input_to_hash.size() );
			
			// TIMER STARTS HERE
				startTime = clock();
    		
			
			block = MapWordToFiniteField(hashedData, hashedLength, params.q) ;
			
			// CODE BLOCK ENDS HERE
    			endTime = clock();
    			
    			//elapsed_time = static_cast<double>(endTime - startTime);
    			elapsed_time = ((static_cast<double>(endTime - startTime))/CLOCKS_PER_SEC)*1000;
    			totalTimeTaken = (totalTimeTaken + elapsed_time);
			
			
	    		//cout << "block-" << (j+1) << ": " << block << endl;
	    
	}
	
	
	avgTimeTaken = (totalTimeTaken / n);
	
	//cout << endl << "Execution Cost = " << avgTimeTaken << " CPU-Cycles" << endl;
	cout << endl << "Execution Cost = " << avgTimeTaken << " ms" << endl;
	
	configFile.close();
	ParamsFile.close();
    	return 0;
}

