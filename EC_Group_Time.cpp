/*  USAGE: ./<object_name> <mode>
    where, mode = 1 for printing the time in 'ms' mode = 0 for priting in no. of CPU-Cycles.
    For eaxmple:   ./EC_Group_Time 1
    or, ./EC_Group_Time 0
*/

#include <iostream>
#include <cryptopp/osrng.h>
#include <cryptopp/integer.h>
#include <cryptopp/nbtheory.h>
#include <cryptopp/eccrypto.h> // Include Crypto++ ECC header
#include <cryptopp/oids.h>     // For predefined curve OIDs

using namespace std;
using namespace CryptoPP;


int main(int argc, char **argv) {
	if(argc < 2){
		cerr << "Error! Incorrect Usage: ./<object_name> <mode>" << endl;
		exit(1);
	}
	size_t mode;
	mode = stoi(argv[1]);
	
	bool debug = 0;
	size_t n = 10000; // accuracy vs. cost
	
	clock_t startTime1, endTime1;
    	double elapsed_time1;
    	double avgTimeTaken1, totalTimeTaken1 = 0.0;
    	
    	clock_t startTime2, endTime2;
    	double elapsed_time2;
    	double avgTimeTaken2, totalTimeTaken2 = 0.0;
    	
    	clock_t startTime3, endTime3;
    	double elapsed_time3;
    	double avgTimeTaken3, totalTimeTaken3 = 0.0;
    	
    	
    	
	//    Initializations
	AutoSeededRandomPool prng;
	Integer a, b;
    	// Create ECC key
    	DL_GroupParameters_EC<ECP> params;
    	params.Initialize(ASN1::secp256r1()); // Selecting NIST P-256 curve

    	Integer p = params.GetCurve().GetField().GetModulus();
    	Integer q = params.GetSubgroupOrder();   
	ECP::Point P = params.GetSubgroupGenerator();
	
	if(debug){
		cout << "Finite Field prime p: " << p << endl;
	    	cout << endl;
	    	cout << "Generated subgroup order q: " << q << endl;
	    	cout << endl;
	    	cout << "Generator Point of the Subgroup P(x,y): (" << P.x << ", " << P.y << ")" << endl;
	    	cout << endl;
    	}
    	
    	for(size_t i = 0; i < n; i++){
    		a.Randomize(prng, Integer::One(), q - Integer::One());
    		b.Randomize(prng, Integer::One(), q - Integer::One());
    	
    		if(debug){
    			cout << "a: " << std::dec << a << endl;
			cout << "b: " << std::dec << b << endl;
		}
    		
    		startTime1 = clock();
    		
      		ECP::Point Q = params.GetCurve().Multiply(a, P);
      		
    		endTime1 = clock();
    		if(mode) elapsed_time1 = static_cast<double>(endTime1 - startTime1)/CLOCKS_PER_SEC*1000;
    		else elapsed_time1 = static_cast<double>(endTime1 - startTime1);
    		totalTimeTaken1 = (totalTimeTaken1 + elapsed_time1);
    		
    		startTime2 = clock();
    		
      		ECP::Point R = params.GetCurve().Add(P, Q);
      		
    		endTime2 = clock();
    		if(mode) elapsed_time2 = static_cast<double>(endTime2 - startTime2)/CLOCKS_PER_SEC*1000;
    		else elapsed_time2 = static_cast<double>(endTime2 - startTime2);
    		totalTimeTaken2 = (totalTimeTaken2 + elapsed_time2);
    		
    		startTime3 = clock();
    		
      		ECP::Point negQ = params.GetCurve().Inverse(Q);
      		
    		endTime3 = clock();
    		if(mode) elapsed_time3 = static_cast<double>(endTime3 - startTime3)/CLOCKS_PER_SEC*1000;
    		else elapsed_time3 = static_cast<double>(endTime3 - startTime3);
    		totalTimeTaken3 = (totalTimeTaken3 + elapsed_time3);
    		
    		if(debug){
	    		ECP::Point P2 = params.GetCurve().Add(R, negQ);
	    		cout << "P(x,y): (" << P.x << ", " << P.y << ")" << endl;
	    		cout << "Q(x,y): (" << Q.x << ", " << Q.y << ")" << endl;
	    		cout << "negQ(x,y): (" << negQ.x << ", " << negQ.y << ")" << endl;
	    		cout << "P2(x,y): (" << P2.x << ", " << P2.y << ")" << endl;
    		}
    	}
    	
    	avgTimeTaken1 = (totalTimeTaken1 / n);
    	avgTimeTaken2 = (totalTimeTaken2 / n);
    	avgTimeTaken3 = (totalTimeTaken3 / n);
    	
    	
    	cout << endl << "CLOCKS_PER_SEC = " << CLOCKS_PER_SEC << endl;
	if(mode){
		cout << endl << "Point Addition Cost = " << avgTimeTaken2 << " ms" << endl;
		cout << endl << "Point Scalar-Multiplication Cost = " << avgTimeTaken1 << " ms" << endl;
		cout << endl << "Point Inverse (negation) Cost = " << avgTimeTaken3 << " ms" << endl;
	}
	else{
		cout << endl << "Point Addition Cost = " << avgTimeTaken2 << " CPU-Cycles" << endl;
		cout << endl << "Point Scalar-Multiplication Cost = " << avgTimeTaken1 << " CPU-Cycles" << endl;
		cout << endl << "Point Inverse (negation) Cost = " << avgTimeTaken3 << " CPU-Cycles" << endl;
	}
	return 0;
}
