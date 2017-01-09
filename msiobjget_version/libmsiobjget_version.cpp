// Finds the version of the MSI package

#include "rods.h"
#include "reGlobalsExtern.hpp"
#include "irods_ms_plugin.hpp"
#include "modAVUMetadata.h"
#include "reFuncDefs.hpp"
#include "apiHeaderAll.h"
#include "objMetaOpr.hpp"
#include "dataObjOpr.hpp"
#include "physPath.hpp"
#include "miscServerFunct.hpp"
#include "rcGlobalExtern.h"
#include "reGlobalsExtern.hpp"
#include "irods_log.hpp"
#include "irods_file_object.hpp"
#include "irods_stacktrace.hpp"
#include "irods_resource_redirect.hpp"

#define MSI_LOG "[Metalnx GET Version MSI]"
#define MSI_VERSION "1.1.0"

#define MSI_ERROR -1
#define MSI_SUCCESS 0

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




