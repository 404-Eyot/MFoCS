#include <stdio.h>
#include "Primpoly.h"


/*==============================================================================
|                                   eval_poly                                  |
================================================================================

DESCRIPTION

     Evaluate the polynomial f( x ) with modulo p arithmetic.

INPUT

     f (int *)      nth degree mod p polynomial
         
                              n         n-1     
                    f( x ) = x  +  a   x  + ... + a    0 <= a  < p
                                    n-1            0         i          
     x (int)        0 <= x < p

     n (int)        n >= 1

     p (int)         

RETURNS

     f( x )  (int)

EXAMPLE 
                                  4
     Let n = 4, p = 5 and f(x) = x  + 3x + 3.  Then f(x) = 
     (((x + 0)x + 0)x + 3)x + 3.  f(2) = (((2 + 0)2 + 0)2 + 3) =
     (8 + 3)2 + 3 = 1 + 2 + 3 (mod 5) = 0.

METHOD

     By Horner's rule, f(x) = ( ... ( (x + a   )x + ... )x + a .
                                            n-1               0
     We evaluate recursively, f := f * x + a (mod p), starting
                                            i 
     with f = 1 and i = n-1.

BUGS

     None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int
    eval_poly( int * f, int x, int n, int p )
{

/*------------------------------------------------------------------------------
|                               Local Variables                                |
------------------------------------------------------------------------------*/

int
    val = 1 ;   /*  The value of f(x) which is returned. */

/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

while ( --n >= 0 )

    val = mod( val * x + f[ n ], p ) ;

return( val ) ;


} /* ========================= end of function eval_poly ==================== */


/*==============================================================================
|                                     linear_factor                            |
================================================================================

DESCRIPTION

     Check if the polynomial f(x) has any linear factors.

INPUT

    f (int *)  nth degree monic mod p polynomial f(x). 

    n (int)    Its degree.

    p (int)    Modulus for coefficient arithmetic.

RETURNS

    YES    if f( a ) = 0 (mod p) for a = 1, 2, ... p-1.
    NO     otherwise

    i.e. check if f(x) contains a linear factor (x - a).  We don't need to test
    for the root a = 0 because f(x) was chosen in main to have a non-zero 
    constant term, hence no zero root.

EXAMPLE 
                                 4      
    Let n = 4, p = 5 and f(x) = x  + 3x + 3.  Then f(1) = 2 (mod 5), but 
    f(2) = 0 (mod 5), so we exit immediately with a yes.

METHOD

    Evaluate f(x) at x = 1, ..., p-1 by Horner's rule.  Return instantly the 
    moment f(x) evaluates to 0.

BUGS

    None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int 
    linear_factor( int * f, int n, int p )
{

/*------------------------------------------------------------------------------
|                               Local Variables                                |
------------------------------------------------------------------------------*/

int i ;  /*  Loop counter. */

/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

for (i = 1 ;  i <= p-1 ;  ++i)

    if (eval_poly( f, i, n, p ) == 0)

        return( YES ) ;

return( NO ) ;

} /* ====================== end of function linear_factor =================== */


/*==============================================================================
|                                 is_integer                                   |
================================================================================

DESCRIPTION

     Test if a polynomial is a constant.

INPUT

    t (int *)  mod p polynomial t(x). 
    n (int)    Its degree.

RETURNS

    YES    if t(x) is a constant polynomial.
    NO     otherwise

EXAMPLE 
                               2
    Let n = 3.  Then t(x) = 2 x  is not constant but t(x) = 2 is.

METHOD

    A constant polynomial is zero in its first through n th degree
    terms.  Return immediately with NO if any such term is non-zero.

BUGS

    None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int 
    is_integer( int * t, int n )
{

/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

while ( n >= 1 )

    if (t[ n-- ] != 0) 

        return( NO ) ;

return( YES ) ;

} /* ====================== end of function is_integer ====================== */


/*==============================================================================
|                               construct_power_table                          |
================================================================================

DESCRIPTION

     Construct a table of powers of x:

      n                     2n-2
     x  (mod f(x), p)  ... x    (mod f(x), p)

INPUT

    f (int *)   Coefficients of f(x), a monic polynomial of degree n.
    n (int, -infinity < n < infinity)
    p (int, p > 0)

RETURNS

    power_table (int *)  power_table[i][j] is the coefficient of 
     j       n+i
    x   in  x   (mod f(x), p) where 0 <= i <= n-2 and 0 <= j <= n-1. 

EXAMPLE 
                                  4     2                     4
     Let n = 4, p = 5 and f(x) = x  +  x  +  2x  +  3.  Then x  = 

         2                  2
     -( x  +  2x  + 3) = 4 x  + 3 x + 2 (mod f(x), 5), and we get

      4                    2
     x  (mod f(x), 5) = 4 x  + 3 x + 2 = power_table[0].

      5                       2                 3      2
     x  (mod f(x), 5) = x( 4 x  + 3 x + 2) = 4 x  + 3 x  + 2x 
                      = power_table[1].

      6                       3      2           4      3      2
     x  (mod f(x), 5) = x( 4 x  + 3 x + 2 x) = 4x  + 3 x  + 2 x 

                              2                 3      2
                      = 4 ( 4x  + 3 x + 2) + 3 x  + 2 x  =

                           3     2
                      = 3 x + 3 x + 2 x + 3 = power_table[2].

                                    j
     power_table[i][j]:       | 0  1  2  3
                           ---+-------------
                            0 | 2  3  4  0 
                        i   1 | 0  2  3  4 
                            2 | 3  2  3  3 

METHOD
                              n-1
     Beginning with t( x ) = x,    compute the next power of x from the last
                                                         n
     one by multiplying by x.  If necessary, substitute x  =
             n-1
     -( a   x    + ... + a ) to reduce the degree.  This formula comes from
         n-1              0
                               n         n-1
     the observation f( x ) = x   + a   x    + ... + a    = 0 (mod f(x), p).
                                     n-1              0
BUGS

     None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

void 
    construct_power_table( int power_table[][ MAXDEGPOLY ], int * f, 
                           int n, int p )
{

/*------------------------------------------------------------------------------
|                               Local Variables                                |
------------------------------------------------------------------------------*/

int 
    i, j,                  /*  Loop counters.  */
    coeff,                 /*  Coefficient of x ^ n in t(x) */
    t[ MAXDEGPOLY + 1 ] ;  /*  t(x) is temporary storage for x ^ k (mod f(x),p)
                               n <= k <= 2n-2.  Its degree can go as high as
                               n before it is reduced again. */

/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

/*
                         n-1
    Initialize t( x ) = x 
*/

for (i = 0 ;  i <= n-2 ;  ++i)

    t[ i ] = 0 ;

t[ n-1 ] = 1 ;



/*
                                        i+n
    Fill the ith row of the table with x   (mod f(x), p).
*/

for (i = 0 ;  i <= n-2 ;  ++i)
{

    /*  Compute t(x) = x t(x)  */
    for (j = n ;  j >= 1 ;  --j)

        t[ j ] = t[ j-1 ] ;

    t[ 0 ] = 0 ;


	/*  Coefficient of the nth degree term of t(x).  */
    if ( (coeff = t[ n ]) != 0)
    {
        t[ n ] = 0 ;               /*  Zero out the x ^ n th term. */

       /*
                 n       n                        n-1
        Replace x  with x  (mod f(x), p) = -(a   x   + ... + a )
		                                      n-1             0
       */

        for (j = 0 ;  j <= n-1 ;  ++j)

            t[ j ] = mod( t[ j ] + 
                          mod( -coeff * f[ j ], p ),
                          p ) ;
    }  /* end if */


    /*  Copy t(x) into row i of power_table. */
    for (j = 0 ;  j <= n-1 ; ++j)

        power_table[i][j] = t[ j ] ;

} /* end for */

return ;

} /* ================== end of function construct_power_table =============== */



/*==============================================================================
|                                 autoconvolve                                  |
================================================================================

DESCRIPTION

     Compute a convolution-like sum for use in function coeff_of_square.

INPUT
                                             n-1
    t (int *)   Coefficients of t(x) = t    x    + ... + t x  + t
                                        n-1               1      0
    k (int), 0 <= k <= 2n - 2)

    lower (int, 0 <= lower <= n-1) 

    uppper (int, 0 <= upper <= n-1) 

    p (int, p > 0)

RETURNS

     upper
     ---
     \    t  t       But define the sum to be 0 when lower > upper to catch
     /     i  k-i    the special cases that come up in function coeff_of_square.
     ---
     i=lower

EXAMPLE 
                       3      2      
     Suppose t(x) = 4 x  +  x  +  3 x  +  3, lower = 1, upper = 3, n = 3,

     and p = 5.  For k = 3, auto_convolve = t[ 1 ] t[ 2 ] + t[ 2 ] t[ 1 ] +

     t[ 3 ] t[ 0 ] = 3 * 1 + 1 * 3 + 4 * 3 = 18 mod 5 = 3.  For lower = 0,

     upper = -1, or for lower = 3 and upper = 2, auto_convolve = 0, no matter what

     k is.

METHOD

    A "for" loop in the C language is not executed when its lower limit 

    exceeds its upper limit, leaving sum = 0.

BUGS

     None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int
    auto_convolve( int * t, int k, int lower, int upper, int p )
{

/*------------------------------------------------------------------------------
|                               Local Variables                                |
------------------------------------------------------------------------------*/

int 
    sum = 0,      /* Convolution sum. */
    i ;           /* Loop counter.    */

/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

for (i = lower ;  i <= upper ;  ++i)

   sum = mod( sum + mod( t[ i ] * t[ k - i ], p ), p ) ;

return( sum ) ;

} /* ==================== end of function auto_convolve ===================== */


/*==============================================================================
|                                 convolve                                     |
================================================================================

DESCRIPTION

     Compute a convolution-like sum.

INPUT
                                             n-1
    s (int *)   Coefficients of s(x) = s    x    + ... + s x  + s
                                        n-1               1      0  
										     n-1
    t (int *)   Coefficients of t(x) = t    x    + ... + t x  + t
                                        n-1               1      0

    k (int), 0 <= k <= 2n - 2)

    lower (int, 0 <= lower <= n-1) 

    uppper (int, 0 <= upper <= n-1) 

    p (int, p > 0)

RETURNS

     upper
     ---
     \    s  t       But define the sum to be 0 when lower > upper to catch
     /     i  k-i    the special cases
     ---
     i=lower

EXAMPLE 
                       3      2      
     Suppose s(x) = 4 x  +  x  +  3 x  +  3, 

	                    3      2      
     Suppose t(x) = 4 x  +  x  +  3 x  +  3, 

	   
	 lower = 1, upper = 3, n = 3,

     and p = 5.  For k = 3, convolve = t[ 1 ] t[ 2 ] + t[ 2 ] t[ 1 ] +

     t[ 3 ] t[ 0 ] = 3 * 1 + 1 * 3 + 4 * 3 = 18 mod 5 = 3.  For lower = 0,

     upper = -1, or for lower = 3 and upper = 2, convolve = 0, no matter what

     k is.

METHOD

    A "for" loop in the C language is not executed when its lower limit 

    exceeds its upper limit, leaving sum = 0.

BUGS

     None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int
    convolve( int * s, int * t, int k, int lower, int upper, int p )
{

/*------------------------------------------------------------------------------
|                               Local Variables                                |
------------------------------------------------------------------------------*/

int 
    sum = 0,      /* Convolution sum. */
    i ;           /* Loop counter.    */

/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

for (i = lower ;  i <= upper ;  ++i)

   sum = mod( sum + mod( s[ i ] * t[ k - i ], p ), p ) ;

return( sum ) ;

} /* ======================= end of function convolve ======================= */


/*==============================================================================
|                               coeff_of_square                                |
================================================================================

DESCRIPTION
                                    th                2
     Return the coefficient of the k   power of x in t( x )  modulo p.
 
INPUT

    t (int *)  Coefficients of t(x), of degree <= n-1.

    k (int, 0 <= k <= 2n-2)

    n (int, -infinity < n < infinity)

    p (int, p > 0)

RETURNS
                        th                2
    coefficient of the k   power of x in t(x) mod p.

EXAMPLE 
                                    3     2           2
    Let n = 4, p = 5, and t(x) = 4 x  +  x  + 4.  t(x) = 

             0                   1                         2
    (4 * 4) x  +  (2 * 1 * 4) x  +  (2 * 1 * 4 + 1 * 1) x  +

                             3                        4
    (2 * 1 * 1 + 2 * 4 * 4) x  + (2 * 4 * 1 + 1 * 1) x  +

                 5            6     
    (2 * 4 * 1) x  + (4 * 4) x  =  

     6      5     4     3     2
    x  + 3 x + 4 x + 4 x + 4 x  + 3 x + 1, all modulo 5.

            k        |  0  1  2  3  4  5  6
     ----------------+---------------------
     coeff_of_square |  1  3  4  4  4  3  1

METHOD
                                                         2
    The formulas were gotten by writing out the product t (x) explicitly.

    The sum is 0 in two cases:  

	    (1) when k = 0 and the limits of summation are 0 to -1 

        (2) k = 2n - 2, when the limits of summation are n to n-1.

    To derive the formulas, let 

                      n-1           
    Let t (x) = t    x     +  ... + t x + t
                 n-1                 1     0

    Look at the formulas in coeff_of_product for each power of x,
	replacing s with t, and observe that half of the terms are
	duplicates, so we can save computation time.

    Inspection yields the formulas,


    for 0 <= k <= n-1, even k,

     k/2-1
      ---             2
   2  \   t  t     + t
	  /    i  k-i     k/2
 	  ---
	  i=0

    for 0 <= k <= n-1, odd k,

      (k-1)/2
      ---  
    2 \   t  t    
	  /    i  k-i
	  ---
	  i=0


    and for n <= k <= 2n-2, even k,

      n-1
      ---            2
   2  \   t  t    + t
	  /    i  k-i    k/2
	  ---
	  i=k/2+1

      and for n <= k <= 2n-2, odd k,

      n-1
      ---  
   2  \   t  t    
	  /    i  k-i   
	  ---
	  i=(k+1)/2



BUGS

     None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int 
    coeff_of_square( int * t, int k, int n, int p )
{

/*------------------------------------------------------------------------------
|                               Local Variables                                |
------------------------------------------------------------------------------*/

int 
    sum = 0 ;      /* kth coefficient of t(x) ^ 2. */

/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

if (0 <= k && k <= n-1)
{

    if (k % 2 == 0)        /* Even k */

        sum = mod( mod( 2 * auto_convolve( t, k, 0, k/2 - 1, p ), p) +
                   t[ k/2 ] * t[ k/2 ], p ) ;

     else                  /* Odd k */

         sum = mod( 2 * auto_convolve( t, k, 0, (k-1)/2, p ), p) ;
}
else if (n <= k && k <= 2 * n - 2)
{

    if (k % 2 == 0)        /* Even k */

        sum = mod( mod( 2 * auto_convolve( t, k, k/2 + 1, n-1, p ), p) +
                   t[ k/2 ] * t[ k/2 ], p ) ;

     else                  /* Odd k */

         sum = mod( 2 * auto_convolve( t, k, (k+1)/2, n-1, p ), p) ;
}

return( sum ) ;

} /* ================== end of function coeff_of_square ===================== */


/*==============================================================================
|                               coeff_of_product                               |
================================================================================

DESCRIPTION
                                    th
     Return the coefficient of the k   power of x in s( x ) t( x )  modulo p.
 
INPUT

    t (int *)  Coefficients of t(x), of degree <= n-1.

    k (int, 0 <= k <= 2n-2)

    n (int, -infinity < n < infinity)

    p (int, p > 0)

RETURNS
                        th
    coefficient of the k   power of x in s(x) t(x) mod p.

EXAMPLE  
                              3     2                  2
  Let n = 4, p = 5, t(x) = 4 x  +  x  + 4, s( x ) = 3 x  + x + 2

  We'll do the case k=3,

  t3 s0 + t2 s1 + t1 s2 + t0 s3 = 4 * 2 + 1 * 1 + 0 * 3 + 4 * 0 = 9 = 4 (mod 5).


            k         |  0  1  2  3  4  5  6
     -----------------+---------------------
     coeff_of_product |  3  4  4  4  2  2  0

METHOD
                                                         
    The formulas were gotten by writing out the product s(x) t (x) explicitly.

    The sum is 0 in two cases:  

	    (1) when k = 0 and the limits of summation are 0 to -1 

        (2) k = 2n - 2, when the limits of summation are n to n-1.


    To derive the formulas, let 

                      n-1           
    Let s (x) = s    x     +  ... + s x + s  and
                 n-1                 1     0

                      n-1           
        t (x) = t    x     +  ... + t x + t 
                 n-1                 1     0

    and multiply out the terms, collecting like powers of x:


    Power of x     Coefficient
	==========================
     2n-2
    x              s    t
                    n-1  n-1

     2n-3
    x              s    t    +  s    t
                    n-2  n-1     n-1  n-2     

     2n-4
    x              s    t    +  s    t    +  s    t
                    n-3  n-1     n-2  n-2     n-3  n-1

     2n-5
    x              s    t    +  s    t    +  s    t    +  s    t
                    n-4  n-1     n-3  n-2     n-2  n-3     n-1  n-4

     . . .

     n
    x              s  t    +  s  t    + ...  +  s    t
                    1  n-1     2  n-2            n-1  1

     n-1
    x              s  t    +  s  t    + ...  +  s    t
                    0  n-1     1  n-2            n-1  0

    . . .

     3
    x              s  t  +  s  t  +  s  t  +  s  t
                    0  3     1  2     2  1     3  0

     2
    x              s  t  +  s  t  +  s  t 
                    0  2     1  1     2  0

     
    x              s  t  +  s  t 
                    0  1     1  0 

    1              s  t
                    0  0


    Inspection yields the formulas,


    for 0 <= k <= n-1,

      k
     ---
     \   s  t
	 /    i  k-i
	 ---
	 i=0


    and for n <= k <= 2n-2,

     n-1
     ---
     \   s  t
	 /    i  k-i
	 ---
	 i=k-n+1

BUGS

     None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int 
    coeff_of_product( int * s, int * t, int k, int n, int p )
{

/*------------------------------------------------------------------------------
|                               Local Variables                                |
------------------------------------------------------------------------------*/

int 
    sum = 0 ;      /* kth coefficient of t(x) ^ 2. */

/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

if (0 <= k && k <= n-1)
{
    sum = convolve( s, t, k, 0, k, p ) ;
}
else if (n <= k && k <= 2 * n - 2)
{
    sum = convolve( s, t, k, k - n + 1, n - 1, p ) ;
}

return( sum ) ;

} /* ================== end of function coeff_of_product ==================== */


/*==============================================================================
|                                  square                                      |
================================================================================

DESCRIPTION
              2
     Compute t ( x ) (mod f(x), p)

     Uses a precomputed table of powers of x.

INPUT

    t (int *)              Coefficients of t (x), of degree <= n-1.

    power_table (int **)   x ^ k (mod f(x), p) for n <= k <= 2n-2, f monic.

    n (int, -infinity < n < infinity)  Degree of f(x).

    p (int, p > 0)         Mod p coefficient arithmetic.

OUTPUT
                                             2
    t (int *)              Overwritten with t (x) (mod f(x), p)

EXAMPLE 
                                    3     2        
    Let n = 4, p = 5, and t(x) = 4 x  +  x  + 4.  Let f(x) = 

     4    2                   2       6      5      4      3     2
    x  + x  + 2 x + 3.  Then t (x) = x  + 3 x  + 4 x  + 4 x + 4 x + 3 x

           2                            3     2
    + 1.  t (x) (mod f(x), p) = 1 * (3 x + 3 x + 2 x + 3) +

            3      2                 2                3     2
    3 * (4 x  + 3 x + 2 x) + 4 * (4 x + 3 x + 2) + 4 x + 4 x + 3 x + 1

         3     2
    = 4 x + 2 x + 3 x + 2.


METHOD
         2            2n-2              n         n-1
    Let t (x) = t    x     +  ... + t  x  +  t   x   +  ... + t .
                 2n-2                n        n-1              0

    Compute the coefficients t  using the function coeff_of_square.
                              k

                                2
    The next step is to reduce t (x) modulo f(x).  To do so, replace

                           k                                      k
    each non-zero term t  x,  n <= k <= 2n-2, by the term t * [ x   mod f(x), p)]
                        k                                  k

    which we get from the array power_table.

BUGS

     None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

void 
    square( int * t, int power_table[][ MAXDEGPOLY ], int n, int p )
{

/*------------------------------------------------------------------------------
|                               Local Variables                                |
------------------------------------------------------------------------------*/

int 
    i, j,                     /* Loop counters. */
    coeff,                    /* Coefficient of x ^ k term of t(x) ^2 */
    temp[ MAXDEGPOLY + 1 ] ;  /* Temporary storage for the new t(x). */

/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

/*
                                 0        n-1
    Compute the coefficients of x , ..., x.   These terms do not require

    reduction mod f(x) because their degree is less than n.
*/

for (i = 0 ;  i <= n ;  ++i)

    temp[ i ] = coeff_of_square( t, i, n, p ) ;

/*
                                 n        2n-2             k
    Compute the coefficients of x , ..., x.    Replace t  x  with
            k                                            k
    t  * [ x  (mod f(x), p) ] from array power_table when t is 
     k                                                     k
    non-zero.

*/

for (i = n ;  i <= 2 * n - 2 ;  ++i)

    if ( (coeff = coeff_of_square( t, i, n, p )) != 0 )

        for (j = 0 ;  j <= n - 1 ;  ++j)

            temp[ j ] = mod( temp[ j ] +
                             mod( coeff * power_table[ i - n ] [ j ], p ),
                             p ) ;

for (i = 0 ;  i <= n - 1 ;  ++i)

    t[ i ] = temp[ i ] ;

} /* ======================== end of function square ======================== */



/*==============================================================================
|                                  product                                     |
================================================================================

DESCRIPTION

     Compute s( x ) t( x ) (mod f(x), p)

     Uses a precomputed table of powers of x.

INPUT

    s (int *)              Coefficients of s(x), of degree <= n-1.

    t (int *)              Coefficients of t(x), of degree <= n-1.

    power_table (int **)    x ^ k (mod f(x), p) for n <= k <= 2n-2, f monic.

    n (int, -infinity < n < infinity)  Degree of f(x).

    p (int, p > 0)         Mod p coefficient arithmetic.

OUTPUT

    s (int *)              Overwritten with s( x ) t( x ) (mod f(x), p)

EXAMPLE 
                                     3    2                 2
    Let n = 4 and p = 5, t( x ) = 4 x  + x + 4, s( x ) = 3 x + x + 2

                            5      4     3     2
	Then s( x ) t( x ) = 2 x  + 2 x + 4 x + 4 x + 4 x + 3, modulo 5, 

                                         4   2    
    and after reduction modulo f( x ) = x + x + 2 x + 3, using the power

	                   4      2               5      3      2
	table entries for x  = 4 x + 3 x + 2 and x  = 4 x  + 3 x + 2 x, we get
	
	                                   3      2
    s( x ) t( x ) (mod f( x ), p) = 2 x  + 3 x  + 4 x + 2	  
		

METHOD

    Compute the coefficients using the function coeff_of_product.
                                 
    The next step is to reduce s(x) t(x) modulo f(x) and p.  To do so, replace

                           k                                      k
    each non-zero term t  x,  n <= k <= 2n-2, by the term t * [ x   mod f(x), p)]
                        k                                  k

    which we get from the array power_table.

BUGS

     None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

void 
product( int  * s, int * t, int power_table[][ MAXDEGPOLY ], int n, int p )
{

/*------------------------------------------------------------------------------
|                               Local Variables                                |
------------------------------------------------------------------------------*/

int 
    i, j,                     /* Loop counters. */
    coeff,                    /* Coefficient of x ^ k term of t(x) ^2 */
    temp[ MAXDEGPOLY + 1 ] ;  /* Temporary storage for the new t(x). */

/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

/*
                                 0        n-1
    Compute the coefficients of x , ..., x.   These terms do not require

    reduction mod f(x) because their degree is less than n.
*/

for (i = 0 ;  i <= n ;  ++i)

    temp[ i ] = coeff_of_product( s, t, i, n, p ) ;

/*
                                 n        2n-2             k
    Compute the coefficients of x , ..., x.    Replace t  x  with
            k                                            k
    t  * [ x  (mod f(x), p) ] from array power_table when t is 
     k                                                     k
    non-zero.

*/

for (i = n ;  i <= 2 * n - 2 ;  ++i)

    if ( (coeff = coeff_of_product( s, t, i, n, p )) != 0 )

        for (j = 0 ;  j <= n - 1 ;  ++j)

            temp[ j ] = mod( temp[ j ] +
                             mod( coeff * power_table[ i - n ] [ j ], p ),
                             p ) ;

for (i = 0 ;  i <= n - 1 ;  ++i)

    s[ i ] = temp[ i ] ;

} /* ======================== end of function product ======================= */


/*==============================================================================
|                                  times_x                                     |
================================================================================

DESCRIPTION

     Compute x t(x) (mod f(x), p)

     Uses a precomputed table of powers of x.

INPUT
                                            2
    t (int *)              Coefficients of t (x), of degree <= n-1.

    power_table (int **)    x ^ k (mod f(x), p) for n <= k <= 2n-2, f monic.

    n (int, -infinity < n < infinity)  Degree of f(x).

    p (int, p > 0)         Mod p coefficient arithmetic.

OUTPUT

    t (int *)              Overwritten with x t(x) (mod f(x), p)

EXAMPLE 
                                    3       2        
    Let n = 4, p = 5, and t(x) = 2 x  +  4 x  + 3 x.  Let f(x) = 
     4    2                                  4      3      2
    x  + x  + 2 x + 3.  Then x t (x) = 2 x  + 4 x  + 3 x  and
                                     2                3     2
    x t(x) (mod f(x), p) = 2 * (4 x + 3 x + 2) + 4 x + 3 x  =
       3    2 
    4 x  + x + x + 4.
         3     2
    = 4 x + 2 x + 3 x + 2.

METHOD
                          n-1
    Multiply t(x) = t    x    + ... + t  by shifting the coefficients
                     n-1               0
                             n
    to the left.  If an     x   term appears, eliminate it by 

    substitution using power_table.

BUGS

     None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

void 
    times_x( int * t, int power_table[][ MAXDEGPOLY ], int n, int p )
{

/*------------------------------------------------------------------------------
|                               Local Variables                                |
------------------------------------------------------------------------------*/

int 
    i,              /* Loop counter. */
    coeff ;         /* Coefficient of x ^ n term of x t(x). */

/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

/*
    Multiply t(x) by x.  Do it by shifting the coefficients left in the array.
*/

coeff = t[ n - 1 ] ;

for (i = n-1 ;  i >= 1 ;  --i)

    t[ i ] = t[ i-1 ] ;

t[ 0 ] = 0 ;

/*
                  n                          n
    Zero out t  x .  Replace it with t * [ x  (mod f(x), p) ] from array
              n
    power_table.
*/

if (coeff != 0)
{
    for (i = 0 ;  i <= n - 1 ;  ++i)

        t[ i ] = mod( t[ i ] +
                      mod( coeff * power_table[ 0 ] [ i ], p ),
                      p ) ;
}

} /* ======================== end of function times_x ======================= */



/*==============================================================================
|                                  x_to_power                                  |
================================================================================

DESCRIPTION
                     m
     Compute g(x) = x (mod f(x), p).

INPUT

    m (int)       1 <= m <= r.  The exponent.

    power_table (int **)    x ^ k (mod f(x), p) for n <= k <= 2n-2, f monic.

    n (int, n >= 1)     Degree of monic polynomial f(x).

    p (int, p >= 2)     Modulo p coefficient arithmetic.

OUTPUT

    g (int *)     Polynomial of degree <= n-1.

EXAMPLE 
                              4   2 
    Let n = 4, p = 5, f(x) = x + x + 2 x + 3, and m = 156.

    156 = 0  . . . 0  1  0  0  1  1  1  0  0 (binary representation)
          |<- ignore ->| S  S SX SX SX  S  S (exponentiation rule,
		                                      S = square, X = multiply by x)
     m
    x (mod f(x), p) =

         2     2
    S   x  =  x

         4       2
    S   x  =  4 x  + 3 x + 2

         8       3      2
    S   x  =  4 x  + 4 x + 1

         9       3     2
    X   x  =  4 x  +  x + 3 x + 3

         18       2  
    S   x  =  2 x  +  x + 2

         19      3     2
    X   x  =  2 x  +  x  + 2 x

         38      3       2
    S   x  =  2 x  +  4 x  +  3 x

         39      3       2
    X   x  =  4 x  +  2 x  +  x + 4

         78      3       2
    S   x  =  4 x  +  2 x  +  3 x + 2

         156 
    S   x    = 3

METHOD

    Exponentiation by repeated squaring, using precomputed table of
	powers.  See ART OF COMPUTER PROGRAMMING, vol. 2, 2nd id,
	D. E. Knuth,  pgs 441-443.

     n         2n-2
    x,  ... , x    (mod f(x), p)

                    m
    to find g(x) = x   (mod f(x), p), expand m into binary,

           k        k-1
    m = a 2  + a   2    + . . . + a 2 + a
         k      k-1                1     0

                            m
	where a = 1, and split x   apart into
           k

            k      k
           2      2  a             2 a    a
     m                k-1             1    0
    x  =  x     x           . . .  x     x


    Then to raise x to the mth power, do


    t( x ) = x

    return if m = 1


    for i = k-1 downto 0 do

                   2
        t(x) = t(x)  (mod f(x), p)       // Square each time.

        if a = 1 then
		    i

            t(x) = x t(x) (mod f(x), p)  // Times x only if current bit is 1
        endif

    endfor
                                                        k
                                                       2
    The initial t(x) = x gets squared k times to give x  .  If a  = 1 for
	                                                            i
    0 <= i <= k-1, we multiply by x which then gets squared i times more

              i
             2
    to give x .  On a binary computer, we use bit shifting and masking to

    identify the k bits  { a    . . .  a  } to the right of the leading 1
	                        k-1         0

    bit.  There are log ( m ) - 1 squarings and (number of 1 bits) - 1
		                   2
	multiplies.  

BUGS

     None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

void 
    x_to_power( bigint m, int * g, int power_table[][ MAXDEGPOLY ], int n, int p )
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

bigint 
    mask = ((bigint)1 << (NUMBITS - 1)) ;

int 
    bit_count = 0,  /* Number of bits in m to the right of the leading bit. */
    i ;             /* Loop counter. */


/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

#ifdef DEBUG_PP_PRIMPOLY
printf( "x to power = x ^ %lld\n", m ) ;
#endif

/*
    Initialize g(x) to x.  Exit right away if m = 1.
*/

for (i = 0 ;  i <= n-1 ;  ++i)

    g[ i ] = 0 ;

g[ 1 ] = 1 ;

if (m == 1)

    return ;

/*
    Advance the leading bit of m up to the word's left hand boundary.
    Count how many bits were to the right of the leading bit.
*/

while (! (m & mask))
{
    m <<= 1 ;
    ++bit_count ;
}

bit_count = NUMBITS - bit_count ;



/*
    Exponentiation by repeated squaring.  Discard the leading 1 bit.
    Thereafter, square for every 0 bit;  square and multiply by x for
    every 1 bit.
*/

while ( --bit_count > 0 )
{

    m <<= 1 ;       /*  Expose the next bit. */

    square( g, power_table, n, p ) ;

    #ifdef DEBUG_PP_PRIMPOLY
    printf( "    after squaring, poly = \n" ) ;
    write_poly( g, n-1 ) ;
    printf( "\n\n" );
    #endif

    if (m & mask)   /*  Leading bit is 1. */
    {
        times_x( g, power_table, n, p ) ;

        #ifdef DEBUG_PP_PRIMPOLY
        printf( "    after additional times x, poly = \n" ) ;
        write_poly( g, n-1 ) ;
        printf( "\n\n" );
        #endif
    }
}

} /* ===================== end of function x_to_power ======================= */
