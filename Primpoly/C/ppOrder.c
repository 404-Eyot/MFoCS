#include <stdio.h>
#include <stdlib.h> /* for rand() function */

#include "Primpoly.h"


/*==============================================================================
|                                order_m                                       |
================================================================================

DESCRIPTION
                m
    Check that x  (mod f(x), p) is not an integer for m = r / p  but skip
                                           n                   i
                                          p  - 1
    this test if p  | (p-1).  Recall r = -------.
                  i                       p - 1

INPUT

     power_table (int **)     x ^ k (mod f(x), p) for n <= k <= 2n-2, f monic.
     n      (int, n >= 1)     Degree of f(x).
     p      (int)             Modulo p coefficient arithmetic.
     r (int)                  See above.
     primes (bigint *)        Distinct prime factors of r.
     prime_count              Number of primes.

RETURNS

     YES    if all tests are passed.
     NO     if any one test fails.

EXAMPLE
                                           2
     Let n = 4 and p = 5.  Then r = 156 = 2 * 3 * 13, and p = 2, p = 3,
                                                            1      2
     and p = 13.  m = { 156/2, 156/3, 156/13 } = { 78, 52, 12 }.  We can
          3
     skip the test for m = 78 because p = 2 divides p-1 = 4.  Exponentiation
                                       1
            52       3   2                                      12
     gives x    = 2 x + x + 4 x, which is not an integer and x =
        3       2
     4 x  +  2 x  +  4 x  + 3 which is not an integer either, so we return

     YES.

METHOD

    Exponentiate x with x_to_power and test the result with is_integer.
    Return right away if the result is not an integer.

BUGS

    None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int
    order_m( int power_table[][ MAXDEGPOLY ], int n, int p, bigint r,
             bigint * primes, int prime_count )
{

/*------------------------------------------------------------------------------
|                               Local Variables                                |
------------------------------------------------------------------------------*/

int
    i,                  /*  Loop counter.  */
    g[ MAXDEGPOLY ] ;   /* g(x) = x ^ m (mod f(x), p) */

bigint
    m ;                 /*  Exponent of m. */

/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

for (i = 0 ;  i <= prime_count ;  ++i)

    if (!skip_test( i, primes, p ))
    {
        m = r / primes[ i ] ;

        x_to_power( m, g, power_table, n, p ) ;

        #ifdef DEBUG_PP_PRIMPOLY
        printf( "    order m test for prime = %lld, x^ m = x ^ %lld = ", primes[i], m ) ;
        write_poly( g, n-1 ) ;
        printf( "\n\n" );
        #endif

        if (is_integer( g, n-1 ))

            return( NO ) ;
    }

return( YES ) ;

} /* ====================== end of function order_m ========================= */



/*==============================================================================
|                                order_r                                       |
================================================================================

DESCRIPTION
              r
    Check if x  (mod f(x), p) = a, where a is an integer.

INPUT

     power_table (int **)     x ^ k (mod f(x), p) for n <= k <= 2n-2, f monic.
     n      (int, n >= 1)     Degree of f(x).
     p      (int)             Modulo p coefficient arithmetic.
     r (int)                  See above.
     a (int *)                Pointer to value of a.

RETURNS

     YES    if the formula above is true.
     NO     if it isn't.

EXAMPLE
                          4    2
     Let r = 156, f(x) = x  + x  + 2 x + 3, n = 4 and p = 5.  It turns
               156                           4
     out that x    = 3 (mod f(x), 5) = (-1)  * 3, so we return YES.

METHOD
                          r
    First compute g(x) = x (mod f(x), p).
    Then test if g(x) is a constant polynomial,

BUGS

    None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int
    order_r( int power_table[][ MAXDEGPOLY ], int n, int p, bigint r, int * a )
{

/*------------------------------------------------------------------------------
|                               Local Variables                                |
------------------------------------------------------------------------------*/

int
    g[ MAXDEGPOLY ] ;   /* g(x) = x ^ m (mod f(x), p) */

/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

x_to_power( r, g, power_table, n, p ) ;
 
#ifdef DEBUG_PP_PRIMPOLY
printf( "    order r test for x^r = x ^ %lld = ", r ) ;
write_poly( g, n-1 ) ;
printf( "\n\n" );
#endif

/*  Return the value a = constant term of g(x) */
*a = g[ 0 ] ;

return( is_integer( g, n-1 ) ? YES : NO  ) ;

} /* ====================== end of function order_r ========================= */



/*==============================================================================
|                                  maximal_order                               |
================================================================================

DESCRIPTION
              k                                  n
    Check if x  = 1 (mod f(x), p) only when k = p  - 1 and not for any smaller
    power of k, i.e. that f(x) is a primitive polynomial.

INPUT

     f (int *)                Monic polynomial f(x).
     n      (int, n >= 1)     Degree of f(x).
     p      (int)             Modulo p coefficient arithmetic.

RETURNS

     YES    if f( x ) is primitive.
     NO     if it isn't.

EXAMPLE

               4
     f( x ) = x  + x  +  1 is a primitive polynomial modulo p = 2,
                                         4
     because it generates the group GF( 2  ) with the exception of
                            2         15
     zero.  The powers {x, x , ... , x  } modulo f(x), mod 2 are
                                        16       4
     distinct and not equal to 1 until x   (mod x  + x + 1, 2) = 1.

METHOD

    Confirm f(x) is primitive using the definition of primitive
    polynomial as a generator of the Galois group
         n                   n
    GF( p ) by testing that p - 1 is the smallest power for which
     k
    x = 1 (mod f(x), p).

BUGS

    None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int maximal_order( int * f, int n, int p )
{
    int g[ MAXDEGPOLY ] ;   /* g(x) = x ^ m (mod f(x), p) */
    bigint maxOrder ;
    bigint k ;
    int power_table[ MAXDEGPOLY - 1 ] [ MAXDEGPOLY ] ;    /*  x ^ n , ... , x ^ 2n-2 (mod f(x), p) */

    /*                         n         2n-2
        Precompute the powers x ,  ..., x     (mod f(x), p)
        for use in all later computations.
    */
    construct_power_table( power_table, f, n, p ) ;

    /*  Highest possible order for x. */
    maxOrder = power( p, n ) - 1 ;

    for (k = 1 ;  k <= maxOrder ;  ++k)
    {
        x_to_power( k, g, power_table, n, p ) ;

        if (is_integer( g, n-1 ) &&
            g[0] == 1 &&
            k < maxOrder)
        {
            return 0 ;
        }

    } /* end for k */

    return 1 ;

} /* ================= end of function maximal_order ======================== */
