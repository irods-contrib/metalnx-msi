#ifndef RE_VOCABULARY_HPP
#define RE_VOCABULARY_HPP

#include "rods.h"
#include "reFuncDefs.hpp"
#include "apiHeaderAll.h"
#include "miscServerFunct.hpp"

#include<sqlite3.h>
#include <stdio.h>

#define VOCABULARY_MSI_LOG "[Metalnx Vocabulary MSI]"

/*
 * Remove a vocabulary from a given path in the grid and from the file system.
 * */
bool remove_vocabulary(char*, char*, ruleExecInfo_t*);

/*
 * Open up a connection with a Sqlite3 database existing in the given path.
 * */
sqlite3* open_db_connection(char*);

/*
 * Closes a Sqlite3 database connection.
 * */
void close_db_connection(sqlite3*);

/*
 * Creates a vocabulary entry in the database
 * */
bool create_vocabulary(sqlite3*, char*, char*, char*);

/*
 * Checks whether or not exist a vocabulary in the given path
 * */
bool is_there_any_vocab_under_path (char* vocab_irods_path, ruleExecInfo_t* rei);

/*
 * Auxiliar function called by sqlite when an operation in the database is done.
 * */
static int callback(void*, int, char**, char**);

/*
 * Creates a SQLite database schema for a vocabulary
 * */
void create_vocabulary_database_schema(sqlite3*);

#endif /*RE_VOCABULARY_HPP*/