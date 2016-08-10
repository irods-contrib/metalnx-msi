import os
import shutil
import subprocess


class VocabConfig:
    VOCAB_TABLE_NAME = 'VOCABULARIES'
    VOCAB_METADATA_TABLE_NAME = 'VOCABULARY_METADATA'

    IRODS_TEST_COLL_PATH = 'msiZone/home/rods'

    CREATE_VOCAB_RULE_ARGS = ['irule', 'mlxCreateVocabulary', '"null"', '"null"']
    ADD_VOCAB_METADATA_RULE_ARGS = ['irule', 'mlxAddVocabMetadata', '"null"', '"null"']
    REMOVE_VOCAB_METADATA_RULE_ARGS = ['irule', 'mlxRemoveVocabMetadata', '"null"', '"null"']
    REMOVE_VOCAB_RULE_ARGS = ['irule', 'mlxRemoveVocabulary', '"null"', '"null"']

    VOCAB_NAME = 'test.vocab'
    VOCAB_AUTHOR = 'rods'
    VOCAB_DIR = '/etc/irods/vocabularies'
    VOCAB_RULE_FILE_NAME = 'vocabulary_rules.re'

    def __init__(self):
        pass

    def _call(self, args):
        with open(os.devnull, 'w') as os_devnull:
            return subprocess.call(args, stdout=os_devnull, stderr=os_devnull)

    def call_create_vocab_rule(self):
        return self._call(self.CREATE_VOCAB_RULE_ARGS)

    def call_remove_vocab_rule(self):
        return self._call(self.REMOVE_VOCAB_RULE_ARGS)

    def call_add_metadata_to_vocab_rule(self):
        return self._call(self.ADD_VOCAB_METADATA_RULE_ARGS)

    def call_remove_metadata_from_vocab_rule(self):
        return self._call(self.REMOVE_VOCAB_METADATA_RULE_ARGS)

    def call_irm_vocab(self):
        return self._call(['irm', self.VOCAB_NAME])

    def copy_vocab_rules_file_to_etc_irods(self):
        """
        Copy the vocabulary_rules.re file to /etc/irods
        """
        shutil.copyfile(os.path.join(os.getcwd(), __name__, self.VOCAB_RULE_FILE_NAME),
                        '/etc/irods/vocabulary_rules.re')

    def rm_rf_vocab_file(self):
        # 1. Unlink *.vocab file from iRODS
        self._call(['irm', self.VOCAB_NAME])

        # 2. rm -rf *.vocab file from file system
        if os.path.exists(self.VOCAB_DIR):
            shutil.rmtree(self.VOCAB_DIR)
