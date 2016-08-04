//==============================================================================
// Name        : msicreate_vocabulary.cpp
// Version     : 1.0
// Copyright   : 2015-2016 EMC Corporation. All rights reserved.
// Description : Create vocabulary microservice responsible for creating a
//               vocabulary in a given path of the grid.
//==============================================================================
#include "reVocabulary.hpp"

extern "C" {
    int msicreate_vocabulary(msParam_t* _objPath, msParam_t* _vocabName, msParam_t* _vocabAuthor, ruleExecInfo_t* rei) {
    	rodsLog(LOG_NOTICE, "%s Metalnx Vocabulary Microservice called.", VOCABULARY_MSI_LOG);

    	char* obj_path = parseMspForStr(_objPath);
    	char* vocab_name = parseMspForStr(_vocabName);
    	char* vocab_author = parseMspForStr(_vocabAuthor);
    	char* vocab_path = "/etc/irods/test.vocab";

        if (is_there_any_vocab_under_path (obj_path, rei)) {
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