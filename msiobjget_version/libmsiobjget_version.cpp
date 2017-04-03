//==============================================================================
// Name        : Get version
// Copyright   : 2015-2017 Dell EMC. All rights reserved.
// Description : Finds the version of the MSI package
//==============================================================================

#include "metalnx.h"

#define MSI_LOG "[Metalnx GET Version MSI]"

extern "C" {
    int msiobjget_version(msParam_t* outVersion, ruleExecInfo_t* rei) {
        rodsLog( LOG_NOTICE, "%s MSI Get Version [%s]\n",  MSI_LOG, MSI_VERSION);

        fillStrInMsParam(outVersion, MSI_VERSION);

        return MSI_SUCCESS;
    }

    // =-=-=-=-=-=-=-
    // plugin factory
    irods::ms_table_entry*  plugin_factory( ) {
        // =-=-=-=-=-=-=-
        // instantiate a new msvc plugin
        irods::ms_table_entry* msvc = new irods::ms_table_entry( 1 );

        // =-=-=-=-=-=-=-
        // wire the implementation to the plugin instance
        msvc->add_operation( "msiobjget_version", "msiobjget_version" );

        // =-=-=-=-=-=-=-
        // hand it over to the system
        return msvc;

    } // plugin_factory

} // extern "C"




