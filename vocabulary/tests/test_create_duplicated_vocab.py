"""
Test creation of duplicated vocabularies.
"""

import unittest
import subprocess
import shutil
import os

CREATE_VOCAB_RULE_ARGS = ['irule', 'mlxCreateVocabulary', '"null"', '"null"']

VOCABULARY_NAME = 'test.vocab'

VOCABULARIES_DIR = '/etc/irods/vocabularies'


def copy_vocab_rules_file_to_etc_irods():
    """
    Copy the vocabulary_rules.re file to /etc/irods
    """
    shutil.copyfile('./vocabulary_rules.re', '/etc/irods/vocabulary_rules.re')


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

        copy_vocab_rules_file_to_etc_irods()

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
