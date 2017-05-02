//==============================================================================
// Name        : bam metadata extraction
// Copyright   : 2015-2017 Dell Inc. All rights reserved.
// Description : extracts metadata from BAM files.
//==============================================================================

#include "metalnx.h"
#include "metalnx_msi_version.h"
#include "rsModAVUMetadata.hpp"

#include "htslib/bgzf.h"
#include "bam.h"
#include <string>

#define MSO_OBJ_PUT_FAILED -1118000

extern "C" {
    int msiobjput_mdbam(msParam_t* inObjPathParam, msParam_t* inFilePathParam, ruleExecInfo_t* rei ) {
		rodsLog( LOG_NOTICE, "Attempting to read SAM file..." );
		
		// Getting file name
		char *fileName, *objPath;
		fileName = (char *) inFilePathParam->inOutStruct;
		objPath = (char *) inObjPathParam->inOutStruct;
		rodsLog( LOG_NOTICE, fileName );
		rodsLog( LOG_NOTICE, objPath );

		// Opening BAM file
		samFile *filePointer = sam_open(fileName/*, "r"*/);
		bam_header_t *bam_header = sam_hdr_read(filePointer);

		// Metadata control block
		modAVUMetadataInp_t modAVUMetadataInp;
		memset(&modAVUMetadataInp, 0, sizeof(modAVUMetadataInp_t));
		modAVUMetadataInp.arg0 = (char*) "add";
		modAVUMetadataInp.arg1 = (char*) "-d";
		modAVUMetadataInp.arg2 = objPath;	

        // Splitting lines
        char * line = strtok(bam_header->text, "\n");
		while(line != NULL) {

			// Locating the @RG tag
			if (!strncmp(line, "@RG", 3)) {
				char * line_parts = strtok(line, "\t");
				while(line_parts != NULL) {

					// Extracting sequencer name
					if (!strncmp(line_parts, "PL:", 3)) {
						char * sequencer = line_parts + 3;
						modAVUMetadataInp.arg3 = (char*) "SEQUENCER";
						modAVUMetadataInp.arg4 = sequencer;
						modAVUMetadataInp.arg5 = (char*) "";

						int status = rsModAVUMetadata( rei->rsComm, &modAVUMetadataInp );
						if ( status < 0 ) {
							irods::log( ERROR( status, "msiobjput_mdbam failed." ) );
						}

						rodsLog( LOG_NOTICE, "msiobjput_mdbam added [%s, %s] on %s\n", "SEQUENCER", sequencer, objPath );
					}

					// Extracting sequencing center name
					else if (!strncmp(line_parts, "CN:", 3)) {
						char * sequencing_center_name = line_parts + 3;
						
						modAVUMetadataInp.arg3 = (char*) "SEQUENCER_CENTER_NAME";
						modAVUMetadataInp.arg4 = sequencing_center_name;
						modAVUMetadataInp.arg5 = (char*) "";

						int status = rsModAVUMetadata( rei->rsComm, &modAVUMetadataInp );
						if ( status < 0 ) {
							irods::log( ERROR( status, "msiobjput_mdbam failed." ) );
						}

						rodsLog( LOG_NOTICE, "msiobjput_mdbam added [%s, %s] on %s\n", "SEQUENCER_CENTER_NAME", sequencing_center_name, objPath );
					}

					line_parts = strtok(NULL, "\t");
				}
			}

			line = strtok(NULL, "\n");
		}

        // if sam file doesn't get closed, it won't be entirely removed from host when deleted
        sam_close(filePointer);
	    free(bam_header);

        return 0;

    } // msiobjput_mdbam

    // =-=-=-=-=-=-=-
    // plugin factory
    irods::ms_table_entry*  plugin_factory( ) {
        // =-=-=-=-=-=-=-
        // instantiate a new msvc plugin
        irods::ms_table_entry* msvc = new irods::ms_table_entry( 2 );

        // =-=-=-=-=-=-=-
        // wire the implementation to the plugin instance
        msvc->add_operation<
		msParam_t*, 
		msParam_t*, 
		ruleExecInfo_t*>("msiobjput_mdbam", 
				std::function<int(
					msParam_t*,
					msParam_t*,
					ruleExecInfo_t*)>(msiobjput_mdbam));

        // =-=-=-=-=-=-=-
        // hand it over to the system
        return msvc;

    } // plugin_factory

}; // extern "C"




