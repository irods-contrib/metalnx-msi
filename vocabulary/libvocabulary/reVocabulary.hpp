#ifndef RE_VOCABULARY_HPP
#define RE_VOCABULARY_HPP

#include "rods.h"
#include "reFuncDefs.hpp"
#include "apiHeaderAll.h"
#include "miscServerFunct.hpp"

#include<sqlite3.h>
#include <stdio.h>

#include <iostream>
#include <boost/filesystem.hpp>

#define MAX_STR_LEN 200
#define VOCABULARY_MSI_LOG "[Metalnx Vocabulary MSI]"
#define CREATE_TABLE_VOCABULARY  "CREATE TABLE VOCABULARIES ("  \
                                 "NAME           text           not null," \
                                 "AUTHOR         text           not null," \
                                 "CREATED_AT     datetime       default current_timestamp," \
                                 "MODIFIED_AT    datetime       default current_timestamp );"

#define CREATE_TABLE_VOCABULARY_METADATA "CREATE TABLE VOCABULARY_METADATA ("  \
                                         "ATTR      text      not null," \
                                         "UNIT      text      not null," \
                                         "TYPE      text      not null );"

#define SELECT_ALL_FROM_VOCABULARIES "SELECT * FROM VOCABULARIES"
#define INSERT_INTO_VOCABULARIES "INSERT INTO VOCABULARIES (NAME, AUTHOR) VALUES (\'%s\', \'%s\')"
#define INSERT_INTO_VOCABULARY_METADATA "INSERT INTO VOCABULARY_METADATA (ATTR, UNIT, TYPE) VALUES (\'%s\', \'%s\', \'%s\');"
#define DELETE_FROM_VOCAB_METADATA "delete from vocabulary_metadata where attr=\'%s\';"

#define VOCABULARIES_BASE_DIR "/etc/irods/vocabularies"


char* find_vocab_irods_path(char*);

/*
 * Finds the directory in the local file system where the vocabulary database file is.
 * */
char* find_vocab_phy_dir(char* irods_obj_path);

/*
 * Removes a file from the local file system.
 * */
bool rm_file(char*);

/*
 * Resolves the vocabulary database physical path based on an iRODS path.
 * */
char* find_vocab_phy_path(char*);

/*
 * Adds metadata (AVU) to a vocabulary existing in a path.
 * */
bool add_metadata_to_vocabulary(char*, char*, char*, char*, ruleExecInfo_t*);

/*
 * Removes a metadata from a vocabulary based on its attribute name.
 * */
bool remove_vocab_metadata(char*, char*);

/*
 * Unlinks a data object from the grid.
 * */
bool unlink_obj(char* irods_obj_path, ruleExecInfo_t* rei);

/*
 * Remove a vocabulary from a given path in the grid and from the file system.
 * */
bool remove_vocabulary(char*, ruleExecInfo_t*);

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
bool create_vocabulary(char*, char*, char*, ruleExecInfo_t*);

/*
 * Checks whether or not exist a vocabulary in the given path
 * */
bool is_there_any_vocab_under_path (char* vocab_irods_path, ruleExecInfo_t* rei);

/*
 * Auxiliar function called by sqlite when an operation in the database is done.
 * */
int callback(void*, int, char**, char**);

/*
 * Creates a SQLite database schema for a vocabulary
 * */
void create_vocabulary_database_schema(sqlite3*);

#endif /*RE_VOCABULARY_HPP*/