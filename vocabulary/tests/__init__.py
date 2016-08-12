import os
import shutil
import subprocess


class VocabConfig:
    VOCAB_TABLE_NAME = 'VOCABULARIES'
    VOCAB_METADATA_TABLE_NAME = 'VOCABULARY_METADATA'

    IRODS_TEST_COLL_ABS_PATH = '/msiZone/home/rods'
    IRODS_TEST_COLL_REL_PATH = 'msiZone/home/rods'
    IRODS_TEST_RESC = 'demoResc'

    CREATE_VOCAB_RULE_ARGS = ['irule', 'mlxCreateVocabulary', '"null"', '"null"']

    TEST_VOCAB_NAME = 'test.vocab'
    TEST_VOCAB_AUTHOR = 'rods'
    VOCAB_DIR = '/etc/irods/vocabularies'
    VOCAB_RULE_FILE_NAME = 'vocabulary_rules.re'

    TEST_ATTR_NAME = "ATTR_NAME"
    TEST_ATTR_UNIT = "ATTR_UNIT"
    TEST_ATTR_TYPE = "TEXT"

    def __init__(self):
        pass

    def _call(self, path_header_file, **kwargs):
        with open(path_header_file, 'r') as header_file:
            header_content = header_file.read()

        input_str = 'INPUT '
        for key, value in kwargs.iteritems():
            input_str += '*{}="{}",'.format(key, value)

        rule_filename = os.path.basename(path_header_file)
        rule_content = '{}\n{}\nOUTPUT ruleExecOut'.format(header_content, input_str[:-1])

        with open(rule_filename, 'w') as rule_file:
            rule_file.write(rule_content)

        with open(os.devnull, 'w') as os_devnull:
            return subprocess.call(['irule', '-F', rule_filename], stdout=os_devnull, stderr=os_devnull)

    def call_create_vocab_rule(self, path, resc, vocab_name, vocab_author):
        return self._call(
            os.path.join(os.path.dirname(os.path.realpath(__file__)), 'rules', 'headers', 'mlxCreateVocabulary.r'),
            path=path,
            resc=resc,
            vocab_name=vocab_name,
            vocab_author=vocab_author,
        )

    def call_remove_vocab_rule(self, path, resc):
        return self._call(
            os.path.join(os.path.dirname(os.path.realpath(__file__)), 'rules', 'headers', 'mlxRemoveVocabulary.r'),
            path=path,
            resc=resc,
        )

    def call_add_metadata_to_vocab_rule(self, path, resc, attr_name, attr_unit, attr_type):
        return self._call(
            os.path.join(os.path.dirname(os.path.realpath(__file__)), 'rules', 'headers', 'mlxAddVocabMetadata.r'),
            path=path,
            resc=resc,
            attr_name=attr_name,
            attr_unit=attr_unit,
            attr_type=attr_type,
        )

    def call_remove_metadata_from_vocab_rule(self, path, resc, attr_name):
        return self._call(
            os.path.join(os.path.dirname(os.path.realpath(__file__)), 'rules', 'headers', 'mlxRemoveVocabMetadata.r'),
            path=path,
            resc=resc,
            attr_name=attr_name,
        )

    def call_irm_vocab(self):
        with open(os.devnull, 'w') as os_devnull:
            return subprocess.call(['irm', self.TEST_VOCAB_NAME], stdout=os_devnull, stderr=os_devnull)

    def copy_vocab_rules_file_to_etc_irods(self):
        """
        Copy the vocabulary_rules.re file to /etc/irods
        """
        shutil.copyfile(os.path.join(os.getcwd(), __name__, self.VOCAB_RULE_FILE_NAME),
                        '/etc/irods/vocabulary_rules.re')

    def rm_rf_vocab_file(self):
        # 1. Unlink *.vocab file from iRODS
        self.call_irm_vocab()

        # 2. rm -rf *.vocab file from file system
        if os.path.exists(self.VOCAB_DIR):
            shutil.rmtree(self.VOCAB_DIR)
