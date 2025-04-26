#include <iostream>
#include <cryptopp/osrng.h>
#include <cryptopp/integer.h>
#include <cryptopp/nbtheory.h>
#include <cryptopp/eccrypto.h> // Include Crypto++ ECC header
#include <cryptopp/oids.h>     // For predefined curve OIDs

using namespace std;
using namespace CryptoPP;


int main() {
	AutoSeededRandomPool prng;
	Integer a, b;
    	// Create ECC key
    	DL_GroupParameters_EC<ECP> params;
    	params.Initialize(ASN1::secp256r1()); // Selecting NIST P-256 curve

    	Integer p = params.GetCurve().GetField().GetModulus();
    	Integer q = params.GetSubgroupOrder();   
	ECP::Point P = params.GetSubgroupGenerator();
	
	cout << "Finite Field prime p: " << p << endl;
    	cout << endl;
    	std::cout << "Size of p (in bytes): " << p.ByteCount() << std::endl;
    	cout << endl;
    	cout << "Generated subgroup order q: " << q << endl;
    	cout << endl;
    	std::cout << "Size of q (in bytes): " << q.ByteCount() << std::endl;
    	cout << endl;
    	cout << "Generator Point of the Subgroup P(x,y): (" << P.x << ", " << P.y << ")" << endl;
    	cout << endl;
    	std::cout << "Size of coordinate (x or y) (in bytes): " << P.x.ByteCount() << std::endl;
    	cout << endl;
    	
    	a.Randomize(prng, Integer::One(), q - Integer::One());
    	b.Randomize(prng, Integer::One(), q - Integer::One());
    	
    	cout << "a: " << std::dec << a << endl;
	cout << endl;
	std::cout << "Size of a (in bytes): " << a.ByteCount() << std::endl;
    	cout << endl;
	cout << "b: " << std::dec << b << endl;
	cout << endl;
	
	//a = Integer(112450978446637834142616000616131638841952418416289275511814081188403085701318);
	
	Integer largeInt("112450978446637834142616000616131638841952418416289275511814081188403085701318");
	Integer modulus("115792089210356248762697446949407573529996955224135760342422259061068512044369");
	largeInt = a_exp_b_mod_c(largeInt, 3, modulus);

	ECP::Point A = params.GetCurve().Multiply(largeInt, P);
	ECP::Point B = params.GetCurve().Multiply(b, P);
	
	cout << "A(x,y): (" << A.x << ", " << A.y << ")" << endl;
	cout << "B(x,y): (" << B.x << ", " << B.y << ")" << endl;
	
	ECP::Point K1 = params.GetCurve().Multiply(b, A);
	ECP::Point K2 = params.GetCurve().Multiply(a, B);
	
	cout << "K1(x,y): (" << K1.x << ", " << K1.y << ")" << endl;
	cout << "K2(x,y): (" << K2.x << ", " << K2.y << ")" << endl;
	
	
	
	ECP::Point ID = params.GetCurve().Multiply(q, P);
	
	cout << "ID(x,y): (" << ID.x << ", " << ID.y << ")" << endl;
	
	
	Integer c = (a+b).Modulo(q);
	ECP::Point C = params.GetCurve().Add(A, B);
	ECP::Point D = params.GetCurve().Multiply(c, P);
	cout << "C(x,y): (" << C.x << ", " << C.y << ")" << endl;
	cout << "D(x,y): (" << D.x << ", " << D.y << ")" << endl;
	
	
	cout << endl << endl;
	cout << "PBC Params Size calculations" << endl;
	Integer PBC_q, PBC_r, va;
	va = Integer("256");
	PBC_q = Integer("8780710799663312522437781984754049815806883199414208211028653399266475630880222957078625179422662221423155858769582317459277713367317481324925129998224791");
	PBC_r = Integer("730750818665451621361119245571504901405976559617");
	cout << "Size of va = " << va.ByteCount() << " bytes" << endl;
	cout << "Size of PBC_q = " << PBC_q.ByteCount() << " bytes" << endl;
	cout << "Size of PBC_r = " << PBC_r.ByteCount() << " bytes" << endl;
	return 0;
}
