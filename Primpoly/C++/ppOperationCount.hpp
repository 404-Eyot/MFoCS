#ifndef __PP_STATISTICS_H__
#define __PP_STATISTICS_H__


/*=============================================================================
 |
 | NAME
 |
 |     OperationCount
 |
 | DESCRIPTION
 |
 |     Class for collecting statistics on operation counts for
 |     factoring and polynomial testing.
 |
 +============================================================================*/

class OperationCount
{
    public:
        OperationCount() ;

        ~OperationCount() ;

        OperationCount( const OperationCount & statistics ) ;

        OperationCount & operator=( const OperationCount & statistics ) ;

        friend ostream & operator<<( ostream & , const OperationCount & ) ;

    // Allow direct access to this simple data type for convenience.
    public:
        ppuint n ;                            // Degree of the polynomial.
        ppuint p ;                            // Modulus of the polynomial.

        BigInt max_num_possible_poly ;           // Number of possible degree n modulo p polynomials.
        BigInt num_primitive_poly ;             // Number of primitive degree n modulo p polynomials.
        BigInt num_poly_tested ;                // Number of polynomials tested.
        
        BigInt num_gcds ;                      // Number of gcd computations.
        BigInt num_primality_tests ;            // Number primality tests.
        BigInt num_squarings ;                 // Number of squarings.
        BigInt num_trial_divides ;              // Number of trial divisions.

        BigInt num_free_of_linear_factors ;       // Number of polynomials which have no linear factors.
        BigInt num_where_const_coeff_is_primitive_root ;  // Number of polynomials whose constant is a primitive root of p.
        BigInt num_passing_const_coeff_test ; // Number of polynomials whose constant term passes a consistency check.
        BigInt num_irreducible_to_power ;       // Number of polynomials which are of the form irreducible poly to a power >= 1.
        BigInt num_order_m ;                   // The number of polynomials which pass the x^m not an integer test.
        BigInt num_order_r ;                   // The number of polynomials which pass the x^r = integer test.
} ;

#endif // __PP_STATISTICS_H__
