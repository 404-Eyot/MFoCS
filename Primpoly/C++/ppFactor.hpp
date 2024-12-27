#ifndef __PPFACTOR_H__
#define __PPFACTOR_H__


/*=============================================================================
|
| NAME
|
|     FactorError           General factor error, including internal memory errors.
|     FactorRangeError      Input range error.
|
| DESCRIPTION
|
|     Exception classes for the Factor class
|     derived from the STL exception class runtime_error.
|
+============================================================================*/

class FactorError : public runtime_error
{
    public:
        // Throw with error message, file name and line number.
         FactorError( const string & description, const string & file, const int & line )
         : runtime_error( description + " in file " + file + " at line " + to_string(line) )
         {
         } ;

        // Throw with an error message.
        FactorError( const string & description )
			: runtime_error( description )
        {
        } ;

        // Default throw with no error message.
        FactorError()
			: runtime_error( "Factor error:  " )
        {
        } ;

} ; // end class BigIntMathError


class FactorRangeError : public FactorError
{
    public:
        // Throw with error message, file name and line number.
        FactorRangeError( const string & description, const string & file, const int & line )
        : FactorError( description + " in file " + file + " at line " + to_string(line) )
        {
        };
    
        // Throw with an error message.
        FactorRangeError( const string & description )
			: FactorError( description )
        {
        } ;

        // Default throw with no error message.
        FactorRangeError()
			: FactorError( "Factor range error:  " )
        {
        } ;

} ; // end class FactorRangeError


/*=============================================================================
 |
 | NAME
 |
 |     PrimeFactor
 |
 | DESCRIPTION
 |
 |     Class to neatly package up unique prime factors to powers.
 |
 +============================================================================*/

template <typename IntType>
class PrimeFactor
{
    public:
        explicit PrimeFactor( IntType prime = static_cast<IntType>( 0u ), int count = 0 )
            : prime_( prime )
            , count_( count )
        {
        } ;

        ~PrimeFactor()
        {
        } ;

        PrimeFactor( const PrimeFactor & factor )
            : prime_( factor.prime_ )
            , count_( factor.count_ )
        {
        }

        PrimeFactor & operator=( const PrimeFactor & factor )
        {
            // Check for assigning to oneself:  just pass back a reference to the unchanged object.
            if (this == &factor)
                return *this ;

            prime_ = factor.prime_ ;
            count_ = factor.count_ ;
			
			return *this ;
        } ;
    
        // Print function for a factor.
        friend ostream & operator<<( ostream & out, const PrimeFactor & factor )
        {
            out << factor.prime_ << " ^ " << factor.count_ << " " ;
        
            return out ;
        }

    // Allow direct access to this simple data type for convenience.
    public:
        IntType prime_ ;
        int     count_ ;
} ;


/*=============================================================================
 |
 | NAME
 |
 |     CompareFactor
 |
 | DESCRIPTION
 |
 |     Class to sort unique prime factors to powers into order by prime size.
 |
 +============================================================================*/

template <typename IntType>
class CompareFactor
{
    public:
        bool operator()( const PrimeFactor<IntType> & s1, const PrimeFactor<IntType> & s2 )
        {
            return s1.prime_ < s2.prime_ ;
        }
} ;


/*=============================================================================
 |
 | NAME
 |
 |     Unit
 |
 | DESCRIPTION
 |                                                  0     e
 |     Class to check for unit factors of the form p  or 1  = 1
 |
 +============================================================================*/

template <typename IntType>
class Unit
{
    public:
        bool operator()( const PrimeFactor<IntType> & s )
        {
            return s.count_ == 0u || s.prime_ == static_cast<IntType>( 1u ) ;
        }
} ;


/*=============================================================================
|
| NAME
|
|     Factorization
|
| DESCRIPTION
|
|     Class for single and multiprecision factoring.
|
| EXAMPLE
|
|     ppuint p{ 2 } ;
|     ppuint n{ 4 } ;
|     BigInt p_to_n_minus_1 { power( p, 4 ) - 1 } ;
|
|                                       n                                         n
|     This will give us the factors of p - 1.  We can pass in either p and n or (p  - 1), but p and n might be
|     the better choice for really large numbers because then we can look up the factorization in a table instead
|     of computing it.
|
|     Factorization<BigInt> factors_of_p_to_n_minus_1( p_to_n_minus_1, FactoringAlgorithm::Automatic, p, n ) ;
|
|     And so will this for a smaller sized integer.
|
|     Factorization<ppuint> fact( power( p, 4 ) - 1 ) ;
|
|     int numDistinctFactors = fact.num() ;
|     for (int i = 0 ;  i < numDistinctFactors ;  ++i)
|     {
|         BigInt primeFactor  = fact[ i ] ;
|         int    multiplicity = fact.multiplicity( i ) ;
|     }
|
| NOTES
|
|     The member functions and friends are documented in detail ppBigInt.cpp
|
+============================================================================*/

// Different flavors of factoring algorithm.
enum class FactoringAlgorithm
{ 
    Automatic,
    TrialDivisionAlgorithm,
    pollardRhoAlgorithm,
    FactorTable
} ;


// We need both single precision and multi-precision factoring, so use
// a template with an integer type.
template <typename IntType>
class Factorization
{
    public:
		// Factor n into distinct primes.  Default constructor factors nothing.
        Factorization( const IntType num = static_cast<IntType>( 1u ),
                       FactoringAlgorithm type = FactoringAlgorithm::Automatic, ppuint p = 0, ppuint m = 0 ) ;

       ~Factorization() ;
	   
        // Copy constructor.
        Factorization( const Factorization<IntType> & f ) ;
		
        // Assignment operator.
        Factorization<IntType> & operator=( const Factorization<IntType> & g ) ;

	    // Return the number of distinct factors.
		ppuint numDistinctFactors() const ;

        // Return the ith prime factor along with its multiplicity as an lvalue so we can change either.
		PrimeFactor<IntType> & operator[]( int i ) ;
    
        // Return the ith prime factor.
        IntType primeFactor( int i ) const ;

        // Return the multiplicity of the ith prime factor.
        int multiplicity( int i ) const ;
        
        // True if p  | (p - 1).
        //          i
        bool skipTest( ppuint p, int i ) const ;

        // Factoring with tables.
        bool factorTable( ppuint p, ppuint n ) ;

        //                                     ---
        // Factoring by trial division up to \/ n
        void trialDivision() ;

        // Fast probabilistic factoring method.
        bool pollardRho( const IntType & c = static_cast<IntType>( 2u )) ;

        // Return a vector of only the distinct prime factors.
        vector<IntType> getDistinctPrimeFactors() ;
        
    // Allow direct access to this simple data type for convenience.
    public:
        OperationCount statistics_ ;

    private:
        // The unfactored remainder.
        IntType n_ ;

		// Total number of distinct factors.
		ppuint numFactors_ ;

        // Array of distinct prime factors of n with their multiplicities.
		vector< PrimeFactor<IntType> > factor_ ;

        // Distinct prime factors.
        vector<IntType> distinctPrimeFactors_ ;
} ;


/*=============================================================================
|
| NAME
|
|     Primality
|
| DESCRIPTION
|
|     What confidence a number is prime?
|
+============================================================================*/

enum class Primality
{
    Prime = 0,
    Composite,
    ProbablyPrime, 
    Undefined

}  ;


/*=============================================================================
 |
 | NAME
 |
 |     isProbablyPrime
 |
 | DESCRIPTION
 |
 |     True if n is likely to be prime.  Tests on a random integer x.
 |
 +============================================================================*/

template <typename IntType>
Primality isProbablyPrime( const IntType & n, const IntType & x ) ;


/*=============================================================================
 |
 | NAME
 |
 |     UniformRandomNumber
 |
 | DESCRIPTION
 |
 |     Return a uniform random integer in [0, range).
 |
 | NOTES
 |
 |     Use the JKISS random number generator from the article,
 |         Good Practice in (Pseudo) Random Number Generation for Bioinformatics Applications 
 |         by David Jones, UCL Bioinformatics Group (E-mail: d.jones@cs.ucl.ac.uk) 
 |         (Last revised May 7th 2010)
 |
 +============================================================================*/

template <typename IntType>
class UniformRandomIntegers
{
    public:
        // Seed the JKISS generator parameters x, y, z, c, and set up the range of the generator [0, range)
        explicit UniformRandomIntegers( const IntType range )
        : range( range )
        {
            ++num_of_initializations ;
            
            // Reseed at the first call to this class and only occasionally thereafter because it's slow.
            if (num_of_initializations % how_often_to_reseed == 0)
            {
                x = trueRandomNumberFromDevice() ;

                // Seed y must not be zero!
                while ( !(y = trueRandomNumberFromDevice()) ) ;
                
                z = trueRandomNumberFromDevice() ;

                // We don’t really need to set c as well but let's anyway...
                // NOTE: offset c by 1 to avoid z = c = 0
                c = trueRandomNumberFromDevice() % JKISS_MAGIC_NUM + 1 ; // Should be less than 698769069
            }
         }

        // Destructor has nothing to do.
        UniformRandomIntegers()
        {
        } ;

        IntType rand()
        {
            ppuint32 kiss = jKiss() ;
            
            // [0, range) falls within our generator's range [0, JKISSMAX).
            // To preserve a uniform distribution, map numbers in [0, JKISSMAX) down to [0, range) in the following way:
            // Map [0, range), => [0, range)
            // Map [range+1, 2 range) => [0, range)
            // ...
            // Map [(n-1) range + 1, n range) => [0, range)
            // Discard numbers in [n range + 1, JKISSMAX) because mapping numbers in this interval < range would inject a non-uniform bias.
            if (range < IntType( JKISSMAX ))
            {
                // It's safe to down convert the integer type.
                ppuint32 range2{ static_cast<ppuint32>( range ) } ;
                
                // Discard the random number unless it falls in a multiple of the range.
                // Retry with a new random number.  I hope we don't get an infinite loop!
                ppuint32 within_multiple_of_range{ JKISSMAX - (JKISSMAX % static_cast<ppuint32>(range)) } ;
                while (kiss > within_multiple_of_range)
                    kiss = jKiss() ;
                kiss = jKiss() % range2 ;
            }
            // If the range is larger than the maximum of our generator, do nothing.  We don't want to scale up
            // and have a non-uniform distribution.
            return static_cast<IntType>( kiss ) ;
        } ;

        private:
            static int num_of_initializations ;
            static const int how_often_to_reseed = 10000 ;
            static const int JKISS_MAGIC_NUM = 698769068 ;
    
            // Default seeds for JKISS.
            ppuint32 x{ 123456789 } ;
            ppuint32 y{ 987654321 } ;
            ppuint32 z{  43219876 } ;
            ppuint32 c{   6543217 } ;

            // Top end of the jKiss() generator range.
            const ppuint32 JKISSMAX { numeric_limits<ppuint32>::max() } ;
    
            // Return uniform random numbers in the range [0, range).
            IntType range ;

            // Read from the pseudo device /dev/urandom which return true random integers.
            unsigned int trueRandomNumberFromDevice()
            {
                // The device /dev/urandom only returns bytes, so grab four at a time to make on 32-bit unsigned integer.
                union RandomDeviceByteToInteger
                {
                    ppuint32 integer32 ;
                    char bytes4[ 4 ] ;
                } ;
                
                RandomDeviceByteToInteger numFromRandomDev ;
                
                ifstream fin( "/dev/urandom", ifstream::binary ) ;
                if (fin.good())
                {
                    fin.read( numFromRandomDev.bytes4, 4 ) ;
                    
                    if (!fin)
                    {
                        ostringstream os ;
                        os << "Cannot read from random device /dev/urandom" ;
                        throw FactorError( os.str(), __FILE__, __LINE__ ) ;
                    }
                }
                else
                {
                    ostringstream os ;
                    os << "Cannot open random device /dev/urandom" ;
                    throw FactorError( os.str(), __FILE__, __LINE__ ) ;
                }

                return numFromRandomDev.integer32 ;
            }

            // 32-bit integer random number generator.
            ppuint32 jKiss()
            {
                x = 314527869 * x + 1234567 ;

                y ^= (y <<  5) ;
                y ^= (y >>  7) ;
                y ^= (y << 22) ;

                ppuint t{ 4294584393ULL * z + c } ;  // This must be a 64-bit integer type.
                c = t >> 32 ;
                z = static_cast<ppuint32>( t ) ;

                return x + y + z ;
            }
} ;


/*==============================================================================
|                     Static Class Variables Initialization                    |
==============================================================================*/

// Static class variables must be initialized outside the class.

template <typename IntType> int UniformRandomIntegers<IntType>::num_of_initializations = 0 ;


/*=============================================================================
|
| NAME
|
|     isAlmostSurelyPrime
|
| DESCRIPTION
|
|     True if n is probabilistically prime.
|
+============================================================================*/

template <typename IntType>
bool isAlmostSurelyPrime( const IntType & n ) ;

#endif // __PPFACTOR_H__
