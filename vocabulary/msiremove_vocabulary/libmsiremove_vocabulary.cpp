//==============================================================================
// Name        : msiremove_vocabulary.cpp
// Version     : 1.0
// Copyright   : 2015-2016 EMC Corporation. All rights reserved.
// Description : Remove vocabulary microservice responsible for removing a
//               vocabulary from a given path of the grid.
//==============================================================================
#include "reVocabulary.hpp"

extern "C" {
    int msiremove_vocabulary(msParam_t* _objPath, ruleExecInfo_t* rei) {
    	rodsLog(LOG_NOTICE, "%s Metalnx Vocabulary Microservice called [Remove operation].", VOCABULARY_MSI_LOG);

    	char* obj_path = parseMspForStr(_objPath);

        if (!remove_vocabulary(obj_path, rei)) {
            rodsLog(LOG_NOTICE,
                    "%s No vocabulary found in %s. Remove operation cannot be completed.\n",
                    VOCABULARY_MSI_LOG, obj_path);
            return 0;
        }

        return 0;
    }

    irods::ms_table_entry* plugin_factory() {
        irods::ms_table_entry* msvc = new irods::ms_table_entry(1);
        msvc->add_operation("msiremove_vocabulary", "msiremove_vocabulary");
        return msvc;
    }
}