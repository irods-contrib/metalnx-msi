//==============================================================================
// Name        : Illumina.cpp
// Copyright   : 2015-2017 Dell Inc. All rights reserved.
// Description : Illumina Microservice used to extract metadata from the Sample
//				 Sheet csv file and apply it to all files in a run.
//==============================================================================
#include "metalnx.h"

#include<fstream>

#define METADATA_CHAR_TAM 50
#define CSV_HEADER "[Header]"
#define CSV_READS "[Reads]"
#define CSV_SETTINGS "[Settings]"
#define CSV_DATA "[Data]"
#define SAMPLE_SHEET_FILE_NAME "SampleSheet.csv"
#define COLL_TO_APPLY_METADATA "/Data"

#define ILLUMINA_MSI_LOG "[Metalnx Illumina MSI]"

std::string CSV_Header(CSV_HEADER);
std::string CSV_Reads(CSV_READS);
std::string CSV_Settings(CSV_SETTINGS);
std::string CSV_Data(CSV_DATA);

/**
 * Defines an object for a metadata tag in iRODS.
 * A - attribute
 * V - value
 * U - unit
 */
 typedef struct Metadata {
	char * A;
	char * V;
	char * U;
	struct Metadata* next;
} IlluminaX10Metadata;

/**
 * Defines an object for a Sample Sheet file.
 * phy_path - physical file path
 * path - iRODS path
 */
typedef struct SampleSheetCSVFile {
	char* phy_path;
	char* path;
} SampleSheet;

SampleSheet* create_sample_sheet(char*, char*);

/**
 * Reads the SampleSheet.csv file to extract its metadata.
 * */
void read_metadata_from_csv_file(char*, IlluminaX10Metadata**);

/**
 * Add an AVU tuple to a specific file in the grid.
 * */
bool add_metadata_to_file(ruleExecInfo_t*, char*, char*, char*, char*);

/**
 * Add metadata extracted from the SampleSheet.csv file to all data objects under a given path.
 * */
bool add_metadata_to_files_under_path(ruleExecInfo_t*, char*, char*, IlluminaX10Metadata*);

/**
 * Find the parent path of the given path.
 */
char* find_parent_path (char*);

/**
 * Remove a specific character from a string.
 */
void remove_char_from_str(std::string&, char);

/**
 * Converts a string into a char*
 */
char* str_to_char_array (std::string);

/**
 * Count character occurences in a string.
 */
int count_char_in_str(std::string, char);

/**
 * Creates an AVU object in memory.
 */
IlluminaX10Metadata* createIlluminaAVU (char* A, char* V, char* U);

/**
 * Finds all headers of the Data section from the Sample Sheet CSV file.
 */
char** get_data_section_headers(int, char*);

/**
 * Frees up the memory used for the sample sheet metadata.
 */
void free_illumina_metadata(IlluminaX10Metadata**);

/**
 * Concatenates two char arrays into a single one.
 */
char* concat(char*, char*);

/**
 * Creates an AVU object
 */
void add_avu_to_metadata_list(IlluminaX10Metadata**, char*, char*, char*);

/**
 * Looks for the SampleSheet.csv file under the specified path.
 * Returns the physical file path of the CSV file, if any. NULL, otherwise.
 */
void find_sample_sheet_under_path(ruleExecInfo_t* rei, char*, char*, SampleSheet**);

extern "C" {
    int msiget_illumina_meta_impl(msParam_t* _objPath, msParam_t* _destResc, ruleExecInfo_t* rei) {
    	rodsLog(LOG_NOTICE, "%s Metalnx Illumina Microservice called.", ILLUMINA_MSI_LOG);

    	char* destResource = parseMspForStr(_destResc);
        char* tar_obj_path = parseMspForStr(_objPath);

        // Find the tar's parent path in iRODS to look for the SampleSheet.csv file
        char* tar_obj_parent_path = find_parent_path(tar_obj_path);

        // Finding the physical path of the SampleSheet.csv file
        SampleSheet* sample_sheet_file = NULL;
        find_sample_sheet_under_path(rei, tar_obj_parent_path, destResource, &sample_sheet_file);

        if( !sample_sheet_file->phy_path ) {
        	rodsLog(LOG_ERROR, "%s msiget_illumina_meta - Sample Sheet not found in %s", ILLUMINA_MSI_LOG, tar_obj_path);
            return SYS_INVALID_INPUT_PARAM;
        }

        IlluminaX10Metadata* sample_sheet_metadata = NULL;

        // Parsing the SampleSheet.csv file and extracting its metadata
		read_metadata_from_csv_file(sample_sheet_file->phy_path, &sample_sheet_metadata);

        // Collection where the metadata should be applied recursively
        char* data_coll_path = concat(find_parent_path(sample_sheet_file->path), COLL_TO_APPLY_METADATA);

		// Applying the metadata got from the CSV file into a specific collection (recursively)
        add_metadata_to_files_under_path(rei, data_coll_path, destResource, sample_sheet_metadata);

        if (sample_sheet_metadata) free_illumina_metadata(&sample_sheet_metadata);

        if (sample_sheet_file) {
        	free(sample_sheet_file->phy_path);
        	free(sample_sheet_file->path);
        	free(sample_sheet_file);
        }

        if (tar_obj_parent_path) free(tar_obj_parent_path);

        if (data_coll_path) free(data_coll_path);

        return 0;
    }

    irods::ms_table_entry* plugin_factory() {
        irods::ms_table_entry* msvc = new irods::ms_table_entry(2);

        msvc->add_operation("msiget_illumina_meta_impl", "msiget_illumina_meta");

        return msvc;
    }
}

SampleSheet* create_sample_sheet(char* phy_path, char* path) {
	if (phy_path == NULL || path == NULL) {
		rodsLog( LOG_ERROR, "%s Could not create sample sheet struct. NULL parameter provided.", ILLUMINA_MSI_LOG );
		return NULL;
	}

	int phy_path_len = strlen(phy_path) + 1;
	int path_len = strlen(path) + 1;

	SampleSheet* ss = (SampleSheet *) malloc(sizeof(SampleSheet));
	ss->phy_path = (char *) malloc(phy_path_len * sizeof(char));
	ss->path = (char *) malloc(path_len * sizeof(char));

	memset(ss->phy_path, 0, phy_path_len);
	memset(ss->path, 0, path_len);

	strncpy(ss->phy_path, phy_path, phy_path_len);
	strncpy(ss->path, path, path_len);

	return ss;
}

void find_sample_sheet_under_path(ruleExecInfo_t* rei, char* path, char* dest_resource, SampleSheet** sample_sheet) {
	if (path == NULL) {
		return;
	}

	collEnt_t* collEnt = NULL;
	collInp_t collInp;
	bzero( &collInp, sizeof( collInp ) );
	collInp.flags = RECUR_QUERY_FG | VERY_LONG_METADATA_FG | NO_TRIM_REPL_FG | INCLUDE_CONDINPUT_IN_QUERY;
	rstrcpy( collInp.collName, path, MAX_NAME_LEN );
	addKeyVal( &collInp.condInput, RESC_NAME_KW, dest_resource );

	int handleInx = rsOpenCollection( rei->rsComm, &collInp );

	if ( handleInx < 0 ) {
		rodsLog( LOG_ERROR,
				 "%s find_sample_sheet_file_path: rsOpenCollection of %s error. status = %d",
				 ILLUMINA_MSI_LOG, path, handleInx );

		return;
	}

	int status = rsReadCollection( rei->rsComm, &handleInx, &collEnt );

	while ( ( status = rsReadCollection( rei->rsComm, &handleInx, &collEnt ) ) >= 0 ) {
		if ( NULL == collEnt ) {
			rodsLog(LOG_ERROR, "%s find_sample_sheet_file_path rsReadCollection: collEnt is NULL", ILLUMINA_MSI_LOG );
			continue;
		}

		if ( collEnt->objType == DATA_OBJ_T ) {
			std::string data_obj_name(collEnt->dataName);
			if (data_obj_name.compare(SAMPLE_SHEET_FILE_NAME) == 0) {
				rodsLog(LOG_NOTICE, "%s SAMPLE_SHEET_FILE_NAME %s", ILLUMINA_MSI_LOG, str_to_char_array(data_obj_name) );
				*sample_sheet = create_sample_sheet(collEnt->phyPath, concat(concat(collEnt->collName, "/"), collEnt->dataName));
				break;
			}
		}
	}

	rsCloseCollection( rei->rsComm, &handleInx );

	return;
}

bool add_metadata_to_files_under_path(ruleExecInfo_t* rei, char* path, char* dest_resource, IlluminaX10Metadata* sample_sheet_metadata) {

	collEnt_t* collEnt = NULL;
	collInp_t collInp;
	bzero( &collInp, sizeof( collInp ) );
	collInp.flags = RECUR_QUERY_FG | VERY_LONG_METADATA_FG | NO_TRIM_REPL_FG | INCLUDE_CONDINPUT_IN_QUERY;
	rstrcpy( collInp.collName, path, MAX_NAME_LEN );
	addKeyVal( &collInp.condInput, RESC_NAME_KW, dest_resource );

	int handleInx = rsOpenCollection( rei->rsComm, &collInp );

	if ( handleInx < 0 ) {
		rodsLog( LOG_ERROR,
				 "%s add_metadata_to_files_under_path: rsOpenCollection of %s error. status = %d",
				 ILLUMINA_MSI_LOG, collInp.collName, handleInx );

		return false;
	}

	int status = rsReadCollection( rei->rsComm, &handleInx, &collEnt );

	while ( ( status = rsReadCollection( rei->rsComm, &handleInx, &collEnt ) ) >= 0 ) {
		if ( NULL == collEnt ) {
			rodsLog(LOG_ERROR, "%s rsReadCollection: collEnt is NULL", ILLUMINA_MSI_LOG );
			continue;
		}

		if ( collEnt->objType == DATA_OBJ_T ) {
			int obj_path_len = strlen(collEnt->collName) + strlen(collEnt->dataName) + 2;
			char* obj_path = (char*) malloc(obj_path_len);

			memset(obj_path, 0, obj_path_len);
			strcpy(obj_path, collEnt->collName);
			strcat(obj_path, "/");
			strcat(obj_path, collEnt->dataName);

			IlluminaX10Metadata* metadata_tag = sample_sheet_metadata;
			while (metadata_tag){
				rodsLog(LOG_NOTICE, "%s Adding metadata ('%s' '%s' '%s') to %s", ILLUMINA_MSI_LOG, metadata_tag->A, metadata_tag->V, metadata_tag->U, obj_path);
				add_metadata_to_file(rei, obj_path, metadata_tag->A, metadata_tag->V, metadata_tag->U);
				metadata_tag = metadata_tag->next;
			}

			free(obj_path);
		}
	}

	rsCloseCollection( rei->rsComm, &handleInx );

	return true;
}

void read_metadata_from_csv_file(char* csv_file_path, IlluminaX10Metadata** sample_sheet_avu_begin) {

	using namespace std;

	rodsLog(LOG_NOTICE, "%s Reading metadata from CSV file...", ILLUMINA_MSI_LOG);

	string str;
	char* token;
	char* line;
	char** data_section_headers = NULL;
	int data_section_headers_len = 0;
	bool isHeaderSection = false;
	bool isSettingsSection = false;
	bool isReadsSection = false;
	bool isDataSection = false;

	ifstream in;
	in.open (csv_file_path, std::ifstream::in);
	while ( getline(in, str) ) {
		remove_char_from_str(str, '\r');
		line = str_to_char_array(str);
		token = strtok(line, ",");

		if (token == NULL) {
			continue;
		}

		if (CSV_Header.compare(token) == 0) {
			isHeaderSection = true;
			isSettingsSection = false;
			isReadsSection = false;
			isDataSection = false;
		}
		else if (CSV_Settings.compare(token) == 0) {
			isHeaderSection = false;
			isSettingsSection = true;
			isReadsSection = false;
			isDataSection = false;
		}
		else if (CSV_Reads.compare(token) == 0) {
			isHeaderSection = false;
			isSettingsSection = false;
			isReadsSection = true;
			isDataSection = false;
		}
		else if (CSV_Data.compare(token) == 0) {
			isHeaderSection = false;
			isSettingsSection = false;
			isReadsSection = false;
			isDataSection = true;

			getline(in, str);
			remove_char_from_str(str, '\r');
			data_section_headers_len = count_char_in_str(str, ',') + 1;
			data_section_headers = get_data_section_headers(data_section_headers_len, str_to_char_array(str));
		}
		else {
			if (isHeaderSection) {
				add_avu_to_metadata_list(sample_sheet_avu_begin, token, strtok(NULL, ","), str_to_char_array(CSV_Header));
			}
			else if (isSettingsSection) {
				add_avu_to_metadata_list(sample_sheet_avu_begin, token, strtok(NULL, ","), str_to_char_array(CSV_Settings));
			}
			else if (isReadsSection) {
				add_avu_to_metadata_list(sample_sheet_avu_begin, token, strtok(NULL, ","), str_to_char_array(CSV_Reads));
			}
			else if (isDataSection) {
				for(int i = 0; i < data_section_headers_len; i++) {
					add_avu_to_metadata_list(sample_sheet_avu_begin, data_section_headers[i], token, str_to_char_array(CSV_Data));
					token = strtok(NULL, ",");
				}
			}
		}

		free(line);
	}
	in.close();

	for(int i = 0; i < data_section_headers_len; i++) {
		free(data_section_headers[i]);
	}
	free(data_section_headers);
}

bool add_metadata_to_file(ruleExecInfo_t* rei, char* obj_path, char* A, char* V, char* U) {

	if (A == NULL || V == NULL) {
		rodsLog(LOG_ERROR, "%s Metadata cannot be added. Invalid A V U values: %s %s %s", ILLUMINA_MSI_LOG, A, V, U);
		return false;
	}

	rodsLog(LOG_NOTICE, "%s Adding metadata to file: %s %s %s", ILLUMINA_MSI_LOG, A, V, U);

	modAVUMetadataInp_t modAVUMetadataInp;
	memset(&modAVUMetadataInp, 0, sizeof(modAVUMetadataInp_t));
	modAVUMetadataInp.arg0 = "add";
	modAVUMetadataInp.arg1 = "-d";
	modAVUMetadataInp.arg2 = obj_path;
	modAVUMetadataInp.arg3 = A;
	modAVUMetadataInp.arg4 = V;
	modAVUMetadataInp.arg5 = U;

	int status = rsModAVUMetadata( rei->rsComm, &modAVUMetadataInp );

	if ( status < 0 ) {
		rodsLog(LOG_NOTICE, "%s Metadata could not be added: %s %s %s", ILLUMINA_MSI_LOG, A, V, U);
		return false;
	}

	return true;
}

void remove_char_from_str(std::string &str, char c) {
	str.erase(std::remove(str.begin(), str.end(), c), str.end() );
}

char* str_to_char_array (std::string str) {
	char * char_array = new char[str.size() + 1];
	str.copy(char_array, str.size(), 0);
	char_array[str.size()] = '\0';
	return char_array;
}

char* find_parent_path (char* path) {

	if (path == NULL || strlen(path) == 0) {
		rodsLog(LOG_NOTICE, "%s Cannot find parent path of %s", ILLUMINA_MSI_LOG, path);
		return "";
	}

	int path_len = strlen(path);

	int pos = path_len - 1;
	while(path[pos] != '\0' && path[pos] != '/') {
		pos--;
	}

	char* parent_path = (char *) malloc((pos + 1) * sizeof(char));
	strncpy (parent_path, path, pos);
	parent_path[pos] = '\0';

	return parent_path;
}

int count_char_in_str(std::string str, char c) {
	return std::count(str.begin(), str.end(), c);
}

IlluminaX10Metadata* createIlluminaAVU (char* A, char* V, char* U) {
	IlluminaX10Metadata* illuminaAVU = (IlluminaX10Metadata *) malloc(sizeof(IlluminaX10Metadata));

	if (A == NULL) {
		rodsLog(LOG_ERROR, "%s Attribute cannot be NULL", ILLUMINA_MSI_LOG);
		return NULL;
	}

	if (V == NULL) {
		V = " ";
	}

	int a_len = strlen(A) + 1;
	int v_len = strlen(V) + 1;
	int u_len = strlen(U) + 1;

	illuminaAVU->A = (char *) malloc(a_len * sizeof(char));
	illuminaAVU->V = (char *) malloc(v_len * sizeof(char));
	illuminaAVU->U = (char *) malloc(u_len * sizeof(char));

	memset(illuminaAVU->A, 0, a_len);
	memset(illuminaAVU->V, 0, v_len);
	memset(illuminaAVU->U, 0, u_len);

	strncpy(illuminaAVU->A, A, a_len);
	strncpy(illuminaAVU->V, V, v_len);
	strncpy(illuminaAVU->U, U, u_len);
	illuminaAVU->next = NULL;

	return illuminaAVU;
}

char** get_data_section_headers(int data_section_headers_len, char* data_section_line) {
	char** data_section_headers = (char**) malloc(data_section_headers_len * sizeof( char* ));
	char* token = strtok(data_section_line, ",");

	int header_len = strlen(token) + 1;
	data_section_headers[0] = (char*) malloc(header_len * sizeof(char));
	memset(data_section_headers[0], 0, header_len);
	strncpy(data_section_headers[0], token, header_len);

	for(int i = 1; i < data_section_headers_len; i++) {
		token = strtok(NULL, ",");
		header_len = strlen(token) + 1;
		data_section_headers[i] = (char*) malloc(header_len * sizeof(char));
		memset(data_section_headers[i], 0, header_len);
		strncpy(data_section_headers[i], token, header_len);
	}

	return data_section_headers;
}

void free_illumina_metadata(IlluminaX10Metadata** sample_sheet_metadata) {
	rodsLog(LOG_NOTICE, "%s Free memory used for the Sample Sheet metadata", ILLUMINA_MSI_LOG);

	IlluminaX10Metadata* metadata_tag = *sample_sheet_metadata;

	while (metadata_tag){
		IlluminaX10Metadata* currTag = metadata_tag;
		metadata_tag = metadata_tag->next;

		free(currTag->A);
		free(currTag->V);
		free(currTag->U);
		free(currTag);
	}

	*sample_sheet_metadata = NULL;
}

char* concat (char* str1, char* str2) {
	if (str1 == NULL || str2 == NULL) {
		return NULL;
	}

	int str_concat_len = strlen(str1) + strlen(str2) + 1;
	char* str_concat = (char*) malloc(str_concat_len * sizeof(char));

	memset(str_concat, 0, str_concat_len);

	strncpy(str_concat, str1, strlen(str1));
	strncat(str_concat, str2, strlen(str2));

	return str_concat;
}


void add_avu_to_metadata_list(IlluminaX10Metadata** metadata_list, char* A, char* V, char* U) {
	IlluminaX10Metadata* newAVU = createIlluminaAVU (A, V, U);

	if (*metadata_list && newAVU != NULL) {
		newAVU->next = *metadata_list;
	}

	*metadata_list = newAVU;
}
