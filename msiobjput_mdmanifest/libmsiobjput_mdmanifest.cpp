//==============================================================================
// Name        : Manifest extraction
// Copyright   : 2015-2017 Dell EMC. All rights reserved.
// Description : Extracts metadata from manifest files
//==============================================================================
#include "metalnx.h"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <libxml/xmlreader.h>

#define MSO_OBJ_PUT_FAILED -1118000

extern "C" {

	int end = 0;

	/**
	 * Creates the specified metadata object on the given object path
    **/	 
	bool createMetadataOnObject(char * objPath, char * attributeName, char * attributeValue, char * attributeUnit, ruleExecInfo_t* rei) {

		rodsLog( LOG_NOTICE, "Calling createMetadataOnObject()\n");
	
        // Metadata control block
        modAVUMetadataInp_t modAVUMetadataInp;
        memset( &modAVUMetadataInp, 0, sizeof( modAVUMetadataInp_t ) );
        modAVUMetadataInp.arg0 = "add";
        modAVUMetadataInp.arg1 = "-d";
        modAVUMetadataInp.arg2 = objPath;
        modAVUMetadataInp.arg3 = attributeName;
	    modAVUMetadataInp.arg4 = attributeValue;
        modAVUMetadataInp.arg5 = attributeUnit;

        int status = rsModAVUMetadata( rei->rsComm, &modAVUMetadataInp );

        if ( status < 0 ) {
            irods::log( ERROR( status, "msiobjput_mdmanifest failed." ) );
			return false;
        }
		
		return true;
    }

	/**
	 * Callback for XML-Node processing
	**/
	void processNode(xmlTextReaderPtr reader, char *objPath, ruleExecInfo_t* rei) {
		xmlChar *name, *value;
		
		rodsLog( LOG_NOTICE, "Calling processNode()\n");

		name = xmlTextReaderName(reader);
		value = xmlTextReaderValue(reader);

		if (xmlStrEqual(name, BAD_CAST "spectrumList")) {
			end = 1;
		}
		
		else if (xmlStrEqual(name, BAD_CAST "cvParam")) {
			if(xmlTextReaderHasAttributes(reader) > 0) {
				rodsLog(LOG_NOTICE, "Extracting cvParam information...\n");
			
				xmlChar *cvLabel = xmlTextReaderGetAttribute(reader, (xmlChar *) "cvLabel");
				xmlChar *cvAccession = xmlTextReaderGetAttribute(reader, (xmlChar *) "accession");
				xmlChar *cvName = xmlTextReaderGetAttribute(reader, (xmlChar *) "name");
				xmlChar *cvValue = xmlTextReaderGetAttribute(reader, (xmlChar *) "name");
				printf("cvParam: [%s] [%s] [%s] [%s]\n", cvLabel, cvAccession, cvName, cvValue);
				
				createMetadataOnObject(objPath, "cvParam", (char *) cvLabel, "", rei);
				createMetadataOnObject(objPath, (char *) cvLabel, (char *) cvValue, (char *) cvAccession, rei);
				createMetadataOnObject(objPath, (char *) cvLabel, (char *) cvName, "", rei);
			}
		}
		
	}

	/**
	 * Streams XML file using callback
	**/
	int streamFile(char *filename, char *objPath, ruleExecInfo_t* rei) {
		xmlTextReaderPtr reader;
		int ret;

		rodsLog( LOG_NOTICE, "Calling streamFile()\n");
		
		reader = xmlNewTextReaderFilename(filename);
		if (reader != NULL) {
			ret = xmlTextReaderRead(reader);
			while (ret == 1 && end == 0) {
				processNode(reader, objPath, rei);
				ret = xmlTextReaderRead(reader);
			}
			xmlFreeTextReader(reader);
			end = 0;
		} else {
			printf("Unable to open %s\n", filename);
		}
		return 0;
	}


    int msiobjput_mdmanifest(msParam_t* inObjPathParam, msParam_t* inFilePathParam, msParam_t* manifestPathParam, ruleExecInfo_t* rei ) {
		// Getting file name
		char *fileName, *objPath, *manifestFileName;
		fileName = (char *) inFilePathParam->inOutStruct;
		manifestFileName = (char *) manifestPathParam->inOutStruct;
		objPath = (char *) inObjPathParam->inOutStruct;
		
		rodsLog( LOG_NOTICE, "Getting metadata from [%s] and populating [%s].\n", manifestFileName, objPath );
		
		streamFile(manifestFileName, objPath, rei);
        return 0;

    } // msiobjput_mdmanifest

    // =-=-=-=-=-=-=-
    // plugin factory
    irods::ms_table_entry*  plugin_factory( ) {
        // =-=-=-=-=-=-=-
        // instantiate a new msvc plugin
        irods::ms_table_entry* msvc = new irods::ms_table_entry( 3 );

        // =-=-=-=-=-=-=-
        // wire the implementation to the plugin instance
        msvc->add_operation( "msiobjput_mdmanifest", "msiobjput_mdmanifest" );

        // =-=-=-=-=-=-=-
        // hand it over to the system
        return msvc;

    } // plugin_factory

}; // extern "C"




