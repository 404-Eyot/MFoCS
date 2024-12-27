#include <cstdlib>      // abort()
#include <iostream>     // Basic stream I/O.
#include <new>          // set_new_handler()
#include <limits>       // Numeric limits.
#include <cmath>        // Basic math functions e.g. sqrt()
#include <complex>      // Complex data type and operations.
#include <fstream>      // File stream I/O.
#include <sstream>      // String stream I/O.
#include <vector>       // STL vector class.
#include <string>       // STL string class.
#include <algorithm>    // Iterators.
#include <stdexcept>    // Exceptions.
#include <cassert>      // assert()

using namespace std ;   // So we don't need to say std::vector everywhere.



/*------------------------------------------------------------------------------
|                                PP Include Files                              |
------------------------------------------------------------------------------*/

#include "Primpoly.hpp"          // Global functions.
#include "ppArith.hpp"           // Basic arithmetic functions.
#include "ppBigInt.hpp"          // Arbitrary precision integer arithmetic.
#include "ppOperationCount.hpp"  // OperationCount collection for factoring and poly finding.
#include "ppFactor.hpp"          // Prime factorization and Euler Phi.
#include "ppPolynomial.hpp"      // Polynomial operations and mod polynomial operations.
#include "ppParser.hpp"          // Parsing of polynomials and I/O services.
#include "ppUnitTest.hpp"        // Complete unit test.



/*==============================================================================
|                     Static Class Variables Initialization                    |
==============================================================================*/

// Static class variables must be initialized outside the class.

// Pointer to number system base.  Used for unit testing only.
ppuint * BigInt::pbase = 0 ;



/*=============================================================================
|
| NAME
|
|     BigInt::BigInt()
|
| DESCRIPTION
|
|    Default construct a multiple precision integer u with no digits.
|
| EXAMPLE
|
|    Called during default construction e.g.
|        BigInt n ;
|        foo( BigInt x )
|
+============================================================================*/

BigInt::BigInt()
       : digit_( 0 )                // Construct a vector with zero length.
{
    // Make certain we have zero digits.
    digit_.clear() ;
}




/*=============================================================================
|
| NAME
|
|     ~BigInt
|
| DESCRIPTION
|
|    Default destructor for a BigInt type.
|
| EXAMPLE
|
|    Called upon leaving scope:
|    {
|         BigInt n ;
|    } <--- called here.
|
+============================================================================*/

BigInt::~BigInt()
{
    // Digits are in a vector which frees itself when it goes out of scope.
    // Other class variables are primitive types.
    // We obviously don't throw any exceptions from this destructor so
    // terminate() won't ever be called.
} 



/*=============================================================================
|
| NAME
|
|     BigInt::BigInt( ppuint d )
|
| DESCRIPTION
|
|     Construct a BigInt from an unsigned 64-bit integer.
|
| EXAMPLE
|
|    try
|    {
|        ppuint d{ 123 } ;
|        BigInt u( d ) ;
|    }
|    catch( BigIntMathError & e )
|    {
|        cerr << e.what() << endl ;
|    }
|
| METHOD
|
|    The unsigned integer d written in BigInt format using
|    base b is
|                                         n-1
|       d  =  (u     . . . u )   = u    b    + ... + u  b +  u
|               n-1         0  b    n-1               1       0
|   
|       The first digit of u is
|   
|       d mod b = u
|                  0
|
|       after which the remainder is
|
|                                n-2
|           | d / b |  =  u    b    + ... + u
|           --     --      n-1               1
|   
|       then we set d <-- | d / b |  and repeat to get u , etc.
|                         --     --                     1
|   
|       So the algorithm for extracting the BigInt digits is
|   
|       do until d == 0:
|   
|           U = d mod b
|   
|           u = |  d / b |
|               --      --
|
+============================================================================*/

BigInt::BigInt( const ppuint d )
       : digit_( 0 )                // Construct a vector with zero length.
{
    ppuint b{ base_() } ;

    try
    {
        // Early return if d = 0;  d will never be negative.
        if (ppuint d2{ d } ; d2 == 0)
        {
            digit_.push_back( 0 ) ;
        }
        else
            while (d2 != 0)
            {
                ppuint digit{ d2 % b } ;

                // Can throw from argument operator=().
                digit_.push_back( digit ) ;

                d2 = d2 / b ;
            }
    }
    catch( bad_alloc & e )
    {
        ostringstream os ;
        os << "BigInt::BigInt( const ppuint d ) constructor had a bad_alloc memory allocation exception"
           << "while adding digits to BigInt" ;
        throw BigIntOverflow( os.str(), __FILE__, __LINE__ ) ;
    }
} 


/*=============================================================================
 |
 | NAME
 |
 |     BigInt::BigInt( ppuint32 d )
 |
 | DESCRIPTION
 |
 |     Construct a BigInt from an unsigned 32-bit integer.
 |
 | EXAMPLE
 |
 |    try
 |    {
 |        ppuint32 d{ 123 } ;
 |        BigInt u( d ) ;
 |    }
 |    catch( BigIntMathError & e )
 |    {
 |        cerr << e.what() << endl ;
 |    }
 |
 +============================================================================*/

BigInt::BigInt( const ppuint32 d )
: digit_( 0 )                // Construct a vector with zero length.
{
    // Cast the 32-bit number into a 64-bit number without any loss of precision,
    // and then into a BigInt.
    BigInt w { static_cast<ppuint>( d ) } ;
    
    // Swap the digits of w into this number.
    swap( w.digit_, digit_ ) ;
}


/*=============================================================================
|
| NAME
|
|     BigInt::BigInt( string )
|
| DESCRIPTION
|
|  Construct a BigInt
|
|   (u    ... u  )
|     n-1      0  b
|
|   from a decimal string
|
|   (U    ... U  )
|     m-1      0  10
|
|
|  EXAMPLE
|
|    BigInt v( "1234567890" ) ;
|
+============================================================================*/

BigInt::BigInt( const string & s )
       : digit_( 0 )                  // Construct a vector with zero length.
{
    // Construct temporary big integer w = 0 or else throw exception upwards.
    // If this fails, memory for w is automatically released during stack unwind,
    // this BigInt object isn't constructed.
    BigInt w( 0u ) ;

    #ifdef DEBUG_PP_BIGINT
    cout << "BigInt( string ) digits " ;
    #endif

    //  Use Horner's rule on base 10 numerals to convert decimal string
    //  of digits to base b.  e.g. 123 = 10 * (10 * ((10 * 0) + 1) + 2) + 3
    for (auto & c : s)
    {
        w *= static_cast<ppuint>( 10u ) ;

        // This only works for ASCII characters.
        int digit = 0 ;
        if (isdigit( c ))
        {
            char asciiDigit[ 2 ] ;
            asciiDigit[ 0 ] = c ;
            asciiDigit[ 1 ] = '\0' ;
            digit = atoi( asciiDigit ) ;
        }
        else
        {
            ostringstream os ;
            os << "BigInt::BigInt( string ):  range error from character = " << c ;
            throw BigIntRangeError( os.str(), __FILE__, __LINE__ ) ;
        }

        #ifdef DEBUG_PP_BIGINT
        cout << digit << " " ;
        #endif

        w += static_cast<ppuint>( digit ) ;
    }
    
    #ifdef DEBUG_PP_BIGINT
    cout << endl ;
    #endif

    // Swap the digits of w into this number.
    swap( w.digit_, digit_ ) ;

    // As we go out of scope, w's destructor will be called.
}



/*=============================================================================
|
| NAME
|
|     BigInt::BigInt
|
| DESCRIPTION
|
|     Copy constructor.
|
| EXAMPLE
|
|        BigInt u, v ;
|        BigInt u( v ) ;
|
+============================================================================*/

BigInt::BigInt( const BigInt & u )
       : digit_( u.digit_ )
{

} 



/*=============================================================================
|
| NAME
|
|     BigInt::operator=
|
| DESCRIPTION
|
|    Assignment operator.
|
| EXAMPLE
|
|    BigInt m = n ;
|
+============================================================================*/

BigInt & BigInt::operator=( const BigInt & n )
{
    // Check for assigning to oneself:  just pass back a reference to the unchanged object.
    if (this == &n)
        return *this ;

    try
    {
        vector<ppuint> tempDigits( n.digit_ ) ;
    
        // Move the old values into the temporary, and the new values into the object.
        // The temporary containing the old values will be destroyed when we leave scope.
        swap( digit_, tempDigits ) ;
    }
    catch( bad_alloc & e )
    {
        ostringstream os ;
        os << "BigInt::operator=( const BigInt & n ) had a bad_alloc memory allocation exception" ;
        throw BigIntOverflow( os.str(), __FILE__, __LINE__ ) ;
    }

    // Return a reference to assigned object to make chaining possible.
    return *this ;
}



/*=============================================================================
|
| NAME
|
|     operator ppuint
|
| DESCRIPTION
|
|    Convert an n-place number
|
|    (u    ... u  )
|      n-1      0  b
|
|    as an unsigned integer.  If the number is too
|    large, throw a BigIntOverflow exception.
|
+============================================================================*/

BigInt::operator ppuint() const
{
    ppuint result{ 0 } ;
    ppuint b{ base_() } ;

    for (int i = static_cast<unsigned int>( digit_.size()) - 1 ;  i >= 0 ;  --i)
    {
        // Check for overflow before it happens:  Will result * base + digit > (maximum unsigned integer)?
        if (result > (numeric_limits<ppuint>::max() - digit_[ i ]) / b )
        {
            ostringstream os ;
            os << "BigInt::operator ppuint() is about to overflow "
               << " partial result [" << result << "] * base [" << b << "] + digit [" << digit_[ i ]
               << "] > numeric limit for ppuint [" << numeric_limits<ppuint>::max() << "]" ;
            throw BigIntOverflow( os.str(), __FILE__, __LINE__ ) ;
        }
        else
            result = result * b + digit_[ i ] ;
    }

    return result ;
}



/*=============================================================================
|
| NAME
|
|     Bigint::toString
|
| DESCRIPTION
|
|    Print n-place number
|
|    (u    ... u  )
|      n-1      0  b
|
|    as a decimal string,
|
|    (U    ... U  )
|      m-1      0  10
|
| EXAMPLE
|
|     // Set up multiprecision calculations up to the size p^n.
|     Bigint n = "31415926535897932" ;
|     string s = n.toString() ;
|
+============================================================================*/

string BigInt::toString() const
{
    const ppuint decimal_base{ static_cast<ppuint>( 10u ) } ;

    // Output stream to hold the digits.
    ostringstream os ;
    string s = "" ;

    // Pull out the decimal digits in reverse:
    //
    //    do until u == 0:
    //        U = u mod 10
    //        u = |  u / 10 |
    //            --       --
    BigInt u( *this ) ;

    // Special cases.
    if (u == BigInt( static_cast<ppuint>( 0u ) ))
        return "0" ;
    else if (u == BigInt( static_cast<ppuint>( 1u ) ))
        return "1" ;

    #ifdef DEBUG_PP_BIGINT
    cout << "toString digits = "  ;
    #endif

    while (u != BigInt( static_cast<ppuint>( 0u ) ))
    {
        ppuint digit{ u % decimal_base } ;

        #ifdef DEBUG_PP_BIGINT
        // This line was recursing in and out of BigInt::toString and BigInt::printNumber, ending in a memory violation.
        // cout << "toString:  number to convert u = " ; printNumber( u, cout ) ;
        cout << digit << " "  ;
        #endif

        if (!(os << digit))
        {
            ostringstream os ;
            os << "BigInt::toString can't convert digit = " << digit ;
            throw BigIntRangeError( os.str(), __FILE__, __LINE__ ) ;
        }

        u /= decimal_base ;
    }
    
    #ifdef DEBUG_PP_BIGINT
    cout << endl ;
    #endif

    s = os.str() ;
    reverse( s.begin(), s.end() ) ;

    return s ;
}



/*=============================================================================
|
| NAME
|
|     Operator << for BigInt
|
| DESCRIPTION
|
|     Print a BigInt to the output stream.
|
| EXAMPLE
|
|     try
|     {
|         BigInt x( "123" ) ;
|         cout << x << endl ;
|     }
|     catch( BigIntRangeError & e )
|     {
|         cout << "Error in outputting BigInt x" << endl ;
|     }
|
+============================================================================*/

ostream & operator<<( ostream & out, const BigInt & u )
{
    // Convert to a string first, then output as a string.
    // Can throw an exception, which we'll catch at higher level.
    out << u.toString() ;

    return out ;
}



/*=============================================================================
|
| NAME
|
|     Operator >> for BigInt
|
| DESCRIPTION
|
|     BigInt stream input.
|
| EXAMPLE
|
|     try
|     {
|         BigInt x ;  // Declare a BigInt object.
|         cin >> x ;  // Read a long decimal string from the standard input.
|                     // e.g. type in 31415926535897932
|     }
|     catch( BigIntRangeError & e )
|     {
|         cout << "Error in inputting BigInt x = 31415926535897932" << endl ;
|     }
|
+============================================================================*/

istream & operator>>( istream & in, BigInt & u )
{
    // Input the number as a string first.
    string s ;
    in >> s ;

    // Convert to BigInt and copy into argument.
    // We may throw a BigIntRangeError at this point if s is corrupted.
    u = BigInt( s ) ;

    return in ;
}



/*=============================================================================
|
| NAME
|
|    +
|
| DESCRIPTION
|
|    Add n-place numbers,
|
|    (u    ... u  )  +
|      n-1      0  b
|
|    (v    ... v  )  =
|      n-1      0  b
|
|    (w    ... w  )  +
|      n-1      0  b
|
|
| EXAMPLE
|
|    try
|    {
|        BigInt u, v, w ;
|        u = v + w ;
|    }
|    catch( BigIntMathError & e )
|    {
|        cerr << e.what() << endl ;
|    }
|
+============================================================================*/

const BigInt operator+( const BigInt & u, const BigInt & v )
{
    // Do + in terms of += to maintain consistency.
    // Copy construct temporary copy and add to it.
    // Return value optimization compiles away the copy constructor.
    // const on return type disallows doing (u+v) = w ;
    return BigInt( u ) += v ;
}



/*=============================================================================
|
| NAME
|
|     +
|
| DESCRIPTION
|
|    Add n-place number and a digit,
|
|    (u    ... u  )  + d
|      n-1      0  b
|
| EXAMPLE
|
|    try
|    {
|        BigInt u, w ;
|        ppuint d ;
|        u = v + d ;
|    }
|    catch( BigIntMathError & e )
|    {
|        cerr << e.what() << endl ;
|    }
|
+============================================================================*/

const BigInt operator+( const BigInt & u, ppuint d )
{
    // Do + in terms of += to maintain consistency.
    // Return value optimization compiles away the copy constructor.
    // const on return type disallows doing (u+v) = w ;
    return BigInt( u ) += d ;
}



/*=============================================================================
|
| NAME
|
|     +=
|
| DESCRIPTION
|
|    Add n-place number and an m-place number,
|
|    (u    ... u  )  +=
|      n-1      0  b
|
|    (0 ... v    ... v  ) 
|            m-1      0  b
|
| EXAMPLE
|
|    try
|    {
|        BigInt u, v ;
|        u += v ;
|    }
|    catch( BigIntMathError & e )
|    {
|        cerr << e.what() << endl ;
|    }
|
+============================================================================*/

BigInt & BigInt::operator+=( const BigInt & v )
{
    // Pull out the number of digits and base.
    int  n{ static_cast<int>( digit_.size() ) } ;
    int  m{ static_cast<int>( v.digit_.size() ) } ;
    ppuint b{ base_() } ;

#ifdef DEBUG_PP_BIGINT
    printNumber( *this, cout ) ;
    printNumber( v,  cout ) ;
#endif

    // Allocate temporary space for the sum.
    BigInt w ;

    ppuint carry{ 0 } ;  // Always < 2b
    ppuint sum{ 0 };  // Always in [0, 2b)

    //  Add and carry, starting with the least significant
    //  digits in each number.
    for (int i = 0 ;  i < (m < n ? m : n) ;  ++i)
    {
        sum         = digit_[ i ] + v.digit_[ i ] + carry ;
        // Can throw from argument operator=().
        w.digit_.push_back( sum % b ) ;
        carry       = sum / b ;
    }

    if (n < m)
        for (int i = n ;  i < m ;  ++i)
        {
            sum         = 0 + v.digit_[ i ] + carry ;
            // Can throw from argument operator=().
            w.digit_.push_back( sum % b ) ;
            carry       = sum / b ;
        }
    else if (m < n)
        for (int i = m ;  i < n ;  ++i)
        {
            sum         = digit_[ i ] + 0 + carry ;
            // Can throw from argument operator=().
            w.digit_.push_back( sum % b ) ;
            carry       = sum / b ;
        }


    // Add the carry digit as the most significant digit.
    if (carry != 0)
        // Can throw from argument operator=().
        w.digit_.push_back( carry ) ;

    // Swap the digits of w into this number.
    swap( w.digit_, digit_ ) ;

    // Return current object now containing the sum.
    return *this ;

    // As we go out of scope, w's destructor will be called.
}



/*=============================================================================
|
| NAME
|
|     +=
|
| DESCRIPTION
|
|    Add an unsigned integer d to an n-place number u
|
|    (u    ... u  )   + d
|      n-1      0  b
|
| EXAMPLE
|
|    try
|    {
|        BigInt u ;
|        ppuint d ;
|        u += d ;
|    }
|    catch( BigIntMathError & e )
|    {
|        cerr << e.what() << endl ;
|    }
|
+============================================================================*/

BigInt & BigInt::operator+=( const ppuint d )
{
    // Pull out the number of digits and base.
    int  n = static_cast<int>( digit_.size() ) ;
    ppuint b{ base_() } ;

    if (d >= b)
    {
        // TODO:   We should do BigInt + here instead of aborting.
        ostringstream os ;
        os << "BigInt::operator+=  Adding BigInt = " << *this << " + d = " << d << " but d >= base = " << b
           << " which is bigger than the largest value a digit of BigInt can have.  Please email me to request a feature enhancement" ;
        throw BigIntOverflow( os.str(), __FILE__, __LINE__ ) ;
    }

    // Allocate temporary space for the sum.
    BigInt w ;

    // Add the first digit.
    ppuint sum{ digit_[ 0 ] + d } ; // Always in [0, 2b)
    w.digit_.push_back( sum % b ) ;

    ppuint carry{ sum / b } ; // Always < 2b

    //  Add and carry in the other digits if needed.
    for (int i = 1 ;  i < n ;  ++i)
    {
        sum         = digit_[ i ] + carry ;
        // Can throw from argument operator=().
        w.digit_.push_back( sum % b ) ;
        carry       = sum / b ;
    }

    // Add the carry digit as the most significant digit.
    if (carry != 0)
        // Can throw from argument operator=().
        w.digit_.push_back( carry ) ;

    // Swap the digits of w into this number.
    swap( w.digit_, digit_ ) ;

    // Return current object now containing the sum.
    return *this ;

    // As we go out of scope, w's destructor will be called.
}



/*=============================================================================
|
| NAME
|
|     Prefix ++
|
| DESCRIPTION
|
|     Prefix increment operator.
|
| EXAMPLE
|
|     BigInt i ;
|     ++i ;
|     i.operator++() ; // Equivalent
|
+============================================================================*/

BigInt & BigInt::operator++()
{
    // Add 1.
    (*this) += 1 ;

    // Return reference to incremented BigInt object so we
    // can modify the object, e.g.  ++i++ ;
    return *this ;
}



/*=============================================================================
|
| NAME
|
|     Postfix ++
|
| DESCRIPTION
|
|     Postfix
|
| EXAMPLE
|
|     BigInt i ;
|     ++i ;
|     i.operator++(0) ; // Equivalent
|
+============================================================================*/

const BigInt BigInt::operator++( int ) // Dummy argument to distinguish postfix form.
{
    BigInt save = *this ;

    // Add 1 using prefix operator.
    ++(*this) ;

    // Return const copy of original
    // to prevent doing i++++ which is the same as i.operator++(0).operator++(0)
    // which does i += 1, returns i, then does i += 1 which would increment
    // by 1;  not what we wanted.
    return save ;
}



/*=============================================================================
|
| NAME
|
|     -
|
| DESCRIPTION
|
|    BigInt - BigInt
|
| EXAMPLE
|
|     BigInt u = 3 ;
|     BigInt v = 4 ;
|     BigInt w = u - v ;
|
+============================================================================*/

const BigInt operator-( const BigInt & u, const BigInt & v )
{
    // Do - in terms of -= to maintain consistency.
    // Return value optimization compiles away the copy constructor.
    // const on return type disallows doing (u-v) = w ;
    return BigInt( u ) -= v ;
}



/*=============================================================================
|
| NAME
|
|     -
|
| DESCRIPTION
|
|     BigInt - integer
|
| EXAMPLE
|
|     BigInt u{ 3 } ;
|     ppuint v{ 4 } ;
|     BigInt w = u - v ;
|
+============================================================================*/

const BigInt operator-( const BigInt & u, const ppuint d )
{
    // Do - in terms of -= to maintain consistency.
    // Return value optimization compiles away the copy constructor.
    // const on return type disallows doing (u-v) = w ;
    return BigInt( u ) -= d ;
}



/*=============================================================================
|
| NAME
|
|     -=
|
| DESCRIPTION
|
|     u -= v
|
|    Subtract u - v = (u1 ... un) - (v1 ... vm) = (w1 ... wn) = w.
|    Assume u >= v.  If u < v, the carry is negative, and we abort.
|
+============================================================================*/

BigInt & BigInt::operator-=( const BigInt & v )
{
    // Pull out the number of digits and the base.
    int  n = static_cast<int>( digit_.size()) ;
    int  m = static_cast<int>( v.digit_.size()) ;
    ppuint b{ base_() } ;

    if (m > n)
    {
        ostringstream os ;
        os << "BigInt::operator-= " << " negative result for u - v = " << *this << " - " << v ;
        throw BigIntUnderflow( os.str(), __FILE__, __LINE__ ) ;
    }

    // Allocate temporary space for the difference.
    BigInt w ;

    // Subtract u - v starting with nth digits assuming n >= m.
    ppuint borrow{ 0 } ;
    for (int i = 0 ;  i < n ;  ++i)
    {
        // t in [-b, b)
        ppsint t = digit_[ i ] - (i >= m ? 0 : v.digit_[ i ]) + borrow ;

        // Subtract, allowing for where u < v and we must borrow.
        // Can throw from argument operator=().
        w.digit_.push_back( (t > 0) ? t % b : (t + b) % b ) ;

        borrow = (t < 0) ? -1 : 0 ; // -1 if u - v + borrow < 0, else 0
    }

    if (borrow == -1)
    {
        ostringstream os ;
        os << "BigInt::operator-= " << " negative result for u - v = " << *this << " - " << v ;
        throw BigIntUnderflow( os.str(), __FILE__, __LINE__ ) ;
    }

    // Trim leading zeros, if any, but stop if q = 0.
    while (w.digit_.size() > 1 && w.digit_.back() == 0)
        w.digit_.pop_back() ;

    // Swap the digits of w into this number.
    swap( w.digit_, digit_ ) ;

    // Return (reference to) the sum.
    return *this ;

    // As we go out of scope, w's destructor will be called.
}



/*=============================================================================
|
| NAME
|
|     -=
|
| DESCRIPTION
|
+============================================================================*/

BigInt & BigInt::operator-=( const ppuint u )
{
    // Get the base and number of digits.
    ppuint b{ base_() } ;
    int  n = static_cast<int>( digit_.size()) ;

    if (u >= b)
    {
        // TODO:   We should do BigInt - here instead of aborting.
        ostringstream os ;
        os << "BigInt::operator-=( u ) BigInt = " << *this << " - u = " << u << " >= base = " << b
           << " please email me to request a feature enhancement" ;
        throw BigIntUnderflow( os.str(), __FILE__, __LINE__ ) ;
    }

    // Subtract 1 from the least significant digit.
    ppsint t = digit_[ 0 ] - u ;

    // Subtract and allow for borrow.
    digit_[ 0 ] = (t > 0) ? t % b : (t + b) % b ;
    ppsint borrow  = (t < 0) ? -1 : 0 ; // -1 if u - 1 + borrow < 0, else 0

    // Propagate the subtraction up to the (n-1)st digit.
    for (int i = 1 ;  i < n ;  ++i)
    {
        t = digit_[ i ] + borrow ;

        // Subtract and allow for borrow.
        digit_[ i ] = (t > 0) ? t % b : (t + b) % b ;
        borrow      = (t < 0) ? -1 : 0 ; // -1 if u - 1 + borrow < 0, else 0
    }

    if (borrow == -1)
    {
        ostringstream os ;
        os << "BigInt::operator-= " << " underflow, borrow = -1" ;
        throw BigIntUnderflow( os.str(), __FILE__, __LINE__ ) ;
    }

    // Trim leading zeros, if any, but stop if n = 0.
    while (digit_.size() > 1 && digit_.back() == 0)
        digit_.pop_back() ;
    
    // Return (reference to) the sum.
    return *this ;
}



/*=============================================================================
|
| NAME
|
|     --
|
| DESCRIPTION
|
|     Subtract u - 1.
|
|     ( u  u     . . . u )
|        n  n-1         0
|    -                1
|    --------------------
|     ( w  w     . . . w )
|        n  n-1         0
|
|    Subtract u - 1 = (u1 ... un) - (0 ... 1) = (w1 ... wn) = w.
|    Assume u >= 1.  If u < 0, the carry is negative, and we abort.
|
+============================================================================*/

BigInt & BigInt::operator--()
{
    ppuint b{ base_() } ;

    // Subtract 1 from the least significant digit.
    ppsint t = digit_[ 0 ] - 1 ;

    // Subtract and allow for borrow.
    digit_[ 0 ] = (t > 0) ? t % b : (t + b) % b ;
    ppsint borrow        = (t < 0) ? -1 : 0 ; // -1 if u - 1 + borrow < 0, else 0

    // Subtract u - 1 starting with nth digits.
    for (unsigned int i = 1 ;  i < digit_.size() ;  ++i)
    {
        t = digit_[ i ] + borrow ;

        // Subtract and allow for borrow.
        digit_[ i ] = (t > 0) ? t % b : (t + b) % b ;
        borrow        = (t < 0) ? -1 : 0 ; // -1 if u - 1 + borrow < 0, else 0
    }

    if (borrow == -1)
    {
        ostringstream os ;
        os << "BigInt::operator-- " << " underflow, borrow = -1" ;
        throw BigIntUnderflow( os.str(), __FILE__, __LINE__ ) ;
    }

    return *this ;
}



/*=============================================================================
|
| NAME
|
|     --
|
| DESCRIPTION
|
+============================================================================*/

BigInt BigInt::operator--( int )
{
    BigInt save = *this ;

    // Subtract 1.
    --(*this) ;

    return save ;
}



/*=============================================================================
|
| NAME
|
|     *
|
| DESCRIPTION
|
+============================================================================*/

//
//  Multiply (u1 ... un) * digit = (w1 ... wn).
//  Numbers are right justified, so that un and wn are in array
//  location n.
const BigInt operator*( const BigInt & u, const BigInt & v )
{
    // Do * in terms of *= to maintain consistency.
    // Return value optimization compiles away the copy constructor.
    // const on return type disallows doing (u*v) = w ;
    return BigInt( u ) *= v ;
}



/*=============================================================================
|
| NAME
|
|     *
|
| DESCRIPTION
|
+============================================================================*/

//
//  Multiply (u1 ... un) * digit = (w1 ... wn).
//  Numbers are right justified, so that un and wn are in array
//  location n.
const BigInt operator*( const BigInt & u, const ppuint digit )
{
    // Do * in terms of *= to maintain consistency.
    // Return value optimization compiles away the copy constructor.
    // const on return type disallows doing (u*v) = w ;
    return BigInt( u ) *= digit ;
}



/*=============================================================================
|
| NAME
|
|     *=
|
| DESCRIPTION
|
|    Multiply
|
|    (u    ...  u  )
|      m-1       0  b
|
|      x
|
|    (v    ...  v  )
|      n-1       0  b
|
|     =
|
|    (u    ...  u  )
|      m+n-1     0  b
|
+============================================================================*/

BigInt & BigInt::operator*=( const BigInt & v )
{
    // Pull out the number of digits and base.
    int m = static_cast<int>( digit_.size() ) ;
    int n = static_cast<int>( v.digit_.size() ) ;

    ppuint b{ base_() } ;

    if (m  == 0 || n == 0)
    {
        ostringstream os ;
        os << "BigInt::operator*= Internal error.  Num digits in u = " << m << " and v = " << n ;
        throw BigIntMathError( os.str(), __FILE__, __LINE__ ) ;
    }

    // Allocate temporary space for the product.
    BigInt w ;
    try
    {
        w.digit_.resize( m + n ) ;
    }
    catch( length_error & e )
    {
        ostringstream os ;
        os << "BigInt::operator*=() had a length_error exception during w.digit_resize(m+n = " << m+n << ")" ;
        throw BigIntOverflow( os.str(), __FILE__, __LINE__ ) ;
    }

    ppuint carry{ 0 } ;

    // Zero out leading digits of product.
    for (int j = m - 1 ;  j >= 0 ;  --j)
        w.digit_[ j ] = 0 ;

    // Multiply u by each digit of v from right to left.
    for (int j = 0 ;  j < n ;  ++j)
    {
        // Skip if digit of v is zero.
        if (v.digit_[ j ] == 0)
        {
            w.digit_[ j + m ] = 0 ;
            continue ;
        }

        // Multiply u by the jth digit of v.
        carry = 0 ;
        for (int i = 0 ;  i < m ;  ++i)
        {
            ppuint t{ digit_[ i ] * v.digit_[ j ] + w.digit_[ i + j ] + carry } ;
            w.digit_[ i + j ] = t % b ;
            carry             = t / b ;
        }

        w.digit_[ j + m ] = carry ;
    }

    // Trim a leading zero digit.
    if (w.digit_[ m + n - 1 ] == 0)
    {
        w.digit_.pop_back() ;
    }

    // Swap the digits of w into this number.
    swap( w.digit_, digit_ ) ;

    // Trim off leading zero digits.

    // Return (reference to) the sum.
    return *this ;

    // As we go out of scope, w's destructor will be called.
}


/*=============================================================================
|
| NAME
|
|     *=
|
| DESCRIPTION
|
|
|  Multiply u * d =
|
|    (u    ...  u  )
|      m-1       0  b
|
|      x
|
|      d
|
+============================================================================*/

BigInt & BigInt::operator*=( ppuint d )
{
    // Pull out the number of digits.
    int  n{ static_cast<int>( digit_.size() ) } ;
    ppuint b{ base_() } ;

    // Allocate temporary space for the product.
    BigInt w ;

    if (d > b)
    {
        // TODO:   We should do BigInt * here instead of aborting.
        //      w *= static_cast<BigInt>( d ) ;
        ostringstream os ;
        os << "BigInt::operator*=   digit d = " << d << " > base b = " << b
           << " please email me to request a feature enhancement" ;
        throw BigIntOverflow( os.str(), __FILE__, __LINE__ ) ;
    }
    // In this special case, we just shift digits left and zero fill.
    // But do nothing if the number is zero.
    else if (d == b && *this != static_cast<BigInt>( static_cast<ppuint>( 0u )))
    {
        w.digit_.push_back( 0 ) ;

        for (int i = 0 ;  i < n ;  ++i)
            w.digit_.push_back( digit_[ i ] ) ;
    }
    else
    {
        ppuint carry = 0 ;

        // Multiply digits and carry.
        for (int i = 0 ;  i < n ;  ++i)
        {
            ppuint t = digit_[ i ] * d + carry ;
            ppuint r = t % b ;

            w.digit_.push_back( r ) ;
            carry = t / b ;

            #ifdef DEBUG_PP_BIGINT
            cout << "BigInt::operator*=(ppuint)" << endl ;
            cout << "d        = " << d << endl ;
            cout << "digit[i] = " << digit_[ i ] << " i = " << i << endl ;
            cout << "t        = " << t << endl ;
            cout << "r        = " << r << endl ;
            cout << "carry    = " << carry << endl ;
            #endif
        }

        // Additional carry beyond the nth digit.
        if (carry)
            w.digit_.push_back( carry ) ;
    }

    // Swap the digits of w into this number.
    swap( w.digit_, digit_ ) ;

    // Return (reference to) the product.
    return *this ;

    // As we go out of scope, w's destructor will be called.
}



/*=============================================================================
|
| NAME
|
|     /
|
| DESCRIPTION
|
|    Integer division
|
| EXAMPLE
|
|  u / v =
|
+============================================================================*/

const BigInt operator/( const BigInt & u, const BigInt & v )
{
    // Do / in terms of /= to maintain consistency.
    // Return value optimization compiles away the copy constructor.
    // const on return type disallows doing (u/v) = w ;
    return BigInt( u ) /= v ;
}



BigInt & BigInt::operator/=( const BigInt & v )
{
    BigInt r ; // Thrown away.
    BigInt q ;

    divMod( *this, v, q, r ) ;

    // Swap the digits of q and current object.
    swap( q.digit_, digit_ ) ;

    return *this ;
}



/*=============================================================================
|
| NAME
|
|     /
|
| DESCRIPTION
|
|     Divide by a digit.
|
+============================================================================*/

const BigInt operator/( const BigInt & u, ppuint d )
{
    // Do / in terms of /= to maintain consistency.
    // Return value optimization compiles away the copy constructor.
    // const on return type disallows doing (u/v) = w ;
    return BigInt( u ) /= d ;
}

/*=============================================================================
 |
 | NAME
 |
 |     /=
 |
 | DESCRIPTION
 |
 |     Divide by a digit.
 |
 +============================================================================*/

BigInt & BigInt::operator/=( ppuint d )
{
    
    BigInt q ;
    ppuint   r ; // Thrown away.

    divMod( *this, d, q, r ) ;

    // Swap the digits of q and current object.
    swap( q.digit_, digit_ ) ;

    return *this ;
}



/*=============================================================================
|
| NAME
|
|     %
|
| DESCRIPTION
|
|     u mod v  for BigInts
|
+============================================================================*/

BigInt operator%( const BigInt & u, const BigInt & v )
{
    BigInt q ; // Thrown away.
    BigInt r ;

    divMod( u, v, q, r ) ;

    // Create a copy of r upon return and destruct r.
    return r ;
}



/*=============================================================================
|
| NAME
|
|     %
|
| DESCRIPTION
|
|     u mod d  for BigInt u and integer d
|
+============================================================================*/

ppuint operator%( const BigInt & u, const ppuint d )
{
    BigInt q ; // Thrown away.
    ppuint r ;

    divMod( u, d, q, r ) ;

    return r ;
}



/*=============================================================================
|
| NAME
|
|     divMod
|
| DESCRIPTION
|
|    Compute the quotient and remainder for
|    u / d where u is a BigInt and d is an integer.
|
|        (u    ... u  )
|          n-1      0  b
|    q = ------------------ = ( w    ... w  )
|                d               n-1      0  b
|
|    r = (u    ... u  )   mod d
|          n-1      0  b
|
+============================================================================*/

void divMod( const BigInt & u, 
             const ppuint   d,
             BigInt &       q, 
             ppuint &       r )
{
    // Get the number of digits and the base.
    ppuint b{ u.base_() } ;
    int  n{ static_cast<int>( u.digit_.size() ) } ;

    // Don't allow zero divide.
    if (d == static_cast<ppuint>( 0u ))
    {
        ostringstream os ;
        os << "BigInt::divMod  Divide by 0" ;
        throw BigIntZeroDivide( os.str(), __FILE__, __LINE__ ) ;
    }

    // u has no digits.
    if (n <= 0)
    {
        ostringstream os ;
        os << "BigInt::divMod  u/d = qd+r u has zero digits" ;
        throw BigIntMathError( os.str(), __FILE__, __LINE__ ) ;
    }

    // If q is not empty, clear it.
    if (q.digit_.size() > 0)
        q.digit_.clear() ;

    // Call multiprecision divide.
    if (d > b)
    { 
        // TODO:   We should do BigInt divMod here instead of aborting.
        //    BigInt rr ;
        //    divMod( u, static_cast<BigInt>( d ), q, rr ) ;
        //    r = static_cast<ppuint>( rr ) ;
        ostringstream os ;
        os << "BigInt::divMod " << " d = " << d << " > base b = " << b
           << " please email me to request a feature enhancement" ;
        throw BigIntOverflow( os.str(), __FILE__, __LINE__ ) ;
    }
    // In this special case, we just shift digits right.
    else if (d == b)
    {
        r = 0 ;
        
        // u is zero;  leave it alone.
        if (u == static_cast<BigInt>( 0u ))
            ;
        // The single digit of u is shifted out.  Only zero remains.
        else if (n == 1)
        {
            r = u.digit_[ 0 ] ;
            q.digit_.push_back( 0 ) ;
        }
        // Remainder r is the zeroth digit.  Shift all other digits into the quotient.
        else
        {
            r = u.digit_[ 0 ] ;

            for (int j = 1 ; j <= n-1 ;  ++j)
                q.digit_.push_back( u.digit_[ j ] ) ;

            #ifdef DEBUG_PP_BIGINT
            cout << "BigInt::divMod: the special case of divisor d = " << d << " equals " << " base b = " << b << endl ;
            #endif
        }
    }
    else
    {
        r = 0 ;

        // Long division from most to least significant digit.
        for (int j = n - 1 ; j >= 0 ;  --j)
        {
            ppuint t         = r * b + u.digit_[ j ] ;
            q.digit_.push_back( t / d ) ;
            r             = t % d ;
        }

        // Put the digits into correct order.
        reverse( q.digit_.begin(), q.digit_.end() ) ;

        // Trim leading zeros, if any, but stop if q = 0.
        while (q.digit_.size() > 1 && q.digit_.back() == 0)
            q.digit_.pop_back() ;
    }

    return ;
}



/*=============================================================================
|
| NAME
|
|     divMod
|
| DESCRIPTION
|
|     Compute u / v = q, r where all quantities are BigInts.
|
|    u = (u      ... u  u  )
|          m+n-1      1  0  b
|
|    v = (v    ... v  v  )
|          n-1      1  0  b
|
|    v   != 0,   n > 1
|     n-1
|
|    q =  | u / v | = (q  q... q  )
|         --     --     m  m-1  0  b
|
|    r = u mod v = (r   ... r  )
|                    n-1     0  b
|
|    u = q v + r
|
|    Throw BigIntZeroDivide if v = 0.
|
| EXAMPLE
|
|   b = 10
|
|   n = 4 
|   v = (v    v    v   v ) = 3457
|         n-1  n-2  1   0
|
|   m+n = 6 
|   u = (u      u       u  u  u  u ) = 398765
|         m+n-1  m+n-2   3  2  1  0
|
|   m = 2 
|
|           v3 v2 v1 v0
|                        --------------------
|                        ) u5 u4 u3 u2 u1 u0
|       v = 3  4  5  7   )  3  9  8  7  6  5 = u
|
|
|   Now copy v2 = v and u2 = u
|
|                            |       b       |   |   10      |
|   Normalizing factor = d = |   ----------  | = |   -----   | = 2
|                            --  vn-1 + 1   --   --  3 + 1  --
|
|   v2 = v2 * d
|   u2 = u2 * d
|
|   But let's use u and v instead of u2 and v2 for simplicity of notation.
|   The normalized division is then.  Note extra 0 appended at the left of u.
|
|           v3 v2 v1 v0                      1   1   5
|                         ----------------------------
|                         ) u6  u5  u4  u3  u2  u1  u0
|           6   9  1  4   )  0   7   9   7   5   3   0
|          vn-1            um+n
|                         -  0   6   9   1   4
|                            -----------------
|                            0   1   0   6   1   3
|                            -   0   6   9   1   4
|                                -----------------
|                                0   3   6   9   9   0
|                             -  0   3   4   5   7   0
|                                ---------------------
|                                    0   2   4   2   0
|
|   Recall n = 4  m = 2   m+n = 6 
|
|                                        *** Correction needed? ***
|
|        j      q           r            q >= b?    q >= b r + u  ?
|                                                               j+n-2
|
|        2   | 0 7 |      0 10 + 7       no         no
|            | --- |= 1   mod 6 = 1
|            -  6 -
|
|    subtraction (uj+n...uj) = 01061
|
|    trial quotient q2 = 1 r2 = 4
|    corrected trial quotient q2 = 1 r2 = 4
|    final trial quotient q2 = 1 r2 = 4
|    i = 0 j+i = 1 q2 = 1 borrow = 0
|    v2[ i ] = 4 u2[j+i] = 3 t2 = -1
|    corrected borrow = -1u2.[j+i] = 9
|    i = 1 j+i = 2 q2 = 1 borrow = -1
|    v2[ i ] = 1 u2[j+i] = 1 t2 = -1
|    corrected borrow = -1u2.[j+i] = 9
|    i = 2 j+i = 3 q2 = 1 borrow = -1
|    v2[ i ] = 9 u2[j+i] = 6 t2 = -4
|    corrected borrow = -1u2.[j+i] = 6
|    i = 3 j+i = 4 q2 = 1 borrow = -1
|    v2[ i ] = 6 u2[j+i] = 0 t2 = -7
|    corrected borrow = -1u2.[j+i] = 3
|    i = 4 j+i = 5 q2 = 1 borrow = -1
|    v2[ i ] = 0 u2[j+i] = 1 t2 = 0
|    corrected borrow = 0u2.[j+i] = 0
|    j = 1
|    After subtraction (uj+n...uj) = 03699
|    trial quotient q2 = 6 r2 = 0
|    corrected trial quotient q2 = 5 r2 = 6
|    final trial quotient q2 = 5 r2 = 6
|    i = 0 j+i = 0 q2 = 5 borrow = 0
|    v2[ i ] = 4 u2[j+i] = 0 t2 = -20
|    corrected borrow = -2u2.[j+i] = 0
|    i = 1 j+i = 1 q2 = 5 borrow = -2
|    v2[ i ] = 1 u2[j+i] = 9 t2 = 2
|    corrected borrow = 0u2.[j+i] = 2
|    i = 2 j+i = 2 q2 = 5 borrow = 0
|    v2[ i ] = 9 u2[j+i] = 9 t2 = -36
|    corrected borrow = -4u2.[j+i] = 4
|    i = 3 j+i = 3 q2 = 5 borrow = -4
|    v2[ i ] = 6 u2[j+i] = 6 t2 = -28
|    corrected borrow = -3u2.[j+i] = 2
|    i = 4 j+i = 4 q2 = 5 borrow = -3
|    v2[ i ] = 0 u2[j+i] = 3 t2 = 0
|    corrected borrow = 0u2.[j+i] = 0
|    j = 0
|    After subtraction (uj+n...uj) = 02420
|    m = 2n = 4
|    Quotient q = 115
|    Normalized remainder r = 1210
|
+============================================================================*/

void divMod( const BigInt & u, 
             const BigInt & v,
             BigInt &       q, 
             BigInt &       r )
{
    ppuint b{ u.base_() } ;
    int  m_n{ static_cast<int>( u.digit_.size()) } ;
    int    n{ static_cast<int>( v.digit_.size()) } ;
    int    m{ m_n - n } ; //  Compute m from the fact that u has m + n digits, and v has n digits.

    #ifdef DEBUG_PP_BIGINT
    cout << "\t------------divMod( " << u << " , " << v << " BigInt )---------" << endl ;
    cout << "\tm = " << m_n << endl ;
    cout << "\tn = " << n << endl ;
    cout << "\tu = " ; printNumber( u, cout ) ;
    cout << "\tv = " ; printNumber( v, cout ) ;
    #endif

    if (n < 1 || m_n < 1)
    {
        ostringstream os ;
        os << "BigInt::divMod() zero digits for either "
           << "num digits of u = " << m_n << " or num digits of v = " << n ;
        throw BigIntRangeError( os.str(), __FILE__, __LINE__ ) ;
    }

    // One digit division.
    if (n == 1)
    {
        // Make r have at least one digit.
        try
        {
            r.digit_.resize( 1 ) ;
        }
        catch( length_error & e )
        {
            ostringstream os ;
            os << "BigInt::divMod() had a length_error exception during r.digit_.resize( 1 )" ;
            throw BigIntRangeError( os.str(), __FILE__, __LINE__ ) ;
        }

        divMod( u, v.digit_[0], q, r.digit_[0] ) ;
        return ;
    }

    #ifdef DEBUG_PP_BIGINT
    cout << "\tm = " << m << endl ;
    #endif
    // If u < v, return q = 0, r = u.
    if (m < 0)
     {
        q = BigInt( 0u ) ;
        r = u ;
        return ;
    }

    // Allocate temporary space for normalized copies of u and v.  
    // Destructors for these temporaries will be called when we go out 
    // of scope at the end of this function.
    BigInt u2( u ) ;
    BigInt v2( v ) ;

    // Add an extra zero digit to the beginning of u:  u[ m+n ] = 0.
    u2.digit_.push_back( 0 ) ;

    // Add an extra zero digit to the beginning of v:  v[ n ] = 0.
    v2.digit_.push_back( 0 ) ;

    #ifdef DEBUG_PP_BIGINT
    cout << "\tu2 = " ; printNumber( u2, cout ) ;
    cout << "\tv2 = " ; printNumber( v2, cout ) ;
    #endif
    

    // Make certain the number of digits in q and r are sufficient.
    try
    {
        q.digit_.resize( m+1 ) ;
        r.digit_.resize( n ) ;
    }
    catch( length_error & e )
    {
        ostringstream os ;
        os << "BigInt::divMod() had a length_error exception during q.digit_resize( m = " << m+1 << " )"
           << "or r.digit_.resize( n = " << n << " )" ;
        throw BigIntRangeError( os.str(), __FILE__, __LINE__ ) ;
    }

    //  Assume v    != 0, and n > 1.  If not, skip leading zero digits.
    //          n-1
    //
    int k = static_cast<int>( v2.digit_.size() ) - 1 ;
    while( v2.digit_[ k ] == 0 && k >= 0)
        --k ;

    if (k < 0)
    {
        ostringstream os ;
        os << "BigInt::divMod " << " zero divide, k = " << k ;
        throw BigIntZeroDivide( os.str(), __FILE__, __LINE__ ) ;
    }

    if (n <= 1 || v2.digit_[ n-1 ] == 0)
    {
        ostringstream os ;
        os << "BigInt::divMod  Zero divide n = " << n << "v2.digit_[ " << n-1 << " ]" << v2.digit_[ n-1 ] ;
        throw BigIntZeroDivide( os.str(), __FILE__, __LINE__ ) ;
    }

    //  Normalizer.
    ppuint d = b / (v2.digit_[ n-1 ] + 1) ;

    #ifdef DEBUG_PP_BIGINT
    cout << "\tnormalizer d = " << d << endl ;
    #endif

    //  Skip normalizing step if d = 1.
    if (d == 1)
    {
        u2.digit_[ m+n ] = 0 ;
    }
    else
    {
        // Normalize (u     ... u   ) = (u      ... u )  d
        //             m+n-1     0        m+n-1      0

        ppuint carry = 0 ;
        for (int j = 0 ;  j <= m + n - 1 ;  ++j)
        {
            ppuint t{ u2.digit_[ j ] * d + carry } ;

            u2.digit_[ j ] = t % b ;
            carry          = t / b ;
        }
        u2.digit_[ m+n ] = carry ;

        // Normalize (v   ... v  ) = (v   ... v  )  d
        //             n-1     0       n-1     0
        //
        //
        carry = 0 ;
        for (int j = 0 ;  j <= n-1 ;  ++j)
        {
            ppuint t = v2.digit_[ j ] * d + carry ;

            v2.digit_[ j ] = t % b ;
            carry          = t / b ;
        }

        // No carry can occur, so flag an error if it happens.
        if (carry != 0)
        {
            ostringstream os ;
            os << "BigInt::divMod " << " carry = " << carry ;
            throw BigIntMathError( os.str(), __FILE__, __LINE__ ) ;
        }
    }

    #ifdef DEBUG_PP_BIGINT
    cout << "\tnormalized u2 = " ; printNumber( u2, cout ) ;
    cout << "\tnormalized v2 = " ; printNumber( v2, cout ) ;
    #endif


    //  Find the quotient digit,
    //
    //         |  (u    ... u  )   |
    //         |    j+n      j  b  |
    //   q  =  | ----------------- |
    //    j    |  (v   ... v  )    |
    //         |    n-1     0  b   |
    //         --                 --
    for (int j = m ;  j >= 0 ;  --j)
    {
        // First determine a trial quotient digit
        //
        //       |  (u    u     )    |
        //   ^   |    j+n  j+n-1  b  |
        //   q = | ----------------- |
        //       |      v            |
        //       |       n-1         |
        //       --                 --
        //
        ppuint temp = u2.digit_[ j+n ] * b + u2.digit_[ j+n-1 ] ;
        ppuint q2   = temp / v2.digit_[ n-1 ] ;
        ppuint r2   = temp % v2.digit_[ n-1 ] ;

        #ifdef DEBUG_PP_BIGINT
        cout << "\ttrial quotient q2 = " << q2 << " r2 = " << r2 << endl ;
        #endif

        // Correction if necessary.
        if (q2 >= b || (q2 * v2.digit_[ n-2 ] > b * r2 + u2.digit_[ j+n-2 ]))
        {
            --q2 ;
            r2 += v2.digit_[ n-1 ] ;
        }

        #ifdef DEBUG_PP_BIGINT
        cout << "\tcorrected trial quotient q2 = " << q2 << " r2 = " << r2 << endl ;
        #endif

        // Low probability repeat correction if necessary.
        if (r2 < b)
        {
            if (q2 >= b || (q2 * v2.digit_[ n-2 ] > b * r2 + u2.digit_[ j+n-2 ]))
            {
                --q2 ;
                // We don't use the remainder since this is the last correction.
                ///r2 += v2.digit_[ n-1 ] ;
            }
        }
        
        #ifdef DEBUG_PP_BIGINT
        cout << "\trepeat corrected trial quotient q2 = " << q2 << " r2 = " << r2 << endl ;
        #endif

        #ifdef DEBUG_PP_BIGINT
        cout << "\tfinal trial quotient q2 = " << q2 << " r2 = " << r2 << endl ;
        #endif

        // Multiply and subtract:
        //
        //         (u    u      ... u  )
        //           j+n  j+n-1      j
        //
        //  - q2 * (0    v    ...   v  )
        //                n-1        0
        //
        //  =
        //         (u    u      ... u  )
        //           j+n  j+n-1      j

        ppsint borrow{ 0 } ;
        for (int i = 0 ;  i <= n ;  ++i)
        {
            ppsint t2  = borrow + u2.digit_[ j + i ] - q2 * v2.digit_[ i ] ;

            #ifdef DEBUG_PP_BIGINT
            cout << "\t\ti = " << i << " j+i = " << j+i << " q2 = " << q2 << " borrow = " << borrow << endl ;
            cout << "\t\tv2[ i ] = " << getDigit( v2, i ) << " u2[j+i] = " << getDigit( u2, j+i ) << " t2 = " << t2 << endl ;
            #endif

            // Digit subtraction is positive:  don't need to borrow.
            if (t2 >= 0)
            {
                u2.digit_[ j + i ] = t2 ;
                borrow = 0 ;
            }
            //  If t2 is negative, we need to borrow.
            else
            {
                    // e.g. t = -1 -> -10,
                    // borrow = floor( (t+1)/b ) - 1 = 0/10 - 1 -> -9/10 - 1 => -1;
                    //  -11 -> -20:  borrow -2
                    borrow = -1 + (t2 + 1) / (ppsint) b ;

                    // Add the positive borrow to the digit to force it positive.
                    u2.digit_[ j + i ] = -borrow * (ppsint) b + t2 ;
            }
            #ifdef DEBUG_PP_BIGINT
            cout << "\t\tcorrected borrow = " << borrow << "u2.[j+i] = "<< getDigit( u2,j+i ) << endl ;
            #endif
        }

        #ifdef DEBUG_PP_BIGINT
        cout << "\tj = " << j << endl ;
        cout << "\tAfter subtraction (uj+n...uj) = " ;
        for (int k = j+n ;  k >= j ;  --k)
           cout << getDigit( u2, k ) ;
        cout << endl ;
        #endif

        // Save the quotient.
        q.digit_[ j ] = q2 ;


        // Decrease q2 and add back correction if q2 is too big.
        // Probability of this step given random digits is about 2 / b.
        //
        //      (u    u      ... u )
        //        n+j  n+j-1      j
        //
        //  +   (0    v    ...   v )
        //             n-1        0
        //  =
        //      (u    u      ... u )
        //        n+j  n+j-1      j
        //
        // Ignore the carry to the left of u    since it cancels with the earlier borrow.
        //                                  n+j
        if (borrow < 0)
        {
            --q.digit_[ j ] ;

            ppuint carry = 0 ;
            for (int i = 0 ;  i <= n ;  ++i)
            {
               ppsint t = u2.digit_[ j + i ] + v2.digit_[ i ] + carry ;

                u2.digit_[ j + i ] = t % b ;
                carry              = t / b ;
            }
        }
    } // end for j


    // Quotient is (q    ... q  )
    //               m        0  b

    // Trim leading zeros, if any, but stop if q = 0.
    while (q.digit_.size() > 1 && q.digit_.back() == 0)
        q.digit_.pop_back() ;

    #ifdef DEBUG_PP_BIGINT
    cout << "\tm = " << m << " n = " << n << endl ;
    cout << "\tQuotient q = " ; printNumber( q, cout ) ;
    #endif


    // Remainder:  (r    ... r  )   = (u    ... u  )  / d
    //               n-1      0  b      n-1      0  b
    if (d != 1)
    {
        ppuint remainder = 0 ;
        for (int j = n-1 ;  j >= 0 ;  --j)
        {
            ppuint t{ u2.digit_[ j ] + remainder * b } ;

            r.digit_[ j ] = t / d ;
            remainder     = t % d ;
    
            #ifdef DEBUG_PP_BIGINT
            cout << "\tRemainder normalization:  j = " << j << " t = " << t << endl ;
            cout << "\tr[ j ] = " << r.digit_[ j ] << " remainder = " << remainder << endl ;
            #endif
        }
    }
    // No need to normalize r, just copy digits of u.
    else
       for (int j = n-1 ;  j >= 0 ;  --j)
            r.digit_[ j ] = u2.digit_[ j ] ;

    
    #ifdef DEBUG_PP_BIGINT
    cout << "\tRemainder r = " ; printNumber( r, cout ) ;
    #endif

    // Trim leading zeros, if any, but stop if r = 0.
    while (r.digit_.size() > 1 && r.digit_.back() == 0)
        r.digit_.pop_back() ;

    #ifdef DEBUG_PP_BIGINT
    cout << "\tNormalized remainder r = " ;  printNumber( r, cout ) ;
    #endif

    return ;
}



/*=============================================================================
|
| NAME
|
|    power
|
| DESCRIPTION
|
| METHOD
|
|     Speed up by doing multiplication by repeated squaring.
|     Expand exponent n in binary.  Discard the leading 1 bit.
|     Thereafter, square for every 0 bit;  square and multiply
|     by p for every 1 bit.
|
|    100 = 0  . . . 0  1  1  0  0  1  0  0 (binary representation)
|          |<- ignore ->| S  S SX SX SX  S (exponentiation rule,
|          S = square, X = multiply by x)
|
+============================================================================*/

BigInt power( ppuint p, ppuint n )
{
    // Special cases first.
    if (p == static_cast<ppuint>( 0u ))
    {
        //   0
        //  0  = undefined
        if (n == static_cast<ppuint>( 0u ))
        {
            ostringstream os ;
            os << "BigInt::power  Attempt to do 0 ^ 0 in pow" ;
            throw BigIntRangeError( os.str(), __FILE__, __LINE__ ) ;
        }
        //   nonzero
        //  0        = 0
        else
            return BigInt( static_cast<ppuint>( 0u ) ) ;
    }
   else if (n == static_cast<ppuint>( 0u ))
   {
       //        0
       // nonzero  = 1
       return BigInt( static_cast<ppuint>( 1u ) ) ;
   }
    
    // Find the number of the leading bit.
    int bitNum = sizeof( n ) * 8u - 1u ; // Number of highest possible bit integer n.

    #ifdef DEBUG_PP_BIGINT
    cout << "\tn = " << n << " p = " << p << endl ;
    cout << "initial max bitNum = " << bitNum << endl ;
    #endif // NDEBUG

    while (!testBit( n, bitNum ))
        --bitNum ;

    #ifdef DEBUG_PP_BIGINT
    cout << "after skipping leading 0 bits, bitNum = " << bitNum << endl ;
    #endif // NDEBUG

    if (bitNum == -1)
    {
        ostringstream os ;
        os << "BigInt::power bitNum == -1 internal error in BigInt" ;
        throw BigIntMathError( os.str(), __FILE__, __LINE__ ) ;
    }

    // Exponentiation by repeated squaring.  Skip over the leading 1 bit.
    // Thereafter, square for every 0 bit;  square and multiply by p for
    // every 1 bit.

    BigInt w( p ) ;
    while( --bitNum >= 0 )
    {
        // Square.
        w *= w ;

        // Times p.
        if (testBit( n, bitNum ))
            w *= p ;

            #ifdef DEBUG_PP_BIGINT
            cout << "bitNum = " << bitNum << endl ;
            cout << "testBit( n ) = " << testBit(n,bitNum) << endl ;
            cout << "intermediate w = " << w << endl ;
            #endif // NDEBUG
    }

    // Return a copy of power.
    // As we go out of scope, power's destructor will be called.
    return w ;
}



/*=============================================================================
|
| NAME
|
|     ceilLg
|
| DESCRIPTION
|
|     --            --
|     |  log  ( n )  |
|            2
|
| EXAMPLE
|
|      BigInt n = 6 ;
|      n.ceilLg() => 3
|
| NOTES
|
|      Let n be written in binary, m bits long with a leading 1 bit:
|
|      n = 1 *  ...  *
|
|                 m       m
|      Then n <= 2 - 1 < 2
| 
|      Since log  = lg is monotonically increasing,
|               2
|
|      lg( n ) < m.
|      We define ceilLg( n ) = m
|
|      n = 6 = 110
|      There are m = 3 bits counting from leading 1 bit to lsb.
|      lg( n ) = 2.58496... = < 3 = ceilLg( n )
|
+============================================================================*/

int BigInt::ceilLg()
{
    int bitNum ;
    
    for (bitNum = maxBitNumber();  testBit( bitNum ) == false && bitNum >= 0 ;  --bitNum)
    #ifdef DEBUG_PP_BIGINT
    cout << "bitNum = " << bitNum << " testBit = " << testBit( bitNum ) << endl
    #endif // NDEBUG
     ;

    return bitNum + 1 ;
}



/*=============================================================================
|
| NAME
|
|     ==
|
| DESCRIPTION
|
+============================================================================*/

bool operator==( const BigInt & u, const BigInt & v )
{
    //  Get the number of digits.
    int  m     = static_cast<int>( u.digit_.size()) ;
    int  n     = static_cast<int>( v.digit_.size()) ;
    int max_n = (n > m) ? n : m ;

    // Different number of non-zero digits.
    if (m != n)
        return false ;

    for (int i = 0 ;  i < max_n ;   ++i)
    {
        // Two digits are not equal.
        if (u.digit_[ i ] != v.digit_[ i ])
            return false ;
    }

    // All digits equal.
    return true ;
}



/*=============================================================================
|
| NAME
|
|     ==
|
| DESCRIPTION
|
+============================================================================*/

bool operator==( const BigInt & u, const ppuint d )
{
    ppuint b{ u.base_() } ;

    // TODO:   We should do BigInt == here instead of aborting.
    if (d > b)
    {
        ostringstream os ;
        os << "BigInt::operator== " << " digit d = " << d << " > base b = " << u.base_()
           << " please email me to request a feature enhancement" ;
        throw BigIntRangeError( os.str(), __FILE__, __LINE__ ) ;
    }
    // Special case check to see if u = 10 in base b.
    else if (d == b)
    {
        if (u.digit_.size() != 2 || u.digit_[ 0 ] != 0 || u.digit_[ 1 ] != 1)
            return false ;
    }
    // More than one base b digit or no digits.
    else if (u.digit_.size() != 1)
        return false ;
    // One digit which equals d.
    else if (u.digit_[ 0 ] == d)
        return true ;

    return false ;
}


/*=============================================================================
|
| NAME
|
|     !=
|
| DESCRIPTION
|
+============================================================================*/

bool operator!=( const BigInt & u, const BigInt & v )
{
return !(u == v) ;
}



/*=============================================================================
|
| NAME
|
|     !=
|
| DESCRIPTION
|
+============================================================================*/

bool operator!=( const BigInt & u, const ppuint d )
{
    return !(u == d) ;
}



/*=============================================================================
|
| NAME
|
|    >
|
| DESCRIPTION
|
+============================================================================*/

bool operator>( const BigInt & u, const BigInt & v )
{
    //  Get the number of digits.
    int m = static_cast<int>( u.digit_.size()) ;
    int n = static_cast<int>( v.digit_.size()) ;

    // u has more non-zero digits than v.
    if (m > n)
        return true ;
    
    // u has fewer nonzero digits than v.
    else if (m < n)
       return false ;

    // Same number of digits.  Is a leading digit of u bigger?
    for (int i = n-1 ;  i >= 0 ;   --i)
    {
        if (u.digit_[ i ] > v.digit_[ i ])
            return true ;
        else if (u.digit_[ i ] < v.digit_[ i ])
            return false ;
    }

    // All digits equal.
    return false ;
}



/*=============================================================================
|
| NAME
|
|    >
|
| DESCRIPTION
|
+============================================================================*/

bool operator>( const BigInt & u, const ppuint d )
{
    ppuint b{ u.base_() } ;

    if (d >= b)
    {
        ostringstream os ;
        os << "BigInt::operator> " << "d = " << d << " > base = " << b ;
        throw BigIntOverflow( os.str(), __FILE__, __LINE__ ) ;
    }

    // More digits:  definitely greater.
    if (u.digit_.size() != 1)
        return true ;

    // One digit and greater.
    if (u.digit_[ 0 ] > d)
        return true ;

    return false ;
}



/*=============================================================================
|
| NAME
|
|     <
|
| DESCRIPTION
|
+============================================================================*/

bool operator<( const BigInt & u, const BigInt & v )
{
    return !(u > v || u == v) ;
}



/*=============================================================================
|
| NAME
|
|    <
|
| DESCRIPTION
|
+============================================================================*/

bool operator<( const BigInt & u, const ppuint d )
{
    return !(u > d || u == d) ;
}



/*=============================================================================
|
| NAME
|
| DESCRIPTION
|
+============================================================================*/

bool operator>=( const BigInt & u, const BigInt & v )
{
    return (u > v) || u == v ;
}



/*=============================================================================
|
| NAME
|
|     >=
|
| DESCRIPTION
|
+============================================================================*/

bool operator>=( const BigInt & u, ppuint d )
{
    return (u > d) || u == d ;
}



/*=============================================================================
|
| NAME
|
| DESCRIPTION
|
+============================================================================*/

bool operator<=( const BigInt & u, const BigInt & v )
{
    return u < v || u == v ;
}



/*=============================================================================
|
| NAME
|
|     <=
|
| DESCRIPTION
|
+============================================================================*/

bool operator<=( const BigInt & u, const ppuint d )
{
    return u < d || u == d ;
}



/*=============================================================================
|
| NAME
|
| DESCRIPTION
|
+============================================================================*/

BigInt operator&( const BigInt & u, const BigInt & n )
{
    // Allocate temporary space for the result which will
    // be destructed as this function goes out of scope.
    BigInt w ;

    // Return a copy of the result.
    return w ;
}



/*=============================================================================
|
| NAME
|
|     Left shift operator << for BigInt
|
| DESCRIPTION
|
|     Bit shift left.
|
+============================================================================*/

BigInt operator<<( const BigInt & u, ppuint n )
{
    // Allocate temporary space for the result which will
    // be destructed as this function goes out of scope.
    BigInt w ;

    // Return a copy of the result.
    return w ;
}



/*=============================================================================
|
| NAME
|
|     testBit
|
| DESCRIPTION
|
|     Bit test for BigInt precision integers.
|
+============================================================================*/

const bool BigInt::testBit( const int bitNum ) const
{
    int n = static_cast<int>( digit_.size()) ;

    // Compute the digit in which the bit lies.
    //
    // e.g. if there are 7 bits per digit, bitNum = 8 lies in digit = 1.
    // and is subbit number 1.
    //
    //      87 6543210    Test the 8th bit.
    // 0000010 0000000    bitNum    = 8
    //       1       0    digitNum  = 1
    // 6543210 6543210    subBitNum = 8 - 1 * 7 = 1
    int digitNum = bitNum / numBitsPerDigit_() ;

    if (digitNum > n - 1)
    {
        ostringstream os ;
        os << "BigInt::testBit( " << bitNum << " ) is out of range" << endl
           << "The number has " << numBitsPerDigit_() * n << " bits" << endl ;
                throw BigIntRangeError( os.str(), __FILE__, __LINE__ ) ;
    }

    int subBitNum = bitNum - digitNum * numBitsPerDigit_() ;

	return (digit_[ digitNum ] & (static_cast<ppuint>( 1u ) << subBitNum)) ? true : false ;
}

//  Bit test for low precision integers.
const bool testBit( const ppuint n, const int bitNum )
{
	return (n & (static_cast<ppuint>( 1u ) << bitNum)) ? true : false ;
}



/*=============================================================================
|
| NAME
|
|     maxBitNumber
|
| DESCRIPTION
|
|     Highest bit number in a BigInt number.
|
+============================================================================*/

int BigInt::maxBitNumber() const
{
    // Highest bit number = number of digits * bits per digit - 1
    return numBitsPerDigit_() * static_cast<int>( digit_.size()) - 1 ;
}

/*=============================================================================
|
| NAME
|
|     getBase
|
| DESCRIPTION
|
|     Return the BigInt base.
|
+============================================================================*/

const ppuint BigInt::getBase()
{
    return base_() ;
}


/*=============================================================================
|
| NAME
|
|     getDigit
|
| DESCRIPTION
|
|     Return the nth digit of the BigInt.  For testing only.
|
+============================================================================*/

const ppuint getDigit( const BigInt & u, const int n )
{
    if (n >= 0 && n < (int)u.digit_.size())
        return u.digit_[ n ] ;
    else
    {
        ostringstream os ;
        os << "BigInt::getDigit( " << n << " ) is out of range"
           << "The number has " << u.digit_.size() << " digits" ;
                throw BigIntRangeError( os.str(), __FILE__, __LINE__ ) ;
    }
}


/*=============================================================================
|
| NAME
|
|     getNumDigits
|
| DESCRIPTION
|
|     Return the number BigInt digits n.
|
+============================================================================*/

const int getNumDigits(const BigInt & u )
{
    return static_cast<int>( u.digit_.size() ) ;
}


/*=============================================================================
|
| NAME
|
|     setBase
|
| DESCRIPTION
|
|     Forcibly reset the base for all BigInt numbers.  Used for testing only.
|
+============================================================================*/

void setBase( const BigInt & u, const ppuint base )
{
   *u.pbase = base ;
}

            
/*=============================================================================
|
| NAME
|
|     printNumber
|
| DESCRIPTION
|
|     Print out a BigInt quantity to an output stream.  If the BigInt is not
|     defined or initialized, print out "undefined".
|
| EXAMPLE
|
|     For standard output to the console,
|         BigInt w( "68719476735", cout ) ;
|        printNumber(r)
|     gives
|        68719476735 [digits = 31 2147483647 base b = 2147483648 number of digits = 2]
|
+============================================================================*/

void printNumber( const BigInt & u, ostream & out = cout )
{
    // If the BigInt was not initialized. Don't throw an exception since this is
    // only a debug print routine.
    if (u.base_() == 0 || u.digit_.size() == 0)
    {
        out << "undefined" << endl ;
        return ;
    }
    
    // The number itself in decimal.
    out << u.toString() ;
                                         
    // Base b digits.
    out << "    [ " << getNumDigits( u ) << " digit" << (getNumDigits(u) > 1 ? "s" : "") << " = " ;

    for (int i = getNumDigits( u ) - 1 ;  i >= 0 ; --i)
        out << getDigit( u, i ) << " " ;

    out << " base b = " << BigInt::getBase() << " ]" << endl ;
}

            
/*=============================================================================
|
| NAME
|
|     printNumber
|
| DESCRIPTION
|
|     Print out a BigInt quantity to the standard output stream.
|     You can call it in the lldb debugger from the command line.
|
| EXAMPLE
|
|    (lldb) expr printNumber(r)
|    68719476735 [digits = 31 2147483647 base b = 2147483648 number of digits = 2]
|
+============================================================================*/

void printNumber( const BigInt & u )
{
    printNumber( u, cout ) ;
}

/*=============================================================================
|
| NAME
|
|     Bigint::base_
|
| DESCRIPTION
|
|     Initialize the base of the multiprecision arithmetic system.
|     Throw BigIntRangeError.
|
| EXAMPLE
|
|     BigInt::useBase()
|     {
|         ppuint b{ base_() } ;
|         ...
|     }
|
+============================================================================*/

ppuint & BigInt::base_()
{
    // Base of the number system used for each digit.  If a digit has can hold N bits,
    // the signed integer maximum is
    //          N-1
    //         2    - 1
    //
    // If we let the base be
    //
    //          N/2 - 1
    //     b = 2
    //
    //       2    N-2
    // then b  = 2    will not only fit into a signed integer but will also be a power
    //
    // of 2, making bit shifting and testing much easier.
    //
    // e.g. for N = 8, the largest positive number = 01111111 = 127, base b = 8 whose
    // square, 64 is less than the signed maximum.
    //
    static ppuint base = static_cast<ppuint>(1u) << numBitsPerDigit_() ;
    
    // Point to it for testing.  Use reference?
    pbase = &base ;

    // Detect if the base is way out of range, and build up an exception.
    if (base <= 0)
    {
        ostringstream os ;
        os << "BigInt::base_() base  = " << base << " <= 0 " ;
        throw BigIntRangeError( os.str(), __FILE__, __LINE__ ) ;
    }

    return base ;
} 



/*=============================================================================
|
| NAME
|
|     Bigint::numBitsPerDigit_
|
| DESCRIPTION
|
|     Initialize the base of the multiprecision arithmetic system.
|     Throw BigIntRangeError.
|
| EXAMPLE
|
|     BigInt::useBase()
|     {
|         ppuint b = base_() ;
|         ...
|     }
|
+============================================================================*/

int & BigInt::numBitsPerDigit_()
{
    // Base of the number system used for each digit.  If a digit has can hold N bits,
    // we let
    //          N/2 - 1
    //     b = 2
    //
    //          2
    // so that b  fits into a digit and b is an integer number of bits in
    // length, making bit shifting and testing easier.
    //
    // Use half the number of bits in an unsigned integer.
    static int num_bits_per_digit = (sizeof( ppuint ) * 8) / 2 - 1 ;

    #ifdef DEBUG_PP_BIGINT
    cout << "numBitsPerDigit_():" << endl ;
    cout << "sizeof( ppuint ) = " << sizeof( ppuint ) << " bytes" << endl ;
    cout << "num_bits_per_digit = " << num_bits_per_digit << endl ;
    cout << "base = 1 << num_bits_per_digit = " << (static_cast<ppuint>(1u) << num_bits_per_digit) << endl ;
    #endif

    if (num_bits_per_digit <= 0)
    {
        ostringstream os ;
        os << "BigInt::numBitsPerDigit()_  num_bits_per_digit  = " << num_bits_per_digit << " <= 0" ;
        throw BigIntRangeError( os.str(), __FILE__, __LINE__ ) ;
    }

    return num_bits_per_digit ;
}
