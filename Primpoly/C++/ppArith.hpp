#ifndef __PPARITH_H__
#define __PPARITH_H__

/*=============================================================================
|
| NAME
|
|     ArithModPException
|
| DESCRIPTION
|
|     Helper class for exceptions thrown by ArithModP.
|
+============================================================================*/

class ArithModPError : public runtime_error
{
    public:
        // Throw with error message, file name and line number.
        ArithModPError( const string & description, const string & file, const int & line )
        : runtime_error( description + " in file " + file + " at line " + to_string(line) )
        {
        } ;
    
        // Throw with error message.
        ArithModPError( const string & description )
			: runtime_error( description )
        {
        } ;

        // Throw with no error message.
        ArithModPError()
			: runtime_error( "ArithModP exception:  " )
        {
        } ;

} ; // end class ArithModPError


/*=============================================================================
|
| NAME
|
|     ArithModP
|
| DESCRIPTION
|
|     Abstract classes for modulo p arithmetic operations on integers.
|
|     ppuint p{ 2 ;
|     ppuint n{ 4 } ;
|     ppuint a0{ 14 } ;
|     ArithModP modp( p ) ;
|     modp.constCoeffIsPrimitiveRoot( a0, n ) ;
|
| NOTES
|
|     Use the functionoid approach so we can (1) save state and (2) have
|     a function interface.
|     The member functions and friends are documented in detail ppArith.cpp
|     Prevent implicity integer type conversion with "explicit".
|
+============================================================================*/

class ArithModP
{
	public:
        ArithModP() : p_( 2 ) {} ;
        explicit ArithModP( ppuint p ) : p_( p ) {} ;

        bool constCoeffTest( ppsint a, ppsint a0, int n ) ;
        bool constCoeffIsPrimitiveRoot( ppuint a0, int n ) ;

    protected:
        ppuint p_ ;    // Modulus for all arithmetic operations.

    private:
 	    // Don't let anyone define the default constructor,
        // copy constructor or assignment
        // operator for this class.
        ArithModP( const ArithModP & ) ;
        ArithModP & operator=( const ArithModP & ) ;
} ;



/*=============================================================================
|
| NAME
|
|     ModP
|
| DESCRIPTION
|
|     Abstract classes for modulo p arithmetic operations on integers.
|
|     ppuint p{ 7 } ;
|     ModP modp( p ) ;                   // Set p = 7 for all subsequent operations.
|     ppuint rem_33_mod_7 = modp( 33 ) ; // Use as a functionoid.
|
| NOTES
|
|     Use the functionoid approach so we can (1) save state and (2) have
|     a function interface.
|     The member functions and friends are documented in detail ppArith.cpp
|
+============================================================================*/

template <typename UIntType, typename SIntType>
class ModP
{
    public:
        explicit ModP( UIntType p )
        {
            p_ = p ;
        } ;

        ModP( const ModP & mod )
              : p_( mod.p_ )
        {
        }

        void set( UIntType p )
        {
            p_ = p ;
        }

        UIntType operator()( SIntType n ) ;

    protected:
        UIntType p_ ;    // Modulus for all arithmetic operations.
} ;


/*=============================================================================
|
| NAME
|
|     PowerMod
|
| DESCRIPTION
|
|     Abstract classes for modulo p arithmetic operations on integers.
|
|     ppuint p{ 7 } ;
|     PowerMod power_mod( p ) ;
|     ppuint threeToTheTenthmodp = power_mod( 3, 10 ) ;   // Use as a functionoid.
|
| NOTES
|
|     Use the functionoid approach so we can (1) save state and (2) have
|     a function interface.
|     The member functions and friends are documented in detail ppArith.cpp
|
+============================================================================*/

// Generic class.
template <typename IntType>
class PowerMod
{
    public:
        explicit PowerMod( const IntType & p ) : p_( p ) {} ;

        IntType operator()( const IntType & a, const IntType & n ) ;

    protected:
        IntType p_ ;    // Modulus for all arithmetic operations.
} ;


/*=============================================================================
|
| NAME
|
|     InverseModP
|
| DESCRIPTION
|
|     Abstract classes for modulo p arithmetic operations on integers.
|
|     InverseModP modp( p ) ;           // Set p = 7 for all subsequent operations.
|     ppuint sevenmodp = modp( 33 ) ;   // Use as a functionoid.
|
|
| NOTES
|
|     Use the functionoid approach so we can (1) save state and (2) have
|     a function interface.
|     The member functions and friends are documented in detail ppArith.cpp
|
+============================================================================*/

class InverseModP
{
      public:
         explicit InverseModP( ppuint p ) { p_ = p ; } ;
         ppsint operator()( ppsint a ) ;
    
    protected:
        ppuint p_ ;    // Modulus for all arithmetic operations.
} ;


/*=============================================================================
 | 
 | NAME
 | 
 | 
 | 
 | DESCRIPTION
 | 
 | 
 +============================================================================*/
 
class IsPrimitiveRoot
{
    public:
        explicit IsPrimitiveRoot( ppuint p )
        {
            p_ = p ;
        } ;

        bool operator()( ppuint a ) ;
    
    protected:
        ppuint p_ ;    // Modulus for all arithmetic operations.
} ;



/*=============================================================================
|
| NAME
|
|     Stand alone generic IntType functions:
|         gcd
|         addMod
|         timesTwoMod
|         multiplyMod
|
| DESCRIPTION
|
|     GCD of nonnegative integer types.
|
+============================================================================*/

template <typename IntType> IntType gcd( const IntType & u, const IntType & v ) ;
template <typename IntType> IntType addMod(      IntType a,             IntType b,       IntType n ) ;
template <typename IntType> IntType timesTwoMod( IntType a,                              IntType n ) ;
template <typename IntType> IntType multiplyMod( const IntType a, const IntType b, const IntType n ) ;

#endif // __PPARITH_H__
