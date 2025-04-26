/*  USAGE: ./<object_name> <k1> <k2> <mode>
    where, k1 and k2 are the sizes (in bits) of 'p' and 'q' and mode = 1 for printing the time in 'ms' mode = 0 for priting in no. of CPU-Cycles.
    For eaxmple:   ./FP_Group_Time 3072 256 1
    or, ./FP_Group_Time 1024 160 0
*/

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
	if(argc < 4){
		cerr << "Error! Incorrect Usage: ./<object_name> <k1> <k2> <mode>" << endl;
		exit(1);
	}
	size_t mode, k1, k2;
	k1 = stoi(argv[1]);
	k2 = stoi(argv[2]);
	mode = stoi(argv[3]);
	
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
    	
    	clock_t startTime4, endTime4;
    	double elapsed_time4;
    	double avgTimeTaken4, totalTimeTaken4 = 0.0;

	clock_t startTime5, endTime5;
    	double elapsed_time5;
    	double avgTimeTaken5, totalTimeTaken5 = 0.0;
    	
    	clock_t startTime6, endTime6;
    	double elapsed_time6;
    	double avgTimeTaken6, totalTimeTaken6 = 0.0;
    	
	//    Initializations
	
	AutoSeededRandomPool prng;
   	Integer p, q, a, b, c, binv, div, g;
        PrimeAndGenerator pg;
	pg.Generate(1, prng, k1, k2);
    	
    	p = pg.Prime();
    	q = pg.SubPrime();
    	g = pg.Generator();
	
    	
    	
    cout << "Generated prime p: " << std::dec << p << endl;
    cout << endl;
    std::cout << "Size of p (in bytes): " << p.ByteCount() << std::endl;
    cout << endl;
    cout << "Generated subgroup order q: " << std::dec << q << endl;
    cout << endl;
    std::cout << "Size of q (in bytes): " << q.ByteCount() << std::endl;
    cout << endl;
    cout << "Generated generator g: " << std::dec << g << endl;
    cout << endl;
    std::cout << "Size of g (in bytes): " << g.ByteCount() << std::endl;
    cout << endl;
    	
    	
    	for(size_t i = 0; i < n; i++){
    		a.Randomize(prng, Integer::One(), q - Integer::One());
    		b.Randomize(prng, Integer::One(), q - Integer::One());
    		
    		/* cout << "a: " << std::dec << a << endl;
    		cout << "b: " << std::dec << b << endl; */
    		
    		startTime1 = clock();
      		(a + b).Modulo(q);
    		endTime1 = clock();
    		if(mode) elapsed_time1 = static_cast<double>(endTime1 - startTime1)/CLOCKS_PER_SEC*1000;
    		else elapsed_time1 = static_cast<double>(endTime1 - startTime1);
    		totalTimeTaken1 = (totalTimeTaken1 + elapsed_time1);
    		
    		startTime2 = clock();
      		(a - b).Modulo(q);
    		endTime2 = clock();
    		if(mode) elapsed_time2 = static_cast<double>(endTime2 - startTime2)/CLOCKS_PER_SEC*1000;
    		else elapsed_time2 = static_cast<double>(endTime2 - startTime2);
    		totalTimeTaken2 = (totalTimeTaken2 + elapsed_time2);
    		
    		startTime3 = clock();
      		(a * b).Modulo(q);
    		endTime3 = clock();
    		if(mode) elapsed_time3 = static_cast<double>(endTime3 - startTime3)/CLOCKS_PER_SEC*1000;
    		else elapsed_time3 = static_cast<double>(endTime3 - startTime3);
    		totalTimeTaken3 = (totalTimeTaken3 + elapsed_time3);
    		
    		startTime4 = clock();
      		b.InverseMod(q);
    		endTime4 = clock();
    		if(mode) elapsed_time4 = static_cast<double>(endTime4 - startTime4)/CLOCKS_PER_SEC*1000;
    		else elapsed_time4 = static_cast<double>(endTime4 - startTime4);
    		totalTimeTaken4 = (totalTimeTaken4 + elapsed_time4);
    		
    		
    		startTime5 = clock();
    		binv = b.InverseMod(q);
    		div = (a * binv).Modulo(q);
    		endTime5 = clock();
    		if(mode) elapsed_time5 = static_cast<double>(endTime5 - startTime5)/CLOCKS_PER_SEC*1000;
    		else elapsed_time5 = static_cast<double>(endTime5 - startTime5);
    		totalTimeTaken5 = (totalTimeTaken5 + elapsed_time5);
    		
    		startTime6 = clock();
      		c = a_exp_b_mod_c(g, a, p);
    		endTime6 = clock();
    		if(mode) elapsed_time6 = static_cast<double>(endTime6 - startTime6)/CLOCKS_PER_SEC*1000;
    		else elapsed_time6 = static_cast<double>(endTime6 - startTime6);
    		totalTimeTaken6 = (totalTimeTaken6 + elapsed_time6);
    	}
    	
    	avgTimeTaken1 = (totalTimeTaken1 / n);
    	avgTimeTaken2 = (totalTimeTaken2 / n);
    	avgTimeTaken3 = (totalTimeTaken3 / n);
    	avgTimeTaken4 = (totalTimeTaken4 / n);
    	avgTimeTaken5 = (totalTimeTaken5 / n);
    	avgTimeTaken6 = (totalTimeTaken6 / n);
    	
    	cout << endl << "CLOCKS_PER_SEC = " << CLOCKS_PER_SEC << endl;
	
	if(mode){
		cout << endl << "Addition Cost = " << avgTimeTaken1 << " ms" << endl;
		cout << endl << "Subtraction Cost = " << avgTimeTaken2 << " ms" << endl;
		cout << endl << "Multiplication Cost = " << avgTimeTaken3 << " ms" << endl;
		cout << endl << "Inverse Cost = " << avgTimeTaken4 << " ms" << endl;
		cout << endl << "Division Cost = " << avgTimeTaken5 << " ms" << endl;
		cout << endl << "Exponentiation Cost = " << avgTimeTaken6 << " ms" << endl;
	}
	else{
		cout << endl << "Addition Cost = " << avgTimeTaken1 << " CPU-Cycles" << endl;
		cout << endl << "Subtraction Cost = " << avgTimeTaken2 << " CPU-Cycles" << endl;
		cout << endl << "Multiplication Cost = " << avgTimeTaken3 << " CPU-Cycles" << endl;
		cout << endl << "Inverse Cost = " << avgTimeTaken4 << " CPU-Cycles" << endl;
		cout << endl << "Division Cost = " << avgTimeTaken5 << " CPU-Cycles" << endl;
		cout << endl << "Exponentiation Cost = " << avgTimeTaken6 << " CPU-Cycles" << endl;
	}
	
	return 0;
}
