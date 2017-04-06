#include "metalnx.h"
#include "metalnx_msi_version.h"
#include "rsModAVUMetadata.hpp"

#include <string>
#include <ctype.h>
#include <libexif/exif-loader.h>

extern "C" {
    /**
     * \fn msiobjget_irods(msParam_t*  inRequestPath, msParam_t* inFileMode, msParam_t* inFileFlags, msParam_t* inCacheFilename,  ruleExecInfo_t* rei )
     *
     * \brief Gets an iRODS object
     *
     * \module msoDrivers_irods
     *
     * \since 3.0
     *
     *
     * \usage See clients/icommands/test/rules3.0/
     *
     * \param[in] inRequestPath - a STR_MS_T request string to external resource
     * \param[in] inFileMode - a STR_MS_T mode of cache file creation
     * \param[in] inFileFlags - a STR_MS_T flags for cache file creation
     * \param[in] inCacheFilename - a STR_MS_T cache file name (full path)
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

    typedef struct _exifForeachHandlerData {
        ruleExecInfo_t* rei;
        char * objPath;
    } ExifForeachHandlerData;

    void exifTagHandler(ExifEntry * entry, void * user_data) {
        ExifForeachHandlerData *userExifHandler = (ExifForeachHandlerData *) user_data;

        // Getting tag name and description
        const char *tagName = exif_tag_get_name(entry->tag);

        char *objPath = userExifHandler->objPath;
        rodsLog( LOG_NOTICE, "\tFound tag name is [%s] on file [%s]\n", tagName, objPath );

	    char tagValue[256];

        if (entry->format == EXIF_FORMAT_ASCII) {
            rodsLog( LOG_NOTICE, "\tTag format is ASCII and its size is %u\n", entry->size );
	        strncpy(tagValue, (char *) entry->data, entry->size);
	        tagValue[entry->size] = 0;
        }
        else if (entry->format == EXIF_FORMAT_LONG) {
            rodsLog( LOG_NOTICE, "\tTag format is long and its size is %u\n", entry->size );
	        sprintf(tagValue, "%ld", *((long int *) entry->data));
        }
        else if (entry->format == EXIF_FORMAT_SHORT) {
            rodsLog( LOG_NOTICE, "\tTag format is short and its size is %u\n", entry->size );
	        sprintf(tagValue, "%hd", *((short int *) entry->data));
        }
        else if (entry->format == EXIF_FORMAT_FLOAT) {
            rodsLog( LOG_NOTICE, "\tTag format is float and its size is %u\n", entry->size );
	        sprintf(tagValue, "%f", *((float *) entry->data));
        }
        else if (entry->format == EXIF_FORMAT_DOUBLE) {
            rodsLog( LOG_NOTICE, "\tTag format is double and its size is %u\n", entry->size );
	        sprintf(tagValue, "%f", *( (double *) entry->data));
        }
        else {
            return;
        }

        // Metadata control block
        modAVUMetadataInp_t modAVUMetadataInp;
        memset(&modAVUMetadataInp, 0, sizeof(modAVUMetadataInp_t));
        modAVUMetadataInp.arg0 = (char*) "add";
        modAVUMetadataInp.arg1 = (char*) "-d";
        modAVUMetadataInp.arg2 = (char *) objPath;
        modAVUMetadataInp.arg3 = (char *) tagName;
 	modAVUMetadataInp.arg4 = tagValue;
        modAVUMetadataInp.arg5 = (char*) "";

        int status = rsModAVUMetadata( userExifHandler->rei->rsComm, &modAVUMetadataInp );

        if ( status < 0 ) {
            irods::log( ERROR( status, "msiobjjpeg_extract failed." ) );
        }
    }

    int msiobjjpeg_extract(
        msParam_t* inObjPathParam,
        msParam_t* inFilePathParam,
        ruleExecInfo_t* rei ) {

         rodsLog( LOG_NOTICE, "Extracting metadata from JPEG file.." );

         ExifLoader *loader = exif_loader_new();

         if (!loader) {
             rodsLog ( LOG_ERROR, "Could not create new EXIF context to load image tags\n" );
             return 1;
         }

        ExifData *data = NULL;
        exif_loader_write_file(loader, (char *) inFilePathParam->inOutStruct);
        data = exif_loader_get_data(loader);

        if (!data) {
            rodsLog( LOG_ERROR, "Could not extract ExifData information from file.\n" );
            return 1;
        }

        // The EXIF loader is no longer needed
        exif_loader_unref(loader);
        loader = NULL;

        if (data) {

            ExifForeachHandlerData *user_data = (ExifForeachHandlerData *) malloc(sizeof(struct _exifForeachHandlerData));
            char * objPath = (char *) inObjPathParam->inOutStruct;

            user_data->rei = rei;
            user_data->objPath = (char *) malloc(strlen(objPath) + 1);
            strncpy(user_data->objPath, objPath, strlen(objPath));
	        user_data->objPath[strlen(objPath)] = 0;

            for(int i = 0; i < EXIF_IFD_COUNT; i++) {
                ExifContent *tagContent = data->ifd[i];
                exif_content_foreach_entry(tagContent, exifTagHandler, (void *) user_data);
            }

            free(user_data->objPath);
            free(user_data);
        }

        return 0;

    }

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
		ruleExecInfo_t*>("msiobjjpeg_extract", 
				std::function<int(
 					msParam_t*,
					msParam_t*,
					ruleExecInfo_t*)>(msiobjjpeg_extract));

        // =-=-=-=-=-=-=-
        // hand it over to the system
        return msvc;

    } // plugin_factory

} // extern "C"




