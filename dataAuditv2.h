#include"dataAuditUtils.h"



///////////////////                          Setup  (PP Version)                    /////////////////////////////////////////


void Setup() {
	
	// Variable decalrations
	string outputFilename;
	Integer alpha;
	AutoSeededRandomPool prng;
    	PrimeAndGenerator pg;
    	MyFiniteField Fq;
    	GlobalParams params;
    	vector<vector<Integer>> keymatrix;
    	pg.Generate(1, prng, k1, k2);
    	clock_t startTime, endTime;
    	double elapsed_time;
    	double avgTimeTaken, totalTimeTaken = 0.0;
    	
    	// Create and Open an Output file 'alpha.bin'
    	outputFilename = "alpha.bin";
    	ofstream alpha_file(outputFilename, ios::binary);
    	if (!alpha_file.is_open()) {
        	cerr << "Unable to open output file: " << outputFilename << endl;
        	alpha_file.close();
        	return;
    	}
    
    	// Create and Open an Output file 'Params.bin'
    	outputFilename = "Params.bin";
    	ofstream params_file(outputFilename, ios::binary);
    	if (!params_file.is_open()) {
        	cerr << "Unable to open output file: " << outputFilename << endl;
        	params_file.close();
        	return;
    	}
    	
    	/******** START TIMER  **************/
    		startTime = clock();
    		
    	// Generating Param Values: p, q, g
	params.p = pg.Prime();
	params.q = pg.SubPrime();
	params.g = pg.Generator();
	
    	// Generating alpha
    	Fq.Initialize(params.q);

	alpha = Fq.RandomElement();
    	if(debug) {
	    	cout << "p = " << params.p  << endl;
	    	cout << "g = " << params.g  << endl;
	    	cout << "q = " << params.q  << endl;
	    	cout << "alpha = " << alpha  << endl;
	}
    	
    	// Generate KeyMatrix as an one-dimensional Column-Matrix: <alpha, alpha^2, alpha^3, .., alpha^s>
    	keymatrix = GenerateKeyMatrix(alpha, SECPERBLOCK, Fq);
    	
    	// Compute:  h1 = g^{alpha}, h2 = g^{alpha^2}, ..., hs = g^{alpha^s}. everything in mod p
    	for(size_t j = 0; j < SECPERBLOCK; j++){
    		params.h_Values.push_back( a_exp_b_mod_c(params.g, keymatrix[j][0], params.p)  );
    	}
    	
    		endTime = clock();
        /******** STOP TIMER  **************/
        
        // Time Calculations
        elapsed_time = static_cast<double>(endTime - startTime) / CLOCKS_PER_SEC * 1000;
    	totalTimeTaken = (totalTimeTaken + elapsed_time);
    	
    	if(debug) {
    		cout << "H-Values: " << endl;
    		PrintVector(params.h_Values);
    	}
    	
    	// Save params and alpha to file
    	params.SaveValuesToFile(params_file);
    	SaveIntegerToBinaryFile(alpha, alpha_file, ENCODESIZE_2);
    	
    	// Finalizing
    	alpha_file.close();
    	params_file.close();
    	if(debug) {
	    	cout << "Setup Executed Successfully." << endl;
	    	cout << "Genereted Params <p, q, g, h1, h2, ..., h" << SECPERBLOCK << "> Saved into Params.bin file"  << endl; 
	    	cout << "Genereted Private-Key(alpha) stored into alpha.bin file!" << endl;
    	}
    	
    	statFile << "Setup Time = " << totalTimeTaken << " ms" << endl;
    	if(timeDebug) cout << totalTimeTaken << endl;
}	




 ///////////////////                          TagGen                      /////////////////////////////////////////
 
 
void TagGen(string dataFileName, Integer alpha, GlobalParams& params) {
    // Variable Declarations
    size_t n; // Total no. of blocks in the data-file
    vector<vector<Integer>> keymatrix;
    vector<Integer> block; 
    vector<Integer> TagVector;
    void * buffer =  malloc(BLOCKSIZE);
    clock_t startTime, endTime;
    double elapsed_time, keyMatCalTime;
    double avgTimeTaken, totalTimeTaken = 0.0;
    
    // Creating Output File for saving the Tags
    string outputFilename = "metaData.bin";
    ofstream outputFile(outputFilename, ios::binary);
    if (!outputFile.is_open()) {
        cerr << "Unable to open output file: " << outputFilename << endl;
        exit(1);
    }
    
    // Creating a MyFile Object for the given Data-File
    
    MyFileClass myfile(dataFileName, BLOCKSIZE);
    n = myfile.getBlockCount(); 
   
    // Initializing the Finite Field Fq from given params
    MyFiniteField Fq;
    Fq.Initialize(params.q);
    
    /******** START TIMER  **************/
    	startTime = clock();
    	
    // Generate KeyMatrix as an one-dimensional Column-Matrix: <alpha, alpha^2, alpha^3, .., alpha^s>
    keymatrix = GenerateKeyMatrix(alpha, SECPERBLOCK, Fq);
    
    	endTime = clock();
    /******** STOP TIMER  **************/
	
    // Time Calculations
    keyMatCalTime = static_cast<double>(endTime - startTime) / CLOCKS_PER_SEC * 1000;
    
    if(debug){
    	cout << "Key Matrix(s x 1):" << endl;
    	PrintMatrix(keymatrix);
    }	  
      
    // Process the data-blocks 0 to n-1 of the data-file
    
    for(size_t i = 0; i < n; i++){
   
    	// Get the i'th Block of the Data-File. 
    	myfile.GetBinaryBlock((char *) buffer, i);
    	
    	
    	/******** START TIMER  **************/
    		startTime = clock();
    	
    	block = ConvertSectorsInFq((char *) buffer, BLOCKSIZE, SECSIZE, params.q); /* Convert the extracted binary block as a vector of elements in Fq 
    											(where each sector of the block is converted as an element in Fq)*/
	TagVector = Fq.MatrixMultiply(block, keymatrix); /* [Row-Matrix(Vector)] x [Column-Matrix]: Result: <bi1*alpha + bi2*alpha^2 + ... + bis*alpha^s> =  Sigma-i */
	
		endTime = clock();
	/******** STOP TIMER  **************/
	
	// Time Calculations
	elapsed_time = static_cast<double>(endTime - startTime) / CLOCKS_PER_SEC * 1000;
	totalTimeTaken = (totalTimeTaken + elapsed_time);
	
	if(debug){ 
		cout << "Block-" << i << " : " ;
		PrintVector(block);
		cout << "Sigma-" << i << " : " ;
		PrintVector(TagVector);
	}
	
    	SaveVectorToFile(TagVector, ENCODESIZE_2, outputFile);
    }
    
    avgTimeTaken = keyMatCalTime + (totalTimeTaken / n);
    
    // Finalization
    outputFile.close();
    myfile.close();
    if(debug) 	cout << "Genereted Tags <sigma-1, ... sigma-"<< n <<"> Saved into metadata.bin file"  << endl; 
    
    statFile << "Average Tag Generation Time (for one block) = " << avgTimeTaken << " ms" << endl;
    if(timeDebug) cout << avgTimeTaken << endl;
}

///////////////////                          ChallengeGen                      /////////////////////////////////////////

void 
ChallengeGen(string outputFilename){
	// Creating Output File for saving the Generated Random Challenge String
    	ofstream chalFile(outputFilename, ios::binary);
    	if (!chalFile.is_open()) {
        	cerr << "Unable to open output file: " << outputFilename << endl;
        	return;
    	}
    	
    	// Variable Declarations
	size_t length = 135; // length of the generated string (in bytes)
	AutoSeededRandomPool rng; 
    	string randomString;
    	randomString.resize(length);
	unsigned char uc;
	

    	rng.GenerateBlock(reinterpret_cast<CryptoPP::byte*>(&randomString[0]), length);

    	// Map bytes to printable ASCII characters (33 to 126)
	for (char& c : randomString) {
		// Convert to unsigned char to avoid negative values
        	uc = static_cast<unsigned char>(c);
        	// Map to printable ASCII range
        	c = 33 + (uc % 94);
	}
	
    	if(debug) cout << "Generated random string: " << "\""<< randomString << "\""<< endl;
    	// Write the string to the file
    	chalFile << randomString;
	// Finalization
    	chalFile.close(); 
}



///////////////////                          ProofGen  (PP Version)                 /////////////////////////////////////////

void
ProofGen(string dataFileName, string seed_str, GlobalParams& params){
	//cout << "entered string: " << seed_str << endl;
	
	// Variable Declarations
    	vector<Integer> MIU(SECPERBLOCK); 
    	vector<Integer> block;
    	size_t indx, item;
    	Integer coeff;
    	vector<size_t> indices;
    	void * buffer = malloc(BLOCKSIZE);
    	clock_t startTime, endTime;
    	double elapsed_time;
    	double totalTimeTaken = 0.0;
    	
    	// Create and Open an Output file 'POP.bin'
    	string outputFilename = "POP.bin";
    	ofstream POP_file(outputFilename, ios::binary);
    	if (!POP_file.is_open()) {
        	cerr << "Unable to open output file: " << outputFilename << endl;
        	POP_file.close();
        	return;
    	}
    	
	// Creating MyFile Object for the datafile
    	MyFileClass myfile(dataFileName, BLOCKSIZE);
    	size_t n = myfile.getBlockCount(); // no. of blocks in the data-file
    	size_t c = (n * AUDIT_PERCENT); // No. of challenged blocks
    	if(debug){
    		cout << "Number of blocks(n) = "<< n << endl;
    		cout << "Number of blocks to be challenged(c) = "<< c << endl;
    	}
    	
    	// Initialize Fq from given params
    	MyFiniteField Fq;
    	Fq.Initialize(params.q);
    	
    	// Creating Objects of Random-Fq-element-generator and Random-Zn-element-generator from the seed string
    	getRandomFq randFq(seed_str, params.q);
    	getRandomZn randZn(seed_str, n);
    	
    	// Generate 'c' random indices in Zn and keep them in sorted order (to ensure a sequential access of the file)
    	for(size_t i = 0; i < c; i++){
    		item = randZn.getElementUnique();
    		auto position = std::lower_bound(indices.begin(), indices.end(), item);
    		indices.insert(position, item);
    	}
    	
    	/******** START TIMER **************/
    		startTime = clock();
    		
        //Initializing the Accumulator(MIU) to all zero
        for(size_t j = 0; j < SECPERBLOCK; j++) 	MIU[j] = 0;
    	
    		endTime = clock();
	/******** STOP TIMER **************/
		
	// Time Calculations
	elapsed_time = static_cast<double>(endTime - startTime) / CLOCKS_PER_SEC * 1000;
	totalTimeTaken = (totalTimeTaken + elapsed_time);
	
    	for(size_t i = 0; i < c; i++){
    	
    		// Get the i'th index and i'th coefficient
    		indx = indices[i];
    		coeff = randFq.getElement();
    		if(debug) cout << "(" << indx << ", " << coeff << ");  " << endl;
    		
    		// Get the i'th indexed Block of the Data-File. 
    		myfile.GetBinaryBlock((char *) buffer, indx);
    	
    		/******** START TIMER **************/
    			startTime = clock();
    			
    		// Convert the extracted binary block as a vector of elements in Fq (where each sector of the block is converted as an element in Fq)
    		block = ConvertSectorsInFq((char *) buffer, BLOCKSIZE, SECSIZE, params.q);
    	
	    	if(debug){
    			cout << "Challenged-Block-" << indx << " : " ;
			PrintVector(block);
		}
		
		/*Multiply each sector of the retrieved (challenged) block by the i'th coefficient of the challenge vector.
			This is a scalar multiplication between the i'th coefficient and the i'th-Indexed Block*/
		block = Fq.ScalarMultipliation(coeff, block);
		
		/* Accumulate the resulted block-vector in the MIU-Vector.
			This is a Vector-Addition between MIU-Vector and the resulted block-vector. */
		MIU = Fq.VectorAddition(MIU, block);
		
			endTime = clock();
		/******** STOP TIMER **************/
		
		// Time Calculations
		elapsed_time = static_cast<double>(endTime - startTime) / CLOCKS_PER_SEC * 1000;
		totalTimeTaken = (totalTimeTaken + elapsed_time);
    	}
    	
    	
		
    	if(debug){ 
    		cout << "Miu: " << endl;
    		PrintVector(MIU);
    	}
 
 	// Calculate POP-Proof = (h1^{miu1} * h2^{miu2} * ... * hs^{mius}) mod p
    	
    	Integer temp, Proof = 1;
    	
    	/******** START TIMER **************/
    		startTime = clock();
    			
    	// For each sector 1 ... s
    	for(size_t j = 0; j < SECPERBLOCK; j++){
    	
    		//Compute: hj^{miuj} mod p
    		temp = a_exp_b_mod_c(params.h_Values[j], MIU[j], params.p);
    		
    		// Accumulate the product
    		Proof = (Proof * temp).Modulo(params.p);
    	}
    	
    		endTime = clock();
    	/******** STOP TIMER **************/
		
	// Time Calculations
	elapsed_time = static_cast<double>(endTime - startTime) / CLOCKS_PER_SEC * 1000;
	totalTimeTaken = (totalTimeTaken + elapsed_time);
    	
    	if(debug) cout << "PP-POP = " << Proof << endl;
    	
    	PP_PoPData popData;
    	popData.setData(Proof);
    	popData.SaveValuesToFile(POP_file); 
    	
    	
    	// Finalizing
    	myfile.close();
    	POP_file.close();
    	if(debug) cout << "PP-POP Saved into POP.bin file"  << endl; 
    	statFile << "Proof Generation Time (excluding Disk I/O and chellenge-vector generation time) = " << totalTimeTaken << " ms" << endl;
    	if(timeDebug) cout << totalTimeTaken << endl;
}

///////////////////                          VerifyProof  (PP Version)                    /////////////////////////////////////////


void VerifyProof(string tagFileName, PP_PoPData& pp_popData, string seed_str, GlobalParams& params){	
	// Variable declarations
	vector<vector<Integer>> keymatrix;
	vector<Integer> MIUSig;
	size_t indx, item, fileSize, n, c;
	vector<size_t> indices;
	Integer tag, coeff, SUM, temp;
	clock_t startTime, endTime;
    	double elapsed_time;
    	double totalTimeTaken = 0.0;
    	
	ifstream tagFile(tagFileName, ios::binary);
    	if (!tagFile.is_open()) {
        	cerr << "Unable to open input file: " << tagFileName << endl;
        	exit(1);
    	}
    	fileSize = getFileSize(tagFile)+1;
    	n = ceil(fileSize / TAGSIZE);
    	c = (n * AUDIT_PERCENT);
    	if(lastProgramDebug){
    		cout << " Tag-fileSize ===== " << fileSize << endl;
    		cout << " TAGSIZE ===== " << TAGSIZE << endl;
		cout << " n ===== " << n << endl;
		cout << " c ===== " << c << endl;
	}
	
	// Initializing the Finite Field Fq from given params
    	MyFiniteField Fq;
    	Fq.Initialize(params.q);
    	
	// STEP-1: CALCULATE THE LINEAR COMBINATION OF THE CORRESPONDING TAGS
	
	// Creating Objects of Random-Fq-element-generator and Random-Zn-element-generator from the seed string
    	getRandomFq randFq(seed_str, params.q);
    	getRandomZn randZn(seed_str, n);
    	
    	SUM = 0;
    	
    	// Generate 'c' random indices in Zn and keep them in sorted order (to ensure a sequential access of the file)
    	for(size_t i = 0; i < c; i++){
    		item = randZn.getElementUnique();
    		auto position = std::lower_bound(indices.begin(), indices.end(), item);
    		indices.insert(position, item);
    	}
    	
    	for(size_t i = 0; i < c; i++){
    	
    		// Get the i'th index and i'th coefficient
    		indx = indices[i];
    		coeff = randFq.getElement();
    		if(lastProgramDebug) cout << "(" << indx << ", " << coeff << ");  " << endl;
    		
    		// Get the i'th-Indexed Tag of the MetaData-File. 

    		tag = LoadIntegerFromBinaryFile(tagFile, TAGSIZE, indx);
	    	
    		if(lastProgramDebug) cout << "Sigma-" << indx << " : " << tag << endl;

		/******** START TIMER **************/
			startTime = clock();
		
		temp = Fq.Mul(coeff, tag);
		SUM = Fq.Add(SUM, temp);
		
			endTime = clock();
    		/******** STOP TIMER **************/
    	
    		// Time Calculations
    		elapsed_time = static_cast<double>(endTime - startTime) / CLOCKS_PER_SEC * 1000;
    		totalTimeTaken = (totalTimeTaken + elapsed_time);
    	}
    	
    	if(lastProgramDebug) cout << "SUM: " << SUM << endl << endl;
    	
    	// STEP-2: CALCULATE g^SUM and match it with the received POP-Proof
    	
    	/******** START TIMER **************/
		startTime = clock();
    	
    	Integer tau = a_exp_b_mod_c(params.g, SUM, params.p);
    	
    	endTime = clock();
    	/******** STOP TIMER **************/
    	
    	// Time Calculations
    	elapsed_time = static_cast<double>(endTime - startTime) / CLOCKS_PER_SEC * 1000;
    	totalTimeTaken = (totalTimeTaken + elapsed_time);
    	
    	if(lastProgramDebug){
    		cout << "TAU: " << tau << endl << endl;
    		cout << "Received Proof: " << pp_popData.getData() << endl;
    	}
    	
	// STEP-3: MATCH THE ABOVE TWO
	
	if(tau == pp_popData.getData() ){
		if(lastProgramDebug) cout << "Verification is Successful!!! " << endl;
		if(!(timeDebug)) cout << 1 << endl;
	}
	else{
		if(lastProgramDebug) cout << "Verification FAILED !!! " << endl;
		if(!(timeDebug))  cout << 0 << endl;
	}
	tagFile.close();
	statFile << "Proof Verification Time (excluding Disk I/O and chellenge-vector generation time) = " << totalTimeTaken << " ms" << endl;
	if(timeDebug) cout << totalTimeTaken << endl;
}
