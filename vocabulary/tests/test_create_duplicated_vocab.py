"""
Test Vocabulary creation in iRODS.
"""

import unittest
import subprocess
import shutil
import os

CREATE_VOCAB_RULE_ARGS = ['irule', 'mlxCreateVocabulary', '"null"', '"null"']

VOCABULARY_NAME = 'test.vocab'

VOCABULARIES_DIR = '/etc/irods/vocabularies'


def rm_rf_vocab_file():
    # 1. Unlink *.vocab file from iRODS
    subprocess.call(['irm', VOCABULARY_NAME])

    # 2. rm -rf *.vocab file from file system
    if os.path.exists(VOCABULARIES_DIR):
        shutil.rmtree(VOCABULARIES_DIR)


class TestCreateDuplicatedVocabulary(unittest.TestCase):
    def setUp(self):
        # subprocess.check_call(['su', '-', 'irods'])
        subprocess.call(['irm', VOCABULARY_NAME])

        if os.path.exists(VOCABULARIES_DIR):
            shutil.rmtree(VOCABULARIES_DIR)

        subprocess.call(CREATE_VOCAB_RULE_ARGS)

    def test_create_duplicated_vocab(self):
        """
        mlxCreateVocabulary rule should throw an error when adding a vocabulary to a collection that already has one.
        """

        self.assertTrue(subprocess.call(CREATE_VOCAB_RULE_ARGS) != 0)

    def tearDown(self):
        rm_rf_vocab_file()


if __name__ == '__main__':
    unittest.main()
