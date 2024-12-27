#include <cstdlib>      // abort()
#include <iostream>     // Basic stream I/O.
#include <new>          // set_new_handler()
#include <cmath>        // Basic math functions e.g. sqrt()
#include <limits>       // Numeric limits.
#include <complex>      // Complex data type and operations.
#include <fstream>      // File stream I/O.
#include <sstream>      // String stream I/O.
#include <vector>       // STL vector class.
#include <string>       // STL string class.
#include <algorithm>    // Iterators.
#include <stdexcept>    // Exceptions.
#include <cassert>      // assert()

using namespace std ;   // So we don't need to say std::vector everywhere.



/*------------------------------------------------------------------------------
|                                PP Include Files                              |
------------------------------------------------------------------------------*/

#include "Primpoly.hpp"         // Global functions.
#include "ppArith.hpp"          // Basic arithmetic functions.
#include "ppBigInt.hpp"         // Arbitrary precision integer arithmetic.
#include "ppOperationCount.hpp" // OperationCount collection for factoring and poly finding.
#include "ppFactor.hpp"         // Prime factorization and Euler Phi.
#include "ppPolynomial.hpp"     // Polynomial operations and mod polynomial operations.
#include "ppParser.hpp"         // Parsing of polynomials and I/O services.
#include "ppUnitTest.hpp"       // Complete unit test.


/*=============================================================================
 |
 | NAME
 |
 |     OperationCount
 |
 | DESCRIPTION
 |
 |     Default constructor.
 |
 +============================================================================*/

OperationCount::OperationCount()
    : n( 0u )
    , p( 0u )
    , max_num_possible_poly( 0u )
    , num_primitive_poly( 0u )
    , num_poly_tested( 0u )
    , num_gcds( 0u )
    , num_primality_tests( 0u )
    , num_squarings( 0u )
    , num_trial_divides( 0u )
    , num_free_of_linear_factors( 0u )
    , num_where_const_coeff_is_primitive_root( 0u )
    , num_passing_const_coeff_test( 0u )
    , num_irreducible_to_power( 0u )
    , num_order_m( 0u )
    , num_order_r( 0u )
{
}



/*=============================================================================
 |
 | NAME
 |
 |     OperationCount
 |
 | DESCRIPTION
 |
 |     No special destruction needed.
 |
 +============================================================================*/

OperationCount::~OperationCount()
{
}



/*=============================================================================
 |
 | NAME
 |
 |     OperationCount
 |
 | DESCRIPTION
 |
 |     Copy constructor.
 |
 +============================================================================*/

OperationCount::OperationCount( const OperationCount & statistics )
           :n( statistics.n )
           ,p( statistics.p )
           ,max_num_possible_poly( statistics.max_num_possible_poly )
           ,num_primitive_poly( statistics.num_primitive_poly )
           ,num_poly_tested( statistics.num_poly_tested )
           ,num_gcds( statistics.num_gcds )
           ,num_primality_tests( statistics.num_primality_tests )
           ,num_squarings( statistics.num_squarings )
           ,num_trial_divides( statistics.num_trial_divides )
           ,num_free_of_linear_factors( statistics.num_free_of_linear_factors )
           ,num_where_const_coeff_is_primitive_root( statistics.num_where_const_coeff_is_primitive_root )
           ,num_passing_const_coeff_test( statistics.num_passing_const_coeff_test )
           ,num_irreducible_to_power( statistics.num_irreducible_to_power )
           ,num_order_m( statistics.num_order_m )
           ,num_order_r( statistics.num_order_r )

{
}



/*=============================================================================
 |
 | NAME
 |
 |     OperationCount
 |
 | DESCRIPTION
 |
 |     Assignment.
 |
 +============================================================================*/

OperationCount & OperationCount::operator=( const OperationCount & statistics )
{
    // Check for assigning to oneself:  just pass back a reference to the unchanged object.
    if (this == &statistics)
        return *this ;

    n                            = statistics.n ;
    p                            = statistics.p ;

    max_num_possible_poly           = statistics.max_num_possible_poly ;
    num_primitive_poly             = statistics.num_primitive_poly ;

    num_poly_tested                = statistics.num_poly_tested ;
    num_gcds                      = statistics.num_gcds ;
    num_primality_tests            = statistics.num_primality_tests ;
    num_squarings                 = statistics.num_squarings ;
    num_trial_divides              = statistics.num_trial_divides ;
    num_free_of_linear_factors       = statistics.num_free_of_linear_factors ;
    num_where_const_coeff_is_primitive_root = statistics.num_where_const_coeff_is_primitive_root ;
    num_passing_const_coeff_test  = statistics.num_passing_const_coeff_test ;
    num_irreducible_to_power        = statistics.num_irreducible_to_power ;
    num_order_m                    = statistics.num_order_m ;
    num_order_r                    = statistics.num_order_r ;

    return *this ;
}



/*=============================================================================
 |
 | NAME
 |
 |     OperationCount
 |
 | DESCRIPTION
 |
 |     Print out a report of the operation count to the console.
 |
 +============================================================================*/

ostream & operator<<( ostream & out, const OperationCount & op )
{
    out << "+--------- OperationCount --------------------------------\n" ;
    out << "|\n" ;
    out << "| Integer factorization:  Table lookup + Trial division + Pollard Rho\n" ;
    out << "|\n" ;
    out << "| Number of trial divisions :           " << op.num_trial_divides << endl ;
    out << "| Number of gcd's computed :            " << op.num_gcds << endl ;
    out << "| Number of primality tests :           " << op.num_primality_tests << endl ;
    out << "| Number of squarings:                  " << op.num_squarings << endl ;
    out << "|\n" ;
    out << "| Polynomial Testing\n" ;
    out << "|\n" ;
    out << "| Total num. degree " << op.n << " poly mod " << op.p << " :      " << op.max_num_possible_poly << endl ;
    out << "| Number of possible primitive poly:    " << op.num_primitive_poly << endl ;
    out << "| Polynomials tested :                  " << op.num_poly_tested << endl ;
    out << "| Const. coeff. was primitive root :    " << op.num_where_const_coeff_is_primitive_root << endl ;
    out << "| Free of linear factors :              " << op.num_free_of_linear_factors << endl ;
    out << "| Irreducible to power >=1 :            " << op.num_irreducible_to_power << endl ;
    out << "| Had order r (x^r = integer) :         " << op.num_order_r << endl ;
    out << "| Passed const. coeff. test :           " << op.num_passing_const_coeff_test << endl ;
    out << "| Had order m (x^m != integer) :        " << op.num_order_m << endl ;
    out << "|\n" ;
    out << "+-----------------------------------------------------\n" ;
    
    return out ;
}
