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
    def _call(args):
        response_status = -1
        with open(os.devnull, 'w') as os_devnull:
            response_status = subprocess.call(args, stdout=os_devnull, stderr=os_devnull)
        return response_status

    @staticmethod
    def call_create_vocab_rule():
        return VocabConfig._call(VocabConfig.CREATE_VOCAB_RULE_ARGS)

    @staticmethod
    def call_remove_vocab_rule():
        return VocabConfig._call(VocabConfig.REMOVE_VOCAB_RULE_ARGS)

    @staticmethod
    def call_add_metadata_to_vocab_rule():
        return VocabConfig._call(VocabConfig.ADD_VOCAB_METADATA_RULE_ARGS)

    @staticmethod
    def call_remove_metadata_from_vocab_rule():
        return VocabConfig._call(VocabConfig.REMOVE_VOCAB_METADATA_RULE_ARGS)

    @staticmethod
    def call_irm_vocab():
        return VocabConfig._call(['irm', VocabConfig.VOCAB_NAME])

    @staticmethod
    def copy_vocab_rules_file_to_etc_irods():
        """
        Copy the vocabulary_rules.re file to /etc/irods
        """
        shutil.copyfile(os.path.join(os.getcwd(), 'tests', VocabConfig.VOCAB_RULE_FILE_NAME),
                        '/etc/irods/vocabulary_rules.re')

    @staticmethod
    def rm_rf_vocab_file():
        # 1. Unlink *.vocab file from iRODS
        VocabConfig._call(['irm', VocabConfig.VOCAB_NAME])

        # 2. rm -rf *.vocab file from file system
        if os.path.exists(VocabConfig.VOCAB_DIR):
            shutil.rmtree(VocabConfig.VOCAB_DIR)
