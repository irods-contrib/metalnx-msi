"""
Test Vocabulary creation in iRODS.
"""

import unittest
import subprocess
import shutil
import os

CREATE_VOCAB_RULE_ARGS = ['irule', 'mlxCreateVocabulary', '"null"', '"null"']
ADD_VOCAB_METADATA_RULE_ARGS = ['irule', 'mlxAddVocabMetadata', '"null"', '"null"']

VOCABULARY_NAME = 'test.vocab'

VOCABULARIES_DIR = '/etc/irods/vocabularies'


def rm_rf_vocab_file():
    # 1. Unlink *.vocab file from iRODS
    subprocess.call(['irm', VOCABULARY_NAME])

    # 2. rm -rf *.vocab file from file system
    if os.path.exists(VOCABULARIES_DIR):
        shutil.rmtree(VOCABULARIES_DIR)


class TestAddMetadataToVocabulary(unittest.TestCase):
    def setUp(self):
        # subprocess.check_call(['su', '-', 'irods'])
        rm_rf_vocab_file()

        subprocess.call(CREATE_VOCAB_RULE_ARGS)

    def test_add_metadata_to_vocab_rule(self):
        """
        mlxAddVocabMetadata rule should be executed successfully in iRODS when adding a new metadata to a vocabulary.
        """

        self.assertTrue(subprocess.call(ADD_VOCAB_METADATA_RULE_ARGS) == 0)

    def tearDown(self):
        rm_rf_vocab_file()


if __name__ == '__main__':
    unittest.main()