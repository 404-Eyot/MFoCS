#ifndef __PP_BIGINT_H__
#define __PP_BIGINT_H__




class BigIntMathError : public runtime_error
{
    public:
        
        BigIntMathError( const string & description, const string & file, const int & line )
        : runtime_error( description + " in file " + file + " at line " + to_string(line) )
        {
        } ;

        
        BigIntMathError( const string & description )
			: runtime_error( description )
        {
        } ;

        
        BigIntMathError()
			: runtime_error( "BigInt math error:  " )
        {
        } ;

} ; 


class BigIntRangeError : public BigIntMathError
{
    public:
        
        BigIntRangeError( const string & description, const string & file, const int & line )
        : BigIntMathError( description + " in file " + file + " at line " + to_string(line) )
        {
        } ;

        
        BigIntRangeError( const string & description )
			: BigIntMathError( description )
        {
        } ;

        
        BigIntRangeError()
			: BigIntMathError( "BigInt range error:  " )
        {
        } ;

} ; 


class BigIntDomainError : public BigIntMathError
{
    public:
        
        BigIntDomainError( const string & description )
			: BigIntMathError( description )
        {
        } ;

        
        BigIntDomainError()
			: BigIntMathError( "BigInt domain error:  " )
        {
        } ;

} ; 


class BigIntOverflow : public BigIntMathError
{
      public:
         
         BigIntOverflow( const string & description, const string & file, const int & line )
         : BigIntMathError( description + " in file " + file + " at line " + to_string(line) )
         {
         } ;

          BigIntOverflow( const string & description )
              : BigIntMathError( description )
          {
          } ;

          BigIntOverflow()
              : BigIntMathError( "BigInt overflow. " )
          {
          }
} ;


class BigIntUnderflow : public BigIntMathError
{
      public:
         
         BigIntUnderflow( const string & description, const string & file, const int & line )
         : BigIntMathError( description + " in file " + file + " at line " + to_string(line) )
         {
         } ;

          BigIntUnderflow( const string & description )
              : BigIntMathError( description )
          {
          } ;

          BigIntUnderflow()
              : BigIntMathError( "BigInt underflow. " )
          {
          }
} ;


class BigIntZeroDivide : public BigIntMathError
{
      public:
        
        BigIntZeroDivide( const string & description, const string & file, const int & line )
        : BigIntMathError( description + " in file " + file + " at line " + to_string(line) )
        {
        } ;

          BigIntZeroDivide( const string & description )
              : BigIntMathError( description )
          {
          } ;

          BigIntZeroDivide()
              : BigIntMathError( "BigInt zero divide. " )
          {
          }
} ;




class BigInt
{
    public:
        

        

		
        
        BigInt() ;

        
        ~BigInt() ;

        
        
        
        explicit BigInt( const ppuint d ) ;
        explicit BigInt( const ppuint32 d ) ;

        
        
        explicit BigInt( const string & s ) ;

        
        
        BigInt( const BigInt & u ) ;

        
        
        BigInt & operator=( const BigInt & n ) ;

		
        
        operator ppuint() const ;


        
        
        
        friend ostream & operator<<( ostream & out, const BigInt & u ) ;

        
        friend istream & operator>>( istream & in,        BigInt & u ) ;

        
        friend const BigInt operator+( const BigInt & u, const BigInt & v ) ;

        friend const BigInt operator+( const BigInt & u, const ppuint d ) ;

        BigInt & operator+=( const BigInt & u ) ;

        BigInt & operator+=( const ppuint d ) ;

        
        BigInt & operator++() ;

        const BigInt operator++( int ) ;

        
        friend const BigInt operator-( const BigInt & u, const BigInt & v ) ;

        friend const BigInt operator-( const BigInt & u, const ppuint d ) ;

        BigInt & operator-=( const BigInt & u ) ;

        BigInt & operator-=( const ppuint d ) ;

        
        BigInt & operator--() ;

        BigInt operator--( int ) ;

        
        friend const BigInt operator*( const BigInt & u, const BigInt & v ) ;

        friend const BigInt operator*( const BigInt & u, const ppuint d ) ;

        BigInt & operator*=( const BigInt & u ) ;

        BigInt & operator*=( ppuint d ) ;

        
        
        friend const BigInt operator/( const BigInt & u, const BigInt & v ) ;

        friend const BigInt operator/( const BigInt & u, const ppuint d ) ;

        BigInt & operator/=( const BigInt & u ) ;

        BigInt & operator/=( ppuint d ) ;

        
        friend BigInt operator%( const BigInt & u, const BigInt & v ) ;

        friend ppuint   operator%( const BigInt & u, const ppuint d ) ;

        
        
        int ceilLg() ;

        
        
        
        friend bool operator==( const BigInt & u, const BigInt & v ) ;
        friend bool operator==( const BigInt & u, const ppuint d     ) ;
        friend bool operator!=( const BigInt & u, const BigInt & v ) ;
        friend bool operator!=( const BigInt & u, const ppuint d     ) ;
        friend bool operator> ( const BigInt & u, const BigInt & v ) ;
        friend bool operator> ( const BigInt & u, const ppuint d     ) ;
        friend bool operator< ( const BigInt & u, const BigInt & v ) ;
        friend bool operator< ( const BigInt & u, const ppuint d     ) ;
        friend bool operator>=( const BigInt & u, const BigInt & v ) ;
        friend bool operator>=( const BigInt & u, const ppuint d     ) ;
        friend bool operator<=( const BigInt & u, const BigInt & v ) ;
        friend bool operator<=( const BigInt & u, const ppuint d     ) ;

        
        friend BigInt operator& ( const BigInt & u, const BigInt & v ) ;

        friend BigInt operator<<( const BigInt & u, ppuint n ) ;

        const bool testBit( const int bitNum ) const ;
 
        

        
        
        string toString() const ;
    
        
        friend void divMod( const BigInt & u, const BigInt & v,
                            BigInt &       q, BigInt &       r ) ;

        friend void divMod( const BigInt & u, const ppuint d,
                            BigInt &       q, ppuint & r ) ;

        
        int maxBitNumber() const ;

        
        
        static const ppuint getBase() ;

        

        friend const ppuint getDigit( const BigInt & u, const int n ) ;

        friend const int getNumDigits( const BigInt & u ) ;

        friend void setBase( const BigInt & u, const ppuint base ) ;

        friend void printNumber( const BigInt & u, ostream & out ) ;
    
        friend void printNumber( const BigInt & u ) ;

    
    
    
    private:
        
        
        static ppuint & base_() ;

        
        static ppuint * pbase ;

        static int & numBitsPerDigit_() ;

    
    private:
		
        
        
        
        
    	
		
		
		
        
		
		
		
		
		
	   	
    	vector<ppuint> digit_ ;
} ;





BigInt power( ppuint p, ppuint n ) ;
    
    

    
const bool testBit( const ppuint n, const int bitNum ) ;

#endif 
