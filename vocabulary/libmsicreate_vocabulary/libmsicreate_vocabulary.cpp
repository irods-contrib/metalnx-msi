//==============================================================================
// Name        : Illumina.cpp
// Version     : 1.0
// Copyright   : 2015-2016 EMC Corporation. All rights reserved.
// Description : Illumina Microservice used to extract metadata from the Sample
//				 Sheet csv file and apply it to all files in a run.
//==============================================================================
#include "rods.h"
#include "reFuncDefs.hpp"
#include "miscServerFunct.hpp"

#include<sqlite3.h>

#include<fstream>

#define VOCABULARY_MSI_LOG "[Metalnx Vocabulary MSI]"

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
bool is_there_any_vocab_under_path (char* path);

/*
 * Auxiliar function called by sqlite when an operation in the database is done.
 * */
static int callback(void*, int, char**, char**);

/*
 * Creates a SQLite database schema for a vocabulary
 * */
void create_vocabulary_database_schema(sqlite3*);

extern "C" {
    int msicreate_vocabulary(msParam_t* _objPath, msParam_t* _vocabName, msParam_t* _vocabAuthor, ruleExecInfo_t* rei) {
    	rodsLog(LOG_NOTICE, "%s Metalnx Vocabulary Microservice called.", VOCABULARY_MSI_LOG);

    	char* obj_path = parseMspForStr(_objPath);
    	char* vocab_name = parseMspForStr(_vocabName);
    	char* vocab_author = parseMspForStr(_vocabAuthor);
    	char* vocab_path = "/etc/irods/test.vocab";

        if (is_there_any_vocab_under_path (obj_path)) {
            rodsLog(LOG_NOTICE, "%s Could not create a new Vocabulary. Another one already exists.\n", VOCABULARY_MSI_LOG);
            return -1;
        }

        sqlite3* db = open_db_connection(vocab_path);

    	create_vocabulary_database_schema(db);

        if (!create_vocabulary(db, obj_path, vocab_name, vocab_author)) {
            return -1;
        }

        close_db_connection(db);

        return 0;
    }

    irods::ms_table_entry* plugin_factory() {
        irods::ms_table_entry* msvc = new irods::ms_table_entry(3);
        msvc->add_operation("msicreate_vocabulary", "msicreate_vocabulary");
        return msvc;
    }
}

sqlite3* open_db_connection(char* db_path) {
    rodsLog(LOG_NOTICE, "%s Opening a connection to the vocabulary database in %s\n", VOCABULARY_MSI_LOG, db_path);

    if (db_path == NULL) {
        rodsLog(LOG_NOTICE, "%s Could not open connection to the Vocabulary database. Path cannot be null.\n", VOCABULARY_MSI_LOG);
    }

    sqlite3 *db = NULL;
    int rc;

    rc = sqlite3_open(db_path, &db);

    if( rc ) {
      rodsLog(LOG_NOTICE, "%s Could not open Vocabulary database. %s\n", VOCABULARY_MSI_LOG, sqlite3_errmsg(db));
    }
    else {
      rodsLog(LOG_NOTICE, "%s Vocabulary database opened successfully. %s\n", VOCABULARY_MSI_LOG, sqlite3_errmsg(db));
    }

    return db;
}

void close_db_connection(sqlite3* db) {
    if(db) sqlite3_close(db);
}

bool create_vocabulary (sqlite3* db, char* obj_path, char* vocab_name, char* vocab_author) {
	if (db == NULL || obj_path == NULL || vocab_name == NULL || vocab_author == NULL) {
		rodsLog( LOG_ERROR, "%s Could not create Vocabulary struct. NULL parameter provided.", VOCABULARY_MSI_LOG );
		return false;
	}

    int rc;
    char* zErrMsg;
    char sql[50];

	sprintf(sql,
	        "INSERT INTO VOCABULARIES (NAME, AUTHOR) VALUES (\'%s\', \'%s\')",
	        vocab_name, vocab_author);

	rodsLog(LOG_NOTICE, "%s %s", VOCABULARY_MSI_LOG, sql);

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if( rc != SQLITE_OK ) {
        rodsLog(LOG_ERROR, "%s Could not insert vocabulary info into the database: %s.\n", VOCABULARY_MSI_LOG, zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else {
        rodsLog(LOG_NOTICE, "%s Vocabulary info inserted into database successfully.\n", VOCABULARY_MSI_LOG);
    }

    sqlite3_exec(db, "SELECT * FROM VOCABULARIES", callback, 0, &zErrMsg);

	return true;
}

bool is_there_any_vocab_under_path (char* path) {
    // TODO - Implement it!

    return false;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;

   for(i = 0; i < argc; i++){
      rodsLog(LOG_NOTICE, "%s %s = %s\n", VOCABULARY_MSI_LOG, azColName[i], argv[i] ? argv[i] : "NULL");
   }

   return 0;
}

void create_vocabulary_database_schema(sqlite3* db) {
    if (db == NULL) {
		rodsLog( LOG_ERROR, "%s Could not create Vocabulary schema. NULL params provided.", VOCABULARY_MSI_LOG );
		return;
    }

    char *zErrMsg = 0;
    int  rc;
    char* sql =  "CREATE TABLE VOCABULARIES ("  \
                 "NAME           text           not null," \
                 "AUTHOR         text           not null," \
                 "CREATED_AT     datetime       default current_timestamp," \
                 "MODIFIED_AT    datetime       default current_timestamp );";

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
        rodsLog(LOG_ERROR, "%s Could not Create Table VOCABULARIES. SQL error: %s\n", VOCABULARY_MSI_LOG, zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else {
        rodsLog(LOG_NOTICE, "%s VOCABULARIES Table Created Successfully.\n", VOCABULARY_MSI_LOG);
    }

    sql =  "CREATE TABLE VOCABULARY_METADATA ("  \
           "ATTR      text      not null," \
           "VALUE     text      not null," \
           "UNIT      text      not null," \
           "TYPE      text      not null );";

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
        rodsLog(LOG_ERROR, "%s Could not Create Table VOCABULARY_METADATA. SQL error: %s\n", VOCABULARY_MSI_LOG, zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else {
        rodsLog(LOG_NOTICE, "%s VOCABULARY_METADATA Table Created Successfully.\n", VOCABULARY_MSI_LOG);
    }

    // TODO - SQL Statement for the Vocabulary Auditing table

    return;
}