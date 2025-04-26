/* This Header file contains the General Purpose Classes and Utility-Functions related to File-Processing, Finite-Filed Arithmatic, Crypto++ Library etc. (NOT specific for our Data-Auditing Protocol) */


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

using namespace std;
using namespace CryptoPP;

///                                  PROTOTYPES OF ALL CLASSES    /////////////////////////////////////////////////////////////////////////////////////////////


/******             Class For Efficient Handling of Finite Field (Fp) Arithmatic. Where 'p' is a large prime number                    *******/

class MyFiniteField{
	public:
		// Constructors and Initializers
		MyFiniteField();
		MyFiniteField(Integer x);
		void Initialize(Integer x);
		
		// Returns the modulus ('p') of this Finite Field 
		Integer getModulus();
		
		// Returns a random element from Fp
		Integer RandomElement();
		
		// Returns a vector of length 'l'. Each element of the vector is a random element from Fp
		vector<Integer> GetRandomVector(size_t l);
		
		//  Returns x modulo p
		Integer MapToField(Integer x);
		
		// Simple Field-Arithmatic Operations on Fp
		Integer Add(Integer a, Integer b);
		Integer Sub(Integer a, Integer b);
		Integer Mul(Integer a, Integer b);
		Integer Inv(Integer a);
		Integer Div(Integer a, Integer b);
		Integer Expo(Integer a, Integer b);
		
		// Addition between two vectors in Fp
		vector<Integer> VectorAddition(const vector<Integer>& v1, const vector<Integer>& v2);
		
		// Multiplication of a vector by a scalar in Fp
		vector<Integer> ScalarMultipliation(Integer x, const vector<Integer>& v);
		
		// Multiplication between two matrices in Fp
		vector<vector<Integer>> MatrixMultiply(const vector<vector<Integer>>& m1, const vector<vector<Integer>>& m2);
		
		// Multiplication of a matrix by a vector in Fp
		vector<Integer> MatrixMultiply(const vector<Integer>& v, const vector<vector<Integer>>& m);
		
		private:
		AutoSeededRandomPool prng;
		Integer p;
};

/******             Class For Efficient Handling of File                    *******/


class MyFileClass{
	public:
	
	//  Takes the name of the file and it's blocksize to create an object
	MyFileClass(string FileName, size_t blockSize);
	
	// Returns the size of the file (in bytes)
	size_t getSize();
	
	// Returns the total no. of blocks in the file (including the last-one which may be incomplete)
	size_t getBlockCount();
	
	// Writes the content of the given 'buffer' at the index'th block location of the file 
	void WriteBinaryBlock(char * buffer, size_t index);
	
	// Reads the index'th block of the file as binary data into the given 'buffer'
	void GetBinaryBlock(char * buffer, size_t index);
	
	// Closes the File
	void close();
	
	private:
	fstream File;
	size_t fileSize;
	size_t blocksize;
	size_t n, r;
};


///////////////////////////////////////     PROTOTYPES OF ALL GLOBAL FUNCTIONS /////////////////////////////////////////////////////////////////////////

// Returns a random string of the specified length (in bytes)
string  GetRandomString(size_t requiredStrlength);

// Returns the size (in bytes) of the given input-file
std::streamsize getFileSize(std::istream& fileStream);

//   Performs bitwise XOR between the given binary data 'a' and 'b' and assigns the result in 'c'. 'len' indicates the lengths of 'a', 'b' and 'c'
void bitwiseXOR(unsigned char *c, const unsigned char *a, const unsigned char *b, size_t len);

// Our customized SHA256 function. Returns the output as a HEX string
string computeSHA256(const unsigned char * inputData, size_t inLen);

// hashData: {0,1}* --> {0,1}^n
void hashData(unsigned char * output, size_t n, const unsigned char * inputData, size_t inLen);

/* Converts a given string into an unsigned int values. Make sure the string has at least 4 bytes.
Ignores the rest of the bytes if you give a string more than 4 bytes. */
unsigned int stringToUnsignedInt(const std::string& str);

// Prints the given vector. Elements of the vector should be of data-type 'size_t'
void PrintVector(const vector<size_t>& vec);

// Prints the given vector. Elements of the vector should be of data-type 'Integer'
void PrintVector(const vector<Integer>& vec);

// Prints the given matrix (row-by-row). Elements of the matrix should be of data-type 'Integer'
void PrintMatrix(const vector<vector<Integer>>& mat); 

/* Stores the given Integer varibale value into the specified output file. 
The encoding-size for the Integer must be mentioned in the 'IntegerSize' argument.*/
void SaveIntegerToBinaryFile(const CryptoPP::Integer& x, ofstream& outputFile, size_t IntegerSize);

/* Stores the given vector into the specified output file. Elements of the vector should be of data-type 'Integer'. 
The encoding-size for Integers must be mentioned in the 'IntegerSize' argument. */
void SaveVectorToFile(const vector<Integer>& vec, size_t IntegerSize, ofstream& outputFile);

/* Stores the given matrix into the specified output file (row-by-row). Elements of the matrix should be of data-type 'Integer'. 
The encoding-size for Integers must be mentioned in the 'IntegerSize' argument. */
void SaveMatrixToFile(const vector<vector<Integer>>& mat, size_t IntegerSize, ofstream& outputFile);

/* Retrieves 'IntegerSize' bytes from the specified input file, decodes it as an Cryptopp::Integer and returns the decoded value.  
The retrieval is performed from the current location of the read-cursor in the input-file. */
Integer LoadIntegerFromBinaryFile(ifstream& inputFile, size_t IntegerSize);

/* Retrieves 'IntegerSize' bytes from the specified input file, decodes it as a Cryptopp::Integer and returns the decoded value.  
This retrieval is performed from the specified location of the input-file mentioned in the 'index' argument. It fetches the read-cursor
in the file (index*IntegerSize) bytes from the begining of the file before performing the read. 
The function actually assumes that the input file conains sequence of Integervalues only.  */
Integer LoadIntegerFromBinaryFile(ifstream& inputFile, size_t IntegerSize, size_t index);

////////////////////////////////////                          IMPLEMENTATATION OF THE CLASS MEMBER FUNCTIONS     ////////////////////////////////////////////////////////////////////////////

/*                          Member Functions of the MyFiniteField Class                   */   


MyFiniteField::MyFiniteField(){}
		
MyFiniteField::MyFiniteField(Integer x){
	p = x;
}
		
void MyFiniteField::Initialize(Integer x){
	p = x;
}

Integer MyFiniteField::getModulus(){
	return p;
}
		
Integer MyFiniteField::RandomElement(){
	Integer r;
	r.Randomize(prng, Integer::Two(), p-Integer::Two());
	return (r);
}
		
vector<Integer> MyFiniteField::GetRandomVector(size_t l){
	vector<Integer> vector(l);
	for (size_t i = 0; i < l; ++i) {
            	vector[i] = RandomElement();
    	}
	return vector;
}
		
Integer MyFiniteField::MapToField(Integer x){
	return (x).Modulo(p);
}
		
Integer MyFiniteField::Add(Integer a, Integer b){
	return (a + b).Modulo(p);
}
		
Integer MyFiniteField::Sub(Integer a, Integer b){
	return (a - b).Modulo(p);
}
		
Integer MyFiniteField::Mul(Integer a, Integer b){
	return (a * b).Modulo(p);
}
		
Integer MyFiniteField::Inv(Integer a){
	return a.InverseMod(p);
}
		
Integer MyFiniteField::Div(Integer a, Integer b){
	Integer binv = b.InverseMod(p);
	return (a * binv).Modulo(p);
}
		
Integer MyFiniteField::Expo(Integer a, Integer b){
	return a_exp_b_mod_c(a, b, p);
}
		
vector<Integer> MyFiniteField::VectorAddition(const vector<Integer>& v1, const vector<Integer>& v2){
	size_t c1 = v1.size();
	size_t c2 = v2.size();
	if(c1 != c2){
		cerr << "Error: Dimensions of the two vectors must be same for addition" << endl;
		exit(1);
	}
	vector<Integer> result(c1);
	for (size_t i = 0; i < c1; ++i) {
				result[i] = Add(v1[i], v2[i]);
	}
	return result;
		
}
		
vector<Integer> MyFiniteField::ScalarMultipliation(Integer x, const vector<Integer>& v){
	size_t c = v.size();
	vector<Integer> result(c);
			
	for (size_t i = 0; i < c; ++i) {
		result[i] = Mul(v[i], x);
	}
	return result;
}
		
vector<vector<Integer>> MyFiniteField::MatrixMultiply(const vector<vector<Integer>>& m1, const vector<vector<Integer>>& m2) {
	size_t r1 = m1.size();
	size_t c1 = m1[0].size();
	size_t r2 = m2.size();
	size_t c2 = m2[0].size();
	
	if(c1 != r2){
		cerr << "Error: Dimensions of the matrices do not match to perform mul." << endl;
		exit(1);
	}
			
	vector<vector<Integer>> result(r1, vector<Integer>(c2, Integer::Zero()));
		    
	for (size_t i = 0; i < r1; ++i) {
		for (size_t j = 0; j < c2; ++j) {
			Integer sum = Integer::Zero();
			for (size_t k = 0; k < c1; ++k) {
				sum = Add(sum, Mul(m1[i][k], m2[k][j]));
			}
			result[i][j] = sum;
		}
	}
		    
	return result;
}
		
vector<Integer> MyFiniteField::MatrixMultiply(const vector<Integer>& v, const vector<vector<Integer>>& m) {
	size_t c1 = v.size();
	size_t r2 = m.size();
	size_t c2 = m[0].size();
	if(c1 != r2){
		cerr << "Error: Dimensions of the matrices do not match to perform mul." << endl;
		exit(1);
	}
		    
	vector<Integer> result(c2);
		   
	for (size_t j = 0; j < c2; ++j) {
		Integer sum = Integer::Zero();
		for (size_t k = 0; k < c1; ++k) {
			sum = Add(sum, Mul(v[k], m[k][j]));
		}
		result[j] = sum;
	}
		    
	return result;
}

/*                          Member Functions of the MyFileClass Class                   */   


	
MyFileClass::MyFileClass(string FileName, size_t blockSize){
	File.open(FileName, std::ios::in | std::ios::out | ios::binary);
    	if (!File.is_open()) {
        	cerr << "Unable to open input file: " << FileName << endl;
        	exit(1);
    	}
    		
	fileSize = getFileSize(File);
    	blocksize = blockSize;
    	n =  fileSize / (blocksize);
    	r = fileSize % (blocksize);
    	if(r > 0) n = n + 1;
}
	
void MyFileClass::WriteBinaryBlock(char * buffer, size_t index){
	File.seekp(index*blocksize, std::ios::beg);
	File.write(reinterpret_cast<const char*> (buffer), blocksize);
}
	
void MyFileClass::GetBinaryBlock(char * buffer, size_t index){
		
	// Initilize the buffer - all bytes set to the binary of '100'  - so that padded bytes of the last block will be 100
    	for (size_t i = 0; i < blocksize; i++)    buffer[i] = 100;
    		
    	File.seekg(index*blocksize, std::ios::beg);
    		
	// Read binary data (one blocksize or leftout data) from file into the buffer
	if( (r > 0) && (index == (n-1)) )
	    File.read(buffer, r );
	else
	    File.read(buffer, (blocksize) );
}
	
void MyFileClass::close(){
	File.close();
}


size_t MyFileClass::getSize(){
	return fileSize;
}

size_t MyFileClass::getBlockCount(){
	return n;
}
////////////////////////////////////////////////      IMPLEMENTATATION OF THE GLOBAL FUNCTIONS ///////////////////////////////////////////////////////////////


//    GetRandomString()

string  
GetRandomString(size_t requiredStrlength){
	AutoSeededRandomPool rng; 

    	string randomString;
    	randomString.resize(requiredStrlength);
	unsigned char uc;
	

    	rng.GenerateBlock(reinterpret_cast<CryptoPP::byte*>(&randomString[0]), requiredStrlength);

    	// Map bytes to printable ASCII characters (33 to 126)
	for (char& c : randomString) {
		// Convert to unsigned char to avoid negative values
        	uc = static_cast<unsigned char>(c);
        	// Map to printable ASCII range
        	c = 33 + (uc % 94);
	}
	
    	//cout << "Generated random string: " << "\""<< randomString << "\""<< endl;
    	return randomString;
}


//       getFileSize()

std::streamsize 
getFileSize(std::istream& fileStream) {
    if (!fileStream) {
        std::cerr << "File stream is not open!" << std::endl;
        exit(1);
    }

    // Save the current position of the file pointer
    std::streampos currentPos = fileStream.tellg();

    // Seek to the end to get the size of the file
    fileStream.seekg(0, std::ios::end);
    std::streamsize fileSize = fileStream.tellg();

    // Restore the file pointer to the original position
    fileStream.seekg(currentPos, std::ios::beg);

    return (fileSize-1); // excluding EOF
}

//       bitwiseXOR()

void bitwiseXOR(unsigned char *out, const unsigned char *a, const unsigned char *b, size_t len) {
    for (size_t i = 0; i < len; i++) {
        out[i] = a[i] ^ b[i];
    }
}


//           hashData()

void 
hashData(unsigned char * outputData, size_t outLen, const unsigned char * inputData, size_t inLen) {
    size_t i, k = 0;
    SHA256 hash;
    size_t len = inLen + 1;
    unsigned char * input_to_SHA = (unsigned char *) malloc(len);
    string digest;
    char c = 0;
    while (1) {
    	for (i = 0; i < (len - 1); i++) {
            input_to_SHA[i] = inputData[i];
        }
        
        input_to_SHA[i] = c;
        c = (c+1)%256;
        
        digest = computeSHA256(input_to_SHA, len);
        
        for (i = 0; i < digest.size(); i++) {
            outputData[k++] = digest[i];
            if (k == outLen) {
                free(input_to_SHA);
                return;
            }
        }
        
    }
}

//                computeSHA256()

std::string 
computeSHA256(const unsigned char * inputData, size_t inLen){
	std::string output;
	// Step 2: Create a SHA256 object
    	CryptoPP::SHA256 hash;

    	// Step 3: Compute the hash
    	CryptoPP::SecByteBlock digest(hash.DigestSize());
    	hash.Update(reinterpret_cast<const CryptoPP::byte*>(inputData), inLen);
    	hash.Final(digest);

    	// Step 4: Convert the hash to a hex string
    	CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(output));
    	encoder.Put(digest, digest.size());
    	encoder.MessageEnd();
	return output;
}



//                stringToUnsignedInt()

unsigned int stringToUnsignedInt(const std::string& str) {
    // Make sure the string has at least 4 bytes to fit an unsigned int
    if (str.size() < sizeof(unsigned int)) {
        std::cerr << "Warning: String is too short, padding with zeros." << std::endl;
        exit(1);
    }

    unsigned int result = 0;
    
    // Copy up to 4 bytes from the string to the result
    std::memcpy(&result, str.data(), std::min(str.size(), sizeof(unsigned int)));
    
    return result;
}

//               PrintVector()


void 
PrintVector(const vector<size_t>& vec) {
    for (const auto& elem : vec) {
        cout << elem << " ";
    }
    cout << endl;
}

//        PrintVector()-v2

void 
PrintVector(const vector<Integer>& vec) {
    for (const auto& elem : vec) {
        cout << elem << " ";
    }
    cout << endl;
}

//       PrintMatrix()


void 
PrintMatrix(const vector<vector<Integer>>& mat) {
    for (const auto& row : mat) {
        for (const auto& elem : row) {
            cout << elem << " ";
        }
        cout << endl;
    }
}

//            SaveIntegerToBinaryFile()


void 
SaveIntegerToBinaryFile(const CryptoPP::Integer& x, ofstream& outputFile, size_t IntegerSize) {
    CryptoPP::byte* outbuffer = (CryptoPP::byte *) malloc(IntegerSize);
    x.Encode(outbuffer, IntegerSize);
    outputFile.write(reinterpret_cast<const char*> (outbuffer), IntegerSize);
    free(outbuffer);
}


//              LoadIntegerFromBinaryFile()

Integer 
LoadIntegerFromBinaryFile(ifstream& inputFile, size_t IntegerSize) {
    Integer x;
    CryptoPP::byte* inbuffer = (CryptoPP::byte *) malloc(IntegerSize);
    inputFile.read(reinterpret_cast<char*> (inbuffer), IntegerSize);
    x.Decode(inbuffer, IntegerSize);
    free(inbuffer);
    return x;
}

//      LoadIntegerFromBinaryFile()-v2


Integer 
LoadIntegerFromBinaryFile(ifstream& inputFile, size_t IntegerSize, size_t index) {
    Integer x;
    CryptoPP::byte* inbuffer = (CryptoPP::byte *) malloc(IntegerSize);
    inputFile.seekg(index*IntegerSize, std::ios::beg);
    inputFile.read(reinterpret_cast<char*> (inbuffer), IntegerSize);
    x.Decode(inbuffer, IntegerSize);
    free(inbuffer);
    return x;
}



//            SaveVectorToFile()

void 
SaveVectorToFile(const vector<Integer>& mat, size_t IntegerSize, ofstream& outputFile) {
        for (const auto& elem : mat) 
             SaveIntegerToBinaryFile(elem, outputFile, IntegerSize);
}

//                    SaveMatrixToFile()

void 
SaveMatrixToFile(const vector<vector<Integer>>& mat, size_t IntegerSize, ofstream& outputFile) {
    for (const auto& row : mat) {
        for (const auto& elem : row) 
             SaveIntegerToBinaryFile(elem, outputFile, IntegerSize);
    }
}


