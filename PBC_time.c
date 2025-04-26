#include <stdint.h> // for intptr_t
#include <pbc/pbc.h>
#include <pbc/pbc_test.h>

int main(int argc, char **argv) {
	double t0, t1, ttotal, avgtime;
	ttotal = 0.0;
	
	pairing_t params;
	pbc_demo_pairing_init(params, argc, argv);
	size_t n = 10000; // accuracy vs. cost
	

	element_t P1, P2, P3, P4, d;
  
	  element_init_G1(P1, params);
	  element_init_G2(P2, params);
	  element_init_GT(P3, params);
	  element_init_GT(P4, params);
	  element_init_Zr(d, params);
	  
	  element_random(P1);
	  element_random(P2);
	  element_random(d);
    	
    	

    	
    	
    	
    	for(size_t i = 0; i < n; i++){
    		
    		t0 = pbc_get_time();
    		
    		// CODE BLOCK STARTS HERE
    		
    		
    		//element_add(P3, P1, P2);
    		//element_mul_zn(P3, P1, d);
    		element_pairing(P4, P1, P2);
    		
    		// CODE BLOCK ENDS HERE
    		t1 = pbc_get_time();
    		ttotal += t1 - t0;
    		
    	}
    	
    	size_t GT_size = element_length_in_bytes(P4);
    	 
    	
    	avgtime = ((ttotal / n) * 1000); 
    	
    	printf("average time = %f ms \n", avgtime );
    	
    	/*size_t G1_size = element_length_in_bytes(P1);
    	size_t G2_size = element_length_in_bytes(P2);
    	
    	size_t Zr_size = element_length_in_bytes(d);
    	
    	size_t mod_size = element_length_in_bytes(q);
    	
    	printf("sizeof Zr = %ld  \n", Zr_size );
	printf("sizeof G1 = %ld  \n", G1_size );
	printf("sizeof G2 = %ld  \n", G2_size );
	printf("sizeof GT = %ld  \n", GT_size );*/
	return 0;
}
