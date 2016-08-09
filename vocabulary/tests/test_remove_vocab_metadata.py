"""
Test Vocabulary metadata removal.
"""

import unittest
import subprocess
import shutil
import os
import sqlite3

VOCAB_AUTHOR = 'rods'

VOCAB_TABLE_NAME = 'VOCABULARIES'
VOCAB_METADATA_TABLE_NAME = 'VOCABULARY_METADATA'

IRODS_TEST_COLL_PATH = 'msiZone/home/rods'

CREATE_VOCAB_RULE_ARGS = ['irule', 'mlxCreateVocabulary', '"null"', '"null"']

VOCAB_NAME = 'test.vocab'

VOCAB_DIR = '/etc/irods/vocabularies'

def rm_rf_vocab_file():
    # 1. Unlink *.vocab file from iRODS
    subprocess.call(['irm', VOCAB_NAME])

    # 2. rm -rf *.vocab file from file system
    if os.path.exists(VOCAB_DIR):
        shutil.rmtree(VOCAB_DIR)


class TestRemoveVocabularyMetadataRule(unittest.TestCase):
    def setUp(self):
        # subprocess.check_call(['su', '-', VOCAB_AUTHOR])

        if os.path.exists(VOCAB_DIR):
            subprocess.call(['irm', VOCAB_NAME])
            shutil.rmtree(VOCAB_DIR)

        subprocess.call(CREATE_VOCAB_RULE_ARGS)
        subprocess.call(['irule', 'mlxAddVocabMetadata', '"null"', '"null"'])

    def test_remove_vocab(self):
        """
        mlxRemoveVocabulary rule should be executed successfully in iRODS for vocabulary removal.
        """
        self.assertTrue(subprocess.call(['irule', 'mlxRemoveVocabMetadata', '"null"', '"null"']) == 0)

    def tearDown(self):
        rm_rf_vocab_file()


class TestRemoveMetadataFromVocabularyDatabase(unittest.TestCase):
    def setUp(self):
        # subprocess.call(['su', '-', 'irods'])
        rm_rf_vocab_file()

        subprocess.call(CREATE_VOCAB_RULE_ARGS)
        subprocess.call(['irule', 'mlxAddVocabMetadata', '"null"', '"null"'])
        subprocess.call(['irule', 'mlxRemoveVocabMetadata', '"null"', '"null"'])

    def test_remove_metadata_from_vocab_database(self):
        """
        mlxAddVocabMetadata rule should add metadata to the vocabulary database
        """
        conn = sqlite3.connect(os.path.join(VOCAB_DIR, IRODS_TEST_COLL_PATH, VOCAB_NAME))
        c = conn.cursor()
        c.execute("SELECT * FROM " + VOCAB_METADATA_TABLE_NAME)

        # no results found after deletion
        self.assertFalse(c.fetchall())

if __name__ == '__main__':
    unittest.main()
