#include "metalnx.h"
#include "metalnx_msi_version.h"
#include "rsModAVUMetadata.hpp"

extern "C" {
    /**
     * \fn msiobjput_populate(msParam_t* inObjPathParam, ruleExecInfo_t* rei )
     *
     * \brief Populates an object with random metadata
     *
     * \module msoDrivers_irods
     *
     * \since 3.0
     *
     *
     * \usage See clients/icommands/test/rules3.0/
     *
     * \param[in] inObjPathParam - a STR_MS_T request string to external resource
     * \param[in,out] rei - The RuleExecInfo structure that is automatically
     *    handled by the rule engine. The user does not include rei as a
     *    parameter in the rule invocation.
     *
     * \DolVarDependence none
     * \DolVarModified none
     * \iCatAttrDependence none
     * \iCatAttrModified none
     * \sideeffect none
     *
     * \return integer
     * \retval 0 on success
     * \pre none
     * \post none
     * \sa none
    **/

	/**
	 * Creates the specified metadata object on the given object path
    **/	 
	bool createMetadataOnObject(char * objPath, char * attributeName, char * attributeValue, char * attributeUnit, ruleExecInfo_t* rei) {

        // Metadata control block
        modAVUMetadataInp_t modAVUMetadataInp;
        memset( &modAVUMetadataInp, 0, sizeof( modAVUMetadataInp_t ) );
        modAVUMetadataInp.arg0 = (char*) "add";
        modAVUMetadataInp.arg1 = (char*) "-d";
        modAVUMetadataInp.arg2 = objPath;
        modAVUMetadataInp.arg3 = attributeName;
	    modAVUMetadataInp.arg4 = attributeValue;
        modAVUMetadataInp.arg5 = attributeUnit;

        int status = rsModAVUMetadata( rei->rsComm, &modAVUMetadataInp );

        if ( status < 0 ) {
            irods::log( ERROR( status, "msiobjput_populate failed." ) );
	    return false;
        }
		
	return true;
    }

    int msiobjput_populate(
        msParam_t* inObjPathParam,
        ruleExecInfo_t* rei ) {

		char * objPath = (char *) inObjPathParam->inOutStruct;
		rodsLog( LOG_NOTICE, "Adding sample metadata to data object [%s]\n",  objPath);

		createMetadataOnObject(objPath, (char*) "ATTRIBUTE01", (char*) "VALUE1", (char*) "chars", rei);
		createMetadataOnObject(objPath, (char*) "ATTRIBUTE02", (char*) "2", (char*) "ints", rei);
		createMetadataOnObject(objPath, (char*) "ATTRIBUTE03", (char*) "2, 3, 4, 5", (char*) "", rei);
		createMetadataOnObject(objPath, (char*) "ATTRIBUTE04", (char*) "VALUE-1235", (char*) "", rei);
		createMetadataOnObject(objPath, (char*) "ATTRIBUTE05", (char*) "1865", (char*) "bytes", rei);
		createMetadataOnObject(objPath, (char*) "ATTRIBUTE06", (char*) "False", (char*) "bool", rei);
		createMetadataOnObject(objPath, (char*) "ATTRIBUTE07", (char*) "_TAG-12342_", (char*) "", rei);
		createMetadataOnObject(objPath, (char*) "ATTRIBUTE08", (char*) "['value1', 'value2, 'value3']", (char*) "as", rei);
		createMetadataOnObject(objPath, (char*) "ATTRIBUTE09", (char*) "MLX", (char*) "", rei);
		createMetadataOnObject(objPath, (char*) "ATTRIBUTE10", (char*) "NULL", (char*) "", rei);
        return 0;

    }

    // =-=-=-=-=-=-=-
    // plugin factory
    irods::ms_table_entry*  plugin_factory( ) {
        // =-=-=-=-=-=-=-
        // instantiate a new msvc plugin
        irods::ms_table_entry* msvc = new irods::ms_table_entry( 1 );

        // =-=-=-=-=-=-=-
        // wire the implementation to the plugin instance
        msvc->add_operation<
		msParam_t*, 
 		ruleExecInfo_t*>("msiobjput_populate",
				std::function<int(
					msParam_t*, 
					ruleExecInfo_t*)>(msiobjput_populate));

        // =-=-=-=-=-=-=-
        // hand it over to the system
        return msvc;

    } // plugin_factory

} // extern "C"




