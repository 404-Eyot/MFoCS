#include <stdio.h>
#include <stdlib.h> /* for rand() function */

#include "Primpoly.h"


/*==============================================================================
|                                    factor                                    |
================================================================================

DESCRIPTION

     Factor n into primes.  Record all the distinct prime factors and how
     many times each occurs.  Return the number of primes - 1.

INPUT
                   
     n      (int, n >= 1)     The integer to be factored.

OUTPUT

     primes (bigint *)   List of distinct prime factors.
     count  (int *)      List of how many times each factor occurred.

     When n = 1, t = 0, and primes[ 0 ] = count[ 0 ] = 1.

RETURNS

     t (int)             Number of prime factors - 1.  
	                     Prime factors and their multiplicites are in locations 0 to t.

EXAMPLE 
                   2
    For n = 156 = 2  * 3 * 13 we have

    k   primes[ k ]   count[ k ]
    ----------------------------
    0        2            2
    1        3            1
    2       13            1

METHOD

    Method described by D.E. Knuth, ART OF COMPUTER PROGRAMMING, vol. 2, 2nd ed.,
	                                                            -----
	in Algorithm A, pgs. 364-365.  The running time is O( max \/ p   , p  )
                                                                  t-1   t
    where p  is the largest prime divisor of n and p    is the next largest.
	       t                                        t-1

    (1)  First divide out all multiples of 2 and 3 and count them.  
	
	(2)  Next, divide n by all integers d >= 5 except multiples of 2 and 3. 

    (3)  Halt either when all prime factors have been divided out (leaving n = 1) 
	     or when the current value of n is prime.  The stopping test 

          (d > | n/d | AND r != 0)
               --   --
         detects when n is prime.

    In the example above, we divided out 2's and 3's first, leaving
    n = 13.  We continued with trial divisor d = 3.  But now the stopping
    test was activated because 5 > | 13/5 | = 2, and remainder = 3 (non-zero).
                                   --    --
    n = 13 itself is the last prime factor of 156.  We return t = 2.  There
    are 2 + 1 = 3 distinct primes.


    If we start with d = 5, and add 2 and 4 in succession, we will run through all
	the integers except multiples of 2 and 3.  To see this, observe the pattern:

    Integers:     1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17
	Mult. of 2:      x     x     x     x     x     x     x     x
	Mult. of 3:         x        x        x        x        x
	d:                        x     x           x     x          x
	Separation:                  2       4         2       4

    The sequence of divisors d, includes all the primes, so is safe to use for
	factorization testing.

    Theorem.  Suppose no divisor d in the sequence divides n.  Suppose at some point,
	q < d with r != 0.  Then n must be prime.

    Proof.  We begin with an integer n which has all powers of 2 and 3 removed.
    Assume, to the contrary, that n is composite:  n = p ... p
	      t                                             1     t
    n >= p    since p  is the smallest prime factor.
	      1          1

	      2                                      
    n >= p    since n is composite, so has at least 2 prime factors.
	      1
		        2
	  >= (d + 1)   since p  > d implies p >= (d + 1).  p > d because we assumed
                          1              1              1
      that no d in the sequence divided n, so d couldn't be any of the prime divisors
	  p
	   i
         2            2               2
	  = d + 2d + 1 = d + d + d + 1 > d + d
         2
      > d + n mod d   since n mod d < d
                                                                              2
      > | n / d | d + n mod d  because our test said q < d, so | n / d | d < d
	    --     --                                              --     --

      = n.  So we get the contradiction n > n.  Thus n must be prime. 
	                                               ---
    Note that this is sharper than the bound d > \/ n

    Theorem.  Conversely, if n is a prime, no d will divide it, so at some point, 
	d will be large enough that q = | n / d | < d.  r != 0 since n is prime.
	                                --     --

    Theorem.  The factoring algorithm works.
	
	Proof.  If n == 1 we exit immediately.  If n is composite, we divide out all powers
	of 2 and 3 (if any).  Since d runs through all possible prime divisors, we divide
	these out also.  Composite trial d causes no problem;  prime factors of d are divided
	out of n before we try to divide by d, so such a d cannot divide n.

    We end in one of two ways (1) All divisors of n have been divided out in which case n = 1.
	(2) n is a prime, so the stopping test is activiated and n != 1 is recorded as a prime
	divisor.


BUGS

    Can be slow when n is a prime.  We could do a probabilistic test for
    the primality of n at the statement, "n_is_prime = (r != 0 && q < d) ;"
    which would speed up the test.  
    
    Or we could implement the Pollard rho algorithm instead.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int 
factor( bigint n, bigint * primes, int * count )
{

/*------------------------------------------------------------------------------
|                               Local Variables                                |
------------------------------------------------------------------------------*/

int
    t = 0,              /*  Array index for primes and count.         */
    cnt = 0,            /*  Counter for factors of 2 and 3.           */
    new_d = YES,        /*  YES if current divisor is different from
                            the previous one.                         */
    n_is_prime = NO,    /*  YES if current value of n is prime.       */
    flag = YES ;        /*  Alternately YES and NO.                   */

bigint
    q,                  /*  The quotient, floor( n / d )              */
    r,                  /*  The remainder, n mod d                    */
    d = 5 ;             /*  Trial divisor and its first value.        */

/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

/*  Handle special cases for speed. 
    These are cases where r = (p^n - 1)/(p-1) is a large prime or has large prime 
	factors and p^n is less than the maximum size allowed by this program.
	(I don't have all these cases).
*/
if (n == 1)
{
    primes[ 0 ] = count[ 0 ] = 1 ;
    return( 0 ) ;
}
else if (n == (bigint)4611686018427387903ULL) /*  (2^62 - 1)/(2 - 1)  */
{
	primes[ 0 ] = 3 ;
	count [ 0 ] = 1 ;

	primes[ 1 ] = 715827883ULL ;
	count [ 1 ] = 1 ;

	primes[ 2 ] = 2147483647ULL ;
	count [ 2 ] = 1 ;

	return( 2 ) ;
}
else if (n == (bigint)2305843009213693951ULL) /*  (2^61 - 1)/(2 - 1)  */
{
	primes[ 0 ] = (bigint)2305843009213693951ULL ;
	count [ 0 ] = 1 ;

	return( 0 ) ;
}


while( n % 2 == 0 )       /*  Remove factors of 2. */
{
    n /= 2 ;
    ++cnt ;
}

if (cnt != 0)
{
    primes[ t ] = 2 ;
    count[ t++ ] = cnt ;
}

cnt = 0 ;  

while( n % 3 == 0 )       /*  Remove factors of 3. */
{
    n /= 3 ;
    ++cnt ;
}

if (cnt != 0)
{
    primes[ t ] = 3 ;
    count[ t++ ] = cnt ;
}


/*
    Factor the rest of n.  Continue until n = 1 (all factors divided out)
    or until n is prime (so n itself is the last prime factor).
*/

do {

	/*  Integer divide to get quotient and remainder:  n = qd + r */

    q = n / d ;

    r = n % d ; 

    n_is_prime = (r != 0 && q < d) ;   /* n is prime. */

    if (r == 0)    
    {
        n = q ;     /*  Divide d out of n. */

        if (new_d)  /*  New prime factor. */
        {
            primes[ t ] = d ;
            count[ t++ ] = 1 ;
            new_d = NO ;
         }
         else

             ++count[ t-1 ] ;   /*  Same divisor again.  Increment its count. */ 
    }
    else {

        d += (flag ? 2 : 4) ;  /* d did not divide n.  Try a new divisor. */

        flag = !flag ;
        new_d = YES ;
    }

} while ( ! n_is_prime && (n != 1)) ;

if (n == 1)       /*  All factors were divided out. */

    return( t - 1 ) ;

else {  /*  Current value of n is prime.  It is the last prime factor. */

    primes[ t ] = n ;
    count[ t ] = 1 ;
    return( t ) ;
}

} /* ========================= end of function factor ======================= */

/*
      4
Phi[ 5  - 1 ] / 4 

   4              4
  5  - 1 = 624 = 2  3  13

  Phi = 624 (1 - 1/2) (1 - 1/3)(1 - 1/13) = 192

  192 / 4 = 48


  Handle special cases.

  Warning: can take a long time due to factorization.
  */

bigint 
EulerPhi( bigint n )
{
	int prime_count = 0 ;
	int i = 0 ;
	bigint phi ;

	/* Factor n into primes. */
	bigint primes[ MAXNUMPRIMEFACTORS ] ;
	int    count [ MAXNUMPRIMEFACTORS ] ;

    /* Error return and special cases. */
	if (n <= 0)
		return 0 ;
	else if (n == 1)
		return 1 ;

	/* Factor n >= 2 into distinct primes. */
    prime_count = factor( n, primes, count ) ;

	/* Compute Euler phi[ n ] =   */
	/*                            */
	/*      num prime factors     */
	/*        -------             */
	/*     n   |  |   (1 - 1/p )  */
	/*         i = 1          i   */
    phi = n ;
	for (i = 0 ;  i <= prime_count ; ++i)
	{
        phi /=  primes[ i ] ;
		phi *= (primes[ i ] - 1) ;
	}

	return phi ;
}


/*==============================================================================
|                             is_probably_prime                                |
================================================================================

DESCRIPTION

     Test whether the number n is probably prime.  If n is composite
     we are correct always.  If n is prime, we will be wrong no more
     than about 1/4 of the time on average, probably less for
     any x and n.

INPUT
                   
     n      (int, n >= 0)    Number to test for primality.
     x      (int, 1 < x < n for n > 6) A random integer.

RETURNS


     YES    If n is probably prime.
     NO     If n is definitely not prime or n < 0,
            or x is out of range.

EXAMPLE 
 
METHOD

     Miller-Rabin probabilistic primality test, described by Knuth.
     If n = 1 + 2^k q is prime, and x^q mod n != 1 the sequence, 
     { x^q mod n, x^(2q) mod n, ..., x^(2^k q) mod n} 
     will end with 1 and the element in the sequence just before 
     1 first appears = n-1, since y^2 = 1 (mod p) satisfies 
     y = +1 or y = -1 only. The probability the algorithm fails is 
     bounded above by about 1/4 for all n.

BUGS

    None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int is_probably_prime( int n, int x )
{

/*------------------------------------------------------------------------------
|                               Local Variables                                |
------------------------------------------------------------------------------*/

int 
    j,
    k = 0,
	nm1,
	q = 0,
	y = 0 ;

/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

/*  Not prime if input is out of range. */
if (n < 0)
    return NO ;

/*  Handle special cases. */
if (n == 0 || n == 1 || n == 4)
    return NO ;

if (n == 2 || n == 3 || n == 5)
    return YES ;

/* Even numbers aren't prime. */
if (n % 2 == 0)
    return NO ;

/* Return not prime if x is out of range. */
if (x <= 1 || x >= n)
    return NO ;

/* Factor out powers of 2 to get n = 1 + 2^k q, q odd. */
nm1 = n - 1 ;
k = 0 ;
while( nm1 % 2 == 0)
{
	nm1 /= 2 ;
	++k ;
}

q = nm1 ;



/*  y = x^q (mod n) */
y = power_mod( x, q, n ) ;

for (j = 0 ;  j < k ;  ++j)
{
    if ( (j == 0 && y == 1) || (y == n-1))
		return YES ;
	else if (j > 0 && y == 1)
		return NO ;

	/* Compute y^2 (mod n) */
	y = power_mod( y, 2, n ) ;
}

/*  Shouldn't get here, but do it anyway for safety. */
return NO ;

} /* =============== end of function is_probably_prime ===================== */



/*==============================================================================
|                           is_almost_surely_prime                             |
================================================================================

DESCRIPTION

     Test whether the number n is almost surely prime.  If n is
     composite, we always return NO.  If n is prime, the 
     probability of returning YES successfully is

                 NUM_PRIME_TEST_TRIALS
        1 - (1/4)

INPUT
                   
     n      (int, n >= 0)

RETURNS


     YES    If n is probably prime with a very high degree of
            probability.
     NO     If n is definitely not prime.
	 -1     If inputs are out of range.

EXAMPLE
  
METHOD

    Use the Miller-Rabin probabilitic primality test for lots of
    random integers x.  If the test shows n is composite for any 
    given x, is is non-prime for sure.  
    
    If it passes the the primality test for several random values
    of x, the probability is it prime is approximately
    
         NUM_PRIME_TEST_TRIALS
    1 - 2

    assuming the random number generator is really random.

BUGS

    The system pseudorandom number generator needs to be a good one
    (i.e. passes the spectral test and other statistical tests).

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int is_almost_surely_prime( int n )
{

/*------------------------------------------------------------------------------
|                               Local Variables                                |
------------------------------------------------------------------------------*/

int 
    trial = 0,
    x     = 3 ;

/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

/* Seed the random-number generator. */
srand( 314159 );
   
for (trial = 1 ;  trial <= NUM_PRIME_TEST_TRIALS ;  ++trial)
{
    /*  Generate a new random integer such that 1 < x < n. */
    x = rand() % n ;
    if (x <= 1) x = 3 ;

	/* Definitely not prime. */
	if (is_probably_prime( n, x ) == NO)
		return NO ;
}

/* Almost surely prime because it passed the probably prime test
 * above many times.
 */
return YES ;

} /* ============== end of function is_almost_surely_prime ================ */
