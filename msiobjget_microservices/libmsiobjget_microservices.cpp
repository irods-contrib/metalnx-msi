// Finds all MSIs under /irods/plugins/microservices

#include "metalnx.h"

#include <boost/filesystem.hpp>

#define MSI_LOG "[Metalnx GET Microservices MSI]"

extern "C" {
    int msiobjget_microservices(msParam_t* outMicroservices, ruleExecInfo_t* rei) {
        rodsLog(LOG_NOTICE, "%s MSI Get Microservices [%s]\n",  MSI_LOG, MSI_VERSION);

        boost::filesystem::path msi_dir_path(MSI_IRODS_DIR);
        rodsLog(LOG_NOTICE, "%s Checking %s\n", MSI_LOG, msi_dir_path.string().c_str());

        if (!boost::filesystem::exists(msi_dir_path)) {
            rodsLog(LOG_ERROR, "%s Directory %s does not exist\n", MSI_LOG, msi_dir_path.string().c_str());
            return MSI_ERROR;
        }

        std::string msis;

        boost::filesystem::directory_iterator end_itr;
        for (boost::filesystem::directory_iterator itr(msi_dir_path); itr != end_itr; itr++) {
            if (boost::filesystem::is_directory(itr->status()) ) continue;

            msis.append(itr->path().filename().string().c_str());
            msis.append(",");
        }

        rodsLog(LOG_NOTICE, "%s -> %s\n", MSI_LOG, msis.c_str());

        fillStrInMsParam(outMicroservices, msis.c_str());

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
        msvc->add_operation( "msiobjget_microservices", "msiobjget_microservices" );

        // =-=-=-=-=-=-=-
        // hand it over to the system
        return msvc;

    } // plugin_factory

} // extern "C"




