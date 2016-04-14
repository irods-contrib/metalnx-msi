#include <cstdio>
#include <string>

#include "rsApiHandler.hpp"
#include "reFuncDefs.hpp"
#include "rods.hpp"
#include "reGlobalsExtern.hpp"
#include "rsGlobalExtern.hpp"
#include "rcGlobalExtern.hpp"
#include "modAVUMetadata.hpp"

#define BUFFER_SIZE 1024
#define MSO_OBJ_PUT_FAILED -1118000

extern "C" {

	/**
	 * Creates the specified metadata object on the given object path
    **/	 
	bool createMetadataOnObject(char * objPath, char * attributeName, char * attributeValue, char * attributeUnit, ruleExecInfo_t* rei) {

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
            irods::log( ERROR( status, "msiobjput_mdvcf failed." ) );
			return false;
        }
		
		return true;
    }

    int msiobjput_mdvcf(msParam_t *inObjPathParam, msParam_t *inFilePathParam, ruleExecInfo_t* rei ) {
	
		// Getting file name
		char *fileName, *objPath;
		fileName = (char *) inFilePathParam->inOutStruct;
		objPath = (char *) inObjPathParam->inOutStruct;
		
		rodsLog( LOG_NOTICE, "Extracting metadata for VCF file [%s]\n...", objPath );

		FILE * filePointer = (FILE *) fopen(fileName, "r");
		if (filePointer == NULL) {
			rodsLog( LOG_ERROR, "Could not open file [%s]: MSO_OBJ_PUT_FAILED\n", fileName );
			return MSO_OBJ_PUT_FAILED;
		}
		
		char line[BUFFER_SIZE];
		while(fscanf(filePointer, " %[^\n]s", line)) {
			
			// If it is not a header line, break the loop
			if (strncmp(line, "##", 2)) {
				break;
			}
			
			// Removing the '##' line init
			int str_size = strlen(line);
			strncpy(line, line + 2, str_size - 2);
			line[str_size - 2] = 0;
			
			rodsLog( LOG_NOTICE, "Line filtered: [%s]\n", line );
			
			// Getting attribute name and value
			int delimiter = strcspn(line, "=");
			char * attrName = (char *) malloc(delimiter + 2);
			char * attrValue = (char *) malloc(str_size - delimiter + 2);
			strncpy(attrName, line, delimiter);
			strncpy(attrValue, line + delimiter + 1, str_size - delimiter + 1);
			attrName[delimiter] = 0;
			attrValue[str_size - delimiter] = 0;
			
			// Handling INFO type of line
			if (!strncmp(attrName, "INFO", 4)) {
				char valueRow[BUFFER_SIZE];
				strncpy(valueRow, attrValue + 1, strlen(attrValue) - 3);
				valueRow[strlen(attrValue) - 3] = 0;
				
				char *valuePart = strtok(valueRow, ",");
				char idValue[BUFFER_SIZE];
				strncpy(idValue, valuePart + 3, strlen(valuePart) - 2);
				idValue[strlen(valuePart) - 2] = 0;
				
				valuePart = strtok(NULL, ",");
				char numberValue[BUFFER_SIZE];
				strncpy(numberValue, valuePart + 7, strlen(valuePart) - 6);
				numberValue[strlen(valuePart) - 6] = 0;
				
				valuePart = strtok(NULL, ",");
				char unitValue[BUFFER_SIZE];
				strncpy(unitValue, valuePart + 5, strlen(valuePart) - 5);
				unitValue[strlen(valuePart) - 5] = 0;
				
				valuePart = strtok(NULL, ",");
				char descValue[BUFFER_SIZE];
				strncpy(descValue, valuePart +  13, strlen(valuePart) - 11);
				descValue[strlen(valuePart) - 11] = 0;
				
				rodsLog( LOG_NOTICE, "\t\t--> [%s] [%s] [%s] [%s]\n", idValue, numberValue, unitValue, descValue );
				
				// Persisting metadata on iCAT
				createMetadataOnObject(objPath, idValue, numberValue, unitValue, rei);
				int unitLen = strlen(idValue);
				strncpy(idValue + unitLen, "_DESCRIPTION", 12);
				idValue[unitLen + 12] = 0;
				createMetadataOnObject(objPath, idValue, descValue, "", rei);
				
			} 
			
			// Handling ALT type
			else if (!strncmp(attrName, "ALT", 3)) {
				char valueRow[BUFFER_SIZE];
				strncpy(valueRow, attrValue + 1, strlen(attrValue) - 3);
				valueRow[strlen(attrValue) - 3] = 0;
				
				char *valuePart = strtok(valueRow, ",");
				char idValue[BUFFER_SIZE];
				strncpy(idValue, valuePart + 3, strlen(valuePart) - 2);
				idValue[strlen(valuePart) - 2] = 0;
				
				valuePart = strtok(NULL, ",");
				char descValue[BUFFER_SIZE];
				strncpy(descValue, valuePart + 13, strlen(valuePart) - 14);
				descValue[strlen(valuePart) - 14] = 0;
				
				createMetadataOnObject(objPath, "ALT", idValue, descValue, rei);
			} 
			
			// Handling FORMAT type
			else if (!strncmp(attrName, "FORMAT", 6)) {
				rodsLog( LOG_NOTICE, "FORMAT line. Skipping..." );
			} 
			
			// Handling other lines
			else {
				rodsLog( LOG_NOTICE, "Normal line" );
				createMetadataOnObject(objPath, attrName, attrValue, "", rei);
			}
			
			free(attrName);
			free(attrValue);
			
		}
	
		return 0;
    } // msiobjput_mdvcf

    // =-=-=-=-=-=-=-
    // plugin factory
    irods::ms_table_entry*  plugin_factory( ) {
        // =-=-=-=-=-=-=-
        // instantiate a new msvc plugin
        irods::ms_table_entry* msvc = new irods::ms_table_entry( 2 );

        // =-=-=-=-=-=-=-
        // wire the implementation to the plugin instance
        msvc->add_operation( "msiobjput_mdvcf", "msiobjput_mdvcf" );

        // =-=-=-=-=-=-=-
        // hand it over to the system
        return msvc;

    } // plugin_factory

}; // extern "C"




