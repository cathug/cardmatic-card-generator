//    Cardmatic card generator - cardmatic_sql.h file
//    C++11 header file

//    SQLite API C++ wrapper to access database file "cardmatic.sqlite".
//      More than 6000 tubes listed in the AVO VCM163 test data subset can be
//      replicated with a Cardmatic Tube Tester.

//    Written by: cathug


#ifndef CARDMATIC_SQL_H
#define CARDMATIC_SQL_H

#include <sqlite3.h>
#include <string>
//#include "cardmatic_tube.h"
//#include "cardmatic_globals.h"





#define NUM_TEXT_COLS_PER_ROW 5
#define NUM_DOUBLE_COLS_PER_ROW 6

//------------------------------------------------------------------------------
//  enum
//------------------------------------------------------------------------------

typedef enum sqlops
{
	CREATE_TABLE,
	INSERT_INTO_TABLE,
	SELECT_TABLE,
	DELETE_TABLE,
	COUNT,
}SQLops;



//------------------------------------------------------------------------------
//  Class
//------------------------------------------------------------------------------

class Database
{    
    public:
        //----------------------------------------------------------------------
        //  constructor and destructor
        //----------------------------------------------------------------------
      
        Database();                             // default constructor
 
        ~Database();                            // destructor


        //----------------------------------------------------------------------                                                          
        //  member functions
        //----------------------------------------------------------------------
       
        // open database file in read-only mode
        // param:   filename - name of database file in UTF-8
        //          flag - i.e. SQLITE_OPEN_READONLY, 
        //                    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE ...
        // post: returns true if file opens successfully, otherwise return false
        bool dbOpen(const char* fileName, 
                    int flag);


        // close database file
        // pre: must be called after sqlite3_finalize
        // post: no memory leaks found after executing function
        void dbClose();


        // execute static query
        // post: delete query when program completes executing
        //vector<columnData> Database::dbQuery(char* query, list<Socket> &tube)
        void dbQuery(const char* param[], 
                     int paramSize,
                     std::string tableName);
        
        
        
        //----------------------------------------------------------------------                                                          
        //  accessors
        //----------------------------------------------------------------------
        
        double getTubeData_doubleAttribute(unsigned int row, 
                                           unsigned int column) const
        { 
            return tubeData_double[column + NUM_TEXT_COLS_PER_ROW * row];
        }
        
        
        std::string getTubeData_strAttribute(unsigned int row, 
                                             unsigned int column) const
        {
            return tubeData_str[column + NUM_TEXT_COLS_PER_ROW * row]; 
        }
        

        unsigned int getNumRowsReturned() const { return num_rows_returned; }
        
        
        std::string* getTubeData_str() const { return tubeData_str; }
        
        
        double* getTubeData_double() const { return tubeData_double; }
        

    private:
        //----------------------------------------------------------------------
        //  variables
        //----------------------------------------------------------------------
        
        sqlite3* database;
        sqlite3_stmt* statement;
        int return_code;	                    // return code
        std::string sql;     	                // string containing sql query
        std::string* tubeData_str;
        double* tubeData_double;
        unsigned int num_rows_returned;
        
        
        
        //----------------------------------------------------------------------
        //  helpers
        //----------------------------------------------------------------------
        
        // chooses query to execute
        // pre: flag must be SQLITE_OPEN_READWRITE 
        //      or SQLITE_OPEN_READWRITE || SQLITE_OPEN_CREATE if operation = 
        //      CREATE_TABLE, INSERT_INTO_TABLE, DELETE
        // param:   operation - chooses which sql query to execute
        //                          CREATE_TABLE, INSERT_INTO_TABLE,
        //                          SELECT_TABLE, DELETE_TABLE, COUNT
        //          tableName - table name
        // post: binding is required before query is evaluated
        bool selectPredefinedQuery(SQLops operation,
                                   std::string tableName);
        
        
        // pre: SQL statement should be valid.  Binding variables allowed.
        // returns: true if preparation is succesful, false otherwise
        bool prepareQuery();
        
        
        // function to bind parameters to placeholders in sql statement
        // pre: SQL statement should be valid.  It can contain placeholder 
        //      variables.  Function should be called after prepareQuery()
        // param:   param - an array of c-style strings to bind to statenebt
        //          paramSize- number of strings in the array  
        // returns: true if preparation is succesful, false otherwise
        bool bindQuery(const char* param[],
                       int paramSize);
        
        
        // function to evaluate query
        // pre: SQL statement should be valid, containing no placeholders.
        //      Function should be called after prepareQuery() and bindQuery()
        // returns: 0 if no rows returned, 1 if at least one row returned, or 
        //          -1 if failed to evaluate query
        int evaluateQuery();
};        


#endif // CARDMATIC_SQL_H
