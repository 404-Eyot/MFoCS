#ifndef __PPPARSER_H__


// Some miscellaneous constants.
#define MAX_NUM_COMMAND_LINE_ARGS 3
const ppuint minModulus{ 2 } ;
const ppuint minDegree{ 2 } ;


/*=============================================================================
 |
 | NAME
 |
 |     Action
 |
 | DESCRIPTION
 |
 |     Types of LR parser actions.
 |
 +============================================================================*/

enum class Action
{
    Shift, Reduce, Accept, Error
} ;


/*=============================================================================
 |
 | NAME
 |
 |     enumToString
 |
 | DESCRIPTION
 |
 |     Helper function to translate enum types into human readable format.
 |
 +============================================================================*/

template< typename SymbolType >
string enumToString( const SymbolType & st ) ;


/*=============================================================================
 |
 | NAME
 |
 |     Symbol
 |
 | DESCRIPTION
 |
 |     Define the VALUE of a symbol.  Used in the parser's syntax directed
 |     translation and also for the final polynomial value returned from the
 |     parser.
 |
 +============================================================================*/

// Tokens and other types of symbols used in parsing.
template< typename SymbolType, typename ValueType >
class Symbol
{
    public:
        Symbol() ;
        
        Symbol( SymbolType type, int state ) ;
        
        Symbol( SymbolType type, int state, ValueType value )  ;
        
        Symbol( const Symbol< SymbolType, ValueType > & s ) ;
    
        // Destructor.  Virtual so derived class destructor
        // automatically calls its base class destructor.
        virtual ~Symbol()
        {
        }
    
        Symbol< SymbolType, ValueType > & operator=( const Symbol<SymbolType,ValueType> & s ) ;

        // Print function for Symbol class.
        // Define this templated non-member friend function here, otherwise the compiler will generate
        // a link error.  If the compiler sees only a declaration here, it doesn't know yet it's a template.
        // See https://isocpp.org/wiki/faq/templates#template-friends
        friend ostream & operator<<( ostream & out, const Symbol<SymbolType,ValueType> & s )
        {
            out << enumToString<SymbolType>( s.type_ ) << "   " ;
            out << s.value_ << "   " ;
            out << "state " << s.state_ ;

            return out ;
        }

    // Allow direct access to this simple data type for convenience.
    public:
        SymbolType type_ ;  // Type of terminal or nonterminal.
        ValueType  value_ ; // Value of the symbol.
        int        state_ ; // State or production number.
} ;

/*=============================================================================
 |
 | NAME
 |
 |     ActionState
 |
 | DESCRIPTION
 |
 |     A pair consisting of action and state for the LR parser.
 |     Actions are (shift s, reduce p, accept, error) and the new state
 |     number to transition to.
 |
 +============================================================================*/

class ActionState
{
    public:
        ActionState() ;
        
        ActionState( Action type, int state ) ;
    
        // Destructor.  Virtual so derived class destructor
        // automatically calls its base class destructor.
        virtual ~ActionState()
        {
        }

        ActionState( const ActionState & as ) ;
        
        ActionState & operator=( const ActionState & as ) ;
    
        friend ostream & operator<<( ostream & out, const ActionState & as ) ;
    
    // Allow direct access to this simple data type for convenience.
    public:
        Action action_ ;
        int    state_ ;
} ;

/*=============================================================================
 |
 | NAME
 |
 |     ParserError
 |
 | DESCRIPTION
 |
 |     Exceptions for the parser class.
 |
 +============================================================================*/

class ParserError : public runtime_error
{
    public:
        // Throw with error message, file name and line number.
        ParserError( const string & description, const string & file, const int & line )
        : runtime_error( description + " in file " + file + " at line " + to_string(line) )
        {
        } ;
    
        // Throw with an error message.
        ParserError( const string & description )
            : runtime_error( description )
        {
        } ;

        // Throw with default error message.
        ParserError()
            : runtime_error( "Parser error:  " )
        {
        } ;

} ; 



/*=============================================================================
|
| NAME
|
|     Parser
|
| DESCRIPTION
|
|     Abstract base class for parsing using an LALR(1) parser.
|
| EXAMPLE
|
|     Example of how to use the PolyParser child class for polynomials,
|
|        // Create the parser.
|        PolyParser<PolySymbol, PolyValue> parser() ;
|
|        // Sample polynomial.
|        string s = "2 x ^ 3 + 3 x + 4, 5" ;
|
|        try 
|        {
|            PolyValue v = parser.parse( s ) ;
|            cout << "Parsing the string = " << s << endl ;
|            cout << "We have a polynomial = " << v << endl ;
|        }
|        catch( ParserError & e )
|        {
|            cout << "Parser error:  " << e.what() << endl ;
|        }
|
+============================================================================*/

template< typename SymbolType, typename ValueType >
class Parser
{
    // The basic member functions to create and initialize the parser,
    // make copies and do a syntax-directed translation of a string.
    public:
        // Default construct the parser.
        Parser() ;
        
        // Destructor.  Virtual so derived class destructor
        // automatically calls its base class destructor.
        virtual ~Parser() ;

        // Copy constructor.
        Parser( const Parser & parser ) ;

        // Assignment operator.
        virtual Parser & operator=( const Parser & parser ) ;

        // Parse an input string, doing a syntax-directed translation, returning the value.
        ValueType parse( string s ) ;

    // Pure virtual functions to be defined only in the child classes, since the lexical analyzer, parse tables and
    // syntax-directed translation depend uniquely on the grammar.
    //
	// Defined within the scope of the Parser class or its derived classes only,
    // so not visible from outside the parser.
    protected:
        // Parse string into tokens.
        virtual void tokenize( string sentence ) = 0 ;
    
        // NOTE:  call this in the child class constructor.  It fills in the ACTION, GOTO, and ERROR_MESSAGE tables.
        virtual void initializeTables() = 0 ;

        // Reduce using production A -> beta, computing A's value from values of the tokens in beta.
        virtual void syntaxDirectedTranslation( int productionNum, int topIndex, Symbol<SymbolType,ValueType> & as ) = 0 ;
    
    // These member values and functions are called directly in the child classes.
    protected:
        // Number of states, productions, etc.
        int num_states_ ;        // Includes the 0 state.
        int num_productions__ ;   // Number of productions.
        int num_terminals_ ;     // Number of terminal symbols.
        int num_non_terminals_ ;  // Number of nonterminal symbols.
    
        void insertAction(       int state,   SymbolType terminal, Action actionType, int actionState ) ;
        void insertGoto(         int state,   SymbolType nonterm,  int    newState ) ;
        void insertProduction(   int prodNum, SymbolType nonTerm,  int    rhsLength ) ;
        void insertErrorMessage( int state,   string errorMessage ) ;

    protected:
        // Parser stack containing symbols tokenized by lexical analyzer.
        vector< Symbol<SymbolType, ValueType> > input_stack_ ;

        // Parse stack containing symbols and states.
        vector< Symbol<SymbolType, ValueType> > parse_stack_ ;

        // Action table.  
        //     action = ACTION_TABLE[ <state at top of parse stack> ][ <lookahead input token> ]
        vector< vector<ActionState> > action_table_ ;

        // GOTO table.
        // For reduce actions,
        //    new goto state to push = GOTO[ current state ][ nonterminal production symbol ]
        vector< vector<int> > goto_table_ ;

        // Error messages.
        vector<string> error_message_ ;
        
        // Productions:  Left side non-terminal symbol + production number.
        vector< Symbol<SymbolType, ValueType> > production_ ;
} ;



// --------------- Derived class parsers and their symbols and values ---------



/*=============================================================================
 | 
 | NAME
 | 
 |     PolyValue
 | 
 | DESCRIPTION
 | 
 |     Define a symbol with a polynomial value.  Used in the parser's syntax directed
 |     translation and also for the final polynomial value returned from the 
 |     parser.
 |
 +============================================================================*/

class PolyValue
{
    public:
        PolyValue() ;
    
        PolyValue( const PolyValue & v ) ;
    
        PolyValue & operator=( const PolyValue & v ) ;
    
        friend ostream & operator<<( ostream & , const PolyValue & v ) ;

    // Allow direct access to this simple data type for convenience.
    public:
        ppuint           scalar_ ;  // Number which can be a polynomial coefficient, power of x, or the modulus p.
                                    // Let it be signed for maximum flexibility.  We'll check if it exceeds the allowed
                                    // range for internal primitive polynomial computations in the parser. 
        vector<ppuint> f_ ;       // Polynomial f(x).
} ;


/*=============================================================================
 |
 | NAME
 |
 |     PolySymbol
 |
 | DESCRIPTION
 |
 |     Define polynomial symbol (tokens).  Used in the parser's syntax directed
 |     translation.
 |
 +============================================================================*/

enum class PolySymbol
{
    // Terminal symbols or tokens.
    Dollar = 0, Integer, Comma, Ecks, Plus, Exp,
    NumTerminals,
    
    // Nonterminal symbols.
    S, Mod, Poly, Term, Multiplier, Power,
    NumSymbols
} ;

/*=============================================================================
|
| NAME
|
|     PolyParser
|
| DESCRIPTION
|
|     Child class for parsing a polynomial grammar inheriting from abstract
|     base class Parser.
|
| EXAMPLE
|
|     Here is the LALR(1) grammar.  Note the use of epsilon productions to
|     make the grammar simpler.
|
|     Productions:
|
|         S          -> Poly Mod
|         Mod        -> comma integer | EPSILON
|         Poly       -> Poly + Term | Term
|         Term       -> Multiplier Power
|         Multiplier -> integer | EPSILON
|         Power      -> x | x ^ integer | EPSILON
|
|     Terminal symbols:
|   
|         comma + integer ^ x $
|
+============================================================================*/

template< typename SymbolType, typename ValueType >
class PolyParser : public Parser< SymbolType, ValueType >
{
    public:
        // Construct the parser.
        PolyParser() ;
        
        // Destructor.
        ~PolyParser() ;
        
        // Copy constructor.
        PolyParser( const PolyParser< SymbolType, ValueType > & parser ) ;
        
        // Assignment operator.
        PolyParser< SymbolType, ValueType > & operator=( const PolyParser< SymbolType, ValueType > & parser ) ;
        
        // Command line argument parsing.
        void parseCommandLine( int argc, const char * argv[] ) ;
        
        // Allow direct access to these command line parameters for convenience.
        bool   test_polynomial_for_primitivity_ ;
        bool   list_all_primitive_polynomials_ ;
        bool   print_operation_count_ ;
        bool   print_help_ ;
        bool   slow_confirm_ ;
        ppuint p ;
        int    n ;
        Polynomial test_polynomial_ ;
    
        // Location of the factorization tables.
        static string current_working_dir_ ;
    
    private:
        // Parse string into tokens.
        void tokenize( string sentence ) ;
        
        // Fill in the ACTION, GOTO, and ERROR_MESSAGE tables.
        void initializeTables() ;

        // Syntax directed translation.  Reduce using production A -> beta, computing A's value from values of 
        // the tokens in beta.
        void syntaxDirectedTranslation( int productionNum, int topIndex, Symbol<SymbolType,ValueType> & as ) ;

        // Even though these member variables are declared in the base class as protected, they are not visible in this
        // child class.  Because we have a templated class, we must tell the C++ compiler explicitly that these symbols
        // are from the base class.
        using Parser<SymbolType, ValueType>::num_states_ ;
        using Parser<SymbolType, ValueType>::num_productions__ ;
        using Parser<SymbolType, ValueType>::num_terminals_ ;
        using Parser<SymbolType, ValueType>::num_non_terminals_ ;

        using Parser<SymbolType, ValueType>::insertAction ;
        using Parser<SymbolType, ValueType>::insertGoto ;
        using Parser<SymbolType, ValueType>::insertProduction ;
        using Parser<SymbolType, ValueType>::insertErrorMessage ;

        using Parser< SymbolType, ValueType >::input_stack_ ;
        using Parser< SymbolType, ValueType >::parse_stack_ ;
        using Parser< SymbolType, ValueType >::action_table_ ;
        using Parser< SymbolType, ValueType >::goto_table_ ;
        using Parser< SymbolType, ValueType >::error_message_ ;
        using Parser< SymbolType, ValueType >::production_ ;
} ;

// Static member variable needs to be initialized outside of the class declaration.
template< typename SymbolType, typename ValueType >
string PolyParser<SymbolType, ValueType>::current_working_dir_ = "" ;


// --------------- Derived class parsers and their symbols and values ---------



/*=============================================================================
 | 
 | NAME
 | 
 |     FactorizationValue
 | 
 | DESCRIPTION
 | 
 |     Define a parser symbol which containst the number string in one line of
 |     the factorization table and also the prime factorization itself.
 |     This is used in the parser's syntax directed translation and also for
 |     the final value returned from the parser.
 |
 +============================================================================*/

template< typename IntType >
class FactorizationValue
{
    public:
        FactorizationValue() ;
    
        FactorizationValue( const FactorizationValue<IntType> & v ) ;
    
        FactorizationValue( const IntType & p, const int & count ) ;

        FactorizationValue<IntType> & operator=( const FactorizationValue<IntType> & v ) ;
    
        // Print out the factorization.
        // Define this templated non-member friend function here, otherwise the compiler will generate
        // a link error.  If the compiler sees only a declaration here, and not the complete implementation,
        // it doesn't know yet it's a template.
        // See https://isocpp.org/wiki/faq/templates#template-friends
        friend ostream & operator<<( ostream & out, const FactorizationValue<IntType> & factorization )
        {
            if (factorization.factor_.size() > 0)
            {
                for (auto & f : factorization.factor_)
                    out << f ;
            }
            if (factorization.numberString_.length() > 0)
                out << factorization.numberString_ ;

            return out ;
        }
    
        static IntType numberStringToInteger( const string & numberString ) ;

    // Allow direct access to data for convenience.
    public:
        string                         numberString_ ;  // Sequence of digits representing an integer.
		vector< PrimeFactor<IntType> > factor_ ;        // Vector of distinct prime factors to powers.
} ;


/*=============================================================================
 |
 | NAME
 |
 |     FactorizationSymbols
 |
 | DESCRIPTION
 |
 |     Define factoriziton symbols (tokens).  Used in the parser's syntax directed
 |     translation.
 |
 +============================================================================*/

enum class FactorizationSymbol
{
    // Terminal symbols or tokens.
    Dollar = 0, Integer, Period, Caret, Backslash,
    NumTerminals,
    
    // Nonterminal symbols.
    S, Factorization, Factor, BigInteger,
    
    // Count number of total symbols.
    NumSymbols
} ;



/*=============================================================================
|
| NAME
|
|     FactorizationParser
|
| DESCRIPTION
|
|     Child class for parsing a factorization table of Cunningham numbers p ^ n - 1
|     inherited from abstract base class Parser.
|
| EXAMPLE
|
|     Each table for p has one line containing the factorization of p ^ n - 1 of the type
|
|          n <num factors p1 ^ m1 . p2 ^ m2 ...
|
|     Dots separate the factors.  Long numbers are continued on the next line with backslashes.
|     For example, in the table for p = 3, the line below is for n = 398 and has the 12 factors
|     2 ^ 4, 3, 3583 ... 11881576435363115110426317067813673631182057431802975257340612015708984828478898969687279497327343
|       
|         398    12  2^4.3.3583.4588543.34266607.2146612951394313997.8670122322845042\
|                    61471.3742361194240057889227626965547117.118815764353631151\
|                    104263170678136736311820574318029752573406120157089\
|                    84828478898969687279497327343
|
|     Here is the LALR(1) grammar.  
| 
|     Productions:
|
|         S             -> integer integer Factorization
|         Factorization -> Factorization . Factor | Factor
|         Factor        -> BigInteger ^ BigInteger | BigInteger
|         BigInteger    -> BigInteger \ integer | integer
|
|     Terminal symbols:
|         integer . ^ \ $
|
+============================================================================*/

template< typename SymbolType, typename ValueType >
class FactorizationParser : public Parser< SymbolType, ValueType >
{
    public:
        // Construct the parser.
        FactorizationParser() ;
        
        // Destructor.
        ~FactorizationParser() ;
        
        // Copy constructor.
        FactorizationParser( const FactorizationParser< SymbolType, ValueType > & parser ) ;
        
        // Assignment operator.
        FactorizationParser< SymbolType, ValueType > & operator=( const FactorizationParser< SymbolType, ValueType > & parser ) ;
        
    private:
        // Parse string into tokens.
        void tokenize( string sentence ) ;
        
        // Fill in the ACTION, GOTO, and ERROR_MESSAGE tables.
        void initializeTables() ;

        // Syntax directed translation.  Reduce using production A -> beta, computing A's value from values of 
        // the tokens in beta.
        void syntaxDirectedTranslation( int productionNum, int topIndex, Symbol<SymbolType,ValueType> & as ) ;

        // Even though these member variables are declared in the base class as protected, they are not visible in this
        // child class.  We have to tell the compiler explicitly that these symbols are base class templated types so it
        // knows what they are.
        using Parser<SymbolType, ValueType>::num_states_ ;
        using Parser<SymbolType, ValueType>::num_productions__ ;
        using Parser<SymbolType, ValueType>::num_terminals_ ;
        using Parser<SymbolType, ValueType>::num_non_terminals_ ;

        using Parser<SymbolType, ValueType>::insertAction ;
        using Parser<SymbolType, ValueType>::insertGoto ;
        using Parser<SymbolType, ValueType>::insertProduction ;
        using Parser<SymbolType, ValueType>::insertErrorMessage ;

        using Parser< SymbolType, ValueType >::input_stack_ ;
        using Parser< SymbolType, ValueType >::parse_stack_ ;
        using Parser< SymbolType, ValueType >::action_table_ ;
        using Parser< SymbolType, ValueType >::goto_table_ ;
        using Parser< SymbolType, ValueType >::error_message_ ;
        using Parser< SymbolType, ValueType >::production_ ;
} ;

#endif // __PPPARSER_H__
