//==============================================================================
// Name        : msiadd_vocab_metadata.cpp
// Version     : 1.0
// Copyright   : 2015-2016 EMC Corporation. All rights reserved.
// Description : Create vocabulary microservice responsible for creating a
//               vocabulary in a given path of the grid.
//==============================================================================
#include "reVocabulary.hpp"

extern "C" {
    int msiadd_vocab_metadata(msParam_t* _objPath, msParam_t* _attr_name, msParam_t* _attr_unit, msParam_t* _attr_type, ruleExecInfo_t* rei) {
    	rodsLog(LOG_NOTICE, "%s Metalnx Vocabulary Microservice called [Add Metadata].", VOCABULARY_MSI_LOG);

    	char* obj_path = parseMspForStr(_objPath);
    	char* attr_name = parseMspForStr(_attr_name);
    	char* attr_unit = parseMspForStr(_attr_unit);
    	char* attr_type = parseMspForStr(_attr_type);

        if (!add_metadata_to_vocabulary(obj_path, attr_name, attr_unit, attr_type, rei)) {
            rodsLog(LOG_ERROR, "%s Could not add metadata to vocabulary.", VOCABULARY_MSI_LOG);
            return -1;
        }

        return 0;
    }

    irods::ms_table_entry* plugin_factory() {
        irods::ms_table_entry* msvc = new irods::ms_table_entry(4);
        msvc->add_operation("msiadd_vocab_metadata", "msiadd_vocab_metadata");
        return msvc;
    }
}