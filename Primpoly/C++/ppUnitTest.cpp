#include <cstdlib>      // abort()
#include <iostream>     // Basic stream I/O.
#include <iomanip>      // I/O manipulators.
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
#include <array>        // array type.

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


#ifdef SELF_CHECK
#include "ppUnitTest.hpp"       // Complete unit test.

/*=============================================================================
 |
 | NAME
 |
 |     UnitTest
 |
 | DESCRIPTION
 |
 |     Constructor for the class with a default file name.
 |
 | EXAMPLE
 |
 |    try
 |    {
 |        UnitTest unitTest ;
 |        if (!unitTest.run())
 |            cerr << "Failed one or more unit tests!" << endl ;
 |    }
 |    catch( UnitTestError & e )
 |    {
 |        cerr << "Failed to run unit test suite at all!" << endl ;
 |        cerr << e.what() << endl ;
 |    }
 |
 +============================================================================*/

UnitTest::UnitTest( const char * fileName )
        : unit_test_log_file_name_( fileName )
{
    // Place results into a log file in the current directory.
    // If the file can't be opened, just print results to the console.
    fout_.open( unit_test_log_file_name_ ) ;
    
    // Test this section of code by making the existing file read only:
    //     chmod 000 unitTest.log
    if (!fout_)
    {
        cerr << "Unit test:  cannot open output log file " << unit_test_log_file_name_ << endl ;
        cerr << "Trying standard output to the console instead." << endl ;

        // Test this section of code by changing "/dev/stdout to "/dev/nonsensewonsense/stdout" and recompiling.
        fout_.open( "/dev/stdout", fstream::out ) ;

        if (fout_.fail())
        {
            ostringstream os ;
            os << "Unit test:  cannot open the output log file " << unit_test_log_file_name_ 
               << " and unable to log standard output to the console. "
               << "Skipping the unit test self check" ;
            throw UnitTestError( os.str(), __FILE__, __LINE__ ) ;
        }
    }

    fout_ << "\nBegin unit testing..." ;
}


/*=============================================================================
 |
 | NAME
 |
 |     ~UnitTest
 |
 | DESCRIPTION
 |
 |     Destructor which closes the file.
 |
 +============================================================================*/

UnitTest::~UnitTest()
{
    fout_.close() ;
}


/*=============================================================================
|
| NAME
|
|     run
|
| DESCRIPTION
|
|     Run all the unit tests.  Return the overall test status.
|
+============================================================================*/

bool UnitTest::run() 
{
    bool status = true ;

    try
    {
        // A C++ array is more lightweight and faster than a vector for this simple application.
        array<bool,8> unitTestStatus ;
        unitTestStatus.fill( true ) ;
        int i = 0 ;

        // Go thorough all the unit tests in order.
        unitTestStatus[i++] = unitTestSystemFunctions() ;
        unitTestStatus[i++] = unitTestBigIntBase10() ;
        unitTestStatus[i++] = unitTestBigIntDefaultBase() ;
        unitTestStatus[i++] = unitTestModPArithmetic() ;
        unitTestStatus[i++] = unitTestFactoring() ;
        unitTestStatus[i++] = unitTestPolynomials() ;
        unitTestStatus[i++] = unitTestPolynomialOrder() ;
        unitTestStatus[i++] = unitTestParser() ;

        // True only if every single test passes.
        for (i = 0 ;  i < unitTestStatus.size() ;  ++i)
            status = status && unitTestStatus[ i ] ;
    }
    // One or more unit tests might throw an exception unexpectedly.
    // This would be a bug since any unit test should catch and handle any exceptions it generates within
    // the test itself and convert to a proper test status.
    //
    // Catch blocks should be in order from more specific to less specific, i.e. up the class hierarchy.
    catch( PrimpolyError & e )
    {
        fout_ << ".........FAIL!\n    caught exception type  PrimpolyError: [ " << e.what() << " ] " << endl ;
        status = false ;
    }
    catch( ParserError & e )
    {
        fout_ << ".........FAIL!\n    caught exception type  ParserError: [ " << e.what() << " ] " << endl ;
        status = false ;
    }
    catch( FactorError & e )
    {
        fout_ << ".........FAIL!" << endl ;
        fout_ << "    caught FactorError: [ " << e.what() << " ] " << endl ;
        status = false ;
    }
    catch( BigIntRangeError & e )
    {
        fout_ << ".........FAIL!\n    caught exception type  BigIntRangeError: [ " << e.what() << " ] " << endl ;
        status = false ;
    }
    catch( BigIntDomainError & e )
    {
        fout_ << ".........FAIL!\n    caught exception type  BigIntDomainError: [ " << e.what() << " ] " << endl ;
        status = false ;
    }
    catch( BigIntUnderflow & e )
    {
        fout_ << ".........FAIL!\n    caught exception type  BigIntUnderflow: [ " << e.what() << " ] " << endl ;
        status = false ;
    }
    catch( BigIntOverflow & e )
    {
        fout_ << ".........FAIL!\n    caught exception type  BigIntOverflow: [ " << e.what() << " ] " << endl ;
        status = false ;
    }
    catch( BigIntZeroDivide & e )
    {
        fout_ << ".........FAIL!\n    caught exception type  BigIntZeroDivide: [ " << e.what() << " ] " << endl ;
        status = false ;
    }
    catch( BigIntMathError & e )
    {
        fout_ << ".........FAIL!\n    caught exception type  BigIntMathError: [ " << e.what() << " ] " << endl ;
        status = false ;
    }
    catch( ArithModPError & e )
    {
        fout_ << ".........FAIL!\n    caught exception type  ArithModPError: [ " << e.what() << " ] " << endl ;
        status = false ;
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << ".........FAIL!\n    caught exception type  PolynomialRangeError: [ " << e.what() << " ] " << endl ;
        status = false ;
    }
    catch( PolynomialError & e )
    {
        fout_ << ".........FAIL!\n    caught exception type  PolynomialError: [ " << e.what() << " ] " << endl ;
        status = false ;
    }
    //
    // Standard library exceptions.
    //
    catch( bad_alloc & e )
    {
        fout_ << ".........FAIL!\n    caught exception type  Caught bad_alloc exception: [ " << e.what() << " ] " << endl ;
        status = false ;
    }
    catch( length_error & e )
    {
        fout_ << ".........FAIL!\n    caught exception type  Caught length_error exception: [ " << e.what() << " ] " << endl ;
        status = false ;
    }
    catch( exception & e )
    {
        fout_ << ".........FAIL!\n    caught exception type  Standard library error: [ " << e.what() << " ] " << endl ;
        status = false ;
    }
    // Catch all other uncaught exceptions, which would otherwise call terminate()
    // which in turn calls abort() and which would halt this program.
    //
    // Limitations:
    // We can't handle the case where terminate() gets called because the
    // exception mechanism finds a corrupt stack or catches a destructor
    // throwing an exception.
    //
    // Also we can't catch exceptions which are thrown by initializing or
    // destructing global variables.
    catch( ... )
    {
        fout_ << ".........FAIL!\n    caught exception type  uncaught exception" << endl ;
        status = false ;
    }
    
    fout_ << "\nEnd unit testing..." ;
    
    if (status)
        fout_ << "\nCONGRATULATIONS!  All tests passed!" << endl ;
    else
        fout_ << "\nSORRY.  One or more unit tests failed!" << endl ;
    
    fout_.close() ;
    
    return status ;
}


bool UnitTest::unitTestSystemFunctions()
{
    bool status = true ;

    #ifdef DEBUG_PP_FORCE_MEMORY_OVERLOAD
    // Test resize() exceptions.
    fout_ << "\nTEST:  C++ resize a vector to > max_size dimensions.  Did resize throw a length_error exception?" ;

    //            30
    // Typically 2  - 1.
    vector<ppuint> testVector ;
    size_t maxSize = testVector.max_size() ;

    try
    {
        // Overload the vector.
        testVector.resize( maxSize + 1 ) ;

        fout_ << ".........FAIL!" << endl ;
        fout_ << "    resize did not throw an exception " << endl ;
        status = false ;
    }
    catch( length_error & e )
    {
        fout_ << ".........PASS!" ;
    }
    catch( ... )
    {
        fout_ << ".........FAIL!" << endl ;
        fout_ << "    resize did not throw a length_error exception" << endl ;
        status = false ;
    }
    #endif // DEBUG_PP_FORCE_MEMORY_OVERLOAD

    #ifdef DEBUG_PP_FORCE_MEMORY_OVERLOAD
    fout_ << "\nTEST:  C++ overload memory for a vector.  Did push_back throw an exception?" ;
    try
    {
        // Overload memory.
        for (int i = 1 ;  i <= maxSize + 1 ;  ++i)
            testVector.push_back( 1000 ) ;

        fout_ << ".........FAIL!" << endl ;
        fout_ << "    push_back did not throw an exception" << endl ;
        status = false ;
    }
    catch( bad_alloc & e )
    {
        fout_ << ".........PASS!" ;
    }
    catch( ... )
    {
        fout_ << ".........FAIL!" << endl ;
        fout_ << "    push_back did not throw a bad_alloc exception" << endl ;
        status = false ;
    }
    #endif // DEBUG_OVERLOADING_MEMORY

    return status ;
}



bool UnitTest::unitTestBigIntBase10()
{
    bool status = true ;

    // ------------------- Set to base 10 ----------------------
    ppuint oldBase = 0 ; // Save the old default base.
    {
        // Instantiate a BigInt object and set its to base 10 to change the base for all BigInts.
        BigInt w ;
        oldBase = BigInt::getBase() ;
        setBase( w, 10 ) ;
    }

    #ifdef DEBUG_PP_FORCE_UNIT_TEST_FAIL
    { BigInt dummy ; setBase( dummy, 11 ) ; }
    #endif

    fout_ << "\nTEST:  BigInt switching from base = " << oldBase << " to new base = " << 10 ;
    {
        // Check all BigInts have the new base of 10.
        if (BigInt::getBase() != 10)
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    Current base is not 10 but rather = " << BigInt::getBase() << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt u default constructor which gives u = 0." ;
    {
        BigInt u ;
        if (getNumDigits(u) != 0)
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    u = " ; printNumber( u, fout_ ) ; fout_ << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  Constructor BigInt u( d ) from ppuint d = 1234" ;
    {
        ppuint d{ 1234 } ;
        BigInt u( d ) ;
        if (getNumDigits( u ) == 4 && getDigit( u, 3 ) == 1 && getDigit( u, 2 ) == 2 && getDigit( u, 1 ) == 3 && getDigit( u, 0 ) == 4)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    u = " ; printNumber( u, fout_ ) ; fout_ << endl ;
            status = false ;
        }
    }

    fout_ << "\nTEST:  Constructor BigInt u( s ) from string s = \"1234\"" ;
    {
        string s = "1234" ;
        BigInt u( s ) ;
        if (getNumDigits( u ) == 4 && getDigit( u, 3 ) == 1 && getDigit( u, 2 ) == 2 && getDigit( u, 1 ) == 3 && getDigit( u, 0 ) == 4)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    u = " ; printNumber( u, fout_ ) ; fout_ << endl ;
            status = false ;
        }
    }

    fout_ << "\nTEST:  Constructor BigInt u( s ) from INVALID string s = \"12x34\"" ;
    try
    {
        string s = "12x34" ;
        BigInt u( s ) ;
        fout_ << ".........FAIL!" << endl ;
        fout_ << "    u = " ; printNumber( u, fout_ ) ; fout_ << endl ;
        status = false ;
    }
    catch( BigIntRangeError & e )
    {
        fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  Copy constructor BigInt v( u ) from BigInt u( 123 )" ;
    {
        BigInt u( 123u ) ;
        BigInt v( u ) ;

        if (getNumDigits( u ) != getNumDigits( v ) ||
            getDigit( u, 0 ) != getDigit( v, 0 ) ||
            getDigit( u, 1 ) != getDigit( v, 1 ) ||
            getDigit( u, 2 ) != getDigit( v, 2 ))
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << " u = " ;  printNumber( u, fout_ ) ; fout_ << endl ;
            fout_ << " v = " ;  printNumber( v, fout_ ) ; fout_ << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  Assignment operator v = u from BigInt v and BigInt u( 123 )" ;
    {
        BigInt u( 123u ) ;
        BigInt v ; // Don't use BigInt v = u because that would call the copy constructor.
        v = u ;

        if (getNumDigits( u ) != getNumDigits( v ) ||
            getDigit( u, 0 ) != getDigit( v, 0 ) ||
            getDigit( u, 1 ) != getDigit( v, 1 ) ||
            getDigit( u, 2 ) != getDigit( v, 2 ))
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    u = " ; printNumber( u, fout_ ) ; fout_ << endl ;
            fout_ << "    v = " ; printNumber( v, fout_ ) ; fout_ << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  Implicit casting ppuint d = u from BigInt u( \"01234\" )" ;
    {
        BigInt u( "01234" ) ;
        ppuint d = u ;
        if (d != static_cast<ppuint>(1234u))
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << " u = " ;  printNumber( u, fout_ ) ; fout_ << endl ;
            fout_ << " d = "  << d << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;

        fout_ << "\nTEST:  Explicit casting static_cast< ppuint >( u ) from BigInt u( \"01234\" )" ;
        if (static_cast< ppuint >( u ) != static_cast< ppuint >( 1234u ))
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    u = " << endl ; printNumber( u, fout_ ) ; fout_ << endl ;
            fout_ << "    static_cast< ppuint > u  = "  << static_cast< ppuint >( u ) << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  Check overflow during ui = static_cast< ppuint >(u) on BigInt u( \"3141592653589793238462643383279\" )" ;
    try
    {
        BigInt u( "3141592653589793238462643383279" ) ;
        ppuint ui{ static_cast<ppuint>(u) } ;

        fout_ << ".........FAIL!" << endl ;
        fout_ << "    Casting BigInt to ppuint didn't overflow." << endl ;
        fout_ << "    u = " ;  printNumber( u, fout_ ) ;  fout_ << endl ;
        fout_ << "    ui = " << ui << endl ;
        status = false ;
    }
    catch( BigIntOverflow & e )
    {
        // Should overflow!
        fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  Stream output os << u from BigInt u( \"1234567890\" )" ;
    {
        BigInt u( "1234567890" ) ;
        ostringstream os ;
        os << u ;
        if (os.str() != "1234567890")
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    u = |" << u << "|" << endl ;
            fout_ << "    os.str() = |" << os.str() << "|" << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  Stream input is >> u for BigInt u where we've loaded the stream is.str( \"314159265358979323846264\" )" ;
    {
        BigInt u ;
        istringstream is ;
        is.clear() ;
        is.str( "314159265358979323846264" ) ;
        is >> u ;

        // Test by streaming out the BigInt and checking its string value.
        ostringstream os ;
        os << u ;
        if (os.str() != "314159265358979323846264" )
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    BigInt = |" << u << "| is.str() = |" << is.str() << "|" << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  Equality test BigInt u == ppuint d?" ;
    {
        BigInt u( "9" ) ;
        ppuint d{ 9 } ;
        if (u == d)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    u = " ; printNumber( u, fout_ ) ; fout_ << endl ;
            fout_ << "    d = " << d << endl ;
            status = false ;
        }
    }

    fout_ << "\nTEST:  Equality test BigInt u == BigInt v" ;
    {
        BigInt u( "1234" ) ;
        BigInt v( "1234" ) ;
        if (u == v)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    u = " ; printNumber( u, fout_ ) ; fout_ << endl ;
            fout_ << "    v = " ; printNumber( v, fout_ ) ; fout_ << endl ;
            status = false ;
        }
    }

    fout_ << "\nTEST:  BigInt u > BigInt v" ;
    {
        BigInt u( "3844035" ) ;
        BigInt v( "933134" ) ;

        if (u > v)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    u = " ; printNumber( u, fout_ ) ; fout_ << endl ;
            fout_ << "    v = " ; printNumber( v, fout_ ) ; fout_ << endl ;
            status = false ;
        }
    }

    fout_ << "\nTEST:  BigInt u( \"1234\" ) -= ppuint d" ;
    try
    {
        BigInt u( "1234" ) ;
        ppuint d = static_cast<ppuint>(5u) ;
        u -= d ;

        if (getNumDigits( u ) != static_cast<ppuint>(4) ||
            getDigit( u, 3 )  != static_cast<ppuint>(1) || getDigit( u, 2 ) != static_cast<ppuint>(2) ||
            getDigit( u, 1 )  != static_cast<ppuint>(2) || getDigit( u, 0 ) != static_cast<ppuint>(9)
           )
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    u = " ; printNumber( u, fout_ ) ; fout_ << endl ;
            fout_ << "    d = " << d << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }
    catch( BigIntMathError & e )
    {
        fout_ << ".........FAIL!" << endl ;
        fout_ << "BigIntMathError: [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  BigInt u -= static_cast<ppuint>(5) underflow" ;
    {
        try
        {
            BigInt u( "4" ) ;
            ppuint d = static_cast<ppuint>(5u) ;
            u -= d ;

            fout_ << ".........FAIL!" << endl ;
            fout_ << "    u = " ; printNumber( u, fout_ ) ; fout_ << endl ;
            fout_ << "    d = " << d << endl ;
            status = false ;
        }
        catch( BigIntUnderflow & e )
        {
           // Caught underflow;  works correctly.
           // fout_ << "Underflow: " << e.what() << endl ;
            fout_ << ".........PASS!" ;
        }
        catch( ... )
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    Didn't catch a BigIntUnderflow exception" << endl ;
            status = false ;
        }
    }

    fout_ << "\nTEST:  BigInt u += ppuint d" ;
    {
        BigInt u( "9994" ) ;
        ppuint d = static_cast<ppuint>(6u) ;
        u += d ;
        if (getNumDigits( u ) != 5 ||
            getDigit( u, 4 )  != 1 ||
            getDigit( u, 3 )  != 0 || getDigit( u, 2 ) != 0 ||
            getDigit( u, 1 )  != 0 || getDigit( u, 0 ) != 0
           )
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    u = " ; printNumber( u, fout_ ) ; fout_ << endl ;
            fout_ << "    d = " << d << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt v = BigInt u * ppuint d" ;
    {
        BigInt u( "123" ) ;
        ppuint d{ 4 } ;
        BigInt v = u * d ;

        if (getNumDigits( v ) != 3 ||
            getDigit( v, 2 ) != 4 ||
            getDigit( v, 1 ) != 9 || getDigit( v, 0 ) != 2
           )
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    u = " ; printNumber( u, fout_ ) ; fout_ << endl ;
            fout_ << "    v = " ; printNumber( v, fout_ ) ; fout_ << endl ;
            fout_ << "    d = " << d << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt u /= ppuint d" ;
    {
        BigInt u( "12" ) ;
        ppuint d{ 4 } ;
        u /= d ;

        if (getNumDigits( u ) != 1 || getDigit( u, 0 ) != 3)
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    u = " ; printNumber( u, fout_ ) ; fout_ << endl ;
            fout_ << "    d = " << d << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt u /= ppuint d underflow to zero." ;
    {
        BigInt u( "3" ) ;
        ppuint d{ 4 } ;
        u /= d ;

        if (getNumDigits( u ) != 1 || getDigit( u, 0 ) != 0)
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    u = " ; printNumber( u, fout_ ) ; fout_ << endl ;
            fout_ << "    d = " << d << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt v = ++BigInt u" ;
    {
        BigInt u( "123" ) ;
        BigInt v = ++u ;

        if (getNumDigits( u ) != 3 || getDigit( u, 2 ) != 1 || getDigit( u, 1 )  != 2 || getDigit( u, 0 ) != 4
           || (u != v))
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    u = " ; printNumber( u, fout_ ) ; fout_ << endl ;
            fout_ << "    v = " ; printNumber( v, fout_ ) ; fout_ << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt v = --BigInt u" ;
    {
        BigInt u( "123" ) ;
        BigInt v = --u ;

        if (getNumDigits( u ) != 3 ||
            getDigit( u, 2 ) != 1 || getDigit( u, 1 )  != 2 || getDigit( u, 0 ) != 2 ||
            (u != v))
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    u = " ; printNumber( u, fout_ ) ; fout_ << endl ;
            fout_ << "    v = " ; printNumber( v, fout_ ) ; fout_ << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt++" ;
    {
        BigInt u( "123" ) ;
        BigInt v = u++ ;

        if (getNumDigits( u ) != 3 ||
            getDigit( u, 2 ) != 1 || getDigit( u, 1 )  != 2 || getDigit( u, 0 ) != 4
           )
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    ++BigInt failed." << endl ;
            printNumber( u, fout_ ) ;
            status = false ;
        }
        else if (getNumDigits( v ) != 3 ||
            getDigit( v, 2 ) != 1 || getDigit( v, 1 )  != 2 || getDigit( v, 0 ) != 3
           )
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    BigInt++ failed." << endl ;
            printNumber( u, fout_ ) ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt--" ;
    {
        BigInt u( "123" ) ;
        BigInt v = u-- ;

        if (getNumDigits( u ) != 3 ||
            getDigit( u, 2 ) != 1 || getDigit( u, 1 )  != 2 || getDigit( u, 0 ) != 2
           )
        {
            fout_ << "\n\tERROR:  BigInt-- failed." << endl ;
            printNumber( u, fout_ ) ;
            status = false ;
        }
        else if (getNumDigits( v ) != 3 ||
            getDigit( v, 2 ) != 1 || getDigit( v, 1 )  != 2 || getDigit( v, 0 ) != 3
           )
        {
            fout_ << "\n\tERROR:  BigInt-- failed." << endl ;
            printNumber( u, fout_ ) ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  one digit BigInt + ppuint" ;
    {
        BigInt u( "3" ) ;
        ppuint d{ 4 } ;
        BigInt w = u + d ;

        if (getNumDigits( w ) != 1 || getDigit( w, 0 ) != 7)
        {
            fout_ << "\n\tERROR:  BigInt + BigInt 3 + 4 = 7 failed." << endl ;
            printNumber( w, fout_ ) ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  two digit BigInt + ppuint" ;
    {
        BigInt u( "3" ) ;
        ppuint d{ 9 } ;
        BigInt w = u + d ;

        if (getNumDigits( w ) != 2 || getDigit( w, 1 ) != 1
                                   || getDigit( w, 0 ) != 2)
        {
            fout_ << "\n\tERROR:  BigInt + BigInt 3 + 9 = 12 failed." << endl ;
            printNumber( w, fout_ ) ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt + BigInt" ;
    {
        BigInt u( "9999" ) ;
        BigInt v(  "999" ) ;
        BigInt w = u + v ;

        if (getNumDigits( w ) != 5 || getDigit( w, 4 ) != 1
                                   || getDigit( w, 3 ) != 0
                                   || getDigit( w, 2 ) != 9
                                   || getDigit( w, 1 ) != 9
                                   || getDigit( w, 0 ) != 8)
        {
            fout_ << "\n\tERROR:  BigInt + BigInt 9999 + 999 = 10998 failed." << endl ;
            printNumber( w, fout_ ) ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt + BigInt" ;
    {
        BigInt u(  "999" ) ;
        BigInt v( "9999" ) ;
        BigInt w = u + v ;

        if (getNumDigits( w ) != 5 || getDigit( w, 4 ) != 1
                                   || getDigit( w, 3 ) != 0
                                   || getDigit( w, 2 ) != 9
                                   || getDigit( w, 1 ) != 9
                                   || getDigit( w, 0 ) != 8)
        {
            fout_ << "\n\tERROR:  BigInt + BigInt 999 + 9999 = 10998 failed." << endl ;
            printNumber( w, fout_ ) ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt - BigInt" ;
    {
        BigInt u( "103" ) ;
        BigInt v(   "9" ) ;
        BigInt w = u - v ;

        if (getNumDigits( w ) != 2 || getDigit( w, 1 ) != 9
                                   || getDigit( w, 0 ) != 4)
        {
            fout_ << "\n\tERROR:  BigInt - BigInt 103 - 9 = 94 failed." << endl ;
            printNumber( w, fout_ ) ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt - BigInt < 0" ;
    try
    {
        BigInt u( "9" ) ;
        BigInt v( "103" ) ;
        BigInt w = u - v ;

        fout_ << "\n\tERROR:  BigInt - BigInt 9 - 103 failed didn't catch range exception." << endl ;
        printNumber( w, fout_ ) ;
        status = false ;
    }
    catch( BigIntUnderflow & e )
    {
        // Caught underflow;  works correctly.
        // fout_ << "Underflow: " << e.what() << endl ;
        fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt - ppuint" ;
    {
        BigInt u( "103" ) ;
        ppuint d{ 9 } ;
        BigInt w = u - d ;

        if (getNumDigits( w ) != 2 || getDigit( w, 1 ) != 9
                                   || getDigit( w, 0 ) != 4)
        {
            fout_ << "\n\tERROR:  BigInt - ppuint 103 - 9 = 94 failed." << endl ;
            printNumber( w, fout_ ) ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  one digit BigInt * BigInt" ;
    {
        BigInt u( "3" ) ;
        BigInt v( "3" ) ;
        BigInt w = u * v ;

        if (getNumDigits( w ) != 1 || getDigit( w, 0 ) != 9)
        {
            fout_ << "\n\tERROR:  BigInt * BigInt 3 * 3 = 9 failed." << endl ;
            printNumber( w, fout_ ) ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  two digit BigInt * BigInt" ;
    {
        BigInt u( "3" ) ;
        BigInt v( "4" ) ;
        BigInt w = u * v ;

        if (getNumDigits( w ) != 2 || getDigit( w, 1 ) != 1 || getDigit( w, 0 ) != 2)
        {
            fout_ << "\n\tERROR:  BigInt * BigInt = 3 * 4 = 12 failed." << endl ;
            printNumber( w, fout_ ) ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt multidigit *" ;
    {
        BigInt w ;
        BigInt u( "329218104" ) ;
        BigInt v(      "3606" ) ;

        w = u * v ;

        string s = w.toString() ;

        if (s != "1187160483024" )
        {
            fout_ << "\n\tERROR:  BigInt multidigit * failed." << endl ;
            fout_ << "u = " ; printNumber( u, fout_ ) ;
            fout_ << "v = " ; printNumber( v, fout_ ) ;
            fout_ << "w = " ; printNumber( w, fout_ ) ;
            status = false ;
        }
        else
        {
            fout_ << ".........PASS!" ;
        }
    }

    fout_ << "\nTEST:  BigInt multidigit *=" ;
    {
        BigInt u( "329218104" ) ;
        BigInt v(      "3606" ) ;

        u *= v ;

        string s = u.toString() ;

        if (s != "1187160483024" )
        {
            fout_ << "\n\tERROR:  BigInt multidigit *= failed." << endl ;
            fout_ << "u = " ; printNumber( u, fout_ ) ;
            fout_ << "v = " ; printNumber( v, fout_ ) ;
            status = false ;
        }
        else
        {
            fout_ << ".........PASS!" ;
        }
    }

    fout_ << "\nTEST:  BigInt / BigInt one digit divisor." ;
    {
        BigInt u( "12" ) ;
        BigInt v( "4" ) ;
        BigInt w = u / v ;

        if (getNumDigits( w ) != 1 || getDigit( w, 0 ) != 3)
        {
            fout_ << "\n\tERROR:  BigInt / BigInt = 12/4 = 3 failed." << endl ;
            printNumber( w, fout_ ) ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt / BigInt multidigit" ;
    {
        BigInt u( "398765" ) ;
        BigInt v( "3457" ) ;
        BigInt w = u / v ;

        if (getNumDigits( w ) != 3 ||
                getDigit( w, 2 ) != 1 || getDigit( w, 1 ) != 1 || getDigit( w, 0 ) != 5)
        {
            fout_ << "\n\tERROR:  BigInt / BigInt = 398765/3457 = 215 failed." << endl ;
            printNumber( w, fout_ ) ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt / BigInt leading zero digit." ;
    {
        BigInt u( "120" ) ;
        BigInt v( "40" ) ;
        BigInt w = u / v ;

        if (getNumDigits( w ) != 1 || getDigit( w, 0 ) != 3)
        {
            fout_ << "\n\tERROR:  BigInt / BigInt = 120/40 = 3 failed." << endl ;
            printNumber( w, fout_ ) ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt / 0 " ;
    try
    {
        BigInt u( "120" ) ;
        BigInt v( "0" ) ;
        BigInt w = u / v ;

        fout_ << "\n\tERROR:  BigInt / 0 = 120/0 failed." << endl ;
        status = false ;
    }
    catch( BigIntZeroDivide & e )
    {
        // Should catch zero divide here.
        fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt % BigInt with u > v" ;
    {
        BigInt u( "398765" ) ;
        BigInt v( "3457" ) ;
        BigInt r = u % v ;

        if (getNumDigits( r ) != 4 ||
                getDigit( r, 3 ) != 1 || getDigit( r, 2 ) != 2 ||
                getDigit( r, 1 ) != 1 || getDigit( r, 0 ) != 0)
        {
            fout_ << "\n\tERROR:  BigInt % BigInt = 398765 / 3457 = 1210 failed." << endl ;
            printNumber( r, fout_ ) ;
            status = false ;
        }
        else
        {
            fout_ << ".........PASS!" ;
        }
    }

    fout_ << "\nTEST:  BigInt multidigit mod with normalizing constant d = 1" ;
    {
        BigInt w ;
        BigInt u( "1369244731822264511994463394" ) ;
        BigInt v(  "954901783703457032047844259" ) ;

        w = u % v ;

        string s{ w.toString() } ;

        if (s != "414342948118807479946619135" )
        {
            fout_ << "\n\tERROR:  BigInt multidigit mod failed." << endl ;
            fout_ << "u = " ; printNumber( u, fout_ ) ;
            fout_ << "v = " ; printNumber( v, fout_ ) ;
            fout_ << "w = " ; printNumber( w, fout_ ) ;
            status = false ;
        }
        else
        {
            fout_ << ".........PASS!" ;
        }
    }

    fout_ << "\nTEST:  BigInt % BigInt with u < v" ;
    {
        BigInt u( "12" ) ;
        BigInt v( "34567" ) ;
        BigInt r{ u % v } ;

        if (getNumDigits( r ) != 2 ||
            getDigit( r, 1 ) != 1 || getDigit( r, 0 ) != 2)
        {
            fout_ << "\n\tERROR:  BigInt % BigInt = 12 mod 345 = 12 failed." << endl ;
            printNumber( r, fout_ ) ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt % ppuint = 314159 / 9 = 5 with ppuint < base " ;
    {
        BigInt u( "314159" ) ;
        ppuint v{ 9u } ;
        ppuint r{ u % v } ;

        if (r != 5)
        {
            fout_ << "\n\tERROR:  [BigInt u % ppuint v = r]:  314159 / 9 = 5 failed" << endl ;
            fout_ << "u = " << u << " v = " << v << " r = " << r << " for base = " << BigInt::getBase() << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt % ppuint = 398765 % 11u with ppuint > base = 10 throws error? " ;
    try
    {
        BigInt u( "398765" ) ;
        ppuint v{ 11u } ;
        ppuint r{ u % v } ;

        fout_ << "\n\tERROR:  BigInt % ppuint = 398765 % 11 = 4 failed for base = " << BigInt::getBase() << endl ;
        fout_ << "\n\tERROR:  [BigInt u % ppuint v = r]:  398765 / 11 = 4 failed" << endl ;
        fout_ << "u = " << u << " v = " << v << " r = " << r << " for base = " << BigInt::getBase() << endl ;
        status = false ;
    }
    catch( BigIntOverflow & e )
    {
        fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt / BigInt low probability if branch." ;
    {
        BigInt u( "4100" ) ;
        BigInt v( "588" ) ;
        BigInt w{ u / v } ;

        if (w != BigInt("6"))
            fout_ << "error" << endl;

        if (getNumDigits( w ) != 1 || getDigit( w, 0 ) != 6)
        {
            fout_ << "\n\tERROR:  BigInt / BigInt = 4100/588 = 6 failed." << endl ;
            printNumber( w, fout_ ) ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  Switching back from base " << 10 << " to oldBase " << oldBase ;
    BigInt dummy ;
    setBase( dummy, oldBase ) ;
    if (BigInt::getBase() != oldBase)
    {
        fout_ << "\n\tERROR: Changing back to default base for all BigInt objects "
             << "       failed.  base = " << BigInt::getBase() << endl ;
        status = false ;
    }
    else
        fout_ << ".........PASS!" ;

    return status ;
}



bool UnitTest::unitTestBigIntDefaultBase()
{
    bool status{ true } ;

    fout_ << "\nTEST:  Decimal string to BigInt conversion and back to decimal string using default base." ;
    {
        BigInt x( "3141592653589793238462643383279" ) ;
        string s{ x.toString() } ;

        if (s != "3141592653589793238462643383279" )
        {
            fout_ << "\n\tERROR:  BigInt default base conversion failed." << endl ;
            fout_ << "x = " << x << " " ; printNumber( x, fout_ ) ;
            fout_ << " NOT EQUAL TO s = 3141592653589793238462643383279 (decimal digits)" << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt z = x * y then x =? z / y multidigit with default base." ;
    {
        BigInt x( "3141592653589793238462643383279" ) ;
        BigInt y( "2718281828459045" ) ;
        BigInt z{ x * y } ;
        BigInt w{ z / y } ;

        if (w != x)
        {
            fout_ << "\n\tERROR:  BigInt z = x * y then x =? z / y multidigit with default base failed." << endl ;
            fout_ << "x = " ; printNumber( x, fout_ ) ;
            fout_ << "y = " ; printNumber( y, fout_ ) ;
            fout_ << "z = " ; printNumber( z, fout_ ) ;
            fout_ << "w = " ; printNumber( w, fout_ ) ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt testBit" ;
    {
        BigInt u( "31415926535897932" ) ;

        if (u.testBit( 0 ) == false  &&
            u.testBit( 1 ) == false  &&
            u.testBit( 2 ) == true   &&
            u.testBit( 3 ) == true   &&
            u.testBit( 4 ) == false  &&
            u.testBit( 5 ) == false  &&
            u.testBit( 6 ) == true   &&
            u.testBit( 7 ) == false)
        {
            fout_ << ".........PASS!" ;
        }
        else
        {
            fout_ << "\n\tERROR:  BigInt testBit failed." << endl ;
            printNumber( u, fout_ ) ;
            fout_ << "testBit 0 = " << (u.testBit( 0 ) == true) << endl ;
            fout_ << "testBit 1 = " << (u.testBit( 1 ) == true) << endl ;
            fout_ << "testBit 2 = " << (u.testBit( 2 ) == true) << endl ;
            fout_ << "testBit 3 = " << (u.testBit( 3 ) == true) << endl ;
            fout_ << "testBit 4 = " << (u.testBit( 4 ) == true) << endl ;
            fout_ << "testBit 5 = " << (u.testBit( 5 ) == true) << endl ;
            fout_ << "testBit 6 = " << (u.testBit( 6 ) == true) << endl ;
            fout_ << "testBit 7 = " << (u.testBit( 7 ) == true) << endl ;
            status = false ;
        }
    }

    fout_ << "\nTEST:  testBit()" ;
    {
        ppuint u{ 0b100101 } ; // Set bits 0, 2 and 5 using C++14 binary literals.

        if (testBit( u, 0 ) == true   &&
            testBit( u, 1 ) == false  &&
            testBit( u, 2 ) == true   &&
            testBit( u, 3 ) == false  &&
            testBit( u, 4 ) == false  &&
            testBit( u, 5 ) == true   &&
            testBit( u, 6 ) == false  &&
            testBit( u, 7 ) == false)
        {
            fout_ << ".........PASS!" ;
        }
        else
        {
            fout_ << "\n\tERROR:  ppuint testBit failed for u = " << u << endl ;
            fout_ << "testBit 0 = " << (testBit( u, 0 ) == true) << endl ;
            fout_ << "testBit 1 = " << (testBit( u, 1 ) == true) << endl ;
            fout_ << "testBit 2 = " << (testBit( u, 2 ) == true) << endl ;
            fout_ << "testBit 3 = " << (testBit( u, 3 ) == true) << endl ;
            fout_ << "testBit 4 = " << (testBit( u, 4 ) == true) << endl ;
            fout_ << "testBit 5 = " << (testBit( u, 5 ) == true) << endl ;
            fout_ << "testBit 6 = " << (testBit( u, 6 ) == true) << endl ;
            fout_ << "testBit 7 = " << (testBit( u, 7 ) == true) << endl ;
            status = false ;
        }
    }

    fout_ << "\nTEST:  BigInt power( ppuint 2, ppuint 100 )" ;
    {
        ppuint p{ 2u } ;
        int n{ 100 } ;
        BigInt u{ power( p, n ) } ;
        string s{ u.toString() } ;

        BigInt v{ 1u } ;
        for (int i = 1 ; i <= n ; ++i)
            v *= p ;
        string sv{ v.toString() } ;

        if (s != sv)
        {
            fout_ << "\n\tERROR:  BigInt power( 2, 100 ) = " << u << endl ;
            fout_ << "correct answer = " << v << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt ceilLg( 6 )" ;
    {
        BigInt u{ 6u } ;

        int ceilingOfLog2 = u.ceilLg() ;
        if (ceilingOfLog2 != 3)
        {
            fout_ << "\n\tERROR:  BigInt ceilingOfLog2( 6 ) = " << ceilingOfLog2 << endl ;
            fout_ << "correct answer = 3" << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt eval 2 ^ 1198 - 1" ;
    {
        BigInt largePowerOf2Minus1 = power( 2, 1198 ) - static_cast< BigInt >( 1u ) ;
        /// cout << "largePowerOf2Minus1 = " << largePowerOf2Minus1 << endl ;
        BigInt f1{ BigInt( 3u ) } ;
        BigInt f2{ BigInt( "366994123" ) } ;
        BigInt f3{ BigInt( "16659379034607403556537" ) } ;
        BigInt f4{ BigInt( "148296291984475077955727317447564721950969097" ) } ;
        BigInt f5{ BigInt( "839804700900123195473468092497901750422530587828620063507554515144683510250490874819119570309824866293030799718783" ) } ;
        BigInt f6{ BigInt( "188446049896780543200161267236930710150747483597643192594833338774867012035362945326134784314021280857050576738677"
            "1290423087216156597588216186445958479269565424431335013281" ) } ;
        BigInt product{ f1 * f2 * f3 * f4 * f5 * f6 } ;
        bool allFactorsPrime{ isAlmostSurelyPrime( f1 ) && isAlmostSurelyPrime( f2 ) && isAlmostSurelyPrime( f3 ) &&
                              isAlmostSurelyPrime( f4 ) && isAlmostSurelyPrime( f5 ) && isAlmostSurelyPrime( f6 ) } ;
        /// cout << "all factors prime = " << allFactorsPrime << endl ;
        /// cout << "product = " << product << endl ;

        if (product != largePowerOf2Minus1 || !allFactorsPrime)
        {
            fout_ << "\nERROR:  BigInt eval 2 ^ 1198 - 1 != 3 * 366994123 * 16659379034607403556537 * 148296291984475077955727317447564721950969097 * "
            "839804700900123195473468092497901750422530587828620063507554515144683510250490874819119570309824866293030799718783 * "
            "18844604989678054320016126723693071015074748359764319259483333877486701203536294532613478431402128085705057673867712"
            "90423087216156597588216186445958479269565424431335013281" << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    return status ;
}


bool UnitTest::unitTestModPArithmetic()
{
    volatile bool status{ true } ; // Suppress XCode warning about Dead store: value never read.

    fout_ << "\nTEST:  ModP 10 = 3 (mod 7)" ;
    {
        ModP<ppuint,ppsint> modp( 7 ) ;
        if (modp( 10 ) != 3)
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    ModP modp( 7 );  modp( 10 ) = " << modp( 10 ) << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  ModP -10 = 4 (mod 7)" ;
    {
        ModP<ppuint,ppsint> modp( 7 ) ;
        if (modp( -10 ) != 4)
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    ModP modp( 7 );  modp( -10 ) = " << modp( -10 ) << endl ;
            ppsint n = -10 ;
            ppsint p = 7 ;
            fout_ << "+ + + + + + + +" << endl ;
            fout_ <<   n << endl ;
            fout_ <<   p << endl ;
            fout_ <<   (n % p) << endl ;
            fout_ << ( (n % p) + p ) << endl ;
            fout_ << "+ + + + + + + +" << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }
    
    fout_ << "\nTEST:  ModP( 0 ) throwing ArithModPError" ;
    {
        try
        {
            ModP<ppuint,ppsint> modp( 0 ) ;
            modp( 10 ) ;
                
            status = false ;
            fout_ << ".........FAIL!\n    did not catch ArithModPError when p <= 0" << endl ;
        }
        catch( ArithModPError & e )
        {
            // Since we cannot debug trace exceptions in XCode, break on this statement so we can see it works OK.
            fout_ << ".........PASS!" ;
        }
    }
    
    fout_ << "\nTEST:  ppuint gcd( 85, 25 ) = 5" ;
    {
        ppuint u{ 85u } ;
        ppuint v{ 25u } ;
        ppuint g{ gcd( u, v ) } ;
        if (g != static_cast<ppuint>(5u))
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    ppuint gcd( 85, 25 ) = " << g << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  BigInt gcd( 779953197883173551166308319545, 1282866356929526866866376009397 ) = 1" ;
    {
        BigInt u( "779953197883173551166308319545" ) ;
        BigInt v( "1282866356929526866866376009397" ) ;
        BigInt g = gcd( u, v ) ;
        if (g != static_cast<BigInt>( static_cast<ppuint>(1u) ))
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    BigInt gcd = " << g << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  c,r = addMod( a, b, n ) for ppuint type " ;
    {
        if (8 * sizeof( ppuint ) == 64)
        {
            fout_ << "of 64 bits " ;
            ppuint a = static_cast<ppuint>(18446744073709551614ULL) ; // 2^64-1-1
            ppuint b = static_cast<ppuint>(18446744073709551615ULL) ; // 2^64-1
            ppuint n = static_cast<ppuint>(18446744073709551615ULL) ; // 2^64-1
            ppuint c = static_cast<ppuint>(18446744073709551614ULL) ; // 2^64-1-1
            ppuint r = addMod( a, b, n ) ;
            if (r != c)
            {
                fout_ << ".........FAIL!" << endl ;
                fout_ << "a = " << a << endl ;
                fout_ << "b = " << b << endl ;
                fout_ << "n = " << n << endl ;
                fout_ << "c = " << c << endl ;
                fout_ << "r = " << r << endl ;
                status = false ;
            }
            else
                fout_ << ".........PASS!" ;
        }           
        else if (8 * sizeof( ppuint ) == 32)
        {
            fout_ << "of 32 bits " ;
            ppuint a{ static_cast<ppuint>(4294967295u) } ; // 2^32-1
            ppuint b{ static_cast<ppuint>(4294967294u) } ; // (2^32-1)-1
            ppuint n{ static_cast<ppuint>(4294967295u) } ; // 2^32-1
            ppuint c{ static_cast<ppuint>(4294967294u) } ; // (2^32-1)-1
            ppuint r{ addMod( a, b, n ) } ;
            if (r != c)
            {
                fout_ << ".........FAIL!" << endl ;
                fout_ << "a = " << a << endl ;
                fout_ << "b = " << b << endl ;
                fout_ << "n = " << n << endl ;
                fout_ << "c = " << c << endl ;
                fout_ << "r = " << r << endl ;
                status = false ;
            }
            else
                fout_ << ".........PASS!" ;
        }
    }

    fout_ << "\nTEST:  c,r = timesTwoMod( a, n ) for ppuint type " ;
    {
        if (8 * sizeof( ppuint ) == 64)
        {
            fout_ << "of 64 bits " ;
            ppuint a = static_cast<ppuint>(18446744073709551614ULL) ; // 2^64-1-1
            ppuint n = static_cast<ppuint>(18446744073709551615ULL) ; // 2^64-1
            ppuint c = static_cast<ppuint>(18446744073709551613ULL) ; // 2^64-1-2
            ppuint r = timesTwoMod( a, n ) ;
            if (r != c)
            {
                fout_ << ".........FAIL!" << endl ;
                fout_ << "a = " << a << endl ;
                fout_ << "n = " << n << endl ;
                fout_ << "c = " << c << endl ;
                fout_ << "r = " << r << endl ;
                status = false ;
            }
            else
                fout_ << ".........PASS!" ;
        }
        else if (8 * sizeof( ppuint ) == 32) 
        {
            fout_ << "of 32 bits " ;
            ppuint a{ static_cast<ppuint>(4294967294u) } ; // 2^32-1-1
            ppuint n{ static_cast<ppuint>(4294967295u) } ; // 2^32-1
            ppuint c{  static_cast<ppuint>(4294967293u) } ; // (2^32-1)-2
            ppuint r{ timesTwoMod( a, n ) } ;
            if (r != c)
            {
                fout_ << ".........FAIL!" << endl ;
                fout_ << "a = " << a << endl ;
                fout_ << "n = " << n << endl ;
                fout_ << "c = " << c << endl ;
                fout_ << "r = " << r << endl ;
                status = false ;
            }
            else
                fout_ << ".........PASS!" ;
        }
    }
    
    fout_ << "\nTEST:  c,r = multiplyMod( a, b, n ) for ppuint type " ;
    {
        if (8 * sizeof( ppuint ) == 64)
        {
            fout_ << "of 64 bits " ;
            ppuint a{ static_cast<ppuint>(18446744073709551614ULL) } ; // 2^64-1-1
            ppuint b{ static_cast<ppuint>(18446744073709551614ULL) } ; // 2^64-1-1
            ppuint n{ static_cast<ppuint>(18446744073709551615ULL) } ; // 2^64-1
            ppuint c{ static_cast<ppuint>(1ULL) } ;                    // 1
            ppuint r{ multiplyMod( a, b, n ) } ;
            if (r != c)
            {
                fout_ << ".........FAIL!" << endl ;
                fout_ << "a = " << a << endl ;
                fout_ << "b = " << b << endl ;
                fout_ << "n = " << n << endl ;
                fout_ << "c = " << c << endl ;
                fout_ << "r = " << r << endl ;
                status = false ;
            }
            else
                fout_ << ".........PASS!" ;
        }
        else if (8 * sizeof( ppuint ) == 32) 
        {
            fout_ << "of 32 bits " ;
            ppuint a{ static_cast<ppuint>(4294967294u) } ; // 2^32-1-1
            ppuint b{ static_cast<ppuint>(4294967294u) } ; // 2^32-1-1
            ppuint n{ static_cast<ppuint>(4294967295u) } ; // 2^32-1
            ppuint c{ static_cast<ppuint>(1u) } ; // 1
            ppuint r{ multiplyMod( a, b, n ) } ;
            if (r != c)
            {
                fout_ << ".........FAIL!" << endl ;
                fout_ << "a = " << a << endl ;
                fout_ << "b = " << b << endl ;
                fout_ << "n = " << n << endl ;
                fout_ << "c = " << c << endl ;
                fout_ << "r = " << r << endl ;
                status = false ;
            }
            else
                fout_ << ".........PASS!" ;
        }
    }
    
    fout_ << "\nTEST:  PowerMod ppuint 3^10 = 4 (mod 7)" ;
    {
        PowerMod<ppuint> powermod( static_cast<ppuint>(7u) ) ;
        if (powermod( static_cast<ppuint>(3u), static_cast<ppuint>(10u) ) != static_cast<ppuint>(4u))
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    PowerMod powermod( 7 );  powermod( 3, 10 ) = " << powermod( 3, 10 ) << " failed." << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  c,r = PowerMod( a, b ) modulo n for ppuint type " ;
    {
        if (8 * sizeof( ppuint ) == 64)
        {
            fout_ << "of 64 bits " ;
            ppuint a{ static_cast<ppuint>(2323123u) } ;
            ppuint b{ static_cast<ppuint>(10u) } ;
            ppuint n{ static_cast<ppuint>(18446744073709551615ULL) } ; // 2^64-1
            ppuint c{ static_cast<ppuint>(17955139022230052569ULL) } ;
            PowerMod<ppuint> powermod( n ) ;
            ppuint r = powermod( a, b ) ;
            if (r != c)
            {
                fout_ << ".........FAIL!" << endl ;
                fout_ << "a = " << a << endl ;
                fout_ << "b = " << b << endl ;
                fout_ << "n = " << n << endl ;
                fout_ << "c = " << c << endl ;
                fout_ << "r = " << r << endl ;
                status = false ;
            }
            else
                fout_ << ".........PASS!" ;
        }
        else if (8 * sizeof( ppuint ) == 32) 
        {
            fout_ << "of 32 bits " ;
            ppuint a{ static_cast<ppuint>(4294967290u) } ;
            ppuint b{ static_cast<ppuint>(10u) } ;
            ppuint n{ static_cast<ppuint>(4294967295u) } ; // 2^32-1
            ppuint c{ static_cast<ppuint>(9765625u) } ;
            PowerMod<ppuint> powermod( n ) ;
            ppuint r{ powermod( a, b ) } ;
            if (r != c)
            {
                fout_ << ".........FAIL!" << endl ;
                fout_ << "a = " << a << endl ;
                fout_ << "b = " << b << endl ;
                fout_ << "n = " << n << endl ;
                fout_ << "c = " << c << endl ;
                fout_ << "r = " << r << endl ;
                status = false ;
            }
            else
                fout_ << ".........PASS!" ;
        }
    }

    fout_ << "\nTEST:  PowerMod BigInt 3^10 = 4 (mod 7)" ;
    PowerMod<BigInt> powermod( static_cast<BigInt>(static_cast<ppuint>(7u)) ) ;
    if (powermod( static_cast<BigInt>(static_cast<ppuint>(3u)), 
                  static_cast<BigInt>(static_cast<ppuint>(10u)) ) != static_cast<BigInt>(static_cast<ppuint>(4u)))
    {
        BigInt three{ 3u } ;
        BigInt ten{ 10u } ;
        fout_ << "\n\tERROR:  PowerMod powermod( 7 );  powermod( 3, 10 ) = " << powermod( three, ten ) << " failed." << endl ;
        status = false ;
    }
    else
        fout_ << ".........PASS!" ;

    fout_ << "\nTEST:  PowerMod with out of range inputs." ;
    try
    {
        PowerMod<BigInt> powermod{ static_cast<BigInt>(static_cast<ppuint>( 7u )) } ;
        powermod( static_cast<BigInt>(static_cast<ppuint>( 0u )), static_cast<BigInt>(static_cast<ppuint>( 0u )) ) ;

        fout_ << "\n\tERROR:  PowerMod on 0^0 didn't throw exception." << endl ;
        status = false ;
    }
    catch( ArithModPError & e )
    {
        fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  InverseModP 3 * 5 = 1 (mod 7)" ;
    InverseModP imodp( 7 ) ;
    if (imodp( 3 ) != 5)
    {
        fout_ << "\n\tERROR:  InverseModP imodp( 7 );  imodp( 3 ) = " << imodp( 3 ) << " failed." << endl ;
        status = false ;
    }
    else
        fout_ << ".........PASS!" ;

    fout_ << "\nTEST:  IsPrimitiveRoot.   3 is a primitive root of 7." ;
    IsPrimitiveRoot isroot( 7 ) ;
    if (isroot( 3 ))
        fout_ << ".........PASS!" ;
    else
    {
        fout_ << "\n\tERROR:  IsPrimitiveRoot( 7 ) isroot ;  isroot( 3 ) = " << isroot( 3 ) << " failed." << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  IsPrimitiveRoot.   2 is a primitive root of 11." ;
    IsPrimitiveRoot isroot11( 11 ) ;
    if (isroot11( 2 ))
        fout_ << ".........PASS!" ;
    else
    {
        fout_ << "\n\tERROR:  IsPrimitiveRoot( 11 ) isroot11 ;  isroot11( 2 ) = " << isroot11( 2 ) << " failed." << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  IsPrimitiveRoot.   3 is NOT a primitive root of 11." ;
    if (isroot11( 3 ))
    {
        fout_ << "\n\tERROR:  IsPrimitiveRoot( 11 ) isroot11 ;  isroot11( 3 ) = " << isroot11( 3 ) << " failed." << endl ;
        status = false ;
    }
    else
        fout_ << ".........PASS!" ;

    fout_ << "\nTEST:  IsPrimitiveRoot.   5 is a primitive root of 65003." ;
    IsPrimitiveRoot isroot65003( 65003 ) ;
    if (isroot65003( 5 ))
        fout_ << ".........PASS!" ;
    else
    {
        fout_ << "\n\tERROR:  IsPrimitiveRoot65003( 5 ) should have said true.  It failed." << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  IsPrimitiveRoot.   8 is NOT a primitive root of 65003." ;
    if (isroot65003( 8 ))
    {
        fout_ << "\n\tERROR:  IsPrimitiveRoot65003( 8 ) should have said false.  It failed." << endl ;
        status = false ;
    }
    else
        fout_ << ".........PASS!" ;

    fout_ << "\nTEST:  constant coefficient test." ;
    ArithModP arith1( 5 ) ;
    if (arith1.constCoeffTest( 4, 1, 11 ))
        fout_ << ".........PASS!" ;
    else
    {
        fout_ << "\n\tERROR:  constant coefficient test failed = " << arith1.constCoeffTest( 4, 1, 11 ) << " failed." << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  constant coefficient is primitive root." ;
    ArithModP arith2( 7 ) ;
    if (arith2.constCoeffIsPrimitiveRoot( 4, 11 ))
        fout_ << ".........PASS!" ;
    else
    {
        fout_ << "\n\tERROR:  constant coefficient test failed = " << arith1.constCoeffIsPrimitiveRoot( 4, 11 ) << " failed." << endl ;
        status = false ;
    }
    
    fout_ << "\nTEST:  UniformRandomIntegers with range [0, 11)" ;
    ppuint range{ 11u } ;
    UniformRandomIntegers<ppuint> randum( range ) ;
    ppuint ran1{ randum.rand() } ;
    if (ran1 >= 0u && ran1 < range)
        fout_ << ".........PASS!" ;
    else
    {
        fout_ << "\n\tERROR:  random number generator out of range.  num = " << ran1 << " not in range [0, " << range << ")" << " failed." << endl ;
        status = false ;
    }
    
    fout_ << "\nTEST:  isProbablyPrime on ppuint prime 97 with random x = 10" ;
    if ( isProbablyPrime( static_cast<ppuint>( 97u ), static_cast<ppuint>( 10u ) ) == Primality::ProbablyPrime)
        fout_ << ".........PASS!" ;
    else
    {
        fout_ << ".........FAIL!" << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  isAlmostSurelyPrime for ppuint prime 97" ;
    if ( isAlmostSurelyPrime( static_cast<ppuint>( 97u ) ))
        fout_ << ".........PASS!" ;
    else
    {
        fout_ << ".........FAIL!" << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  isAlmostSurelyPrime for BigInt prime 97" ;
    if ( isAlmostSurelyPrime( static_cast<BigInt>( "97" ) ))
        fout_ << ".........PASS!" ;
    else
    {
        fout_ << ".........FAIL!" << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  isAlmostSurelyPrime for non-prime BigInt 49" ;
    if (isAlmostSurelyPrime( static_cast<BigInt>( 49u ) ))
    {
        fout_ << ".........FAIL!" << endl ;
        status = false ;
    }
    else
        fout_ << ".........PASS!" ;

    fout_ << "\nTEST:  isAlmostSurelyPrime on the 10000th prime number 104729 of ppuint type" ;
    if (isAlmostSurelyPrime( static_cast<ppuint>( 104729u )))
        fout_ << ".........PASS!" ;
    else
    {
        fout_ << ".........FAIL!" << endl ;
        status = false ;
    }

    return status ;
}


bool UnitTest::unitTestFactoring()
{
    bool status = true ;

    fout_ << "\nTEST:  Factor table method used on unsigned int 3^20 - 1 = 3486784400 = 2^4 5^2 11^2 61 1181" ;
    
    // 3^20 - 1 = 3486784400 = 2^4 5^2 11^2 61 1181
    ppuint num{ 3486784400 } ;
    ppuint   p{ 3 } ;
    ppuint   n{ 20 } ;
    
    Factorization<ppuint> f( num, FactoringAlgorithm::FactorTable, p, n ) ;
    vector<ppuint> df = f.getDistinctPrimeFactors() ;

    if (!(f.multiplicity(0) == 4 && f.primeFactor(0) ==    2u && f.primeFactor(0) == df[0] &&
          f.multiplicity(1) == 2 && f.primeFactor(1) ==    5u && f.primeFactor(1) == df[1] &&
          f.multiplicity(2) == 2 && f.primeFactor(2) ==   11u && f.primeFactor(2) == df[2] &&
          f.multiplicity(3) == 1 && f.primeFactor(3) ==   61u && f.primeFactor(2) == df[2] &&
          f.multiplicity(4) == 1 && f.primeFactor(4) == 1181u && f.primeFactor(2) == df[2] ))
    {
        fout_ << "\n\tERROR:  Table lookup factoring on unsigned int." << endl ;
        status = false ;

        fout_ << "\tFactor<ppuint> failed on 337500 = 2^2 3^3 5^5." << endl ;
        fout_ << "\tNumber of factors = " << f.numDistinctFactors() << endl ;
        fout_ << "\tFactors = " << endl ;
        for (unsigned int i = 0 ;  i < f.numDistinctFactors() ;  ++i)
            fout_ << f.primeFactor( i ) << " ^ " << f.multiplicity( i ) << " " ;
        fout_ << endl ;
    }
    else
        fout_ << ".........PASS!" ;

    
    fout_ << "\nTEST:  Factor table method used on BigInt 3^20 - 1 = 3486784400 = 2^4 5^2 11^2 61 1181" ;
    
    // 3^20 - 1 = 3486784400 = 2^4 5^2 11^2 61 1181
    BigInt num1{ 3486784400u } ;
    ppuint   p1{ 3 } ;
    ppuint   n1{ 20 } ;
    
    Factorization<BigInt> f1( num1, FactoringAlgorithm::FactorTable, p1, n1 ) ;
    vector<BigInt> df1 = f1.getDistinctPrimeFactors() ;

    if (!(f1.multiplicity(0) == 4 && f1.primeFactor(0) == static_cast<BigInt>(    2u ) && f1.primeFactor(0) == df[0] &&
          f1.multiplicity(1) == 2 && f1.primeFactor(1) == static_cast<BigInt>(    5u ) && f1.primeFactor(1) == df[1] &&
          f1.multiplicity(2) == 2 && f1.primeFactor(2) == static_cast<BigInt>(   11u ) && f1.primeFactor(2) == df[2] &&
          f1.multiplicity(3) == 1 && f1.primeFactor(3) == static_cast<BigInt>(   61u ) && f1.primeFactor(2) == df[2] &&
          f1.multiplicity(4) == 1 && f1.primeFactor(4) == static_cast<BigInt>( 1181u ) && f1.primeFactor(2) == df[2] ))
    {
        fout_ << "\n\tERROR:  Table lookup factoring on unsigned int." << endl ;
        status = false ;

        fout_ << "\tFactor<BigInt> failed on 337500 = 2^2 3^3 5^5." << endl ;
        fout_ << "\tNumber of factors = " << f.numDistinctFactors() << endl ;
        fout_ << "\tFactors = " << endl ;
        for (unsigned int i = 0 ;  i < f.numDistinctFactors() ;  ++i)
            fout_ << f.primeFactor( i ) << " ^ " << f.multiplicity( i ) << " " ;
        fout_ << endl ;
    }
    else
        fout_ << ".........PASS!" ;


    fout_ << "\nTEST:  Trial division factoring on unsigned int 337500 = 2^2 3^3 5^5." ;

    Factorization<ppuint> f2( 337500u, FactoringAlgorithm::TrialDivisionAlgorithm ) ;
    vector<ppuint> df2 = f2.getDistinctPrimeFactors() ;

    if (!(f2.multiplicity(0) == 2 && f2.primeFactor(0) == 2u && f2.primeFactor(0) == df2[0] &&
          f2.multiplicity(1) == 3 && f2.primeFactor(1) == 3u && f2.primeFactor(1) == df2[1] &&
          f2.multiplicity(2) == 5 && f2.primeFactor(2) == 5u && f2.primeFactor(2) == df2[2] ))
    {
        fout_ << "\n\tERROR:  Trial division factoring on unsigned int." << endl ;
        status = false ;

        fout_ << "\tFactor<ppuint> failed on 337500 = 2^2 3^3 5^5." << endl ;
        fout_ << "\tNumber of factors = " << f2.numDistinctFactors() << endl ;
        fout_ << "\tFactors = " << endl ;
        for (unsigned int i = 0 ;  i < f2.numDistinctFactors() ;  ++i)
            fout_ << f2.primeFactor( i ) << " ^ " << f2.multiplicity( i ) << " " ;
        fout_ << endl ;
    }
    else
        fout_ << ".........PASS!" ;


    fout_ << "\nTEST:  Trial division factoriing on BigInt 337500 = 2^2 3^3 5^5." ; 

    Factorization<BigInt> f3( static_cast<BigInt>("337500"), FactoringAlgorithm::TrialDivisionAlgorithm ) ;
    vector<BigInt> df3{ f3.getDistinctPrimeFactors() } ;

    if (!(f3.multiplicity(0) == 2u  && f3.primeFactor( 0 ) == static_cast<BigInt>( 2u ) && f3.primeFactor( 0 ) == df3[0] &&
          f3.multiplicity(1) == 3u  && f3.primeFactor( 1 ) == static_cast<BigInt>( 3u ) && f3.primeFactor( 1 ) == df3[1] &&
          f3.multiplicity(2) == 5u  && f3.primeFactor( 2 ) == static_cast<BigInt>( 5u ) && f3.primeFactor( 2 ) == df3[2] ))
    {
        fout_ << "\n\tERROR:Factorization<BigInt> failed on 337500 = 2^2 3^3 5^5." << endl ;
        status = false ;

        fout_ << "Number of factors = " << f3.numDistinctFactors() << endl ;
        fout_ << "Factors = " << endl ;
        for (unsigned int i = 0 ;  i < f3.numDistinctFactors() ;  ++i)
            fout_ << f3.primeFactor( i ) << " ^ " << f3.multiplicity( i ) << " " ;
        fout_ << endl ;
    }
    else
        fout_ << ".........PASS!" ;


    fout_ << "\nTEST:  Pollard Rho factorization on unsigned int 25852 = 2^2 23 281" ; 
    
    Factorization<ppuint> fr( 25852u, FactoringAlgorithm::pollardRhoAlgorithm ) ;
    vector<ppuint> dfr = fr.getDistinctPrimeFactors() ;

    if ( !(fr.multiplicity(0) == 2 && fr.primeFactor(0) ==  2u  && fr.primeFactor(0) == dfr[0] &&
           fr.multiplicity(1) == 1 && fr.primeFactor(1) == 23u  && fr.primeFactor(1) == dfr[1] &&
           fr.multiplicity(2) == 1 && fr.primeFactor(2) == 281u && fr.primeFactor(2) == dfr[2] ))
    {
        fout_ << "\n\tERROR:Factorization<ppuint> failed on 25852 = 2^2 23 281." << endl ;

        status = false ;
        fout_ << "Number of factors = " << fr.numDistinctFactors() << endl ;
        fout_ << "Factors = " << endl ;
        for (unsigned int i = 0 ;  i < fr.numDistinctFactors() ;  ++i)
            fout_ << fr.primeFactor( i ) << " ^ " << fr.multiplicity( i ) << " " ;
        fout_ << endl ;
    }
    else
        fout_ << ".........PASS!" ;


    fout_ << "\nTEST:  Pollard Rho factorization on BigInt 25852 = 2^2 23 281" ; 
    
    Factorization<BigInt> frb( static_cast<BigInt>( 25852u ), FactoringAlgorithm::pollardRhoAlgorithm ) ;
    vector<BigInt> dfrb = frb.getDistinctPrimeFactors() ;

    if ( !(frb.multiplicity(0) == 2 && frb.primeFactor(0) == static_cast<BigInt>( 2u   ) && frb.primeFactor(0) == dfrb[0] &&
           frb.multiplicity(1) == 1 && frb.primeFactor(1) == static_cast<BigInt>( 23u  ) && frb.primeFactor(1) == dfrb[1] &&
           frb.multiplicity(2) == 1 && frb.primeFactor(2) == static_cast<BigInt>( 281u ) && frb.primeFactor(2) == dfrb[2] ))
    {
        fout_ << "\n\tERROR:Factorization<BigInt> failed on 25852 = 2^2 23 281." << endl ;

        status = false ;
        fout_ << "Number of factors = " << frb.numDistinctFactors() << endl ;
        fout_ << "Factors = " << endl ;
        for (unsigned int i = 0 ;  i < frb.numDistinctFactors() ;  ++i)
            fout_ << frb.primeFactor( i ) << " ^ " << frb.multiplicity( i ) << " " ;
        fout_ << endl ;
    }
    else
        fout_ << ".........PASS!" ;


    fout_ << "\nTEST:  BigInt computation of p^n, r, factors of r, EulerPhi[ p^n - 1]/n for p = 2" ;
    {
        try
        {
            ppuint p{ 2u } ;
            ppuint n{ 36u } ;

            //                                             n
            // Create a polynomial of degree n modulo p:  x  + 1
            vector<ppuint> coeff {0} ;
            TrialPolynomial f( coeff ) ;
            f.initialTrialPoly( n, p ) ;
            
            // We'll do the factorization here.
            PolyOrder order( f ) ;

            if ( !( order.getMaxNumPoly() == static_cast<BigInt>( "68719476736" ) && order.getR()  == static_cast<BigInt>( "68719476735" ) &&
                    order.getFactorsOfR().primeFactor(0) == static_cast<BigInt>(  3u ) &&
                    order.getFactorsOfR().multiplicity(0) == 3 &&
                    order.getFactorsOfR().primeFactor(1) == static_cast<BigInt>(  5u ) &&
                    order.getFactorsOfR().multiplicity(1) == 1 &&
                    order.getFactorsOfR().primeFactor(2) == static_cast<BigInt>(  7u ) &&
                    order.getFactorsOfR().multiplicity(2) == 1 &&
                    order.getFactorsOfR().primeFactor(3) == static_cast<BigInt>( 13u ) &&
                    order.getFactorsOfR().multiplicity(3) == 1 &&
                    order.getFactorsOfR().primeFactor(4) == static_cast<BigInt>( 19u ) &&
                    order.getFactorsOfR().multiplicity(4) == 1 &&
                    order.getFactorsOfR().primeFactor(5) == static_cast<BigInt>( 37u ) &&
                    order.getFactorsOfR().multiplicity(5) == 1 &&
                    order.getFactorsOfR().primeFactor(6) == static_cast<BigInt>( 73u ) &&
                    order.getFactorsOfR().multiplicity(6) == 1 &&
                    order.getFactorsOfR().primeFactor(7) == static_cast<BigInt>(109u ) &&
                    order.getFactorsOfR().multiplicity(7) == 1 &&
                    order.getNumPrimPoly() == static_cast<BigInt>( "725594112" )
                ))
            {
                fout_ << "\n\tERROR:  BigInt computation of p^n, r, factors of r, EulerPhi[ p^n - 1]/n for p = 2" << endl ;
                status = false ;

                fout_ << "p = " << p << endl ;
                fout_ << "n = " << n << endl ;
                fout_ << "max_num_possible_poly = " << order.getMaxNumPoly() << endl ;
                fout_ << "r = " << order.getR() << endl ;
                fout_ << "r:  Number of factors = " << order.getFactorsOfR().numDistinctFactors() << endl ;
                fout_ << "Factors = " << endl ;
                for (unsigned int i = 0 ;  i < order.getFactorsOfR().numDistinctFactors() ;  ++i)
                    fout_ << order.getFactorsOfR().primeFactor( i ) << " ^ " << order.getFactorsOfR().multiplicity( i ) << " " ;
                fout_ << endl ;

                fout_ << "num_primitive_poly = " << order.getNumPrimPoly() << endl ;
            }
            else
                fout_ << ".........PASS!" ;
        }
        catch( BigIntMathError & e )
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    caught BigIntMathError: [ " << e.what() << " ] " << endl ;
            status = false ;
        }
        catch( FactorError & e )
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    caught FactorError: [ " << e.what() << " ] " << endl ;
            status = false ;
        }
    }


    fout_ << "\nTEST:  Factor Copy constructor" ;
    Factorization<BigInt> fact( f3 ) ;
    vector<BigInt> dfact = fact.getDistinctPrimeFactors() ;
    if (!(fact.multiplicity(0) == 2 && fact.primeFactor(0) == static_cast<BigInt>( 2u ) && fact.primeFactor(0) == dfact[0] &&
          fact.multiplicity(1) == 3 && fact.primeFactor(1) == static_cast<BigInt>( 3u ) && fact.primeFactor(1) == dfact[1] &&
          fact.multiplicity(2) == 5 && fact.primeFactor(2) == static_cast<BigInt>( 5u ) && fact.primeFactor(2) == dfact[2] ))
    {
        fout_ << "\n\tERROR:  Factor copy constructor failed on 337500 = 2^2 3^3 5^5." << endl ;
        status = false ;

        fout_ << "Number of factors = " << fact.numDistinctFactors() << endl ;
        fout_ << "Factors = " << endl ;
        for (unsigned int i = 0 ;  i < fact.numDistinctFactors() ;  ++i)
            fout_ << fact.primeFactor( i ) << " ^ " << fact.multiplicity( i ) << " " ;
        fout_ << endl ;
    }
    else
        fout_ << ".........PASS!" ;



    fout_ << "\nTEST:  Factor assignment operator" ;
    Factorization<BigInt> fact1 ;
    fact1 = f3 ;
    vector<BigInt> dfact1 = fact1.getDistinctPrimeFactors() ;
    if (!(fact1.multiplicity(0) == 2 && fact1.primeFactor(0) == static_cast<BigInt>( 2u ) && dfact1[0] == fact1.primeFactor(0) &&
          fact1.multiplicity(1) == 3 && fact1.primeFactor(1) == static_cast<BigInt>( 3u ) && dfact1[1] == fact1.primeFactor(1) &&
          fact1.multiplicity(2) == 5 && fact1.primeFactor(2) == static_cast<BigInt>( 5u ) && dfact1[2] == fact1.primeFactor(2) ))
    {
        fout_ << "\n\tERROR:  Factor assignment operator failed on 337500 = 2^2 3^3 5^5." << endl ;
        status = false ;

        fout_ << "Number of factors = " << fact1.numDistinctFactors() << endl ;
        fout_ << "Factors = " << endl ;
        for (unsigned int i = 0 ;  i < fact1.numDistinctFactors() ;  ++i)
            fout_ << fact1.primeFactor( i ) << " ^ " << fact1.multiplicity( i ) << " " ;
        fout_ << endl ;
    }
    else
        fout_ << ".........PASS!" ;

    return status ;
}


bool UnitTest::unitTestPolynomials()
{
    bool status = true ;

    fout_ << "\nTEST:  Polynomial() default constructor." ;
    {
        Polynomial p ;
        if (p.deg() != 0)
        {
            fout_ << "\n\tERROR: Polynomial default constructor failed." << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  Polynomial() from string." ;
    {
        Polynomial p( "2x^2 + 1, 3" ) ;
        if (p.deg() != 2 || p.modulus() != 3 || p[0] != 1 || p[1] != 0 || p[2] != 2)
        {
            fout_ << "\n\tERROR: Polynomial p( \"2x^2 + 1, 3\" ) failed." << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  Polynomial = string." ;
    {
        Polynomial p ;
        p = "2x^2 + 1, 3" ;
        if (p.deg() != 2 || p.modulus() != 3 || p[0] != 1 || p[1] != 0 || p[2] != 2)
        {
            fout_ << "\n\tERROR: Polynomial p = string \"2x^2 + 1, 3\" ) failed." << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  Polynomial() from string with negative constant should give a parser error." ;
    try
    {
        Polynomial p( "x^4-1, 5" ) ;
        fout_ << "\n\tERROR: Polynomial with negative constant failed to throw an exception " << endl ;
        status = false ;
    }
    catch( PolynomialRangeError & e )
    {
        // Match only the error message.
        // The file name and line number where the error occurred may change with different versions of this software.
        string trueErrorMessage = "Error in parser converting polynomial from string x^4-1, 5 for p = 2 Error:  negative number for a polynomial coefficient = -1 is not allowed.  Numbers must be >= 0" ;
        if (static_cast<string>(e.what()).find( trueErrorMessage ) == string::npos)
        {
            fout_ << "\n\tERROR:  Incorrect error message = |" << e.what() << "|" << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  Polynomial() to string." ;
    {
        Polynomial p ; // ( "2x^2 + 1, 3" )
        Polynomial q( p ) ;
        p[0] = 1 ; p[2] = 2 ;
        p.setModulus( 3 ) ;
        string s = p ;
        if (s != "2 x ^ 2 + 1, 3")
        {
            fout_ << "\n\tERROR: Polynomial p( \"2x^2 + 1, 3\" ) to string s = " << s << " failed." << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  Polynomial() copy constructor." ;
    try
    {
        Polynomial p( "2x^2 + 1, 3" ) ;
        Polynomial q( p ) ;
        if (static_cast<string>(q) != "2 x ^ 2 + 1, 3")
        {
            fout_ << "\n\tERROR: Polynomial copy constructor p( q ) = " << q << " failed." << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  PolynomialRangeError error:  copy constructor failed [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  Polynomial equality test operator==()." ;
    try
    {
        Polynomial p1( "2x^2 + 1, 3" ) ;
        Polynomial p2( "2x^2 + 1, 3" ) ;
        if (p1 == p2)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: Polynomial " << p1 << " == " << p2 << " failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  PolynomialRangeError error:  operator()== failed [ " << e.what() << " ] " << endl ;
        status = false ;
    }
    
    fout_ << "\nTEST:  Polynomial inequality test operator!=()." ;
    try
    {
        Polynomial p1( "2x^2 + 1, 3" ) ;
        Polynomial p2( "2x^2 + x + 1, 3" ) ;
        if (p1 != p2)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: Polynomial " << p1 << " != " << p2 << " failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  PolynomialRangeError error:  operator()!= failed [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  Polynomial assignment operator." ;
    try
    {
        Polynomial p( "2x^2 + 1, 3" ) ;
        Polynomial q ;
        q = p ; // If we did Polynomial q = p it would call the copy constructor.
        if (static_cast<string>(q) != "2 x ^ 2 + 1, 3")
        {
            fout_ << "\n\tERROR: Polynomial assignment operator p = q " << q << " failed." << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  PolynomialRangeError error:  assignment operator p = q failed [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  Polynomial()[] read only operator." ;
    try
    {
        Polynomial p( "2x^2 + 1, 3" ) ;
        if (p[ 0 ] == 1 && p[ 1 ] == 0 && p[ 2 ] == 2)
           fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR:  Polynomial [] read only failed on reading p[0], p[1], p[2]." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  Polynomial [] read only failed on reading p[0], p[1], p[2] [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  Polynomial()[] accessing coeff higher than its degree." ;
    try
    {
        const Polynomial p( "2x^2 + 1, 3" ) ;
        ppuint z{ p[ 3 ] } ;

        fout_ << "\n\tERROR:  Polynomial [] failed to throw exception on accessing p[3] = " << z << endl ;
        fout_ << "poly = " << p << " deg of p = " << p.deg() << endl ;
        status = false ;
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  Polynomial()[] lvalue operator." ;
    try
    {
        Polynomial p( "2x^2 + 1, 3" ) ;
        int oldDeg{ p.deg() } ;
        p[ 5 ] = 2 ;
        p[ 1 ] = 1 ;
        int newDeg = p.deg() ;
        if (static_cast<string>(p) != "2 x ^ 5 + 2 x ^ 2 + x + 1, 3" || oldDeg != 2 || newDeg != 5)
        {
            fout_ << "\n\tERROR: Polynomial [] lvalue operator " << p << " failed." << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  Polynomial [] failed [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  Polynomial() += operator." ;
    try
    {
        Polynomial p( "2x^2 + 1, 3" ) ;
        Polynomial q( " x^2 + 1, 3" ) ;
        p += q ;
        if (p.deg() != 1 && static_cast<string>(p) != "2, 3")
        {
            fout_ << "\n\tERROR: Polynomial += " << p << " failed." << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  Polynomial += failed. [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  Polynomial() += operator." ;
    try
    {
        Polynomial p( "2x^2 + 1, 3" ) ;
        Polynomial q( " x^2 + 2, 3" ) ;
        p += q ;
        if (p.deg() != 0 && static_cast<string>(p) != "0, 3")
        {
            fout_ << "\n\tERROR: Polynomial += " << p << " failed." << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  Polynomial += failed [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  Polynomial() + operator." ;
    try
    {
        Polynomial p( "2x^2 + 1, 3" ) ;
        Polynomial q( " x^2 + 1, 3" ) ;
        Polynomial r = p + q ;
        if (static_cast<string>(r) != "2, 3")
        {
            fout_ << "\n\tERROR: Polynomial + " << r << " failed." << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  Polynomial + failed [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  Polynomial * scalar" ;
    try
    {
        Polynomial p( "2x^2 + 1, 3" ) ;
        ppuint k{ 2 } ;

        Polynomial q = p * k ;
        if (static_cast<string>(q) != "x ^ 2 + 2, 3")
        {
            fout_ << "\n\tERROR: Polynomial * scalar operator " << q << " failed." << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  PolynomialRangeError error:  * scalar failed [ " << e.what() << " ] " << endl ;
        status = false ;
    }


    fout_ << "\nTEST:  Polynomial evaluation x^4 + 3x + 3 (mod 5)" ;
    {
        Polynomial p( "x^4 + 3x + 3, 5" ) ;
        ppuint f2{ p( 2 ) } ;
        ppuint f3{ p( 3 ) } ;
        ppuint f0{ p( 0 ) } ;
        if (f2 != 0 || f3 != 3 || f0 != 3)
        {
            fout_ << "\n\tERROR: Polynomial operator() = " << f2 << f3 << f0 << " failed." << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  Polynomial evaluation x^4 + x + 1 (mod 2)" ;
    {
        Polynomial p( "x^4 + x + 1, 2" ) ;
        ppuint f0{ p( 0 ) } ;
        ppuint f1{ p( 1 ) } ;
        if (f0 != 1 || f1 != 1)
        {
            fout_ << "\n\tERROR: Polynomial operator() = " << f0 << f1 << " failed." << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }

    fout_ << "\nTEST:  Polynomial hasLinearFactor is true" ;
    {
        Polynomial p( "x^4 + 3x + 3, 5" ) ;
        bool linFac = p.hasLinearFactor() ;
            if (linFac)
                fout_ << ".........PASS!" ;
            else
            {
                fout_ << "\n\tERROR: Polynomial hasLinearFactor = " << linFac << " failed." << endl ;
                status = false ;
            }
        }

    fout_ << "\nTEST:  Polynomial hasLinearFactor is false" ;
    {
        Polynomial p( "x^4 + 3x^2 + x + 1, 5" ) ;
        bool linFac = p.hasLinearFactor() ;
        if (!linFac)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: Polynomial hasLinearFactor = " << linFac << " failed." << endl ;
            status = false ;
        }
    }

    fout_ << "\nTEST:  Polynomial isInteger" ;
    try
    {
        Polynomial p( "x^4 + 3x + 3, 5" ) ;
        bool isInt = p.isInteger() ;
        if (!isInt)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: Polynomial " << p << " isInteger = " << isInt << " failed." << endl ;
            status = false ;
        }

        Polynomial q( "3, 5" ) ;
        isInt = q.isInteger() ;
        if (isInt)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: Polynomial " << q << " isInteger = " << isInt << " failed." << endl ;
            status = false ;
        }

    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  PolynomialRangeError error: polynomial operator() failed [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  Polynomial initial and next trial polynomials" ;
    try {
        TrialPolynomial p ;
        p.initialTrialPoly( 4, 5 ) ;

        for (int i = 1 ;  i <= 3 ; ++i)
        {
            p.nextTrialPoly() ;
        }
        if (static_cast<string>(p) == "x ^ 4 + 3, 5")
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: Polynomial " << p << " (3rd iteration from x^n initial) failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  PolynomialRangeError error: polynomial operator() failed [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    ////////////////////////////////////////////////////////////////////////
    // Test polynomial mod
    ////////////////////////////////////////////////////////////////////////

    fout_ << "\nTEST:  PolyMod constructor from polynomials." ;
    try {
        Polynomial g( "x^4 + x^2 + 1,2" ) ;
        Polynomial f( "x^4 + x + 1,2" ) ;

        PolyMod p( g, f ) ;
        if (static_cast<string>(p) == "x ^ 2 + x, 2" &&
            static_cast<string>( p.getf() ) == "x ^ 4 + x + 1, 2" && p.getModulus() == 2)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: PolyMod constructor from polynomials, g(x)  = " << p << " failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  PolynomialRangeError error: [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  PolyMod constructor from string and polynomial." ;
    try {
        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;
        PolyMod p( "x^6 + 2x^2 + 3x + 2, 5", f ) ;

        if (static_cast<string>(p) == "3 x ^ 3, 5" &&
            static_cast<string>( p.getf() ) == "x ^ 4 + x ^ 2 + 2 x + 3, 5" && p.getModulus() == 5)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: PolyMod constructor from string and polynomial failed." << endl ;
            fout_ << "\ng(x) mod f(x), p = " << p << endl ;
            fout_ << "\nf(x) = " << f << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  PolynomialRangeError error: [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  PolyMod timesX." ;
    try {
        Polynomial g( "2x^3 + 4x^2 + 3x, 5" ) ;
        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;

        PolyMod p( g, f ) ;

        p.timesX() ;
        if (static_cast<string>(p) == "4 x ^ 3 + x ^ 2 + x + 4, 5" )
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: PolyMod timesX " << p << " failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  PolynomialRangeError error: [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  PolyMod autoconvolve." ;
    try {
        Polynomial t( "4x^3 + x^2 + 3x + 3, 5" ) ;
        int k{ 3 } ; int lower{ 1 } ; int upper{ 3 } ;
        ppuint c{ autoConvolve( t, k, lower, upper ) } ;

        if (c == 3)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: PolyMod autoconvolve = " << c
                 << " for t = " << t << " deg = " << t.deg() << " failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  PolynomialRangeError autoconvolve [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  PolyMod convolve." ;
    try {
        Polynomial s( "4x^3 + x^2 + 3x + 3, 5" ) ;
        Polynomial t( "4x^3 + x^2 + 3x + 3, 5" ) ;
        int k{ 3 } ; int lower{ 1 } ; int upper{ 3 } ;
        ppuint c{ convolve( s, t, k, lower, upper ) } ;

        if (c == 3)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: PolyMod convolve = " << c
                 << " for t = " << t << " deg = " << t.deg() << " failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  PolynomialRangeError convolve [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  PolyMod coeffOfSquare." ;
    try {
        Polynomial g( "4x^3 + x^2 + 3x + 3, 5" ) ;
        int n = 4 ;
        ppuint c0{ coeffOfSquare( g, 0, n ) } ;
        ppuint c1{ coeffOfSquare( g, 1, n ) } ;
        ppuint c2{ coeffOfSquare( g, 2, n ) } ;
        ppuint c3{ coeffOfSquare( g, 3, n ) } ;
        ppuint c4{ coeffOfSquare( g, 4, n ) } ;
        ppuint c5{ coeffOfSquare( g, 5, n ) } ;
        ppuint c6{ coeffOfSquare( g, 6, n ) } ;

        if (c0 == 4 && c1 == 3 && c2 == 0 && c3 == 0 && c4 == 0 && c5 == 3 && c6 == 1)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: PolyMod coeffOfSquare (c0 ... c6) = " << c0 << " " << c1 << " " << c2
                 << " " << c3 << " " << c4 << " " << c5 << " " << c6 << " failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  PolynomialRangeError coeffOfSquare [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  PolyMod coeffOfProduct." ;
    try {
        Polynomial s( "4x^3 + x^2 + 4, 5" ) ;
        Polynomial t( "3x^2 + x + 2, 5" ) ;
        int n = 4 ;
        ppuint c0{ coeffOfProduct( s, t, 0, n ) } ;
        ppuint c1{ coeffOfProduct( s, t, 1, n ) } ;
        ppuint c2{ coeffOfProduct( s, t, 2, n ) } ;
        ppuint c3{ coeffOfProduct( s, t, 3, n ) } ;
        ppuint c4{ coeffOfProduct( s, t, 4, n ) } ;
        ppuint c5{ coeffOfProduct( s, t, 5, n ) } ;
        ppuint c6{ coeffOfProduct( s, t, 6, n ) } ;

        if (c0 == 3 && c1 == 4 && c2 == 4 && c3 == 4 && c4 == 2 && c5 == 2 && c6 == 0)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: PolyMod coeffOfProduct (c0 ... c6) = " << c0 << " " << c1 << " " << c2
                 << " " << c3 << " " << c4 << " " << c5 << " " << c6 << " failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  PolynomialRangeError coeffOfProduct [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  PolyMod square." ;
    try {
        Polynomial g( "4x^3 + x^2 + 4, 5" ) ;
        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;

        PolyMod p( g, f ) ;

        p.square() ;
        if (static_cast<string>(p) == "2 x ^ 3 + 4 x ^ 2 + x + 1, 5" )
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: PolyMod square " << p << " failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  PolynomialRangeError error [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  PolyMod operator* and implicitly, operator*=" ;
    try {
        Polynomial s( "4x^3 + x^2 + 4, 5" ) ;
        Polynomial t( "3x^2 + x + 2, 5" ) ;
        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;

        PolyMod smodf( s, f ) ;
        PolyMod tmodf( t, f ) ;

        PolyMod p = smodf * tmodf ;
        if (static_cast<string>(p) == "2 x ^ 3 + 3 x ^ 2 + 4 x + 2, 5" )
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: PolyMod operator* " << p << " failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  PolynomialRangeError error [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  PolyMod x_to_power and isInteger()" ;
    try {
        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;
        PolyMod x( "x, 5", f ) ; // g(x) = 0, modulus = 5.

        PolyMod p = power( x, static_cast<BigInt>( 156u ) ) ;
        if (static_cast<string>(p) == "3, 5" && p.isInteger())
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: PolyMod x_to_power = |" << static_cast<string>(p) << "| failed." << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  PolynomialRangeError error: [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    return status ;
}


bool UnitTest::unitTestPolynomialOrder()
{
    bool status = true ;

    fout_ << "\nTEST:  PolyOrder reduced Q-I matrix" ;
    try {
        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;
        PolyOrder order( f ) ;

        // Get the fully nullity count.  Don't do early out in findNullity().
        order.hasMultipleDistinctFactors( false ) ;
        string s = order.printQMatrix() ;
        string t = "\n(    0   0   0   0 )\n(    0   4   0   0 )\n(    4   0   0   0 )\n(    0   0   4   0 )\n" ;
        if (s == t)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: PolyOrder reduced Q-I failed = " << s << endl ;
            fout_ << "\n                   true reduced Q-I = " << t << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  PolynomialRangeError error [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  PolyOrder 3 distinct factors out of 4" ;
    try {
        Polynomial f( "x^4 + 3 x^3 + 3 x^2 + 3 x + 2, 5" ) ;
        PolyOrder order( f ) ;

        // Get the fully nullity count.  Don't do early out in findNullity().
        bool multipleFactors = order.hasMultipleDistinctFactors( false ) ;
        if (multipleFactors && order.getNullity() == 3)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: PolyOrder 3 distinct factors out of 4 failed" << endl ;
            fout_ << " f( x ) = " << f << endl ;
            fout_ << " nullity = " << order.getNullity() << endl ;
            fout_ << "\n    reduced Q-I matrix " << order.printQMatrix() << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << "\n\tERROR:  PolynomialRangeError error [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  PolyOrder, reducible polynomial x^3 + 3 mod 5 with 2 distinct factors" ;
    try {
        Polynomial f( "x^3 + 3, 5" ) ;
        PolyOrder order( f ) ;

        // Get the fully nullity count.  Don't do early out in findNullity().
        bool multipleFactors = order.hasMultipleDistinctFactors( false ) ;

        if (multipleFactors && order.getNullity() == 2)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    PolyOrder 2 distinct factors failed" << endl ;
            fout_ << "    f( x ) = " << f << endl ;
            fout_ << "    nullity = " << order.getNullity() << endl ;
            fout_ << "    reduced Q-I matrix " << order.printQMatrix() << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << ".........FAIL!" << endl ;
        fout_ << "    PolynomialRangeError [ " << e.what() << " ] " << endl ;
        status = false ;
    }

    fout_ << "\nTEST:  PolyOrder, irreducible polynomial x^4 + x^2 + 2x + 3 mod 5 (nullity = 1)" ;
    try 
    {
        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;
        PolyOrder order( f ) ;

        // Get the fully nullity count.  Don't do early out in findNullity().
        bool multipleFactors = order.hasMultipleDistinctFactors( false ) ;

        if (multipleFactors == false && order.getNullity() == 1)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    PolyOrder irreducible" << endl ;
            fout_ << "    f( x ) = " << f << endl ;
            fout_ << "    nullity = " << order.getNullity() << endl ;
            fout_ << "    reduced Q-I matrix " << order.printQMatrix() << endl ;
            status = false ;
        }
    }
    catch( PolynomialRangeError & e )
    {
        fout_ << ".........FAIL!" << endl ;
        fout_ << "    PolynomialRangeError: [ " << e.what() << " ] " << endl ;
        status = false ;
    }


    fout_ << "\nTEST:  PolyOrder 1 distinct factor 4 times" ;
    {
        Polynomial f( "x^4 + 4x^3 + x^2 + 4x + 1, 5" ) ;
        PolyOrder order( f ) ;

        // Get the fully nullity count.  Don't do early out in findNullity().
        bool multipleFactors = order.hasMultipleDistinctFactors( false ) ;
        if (multipleFactors == false && order.getNullity() == 1)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: PolyOrder 1 distinct factor 4 times" << endl ;
            fout_ << " f( x ) = " << f << endl ;
            fout_ << " nullity = " << order.getNullity() << endl ;
            fout_ << "\n    reduced Q-I matrix " << order.printQMatrix() << endl ;
            status = false ;
        }
    }

    fout_ << "\nTEST:  PolyOrder orderM()" ;
    {
        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;
        PolyOrder order( f ) ;

        if (order.orderM())
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: PolyOrder orderM failed." << endl ;
            status = false ;
        }
    }

    fout_ << "\nTEST:  PolyOrder orderR() is true" ;
    {
        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;
        PolyOrder order( f ) ;

        ppuint a{ order.orderR() } ;
        if (a == 3)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: PolyOrder orderR failed." << endl ;
            status = false ;
        }
    }

    fout_ << "\nTEST:  PolyOrder orderR() is false" ;
    {
        Polynomial f( "x^4 + x + 3, 5" ) ;
        PolyOrder order( f ) ;

        ppuint a{ order.orderR() } ;
        if (a == 0)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: PolyOrder orderR failed." << endl ;
            status = false ;
        }
    }

    fout_ << "\nTEST:  PolyOrder isPrimitive on non-primitive poly" ;
    {
        Polynomial f( "x^5 + x + 1, 2" ) ;
        PolyOrder order( f ) ;

        bool isPrimitive = order.isPrimitive() ;
        if (!isPrimitive)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: PolyOrder isPrimitive on non-primitive poly" << endl ;
            fout_ << " f( x ) = " << f << endl ;
            status = false ;
        }
    }

    fout_ << "\nTEST:  PolyOrder isPrimitive on primitive poly" ;
    {
        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;
        PolyOrder order( f ) ;

        bool isPrimitive = order.isPrimitive() ;
        if (isPrimitive)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: PolyOrder isPrimitive on primitive poly" << endl ;
            fout_ << " f( x ) = " << f << endl ;
            status = false ;
        }
    }

    fout_ << "\nTEST:  PolyOrder isPrimitive on primitive poly, part II" ;
    {
        Polynomial f0( "x^4+4, 5" ) ;
        PolyOrder order( f0 ) ;

        Polynomial f( "x^4 + x^2 + 2x + 3, 5" ) ;
        order.resetPolynomial( f ) ;

        bool isPrimitive = order.isPrimitive() ;
        if (isPrimitive)
            fout_ << ".........PASS!" ;
        else
        {
            fout_ << "\n\tERROR: PolyOrder isPrimitive on primitive poly, part II" << endl ;
            fout_ << " f( x ) = " << f << endl ;
            status = false ;
        }
    }
    
    return status ;
}


bool UnitTest::unitTestParser()
{
    bool status = true ;
    
    // Create a parser with parse tables.
    string s ;
    PolyValue  v ;
    PolyParser< PolySymbol, PolyValue > p ;
    
    fout_ << "\nTEST:  Parsing command line options for test polynomial x^4 + 1, 2 with -s -t and -c options." ;
    {
        #ifdef DEBUG_PP_FORCE_UNIT_TEST_FAIL
            const char * argv[ 5 ] { "Primpoly", "-s", "-t",  "-c", "x^3 + 1, 2" } ;
            const Polynomial truePoly( "x^3 + 1", 2 ) ;
        #else
            const char * argv[ 5 ] { "Primpoly", "-s", "-t",  "-c", "x^4 + 1, 2" } ;
            const Polynomial truePoly( "x^4 + 1", 2 ) ;
        #endif
        
        p.parseCommandLine( 5, argv ) ;
        
        if (p.test_polynomial_for_primitivity_ && p.print_operation_count_ && p.slow_confirm_ &&
            !p.list_all_primitive_polynomials_ && !p.print_help_ &&
            p.test_polynomial_ == truePoly)
        {
            fout_ << ".........PASS!" ;
        }
        else
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    Test polynomial = " << p.test_polynomial_ ; fout_ << " deg = " << p.test_polynomial_.deg() << endl ;
            fout_ << "    p = " << p.p << "    n = " << p.n << endl ;
            status = false ;
        }
    }
    
    fout_ << "\nTEST:  parsing constant 0" ;
    {
        s = "0" ;
        #ifdef DEBUG_PP_FORCE_UNIT_TEST_FAIL
        s = "2" ;
        #endif
        v = p.parse( s ) ;
        if (!(v.scalar_ == 2 && (v.f_.size()-1) == 0 && v.f_[0] == 0))
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    parsing input " << s << endl << " value = "  << v << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }
    
    fout_ << "\nTEST:  parsing polynomial with a specified modulus:  2 x ^ 3 + 3 x + 4, 5" ;
    {
        #ifdef DEBUG_PP_FORCE_UNIT_TEST_FAIL
            s = "2 x ^ 3 + 3 x + 4, 7" ;
        #else
            s = "2 x ^ 3 + 3 x + 4, 5" ;
        #endif
        v = p.parse( s ) ;
        if (!(v.scalar_ == 5 && (v.f_.size()-1) == 3 && v.f_[0] == 4 && v.f_[1] == 3 && v.f_[2] == 0 && v.f_[3] == 2))
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    parsing input " << s << endl << " value = " << v << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }
    
    fout_ << "\nTEST:  parsing polynomial 2x without a modulus, which will be defaulted to p=2:  2x" ;
    {
        s = "2x" ;
        #ifdef DEBUG_PP_FORCE_UNIT_TEST_FAIL
        s = "2x,3" ;
        #endif
        v = p.parse( s ) ;
        if (!(v.scalar_ == 2 && (v.f_.size()-1) == 1 && v.f_[0] == 0 && v.f_[1] == 2 ))
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    parsing input " << s << endl << " value = " << v << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }
    
    fout_ << "\nTEST:  parsing bad syntax x 1" ;
    try
    {
        s = "x 1" ;
        #ifdef DEBUG_PP_FORCE_UNIT_TEST_FAIL
        s = "x+1" ;
        #endif
        v = p.parse( s ) ;
        if (!(v.scalar_ == 0 && (v.f_.size()-1) == 0 &&
              v.f_[0] == 0 && v.f_[1] == 2 ))
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    Parser did not throw a parsing error " << endl ;
            fout_ << "    while parsing input " << s << endl << " value = " << v << endl ;
            status = false ;
        }
    }
    catch( ParserError & e )
    {
        // Match only the error message.
        // The file name and line number where the error occurred may change with different versions of this software.
        string trueErrorMessage = "Expecting to see x^ or x or x ^ integer in sentence x 1" ;
        if (static_cast<string>(e.what()).find( trueErrorMessage ) == string::npos)
        {
            fout_ << ".........FAIL!" << endl ;
            fout_ << "    Parser correctly threw a parse error exception while parsing " << s << endl << " value = " << v << endl ;
            fout_ << "    but the error message was incorrect!  error = |" << e.what() << "|" << endl ;
            status = false ;
        }
        else
            fout_ << ".........PASS!" ;
    }
    
    return status ;
}
#endif // SELF_CHECK
