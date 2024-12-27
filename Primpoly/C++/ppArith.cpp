#include <cstdlib>      
#include <iostream>     
#include <new>          
#include <cmath>        
#include <complex>      
#include <fstream>      
#include <sstream>      
#include <vector>       
#include <string>       
#include <algorithm>    
#include <stdexcept>    
#include <cassert>      

using namespace std ;



#include "Primpoly.hpp"		  
#include "ppArith.hpp"		  
#include "ppBigInt.hpp"         
#include "ppOperationCount.hpp" 
#include "ppFactor.hpp"         
#include "ppPolynomial.hpp"	  
#include "ppParser.hpp"	      
#include "ppUnitTest.hpp"       



template <typename UIntType, typename SIntType>
inline UIntType ModP<UIntType,SIntType>::operator()( SIntType n )
{
    if (p_ <= 0)
    {
        ostringstream os ;
        os << "ModP::operator()  The modulus p = " << p_ << " <= 0" ;
        throw ArithModPError( os.str(), __FILE__, __LINE__ ) ;
    }
    
    if (n >= 0)
    {
        
        if (p_ == 2)
            return n - ((n >> 1) << 1) ;
        else
            return n % p_ ;
    }
    
     else
        return( (n % static_cast<SIntType>(p_)) + p_ ) ;
}




template <typename IntType>
IntType PowerMod<IntType>::operator()( const IntType & a, const IntType & n )
{
    IntType a1 = a ;

    
    if (a  <  static_cast<IntType>( 0u ) || 
        n  <  static_cast<IntType>( 0u ) || 
        p_ <= static_cast<IntType>( 1u ) || 
       (a  == static_cast<IntType>( 0u ) && n == static_cast<IntType>( 0u )))
    {
        ostringstream os ;
        os << "PowerMod::operator()  One or more parameters out of range:  a = "
           << a << " n = " << n  << " p_ = " << p_ ;
        throw ArithModPError( os.str(),  __FILE__, __LINE__ ) ;
    }

    
    if (a == static_cast<IntType>( 0u ))
        return static_cast<IntType>( 0u ) ;

    if (n == static_cast<IntType>( 0u ))
        return static_cast<IntType>( 1u ) ;

    if (n == static_cast<IntType>( 1u ))
        return a % static_cast<IntType>( p_ ) ;

    int bitNum = n.maxBitNumber() ; 

    #ifdef DEBUG_PP_ARITH
    cout << "initial max bitNum = " << bitNum << endl ;
    cout << "a = " << a << endl ;
    #endif
    
    
    while (!n.testBit( bitNum ))
        --bitNum ;

    #ifdef DEBUG_PP_ARITH
    cout << "after skipping leading 0 bits, bitNum = " << bitNum << endl ;
    #endif

    if (bitNum == -1)
    {
        ostringstream os ;
        os << "PowerMod::operator() " << "bitNum == -1" ;
        throw ArithModPError( os.str(), __FILE__, __LINE__ ) ;
    }

    #ifdef DEBUG_PP_ARITH
    cout << "\nAfter skipping zero bits, bitNum = " << bitNum << endl ;
    #endif

    
    
    
    while ( --bitNum >= 0 )
    {
        a1 = (a1 * a1) % static_cast<IntType>( p_ ) ; 

        if (n.testBit( bitNum ))
            a1 = (a1 * a) % static_cast<IntType>( p_ ) ; 

        #ifdef DEBUG_PP_ARITH
        cout << "S " ;
        if (n.testBit( bitNum ))
            cout << "X " ;
        cout << "Bit num = " << bitNum << " a1 = " << a1 << endl ;
        #endif
    }

    #ifdef DEBUG_PP_ARITH
        cout << "Out of the loop bitNum = " << bitNum << " a1 = " << a1 << endl ;
    #endif

    return a1 ;
}





template <typename IntType>
IntType addMod( IntType a, IntType b, IntType n )
{
    #ifdef DEBUG_PP_ARITH
    cout << "addMod" << endl ;
    cout << "    sizeof IntType = " << 8 * sizeof( IntType ) << " bits" << endl ;
    cout << "    a = " << a << " b = " << b << " n = " << n << endl ;
    #endif

    
    a %= n ;
    b %= n ;

    
    IntType c = a + b ;

    #ifdef DEBUG_PP_ARITH
    cout << "    a mod n = " << a << " b mod n = " << b << endl ;
    cout << "    c = a + b (discarding carry bit) = " << c << endl ;
    #endif

    
    
    
    
    
    
    if (c < a || c < b)
    {
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        c -= n ;

        #ifdef DEBUG_PP_ARITH
        cout << "    Carry!" << " c < a = " << (c < a ) << "|| c < b = " << (c < b) << endl ;
        #endif
    }
    else
    {
        c %= n ;

        #ifdef DEBUG_PP_ARITH
        cout << "    No carry" << endl ;
        #endif
    }

    return c ;
}





template <typename IntType>
IntType timesTwoMod( IntType a, IntType n )
{
    #ifdef DEBUG_PP_ARITH
    cout << "timesTwoMod" << endl ;
    cout << "    sizeof IntType = " << 8 * sizeof( IntType ) << " bits" << endl ;
    cout << "    a = " << a << " n = " << n << endl ;
    #endif

    
    a %= n ;

    #ifdef DEBUG_PP_ARITH
    cout << "    a mod n = " << a << endl ;
    #endif

    
    IntType mask = ((IntType)1 << (8 * sizeof( IntType ) - 1)) ;

    IntType c = (a << 1) ; 

    #ifdef DEBUG_PP_ARITH
    cout << "    mask = " << hex << mask << dec << endl ;
    cout << "    c = 2 a (losing the carry bit) = " << c << endl ;
    #endif

    
    if (mask & a)
    {
        
        c -= n ;

        #ifdef DEBUG_PP_ARITH
        cout << "    Carry!" << endl ;
        #endif
    }
    else
    {
        c %= n ;

        #ifdef DEBUG_PP_ARITH
        cout << "    No carry" << endl ;
        #endif
    }

    return c ;
}





template<typename IntType>
IntType multiplyMod( const IntType a, const IntType b, const IntType n )
{
    
    const int numBits = 8 * sizeof( IntType ) ;
    IntType mask = ((IntType)1 << (numBits - 1)) ;

    IntType r { 0 } ;

    for (int i = numBits-1 ;  i >= 0 ;  --i, mask >>= 1)
    {
        r = timesTwoMod( r, n ) ;

        if (mask & b)
            r = addMod( r, a, n ) ;
    }

    return r ;
}





template<>
ppuint PowerMod<ppuint>::operator()( const ppuint & a, const ppuint & n )
{
    
    
    ppuint mask{ static_cast<ppuint>(1) << (8 * sizeof( ppuint ) - 1) } ;
    int  bit_count{ 0 } ;  
    ppuint n1{ n } ;
    ppuint product{ a }  ;

    
    if (p_ <= 1 || (a == 0 && n1 == 0))
    {
        ostringstream os ;
        os << "PowerMod<ppuint>::operator() parameters are out of range:  a = " << a << " n = " << n  << " p_ = " << p_ ;
        throw ArithModPError( os.str(), __FILE__, __LINE__ ) ;
    }

    
    
    if (a == 0)
        return 0 ;

    if (n == 0)
        return 1 ;

    if (n == 1)
        return a % p_ ;

    #ifdef DEBUG_PP_ARITH
    cout << "a                    = " << a << endl ;
    cout << "n                    = " << n << endl ;
    cout << "p                    = " << p_ << endl ;
    cout << "n1 (before shifting) = " << n1 << endl ;
    #endif

    
    
    while (! (n1 & mask))
    {
        n1 <<= 1 ;
        ++bit_count ;
    }

    bit_count = (8 * sizeof( ppuint )) - bit_count ;

    #ifdef DEBUG_PP_ARITH
    cout << "n1        = " << n1 << endl ;
    cout << "mask      = " << mask << endl ;
    cout << "bit_count = " << bit_count << endl ;
    #endif

    
    
    while ( --bit_count > 0 )
    {
        #ifdef DEBUG_PP_ARITH
        cout << "product (before squaring) = " << product << " n1 = " << n1 << endl ;
        #endif

        
        n1 <<= 1 ;

        
        if (product > BigInt::getBase() || a > BigInt::getBase())
        {
            
            product = multiplyMod( product, product, p_ ) ;

            
            if (n1 & mask)
                product = multiplyMod( a, product, p_ ) ;
        }
        else
        {
            
            product = (product * product) % p_ ;

            
            if (n1 & mask)
                product = (a * product) % p_ ;
        }

        #ifdef DEBUG_PP_ARITH
        cout << "S " ;
        if (n1 & mask)
            cout << "X " ;
        cout << "product = " << product << " n1 = " << n1 << endl ;
        #endif
    }

    return product ;
}





bool IsPrimitiveRoot::operator()( ppuint a )
{
    PowerMod<ppuint> powermod( p_ ) ;

    
    if (a == 0)
        return false ;


    
    
    if (p_ < 2 || a < 1 || (p_ > 2 && (p_ % 2 == 0)))
    {
        ostringstream os ;
        os << "IsPrimitiveRoot::operator()  Inputs are out of range: p = " << p_ << " a = " << a ;
        throw ArithModPError( os.str(), __FILE__, __LINE__ ) ;
    }

    
    
    
    
    
    if ( (p_ == 2  &&  a == 1) ||
         (p_ == 3  &&  a == 2) ||
         (p_ == 5  && (a == 2  || a == 3)) ||
         (p_ == 7  && (a == 3  || a == 5)) ||
         (p_ == 11 && (a == 2  || a == 6   || a == 7 || a == 8)) ||
         (p_ == 13 && (a == 2  || a == 6   || a == 7 || a == 11)))
    {
        return true ;
    }

    
    a = a % p_ ;

    
    if (a == 0)
        return false ;

    
    Factorization<ppuint> factorization( p_ - 1 ) ;

    
    
    
    
    
    
    for (unsigned int i = 0 ;  i < factorization.numDistinctFactors() ;  ++i)
    {
        if (powermod( a, (p_ - 1) / factorization.primeFactor( i )) == 1)
        {
            return false ;
        }
    }

    return true ;
} 





ppsint InverseModP::operator()( ppsint u )
{
    ModP<ppsint,ppsint> mod( p_ ) ;

    
	ppsint t1 = 0 ;
	ppsint t3 = 0 ;
	ppsint q  = 0 ;

    ppsint u1 = 1 ;
    ppsint u3 = u ;
    ppsint v1 = 0 ;
    ppsint v3 = p_ ;

	ppsint inv_v = 0 ;

	while( v3 != 0)
	{
        
        q = static_cast<ppsint>(u3 / v3) ;

		t1 = u1 - v1 * q ;
		t3 = u3 - v3 * q ;

		u1 = v1 ;
		u3 = v3 ;

		v1 = t1 ;
		v3 = t3 ;
	}

    inv_v = mod( u1 ) ;

    
	
    
	if ( mod( u * inv_v ) != 1)
	{
        ostringstream os ;
        os << "InverseModP::operator():  inverse self check failed:  u = " << u << " * u^(-1) = " << inv_v  << " != 1" ;
        throw ArithModPError( os.str(), __FILE__, __LINE__ ) ;
	}

	return inv_v ;
}




bool ArithModP::constCoeffTest( ppsint a, ppsint a0, int n )
{
    ppsint constant_coeff = a0 ;

    ModP<ppuint,ppsint> mod( p_ ) ; 

    if (n % 2 != 0)
        constant_coeff = -constant_coeff ;    

    return( (mod( a - constant_coeff ) == 0) ? true : false ) ;
} 





bool ArithModP::constCoeffIsPrimitiveRoot( ppuint a0, int n )
{
    ppsint constant_coeff = a0 ;

    ModP<ppuint,ppsint> mod( p_ ) ;
    IsPrimitiveRoot isroot( p_ ) ;

    
    if (n % 2 != 0)
        constant_coeff = -constant_coeff ;

    return isroot( mod( constant_coeff ) ) ;
} 





template <typename IntType>
IntType gcd( const IntType & u, const IntType & v )
{
    IntType r ;
    IntType u2 { u } ;
    IntType v2 { v }  ;

    #ifdef DEBUG_PP_ARITH
    cout << "gcd:  u = " << u << " v = " << v << endl ;
    #endif

    while (v2 != static_cast<IntType>(0u))
    {
        r  = u2 % v2 ;
        u2 = v2 ;
        v2 = r ;

       #ifdef DEBUG_PP_ARITH
       cout << "  r = " << r << " u2 = " << u2 << " v2 = " << v2 << endl ;
       #endif
    }

    return u2 ;
}










template ppuint   gcd( const ppuint &, const ppuint & ) ;
template BigInt   gcd( const BigInt &, const BigInt & ) ;

template ppuint addMod( ppuint a, ppuint b, ppuint n ) ;
template ppuint timesTwoMod( ppuint, ppuint ) ;
template ppuint multiplyMod( const ppuint, const ppuint, const ppuint ) ;



template BigInt  PowerMod<BigInt>::operator()( const BigInt &, const BigInt & ) ;

template ModP<ppuint,ppsint>::ModP( ppuint ) ;
template ModP<ppuint,ppsint>::ModP( const ModP & ) ;
template ppuint ModP<ppuint,ppsint>::operator()( ppsint ) ;

