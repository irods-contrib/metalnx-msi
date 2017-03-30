// Finds the version of the MSI package

#include "metalnx.h"
#include "metalnx_msi_version.h"

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
        msvc->add_operation<msParam_t*, ruleExecInfo_t*>("msiobjget_version", std::function<int(msParam_t*, ruleExecInfo_t*)>(msiobjget_version));

        // =-=-=-=-=-=-=-
        // hand it over to the system
        return msvc;

    } // plugin_factory

} // extern "C"




