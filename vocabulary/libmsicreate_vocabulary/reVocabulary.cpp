#include "reVocabulary.hpp"

bool remove_vocab_metadata(char* irods_obj_path, char* attr_name, ruleExecInfo_t* rei) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char sql[50];
    const char* data = "Callback function called";

    db = open_db_connection("/etc/irods/test.vocab");

	sprintf(sql, "delete from vocabulary_metadata where attr=\'%s\';", attr_name);

	rodsLog(LOG_NOTICE, "%s %s", VOCABULARY_MSI_LOG, sql);

    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);

    if( rc != SQLITE_OK ) {
        rodsLog(LOG_ERROR,
                "%s Could not delete metadata from vocabulary in %s: %s\n",
                VOCABULARY_MSI_LOG, irods_obj_path, sqlite3_errmsg(db));
        sqlite3_free(zErrMsg);
        return false;
    }

    rodsLog(LOG_NOTICE,
            "%s Vocabulary metadata removed successfully from vocabulary in %s\n",
            VOCABULARY_MSI_LOG, irods_obj_path);

    close_db_connection(db);

    return true;
}

bool unlink_obj(char* irods_obj_path, ruleExecInfo_t* rei) {
    dataObjInp_t dataObjInp;
    char *outBadKeyWd = NULL;
    int validKwFlags;
    msParam_t* inpParam = (msParam_t*) malloc(sizeof(msParam_t));
    inpParam->type = STR_MS_T;
    inpParam->inOutStruct = (void*) irods_obj_path;

    bzero( &dataObjInp, sizeof( dataObjInp ) );
    validKwFlags = OBJ_PATH_FLAG | RESC_NAME_FLAG | OPEN_FLAGS_FLAG | REPL_NUM_FLAG;
    parseMsKeyValStrForDataObjInp( inpParam, &dataObjInp, OBJ_PATH_KW, validKwFlags, &outBadKeyWd );
    if (rsDataObjUnlink( rei->rsComm, &dataObjInp ) != 0) {
        rodsLog(LOG_ERROR, "%s Could not unlink Vocabulary file from the grid.", VOCABULARY_MSI_LOG );
        return false;
    }

    return true;
}

bool remove_vocabulary(char* phy_path, char* irods_obj_path, ruleExecInfo_t* rei) {
    if (!is_there_any_vocab_under_path (irods_obj_path, rei)) {
        rodsLog(LOG_NOTICE,
                "%s No vocabulary found in %s. Remove operation cannot be completed.\n",
                VOCABULARY_MSI_LOG, irods_obj_path);
        return 0;
    }

    unlink_obj(irods_obj_path, rei);

    // Remove vocabulary file from the file system
    if( remove( phy_path ) != 0 ) {
        rodsLog(LOG_ERROR, "%s Could not delete Vocabulary file from the file system.", VOCABULARY_MSI_LOG );
        return false;
    }

    rodsLog(LOG_NOTICE, "%s Vocabulary file removed from the file system successfully.", VOCABULARY_MSI_LOG );
    return true;
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

bool is_there_any_vocab_under_path (char* vocab_irods_path, ruleExecInfo_t* rei) {
    dataObjInp_t dataObjInp;
    char *outBadKeyWd = NULL;
    int validKwFlags;
    msParam_t* inpParam = (msParam_t*) malloc(sizeof(msParam_t));
    inpParam->type = STR_MS_T;
    inpParam->inOutStruct = (void*) vocab_irods_path;

    bzero( &dataObjInp, sizeof( dataObjInp ) );
    validKwFlags = OBJ_PATH_FLAG | RESC_NAME_FLAG | OPEN_FLAGS_FLAG | REPL_NUM_FLAG;
    parseMsKeyValStrForDataObjInp( inpParam, &dataObjInp, OBJ_PATH_KW, validKwFlags, &outBadKeyWd );
    return rsDataObjOpen( rei->rsComm, &dataObjInp ) >= 0;
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