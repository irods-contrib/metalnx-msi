#include "metalnx.h"
#include "metalnx_msi_version.h"
#include "rsModAVUMetadata.hpp"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"

#include <iostream>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/filesystem.hpp>

#define MSI_LOG "[Metalnx Rule Deployment MSI]"

#define SERVER_CONFIG_FILE "/etc/irods/server_config.json"
#define RULE_DEST_DIR "/etc/irods"
#define PLUGIN_CONFIG_SECTION "plugin_configuration"
#define RULE_ENGINES_SECTION "rule_engines"
#define PLUGIN_SPECIFIC_CONFIG_SECTION "plugin_specific_configuration"
#define RULEBASE_SET_SECTION "re_rulebase_set"
#define RULE_FILE_EXTENSION ".re"
#define BUFFER_SIZE 65536

using namespace boost::filesystem;
using namespace rapidjson;

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

extern "C" {

    void add_rule_to_base_set(char* rule_filename) {
        rodsLog(LOG_NOTICE, "%s Add rule to base set [%s]\n", MSI_LOG,  rule_filename, SERVER_CONFIG_FILE);

        FILE* fp = fopen(SERVER_CONFIG_FILE, "rb");
        char readBuffer[BUFFER_SIZE];
        FileReadStream is(fp, readBuffer, sizeof(readBuffer));

        Document document;
        document.ParseStream(is);

        fclose(fp);

        rodsLog(LOG_NOTICE, "%s Adding rule [%s] to the server config base set [%s]\n", MSI_LOG,  rule_filename, SERVER_CONFIG_FILE);
        Document::AllocatorType& allocator = document.GetAllocator();
        Value& array = document["plugin_configuration"]["rule_engines"][0]["plugin_specific_configuration"]["re_rulebase_set"];
        array.PushBack(StringRef(""), allocator);
        for (SizeType i = array.Size()-1; i > 0u; --i)
            array[i] = array[i-1];
        array[0] = StringRef(rule_filename);

        rodsLog(LOG_NOTICE, "%s Saving new server config file\n", MSI_LOG);
        fp = fopen(SERVER_CONFIG_FILE, "wb");
        char writeBuffer[BUFFER_SIZE];
        FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
        PrettyWriter<FileWriteStream> writer(os);
        document.Accept(writer);
        fclose(fp);
        rodsLog(LOG_NOTICE, "%s Server config file saved\n", MSI_LOG);

        rodsLog(LOG_NOTICE, "%s Added rule [%s] to the server config base set [%s]\n", MSI_LOG, rule_filename, SERVER_CONFIG_FILE);
    }

    /**
    * Copies a file from a source path in iRODS to a destination path.
    */
    bool copy_file_from_irods(const char* src_path, const char* dest_path) {
        rodsLog(LOG_NOTICE, "%s Copying file from [%s] to [%s]\n", MSI_LOG, src_path, dest_path);

        path from (src_path);

        if (!is_regular_file(from)) {
            rodsLog(LOG_ERROR, "%s Source file [%s] is not a regular file\n", MSI_LOG, src_path);
            return false;
        }

        std::ifstream  src(src_path, std::ios::binary);
        std::ofstream  dst(dest_path, std::ios::binary);

        dst << src.rdbuf();

        rodsLog(LOG_NOTICE, "%s Copied file from [%s] to [%s]\n", MSI_LOG, src_path, dest_path);
        return true;
    }

    const char* get_rule_dst_path(char* rule_name) {
        std::string rule_dst_dir (RULE_DEST_DIR);
		std::string rule_dst_file (rule_name);
		std::string rule_dst_path = rule_dst_dir + "/" + rule_dst_file;
		return rule_dst_path.c_str();
    }

    int msirule_deployment(msParam_t* inRuleNameParam, msParam_t* inRuleFilePathParam, ruleExecInfo_t* rei ) {
        rodsLog(LOG_NOTICE, "%s MSI Rule Deployment [%s]\n", MSI_LOG, MSI_VERSION);

		char* rule_name = (char*) inRuleNameParam->inOutStruct;
		char* rule_src_path = (char*) inRuleFilePathParam->inOutStruct;

		rodsLog(LOG_NOTICE, "%s Deploying rule [%s]\n", MSI_LOG, rule_name);

		// copy the rule file from the source path into the /etc/irods directory
		std::string rule_dst_dir (RULE_DEST_DIR);
		std::string rule_dst_file (rule_name);
		rule_dst_file = rule_dst_file + RULE_FILE_EXTENSION;
		std::string rule_dst_path = rule_dst_dir + "/" + rule_dst_file;
		if(!copy_file_from_irods(rule_src_path, rule_dst_path.c_str())) {
		    return MSI_ERROR;
		}

		// add the new rule into the server config file
		add_rule_to_base_set(rule_name);

        return MSI_SUCCESS;
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
 		ruleExecInfo_t*>("msirule_deployment",
				std::function<int(
					msParam_t*,
					msParam_t*,
					ruleExecInfo_t*)>(msirule_deployment));

        // =-=-=-=-=-=-=-
        // hand it over to the system
        return msvc;

    } // plugin_factory

} // extern "C"




