import os
import subprocess

import shutil


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

    @staticmethod
    def copy_vocab_rules_file_to_etc_irods():
        """
        Copy the vocabulary_rules.re file to /etc/irods
        """
        shutil.copyfile(os.path.join(os.getcwd(), __name__, VocabConfig.VOCAB_RULE_FILE_NAME),
                        '/etc/irods/vocabulary_rules.re')

    @staticmethod
    def rm_rf_vocab_file():
        # 1. Unlink *.vocab file from iRODS
        subprocess.call(['irm', VocabConfig.VOCAB_NAME])

        # 2. rm -rf *.vocab file from file system
        if os.path.exists(VocabConfig.VOCAB_DIR):
            shutil.rmtree(VocabConfig.VOCAB_DIR)
