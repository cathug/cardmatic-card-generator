//    Cardmatic card generator - cardmatic_sql.h file
//    C++11 implementation file

//    SQLite API C++ wrapper to access database file "cardmatic.sqlite".
//      More than 6000 tubes listed in the AVO VCM163 test data subset can be
//      replicated with a Cardmatic Tube Tester.

//    Written by: cathug




#include <iostream>
#include <cstring>
#include "cardmatic_sql.h"
#include <fstream>
#include <cmath>    // for nan



//using std::ifstream;
//using std::streampos;
//using std::ios;


//------------------------------------------------------------------------------
// Implementation
//------------------------------------------------------------------------------

// default constructor
Database::Database()  
{

}

//------------------------------------------------------------------------------

// destructor
Database::~Database()
{
//    if (database != NULL) { dbClose(); }
    if (tubeData_double != NULL) { delete[] tubeData_double; }
    if (tubeData_str != NULL) { delete[] tubeData_str; }
}

//------------------------------------------------------------------------------

// open database file in read-only mode
// param:   filename - name of database file in UTF-8
//          flag - i.e. SQLITE_OPEN_READONLY, 
//                    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE ...
// post: returns true if file opens successfully, otherwise return false
bool Database::dbOpen(const char* fileName, int flag)
{
    return_code = sqlite3_open_v2(fileName, &database, flag, NULL);
//	return_code = sqlite3_open_v2(fileName, &database, flag, "unix");
	// replace with line above if defauly SQLite vfs not working properly

    if (return_code != SQLITE_OK)
    {
        std::cerr << "Failed to open database: " << 
        	sqlite3_errmsg(database) << std::endl;
        return false;
    }

    std::cout << "Database opened successfully." << std::endl;
    return true;
}

//------------------------------------------------------------------------------

// close database file
// pre: must be called after sqlite3_finalize
// post: no memory leaks found after executing function
void Database::dbClose()
{
    return_code = sqlite3_close(database);
    if (return_code != SQLITE_OK)
    {
        std::cerr << "Failed to close database: " << 
        	sqlite3_errmsg(database) << std::endl;
        return;
    }

    std::cout << "Database closed successfully." << std::endl;
}

//------------------------------------------------------------------------------

// chooses query to execute
// pre: flag must be SQLITE_OPEN_READWRITE 
//      or SQLITE_OPEN_READWRITE || SQLITE_OPEN_CREATE if operation = 
//      CREATE_TABLE, INSERT_INTO_TABLE, DELETE
// param:   operation - chooses which sql query to execute
//                          CREATE_TABLE, INSERT_INTO_TABLE, SELECT_TABLE, 
//                          DELETE_TABLE, COUNT
//          tableName - table name
// post: binding is required before query is evaluated
bool Database::selectPredefinedQuery(SQLops operation, 
                                     std::string tableName)
{
    // choose a sql query
    switch (operation)
    {
        case CREATE_TABLE:
            sql = "create table " + tableName + " (TubeID TEXT PRIMARY KEY,"
                  " closedSW TEXT, test TEXT, testNum INTEGER PRIMARY KEY);";
            break;
        
        case INSERT_INTO_TABLE:
            sql = "insert into " + tableName + 
                " (TubeID, closedSW) values($tubeID, $closedSW);";
            break;
        
        case SELECT_TABLE:
            sql = "select * from " + tableName + " where TubeID = $tubeID";
            break;
            
        case DELETE_TABLE:
            sql = "delete from " + tableName + " where TubeID = $tubeID";
            break;
            
        case COUNT:
            sql = "select count(*) as count from " + tableName + 
                " where TubeID = $tubeID";
            break;
        
        default:
            return false;
    }

//    std::cout << sql << std::endl;
    return true;
}

//------------------------------------------------------------------------------

// pre: SQL statement should be valid.  Binding variables allowed.
// returns: true if preparation is succesful, false otherwise
bool Database::prepareQuery()
{
    return_code = sqlite3_prepare_v2(database, sql.c_str(), sql.length(), 
        &statement, NULL);
    
//    std::cout <<"return code is " << return_code << std::endl;

    if (return_code != SQLITE_OK)   // if prepare failed
    {
        std::cerr << "Failed to prepare query. " 
            << sqlite3_errmsg(database) << std::endl;
        return false;
    }
    
    return true;
}

//------------------------------------------------------------------------------

// function to bind parameters to placeholders in sql statement
// pre: SQL statement should be valid.  It can contain placeholder variables.
//      Function should be called after prepareQuery()
// param:   param - an array of c-style strings to bind to statenebt
//          paramSize- number of strings in the array  
// returns: true if preparation is succesful, false otherwise
bool Database::bindQuery(const char* param[],
                         int paramSize)
{
    if (sqlite3_bind_parameter_count(statement) != paramSize) { return false; }
    
    for (int i = 0; i < paramSize; i++)
    {
        return_code = sqlite3_bind_text(statement, i + 1, 
            param[i], strlen(param[i]), 
            SQLITE_STATIC); // try SQLITE_TRANSIENT if not working
        
        if (return_code != SQLITE_OK)
        {
            std::cerr << "Failed to bind parameter " 
                << i << " to query. " << sqlite3_errmsg(database) << std::endl;
            return false;
        }
    }
    
    return true;
}

//------------------------------------------------------------------------------

// function to evaluate query
// pre: SQL statement should be valid, containing no placeholders.
//      Function should be called after prepareQuery() and bindQuery()
// returns: 0 if no rows returned, 1 if at least one row returned, or 
//          -1 if failed to evaluate query
int Database::evaluateQuery()
{
    return_code = sqlite3_step(statement);

    // CREATE_TABLE, INSERT_INTO_TABLE, DELETE_TABLE, or if query returns null
    if (return_code == SQLITE_DONE) { return 0; }
    if (return_code != SQLITE_ROW)  // transaction is not ok
    {
        std::cerr << "Failed to evaluate query. " 
            << sqlite3_errmsg(database) << std::endl;
        return -1;
    }
    
    return 1;
}

//------------------------------------------------------------------------------

// execute static query
// post: delete query when program completes executing
//vector<columnData> Database::dbQuery(char* query, list<Socket> &tube)
void Database::dbQuery(const char* param[],
                       int paramSize,
                       std::string tableName)
{
    int i, num_columns, type_check;
    unsigned int j = 0;
    unsigned int k = 0;
    
    
    
    // get row count
    selectPredefinedQuery(COUNT, tableName);
    if (prepareQuery() == false) { return; }
    if (bindQuery(param, paramSize) == false || evaluateQuery() < 1)
    { 
        if (statement != NULL) { sqlite3_finalize(statement); }
        return; 
    }    
    
    num_rows_returned = sqlite3_column_int(statement, 0);
    sqlite3_finalize(statement);     // finalize statement to deallocate
    if (num_rows_returned  == 0 ) { return; }
    
    

    // if row count > 0
    // select, prepare, bind, and evaluate query
    selectPredefinedQuery(SELECT_TABLE, tableName);
    if (prepareQuery() == false) { return; }
    if (bindQuery(param, paramSize) == false || evaluateQuery() < 1)
    { 
        if (statement != NULL) { sqlite3_finalize(statement); }
        return; 
    }  
    
    
    // process rows returned
    num_columns = sqlite3_column_count(statement);
    tubeData_str = new std::string[num_rows_returned * NUM_TEXT_COLS_PER_ROW];
    if (!tubeData_str) { return; }
    
    tubeData_double = new double[num_rows_returned * NUM_DOUBLE_COLS_PER_ROW];
    if (!tubeData_double) { return; }
    
    
    
    // Do the following if query just executed contains data.
    // Second condition check (k) is probably redundant and can be removed
    while (return_code == SQLITE_ROW && k < num_rows_returned)          
    {
        // print entry
        j = 0;
        for (i = 0; i < num_columns; i++)
        {
            type_check = sqlite3_column_type(statement, i);
            // makes sure no out of bounce error in case table is appended
            if (i >= 3 && i <= 8 && i - 3 < NUM_DOUBLE_COLS_PER_ROW)    
            {
                if (type_check == SQLITE_FLOAT)
                {
                    tubeData_double[i - 3 + k * NUM_DOUBLE_COLS_PER_ROW] = 
                        sqlite3_column_double(statement, i);       
                }
                
                else if (type_check == SQLITE_NULL)
                {
                    tubeData_double[i - 3 + k * NUM_DOUBLE_COLS_PER_ROW] = 
                        nan("null entry");                
                }
                
                std::cout << 
                    tubeData_double[i - 3 + k * NUM_DOUBLE_COLS_PER_ROW] << 
                    std::endl;
            }
            
            else
            {
                // makes sure no out of bounce error in case table is appended
                if (j < NUM_TEXT_COLS_PER_ROW)  
                {
                    // the following two lines are not really needed
                    // as columns have to be non-null,
                    // but included to deal with type affinity in SQLite
                    if (type_check == SQLITE_NULL)
                    {
                        tubeData_str[j + k * NUM_TEXT_COLS_PER_ROW] = "";
                    }
                    
                    else if (type_check == SQLITE_TEXT)
                    {
                        tubeData_str[j + k * NUM_TEXT_COLS_PER_ROW] = 
                            reinterpret_cast<char*>(const_cast<unsigned char*>(
                                sqlite3_column_text(statement, i) ) );
                        std::cout << 
                            tubeData_str[j + k * NUM_TEXT_COLS_PER_ROW] << 
                            std::endl;
                    }
                                              
                    j++;
                }
            }
        }
        
//        std::cout << "---end of entry---" << std::endl;
        evaluateQuery();    // process next row
        k++;
    }

    sqlite3_finalize(statement);     // finalize statement to deallocate
}

//------------------------------------------------------------------------------
// End of implementation
//------------------------------------------------------------------------------
