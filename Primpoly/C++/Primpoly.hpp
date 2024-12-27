#ifndef PP_H 
#define PP_H


/*=============================================================================
 |
 | NAME
 |
 |     Debug macros.
 |
 +============================================================================*/

// Let's do this always, so we can gather bug information from users.
#define SELF_CHECK

// These debugging flags are normally be defined in the makefile or build file,
// but you could alternately define them here.
#define DEBUG_ALL
#undef  DEBUG_ALL

#ifdef DEBUG_ALL
    // Highest level primitivity testing:
    #define DEBUG_PP_PRIMITIVITY

    // Polynomial calculations debugging:
    #define DEBUG_PP_POLYNOMIAL

    // Debug the LALR(1) parser for polynomials and factor tables:
    #define DEBUG_PP_PARSER

    // Testing primality:
    #define DEBUG_PP_PRIMALITY_TESTING

    // Factoring into primes:
    #define DEBUG_PP_FACTOR

    // Arbitrary precision arithmetic debugging:
    #define DEBUG_PP_BIGINT

    // Modulo p arithmetic debugging:
    #define DEBUG_PP_ARITH
#endif // DEBUG_ALL

// Forces one or more unit tests to fail and generate test error messages.
// Default is to leave undefined.
//
//    #define DEBUG_PP_FORCE_UNIT_TEST_FAIL
//
// Turn on to check memory exceptions.  Default is to leave it off.
// This may crash some on machines with buggy C++ compilers and OS's.
//
//    #define DEBUG_PP_FORCE_MEMORY_OVERLOAD


/*=============================================================================
 |
 | NAME
 |
 |     Basic integer types.
 |
 | DESCRIPTION
 |
 |     Define the basic integer types we will use for all modulus p calculations,
 |     multiple precision arithmetic, polynomial operations and factoring.
 |     Higher precision will decrease your computation time according to profiling.
 |
 +============================================================================*/

// Microsoft Windows 7 64-bit + Visual C++ compiler or Cygwin.
#if defined( _MSC_VER ) || defined( __CYGWIN__ )
    // 64-bit integer types.
    typedef unsigned long long ppuint ;
    typedef   signed long long ppsint ;

    // 32-bit integer types.
    typedef unsigned int ppuint32 ;
    typedef unsigned int ppsint32 ;
// Mac OS X or Ubuntu Linux.
#else
    // 64-bit integer types.
    typedef unsigned long int ppuint ;
    typedef   signed long int ppsint ;

    // 32-bit integer types.
    typedef unsigned int ppuint32 ;
    typedef unsigned int ppsint32 ;
#endif



/*=============================================================================
 |
 | NAME
 |
 |     ReturnStatus
 |
 | DESCRIPTION
 |
 |     Enumerated type integer status fed back to the Unix shell from main().
 |
 +============================================================================*/

enum class ReturnStatus
{
    Success       = 0,
    AskForHelp    = 1,
    PNotPrime     = 2,
    RangeError    = 3,
    InternalError = 4,
    Reserved      = 5
} ;


/*=============================================================================
 |
 | NAME
 |
 |     PrimpolyError
 |
 | DESCRIPTION
 |
 |     Top level error class used in main() and UnitTest.
 |
 +============================================================================*/

class PrimpolyError : public runtime_error
{
    public:
        // Throw with error message, file name and line number.
        PrimpolyError( const string & description, const string & file, const int & line )
        : runtime_error( description + " in file " + file + " at line " + to_string(line) )
        {
        } ;
    
        // Throw with an error message.
        PrimpolyError( const string & description )
            : runtime_error( description )
        {
        } ;

        // Default throw with no error message.
        PrimpolyError()
            : runtime_error( "Polynomial error:  " )
        {
        } ;

} ; // end class PrimpolyError


/*=============================================================================
 |
 | NAME
 |
 |     Message strings.
 |
 | DESCRIPTION
 |
 |     Messages to console which are used by main() only.
 |
 +============================================================================*/

static const string writeToAuthorMessage
(
    "Dear User,\n"
    "    Sorry you got an error message.  Please email the author at\n"
    "        seanerikoconnor!AT!gmail!DOT!com\n"
    "    with !DOT! replaced by . and the !AT! replaced by @\n"
    #ifdef SELF_CHECK
    "    Please send me all console output from this program.\n"
    "    Attach the unitTest.log file which should be located in the current directory.\n"
    "    However, if the self-check failed, there won't be a log file.\n"
    #else
    "    It looks like you have the unit test self check compiled off."
    "    Please set #define SELF_CHECK in the Primpoly.hpp header file, recompile and rerun."
    #endif
    "Thanks for your help,\n"
    "Sean E. O'Connor\n"
    "\n"
) ;

static const string confirmWarning
(
    "Confirming polynomial is primitive with a brute force check.\n"
    "WARNING:  You may wait an impossibly long time!\n"
    "To abort, hit control-C in your console window to stop this program.\n"
) ;

#endif  //  End of wrapper for header.
