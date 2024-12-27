#include <cstdlib>      // abort()
#include <iostream>     // Basic stream I/O.
#include <iomanip>      // I/O manipulators.
#include <new>          // set_new_handler()
#include <cmath>        // Basic math functions e.g. sqrt()
#include <complex>      // Complex data type and operations.
#include <fstream>      // File stream I/O.
#include <sstream>      // String stream I/O.
#include <vector>       // STL vector class.
#include <string>       // STL string class.
#include <algorithm>    // Iterators.
#include <stdexcept>    // Exceptions.
#include <cassert>      // assert()

using namespace std ;


/*------------------------------------------------------------------------------
|                                PP Include Files                              |
------------------------------------------------------------------------------*/

#include "Primpoly.hpp"         // Global functions.
#include "ppArith.hpp"          // Basic arithmetic functions.
#include "ppBigInt.hpp"         // Arbitrary precision integer arithmetic.
#include "ppOperationCount.hpp" // OperationCount collection for factoring and poly finding.
#include "ppFactor.hpp"         // Prime factorization and Euler Phi.
#include "ppPolynomial.hpp"     // Polynomial operations and mod polynomial operations.
#include "ppParser.hpp"         // Parsing of polynomials and I/O services.
#include "ppUnitTest.hpp"       // Complete unit test.


/*=============================================================================
|
| NAME
|
|     Polynomial()
|
| DESCRIPTION
|
|    Default constructor for Polynomial class.  Constructs the zero degree
|    polynomial p(x) = 0 (mod 2)
|
| EXAMPLE
|
|    Polynomial p ;
|
+============================================================================*/

Polynomial::Polynomial()
                : f_()
                , n_( 0 )
                , p_( 2 )
                , mod( p_ )
{
    // f(x) = 0
    f_.push_back( 0 ) ;
}


/*=============================================================================
 |
 | NAME
 |
 |     Polynomial()
 |
 | DESCRIPTION
 |
 |     Constructor for a polynomial from a vector of integers.
 |
 | EXAMPLE
 |
 |    vector<ppunit> v { 1, 2, 3 } ;
 |    Polynomial p{ v } ;
 |    Polynomial p( v ) ;
 |
 +============================================================================*/

Polynomial::Polynomial( const vector<ppuint> v )
: n_{ static_cast<int>( v.size() - 1) }
, p_( 2 )
, mod( p_ )
{
    // Copy over the polynomial coefficients.
    f_ = v ;
}


/*=============================================================================
|
| NAME
|
|     ~Polynomial
|
| DESCRIPTION
|
|     Destructor.
|
| EXAMPLE
|
|     void add1( const Polynomial & f )
|     {
|         const Polynomial g{ 1u } ;
|         return f + g ;
|         // Destructor for g automatically called as f goes out of scope.
|     }
|
+============================================================================*/

Polynomial::~Polynomial()
{
    // vector f_ frees itself and mod_ calls its own destructor.
}


/*=============================================================================
|
| NAME
|
|     Polynomial
|
| DESCRIPTION
|
|     Copy constructor.
|
| EXAMPLE
|
|     try
|     {
|         Polynomial f ;
|         Polynomial f( g ) ;
|     }
|     catch( PolynomialError & e )
|     {
|         cout << "Error in constructing polynomial f(x) or g(x)" << endl ;
|     }
|
+============================================================================*/

Polynomial::Polynomial( const Polynomial & g )
                : f_( g.f_ )
                , n_( g.n_ )
                , p_( g.p_ )
                , mod( p_ )
{
    // The classes in the initializer above all copy themselves.
}


/*=============================================================================
|
| NAME
|
|     Polynomial::operator=
|
| DESCRIPTION
|
|     Safe assigment operator for polynomials, f( x ) = g( x )
|     which leaves the polynomial f( x ) unchanged if an exception is thrown.
|
| EXAMPLE
|
|     try
|     {
|         Polynomial f ;
|         Polynomial g ;
|         f = g ;
|     }
|     catch( PolynomialError & e )
|     {
|         cout << "Error in constructing polynomial f(x)" << endl ;
|     }
|
+============================================================================*/

Polynomial & Polynomial::operator=( const Polynomial & g )
{
    // Check for assigning to oneself by comparing the unique pointers to the classes for speed.
    // If the user does f = f, just pass back a reference to the unchanged polynomial f.
    if (this == &g)
        return *this ;

    // Assign the scalars first.
    n_ = g.n_ ;
    p_ = g.p_ ;

    // And the modulus functionoid.
    mod( g.p_ ) ;

    // Overwrite the old polynomial coefficients in f_ with the new coefficients in g.f_:
    //   1) Delete the old polynomial coefficients, i.e. destruct the vector valued member variable f_.
    //   2) Construct a new vector f_.
    //   3) Copy the coefficients from g to f_.
    // But here's the problem:  if we fail to construct the new f_ and throw an exception,
    // e.g by requesting a bad vector size, we've destroyed the existing polynomial coefficients f_.
    //
    // The following solution guarantees that if f = g throws an exception, the value of f is unchanged.
    try
    {
        // Create a temporary copy of the polynomial coefficients.
        vector<ppuint> tempCoeff{ g.f_ } ;

        // Move the old values into the temporary, and the new values into the object.
        // The library function swap() exchanges the values in the two containers,
        // guarantees no exceptions will be thrown.
        // The temporary containing the old values will be destroyed when we leave scope.
        swap( f_, tempCoeff ) ;
    }
    // Failed to construct tempPoly.
    catch( bad_alloc & e )
    {
        throw PolynomialError( "Polynomial.operator=() had a bad_alloc memory allocation failure", __FILE__, __LINE__ ) ;
    }

    // Return a reference to the altered object.
    return *this ;
}


/*=============================================================================
|
| NAME
|
|     Polynomial::operator=( string )
|
| DESCRIPTION
|
|     Assigment operator for string to polynomial, f( x ) = "polynomial"
|
| EXAMPLE
|
|     try
|     {
|         Polynomial f ;
|         f = "x^2 + 1" ;
|     }
|     catch( PolynomialError & e )
|     {
|         cout << "Error in constructing polynomial f(x)" << endl ;
|     }
|
+============================================================================*/

Polynomial & Polynomial::operator=( string s )
{    
    try
    {
        // Construct a new polynomial from the string.
        Polynomial g( s ) ;

        n_ = g.n_ ;
        p_ = g.p_ ;
        mod( g.p_ ) ;

        // Right, no exceptions were thrown from the constructor, so
        // we've got a new polynomial object now.
        // Trash the existing polynomial.
        f_.clear() ;
        f_ = g.f_ ;
    }
    catch( bad_alloc & e )
    {
        throw PolynomialRangeError( "Polynomial.operator=(string) had a bad_alloc memory allocation failure", __FILE__, __LINE__ ) ;
    }

    // Return a reference to assigned object to make chaining possible.
    return *this ;
}


/*=============================================================================
|
| NAME
|
|     Polynomial, construct from string.
|
| DESCRIPTION
|
|     Construct a polynomial from a string.
|
| EXAMPLE
|
|     try
|     {
|         Polynomial f( "x^2 + 2x + 1, 3" ) ;
|     }
|     catch( PolynomialRangeError & e )
|     {
|         cout << "Error in construction polynomial f(x) from string" << endl ;
|     }
|
 , n_( 0 )
 , p_( 2 )
+============================================================================*/

Polynomial::Polynomial( string s, ppuint p )
                : f_()
                , mod( 0 )
                , n_( 0 )
                , p_( 2 )
{
    //  The polynomial string must have at least one character in it.
    // Not really an exception but more of a user input error.
    if (s.empty())
        throw PolynomialRangeError( "polynomial string is empty" ) ;

    try
    {
        // Initialize an LALR(1) parser for polynomials.
        PolyParser< PolySymbol, PolyValue > pp ;

        PolyValue v = pp.parse( s ) ;

        // Get the modulus specified by the polynomial.
        p_ = v.scalar_ ;

        // If the modulus is explicitly input, use that instead of the polynomial's modulus.
        if (p > 0)
            p_ = p ;

        // Sanity check the modulus.
        if (p_ <= 0)
        {
            ostringstream os ;
            os << "Error.  Polynomial modulus p must be > 0 but p = " << p_ << endl ;
            throw PolynomialRangeError( os.str() ) ;
        };
        // TODO:  check upper range.

        mod.set( p_ ) ;

        // Sanity check the degree of the polynomial.
        n_ = static_cast<int>( v.f_.size() ) - 1 ;
        if (n_ < 0)
        {
            ostringstream os ;
            os << "Error.  Polynomial degree n must be >= 0 but n = " << n_ << endl ;
            throw PolynomialRangeError( os.str() ) ;
        }

        // Reduce all the (positive) polynomial coefficients modulo p.
        vector<ppuint>::iterator i ;
        for (i = v.f_.begin() ;  i != v.f_.end() ;  ++i)
            *i = mod( *i ) ;

        // Copy over the polynomial coefficients.
        f_ = v.f_ ;
    }
    catch( ParserError & e )
    {
        ostringstream os ;
        os << "Error in parser converting polynomial from string " << s << " for p = " << p_ << " " << e.what() ;
        throw PolynomialRangeError( os.str() ) ;
    }
}


/*=============================================================================
|
| NAME
|
|     string
|
| DESCRIPTION
|
|     Convert a polynomial to a string.
|
| EXAMPLE
|
|     try
|     {
|         Polynomial f( "x^2 + 1,3" ) ;
|         string poly = f ;
|     }
|     catch( PolynomialRangeError & e )
|         cout << "Error in construction polynomial f(x) from string" << endl ;
|
+============================================================================*/

// Operator casting to string type.
Polynomial::operator string() const
{
    // Set up a string stream for convenience.
    ostringstream os ;

    // Spin out the polynomial coefficients from high to low degree.
    // Special case of f(x) = const.
    if (f_.size() == 1)
    {
        if (!(os << f_[ 0 ]))
        {
            ostringstream os ;
            os << "Error in converting polynomial to string: "
               << " with n = " << n_ << " and p = " << p_ ;
            throw PolynomialRangeError( os.str() ) ;
        }
    }
    else
    {
        int lowestDegreeTerm = -1 ;
        for (int deg = n_ ;  deg >= 0 ;  --deg)
            if (f_[ deg ] != 0)
                lowestDegreeTerm = deg ;

        for (int deg = n_ ;  deg >= 0 ;  --deg)
        {
            if (f_[ deg ] != 0)
            {
                // x^n has a nonzero coefficient.
                ppuint coeff = f_[ deg ] ;

                 // Print coeff of x^n unless it is 1.
                 // But print the constant term regardless.
                if (coeff != 1 || deg == 0)
                {
                    string extraBlank = deg == 0 ? "" : " " ;

                    if (!(os << coeff << extraBlank))
                     {
                        ostringstream os ;
                        os << "Error in converting polynomial to string: "
                        << " with n = " << n_ << " and p = " << p_ ;
                        throw PolynomialRangeError( os.str() ) ;
                     }
                }

                // Print x (no exponent).
                if (deg == 1)
                {
                    if ( !(os << "x") )
                     {
                        ostringstream os ;
                        os << "Error in converting polynomial to string: "
                        << " with n = " << n_ << " and p = " << p_ ;
                        throw PolynomialRangeError( os.str() ) ;
                     }
                }
                // Print x^n ... x^2
                else if (deg != 0)
                {
                    if (!(os << "x ^ " << deg))
                     {
                        ostringstream os ;
                        os << "Error in converting polynomial to string: "
                        << " with n = " << n_ << " and p = " << p_
                        << " in file " << __FILE__ << " line " << __LINE__ ;
                        throw PolynomialRangeError( os.str() ) ;
                     }
                }

                // Print +, but only when followed by a lower degree term or constant.
                // e.g. x^2 + 2 x, x + 3
                if (lowestDegreeTerm != -1 && deg > lowestDegreeTerm)
                    if (!(os << " + "))
                     {
                        ostringstream os ;
                        os << "Error in converting polynomial to string with n = " << n_ << " and p = " << p_ ;
                        throw PolynomialRangeError( os.str() ) ;
                     }
            } // end coeff != 0.
        } // end for deg
    } // end f(x) = const.

    // Print out the modulus.
    if (!(os << ", " << p_  ))
                     {
                        ostringstream os ;
                        os << "Error in converting polynomial to string: "
                        << " with n = " << n_ << " and p = " << p_ ;
                        throw PolynomialRangeError( os.str() ) ;
                     }

    // Return the string from the stream.
    return os.str() ;
}


/*=============================================================================
|
| NAME
|
|     operator << for Polynomial
|
| DESCRIPTION
|
|     Print a polynomial to the output stream.
|
| EXAMPLE
|
|     try
|     {
|         Polynomial f( "x^2 + 1,3" ) ;
|         cout << f << endl ;
|     }
|     catch( PolynomialRangeError & e )
|     {
|         cout << "Error in outputting polynomial f(x)" << endl ;
|     }
|
+============================================================================*/

ostream & operator<<( ostream & out, const Polynomial & p )
{
    // Cast to polynomial to a string first, then output as a string.
    // May throw a PolynomialRangeError.
    out << static_cast<string>( p ) ;

    return out ;
}


/*=============================================================================
|
| NAME
|
|     Operator >> for Polynomial
|
| DESCRIPTION
|
|     Polynomial stream input.
|
| EXAMPLE
|
|     try
|     {
|
|         Polynomial f ;
|         cin >> f ;
|     }
|     catch( PolynomialRangeError & e )
|     {
|         cout << "Error in inputting polynomial f(x)" << endl ;
|     }
|
+============================================================================*/

istream & operator>>( istream & in, Polynomial & p )
{
    // Input as a string.
    string s ;
    in >> s ;

    // Copy into argument polynomial.  Can throw an exception.
    p = Polynomial( s ) ;

    return in ;
}


/*=============================================================================
 |
 | NAME
 |
 |     Polynomial operator==
 |
 | DESCRIPTION
 |
 |     Polynomial equality test operator.
 |
 | EXAMPLE
 |
 |     try
 |     {
 |         Polynomial f1( "2x^2 + 1, 3" ) ;
 |         Polynomial f2( "2x^2 + 1", 3 ) ;
 |
 |         if (f1 == f2)
 |             cout << "f1 = " << f1 << " == " << f2 << endl ;
 |         else
 |            cout << "f1 = " << f1 << " != " << f2 << endl ;
 |
 +============================================================================*/

bool operator==( const Polynomial & p1, const Polynomial & p2 )
{
    // The degrees and moduli have to match.
    if ((p1.n_ != p2.n_) || (p1.p_ != p2.p_))
       return false ;
        
    // Test coefficients for equality.
    for (int i = 0 ;  i <= p1.n_ ;  ++i)
        if (p1.f_[ i ] != p2.f_[ i ])
            return false ;
    
    return true ;
}

bool operator!=( const Polynomial & p1, const Polynomial & p2 )
{
    return !( p1 == p2) ;
}


/*=============================================================================
|
| NAME
|
|     Polynomial operator[]
|
| DESCRIPTION
|
|     Polynomial indexing operator which allows an lvalue:  f[ 33 ] = 2 ;
|     If we don't have a coefficient of this degree, create it and backfill
|     earlier coefficients with zero.
|
|     Throws an exception if out of bounds.
|
| EXAMPLE
|
|     try
|     {
|         Polynomial f( "2x^2 + 1, 3" ) ;
|
|         f[ 5 ] = 2 ;
|
|         // Now f(x) = 2 x^5 + 0 x^4 + 0 x^3 + 2 x^2 + 0 x + 1
|         // f_.size() => 5 + 1 = 6
|
|     }
|     catch( PolynomialRangeError & e )
|     {
|         cout << "Error in assigning polynomial f(x) coefficient" << endl ;
|     }
|
+============================================================================*/

ppuint & Polynomial::operator[]( int i )
{
    // We attempt to access beyond the current degree.
    if (i > n_)
    {
        try
        {
            // Extend the vector size with zeros.
            f_.resize( i+1, 0 ) ;
            n_ = i ;
        }
        // Failed to resize the polynomial.
        catch( length_error & e )
        {
            throw PolynomialError( "Polynomial.operator[]:  failed to resize", __FILE__, __LINE__ ) ;
        }
    }

    // Return a reference to the coefficient.
    return f_[ i ] ;
}


/*=============================================================================
|
| NAME
|
|     Polynomial operator[]
|
| DESCRIPTION
|
|     Polynomial indexing operator for read only access:  int coeff = f[ 33 ] ;
|     Throws an exception if out of bounds.
|
| EXAMPLE
|
|     try
|     {
|         Polynomial f ;
|         int value = f[ 33 ] ;
|     }
|     catch( PolynomialRangeError & e )
|     {
|         cout << "Error in getting polynomial f(x) coefficient" << endl ;
|     }
|
+============================================================================*/

const ppuint Polynomial::operator[]( int i ) const
{
    // We throw our own exception here.
    if (i > n_)
	 {
		ostringstream os ;
		os << "Error accessing polynomial with coefficients p[0]...p[n] = (" ;
		for (int j = 0 ;  j <= n_ ;  ++j)
			os << f_[ j ] << " " ;
		os << ")" << endl
		   << " at index i = " << i
		   << " of degree n = " << n_ << " modulo p = " << p_ ;
		throw PolynomialRangeError( os.str(), __FILE__, __LINE__ ) ;
	 }

    return f_[ i ] ;
}


/*=============================================================================
|
| NAME
|
|     deg
|     modulus
|     setModulus
|
| DESCRIPTION
|
|     Return the degree of f(x).
|
| EXAMPLE
|
|     try
|     {
|         Polynomial f ;
|         cout << "Degree of f(x) = " << f.deg() << endl ;
|     }
|
+============================================================================*/

int Polynomial::deg() const
{
    return n_ ;
}

// Return the modulus p of f(x).
ppuint Polynomial::modulus() const
{
    return p_ ;
}

void Polynomial::setModulus( ppuint p )
{
    p_ = p ;

    // And the modulus functionoid.
    mod( p_ ) ;
}


/*=============================================================================
|
| NAME
|
|     Polynomial operator+=
|
| DESCRIPTION
|
|     Polynomial sum f(x) += g(x)
|
|
| EXAMPLE
|
|     try
|     {
|         Polynomial f ;
|         Polynomial g ;
|         f += g ;
|     }
|     catch( PolynomialRangeError & e )
|     {
|         cout << "Error in polynomial sum f(x) += g(x)" << endl ;
|     }
|
+============================================================================*/

Polynomial & Polynomial::operator+=( const Polynomial & g )
{
    // f(x) = x^2 +   + 1
    // g(x) =       x + 3
    //
    // f(x) =       x + 3
    // g(x) = x^2 +   + 1
    //
    int minDeg = (n_ < g.n_) ? n_ : g.n_ ;

    // Add coefficients modulo p for smaller degree terms.
    for (int i = 0 ;  i <= minDeg ;  ++i)
        f_[ i ] = mod( f_[ i ] + g.f_[ i ] ) ;

    // If g(x) has larger degree, extend f(x) and copy the coefficients of g(x).
    if (g.n_ > n_)
    {
        // Extend the vector size with zeros.
        try
        {
            f_.resize( g.n_ + 1, 0 ) ;
        }
        // Failed to resize the polynomial.
        catch( length_error & e )
        {
            throw PolynomialError( "Polynomial::operator+= had a length_error exception while resizing the polynomial", __FILE__, __LINE__ ) ;
        }

        for (int i = n_ + 1 ;  i <= g.n_ ;  ++i)
            f_[ i ] = g.f_[ i ] ;
    }

    // Trim leading zero coefficients, but leave a constant term of zero.
    while( f_.back() == 0 && f_.size() > 1)
    {
       f_.pop_back() ;
	   --n_ ;
	}

    // Return current object now containing the sum.
    return *this ;
}


/*=============================================================================
|
| NAME
|
|     Polynomial operator+()
|
| DESCRIPTION
|
|     Add polynomials.
|
+============================================================================*/

const Polynomial operator+( const Polynomial & f, const Polynomial &g )
{
    // Do + in terms of += to maintain consistency.
    // Copy construct temporary copy, then add to it.
    // Return value optimization compiles away the copy constructor.
    // const on return type disallows doing (u+v) = w ;
    return Polynomial( f ) += g ;
}

/*=============================================================================
|
| NAME
|
|     Polynomial operator*=()
|
| DESCRIPTION
|
|     Scalar multiply polynomials.
|
+============================================================================*/

Polynomial & Polynomial::operator*=( const ppuint k )
{
    // Multiply coefficients modulo p.
    for (int i = 0 ;  i <= n_ ;  ++i)
        f_[ i ] = mod( f_[ i ] * k ) ;

    // Return current object now containing the scalar product.
    return *this ;
}


/*=============================================================================
|
| NAME
|
|     Polynomial operator*()
|
| DESCRIPTION
|
|     Scalar multiply polynomials.
|
+============================================================================*/

const Polynomial operator*( const Polynomial & f, const ppuint k )
{
    // Do * in terms of *= to maintain consistency.
    // Copy construct temporary copy, then add to it.
    // Return value optimization compiles away the copy constructor.
    // const on return type disallows doing (u*k) = w ;
    return Polynomial( f ) *= k ;
}


/*=============================================================================
|
| NAME
|
|     Polynomial operator()
|
| DESCRIPTION
|
|     Evaluate the monic polynomial f( x ) with modulo p arithmetic.
|
|               n         n-1
|     f( x ) = x  +  a   x  + ... + a    0 <= a  < p
|                     n-1            0         i
|
| EXAMPLE
|                                  4
|     Let n = 4, p = 5 and f(x) = x  + 3x + 3.
|
|     By Horner's rule, f(x) = (((x + 0)x + 0)x + 3)x + 3.
|
|     Then f(2) = (((2 + 0)2 + 0)2 + 3) = (8 + 3)2 + 3 = 1 + 2 + 3 (mod 5) = 0.
|     and f(3) = (((3 + 0)3 + 0)3 + 3)3 + 3 (mod 5) = 3
|
| METHOD
|
|     By Horner's rule, f(x) = ( ... ( (x + a   )x + ... )x + a .
|                                            n-1               0
|
|     We evaluate recursively, f := f * x + a (mod p), starting
|                                            i
|     with f = 1 and i = n-1.
|
+============================================================================*/

ppuint
Polynomial::operator()( int x )
{
    ppuint val = 1 ;

    for (int degree = n_- 1 ;  degree >= 0 ;  --degree)
        val = mod( val * x + f_[ degree ]) ;

    return( val ) ;
}


/*=============================================================================
|
| NAME
|
|     hasLinearFactor
|
| DESCRIPTION
|
|     Check if the polynomial f(x) has any linear factors.
|
|     Polynomial f ; // A polynomial f(x) of degree n, modulo p.
|     bool hasFactor = f.hasLinearFactor() ;
|
|     hasFactor is true if f( a ) = 0 (mod p) for a = 1, 2, ... p-1,
|     and is false otherwise.
|
|     i.e. check if f(x) contains a linear factor (x - a).  We don't need to test
|     for the root a = 0 because f(x) was chosen in main to have a non-zero
|     constant term, hence no zero root.
|
| EXAMPLE
|                                  4                               2   2
|     Let n = 4, p = 5 and f(x) = x  + 3x + 3.  Then f(x) = (x + 3)  (x + 4x + 2)
|
|      Then f(0) = 3 (mod 5), f(1) = 2 (mod 5), but
|     f(2) = 0 (mod 5), so we exit immediately with a true.
|
|                       4      2
|      However, f(x) = x  + 3 x  + x + 1 is irreducible, so has no linear factors.
|
| METHOD
|
|    Evaluate f(x) at x = 0, ..., p-1 by Horner's rule.  Return instantly the
|    moment f(x) evaluates to 0.
|
+============================================================================*/

bool
Polynomial::hasLinearFactor()
{
    for (int i = 0 ;  i <= p_ - 1 ;  ++i)
        if ((*this)( i ) == 0)
            return( true ) ;

    return( false ) ;
}


/*=============================================================================
|
| NAME
|
|     Polynomial::isInteger
|
| DESCRIPTION
|
|     Return true if a polynomial is a constant.
|
| EXAMPLE
|
|     Polynomial p( "2 x ^ 2 " ) ;
|     p.isInteger -> false
|
|     Polynomial p( "2 " ) ;
|     p.isInteger -> true
|
| METHOD
|
|     A constant polynomial is zero in its first through n th degree
|     terms.  Return immediately with false if any such term is non-zero.
|
+============================================================================*/

bool
Polynomial::isInteger() const
{
    // Degree 0 is constant.
    if (n_ == 0)
        return true ;

    // Not integer if any coefficients above zero degree term are non-zero.
    for (int i = 1 ;  i <= n_ ;  ++i)
        if (f_[ i ] != 0)
            return( false ) ;

    return( true ) ;
}


/*=============================================================================
 |
 | NAME
 |
 |   initialTrialPoly
 |
 | DESCRIPTION
 |
 |     Create an initial monic polynomial
 |                   n
 |         f( x ) = x
 |
 | EXAMPLE
 |                              4
 |      Let n = 4.  Set f(x) = x  - 1.
 |
 |
 +============================================================================*/

void TrialPolynomial::initialTrialPoly( const ppuint n, const ppuint p )
{
    (*this).setModulus(p);

    // Allocate enough coefficients for an nth degree polynomial and
    // initialize all intermediate coefficients to 0.
    
    if (n > numeric_limits<int>::max())
    {
        ostringstream os ;
        os << "Polynomial::initialTrialPoly:  n = " << n << " is too large for an array index" ;
        throw PolynomialRangeError( os.str(), __FILE__,  __LINE__ ) ;
    }
    
    (*this)[ static_cast<int>( n ) ] = 1 ;
    f_[ 0 ] = 0 ;
}


/*=============================================================================
 |
 | NAME
 |
 |     nextTrialPoly
 |
 | DESCRIPTION
 |
 |     Return the next monic polynomial in the sequence after f(x), explained
 |     below
 |
 | EXAMPLE
 |                                            3
 |      Let n = 3 and p = 5.  Suppose f(x) = x  + 4 x + 4.  As a mod p number,
 |      this is 1 0 4 4.  Adding 1 gives 1 0 4 5.  We reduce modulo
 |      5 and propagate the carry to get the number 1 0 5 0.  Propagating
 |      the carry again and reducing gives 1 1 0 0.  The next polynomial
 |                      3    2
 |      after f(x) is  x  + x .
 |
 | METHOD
 |
 |      Think of the polynomial coefficients as the digits of a number written
 |      in base p.  The "next" polynomial is the one you would get by adding 1
 |      to this number in multiple precision arithmetic.  Our intention is to
 |      run through all possible monic polynomials modulo p.
 |
 |      Propagate carries in digits 1 through n-2 when any digit exceeds p.  No
 |      carries take place in the n-1 st digit because our polynomial is monic.
 |
 |      TODO:  Find polynomials in order of Hamming weight?
 |
 +============================================================================*/

void TrialPolynomial::nextTrialPoly()
{
    ++f_[ 0 ] ;     // Add 1, i.e. increment the coefficient of the x term.

    //   Sweep through the number from right to left, propagating carries.  Skip
    //   the constant and the nth degree terms.
    for (int digit_num = 0 ;  digit_num <= n_ - 2 ;  ++digit_num)
    {
        if (f_[ digit_num ] == p_)   //  Propagate carry to next digit.
        {
            f_[ digit_num ] = 0 ;
            ++f_[ digit_num + 1 ] ;
        }
    }
}



/*------------------------------------------------------------------------------
|                              PolyMod Implementation                          |
------------------------------------------------------------------------------*/

/*=============================================================================
 |
 | NAME
 |
 |     PolyMod default constructor
 |
 | DESCRIPTION
 |
 |
 | EXAMPLE
 |
 | METHOD
 |
 +============================================================================*/

PolyMod::PolyMod()
           : g_()
           , f_()
           , powerTable_()
           , mod( f_.modulus() )
{
    constructPowerTable() ;
    modf() ;
}


/*=============================================================================
 |
 | NAME
 |
 |     PolyMod destructor
 |
 | DESCRIPTION
 |
 |
 | EXAMPLE
 |
 | METHOD
 |
 +============================================================================*/

PolyMod::~PolyMod()
{
// Member fields will clean up themselves.
}


/*=============================================================================
 |
 | NAME
 |
 |     PolyMod constructor
 |
 | DESCRIPTION
 |
 |     Given polynomials f( x ) and g( x ) where g is a string,
 |     construct p( x ) = g( x ) mod f( x ).
 |
 | EXAMPLE
 |
 | METHOD
 |
 +============================================================================*/

PolyMod::PolyMod( const string & g, const Polynomial & f )
         : g_( g )
         , f_( f )
         , powerTable_()
         , mod( f.modulus() )
{
    constructPowerTable() ;
    modf() ;
}


/*=============================================================================
 |
 | NAME
 |
 |     PolyMod constructor
 |
 | DESCRIPTION
 |
 |     Given polynomials f( x ) and g( x ), construct p( x ) = g( x ) mod f( x ).
 |
 | EXAMPLE
 |
 | METHOD
 |
 +============================================================================*/

PolyMod::PolyMod( const Polynomial & g, const Polynomial & f )
         : g_( g )
         , f_( f )
         , powerTable_()
         , mod( f.modulus() )
{
    constructPowerTable() ;
    modf() ;
}


/*=============================================================================
 |
 | NAME
 |
 |     PolyMod string operator
 |
 | DESCRIPTION
 |
 |     Given g( x ) mod f( x ), return g( x ) as a string.
 |
 | EXAMPLE
 |
 | METHOD
 |
 +============================================================================*/

// Operator casting to string type.
PolyMod::operator string() const
{
return static_cast<string>( g_ ) ;
}


/*=============================================================================
 |
 | NAME
 |
 |     Operator << for PolyMod
 |
 | DESCRIPTION
 |
 |     Given g( x ) mod f( x ), output g( x ) as a string.
 |
 | EXAMPLE
 |
 | METHOD
 |
 +============================================================================*/

ostream & operator<<( ostream & out, const PolyMod & p )
{
    // Cast to polynomial to a string first, then output as a string.
    // May throw a PolynomialRangeError.
    out << static_cast<string>( p.g_ ) ;

    return out ;
}


/*=============================================================================
 |
 | NAME
 |
 |     getf
 |
 | DESCRIPTION
 |
 |     Given g( x ) mod f( x ), return f( x ).
 |
 | EXAMPLE
 |
 | METHOD
 |
 +============================================================================*/

const Polynomial PolyMod::getf() const
{
    return f_ ;
}


/*=============================================================================
 |
 | NAME
 |
 |     getModulus
 |
 | DESCRIPTION
 |
 |     Given g( x ) mod (f( x ), p) return p.
 |
 | EXAMPLE
 |
 | METHOD
 |
 +============================================================================*/

const ppuint PolyMod::getModulus() const
{
    return f_.modulus() ;
}


/*=============================================================================
 |
 | NAME
 |
 |     PolyMod copy constructor
 |
 | DESCRIPTION
 |
 |     Copy g2 to g( x ) mod (f( x ), p)
 |
 | EXAMPLE
 |
 | METHOD
 |
 +============================================================================*/

PolyMod::PolyMod( const PolyMod & g2 )
         : g_( g2.g_ )
         , f_( g2.f_ )
         , powerTable_( g2.powerTable_ )
         , mod( f_.modulus() )
{
}


/*=============================================================================
 |
 | NAME
 |
 |     operator=
 |
 | DESCRIPTION
 |
 |     PolyMod assignment operator.
 |
 +============================================================================*/

PolyMod & PolyMod::operator=( const PolyMod & g2 )
{
    // Check for assigning to oneself:  just pass back a reference to the unchanged object.
    if (this == &g2)
        return *this ;

    g_ = g2.g_ ;
    g_ = g2.f_ ;

    powerTable_ = g2.powerTable_ ;
    mod = g2.mod ;

    // Return a reference to the altered object.
    return *this ;
}


/*=============================================================================
 |
 | NAME
 |
 |     operator[]
 |
 | DESCRIPTION
 |
 |     Bounds checked indexing operator for read only access:
 |         coeff = p[ i ] ;
 |
 +============================================================================*/

const ppuint PolyMod::operator[]( int i ) const
{
    // Can throw an exception.
    return g_[ i ] ;
}


/*=============================================================================
|
| NAME
|
|     constructPowerTable
|
| DESCRIPTION
|
|     Construct a table of powers of x:
|
|      n                     2n-2
|     x  (mod f(x), p)  ... x    (mod f(x), p)
|
|
|    powerTable_[i][j] is the coefficient of
|     j       n+i
|    x   in  x   (mod f(x), p) where 0 <= i <= n-2 and 0 <= j <= n-1.
|
| EXAMPLE
|                                  4     2                     4
|     Let n = 4, p = 5 and f(x) = x  +  x  +  2x  +  3.  Then x  =
|
|         2                  2
|     -( x  +  2x  + 3) = 4 x  + 3 x + 2 (mod f(x), 5), and we get
|
|      4                    2
|     x  (mod f(x), 5) = 4 x  + 3 x + 2 = powerTable_[0].
|
|      5                       2                 3      2
|     x  (mod f(x), 5) = x( 4 x  + 3 x + 2) = 4 x  + 3 x  + 2x
|                      = powerTable_[1].
|
|      6                       3      2           4      3      2
|     x  (mod f(x), 5) = x( 4 x  + 3 x + 2 x) = 4x  + 3 x  + 2 x
|
|                              2                 3      2
|                      = 4 ( 4x  + 3 x + 2) + 3 x  + 2 x  =
|
|                           3     2
|                      = 3 x + 3 x + 2 x + 3 = powerTable_[2].
|
|                                    j
|     powerTable_[i][j]:       | 0  1  2  3
|                           ---+-------------
|                            0 | 2  3  4  0
|                        i   1 | 0  2  3  4
|                            2 | 3  2  3  3
|
| NOTES
|                              n-1
|     Beginning with t( x ) = x,    compute the next power of x from the last
|                                                         n
|     one by multiplying by x.  If necessary, substitute x  =
|             n-1
|     -( a   x    + ... + a ) to reduce the degree.  This formula comes from
|         n-1              0
|                               n         n-1
|     the observation f( x ) = x   + a   x    + ... + a    = 0 (mod f(x), p).
|                                     n-1              0
|
+============================================================================*/

void PolyMod::constructPowerTable()
{
    // Get hold of the degree of f(x).
    int n = f_.deg() ;

    // Empty the power table.
    powerTable_.clear() ;

    //
    //  t(x) is temporary storage for x ^ k (mod f(x),p)
    //   n <= k <= 2n-2.  Its degree can go as high as
    //   n before it is reduced again.
    Polynomial t ;


    //                         n-1
    //    Initialize t( x ) = x    mod p.
    t[ n-1 ] = 1 ;
    
    // In Microsoft Visual Studio C++ 2008 we get garbage placed in t[ n ] in the loop
    // at j = n in the step
    //     t[ j ] = t[ j-1 ] ;
    // Why?  We first access the value of t[j-1], the compiler places it in a temporary,
    // we then access t[n], and this causes a resize of f_ in Polynomial.operator[],
    // then t[ j ] = garbage since we apparently lose the temporary. Does not happen if
    // we rewrite the step as
	//			int tmp ;
    //   		tmp = t[ j-1 ] ;
    //          t[ j ] = tmp ;
    // or alternatively, we prevent resizing occurring by pre-expanding:
	t[ n   ] = 0 ;  // Expand the size of t(x) now since we'll access t[n] later.  

    t.setModulus( getModulus() ) ;

    try
	{
		//                                      i+n
		//  Fill the ith row of the table with x   (mod f(x), p)
		//  for i = 0 ... n-2.
		//
		for (int i = 0 ;  i <= n - 2 ;  ++i)
		{
			// Compute t(x) = x t(x) by shifting the coefficients
			// to the left and filling with zero.
			for (int j = n ;  j >= 1 ;  --j)
                t[ j ] = t[ j-1 ] ;

			t[ 0 ] = 0 ;

			//  Coefficient of the x ^ n degree term of t(x).
			ppsint coeff = 0 ;
			if ( (coeff = t[ n ]) != 0)
			{
				//  Zero out the x ^ n th term.
				t[ n ] = 0 ;

				//          n       n                        n-1
				// Replace x  with x  (mod f(x), p) = -(a   x   + ... + a )
				//                                         n-1             0
				for (int j = 0 ;  j <= n-1 ;  ++j)

					t[ j ] = mod( t[ j ] +
								  mod( -coeff * f_[ j ]) ) ;
			}  // end if

			//  Copy t(x) into row i of power_table.
			powerTable_.push_back( t ) ;

		} // end for

		#ifdef DEBUG_PP_POLYNOMIAL
        cout << "PowerTable of polynomials x^n ... x^2n-2 mod f(x), p" << endl ;
        cout << "f(x) = " << getf() << " n = " << n << " p = " << getModulus() << endl ;
        for  (int i = n ;  i <= 2*n-2 ;  ++i)
            cout << "powerTable[ x^" << i << " ] = " << powerTable_[ offset(i) ] << endl ;
		#endif
    }
    catch( bad_alloc & e )
    {
        throw PolynomialRangeError( "PolyMod::constructPowerTable had a bad_alloc memory allocation failure", __FILE__, __LINE__ ) ;
    }

    // t will be automagically freed upon exit.
    return ;
}


/*=============================================================================
|
| NAME
|
|     modf
|
| DESCRIPTION
|
|     Reduce g(x) modulo f(x), and p.
|
| EXAMPLE
|                                  4     2
|     Let n = 4, p = 5 and f(x) = x  +  x  +  2x  +  3.
|
|      6                       3      2           4      3      2
|     x  (mod f(x), 5) = x( 4 x  + 3 x + 2 x) = 4x  + 3 x  + 2 x
|
|
+============================================================================*/

void PolyMod::modf()
{
    // Get hold of the degree of f(x).
    int n = f_.deg() ;
    int m = g_.deg() ;

    if (m > 2 * n - 2)
    {
        ostringstream os ;
        os << "Error in PolyMod::modf():  degree of g(x) higher than power table can handle with deg f = " << n 
           << " deg g = " << m << " and p = " << getModulus() ;
        throw PolynomialRangeError( os.str(), __FILE__, __LINE__ ) ;
    }


    //                                      i+n
    //  Fill the ith row of the table with x   (mod f(x), p)
    //  for i = 0 ... n-2.
    //
    for (int i = n ;  i <= m ;  ++i)
    {
        #ifdef DEBUG_PP_POLYNOMIAL
        cout << "\nBefore converting, g( x ) = " << g_ << endl ;
        #endif

        //  Coefficient of the x ^ i degree term of g(x).
        ppuint coeff{ 0 } ;
        if ( (coeff = g_[ i ]) != 0)
        {
            //  Subtract (zero out) the x ^ i th term.
            g_[ i ] = 0 ;

            //          i       i
            // Replace x  with x  (mod f(x), p) from the power table * coeff.
            g_ += (powerTable_[ offset(i) ] * coeff) ;
         }

         #ifdef DEBUG_PP_POLYNOMIAL
         cout << "\nAfter converting with coeff = " << coeff << " g( x ) = " << g_ << endl ;
         #endif

    } // end for

    return ;
}


/*=============================================================================
 |
 | NAME
 |
 |     autoconvolve
 |
 | DESCRIPTION
 |
 |      Compute a convolution-like sum for use in function coeffOfSquare,
 |
 |      upper
 |      ---
 |      \    t  t       But define the sum to be 0 when lower > upper to catch
 |      /     i  k-i    the special cases that come up in function coeffOfSquare.
 |      ---
 |      i=lower
 |
 |      where
 |                                  n-1
 |     Coefficients of t(x) = t    x    + ... + t x  + t
 |                             n-1               1      0
 |
 | EXAMPLE
 |                        3      2
 |      Suppose t(x) = 4 x  +  x  +  3 x  +  3, lower = 1, upper = 3, n = 3,
 |
 |      and p = 5.  For k = 3, autoConvolve = t[ 1 ] t[ 2 ] + t[ 2 ] t[ 1 ] +
 |
 |      t[ 3 ] t[ 0 ] = 3 * 1 + 1 * 3 + 4 * 3 = 18 mod 5 = 3.  For lower = 0,
 |
 |      upper = -1, or for lower = 3 and upper = 2, autoConvolve = 0, no matter what
 |
 |      k is.
 |
 | METHOD
 |
 |     A "for" loop in the C language is not executed when its lower limit
 |
 |     exceeds its upper limit, leaving sum = 0.
 |
 +============================================================================*/

ppuint autoConvolve( const Polynomial & t, int k, int lower, int upper )
{
    ModP<ppuint,ppsint> mod( t.modulus() ) ;
    int deg_t = t.deg() ;

    ppuint sum { 0 } ;
    for (int i = lower ;  i <= upper ;  ++i)
    {
        // Coeff is zero if higher or lower than degree of polynomial.
        ppuint coeff_ti{ 0u } ;
        ppuint coeff_tkmi{ 0u } ;

        if (i <= deg_t && i >= 0)
            coeff_ti = t[ i ] ;

        if (k-i <= deg_t && k-i >= 0)
            coeff_tkmi = t[ k - i ] ;

        sum = mod( sum + mod( coeff_ti * coeff_tkmi )) ;
    }

    return( sum ) ;
}


/*=============================================================================
 |
 | NAME
 |
 |     convolve
 |
 | DESCRIPTION
 |
 |      Compute a convolution-like sum,
 |
 |      upper
 |      ---
 |      \    s  t       But define the sum to be 0 when lower > upper to catch
 |      /     i  k-i    the special cases
 |      ---
 |      i=lower
 |
 |      where
 |                                   n-1
 |      Coefficients of s(x) = s    x    + ... + s x  + s
 |                              n-1               1      0
 |                                      n-1
 |      Coefficients of t(x) = t    x    + ... + t x  + t
 |                              n-1               1      0
 |
 |      0 <= k <= 2n - 2
 |      0 <= lower <= n-1
 |      0 <= upper <= n-1
 |
 | EXAMPLE
 |                        3      2
 |      Suppose s(x) = 4 x  +  x  +  3 x  +  3,
 |
 |                        3     2
 |      Suppose t(x) = 4 x  +  x  +  3 x  +  3,
 |
 |
 |      lower = 1, upper = 3, n = 3,
 |
 |      and p = 5.  For k = 3, convolve = t[ 1 ] t[ 2 ] + t[ 2 ] t[ 1 ] +
 |
 |      t[ 3 ] t[ 0 ] = 3 * 1 + 1 * 3 + 4 * 3 = 18 mod 5 = 3.  For lower = 0,
 |
 |      upper = -1, or for lower = 3 and upper = 2, convolve = 0, no matter what
 |
 |      k is.
 |
 | METHOD
 |
 |     A "for" loop in the C language is not executed when its lower limit
 |
 |     exceeds its upper limit, leaving sum = 0.
 |
 +============================================================================*/

ppuint convolve( const Polynomial & s, const Polynomial & t,
               const int k, const int lower, const int upper )
{
    ppuint sum{ 0 } ;     // Convolution sum.

    ModP<ppuint,ppsint> mod( s.modulus() ) ; // Initialize the functionoid.

    int deg_s{ s.deg() } ;
    int deg_t{ t.deg() } ;

    for (int i = lower ;  i <= upper ;  ++i)
    {
        // Coeff is zero if higher or lower than degree of polynomial.
        ppuint coeff_s{ 0u } ;
        ppuint coeff_t{ 0u } ;

        if (i <= deg_s && i >= 0)
            coeff_s = s[ i ] ;

        if (k-i <= deg_t && k-i >= 0)
            coeff_t = t[ k- i ] ;

       sum = mod( sum + mod( coeff_s * coeff_t )) ;
    }

    return( sum ) ;
}


/*=============================================================================
 |
 | NAME
 |
 |     coeffOfSquare
 |
 | DESCRIPTION
 |                                     th                2
 |      Return the coefficient of the k   power of x in g ( x )  modulo p,
 |      given of g(x) of degree <= n-1.
 |
 |      where 0 <= k <= 2n-2
 |
 | EXAMPLE
 |                                     3     2                 2
 |     Let n = 4, p = 5, and g(x) = 4 x  +  x  + 3 x + 3.  g(x) =
 |
 |      6      5
 |     x  + 3 x + 3 x + 3 x + 4, all modulo 5.
 |
 |             k        |  0  1  2  3  4  5  6
 |      ----------------+---------------------
 |      coeffOfSquare   |  4  3  0  0  0  3  1
 |
 | METHOD
 |                                                          2
 |     The formulas were gotten by writing out the product g (x) explicitly.
 |
 |     The sum is 0 in two cases:
 |
 |         (1) when k = 0 and the limits of summation are 0 to -1
 |
 |         (2) k = 2n - 2, when the limits of summation are n to n-1.
 |
 |     To derive the formulas, let
 |
 |                      n-1
 |     Let g(x) = g    x     +  ... + g x + g
 |                 n-1                 1     0
 |
 |     Look at the formulas in coeffOfProduct for each power of x,
 |        replacing s with t, and observe that half of the terms are
 |        duplicates, so we can save computation time.
 |
 |     Inspection yields the formulas,
 |
 |     for 0 <= k <= n-1, even k,
 |
 |      k/2-1
 |       ---             2
 |    2  \   g  g     + g
 |       /    i  k-i     k/2
 |       ---
 |       i=0
 |
 |     for 0 <= k <= n-1, odd k,
 |
 |       (k-1)/2
 |       ---
 |     2 \   g  g
 |       /    i  k-i
 |       ---
 |       i=0
 |
 |     and for n <= k <= 2n-2, even k,
 |
 |       n-1
 |       ---            2
 |    2  \   g  g    + g
 |       /    i  k-i    k/2
 |       ---
 |       i=k/2+1
 |
 |       and for n <= k <= 2n-2, odd k,
 |
 |       n-1
 |       ---
 |    2  \   g  g
 |       /    i  k-i
 |       ---
 |       i=(k+1)/2
 |
 +============================================================================*/

ppuint coeffOfSquare( const Polynomial & g, const int k, const int n )
{
    ModP<ppuint,ppsint> mod( g.modulus() ) ; // Initialize the functionoid.

                        //                          2
    ppuint sum { 0 } ;      // kth coefficient of g( x )

    // Coeff is zero if higher or lower than degree of polynomial.
    ppuint coeff_gkd2 { 0 } ;
    if (k/2 <= g.deg() && k/2 >= 0)
        coeff_gkd2 = g[ k/2 ] * g[ k/2 ] ;

    if (0 <= k && k <= n-1)
    {
        if (k % 2 == 0)        // Even k
            sum = mod( mod( 2 * autoConvolve( g, k, 0, k/2 - 1) ) + coeff_gkd2 ) ;

         else                  // Odd k
             sum = mod( 2 * autoConvolve( g, k, 0, (k-1)/2)) ;
    }
    else if (n <= k && k <= 2 * n - 2)
    {

        if (k % 2 == 0)        // Even k
            sum = mod( mod( 2 * autoConvolve( g, k, k/2 + 1, n-1)) + coeff_gkd2 ) ;

         else                  // Odd k
             sum = mod( 2 * autoConvolve( g, k, (k+1)/2, n-1)) ;
    }

    return( sum ) ;
}


/*=============================================================================
 |
 | NAME
 |
 |     coeffOfProduct
 |
 | DESCRIPTION
 |                                     th
 |      Return the coefficient of the k   power of x in s( x ) t( x )  modulo p.
 |
 | EXAMPLE
 |                               3     2                  2
 |   Let n = 4, p = 5, t(x) = 4 x  +  x  + 4, s( x ) = 3 x  + x + 2
 |
 |                            5      4      3      2
 |   then s ( x ) t( x ) = 2 x  + 2 x  + 4 x  + 4 x  + 4 x + 3
 |
 |   We'll do the case k=3,
 |
 |   t3 s0 + t2 s1 + t1 s2 + t0 s3 = 4 * 2 + 1 * 1 + 0 * 3 + 4 * 0 = 9 = 4 (mod 5).
 |
 |             k       |  0  1  2  3  4  5  6
 |      -----------------+---------------------
 |      coeffOfProduct |  3  4  4  4  2  2  0
 |
 | METHOD
 |
 |     The formulas were gotten by writing out the product s(x) t (x) explicitly.
 |
 |     The sum is 0 in two cases:
 |
 |         (1) when k = 0 and the limits of summation are 0 to -1
 |
 |         (2) k = 2n - 2, when the limits of summation are n to n-1.
 |
 |
 |     To derive the formulas, let
 |
 |                       n-1
 |     Let s (x) = s    x     +  ... + s x + s  and
 |                  n-1                 1     0
 |
 |                       n-1
 |         t (x) = t    x     +  ... + t x + t
 |                  n-1                 1     0
 |
 |     and multiply out the terms, collecting like powers of x:
 |
 |
 |     Power of x     Coefficient
 |     ==========================
 |      2n-2
 |     x              s    t
 |                     n-1  n-1
 |
 |      2n-3
 |     x              s    t    +  s    t
 |                     n-2  n-1     n-1  n-2
 |
 |      2n-4
 |     x              s    t    +  s    t    +  s    t
 |                     n-3  n-1     n-2  n-2     n-3  n-1
 |
 |      2n-5
 |     x              s    t    +  s    t    +  s    t    +  s    t
 |                     n-4  n-1     n-3  n-2     n-2  n-3     n-1  n-4
 |
 |      . . .
 |
 |      n
 |     x              s  t    +  s  t    + ...  +  s    t
 |                     1  n-1     2  n-2            n-1  1
 |
 |      n-1
 |     x              s  t    +  s  t    + ...  +  s    t
 |                     0  n-1     1  n-2            n-1  0
 |
 |     . . .
 |
 |      3
 |     x              s  t  +  s  t  +  s  t  +  s  t
 |                     0  3     1  2     2  1     3  0
 |
 |      2
 |     x              s  t  +  s  t  +  s  t
 |                     0  2     1  1     2  0
 |
 |
 |     x              s  t  +  s  t
 |                     0  1     1  0
 |
 |     1              s  t
 |                     0  0
 |
 |
 |     Inspection yields the formulas,
 |
 |
 |     for 0 <= k <= n-1,
 |
 |       k
 |      ---
 |      \   s  t
 |      /    i  k-i
 |         ---
 |      i=0
 |
 |
 |     and for n <= k <= 2n-2,
 |
 |      n-1
 |      ---
 |      \   s  t
 |      /    i  k-i
 |      ---
 |     i=k-n+1
 |
 +============================================================================*/

ppuint coeffOfProduct( const Polynomial & s, const Polynomial & t, const int k, const int n )
{
    // Check if p is the same for s and t, and check the degree of s and t are < n.
	if (s.modulus() != t.modulus() || s.deg()> n || t.deg() > n)
	    throw PolynomialRangeError( "coeffOfProduct:  degree or modulus doesn't agree for polynomials s and t",
                                   __FILE__, __LINE__ ) ;

    ppuint sum { 0 } ;      // kth coefficient of t(x) ^ 2.

    if (0 <= k && k <= n-1)
    {
        sum = convolve( s, t, k, 0, k ) ;
    }
    else if (n <= k && k <= 2 * n - 2)
    {
        sum = convolve( s, t, k, k - n + 1, n - 1 ) ;
    }

    return( sum ) ;
}


/*=============================================================================
 |
 | NAME
 |
 |     *
 |
 | DESCRIPTION
 |
 |      Compute s( x ) t( x ) (mod f(x), p)
 |
 |      s(x), of degree <= n-1.
 |      t(x), of degree <= n-1.
 |
 |      Uses a precomputed table of powers of x,
 |      powerTable contains x ^ k (mod f(x), p) for n <= k <= 2n-2, f monic.
 |
 | EXAMPLE
 |                                      3    2                 2
 |     Let n = 4 and p = 5, t( x ) = 4 x  + x + 4, s( x ) = 3 x + x + 2
 |
 |                             5      4     3     2
 |     Then s( x ) t( x ) = 2 x  + 2 x + 4 x + 4 x + 4 x + 3, modulo 5,
 |
 |                                          4   2
 |     and after reduction modulo f( x ) = x + x + 2 x + 3, using the power
 |
 |                        4      2               5      3      2
 |     table entries for x  = 4 x + 3 x + 2 and x  = 4 x  + 3 x + 2 x, we get
 |
 |                                        3      2
 |     s( x ) t( x ) (mod f( x ), p) = 2 x  + 3 x  + 4 x + 2
 |
 |
 | METHOD
 |
 |     Compute the coefficients using the function coeffOfProduct.
 |
 |     The next step is to reduce s(x) t(x) modulo f(x) and p.  To do so, replace
 |
 |                            k                                      k
 |     each non-zero term t  x,  n <= k <= 2n-2, by the term t * [ x   mod f(x), p)]
 |                         k                                  k
 |
 |     which we get from the array powerTable.
 |
 +============================================================================*/

const PolyMod operator*( const PolyMod & s,
                         const PolyMod & t )
{
    // Do * in terms of *= to maintain consistency.
    // Return value optimization compiles away the copy constructor.
    // const on return type disallows doing (u*v) = w ;
    return PolyMod( s ) *= t ;
}


/*=============================================================================
 |
 | NAME
 |
 |    *=
 |
 | DESCRIPTION
 |
 |     C-like multiply by operator
 |
 +============================================================================*/

PolyMod &
PolyMod::operator*=( const PolyMod & t )
{
    int i, j ;   //                 k             2
    ppuint coeff;  // Coefficient of x  term of t(x)

    // Temporary storage for the new t(x).  Can have degree up to n.
    Polynomial temp ;

    // Get hold of the degree of f(x).
    int n = f_.deg() ;

    //                               0        n-1
    //  Compute the coefficients of x , ..., x.   These terms do not require
    //  reduction mod f(x) because their degree is less than n.
    for (i = 0 ;  i <= n ;  ++i)
        temp[ i ] = coeffOfProduct( g_, t.g_, i, n ) ;

    //                               n        2n-2             k
    //  Compute the coefficients of x , ..., x.    Replace t  x  with
    //                                                      k
    //          k
    //  t  * [ x  (mod f(x), p) ] from array powerTable when t is
    //   k                                                    k
    //  non-zero.
    for (i = n ;  i <= 2 * n - 2 ;  ++i)
        if ( (coeff = coeffOfProduct( g_, t.g_, i, n)) != 0 )
            for (j = 0 ;  j <= n - 1 ;  ++j)
                temp[ j ] = mod( temp[ j ] +
                                 mod( coeff * powerTable_[ offset(i) ] [ j ])) ;

    for (i = 0 ;  i <= n - 1 ;  ++i)
        g_[ i ] = temp[ i ] ;

    // Return (reference to) the product.
    return *this ;
}


/*=============================================================================
 |
 | NAME
 |
 |     timesX
 |
 | DESCRIPTION
 |
 |      Compute x g(x) (mod f(x), p)
 |
 | EXAMPLE
 |
 |     g.timesX( t ) ;
 |
 | EXAMPLE
 |                                     3       2
 |     Let n = 4, p = 5, and g(x) = 2 x  +  4 x  + 3 x.  Let f(x) =
 |      4    2                                  4      3      2
 |     x  + x  + 2 x + 3.  Then x t (x) = 2 x  + 4 x  + 3 x  and
 |                                      2                3     2
 |     x g(x) (mod f(x), p) = 2 * (4 x + 3 x + 2) + 4 x + 3 x  =
 |        3    2
 |     4 x  + x + x + 4.
 |          3     2
 |     = 4 x + 2 x + 3 x + 2.
 |
 | METHOD
 |
 |     Uses a precomputed table of powers of x.
 |
 |                           n-1         n-2
 |     Multiply g(x) = g    x   +  g    x   + ... + g  by shifting the coefficients
 |                      n-1         n-2              0
 |
 |                          n
 |     to the left.  If an x   term appears, eliminate it by
 |
 |     substitution using powerTable.
 |
 +============================================================================*/

void PolyMod::timesX()
{
    int n = f_.deg() ;

    #ifdef DEBUG_PP_POLYNOMIAL
    cout << "timesX:  g( x ) = " << g_ << endl ;
    #endif

    //  Multiply g(x) by x by shifting the coefficients left in the array, giving
    //         n-1
    //   g    x    + ... + g  x + 0
    //    n-2               1
    //
    // but save the coefficient g    first before overwriting it.
    //                           n-1
    ppuint g_coeff{ g_[ n - 1 ] } ;

    for (int i = n-1 ;  i >= 1 ;  --i)
        g_[ i ] = g_[ i-1 ] ;

    g_[ 0 ] = 0 ;

    //                 n                n
    //    Replace g   x  with g    * [ x  (mod f(x), p) ] using
    //             n-1         n-1
    //     n
    //    x  from powerTable

    if (g_coeff != 0)
    {
        for (int i = 0 ;  i <= n - 1 ;  ++i)
            g_[ i ] = mod( g_[ i ] +
                           mod( g_coeff * powerTable_[ offset(n) ] [ i ] )) ;
    }

    #ifdef DEBUG_PP_POLYNOMIAL
    cout << "timesX:  x g( x ) = " << g_ << endl ;
    #endif
}


/*=============================================================================
 |
 | NAME
 |
 |      square
 |
 | DESCRIPTION
 |
 |               2
 |      Compute g (x) (mod f(x), p)
 |
 | EXAMPLE
 |
 |     g.square() ;
 |
 |
 | EXAMPLE
 |                                     3     2
 |     Let n = 4, p = 5, and g(x) = 4 x  +  x  + 4.  Let f(x) =
 |
 |      4    2                   2       6      5     4      3     2
 |     x  + x  + 2 x + 3.  Then t (x) = x  + 3 x  +  x  + 2 x + 3 x +  1
 |
 |     Now subsituting powers of x modulo f(x) from the power table,
 |
 |       2                         3     2
 |      t (x) (mod f(x), p) =  (3 x + 3 x + 2 x + 3) +
 |
 |             3      2                 2                3     2
 |     3 * (4 x  + 3 x + 2 x) + 4 * (4 x + 3 x + 2) + 4 x + 4 x + 3 x + 1
 |
 |          3     2
 |     = 2 x + 4 x + x + 1.
 |
 |
 | METHOD
 |
 |     Uses a precomputed table of powers of x.
 |
 |
 |          2            2n-2              n         n-1
 |     Let g (x) = g    x     +  ... + g  x  +  g   x   +  ... + g .
 |                  2n-2                n        n-1              0
 |
 |     Compute the coefficients g  using the function coeffOfSquare.
 |                               k
 |
 |                                 2
 |     The next step is to reduce g (x) modulo f(x).  To do so, replace
 |
 |                            k                                      k
 |     each non-zero term g  x,  n <= k <= 2n-2, by the term g * [ x   mod f(x), p)]
 |                         k                                  k
 |
 |     which we get from the array powerTable_.
 |
 +============================================================================*/

void PolyMod::square()
{
    // Get hold of the degree of f(x).
    int n = f_.deg() ;

    #ifdef DEBUG_PP_POLYNOMIAL
    cout << "square:  g( x ) = " << g_ << endl ;
    #endif

    // Temporary storage for the new g(x).  Can have degree up to n.
    Polynomial t ;

    //                               0        n-1
    //  Compute the coefficients of x , ..., x.   These terms do not require
    //
    //  reduction mod f(x) because their degree is less than n.
    for (int i = 0 ;  i <= n ;  ++i)
        t[ i ] = coeffOfSquare( g_, i, n ) ;

    //                               n        2n-2             k
    //  Compute the coefficients of x , ..., x.    Replace g  x  with
    //          k                                           k
    //  g  * [ x  (mod f(x), p) ] from array powerTable_ when g is
    //   k                                                     k
    //  non-zero.
    for (int i = n ;  i <= 2 * n - 2 ;  ++i)
    {
        ppuint coeff{ 0 } ;

        if ( (coeff = coeffOfSquare( g_, i, n )) != 0 )

            for (int j = 0 ;  j <= n- 1 ;  ++j)

                t[ j ] = mod( t[ j ] + mod( coeff * powerTable_[ offset(i) ] [ j ])) ;
    }

    for (int i = 0 ;  i <= n - 1 ;  ++i)

        g_[ i ] = t[ i ] ;

    #ifdef DEBUG_PP_POLYNOMIAL
    cout << "square:  g( x ) ^ 2 = " << g_ << endl ;
    #endif
}


/*=============================================================================
 |
 | NAME
 |
 |     power
 |
 | DESCRIPTION
 |                       m
 |      Compute g(x) = x  (mod f(x), p).
 |
 | EXAMPLE
 |                               4    2
 |     Let n = 4, p = 5, f(x) = x  + x  + 2 x + 3, and m = 156.
 |
 |     156 = 0  . . . 0  1  0  0  1  1  1  0  0 (binary representation)
 |           |<- ignore ->| S  S SX SX SX  S  S (exponentiation rule,
 |                                               S = square, X = multiply by x)
 |      m
 |     x  (mod f(x), p) =
 |
 |          2     2
 | 6   S   x  =  x
 |
 |          4       2
 | 5   S   x  =  4 x  + 3 x + 2
 |
 |
 |          8       3      2
 | 4   S   x  =  4 x  + 4 x + 1
 |
 |          9       3     2
 | 4   X   x  =  4 x  +  x + 3 x + 3
 |
 |
 |          18       2
 | 3   S   x  =  2 x  +  x + 2
 |
 |          19      3     2
 | 3   X   x  =  2 x  +  x  + 2 x
 |
 |
 |          38      3       2
 | 2   S   x  =  2 x  +  4 x  +  3 x
 |
 |          39      3       2
 | 2   X   x  =  4 x  +  2 x  +  x + 4
 |
 |
 |          78      3       2
 | 1   S   x  =  4 x  +  2 x  +  3 x + 2
 |
 |          156
 | 0   S   x    = 3
 |
 | METHOD
 |
 |     Exponentiation by repeated squaring, using precomputed table of
 |     powers.  See ART OF COMPUTER PROGRAMMING, vol. 2, 2nd Ed.,
 |     D. E. Knuth,  pgs 441-443.
 |
 |      n         2n-2
 |     x,  ... , x    (mod f(x), p)
 |
 |                     m
 |     to find g(x) = x   (mod f(x), p), expand m into binary,
 |
 |            k        k-1
 |     m = a 2  + a   2    + . . . + a 2 + a
 |          k      k-1                1     0
 |
 |                             m
 |     where a = 1, and split x   apart into
 |            k
 |
 |             k      k
 |            2      2  a             2 a    a
 |      m                k-1             1    0
 |     x  =  x     x           . . .  x     x
 |
 |
 |     Then to raise x to the mth power, do
 |
 |
 |     t( x ) = x
 |
 |     return if m = 1
 |
 |
 |     for i = k-1 downto 0 do
 |
 |                    2
 |         t(x) = t(x)  (mod f(x), p)       // Square each time.
 |
 |         if a = 1 then
 |             i
 |
 |             t(x) = x t(x) (mod f(x), p)  // Times x only if current bit is 1
 |         endif
 |
 |     endfor
 |                                                         k
 |                                                        2
 |     The initial t(x) = x gets squared k times to give x  .  If a  = 1 for
 |                                                                 i
 |     0 <= i <= k-1, we multiply by x which then gets squared i times more
 |
 |               i
 |              2
 |     to give x .  On a binary computer, we use bit shifting and masking to
 |
 |     identify the k bits  { a    . . .  a  } to the right of the leading 1
 |                             k-1         0
 |
 |     bit.  There are log ( m ) - 1 squarings and (number of 1 bits) - 1
 |                            2
 |     multiplies.
 |
 +============================================================================*/

const PolyMod power( const PolyMod & g1, const BigInt & m )
{
    // Return if g(x) != x
    if (g1.f_.deg() == 1 && g1[ 0 ] == 0 && g1[ 1 ] == 1)
    {
        ostringstream os ;
        os << "Error in PolyMod::power():  g( x ) != x "
           << "with deg g = " << g1.f_.deg() << " m = " << m ;
        throw PolynomialRangeError( os.str(), __FILE__, __LINE__ ) ;
    }

    // Exit right away if m = 1 and return a copy of g(x).
    PolyMod g( g1 ) ;

    if (m == static_cast<BigInt>( 1u ))
        return g ;

    // Find the number of the leading bit.
    int bitNum = m.maxBitNumber() ; // Number of highest possible bit.

    #ifdef DEBUG_PP_POLYNOMIAL
    cout << "initial max bitNum = " << bitNum << endl ;
    cout << "g( x ) = " << g << endl ;
    #endif

    while (!m.testBit( bitNum ))
        --bitNum ;

    #ifdef DEBUG_PP_POLYNOMIAL
    cout << "after skipping leading 0 bits, bitNum = " << bitNum << endl ;
    #endif

    if (bitNum == -1)
    {
        ostringstream os ;
        os << "PolyMod::x_to_power " << "bitNum == -1 internal error in PolyMod" ;
        throw PolynomialRangeError( os.str(), __FILE__, __LINE__ ) ;
    }

    #ifdef DEBUG_PP_POLYNOMIAL
    cout << "\nAfter skipping zero bits, bitNum = " << bitNum << endl ;
    #endif

    //  Exponentiation by repeated squaring.  Discard the leading 1 bit.
    //  Thereafter, square for every 0 bit;  square and multiply by x for
    //  every 1 bit.
    while ( --bitNum >= 0 )
    {
        g.square() ;

        if (m.testBit( bitNum ))
           g.timesX() ;

        #ifdef DEBUG_PP_POLYNOMIAL
        cout << "S " ;
        if (m.testBit( bitNum ))
            cout << "X " ;
        cout << "Bit num = " << bitNum << " g( x ) = " << g << endl ;
        #endif
    }

    #ifdef DEBUG_PP_POLYNOMIAL
    cout << "Out of the loop bitNum = " << bitNum << " g( x ) = " << g << endl ;
    #endif

    return g ;
}


/*=============================================================================
 |
 | NAME
 |
 |    isInteger
 |
 | DESCRIPTION
 |
 |    Getter function.
 |
 +============================================================================*/

bool PolyMod::isInteger() const
{
    return g_.isInteger() ;
}



/*------------------------------------------------------------------------------
|                            PolyOrder Implementation                          |
------------------------------------------------------------------------------*/

/*=============================================================================
 |
 | NAME
 |
 |    PolyOrder()
 |
 | DESCRIPTION
 |
 |    Set a new value of f(x) with same degree n and modulus p.
 |
 +============================================================================*/

void PolyOrder::resetPolynomial( const Polynomial & f )
{
    f_ = f ;
}


/*=============================================================================
 |
 | NAME
 |
 |     PolyOrder()
 |
 | DESCRIPTION
 |
 |     Initialize.  Mainly do the prime factoring.
 |
 +============================================================================*/

PolyOrder::PolyOrder( const Polynomial & f )
             : f_( f )
             , n_( f.deg() )
             , p_( f.modulus() )
             , mod( f.modulus() )
             , p_to_n_minus_1_( BigInt( 0u ))
             , r_( 0u )
             , a_( 0u )
             , factors_of_p_to_n_minus_1_()
             , factors_of_R_()
             , num_prim_poly_( 0u )
             , max_num_poly_( 0u )
             , Q_( 0 )
             , nullity_( 0 )
             , statistics_()
{
    // This is the most time consuming step for large n:
    //               n
    //              p  - 1
    //  Compute r = -------- and factor r into the product of primes.
    //              p - 1
    try
    {
        computeMaxNumPoly() ;
        factorR() ;
        computeNumPrimPoly() ;
    }
    catch( BigIntMathError & e )
    {
        ostringstream os ;
        os << "PolyOrder: problem computing p^n or r = (p^n - 1 )/ (p - 1), or factoring r, or finding EulerPhi( p^n - 1 )/ n "
           << " p = " << p_ << " n = " << n_
           << " [ " << e.what() << " ] " ;
       throw PolynomialRangeError( os.str(), __FILE__, __LINE__ ) ;
    }

    // Copy the factoring statistics, and others.
    statistics_ = factors_of_R_.statistics_ ;
    statistics_.p = p_ ;
    statistics_.n = n_ ;
    statistics_.max_num_possible_poly = max_num_poly_ ;
    statistics_.num_primitive_poly = num_prim_poly_ ;

    // Prepare the Q matrix to the proper size.
    try
    {
        Q_.clear() ;
        Q_.resize( n_ ) ;

        for (int row = 0 ;  row < n_ ;  ++row)
        {
            Q_[ row ].resize( n_ ) ;
        }

    }
    // Failed to resize Q matrix.
    catch( length_error & e )
    {
        throw PolynomialError( "PolyOrder::PolyOrder had a length_error exception and failed to allocate the Q matrix", __FILE__, __LINE__ ) ;
    }
}


 /*=============================================================================
  |
  | NAME
  |
  |    factorR
  |
  | DESCRIPTION
  |
  |    This is the most time consuming step for large n due to the integer
  |    factorization.
  |
  |                                             n
  |    Find the maximum number of polynomials  p
  |
  |    Find
  |              n
  |             p  - 1
  |        r = --------
  |             p - 1
  |
  |    Compute the prime factorization of r.
  |                                                 n
  |    Find number of primitive polynomials = Phi( p - 1 ) / n
  |
  | EXAMPLE
  |
  |    See the examples in the code below.
  |
  +============================================================================*/

void PolyOrder::factorR()
{
       //  n
       // p - 1
       p_to_n_minus_1_ = BigInt( max_num_poly_ - static_cast<BigInt>( 1u ) ) ;
     
      //         n
      // Factor p  - 1 into primes.
      // Pass in p and n in case we can do a fast table lookup.
      factors_of_p_to_n_minus_1_ = Factorization<BigInt>( p_to_n_minus_1_,
                                                        FactoringAlgorithm::Automatic, p_, n_ ) ;
     
     //       n
     //      p - 1
     // r = -------
     //      p - 1
     r_ = p_to_n_minus_1_ / (p_ - 1u) ;
     
    #ifdef DEBUG_PP_FACTOR
    cout << "p = " << p_ << endl ;
    cout << "n = " << n_ << endl ;
    cout << "p^n = " << max_num_poly_ << endl ;
    cout << "r = (p^n-1)/(p-1) = " << r_ << endl ;
    cout << "p_to_n_minus_1 = " << p_to_n_minus_1_ << endl ;
    cout << "factorization of p^n - 1 = " << endl ;
    for (unsigned int i = 0 ;  i < factors_of_p_to_n_minus_1_.numDistinctFactors() ;  ++i)
         cout << factors_of_p_to_n_minus_1_.primeFactor( i ) << " ^ " << factors_of_p_to_n_minus_1_.multiplicity( i ) << endl ;
    #endif // DEBUG_PP_FACTOR
     
     //                                                 n
     // Factor r by starting with the factorization of p - 1
     
     // Now we have to divide out all factors of (p - 1).
     // e.g.
     //
     //  n        8       5  2
     // p - 1 = 19 - 1 = 2  3  5  17 181 3833
     //
     //                      2
     // p - 1 = 19 - 1 = 2  3
     //
     //  n        8       4
     // p - 1 = 19 - 1 = 2     5  17 181 3833
     // -----   ------
     // p - 1   19 - 1
     //
     
     //                           n
     // Copy over the factors of p  - 1
     factors_of_R_ = factors_of_p_to_n_minus_1_ ;
     
     // We're done if p - 1 = 1.
     if (p_ > 2)
     {
         // Factor p - 1 into primes.
         Factorization<BigInt> factors_of_p_minus_1( static_cast<BigInt>( p_ - 1u ) ) ;
         
         #ifdef DEBUG_PP_FACTOR
         cout << "factorization of p - 1 = " << endl ;
         for (unsigned int i = 0 ;  i < factors_of_p_minus_1.numDistinctFactors() ;  ++i)
              cout << factors_of_p_minus_1.primeFactor( i ) << " ^ " << factors_of_p_minus_1.multiplicity( i ) << endl ;
         #endif // DEBUG_PP_FACTOR

         //                                             n                   n
         // p-1 cannot have more distinct factors than p - 1 since p - 1 | p  - 1
         if (factors_of_p_minus_1.numDistinctFactors() > factors_of_p_to_n_minus_1_.numDistinctFactors())
         {
             ostringstream os ;
             os << "factorR "
                << " number of distinct prime factors for p-1   = " << factors_of_p_minus_1.numDistinctFactors() << " > "
                << " number of distinct prime factors for p^n-1 = " << factors_of_p_to_n_minus_1_.numDistinctFactors()
                << " which is not possible since (p-1) | (p^n - 1)" ;
             throw BigIntUnderflow( os.str(), __FILE__, __LINE__ ) ;
         }

         // Divide out p-1, one prime factor at a time.
         for (int i = 0, j = 0 ;  i < factors_of_p_minus_1.numDistinctFactors() ;  ++i)
         {
             BigInt factor_of_p_m_1 = factors_of_p_minus_1.primeFactor( i ) ;
             BigInt factor_of_r     = factors_of_R_.primeFactor( j ) ;
             
             // Divide out the common prime factor.  Advance to next prime factor in the numerator.
             if (factor_of_p_m_1 == factor_of_r)
             {
                 factors_of_R_[ j ].count_ -= factors_of_p_minus_1[ i ].count_ ;
                 ++j ;
             }
             // Factor in denominator < factor in numerator.  Advance to next factor in denominator.
             else if (factor_of_p_m_1 > factor_of_r)
                 continue ;
             // Factor in denominator > factor in numerator.  Advance to next factor in numerator.  All smaller factors in numerator should have been divided out already.
             else
                 ++j ;
             
             #ifdef DEBUG_PP_FACTOR
             cout <<  " i = " << i << " prime factor of p-1 = " << factor_of_p_m_1  << "    j = " << j << " prime factor num = " << factor_of_r << endl ;
             #endif // DEBUG_PP_FACTOR
         }

        #ifdef DEBUG_PP_FACTOR
         cout << "factorization of r = " << endl ;
         for (unsigned int i = 0 ;  i < factors_of_R_.numDistinctFactors() ;  ++i)
             cout << factors_of_R_.primeFactor( i ) << " ^ " << factors_of_R_.multiplicity( i ) << endl ;
        #endif // DEBUG_PP_FACTOR
     }
     
     return ;
}


/*=============================================================================
 |
 | NAME
 |
 |    computeMaxNumPoly
 |
 | DESCRIPTION
 |                                                                   n
 |    Maximum number of possible polynomials of degree n modulo p = p
 |
 | EXAMPLE
 |
 +============================================================================*/

void PolyOrder::computeMaxNumPoly()
{
    max_num_poly_ = power( p_, n_ ) ;

    return ;
}


/*=============================================================================
 |
 | NAME
 |
 |    computeNumPrimPoly
 |
 | DESCRIPTION
 |                                                 n
 |    Find number of primitive polynomials = Phi( p - 1 ) / n
 |
 | EXAMPLE
 |
 |    See the examples in the code below.
 |
 +============================================================================*/

void PolyOrder::computeNumPrimPoly()
{
    //                                                     n
    // Compute the number of primitive polynomials = Phi( p - 1 ) / n
    //
    // Recall Euler's totient is
    //
    //              -----               -----             -----
    // Phi[ n ] = n  | | (1 - 1/p ) = n  | |  (p - 1)  /   | |  p
    //                           i              i                i
    //           p = all distinct
    //            i
    //           prime factors of n
    //
    // For example,
    //
    //   8                     5  2
    // 19 - 1 = 16983563040 = 2  3  5  17 181 3833
    //
    //        8           8
    // Phi( 19 - 1 ) = (19 - 1) (2-1) (3-1) (5-1) (17-1) (181-1) (3833-1) / (2 3 5 17 181 3833) = 4237885440
    //
    // You can check with Wolfram Alpha on the web,
    //
    //     http://www.wolframalpha.com/input/?i=eulerphi%28+19^8-1%29
    //                                                   8
    // then the number of primitive polynomials = Phi( 19 - 1) / 8 = 529735680

    num_prim_poly_ = p_to_n_minus_1_ ;
    vector<BigInt> distinct_factors_of_p_to_n_minus_1_ = factors_of_p_to_n_minus_1_.getDistinctPrimeFactors() ;
      
    for (auto & f : distinct_factors_of_p_to_n_minus_1_)
        num_prim_poly_ *= (f - static_cast<BigInt>( 1u )) ;

    for (auto & f : distinct_factors_of_p_to_n_minus_1_)
        num_prim_poly_ /=  f ;

    num_prim_poly_ /= static_cast<BigInt>( n_ ) ;
      
    return ;
}


/*=============================================================================
 |
 | NAME
 |
 |     orderM
 |
 | DESCRIPTION
 |                  m
 |     Check that x  (mod f(x), p) is not an integer for m = r / p  but skip
 |                                                                i
 |                                            n
 |                                           p  - 1           th
 |     this test if p  | (p-1).  Recall r = -------, and p = i   prime in
 |                   i                       p - 1        i
 |
 |     the factorization of r.
 |
 |
 | EXAMPLE
 |                                            2
 |      Let n = 4 and p = 5.  Then r = 156 = 2 * 3 * 13, and p = 2, p = 3,
 |                                                            1      2
 |
 |      and p = 13.  m = { 156/2, 156/3, 156/13 } = { 78, 52, 12 }.  We can
 |           3
 |
 |      skip the test for m = 78 because p = 2 divides p-1 = 4.  Exponentiation
 |                                        1
 |
 |             52       3   2                                    12
 |      gives x    = 2 x + x + 4 x, which is not an integer and x   =
 |
 |         3       2
 |      4 x  +  2 x  +  4 x  + 3 which is not an integer either, so we return
 |
 |      true.
 |
 | METHOD
 |
 |     Exponentiate x with x_to_power and test the result with is_integer.
 |     Return right away if the result is not an integer.
 |
 +============================================================================*/

bool PolyOrder::orderM()
{
    ppuint p{ f_.modulus() } ;

    for (int i = 0 ;  i < factors_of_R_.numDistinctFactors() ;  ++i)
    {
        // Can we skip this order m test?
        if (!factors_of_R_.skipTest( p, i ))
        {
            BigInt m = r_ / factors_of_R_.primeFactor( i ) ;

            Polynomial x1( "x" ) ;
            x1.setModulus( p ) ;
            PolyMod x( x1, f_ ) ;

            PolyMod x_to_m = power( x, m ) ;

            #ifdef DEBUG_PP_POLYNOMIAL
            cout << "Prime factor p[ " << i << " ] = " << factors_of_R_.primeFactor( i ) << endl ;
            cout << "m = " << m << endl ;
            cout << "x^m = " << x_to_m << endl ;
            #endif

            // Early out.
            if (x_to_m.isInteger())
                return( false ) ;
        }
    }

    return( true ) ;

}


/*=============================================================================
 |
 | NAME
 |
 |     orderR
 |
 | DESCRIPTION
 |                                               n
 |              r                               p - 1
 |     Compute x  (mod f(x), p) = a, where r = -------
 |                                              p - 1
 |
 |     If a is not an integer, return 0, else return a itself.
 |
 | EXAMPLE
 |              4    2
 |      f(x) = x  + x  + 2 x + 3, n = 4 and p = 5.  Then r = 156 and
 |
 |       r    156
 |      x  = x    = 3 (mod f(x), 5) = 3, so we return 3.
 |
 |                      4
 |      But for f(x) = x  + x + 3, n = 4, p = 5,
 |
 |       r    156      3
 |      x  = x    = 3 x + 2 x + 1 (mod f(x), 5) so we return 0.
 |
 | METHOD
 |                           r
 |     First compute g(x) = x (mod f(x), p).
 |     Then test if g(x) is a constant polynomial.
 |
 +============================================================================*/

ppuint PolyOrder::orderR()
{
    Polynomial x1( "x", p_ ) ;
    PolyMod x( x1, f_ ) ;

    PolyMod x_to_r = power( x, r_ ) ;

    #ifdef DEBUG_PP_POLYNOMIAL
    cout << "r = " << r_ << endl ;
    cout << "x^r = " << x_to_r << endl ;
    cout << "is integer = " << x_to_r.isInteger() << endl ;
    #endif

    if (x_to_r.isInteger())
        //  Return the value a = constant term of g(x).
        return x_to_r[ 0 ] ;
    else
        return 0 ;
}


/*=============================================================================
 |
 | NAME
 |
 |     max_order
 |
 | DESCRIPTION
 |               k                                  n
 |     Check if x  = 1 (mod f(x), p) only when k = p  - 1 and not for any smaller
 |     power of k, i.e. that f(x) is a primitive polynomial.
 |
 | INPUT
 |
 |      f (int *)                Monic polynomial f(x).
 |      n      (int, n >= 1)     Degree of f(x).
 |      p      (int)             Modulo p coefficient arithmetic.
 |
 | RETURNS
 |
 |      true    if f( x ) is primitive.
 |      false   if it isn't.
 |
 | EXAMPLE
 |
 |                4
 |      f( x ) = x  + x  +  1 is a primitive polynomial modulo p = 2,
 |                                          4
 |      because it generates the group GF( 2  ) with the exception of
 |                             2         15
 |      zero.  The powers {x, x , ... , x  } modulo f(x), mod 2 are
 |                                         16       4
 |      distinct and not equal to 1 until x   (mod x  + x + 1, 2) = 1.
 |
 | METHOD
 |
 |     Confirm f(x) is primitive using the definition of primitive
 |     polynomial as a generator of the Galois group
 |          n                   n
 |     GF( p ) by testing that p - 1 is the smallest power for which
 |      k
 |     x = 1 (mod f(x), p).
 |
 +============================================================================*/

bool PolyOrder::maximal_order()
{
    //  Highest possible order for x.
    BigInt maxOrder = power( f_.modulus(), f_.deg()) - static_cast<BigInt>( 1u ) ;

    BigInt k { 1u } ;
    Polynomial x1( "x", f_.modulus() ) ;
    PolyMod x( x1, f_ ) ;    // g(x) = x (mod f(x), p)

    while ( k <= maxOrder )
    {
        PolyMod x_to_power = power( x, k ) ; // x^k

        if (x_to_power.isInteger() &&
            x_to_power[0] == 1u &&
            k < maxOrder)
        {
            return false ;
        }

        ++k ;

    }

    return true ;
}


/*=============================================================================
 |
 | NAME
 |
 |     hasMultipleDistinctFactors
 |
 | DESCRIPTION
 |
 |    Returns true if the monic polynomial f( x ) has two or more distinct
 |    irreducible factors, false otherwise.
 |
 |    If earlyOut is false, compute the exact nullity in findNullity() instead
 |    of stopping when the nullity is >= 2.
 |
 | EXAMPLE
 |
 |    Let n = 4, p = 5
 |
 |              4    2
 |    f( x ) = x  + x  + 2 x + 3 is irreducible, so has one distinct factor.
 |
 |              4    3   2                  4
 |    f( x ) = x + 4x + x + 4x + 1 = (x + 1)  has one distinct factor.
 |
 |              3         2
 |    f( x ) = x  + 3 = (x + 3x + 4)(x + 2) has two distinct irreducible factors.
 |
 |              4    3    2                          2
 |    f( x ) = x + 3x + 3x + 3x + 2 = (x + 1) (x + 2) (x + 3) has 3 distinct
 |    irreducible factors.
 |
 |            4
 |    f(x) = x + 4 = (x+1)(x+2)(x+3)(x+4) has 4 distinct irreducible factors.
 |
 | METHOD
 |
 |       Berlekamp's method for factoring polynomials over GF( p ), modified to test
 |       for irreducibility only.
 |
 |       See my notes;  I skip the polynomial GCD step which ensures polynomials
 |       are square-free due to time constraints, but this requires a proof that
 |       the method is still valid.
 |
 +============================================================================*/

bool PolyOrder::hasMultipleDistinctFactors( bool earlyOut )

{
    // Generate the Q-I matrix.
    generateQMatrix() ;

    // Find nullity of Q-I
    findNullity( earlyOut ) ;

    // If nullity_ >= 2, f( x ) is a reducible polynomial modulo p since it has
    // two or more distinct irreducible factors.
    //                                     e
    // Nullity of 1 implies f( x ) = p( x )  for some power e >= 1 so we cannot
    // determine reducibility.
    if (nullity_ >= 2)
        return true ;

    return false ;

}


/*=============================================================================
 |
 | NAME
 |
 |     isPrimitive
 |
 | DESCRIPTION
 |
 |     Check if a given polynomial f(x) of degree n modulo p is primitive.
 |
 +============================================================================*/

bool PolyOrder::isPrimitive()
{
    bool isPrimitive = false ;
    ++statistics_.num_poly_tested ;

    try
    {
        BigInt max_num_possible_poly = power( p_, n_ ) ;
        ArithModP modp( p_ ) ;

        // Constant coefficient of f(x) * (-1)^n must be a primitive root of p.
        if (modp.constCoeffIsPrimitiveRoot( f_[0], f_.deg() ))
        {
            ++statistics_.num_where_const_coeff_is_primitive_root ;

            #ifdef DEBUG_PP_PRIMITIVITY
            cout << "    (-1)^n const coeff " << f_[ 0 ] << " is primitive root of " << p_ << endl ;
            #endif

            // f(x) can't have any linear factors.
            if (!f_.hasLinearFactor())
            {
                ++statistics_.num_free_of_linear_factors ;

                #ifdef DEBUG_PP_PRIMITIVITY
                cout << "    No linear factors" << endl ;
                #endif

                // Do more in-depth checking.

                // f(x) can't have two or more distinct irreducible factors.
                if (!hasMultipleDistinctFactors())
                {
                    ++statistics_.num_irreducible_to_power ;

                    #ifdef DEBUG_PP_PRIMITIVITY
                    cout << "    One distinct irreducible factor (possibly repeated)" << endl ;
                    #endif

                    //  r
                    // x  (mod f(x), p) = a_ must be an integer.
                    ppuint a{ orderR() } ;
                    if (a != 0)
                    {
                        ++statistics_.num_order_r ;

                        #ifdef DEBUG_PP_PRIMITIVITY
                        cout << "    x^r = a (integer)" << endl ;
                        #endif

                         //              n
                         // Check if (-1)  (constant coefficient of f(x)) = a_ (mod p)
                         //
                         if (modp.constCoeffTest( f_[ 0 ], a, f_.deg() ))
                         {
                             ++statistics_.num_passing_const_coeff_test ;

                             #ifdef DEBUG_PP_PRIMITIVITY
                             cout << "    a (integer) = (-1)^n f[0]" << endl ;
                             #endif

                             //  x^m != integer for all m = r / q, q a prime divisor of r.
                             if (orderM())
                             {
                                 ++statistics_.num_order_m ;

                                 #ifdef DEBUG_PP_PRIMITIVITY
                                 cout << "    x^m != integer for m = r / prime divisor of r" << endl ;
                                 #endif

                                 isPrimitive = true ;
                                 goto Exit ;

                             } // end order m
                         } // end const coeff test
                    } // end order r
                } // end can't determine if reducible
            } // end no linear factors
        } // end constant coefficient primitive.
    }
    catch( ArithModPError & e )
    {
        ostringstream os ;
        os << "PolyOrder::isPrimitive had a mod p arithmetic error [ " << e.what() << " ] " ;
        throw PolynomialRangeError( os.str(), __FILE__, __LINE__ ) ;
    }

    Exit:
        return isPrimitive ;
}


/*=============================================================================
 |
 | NAME
 |     generateQMatrix
 |
 | DESCRIPTION
 |
 |     Generate n x n matrix Q - I, where rows of Q are the powers,
 |
 |         p                2p                      (n-1) p
 |     1, x  (mod f(x),p), x  (mod f(x), p), ... , x       (mod f(x), p)
 |
 |     for monic polynomial f(x).
 |
 | EXAMPLE
 |
 |             4   2
 |     f(x) = x + x + 2 x + 3, n = 4, p = 5
 |
 |         (    1     )    (    1              )       ( 1    0    0    0 )
 |         (          )    (                   )       (                  )
 |         (     5    )    (         2       3 )       (                  )
 |     Q = (    x     )    (  2x + 3x   + 4 x  )       ( 0    2    3    4 )
 |         (          )    (                   )       (                  )
 |         (          )    (         2     3   )       (                  )
 |         (     10   ) =  (  3 + 4 x   + x    )   =   (                  )
 |         (    x     )    (                   )       ( 3    0    4    1 )
 |         (          )    (                   )       (                  )
 |         (     15   )    (          2    3   )       (                  )
 |         (    x     )    (   4x + 4x + 3x    )       ( 0    4    4    3 )
 |         (          )    (                   )       (                  )
 |
 |                                                                 2    3
 |                                                       1   x    x    x
 |              ( 0 0 0 0 )
 |              ( 0 1 3 4 )
 |     Q - I =  ( 3 0 3 1 )
 |              ( 0 4 4 2 )
 |
 |
 |     The left nullspace has dimension = 1 with basis { (1 0 0 0) }.
 |
 +============================================================================*/

void PolyOrder::generateQMatrix()
{
    // Check for invalid inputs.
    if (n_ < 2 || p_ < 2)
        throw PolynomialRangeError( "generateQMatrix has n < 2 or p < 2", __FILE__, __LINE__ ) ;

    // Row 0 of Q = (1 0 ... 0).
    Q_[ 0 ][ 0 ] = 1 ;
    for (int i = 1 ;  i < n_ ;  ++i)
        Q_[ 0 ][ i ] = 0 ;

    //             p
    // Let q(x) = x (mod f(x),p)
    // and Q[ 1 ] = coefficients of q(x).
    Polynomial x1( "x", p_ ) ;
    PolyMod x( x1, f_ ) ;
    PolyMod xp = power( x, static_cast< BigInt >( p_ ) ) ;

    #ifdef DEBUG_PP_POLYNOMIAL
        cout << "x ^ p (mod f(x),p) = " << xp << endl ;
    cout << "initial Q matrix " << printQMatrix() ;
    #endif

    PolyMod q = xp ;

    for (int i = 0 ;  i < n_ ;  ++i)
        Q_[ 1 ][ i ] = xp[ i ] ;

    //               pk
    // Row k of Q = x   (mod f(x), p) 2 <= k <= n-1, computed by
    //                                   p
    // multiplying each previous row by x (mod f(x),p).
    for (int k = 2 ;  k <= n_- 1 ;  ++k)
    {
        q *= xp ;

        #ifdef DEBUG_PP_POLYNOMIAL
        cout << "x ^ pk (mod f(x),p) = " << q << " for k = " << k << endl ;
        #endif

        for (int i = 0 ;  i < n_ ;  ++i)
             Q_[ k ][ i ] = q[ i ] ;
    }

    #ifdef DEBUG_PP_POLYNOMIAL
    cout << "computed Q matrix " << printQMatrix() ;
    #endif

    //  Subtract Q - I
    for (int row = 0 ;  row < n_ ;  ++row)
    {
        Q_[ row ][ row ] = mod( Q_[ row ][ row ] - 1 ) ;
    }

    #ifdef DEBUG_PP_POLYNOMIAL
    cout << "computed Q-I matrix " << printQMatrix() ;
    #endif

    return ;
}


/*=============================================================================
 |
 | NAME
 |
 |    printQMatrix
 |
 | DESCRIPTION
 |
 |    Print the matrix to the console.
 |
 +============================================================================*/
 
string PolyOrder::printQMatrix() const
{
    // Print the matrix as a string.
    ostringstream os ;

    os << endl ;
    for (int row = 0 ;  row < n_ ;  ++row)
    {
        os << "( " ;
        for (int col = 0 ;  col < n_ ;  ++col)
        {
            os << setw( 4 ) << setfill( ' ' ) << Q_[ row ][ col ] ;
        }
        os << " )" << endl ;
    }

    return os.str() ;
}


/*=============================================================================
 |
 | NAME
 |
 |    findNullity
 |
 | DESCRIPTION
 |
 |     Computes the nullity of Q.  
 |     If earlyOut is true, stop when the nullity is >= 2 and return 2.
 |
 | EXAMPLE
 |
 |     Let p = 5 and n = 3.  We use the facts that -1 = 4 (mod 5), 1/2 = 3, -1/2 = 2,
 |     1/3 = 2, -1/3 = 3, 1/4 = 4, -1/4 = 1.
 |
 |     Consider the matrix
 |
 |         ( 2 3 4 )
 |     Q = ( 0 2 1 )
 |         ( 3 3 3 )
 |
 |     Begin with row 1.  No pivotal columns have been selected yet.  Scan row 1 and
 |     pick column 1 as the pivotal column because it contains a nonzero element.
 |
 |     Normalizing column 1 by multiplying by -1/pivot = -1/2 = 2 gives
 |
 |         ( 4 3 4 )
 |         ( 0 2 1 )
 |         ( 1 3 3 )
 |
 |      Now perform column reduction on column 2 by multiplying the pivotal column 1
 |      by 3 (the column 2 element in the current row) and adding back to row 2.
 |
 |         ( 4 0 4 )
 |         ( 0 2 1 )
 |         ( 1 1 3 )
 |
 |      Column reduce column 3 by multiplying the pivotal column by 4 and adding back to row 3,
 |
 |         ( 4 0 0 )
 |         ( 0 2 1 )
 |         ( 1 1 2 )
 |
 |      For row 2, pick column 2 as the pivotal column, normalize it and reduce columns 1, then 3,
 |
 |         ( 4 0 0 )    ( 4 0 0 )    ( 4 0 0 )    ( 4 0 0 )
 |         ( 0 2 1 ) => ( 0 4 1 ) => ( 0 4 1 ) => ( 0 4 0 )
 |         ( 1 1 2 )    ( 1 2 2 )    ( 1 2 2 )    ( 1 2 4 )
 |                  norm.         c.r. 1      c.r. 3
 |
 |      For row 3, we must pick column 3 as pivotal column because we've used up columns 1 and 2,
 |
 |         ( 4 0 0 )    ( 4 0 0 )    ( 4 0 0 )    ( 4 0 0 )
 |         ( 0 4 0 ) => ( 0 4 0 ) => ( 0 4 0 ) => ( 0 4 0 )
 |         ( 1 2 4 )    ( 1 2 4 )    ( 1 2 4 )    ( 0 0 4 )
 |                  norm.        c.r. 1        c.r. 2
 |
 |      The nullity is zero, since we were always able to find a pivot in each row.
 |
 | METHOD
 |
 |       Modified from ART OF COMPUTER PROGRAMMING, Vol. 2, 2nd ed., Donald E. Knuth, Addison-Wesley.
 |
 |       We combine operations of normalization of columns,
 |
 |       (       c       )                         (        C       )
 |       (       c       )                         (        C       )
 |       (       .       )                         (        C       )
 |       ( . . . q . . . ) row  ================>  ( . . . -1 . . . ) row
 |       (       c       )                         (        C       )
 |       (       c       )      column times       (        C       )
 |       (       c       )      -1/a modulo p      (        C       )
 |            pivotCol                                   pivotCol
 |
 |       and column reduction,
 |
 |       (        C      b       )                         (       C        B       )
 |       (        C      b       )                         (       C        B       )
 |       (        C      b       )                         (       C        B       )
 |       ( . . . -1 . . .e . . . ) row  ================>  ( . . . -1 . . . 0 . . . )
 |       (        C      b       )                         (       C        B       )
 |       (        C      b       )    pivotCol times       (       C        B       )
 |       (        C      b       )    e added back to col  (       C        B       )
 |            pivotCol  col                                       col
 |
 |       to reduce the matrix to a form in which columns having pivots are zero until
 |       the pivotal row.
 |
 |       The column operations don't change the left nullspace of the
 |       matrix.
 |
 |       The matrix rank is the number of pivotal rows since they are linearly
 |       independent.  The nullity is then the number of non-pivotal rows.
 |
 +============================================================================*/

void PolyOrder::findNullity( bool earlyOut )
{
    try
    {
        InverseModP invmod( p_ ) ;

        #ifdef DEBUG_PP_POLYNOMIAL
        cout << "Q-I matrix " << printQMatrix() ;
        #endif

        vector<bool>pivotInCol( n_, false ) ; // Is false if the column has no pivotal element.

        nullity_ = 0 ;
        int pivotCol = -1 ; // No pivots yet.

        // Sweep through each row.
        for (int row = 0 ;  row < n_ ;  ++row)
        {
            // Search for a pivot in this row:  a non-zero element
            // in a column which had no previous pivot.
            bool found = false ;
            for (int col = 0 ;  col < n_ ;  ++col)
            {
                if (Q_[ row ][ col ] > 0 && !pivotInCol[ col ])
                {
                    found = true ;
                    pivotCol = col ;
                    break ;
                }
            }

            // No pivot;  increase nullity by 1.
            if (found == false)
            {
                ++nullity_ ;

                // Early out.
                if (earlyOut && nullity_ >= 2)
                    goto EarlyOut ;
            }
            // Found a pivot, q.
            else
            {
                ppsint q = Q_[ row ][ pivotCol ] ;

                // Compute -1/q (mod p)
                ppsint t = mod( -invmod( q )) ;

                // Normalize the pivotal column.
                for (int r = 0 ;  r < n_ ;  ++r)
                {
                    Q_[ r ][ pivotCol ] = mod( t * Q_[ r ][ pivotCol ]) ;
                }

                // Do column reduction:  Add C times the pivotal column to the other
                // columns where C = element in the other column at current row.
                for (int col = 0 ;  col < n_ ;  ++col)
                {
                    if (col != pivotCol)
                    {
                        q = Q_[ row ][ col ] ;

                        for (int r = 0 ;  r < n_ ;  ++r)
                        {
                            t = mod( q * Q_[ r ][ pivotCol ]) ;
                            Q_[ r ][ col ] = mod( t + Q_[ r ][ col ] ) ;
                        }
                    }
                }

                // Record the presence of a pivot in this column.
                pivotInCol[ pivotCol ] = true ;

                #ifdef DEBUG_PP_POLYNOMIAL
                cout << "row = " << row << " pivot = " << q << " (-1/q) = " << t << " nullity = " << nullity_
                     << " Row reduced Q-I matrix " << printQMatrix() ;
                #endif

            } // found a pivot

        } // end for row

        EarlyOut: ;
            #ifdef DEBUG_PP_POLYNOMIAL
            cout << "Row reduced Q-I matrix " << printQMatrix() ;
            #endif
    }
    catch( ArithModPError & e )
    {
        ostringstream os ;
        os << "PolyOrder::findNullity failed in matrix row reduction [ " << e.what() << " ] " ;
        throw PrimpolyError( os.str(), __FILE__, __LINE__ ) ;
    }

    // Automagically free pivotInCol and mod objects.

} // ===================== end of function findNullity =====================
