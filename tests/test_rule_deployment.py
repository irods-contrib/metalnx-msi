import json
from os.path import join, dirname, realpath, exists
from unittest import TestCase, main

from tests import MetadataExtractConfig, iput, irm, _call


def load_server_config_as_json(path):
    """
    Loads the iRODS server config file as JSON
    :param path: path to the server config file (typically under /etc/irods)
    :return: server config file content as JSON
    """
    with open(path, 'r') as server_config_fp:
        server_config_json = json.loads(server_config_fp.read())
    return server_config_json


def get_rule_base_set_from_config(server_config_json):
    """
    Finds the rule base set section in the iRODS server config file
    :param server_config_json: iRODS server config file as JSON
    :return: the 're_rulebase_set' JSON section (array)
    """

    # plugin_config > rule_engines > first array item > plugin_spec_config > re_rulebase_set
    rule_base_set = server_config_json['plugin_configuration']['rule_engines'][0]['plugin_specific_configuration'][
        're_rulebase_set']
    return rule_base_set


class TestRuleDeployment(TestCase, MetadataExtractConfig):
    def setUp(self):
        self.rule_file_path = join(self.VAULT_ROOT_PATH, self.RULE_CACHE_DIR_NAME, self.RULE_DEPLOYMENT_FILE_NAME)
        self.rule_obj_path = '{}/{}'.format(self.RULE_CACHE_IRODS_PATH, self.RULE_DEPLOYMENT_FILE_NAME)

        self.cleanUp()

        local_rule_file = join(dirname(realpath(__file__)), 'samples', self.RULE_DEPLOYMENT_FILE_NAME)
        iput(local_rule_file, self.RULE_CACHE_IRODS_PATH)  # iput file into /tempZone/.rulecache

    def test_deploy_rule(self):
        self.call_rule_deployment(rule_name=self.RULE_DEPLOYMENT_FILE_NAME[:-3], rule_file_path=self.rule_file_path)
        self.assertTrue(exists(join(self.IRODS_ETC_DIR, self.RULE_DEPLOYMENT_FILE_NAME)))

        server_config_json = load_server_config_as_json(self.IRODS_SERVER_CONFIG_PATH)
        rule_base_set = get_rule_base_set_from_config(server_config_json)

        self.assertTrue(self.RULE_DEPLOYMENT_FILE_NAME[:-3] in rule_base_set)  # rule must be present in the config file
        self.assertEqual(self.RULE_DEPLOYMENT_FILE_NAME[:-3], rule_base_set[0])  # rule must be the first one in the list

    def tearDown(self):
        self.cleanUp()

    def cleanUp(self):
        irm('-rf', self.rule_obj_path)  # remove file from iRODS
        _call('rm', '-rf', self.rule_file_path)  # remove file from local file system
        _call('rm', '-rf', join(self.IRODS_ETC_DIR, self.RULE_DEPLOYMENT_FILE_NAME))  # remove rule from /etc/irods

if __name__ == '__main__':
    main()
