"""
Test Vocabulary creation in iRODS.
"""

import unittest
import subprocess
import shutil
import os

VOCAB_AUTHOR = 'rods'

VOCAB_TABLE_NAME = 'VOCABULARIES'
VOCAB_METADATA_TABLE_NAME = 'VOCABULARY_METADATA'

IRODS_TEST_COLL_PATH = 'msiZone/home/rods'

CREATE_VOCAB_RULE_ARGS = ['irule', 'mlxCreateVocabulary', '"null"', '"null"']

VOCAB_NAME = 'test.vocab'

VOCAB_DIR = '/etc/irods/vocabularies'


def copy_vocab_rules_file_to_etc_irods():
    """
    Copy the vocabulary_rules.re file to /etc/irods
    """
    shutil.copyfile('./vocabulary_rules.re', '/etc/irods/vocabulary_rules.re')


def rm_rf_vocab_file():
    # 1. Unlink *.vocab file from iRODS
    subprocess.call(['irm', VOCAB_NAME])

    # 2. rm -rf *.vocab file from file system
    if os.path.exists(VOCAB_DIR):
        shutil.rmtree(VOCAB_DIR)


class TestCreateVocabularyRule(unittest.TestCase):
    def setUp(self):
        # subprocess.call(['su', '-', VOCAB_AUTHOR])
        subprocess.call(['irm', VOCAB_NAME])

        if os.path.exists(VOCAB_DIR):
            shutil.rmtree(VOCAB_DIR)

        copy_vocab_rules_file_to_etc_irods()

    def test_create_valid_vocab_rule(self):
        """
        mlxCreateVocabulary rule should be executed successfully in iRODS when adding a vocabulary to
        a collection that does not have one.
        """

        self.assertTrue(subprocess.call(CREATE_VOCAB_RULE_ARGS) == 0)

    def tearDown(self):
        rm_rf_vocab_file()


class TestCreateVocabularyInFileSystem(unittest.TestCase):
    def setUp(self):
        # subprocess.call(['su', '-', VOCAB_AUTHOR])
        subprocess.call(['irm', VOCAB_NAME])

        if os.path.exists(VOCAB_DIR):
            shutil.rmtree(VOCAB_DIR)

        copy_vocab_rules_file_to_etc_irods()

        subprocess.call(CREATE_VOCAB_RULE_ARGS)

    def test_create_valid_vocab_dir_tree(self):
        """
        mlxCreateVocabulary rule should create a vocabulary tree under the vocabularies directory
        in the local file system
        """
        self.assertTrue(os.path.exists(VOCAB_DIR))

    def test_create_valid_vocab_db_file(self):
        """
        mlxCreateVocabulary rule should create a *.vocab file in the file system
        """

        self.assertTrue(os.path.join(VOCAB_DIR, IRODS_TEST_COLL_PATH, VOCAB_NAME))

    def tearDown(self):
        rm_rf_vocab_file()


class TestCreateVocabularyInIRODS(unittest.TestCase):
    def setUp(self):
        # subprocess.call(['su', '-', VOCAB_AUTHOR])
        subprocess.call(['irm', VOCAB_NAME])

        if os.path.exists(VOCAB_DIR):
            shutil.rmtree(VOCAB_DIR)

        copy_vocab_rules_file_to_etc_irods()

        subprocess.call(CREATE_VOCAB_RULE_ARGS)

    def test_create_valid_vocab_is_linked(self):
        """
        mlxCreateVocabulary rule should link the vocabulary database file to iRODS
        """

        self.assertIn(VOCAB_NAME, subprocess.check_output(['ils']))

    def tearDown(self):
        rm_rf_vocab_file()


if __name__ == '__main__':
    unittest.main()
