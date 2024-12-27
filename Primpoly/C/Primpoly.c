#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Primpoly.h"


/*==============================================================================
|                                    main                                      |
================================================================================

DESCRIPTION

     Program for finding a primitive polynomial of degree n modulo p for use in
     generating PN sequences and finite fields for error control coding.

INPUT 

     Run the program by typing

         $ Primpoly p n

OUTPUT

     You will get an nth degree primitive polynomial modulo p.

EXAMPLE CALLING SEQUENCE

    Let's find a primitive polynomial of degree 18, modulo the prime 11.
    After a few seconds, even on a slow PC, we get:

        $ primpoly -s 5 20

        Primpoly Version 16.3 - A Program for Computing Primitive Polynomials.
        Copyright (C) 1999-2024 by Sean Erik O'Connor.  All Rights Reserved.

        Primpoly comes with ABSOLUTELY NO WARRANTY; for details see the
        GNU General Public License.  This is free software, and you are welcome
        to redistribute it under certain conditions; see the GNU General Public License
        for details.


        Factoring r = 23841857910156 into
            2^2 3 11 13 41 71 521 9161

        Total number of primitive polynomials = 1280348160000.  Begin testing...


        Primitive polynomial modulo 5 of degree 20

         x ^ 20   +    x ^  2   +    2 x   +    3


        +--------- Statistics ----------------------------
        |
        | Total num. degree 20 polynomials mod  5 :  95367431640625
        | Actually tested :                          39
        | Const. coeff. was primitive root :         16
        | Free of linear factors :                    5
        | Irreducible or irred. to power :            3
        | Had order r (x^r = integer) :               1
        | Passed const. coeff. test :                 1
        | Had order m (x^m != integer) :              1
        |
        +-------------------------------------------------

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int main( int argc, char * argv[] )
{

/*------------------------------------------------------------------------------
|                               Local Variables                                |
------------------------------------------------------------------------------*/

int 
    n,      /*  Degree of the primitive polynomial f(x) to be found, n >= 2.  */

    p ;     /*  Modulo p arithmetic on the polynomial coefficients, p >= 2.   */

bigint
    max_p_to_n = MAXPTON,          /* Maximum value of p ^ n.               */

    max_num_poly,                  /* p ^ n, the number of polynomials to
                                      test for primitivity.                 */

    num_poly = 0,                  /* Number of polynomials tested so far.  */

    r,                             /* The number (p ^ n - 1)/(p - 1).       */

    primes[ MAXNUMPRIMEFACTORS ],  /* The distinct prime factors of r.      */
    prim_poly_count = 0,           /* Counter for primitive polynomials found.        */
    num_prim_poly = 0 ;            /* Total number of possible primitive polynomials. */

int
    count[ MAXNUMPRIMEFACTORS ],   /* ... and their multiplicities.         */
    i,                             /* Prime index. */
    prime_count,                   /* Primes are stored in array locations 0
                                      through prime_count.                  */

    f[ MAXDEGPOLY + 1 ],           /* Coefficients of the polynomial f(x)  
                                      which we test for primitivity.        */

    a = 0,                         /* Integer in the order r test.          */

    is_primitive_poly = NO,        /* Equal to YES as soon as a primitive 
                                      polynomial is found.                  */

    num_free_of_linear_factors = 0,/* The number of polynomials which have  
                                      no linear factors.                    */

    num_const_coeff_prim_root = 0, /* Number of polynomials whose constant  
                                      is a primitive root of p.             */

    num_passing_const_coeff_test = 0, /* Number of polynomials whose constant
                                         term passes a consistency check.   */
    num_irred_to_power = 0,        /* Number of polynomials which are of the
                                      form irreducible poly to a power >= 1.*/
    num_order_m = 0,
    num_order_r = 0,               /* The number of polynomials which pass
                                      the order_m and order_r tests.                    */
    stopTesting = NO,              /* When to stop testing polynomials for primitivity. */
    testPolynomialForPrimitivity = NO, /* Test a given input polnomial for primitivity? */
    testPolynomial[ MAXDEGPOLY + 1 ],  /* Coefficients of the test polynomial.          */
    listAllPrimitivePolynomials  = NO, /* Print ALL primitive polynomials?              */
    printStatistics              = NO, /* Print statistics?                             */
    printHelp                    = NO, /* Print help information?                       */
    selfCheck                    = NO, /* Do a self-check?  Time consuming!             */

    /*  x ^ n , ... , x ^ 2n-2 (mod f(x), p) */
    power_table[ MAXDEGPOLY - 1 ] [ MAXDEGPOLY ] ;


char outputFormat[ _MAX_PATH ] ; /* Formatting for printf's (used only when printing bigints) */

char * help =
{
     "This program generates a primitive polynomial of degree n modulo p.\n\n"
         "Usage:    primpoly p n\n\n"
         "Example:  primpoly 2 4 \n"
	 "          generates the fourth degree polynomial\n\n" 
         "          x ^ 4 + x + 1, whose coefficients use modulo 2 arithmetic.\n\n"

     "Primitive polynomials find many uses in mathematics and communications \n"
     "engineering:\n"
     "   * Generation of pseudonoise (PN) sequences for spread spectrum\n"
     "     communications and chip fault testing.\n"
     "   * Generation of CRC and Hamming codes.\n"
     "   * Generation of Galois (finite) fields for use in decoding Reed-Solomon\n"
     "     and BCH error correcting codes.\n\n"

     "Options:\n"
     "   pp -c 2 4\n"
     "       does an addtional time consuming double check on the primitivity.\n"
     "   pp -s 2 4\n"
     "       prints search statistics.\n"
     "   pp -a 2 4\n"
     "       lists ALL primitive polynomials of degree 4 modulo 2.\n"
     "\n\n"
} ;

/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/



/*  
     Read the parameters p and n from the command line.  Return with an error
     message if there are an incorrect number of inputs on the command line,
     or if p and n are out of bounds.
*/
parse_command_line( argc, 
                    argv,
                    &testPolynomialForPrimitivity,
                    &listAllPrimitivePolynomials,
                    &printStatistics,
                    &printHelp,
                    &selfCheck,
                    &p,
                    &n,
                    testPolynomial ) ;

if (printHelp)
{
    printf(  "%s", help )  ;  

    exit( 1 ) ;
}

if (p < 2)
{
    printf( "ERROR:  p must be 2 or more.\n\n" ) ;
    exit( 1 ) ;
}

if (n > MAXDEGPOLY || n < 2)
{
    printf( "ERROR: n must be between 2 and %d\n\n", MAXDEGPOLY ) ;
    exit( 1 ) ;
}


/*  Check to see if p is a prime. */
if (!is_almost_surely_prime( p ))
{
    printf( "ERROR:  p must be a prime number.\n\n" ) ;
    exit( 1 ) ;
}



/*
               n
    Return if p  > MAXPTON because then we'll exceed the computer integer precision.
*/
if (n * log( (double) p ) > log( (double) (sbigint) max_p_to_n ))
{
    printf( "ERROR:  p to the nth power must be smaller than %llu\n\n", MAXPTON ) ;
    exit( 1 ) ;
}



/*
                         n
              n         p  - 1
     Compute p  and r = ------.
                        p - 1
*/
max_num_poly = power( p, n ) ;

r = (max_num_poly - 1) / (p - 1) ;




/*  Factor r into distinct primes. */
if (printStatistics)
{
    sprintf( outputFormat, "%s%s%s", "\nFactoring r = ", bigintOutputFormat, " into\n    " ) ;
    printf( outputFormat, r ) ;
}

prime_count = factor( r, primes, count ) ;

if (printStatistics)
{
    for (i = 0 ;  i <= prime_count ;  ++i)
    {
        if (count[ i ] == 1)
        {
            sprintf( outputFormat, "%s ", bigintOutputFormat ) ;
            printf(  outputFormat, primes[ i ] ) ;
        }
        else
        {
            sprintf( outputFormat, "%s%s", bigintOutputFormat, "^%d " ) ;
            printf(  outputFormat, primes[ i ], count[ i ] ) ;
        }
    }
    printf( "\n\n" ) ;
}


/*                       n
     Initialize f(x) to x  + (-1).  Then, when f(x) passes through function 
                                                                          n
     next_trial_poly for the first time, it will have the correct value, x
*/
initial_trial_poly( f, n ) ;

if (printStatistics || listAllPrimitivePolynomials)
{
    sprintf( outputFormat, "%s%s%s", "Total number of primitive polynomials = ", bigintOutputFormat, ".  Begin testing...\n\n" ) ;
    num_prim_poly = EulerPhi( power( p, n ) - 1 ) / n ;
    printf( outputFormat, num_prim_poly ) ;
}



/*
     Generate and test all possible n th degree, monic, modulo p polynomials
     f(x).  A polynomial is primitive if passes all the tests successfully.
*/
do {
    next_trial_poly( f, n, p ) ;      /* Try another polynomal. */
    ++num_poly ;

    #ifdef DEBUG_PP_PRIMPOLY
    printf( "\nNext trial polynomial:  " ) ;
    write_poly( f, n ) ;
    printf( "\n" ) ;
    #endif

    /*                         n         2n-2
        Precompute the powers x ,  ..., x     (mod f(x), p)
        for use in all later computations.
    */
    construct_power_table( power_table, f, n, p ) ;


    /* Constant coefficient of f(x) * (-1)^n must be a primitive root of p. */
    if (const_coeff_is_primitive_root( f, n, p ))
    {
        ++num_const_coeff_prim_root ;

        #ifdef DEBUG_PP_PRIMPOLY
        printf( "Coefficient of polynomial is primitive root.\n" ) ;
        #endif

        /* f(x) can't have any linear factors. */
        if (!linear_factor( f, n, p ))
        {
            ++num_free_of_linear_factors ;

            #ifdef DEBUG_PP_PRIMPOLY
            printf( "Free of linear factors.\n" ) ;
            #endif

            /* f(x) can't have two or more distinct irreducible factors. */
            if (!has_multi_irred_factors( power_table, n, p ))
            {
                ++num_irred_to_power ;

                #ifdef DEBUG_PP_PRIMPOLY
                printf( "Has one unique irreducible factor.\n" ) ;
                #endif

                /* x^r (mod f(x), p) = a must be an integer. */
                if (order_r( power_table, n, p, r, &a ))
                {
                    ++num_order_r ;

                     #ifdef DEBUG_PP_PRIMPOLY
                     printf( "Passes the order r test.\n" ) ;
                     #endif

                     /*  Const coeff. of f(x)*(-1)^n must equal a mod p. */
                     if (const_coeff_test( f, n, p, a ))
                     {
                         ++num_passing_const_coeff_test ;

                         #ifdef DEBUG_PP_PRIMPOLY
                         printf( "Passes the constant coefficient test.\n" ) ;
                         #endif

                         /*  x^m != integer for all m = r / q, q a prime divisor of r. */
                         if (order_m( power_table, n, p, r, primes, prime_count ))
                         {
                             ++num_order_m ;
                             is_primitive_poly = YES ;

                             #ifdef DEBUG_PP_PRIMPOLY
                             printf( "Passes the order m tests.\n" ) ;
                             #endif

                             if (listAllPrimitivePolynomials)
                             {
                                 printf( "\n\nPrimitive polynomial " ) ;
                                 sprintf( outputFormat, "%s of %s ", bigintOutputFormat, bigintOutputFormat ) ;
                                 printf(  outputFormat, ++prim_poly_count, num_prim_poly ) ;
                                 printf( "modulo %d of degree %d\n\n", p, n ) ;
                                 write_poly( f, n ) ;
                                 printf( "\n\n" ) ;
                             }
                         }
                     } /* end const coeff test */
                } /* end order r */
            } /* end can't determine if reducible */
        } /* end no linear factors */
    } /* end constant coefficient primitive. */

    /* Stop when we've either checked all possible polynomials or 
       we've not been asked to list all and found the first primtive one.  
    */
    stopTesting = (num_poly > max_num_poly) || 
                  (!listAllPrimitivePolynomials && is_primitive_poly) ;

} while( !stopTesting ) ;

printf( "\n\n" ) ;


/*
     Report on success or failure.
*/

if (listAllPrimitivePolynomials)
    ; /* We're done */
else if (is_primitive_poly)
{
    printf( "\n\nPrimitive polynomial modulo %d of degree %d\n\n", 
            p, n ) ;
    write_poly( f, n ) ;
    printf( "\n\n" ) ;
}
else {

    printf( "Internal error:  \n"
            "Tested all possible polynomials (%llu), but failed\n"
            "to find a primitive polynomial.\n"
            "Please let the author know by e-mail.\n",
            max_num_poly ) ;
    exit( 1 ) ;
}


/*  Print the statistics of the primitivity tests. */

if (printStatistics)
{
    printf( "+--------- Statistics -----------------------------------------------------------------\n" ) ;
    printf( "|\n" ) ;
    sprintf( outputFormat, "%s%s%s", "| Total num. degree %3d polynomials mod %3d :    ", bigintOutputFormat, "\n" ) ;
    printf( outputFormat, n, p, max_num_poly ) ;
    sprintf( outputFormat, "%s%s%s", "| Actually tested :                              ", bigintOutputFormat, "\n" ) ;
    printf( outputFormat,  num_poly ) ;
    printf( "| Const. coeff. was primitive root :      %10d\n",  num_const_coeff_prim_root ) ;
    printf( "| Free of linear factors :                %10d\n",  num_free_of_linear_factors ) ;
    printf( "| Irreducible or irred. to power :        %10d\n",  num_irred_to_power ) ;
    printf( "| Had order r (x^r = integer) :           %10d\n",  num_order_r ) ;
    printf( "| Passed const. coeff. test :             %10d\n",  num_passing_const_coeff_test ) ;
    printf( "| Had order m (x^m != integer) :          %10d\n",  num_order_m ) ;
    printf( "|\n" ) ;
    printf( "+--------------------------------------------------------------------------------------\n" ) ;
}



/*  Confirm f(x) is primitive using a different, but extremely slow test for 
    primitivity.  Disabled when we list all primitive polynomials.
*/
if (selfCheck && !listAllPrimitivePolynomials)
{

    printf( "\nConfirming polynomial is primitive with an independent check.\n"
            "Warning:  You may wait an impossibly long time!\n\n" ) ;

    if (maximal_order( f, n, p ))
        printf( "    -Polynomial is confirmed to be primitive.\n\n" ) ;
    else
    {
        printf( "Internal error:  \n"
                "Primitive polynomial confirmation test failed.\n"
                "Please let the author know by e-mail.\n\n" ) ;
        return 1 ;
    }
}

return 0 ;

} /* ========================== end of function main ======================== */
