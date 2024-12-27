#include <stdio.h>  /* for printf()  */
#include <stdlib.h> /* for _MAX_PATH */

#include "Primpoly.h"


/*==============================================================================
|                               parse_command_line                             |
================================================================================

DESCRIPTION

     Parse the command line.

INPUT
                                                                     n
     a[]  (int *)  Coefficients of the nth degree polynomial a(x) = a x  + ...
                                                                     n
                   + a  x  +  a.    a  is stored at array location a[i], 
                      1        0     i
                   0 <= i < n .

     n    (int)    The polynomial's degree.

OUTPUT

  testPolynomialForPrimitivity 
    

EXAMPLE CALLING SEQUENCE

   pp -h                   Prints help.
   pp -s 2 4               Prints search statistics.
   pp -t 2 4 x^3+x^2+1     Checks a polynomial for primitivity.  No blanks, please!
   pp -a 2 4               Lists all primitive polynomials of degree 4 modulo 2.
   pp -c 2 4               Does a time-consuming double check on primitivity.

METHOD

    

BUGS

    None.


--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

int parse_command_line( int    argc, 
					    char * argv[], 
                        int *  testPolynomialForPrimitivity,
                        int *  listAllPrimitivePolynomials,
                        int *  printStatistics,
                        int *  printHelp,
                        int *  selfCheck,
                        int *  p,
                        int *  n,
                        int *  testPolynomial )
{

int    input_arg_index ;
char * input_arg_string ;
char * option_ptr ;

int    num_arg ;
char * arg_string[ _MAX_PATH ] ;

/*  Initialize to defaults. */
*testPolynomialForPrimitivity = NO ;
*listAllPrimitivePolynomials  = NO ;
*printStatistics              = NO ;
*printHelp                    = NO ;
*selfCheck                    = NO ;
*p                            = 0 ;
*n                            = 0 ;
testPolynomial                = (int *) 0 ;


/*
 *  Parse the command line to get the options and their inputs.
 */

for (input_arg_index = 0, num_arg = 0 ;  input_arg_index < argc ;  
     ++input_arg_index)
{
    /*  Get next argument string. */
    input_arg_string = argv[ input_arg_index ] ;

    /* We have an option:  a hyphen followed by a non-null string. */
    if (input_arg_string[ 0 ] == '-' && input_arg_string[ 1 ] != '\0')
    {
        /* Scan all options. */
        for (option_ptr = input_arg_string + 1 ;  *option_ptr != '\0' ;
             ++option_ptr)
        {
            switch( *option_ptr )
            {
                /* Test a given polynomial for primitivity. */
                case 't':
                    *testPolynomialForPrimitivity = YES ;
                break ;

                /* List all primitive polynomials.  */
                case 'a':
                   *listAllPrimitivePolynomials = YES ;
                break ;

                /* Print statistics on program operation. */
                case 's':
                   *printStatistics = YES ;
                break ;

                /* Print help. */
                case 'h':
				case 'H':
                    *printHelp = YES ;
                break ;

                /* Turn on all self-checking (slow!).  */
                case 'c':
					*selfCheck = YES ;
                break ;

                default:
                   printf( "Cannot recognize the option %c\n", *option_ptr ) ;
                break ;
            }
        }
    }
    else  /* Not an option, but an argument. */
    {
        arg_string[ num_arg++ ] = input_arg_string ;
    }
}


/* Assume the next two arguments are p and n. */
if (num_arg == 3)
{
    *p = atoi( arg_string[ 1 ] ) ;
    *n = atoi( arg_string[ 2 ] ) ;

}
else
{
    printf( "ERROR:  Expecting two arguments, p and n.\n\n" ) ;
	*printHelp = YES ;
}

return 0 ;

} /* ================ end of function parse_command_line ==================== */


/*==============================================================================
|                                    write_poly                                |
================================================================================

DESCRIPTION

     Print a polynomial in a nice format.  Omit terms with coefficients of zero.
     Suppress coefficients of 1, but not the constant term.  Put every
     NUMTERMSPERLINE terms in the polynomial on a new line.

INPUT
                                                                     n
     a[]  (int *)  Coefficients of the nth degree polynomial a(x) = a x  + ...
                                                                     n
                   + a  x  +  a.    a  is stored at array location a[i], 
                      1        0     i
                   0 <= i < n .

     n    (int)    The polynomial's degree.

OUTPUT

    Standard output    A pretty-printed polynomial.

EXAMPLE CALLING SEQUENCE

    If array a[] contains 

    a[0] = 1
    a[1] = 2
    a[2] = 0
    a[3] = 1

    and n = 3,

    write_poly( a, n ) prints

    x ^ 3  +  2 x  + 1

METHOD

    Sheer hacking ingenuity (vulgarity?).

BUGS

    None.

--------------------------------------------------------------------------------
|                                Function Call                                 |
------------------------------------------------------------------------------*/

void write_poly( int * a, int n )
{

/*------------------------------------------------------------------------------
|                               Local Variables                                |
------------------------------------------------------------------------------*/

int
    k,                         /*  Loop conter. */
    coeff,                     /*  Coefficient a sub k.  */
    first_time_through = YES,  /* = NO after printing the first coefficient. */
    num_terms = 0 ;            /*  Number of terms printed so far.  */

/*------------------------------------------------------------------------------
|                                Function Body                                 |
------------------------------------------------------------------------------*/

for (k = n ;  k >= 0 ;  --k)
{

    /*                    
                      k 
    Print the term a x  if a  is nonzero.
                    k       k
    */

    if ( (coeff = a[ k ]) != 0)
    {

       /* After the first time thorough, print leading plus signs "+" to 
          separate the terms of the polynomial. */

        if (!first_time_through)

            printf( " + " ) ;

        first_time_through = NO ;

        /* Always print the constant term a  but print terms of higher degree
                                           0 
           only if a  is not 1.
                    k             */

        if ( (coeff != 1) || (k == 0) )

            printf( "%d", coeff ) ;


        /*                  k                             1      0
            Print the term x.  Exceptions:  print x, not x, omit x  */

        if (k > 1)

            printf( " x ^ %d", k ) ;

        else if (k == 1)

            printf( " x" ) ;

        /*  Start a new line. */

        if (++num_terms % NUMTERMSPERLINE == 0)

            printf( "\n" ) ;

    } /* end if coeff > 0 */

} /* end for */

printf( "\n" ) ;

return ;

} /* ======================= end of function write_poly ===================== */
