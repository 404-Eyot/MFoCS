#include "Primpoly.h"

/*==============================================================================
|                                  mod                                         |
================================================================================

DESCRIPTION

     A mod function for both positive and negative arguments.

INPUT

    n (int, -infinity < n < infinity)
    p (int, p > 0)

RETURNS

    n mod p (0 <= n mod p < p)

EXAMPLE

    The C language gives -5 % 3 = - ( -(-5) mod 3 ) = -( 5 mod 3 ) = -2.
    The result is nonzero, so we add p=3 to give 1.

    C computes -3 % 3 = - ( -(-3) mod 3  ) = 0, which we leave unchanged.

METHOD

     For n >= 0, the C language defines r = n mod p by the equation

         n = kp + r    0 <= r < p

     but when n < 0, C returns the quantity

         r = - ( (-n) mod p )

     To put the result into the correct range 0 to p-1, add p to r if
     r is non-zero.

     By the way, FORTRAN's MOD function does the same thing.

BUGS

     None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int
    mod( int n, int p )
{

/*------------------------------------------------------------------------------
|                               Local Variables                                |
------------------------------------------------------------------------------*/

register int
    raw_mod ;      /*  The value n % p computed by C's mod function.  */

/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

raw_mod = n % p ;

if (raw_mod == 0)

    return( 0 ) ;

else if (n >= 0)             /*  mod is not 0.  n >= 0. */

    return( raw_mod ) ;

else

    return( raw_mod + p ) ;  /* mod is not 0.  n < 0. */

} /* =========================== end of function mod ======================== */



/*==============================================================================
|                                       power                                  |
================================================================================

DESCRIPTION

     Raise a positive integer to a positive power exactly.

INPUT

     x (int, x > 0)   The base.
     y (int, y > 0)   The exponent.

RETURNS

     y
    x

EXAMPLE CALLING SEQUENCE

     power( 2, 3 ) => 8

METHOD

     Multiply x by itself y times.

BUGS

     None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

bigint power( int x, int y )
{

/*------------------------------------------------------------------------------
|                               Local Variables                                |
------------------------------------------------------------------------------*/

bigint
    pow ;   /*  x to the y.  */

/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

for (pow = 1 ;  y > 0 ;  --y)

    pow *= x ;

return( pow ) ;

} /* ========================= end of function power ======================== */



/*==============================================================================
|                                power_mod                                     |
================================================================================

DESCRIPTION

     Raise a positive integer to a positive power modulo an integer.

INPUT

     a (int, a >= 0)   The base.
     n (int, n >= 0)   The exponent.
     p (int, p >= 2)   The modulus.

RETURNS

      n
     a  (modulo p)
    -1 if a < 0, n < 0, or p <= 1, or 0^0 case.

EXAMPLE CALLING SEQUENCE

     power_mod( 2, 3, 7 ) => 1

METHOD

     Multiplication by repeated squaring.

BUGS

     None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int power_mod( int a, int n, int p )
{

/*------------------------------------------------------------------------------
|                               Local Variables                                |
------------------------------------------------------------------------------*/

/*
    mask = 1000 ... 000.  That is, all bits of mask are zero except for the
    most significant bit of the computer word holding its value.  Signed or
	unsigned type for bigint shouldn't matter here, since we just mask and
	compare bits.
*/

int
    mask = (1 << (8 * sizeof( int ) - 1)),
    bit_count = 0 ;  /* Number of bits in exponent to the right of the leading bit. */

/*  Use extra precision since we need to square an integer within the
    calculations below. */
long int product = (int) a ;


/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

/*
    Out of range conditions.
*/

if (a < 0 || n < 0 || p <= 1 || (a == 0 && n == 0))

    return -1 ;

/*  Quick return for 0 ^ n, a^0 and a^1.  */
if (a == 0)
    return 0 ;

if (n == 0)
    return 1 ;

if (n == 1)
    return a % p ;

/*
    Advance the leading bit of the exponent up to the word's left
    hand boundary.  Count how many bits were to the right of the
    leading bit.
*/

while (! (n & mask))
{
    n <<= 1 ;
    ++bit_count ;
}

bit_count = (8 * sizeof( int )) - bit_count ;


/*
    Exponentiation by repeated squaring.  Discard the leading 1 bit.
    Thereafter, square for every 0 bit;  square and multiply by x for
    every 1 bit.
*/

while ( --bit_count > 0 )
{
    /*  Expose the next bit. */
    n <<= 1 ;

	/* Square modulo p. */
	product = (product * product) % p ;

	/*  Leading bit is 1: multiply by a modulo p. */
    if (n & mask)
        product = (a * product) % p ;
}

/*  We don't lose precision because product is always reduced modulo p. */
return (int) product ;

} /* ========================= end of function power_mod ==================== */



/*==============================================================================
|                             is_primitive_root                                |
================================================================================

DESCRIPTION

     Test whether the number a is a primitive root of the prime p.

INPUT

     a      (int, a >= 1)     The number to be tested.
     p      (int, p >= 2)     A prime number.

RETURNS

     YES    If a is a primitive root of p.
     NO     If a isn't a primitive root of p or if inputs are out of range.

EXAMPLE
                                                     1      2      3
     3 is a primitive root of the prime p = 7 since 3 = 3, 3 = 2, 3 = 6,
      4      5                                       p-1    6
     3 = 4, 3 = 5 (mod 7); all not equal to 1 until 3    = 3 = 1 (mod 7).

     So 3 is a primitive root of 7 because it has maximal period.  a = 2
                                                      3
	 isn't a primitive root of p = 7 because already 2 = 1 (mod 7).

METHOD

    From number theory, a is a primitive root of the prime p iff
     (p-1)/q
    a        != 1 (mod p) for all prime divisors q of (p-1).
                            (p-1)
    Don't need to check if a     = 1 (mod p):  Fermat's little
    theorem guarantees it.

BUGS

    We don't check if p is not a prime.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int is_primitive_root( int a, int p )
{

/*------------------------------------------------------------------------------
|                               Local Variables                                |
------------------------------------------------------------------------------*/

bigint
    primes[ MAXNUMPRIMEFACTORS ] ; /* The distinct prime factors of p-1. */

int
    count[ MAXNUMPRIMEFACTORS ],   /* ... and their multiplicities.      */
    t,                             /*  Primes are indexed from 0 to t.   */
	i ;                            /*  Loop index.                       */

/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

/*  Error for out of range inputs, including p
    being an even number greater than 2.
*/
if (p < 2 || a < 1 || (p > 2 && (p % 2 == 0)))
    return NO ;

/*  Special cases:
    1 is the only primitive root of 2;  i.e. 1 generates the unit elements
	of GF( 2 );  2 is the only primitive root of 3, and 2 and 3 are the only
	primitive roots of 5, etc.  See http://mathworld.wolfram.com/PrimitiveRoot.html
*/
if ( (p == 2  && a == 1) ||
     (p == 3  && a == 2) ||
     (p == 5  && (a == 2 || a == 3)) ||
     (p == 7  && (a == 3 || a == 5)) ||
     (p == 11 && (a == 2 || a == 6 || a == 7 || a ==  8)) ||
     (p == 13 && (a == 2 || a == 6 || a == 7 || a == 11)) )
    return YES ;


/*  Reduce a down modulo p. */
a = a % p ;


/*  a = 0 (mod p);  Zero can't be a primitive root of p. */
if (a == 0)
    return NO ;

/*  Factor p-1 into primes. */
t = factor( (bigint)(p - 1), primes, count ) ;


/*  Check if a^(p-1)/q <> 1 (mod p) for all primes q | (p-1).
    If so, we have a primitive root of p, otherwise, we exit early.
 */
for (i = 0 ;  i <= t ;  ++i)
{
	/*  The prime factors can be safely cast from bigint to int
	    because p-1 needs only integer precision.  */
    if (power_mod( a, (p-1) / (int)primes[ i ], p ) == 1)
		return NO ;
}

return YES ;

} /* =============== end of function is_primitive_root ===================== */


/* Divide modulo p.

Do extended Euclid's algorithm on u and v to find u1, u2, and u3 such that

    u u1 + v u2 = u3 = gcd( u, v ).

Now let v = p = prime number, so gcd = u3 = 1.  Then we get

  u u1 + p ? = 1

or u u1 = 0 (mod p) which makes u (mod p) the unique multiplicative
inverse of u.


Assume 0 <= u < p, p is prime >= 2.

*/
int inverse_mod_p( int u, int p )
{
    /*  Do Euclid's algorithm to find the quantities */
	int t1 = 0 ;
	int t3 = 0 ;
	int q  = 0 ;

    int u1 = 1 ;
    int u3 = u ;
    int v1 = 0 ;
    int v3 = p ;

	int inv_v = 0 ;

	while( v3 != 0)
	{
        q = (int)(u3 / v3) ;

		t1 = u1 - v1 * q ;
		t3 = u3 - v3 * q ;

		u1 = v1 ;
		u3 = v3 ;

		v1 = t1 ;
		v3 = t3 ;
	}

    inv_v = mod( u1, p ) ;

	/* Self check. */
	if ( mod( u * inv_v, p ) != 1)
	{
		return 0 ;
	}

	return inv_v ;
}
