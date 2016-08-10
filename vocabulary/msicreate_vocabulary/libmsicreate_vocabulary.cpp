//==============================================================================
// Name        : msicreate_vocabulary.cpp
// Version     : 1.0
// Copyright   : 2015-2016 EMC Corporation. All rights reserved.
// Description : Create vocabulary microservice responsible for creating a
//               vocabulary in a given path of the grid.
//==============================================================================
#include "reVocabulary.hpp"

extern "C" {
    int msicreate_vocabulary(msParam_t* _objPath, msParam_t* _resc, msParam_t* _vocabName, msParam_t* _vocabAuthor, ruleExecInfo_t* rei) {
    	rodsLog(LOG_NOTICE, "%s Metalnx Vocabulary Microservice called.", VOCABULARY_MSI_LOG);

    	char* obj_path = parseMspForStr(_objPath);
    	char* resc = parseMspForStr(_resc);
    	char* vocab_name = parseMspForStr(_vocabName);
    	char* vocab_author = parseMspForStr(_vocabAuthor);

        if (!create_vocabulary(obj_path, resc, vocab_name, vocab_author, rei)) {
            rodsLog(LOG_ERROR, "%s Could not create vocabulary in %s.\n", VOCABULARY_MSI_LOG, obj_path);
            return -1;
        }

        rodsLog(LOG_NOTICE, "%s Vocabulary created successfully in %s.\n", VOCABULARY_MSI_LOG, obj_path);

        return 0;
    }

    irods::ms_table_entry* plugin_factory() {
        irods::ms_table_entry* msvc = new irods::ms_table_entry(4);
        msvc->add_operation("msicreate_vocabulary", "msicreate_vocabulary");
        return msvc;
    }
}