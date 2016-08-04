//==============================================================================
// Name        : msiremove_vocab_metadata.cpp
// Version     : 1.0
// Copyright   : 2015-2016 EMC Corporation. All rights reserved.
// Description : Remove vocabulary metadata microservice responsible for removing a
//               metadata from a vocabulary.
//==============================================================================
#include "reVocabulary.hpp"

extern "C" {
    int msiremove_vocab_metadata(msParam_t* _objPath, msParam_t* _attrName, ruleExecInfo_t* rei) {
    	rodsLog(LOG_NOTICE, "%s Metalnx Vocabulary Microservice called [Remove metadata operation].", VOCABULARY_MSI_LOG);

    	char* obj_path = parseMspForStr(_objPath);
    	char* attr_name = parseMspForStr(_attrName);

        if (!remove_vocab_metadata(obj_path, attr_name, rei)) {
            rodsLog(LOG_ERROR,
                    "%s No metadata found in %s that matches %s.\n",
                    VOCABULARY_MSI_LOG, obj_path, attr_name);
            return -1;
        }

        return 0;
    }

    irods::ms_table_entry* plugin_factory() {
        irods::ms_table_entry* msvc = new irods::ms_table_entry(2);
        msvc->add_operation("msiremove_vocab_metadata", "msiremove_vocab_metadata");
        return msvc;
    }
}