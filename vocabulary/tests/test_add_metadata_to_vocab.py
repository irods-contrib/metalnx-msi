"""
Test Vocabulary creation in iRODS.
"""

import unittest
import subprocess
import shutil
import os
import sqlite3

CREATE_VOCAB_RULE_ARGS = ['irule', 'mlxCreateVocabulary', '"null"', '"null"']
VOCAB_METADATA_TABLE_NAME = 'VOCABULARY_METADATA'

VOCAB_NAME = 'test.vocab'

VOCAB_DIR = '/etc/irods/vocabularies'

IRODS_TEST_COLL_PATH = 'msiZone/home/rods'


def rm_rf_vocab_file():
    # 1. Unlink *.vocab file from iRODS
    subprocess.call(['irm', VOCAB_NAME])

    # 2. rm -rf *.vocab file from file system
    if os.path.exists(VOCAB_DIR):
        shutil.rmtree(VOCAB_DIR)


class TestAddMetadataToVocabularyRule(unittest.TestCase):
    def setUp(self):
        # subprocess.call(['su', '-', 'irods'])
        rm_rf_vocab_file()

        subprocess.call(CREATE_VOCAB_RULE_ARGS)

    def test_add_metadata_to_vocab_rule(self):
        """
        mlxAddVocabMetadata rule should be executed successfully in iRODS when adding a new metadata to a vocabulary.
        """

        self.assertTrue(subprocess.call(['irule', 'mlxAddVocabMetadata', '"null"', '"null"']) == 0)

    def tearDown(self):
        rm_rf_vocab_file()


class TestAddMetadataToVocabularyDatabase(unittest.TestCase):
    def setUp(self):
        # subprocess.call(['su', '-', 'irods'])
        rm_rf_vocab_file()

        subprocess.call(CREATE_VOCAB_RULE_ARGS)
        subprocess.call(['irule', 'mlxAddVocabMetadata', '"null"', '"null"'])

    def test_add_metadata_to_vocab_database(self):
        """
        mlxAddVocabMetadata rule should add metadata to the vocabulary database
        """
        conn = sqlite3.connect(os.path.join(VOCAB_DIR, IRODS_TEST_COLL_PATH, VOCAB_NAME))
        c = conn.cursor()
        c.execute("SELECT * FROM " + VOCAB_METADATA_TABLE_NAME)

        for item in c.fetchone():
            self.assertTrue(item in ('ATTR_NAME', 'UNIT_NAME', 'TEXT'))

    def tearDown(self):
        rm_rf_vocab_file()


if __name__ == '__main__':
    unittest.main()