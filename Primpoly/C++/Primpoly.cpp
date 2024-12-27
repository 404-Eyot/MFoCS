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

using namespace std ;   // I don't want to use the std:: prefix everywhere.

#include "ctype.h"      // C string functions.


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




int main( int argc, const char * argv[] )
{
    try
    {
        // Set up the full parser for both command line parsing and polynomial parsing.
        PolyParser<PolySymbol, PolyValue> parser ;
        parser.parseCommandLine( argc, argv ) ;

        #ifdef SELF_CHECK
        // Do a self check always.  We might fail to pass one or more unit tests, or the unit test itself might fail.
        try
        {
            UnitTest unitTest ;
            if (!unitTest.run())
                  throw PrimpolyError( "Self-check failed!" ) ;
              else
                  cout << "Self-check passes..." << endl ;
        }
        catch (UnitTestError & e)
        {
            throw PrimpolyError( static_cast<string>( "Could not run the self-check!\n" ) + " [ " + e.what() + " ] " ) ;
        }
        #endif

        // The user input a polynomial.  Test it for primitivity.
        if (parser.test_polynomial_for_primitivity_)
        {
            // Test for primitivity with the quick test.
            Polynomial f( parser.test_polynomial_ ) ;
            #ifdef DEBUG_PP_PRIMITIVITY
            cout << "Factoring into primes r = (p^n-1)/(p-1) = " << " for n = " << parser.n << " p = " << parser.p << endl ;
            #endif // DEBUG_PP_PRIMITIVITY
            PolyOrder order( f ) ;
            cout << f << " is " << (order.isPrimitive() ? "" : "NOT") << " primitive!" << endl ;

            if (parser.print_operation_count_)
                cout << order.statistics_ << endl ;

            // Also do a very slow maximal order test for primitivity, if asked to do so.
            if (parser.slow_confirm_)
            {
                cout << confirmWarning ;
                cout << " confirmed " << (order.maximal_order() ? "" : "NOT") << " primitive!" << endl ;
            }
        }
        //  Find one primitive polynomial at random.  Optionally, find all primitive polynomials.
        else
        {
            //   Generate and test all possible n th degree, monic, modulo p polynomials
            //   f(x).  A polynomial is primitive if passes all the tests successfully.
            //                       n
            //   Initialize f(x) to x  + (-1).  Then, when f(x) passes through function
            //                                                                        n
            //   nextTrialPoly for the first time, it will have the correct value, x
            TrialPolynomial f ;
            f.initialTrialPoly( parser.n, parser.p ) ;
            #ifdef DEBUG_PP_PRIMITIVITY
            cout << "Factoring into primes r = (p^n-1)/(p-1) = " << " for n = " << parser.n << " p = " << parser.p << endl ;
            #endif // DEBUG_PP_PRIMITIVITY
            PolyOrder order( f ) ;

            bool is_primitive_poly{ false } ;
            bool tried_all_poly{ false } ;
            bool stopTesting{ false } ;

            BigInt num_poly( 0u ) ;
            BigInt num_primitive_poly( 0u ) ;

            if (parser.list_all_primitive_polynomials_)
                cout << "\n\nThere are " << order.getNumPrimPoly() << " primitive polynomials modulo " << f.modulus() << " of degree " << f.deg() << "\n\n" ;

            do {
                ++num_poly ;
                
                #ifdef DEBUG_PP_PRIMITIVITY
                cout << "Testing polynomial # " << num_poly << ") p(x) = " << f << " for primitivity" << endl ;
                #endif // DEBUG_PP_PRIMITIVITY

                order.resetPolynomial( f ) ;
                is_primitive_poly = order.isPrimitive() ;

                if (is_primitive_poly)
                {
                    ++num_primitive_poly ;
                    cout << "\n\nPrimitive polynomial modulo " << f.modulus() << " of degree " << f.deg() << "\n\n" ;
                    cout << f ;
                    cout << endl << endl ;

                    // Do a very slow maximal order test for primitivity.
                    if (parser.slow_confirm_)
                    {
                        cout << confirmWarning ;
                        if (order.maximal_order())
                            cout << f << " confirmed primitive!" << endl ;
                        else
                        {
                            ostringstream os ;
                            os << "Fast test says " << f << " is a primitive polynomial but slow test disagrees.\n" ;
                            throw PolynomialError( os.str() ) ;
                        }
                    }

                    // Early out if we've found all the primitive polynomials.
                    if (num_primitive_poly >= order.getNumPrimPoly())
                        break ;
                }

                tried_all_poly = (num_poly >= order.getMaxNumPoly()) ;
                stopTesting = tried_all_poly || (!parser.list_all_primitive_polynomials_ && is_primitive_poly) ;

                f.nextTrialPoly() ;      // Try next polynomal in sequence.
            } while( !stopTesting ) ;

            if (parser.print_operation_count_)
                cout << order.statistics_ << endl ;

            // Didn't find a primitive polynomial in the find-only-one-primitive-polynomial case, which is an error.
            if (!parser.list_all_primitive_polynomials_ && !is_primitive_poly)
            {
                ostringstream os ;
                os << "Tested all " << order.getMaxNumPoly() << " possible polynomials, but failed to find a primitive polynomial" ;
                throw PolynomialError( os.str() ) ;
            }
        }
        return static_cast<int>( ReturnStatus::Success ) ;
    }
    // Catch all exceptions and report what happened to the user.
    // First do the user-defined exceptions.
    catch( PrimpolyError & e )
    {
        cerr << "\nTop Level Error: " << " [ " << e.what() << " ] " << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    catch( ParserError & e )
    {
        cerr << "Inputs are incorrect or out of range: " << " [ " << e.what() << " ] " << endl;
        return static_cast<int>( ReturnStatus::RangeError ) ;
    }
    catch( FactorError & e )
    {
        cerr << "Error in prime factorization:  " << " [ " << e.what() << " ] " << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    catch( BigIntRangeError & e )
    {
        cerr << "Internal range error in multiple precision arithmetic:  " << " [ " << e.what() << " ] " << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    catch( BigIntDomainError & e )
    {
        cerr << "Internal domain error in multiple precision arithmetic:  " << " [ " << e.what() << " ] " << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    catch( BigIntUnderflow & e )
    {
        cerr << "Internal underflow error in multiple precision arithmetic:  " << " [ " << e.what() << " ] " << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    catch( BigIntOverflow & e )
    {
        cerr << "Internal overflow error in multiple precision arithmetic:  " << " [ " << e.what() << " ] " << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    catch( BigIntZeroDivide & e )
    {
        cerr << "Internal zero divide error in multiple precision arithmetic:  " << " [ " << e.what() << " ] " << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    catch( BigIntMathError & e )
    {
        cerr << "Internal math error in multiple precision arithmetic:  " << " [ " << e.what() << " ] " << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    catch( ArithModPError & e )
    {
        cerr << "Internal modulo p arithmetic error:  " << " [ " << e.what() << " ] " << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    catch( PolynomialRangeError & e )
    {
        cout << "Error.  Polynomial has bad syntax or coefficients are out of range. " << " [ " << e.what() << " ] " << endl;
        return static_cast<int>( ReturnStatus::RangeError ) ;
    }
    catch( PolynomialError & e )
    {
        cerr << "Error in polynomial arithmetic:  " << " [ " << e.what() << " ] " << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    //  Standard library exceptions.
    catch( bad_alloc & e )
    {
        cerr << "Error allocating memory:  " << " [ " << e.what() << " ] "<< endl ;
        cerr << "Run on a different computer with more RAM or virtual memory." << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    catch( exception & e )
    {
        cerr << "System error: " << e.what() << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }
    // Catch all other uncaught exceptions, which would otherwise call terminate()
    // which in turn calls abort() and which would halt this program.
    //
    // Limitations:
    //     We can't handle the case where terminate() gets called because the
    //     exception mechanism finds a corrupt stack or catches a destructor
    //     throwing an exception.
    // 
    //     Also we can't catch exceptions which are thrown by initializing or
    //     destructing global variables.
    catch( ... )
    {
        cerr << "Unexpected exception: " << endl << writeToAuthorMessage ;
        return static_cast<int>( ReturnStatus::InternalError ) ;
    }

    return static_cast<int>( ReturnStatus::Success ) ;
}
