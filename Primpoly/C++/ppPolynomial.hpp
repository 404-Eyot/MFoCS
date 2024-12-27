#ifndef __POLYNOMIAL_H__
#define __POLYNOMIAL_H__

/*=============================================================================
|
| NAME
|
|     PolynomialError
|     PolynomialRangeError
|
| DESCRIPTION
|
|     Exception classes for classes Polynomial and PolyMod
|     derived from the STL exception class runtime_error.
|
+============================================================================*/

// General purpose exception for a polynomial.
class PolynomialError : public runtime_error
{
    public:
        // Throw with error message, file name and line number.
        PolynomialError( const string & description, const string & file, const int & line )
        : runtime_error( description + " in file " + file + " at line " + to_string(line) )
        {
        } ;
    
        // Throw with an error message.
        PolynomialError( const string & description )
            : runtime_error( description )
        {
        } ;

        // Throw with default error message.
        PolynomialError()
            : runtime_error( "Polynomial error:  " )
        {
        } ;

} ; // end class PolynomialError

// Typically thrown when we parse a bad string which is not a proper polynomial.
class PolynomialRangeError : public PolynomialError
{
    public:
        // Throw with error message, file name and line number.
        PolynomialRangeError( const string & description, const string & file, const int & line )
        : PolynomialError( description + " in file " + file + " at line " + to_string(line) )
        {
        } ;

        // Throw with an error message.
        PolynomialRangeError( const string & description )
            : PolynomialError( description )
        {
        } ;

        // Throw with default error message.
        PolynomialRangeError()
            : PolynomialError( "Polynomial range error:  " )
        {
        } ;

} ; // end class PolynomialRangeError


/*=============================================================================
|
| NAME
|
|     Polynomial
|
| DESCRIPTION
|
|     Represents the monic polynomial f( x ) of degree n, with
|     coefficients in GF( p ).  Precisely,
|
|                   n         n-1
|         f( x ) = x  +  a   x    + ... a x + a     0 <= a  < p
|                         n-1            1     0          i
|
|     The constant polynomial is of degree 0.
|
|     We support these basic operations:
|
|         Polynomial f ;                Construct f(x) = 0 mod 2.
|         <destructor>
|         Polynomial f( f2 ) ;          Copy, f(x) = f2(x)
|         Polynomial f = f2 ;           Overwrite, f(x) = f2(x)
|         Polynomial f( "x^2+1, 3" ) ;  Polynomial from string.
|         String s = f ;                Poly to string.
|         stream << p                   Read the poly.
|         p >> stream                   Print the poly.
|         p1 == p2, p1 != p2            Test for equality.
|         f[ i ] = coeff                Set poly coefficient.
|         f.deg()                       Return the degree of f(x).
|         f.modulus()                   Return the modulus p of f(x).
|         int coeff = f[ i ]            Get poly coefficient.
|         f(x) + g(x)                   Add poly mod p.
|         f(x)                          Evaluate f(x) for integer x
|         f.hasLinearFactor()           Check if f(x) has any linear factors.
|         f.isInteger()                 Is f(x) = constant?
|         f.firstTrialPoly()            Set f(x) = x^n - 1
|         f.nextTrialPoly()             Set f(x) = next poly in sequence.
|
|     Exceptions:  throw PolynomialError or one of its subclasses such as 
|     PolynomialRangeError.
|
| NOTES
|
|     The member functions and friends are documented in ppPolynomial.cpp
|
+============================================================================*/

class Polynomial
{
    public:
        // Default constructor which sets f(x) = 0 modulo 2.
        Polynomial() ;

        // Constructor for a polynomial from a vector of integers.
        explicit Polynomial( const vector<ppuint> ) ;
    
        // Destructor.
        virtual ~Polynomial() ;

        // Copy constructor.
        Polynomial( const Polynomial & g ) ;

        // Assignment.
        virtual Polynomial & operator=( const Polynomial & g ) ;
                                  
        // String to polynomial assignment.
        virtual Polynomial & operator=( string s ) ;
        
        // Construct from string:
        // Polynomial p( "x^2 + 2 x + 1, 3" ) ;
		// If modulus isn't specified, use the one in specified in the
		// polynomial string.
        Polynomial( string s, ppuint p = 0 ) ;
		
        // Operator casting to string type.
        operator string() const ;
    
        // Equality tests.
        friend bool operator==( const Polynomial & p1, const Polynomial & p2 ) ;
        friend bool operator!=( const Polynomial & p1, const Polynomial & p2 ) ;
    
        // cout << p and cin >> p
        friend ostream & operator<<( ostream & out, const Polynomial & p ) ;
        friend istream & operator>>( istream & in,        Polynomial & p ) ;

        // Bounds checked indexing operator which allows an lvalue:
        //  p[ i ] = coeff ;
        ppuint & operator[]( int i ) ;

        // Bounds checked indexing operator for read only access:
        // coeff = p[ i ] ;
        const ppuint operator[]( int i ) const ;

        // Return the degree n of f(x).
        int deg() const ;

        // Return the modulus p of f(x).
        ppuint modulus() const ;
				  
        // Set the modulus p of f(x).
        void setModulus( const ppuint p ) ;

        // Addition modulo p:  f(x) + g(x) mod p
        friend const Polynomial operator+( const Polynomial & f, const Polynomial & g ) ;

        // Addition.
        Polynomial & operator+=( const Polynomial & g ) ;
					  
        // Scalar multiple.
		friend const Polynomial
		   operator*( const Polynomial & f, const ppuint k ) ;
			   
        Polynomial & operator*=( const ppuint k ) ;

        // Evaluation:  f( x ) mod p
        ppuint operator()( int x ) ;

        // Does f(x) have any linear factor?
        bool hasLinearFactor() ;

        // Is f(x) an integer?
        bool isInteger() const ;
    
        // Only to show how we override a virtual function in C++.  We actually don't need to define this function here in the base class.
        virtual void initialTrialPoly( const ppuint n, const ppuint p )
            { throw PolynomialError( "Trying to call initialTrialPoly() from Polynomial class instead of its child class TrialPolynomial", __FILE__, __LINE__ ) ;
} ;

    // Private data accessible by member functions only, and
    // derived classes for convenience.
    protected:
        vector<ppuint>     f_ ;   // Polynomial coefficients:
                                  // f_[0] = const coeff ... f_[n] = nth degree coeff.
                                  // then f_.size() = n+1
        int                 n_ ;  // Degree of the polynomial.
        ppuint              p_ ;  // Coefficients are in GF( p ).
        ModP<ppuint,ppsint> mod ; // modulo p functionoid.
} ;

/*=============================================================================
|
| NAME
|
|     TrialPolynomial
|
| DESCRIPTION
|
|     Trial polynomial used in primitivity testing.  Just a Polynomial with
|     a couple of functions to initialize and iterate over a sequence of
|     polynomials.
|
| NOTES
|
+============================================================================*/

// final means a derived class won't be able to override any of the virtual
// functions.
class TrialPolynomial final : public Polynomial
{
    using Polynomial::Polynomial ; // Inherit all the base class constructors.
    
    public:
        // First trial polynomial.  Set
        //                 n
        //       f( x ) = x  - 1
        // override makes the compiler check the args AND the return type also
        // to make sure there's an exact match to the base class function.
        virtual void initialTrialPoly( const ppuint n, const ppuint p ) override ;

        // Update f( x ) := next polynomial in sequence.
        void nextTrialPoly() ;
} ;

/*=============================================================================
|
| NAME
|
|     PolyMod
|
| DESCRIPTION
|
|     Represents the monic polynomial g( x ) of degree m with coefficients in GF( p ),
|
|                   m         m-1
|         g( x ) = x  +  a   x  + ... + a    0 <= a  < p
|                         m-1            0         i
|
|     modulo f( x ) for monic polynomial f( x ) of degree n over GF( p ),
|
|                   n         n-1
|         f( x ) = x  +  a   x  + ... + a    0 <= a  < p
|                         n-1            0         i
|
|     We support these basic operations:
|     
|         PolyMod p() ;             Set g(x)=0 and f(x) = 0 mod 2
|                                   Destructor.   
|         PolyMod p( g, f )         Constructor from polynomials g(x) and f(x)
|         PolyMod p( p2 )           Copy p(x) = p2(x).
|         PolyMod p = p2            Assign p(x) = p2(x).
|         p.timesX() ;              p(x) := x p( x ) (mod f( x ), p)
|                                            2
|         p.square() ;              p(x) := p( x ) (mod f( x ), p)
|         p *= p2                   Do p(x) = p(x) * p2(x) (mod f( x ), p)
|         p1 * p2                   Do p1(x) * p2(x) (mod f( x ), p)
|                                           m 
|         p.power( m )              p(x) = x  (mod f(x), p)
|     
|     Exceptions:  throw PolynomialError or one of its subclasses such as 
|     PolynomialRangeError.
|
| NOTES
|
|     The member functions and friends are documented in ppPolynomial.hpp
|
+============================================================================*/

// Friends of PolyMod
ppuint autoConvolve( const Polynomial & t, const int k, const int lower, const int upper ) ;

ppuint coeffOfSquare( const Polynomial & g, const int k, const int n )  ;

ppuint coeffOfProduct( const Polynomial & s, const Polynomial & t, const int k, const int n )   ;

ppuint convolve( const Polynomial & s, const Polynomial & t, const int k, const int lower, const int upper ) ;

class PolyMod
{
    public:
        // Set g( x ) = 0 modulo f(x) = 0 and p = 2
        PolyMod() ;

        // Destructor.
        virtual ~PolyMod() ;

        // Construct from polynomials g(x) and f(x).
        PolyMod( const Polynomial & g, const Polynomial & f ) ;

        // Construct from string g and polynomial f(x).
        PolyMod( const string & g, const Polynomial & f ) ;
                         
		// Operator casting g(x) to string type.
        operator string() const ;
				 
         // cout << p prints g(x) to output stream
        friend ostream & operator<<( ostream & out, const PolyMod & p ) ;

        // Copy g( x ) = g2( x ).
        PolyMod( const PolyMod & g2 ) ;

        // Assign g( x ) = g2( x ) 
        virtual PolyMod & operator=( const PolyMod & g2 ) ;

        // Bounds checked indexing operator for read only access:
        // coeff = p[ i ] ;
        const ppuint operator[]( int i ) const ;

        // Multiply by x:  g(x) := g(x) x (mod f( x ), p)
        void timesX() ;

        // Squaring:             2
        //           g(x) := g(x) (mod f( x ), p)
        void square() ;

        // Multiplication:  g(x) := g(x) g2(x) (mod f( x ), p)
        PolyMod & operator*=( const PolyMod & g2 ) ;

        // Multiplication:  g(x) := s(x) t(x) (mod f( x ), p)
        friend const PolyMod operator*( const PolyMod & s, const PolyMod & t ) ;

        // Special exponentiation:  g(x) ^ m (mod f(x), p)
		// for g(x) = x only for now!
        friend const PolyMod power( const PolyMod & g, const BigInt & m ) ;

        bool isInteger() const ;
		
        //-----------------< Helper functions >-------------------------------
		const Polynomial getf() const ;
		
		const ppuint getModulus() const ;
		
       
    private:
        // Polynomial g(x).
        Polynomial g_ ;

        // Modulus polynomial f(x) and modulus p.
        Polynomial f_ ;

        // Two dimensional precomputed power table.
        //
        //  Precompute the n-1 polynomials
        //
        //       n      2n-2
        //      x  ... x     (mod f(x), p)
        //
        //                          n+i
        //      powerTable_[ i ] = x   (mod f(x), p)
        //
        vector<Polynomial> powerTable_ ;

        ModP<ppuint,ppsint>  mod ; //  modulo p functionoid.

    // Helper functions.  Note the friend functions are really public due to C++ rules.
    protected:
	    // Offset into powerTable.
	    int inline offset( const int i ) const { return i - f_.deg() ; }
		
        // Power table of the polynomial.
        void constructPowerTable() ;
		
        // Reduce g( x ) mod f( x ) and p
		void modf() ;

        // Autoconvolution product.
        friend ppuint autoConvolve( const Polynomial & t, const int k, const int lower, const int upper )   ;

        // Convolution product.
        friend ppuint convolve( const Polynomial & s, const Polynomial & t, 
                             const int k, const int lower, const int upper )  ;

        //               2
        // kth coeff of g (x) for degree n.
        friend ppuint coeffOfSquare( const Polynomial & g, const int k, const int n ) ;

        // Coeff of s(x) t(x) for degree n.
        friend ppuint coeffOfProduct( const Polynomial & s, const Polynomial & t, const int k, const int n ) ;
} ;



/*=============================================================================
|
| NAME
|
|     PolyOrder
|
| DESCRIPTION
|
|     Order tests on the monic polynomial f( x ) of degree n,
|     with coefficients in GF( p ).
|
|                   n         n-1
|         f( x ) = x  +  a   x  + ... + a    0 <= a  < p
|                         n-1            0         i
|
|     Exceptions:  throw PolynomialError or one of its subclasses such as
|     PolynomialRangeError.
|
| NOTES
|
|     The member functions and friends are documented in ppPolynomial.hpp
|
+============================================================================*/

class PolyOrder
{
    public:
        // Do tests on the nth degree polynomial f(x) modulo p.
        explicit PolyOrder( const Polynomial & f ) ;
    
        //               n
        //              p  - 1
        //  Compute r = -------- and factor r into the product of primes
        //              p - 1
        void factorR() ;
    
        void computeMaxNumPoly() ;

        void computeNumPrimPoly() ;

        void resetPolynomial( const Polynomial &f ) ;

        //             m                                          r
        // Check that x  (mod f(x), p) is not an integer for m = ---
        //                                                        p
        //                                                         i
        // but skip this test if p  | (p-1).
        //                        i
        bool orderM() ;

        //           r
	    // Check if x  (mod f(x), p) = a, for integer a.
        // If a is not an integer, return 0.
        ppuint orderR() ;

        //           k                                 n
        // Check if x  = 1 (mod f(x), p) only for k = p - 1
		// Note this is O( p^n ); very expensive.
        bool maximal_order() ;
				  
        // Check if the monic polynomial f( x ) has 2 or more distinct factors.
        // Uses x_to_power().
        bool hasMultipleDistinctFactors( bool earlyOut = true ) ;
           
        inline BigInt getR() const { return r_ ; } ;
    
        inline BigInt getNumPrimPoly() const { return num_prim_poly_ ; } ;

        inline BigInt getMaxNumPoly() const { return max_num_poly_ ; } ;

        // Test if a given polynomial f(x) is primitive.
        bool isPrimitive() ;
			  
        // Test function.
	    string printQMatrix() const ;
		
		inline int getNullity() const { return nullity_ ; } ;
    
        inline Factorization<BigInt> getFactorsOfR() const { return factors_of_R_ ; }

    // Allow direct access to this simple data type for convenience.
    public:
        OperationCount statistics_ ;

    protected:
        // Polynomial f(x) modulo p of degree n which is to be tested.
        Polynomial f_ ;
		ppuint     n_ ;
		ppuint     p_ ;
		ModP<ppuint,ppsint> mod ;
        
        //  n
        // p  - 1
        BigInt p_to_n_minus_1_ ;
    
        //           n
        //          p  - 1 
        //   r =   -------
        //          p  - 1
        BigInt r_ ;
 
        // Constant factor a (see notes).
        ppuint a_ ;
        
        //                   n
        // Factorization of p  - 1
        Factorization<BigInt> factors_of_p_to_n_minus_1_ ;
    
        // Factorization of r.
        Factorization<BigInt> factors_of_R_ ;
        
        // Number of possible primitive polynomials.
        BigInt num_prim_poly_ ;

        // Total number of possible polynomials.
        BigInt max_num_poly_ ;

        // Two dimensional Q matrix for irreducibility testing.
        vector< vector<ppsint> > Q_ ;
		
		int nullity_ ;
                
    // Helper functions. 
    protected:
        void generateQMatrix() ;

        void findNullity( bool earlyOut = true ) ;

} ;

#endif // __POLYNOMIAL_H__ --- End of wrapper for header file.
