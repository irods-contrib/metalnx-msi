#include "reVocabulary.hpp"

const char* find_vocab_under_irods_path(char* irods_path, char* dest_resource, ruleExecInfo_t* rei) {
	if (irods_path == NULL) {
		return NULL;
	}

	collEnt_t* collEnt = NULL;
	collInp_t collInp;
	bzero( &collInp, sizeof( collInp ) );
	collInp.flags = RECUR_QUERY_FG | VERY_LONG_METADATA_FG | NO_TRIM_REPL_FG | INCLUDE_CONDINPUT_IN_QUERY;
	rstrcpy( collInp.collName, irods_path, MAX_NAME_LEN );
	addKeyVal( &collInp.condInput, RESC_NAME_KW, dest_resource );

	int handleInx = rsOpenCollection( rei->rsComm, &collInp );

	if ( handleInx < 0 ) {
		rodsLog( LOG_ERROR,
				 "%s find_vocab_under_irods_path: rsOpenCollection of %s error. status = %d",
				 VOCABULARY_MSI_LOG, irods_path, handleInx );
		return NULL;
	}

	int status = rsReadCollection( rei->rsComm, &handleInx, &collEnt );

	while ( ( status = rsReadCollection( rei->rsComm, &handleInx, &collEnt ) ) >= 0 ) {
		if ( NULL == collEnt ) {
			rodsLog(LOG_ERROR, "%s find_vocab_under_irods_path rsReadCollection: collEnt is NULL", VOCABULARY_MSI_LOG );
			continue;
		}

		if ( collEnt->objType != DATA_OBJ_T ) {
		    continue;
		}

        std::string data_obj_name(collEnt->dataName);

        if (data_obj_name.compare("test.vocab") == 0) {
            rodsLog(LOG_NOTICE,
                    "%s Vocabulary %s found in %s\n",
                    VOCABULARY_MSI_LOG, data_obj_name.c_str(), irods_path);
            return data_obj_name.c_str();
        }
	}

	rsCloseCollection( rei->rsComm, &handleInx );

	return NULL;
}

char* find_vocab_phy_path(char* irods_obj_path) {
    char* db_path = (char*) malloc(MAX_STR_LEN * sizeof(char));
    memset(db_path, 0, MAX_STR_LEN);

    strcat (db_path, find_vocab_phy_dir(irods_obj_path));
    strcat (db_path, "/test.vocab");

    rodsLog(LOG_NOTICE, "%s Vocabulary file: %s\n", VOCABULARY_MSI_LOG, db_path);

    return db_path;
}

char* find_vocab_phy_dir(char* irods_obj_path) {
    char* dir = (char*) malloc(MAX_STR_LEN * sizeof(char));
    memset(dir, 0, MAX_STR_LEN);

    strcpy (dir, VOCABULARIES_BASE_DIR);
    strcat (dir, irods_obj_path);

    rodsLog(LOG_NOTICE, "%s Vocabulary dir: %s\n", VOCABULARY_MSI_LOG, dir);

    return dir;
}

char* find_vocab_irods_path(char* irods_dir_path, ruleExecInfo_t* rei) {
    const char* vocab_name = find_vocab_under_irods_path (irods_dir_path, "demoResc", rei);

    if(vocab_name == NULL) return NULL;

    char* db_path = (char*) malloc(MAX_STR_LEN * sizeof(char));
    memset(db_path, 0, MAX_STR_LEN);

    strcat (db_path, irods_dir_path);
    strcat (db_path, "/");
    strcat (db_path, vocab_name);

    rodsLog(LOG_NOTICE, "%s Vocabulary file: %s\n", VOCABULARY_MSI_LOG, db_path);

    return db_path;
}

bool add_metadata_to_vocabulary(char* irods_obj_path, char* attr_name, char* attr_unit, char* attr_type, ruleExecInfo_t* rei) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char sql[MAX_STR_LEN];
    const char* data = "Callback function called";

    if (find_vocab_under_irods_path (irods_obj_path, "demoResc", rei) == NULL) {
        rodsLog(LOG_NOTICE, "%s Vocabulary does not exist.\n", VOCABULARY_MSI_LOG);
        return false;
    }

    db = open_db_connection(find_vocab_phy_path(irods_obj_path));

    sprintf(sql, INSERT_INTO_VOCABULARY_METADATA, attr_name, attr_unit, attr_type);

	rodsLog(LOG_NOTICE, "%s %s", VOCABULARY_MSI_LOG, sql);

	rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);

	if( rc != SQLITE_OK ) {
        rodsLog(LOG_ERROR,
                "%s Could insert metadata to a vocabulary in %s: %s\n",
                VOCABULARY_MSI_LOG, irods_obj_path, sqlite3_errmsg(db));
        sqlite3_free(zErrMsg);
        return false;
    }

    rodsLog(LOG_NOTICE, "%s Metadata added successfully.\n", VOCABULARY_MSI_LOG);

    close_db_connection(db);

    return true;
}

bool remove_vocab_metadata(char* irods_obj_path, char* attr_name) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char sql[MAX_STR_LEN];
    const char* data = "Callback function called";

    db = open_db_connection(find_vocab_phy_path(irods_obj_path));

	sprintf(sql, DELETE_FROM_VOCAB_METADATA, attr_name);

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

bool unlink_obj(const char* irods_obj_path, ruleExecInfo_t* rei) {
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

bool rm_file(char* phy_path) {
    return remove( phy_path ) != 0;
}

bool remove_vocabulary(char* irods_obj_path, ruleExecInfo_t* rei) {
    char* vocab_irods_path = find_vocab_irods_path(irods_obj_path, rei);

    if (vocab_irods_path == NULL) {
        rodsLog(LOG_NOTICE,
                "%s No vocabulary found in %s. Remove operation cannot be completed.\n",
                VOCABULARY_MSI_LOG, irods_obj_path);
        return 0;
    }

    unlink_obj(vocab_irods_path, rei);

    rm_file(find_vocab_phy_path(irods_obj_path));

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

bool create_vocabulary (char* irods_obj_path, char* vocab_name, char* vocab_author, ruleExecInfo_t* rei) {
	if (irods_obj_path == NULL || vocab_name == NULL || vocab_author == NULL) {
		rodsLog( LOG_ERROR, "%s Could not create Vocabulary struct. NULL parameter provided.", VOCABULARY_MSI_LOG );
		return false;
	}

	if (find_vocab_under_irods_path (irods_obj_path, "demoResc", rei) != NULL) {
        rodsLog(LOG_NOTICE, "%s Could not create a new Vocabulary. Another one already exists.\n", VOCABULARY_MSI_LOG);
        return false;
    }

	boost::filesystem::path dir(find_vocab_phy_dir(irods_obj_path));
    boost::filesystem::create_directories(dir);

    sqlite3* db = open_db_connection(find_vocab_phy_path(irods_obj_path));

    create_vocabulary_database_schema(db);

    int rc;
    char* zErrMsg;
    char sql[MAX_STR_LEN];

	sprintf(sql, INSERT_INTO_VOCABULARIES, vocab_name, vocab_author);

	rodsLog(LOG_NOTICE, "%s %s", VOCABULARY_MSI_LOG, sql);

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if( rc != SQLITE_OK ) {
        rodsLog(LOG_ERROR, "%s Could not insert vocabulary info into the database: %s.\n", VOCABULARY_MSI_LOG, zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else {
        rodsLog(LOG_NOTICE, "%s Vocabulary info inserted into database successfully.\n", VOCABULARY_MSI_LOG);
    }

    sqlite3_exec(db, SELECT_ALL_FROM_VOCABULARIES, callback, 0, &zErrMsg);

    close_db_connection(db);

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

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
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

    int  rc;
    char *zErrMsg = 0;

    rodsLog(LOG_NOTICE, "%s %s", VOCABULARY_MSI_LOG, CREATE_TABLE_VOCABULARY);

    rc = sqlite3_exec(db, CREATE_TABLE_VOCABULARY, callback, 0, &zErrMsg);

    if( rc != SQLITE_OK ){
        rodsLog(LOG_ERROR, "%s Could not Create Table VOCABULARIES. SQL error: %s\n", VOCABULARY_MSI_LOG, zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else {
        rodsLog(LOG_NOTICE, "%s VOCABULARIES Table Created Successfully.\n", VOCABULARY_MSI_LOG);
    }

    rodsLog(LOG_NOTICE, "%s %s", VOCABULARY_MSI_LOG, CREATE_TABLE_VOCABULARY_METADATA);

    rc = sqlite3_exec(db, CREATE_TABLE_VOCABULARY_METADATA, callback, 0, &zErrMsg);

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