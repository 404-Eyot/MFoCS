#ifndef __PP_UNITTEST_H__
#define __PP_UNITTEST_H__


/*=============================================================================
|
| NAME
|
|     unitTest
|
| DESCRIPTION
|
|     In the spirit of EXTREME programming, test each class and its member
|     functions.  Run the tests every time we run this application.
|     Place the test results into the current directory in the file
|     unitTest.txt.  If we can't write the file, log an error message
|     to the console and skip the tests.  If we can't even do that, throw
|     an exception.
|
+============================================================================*/

class UnitTest
{
    public:
        // Default constructor with a reasonable default file name.
        UnitTest( const char * fileName = "unitTest.log" ) ;

        ~UnitTest() ;

        // We have only one unit test object;  prevent any copy constructors or
        // assignment operators from being defined.
        UnitTest( const UnitTest & u ) = delete ;
        UnitTest & operator=( const UnitTest & unitTest ) = delete ;

        // Run all unit tests.
        bool run() ;

    protected:
        // Unit tests for different functional areas.
        bool unitTestSystemFunctions() ;
        bool unitTestBigIntBase10() ;
        bool unitTestBigIntDefaultBase() ;
        bool unitTestModPArithmetic() ;
        bool unitTestFactoring() ;
        bool unitTestPolynomials() ;
        bool unitTestPolynomialOrder() ;
        bool unitTestParser() ;

    protected:
        // File to log the unit test results.  Can be standard output (console) if we can't open the file.
        const char * unit_test_log_file_name_ ;
        ofstream fout_ ;
} ;


/*=============================================================================
|
| NAME
|
|     UnitTestError
|
| DESCRIPTION
|
|     Exception classes for the the UnitTest class 
|     derived from the STL exception class runtime_error.
|
+============================================================================*/

class UnitTestError : public runtime_error
{
    public:
        // Throw with error message, file name and line number.
        UnitTestError( const string & description, const string & file, const int & line )
        : runtime_error( description + " in file " + file + " at line " + to_string(line) )
        {
        } ;
    
        // Throw with an error message.
        UnitTestError( const string & description )
            : runtime_error( description )
        {
        } ; 

        // Default throw with no error message.
        UnitTestError()
            : runtime_error( "UnitTest exception: " )
        {
        } ;
} ;
#endif // __PP_UNITTEST_H__ -- End of wrapper for header file.
