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


class TestCreateVocabularyRule(unittest.TestCase):
    def setUp(self):
        # subprocess.check_call(['su', '-', 'irods'])
        subprocess.call(['irm', VOCABULARY_NAME])

        if os.path.exists(VOCABULARIES_DIR):
            shutil.rmtree(VOCABULARIES_DIR)

    def test_create_valid_vocab_rule(self):
        """
        mlxCreateVocabulary rule should be executed successfully in iRODS when adding a vocabulary to
        a collection that does not have one.
        """

        self.assertTrue(subprocess.check_call(CREATE_VOCAB_RULE_ARGS) == 0)

    def tearDown(self):
        rm_rf_vocab_file()


class TestCreateVocabularyInFileSystem(unittest.TestCase):
    def setUp(self):
        # subprocess.check_call(['su', '-', 'irods'])
        subprocess.call(['irm', VOCABULARY_NAME])

        if os.path.exists(VOCABULARIES_DIR):
            shutil.rmtree(VOCABULARIES_DIR)

        subprocess.call(CREATE_VOCAB_RULE_ARGS)

    def test_create_valid_vocab_dir_tree(self):
        """
        mlxCreateVocabulary rule should create a vocabulary tree under the vocabularies directory
        in the local file system
        """
        self.assertTrue(os.path.exists(VOCABULARIES_DIR))

    def test_create_valid_vocab_db_file(self):
        """
        mlxCreateVocabulary rule should create a *.vocab file in the file system
        """

        self.assertTrue(os.path.join(VOCABULARIES_DIR, '/msiZone/home/rods', VOCABULARY_NAME))

    def tearDown(self):
        rm_rf_vocab_file()


class TestCreateVocabularyInIRODS(unittest.TestCase):
    def setUp(self):
        # subprocess.check_call(['su', '-', 'irods'])
        subprocess.call(['irm', VOCABULARY_NAME])

        if os.path.exists(VOCABULARIES_DIR):
            shutil.rmtree(VOCABULARIES_DIR)

        subprocess.call(CREATE_VOCAB_RULE_ARGS)

    def test_create_valid_vocab_is_linked(self):
        """
        mlxCreateVocabulary rule should link the vocabulary database file to iRODS
        """

        self.assertIn(VOCABULARY_NAME, subprocess.check_output(['ils']))

    def tearDown(self):
        rm_rf_vocab_file()


if __name__ == '__main__':
    unittest.main()
