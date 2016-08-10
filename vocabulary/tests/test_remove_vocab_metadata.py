"""
Test Vocabulary metadata removal.
"""

import unittest
import subprocess
import shutil
import os
import sqlite3

from tests import VocabConfig


class TestRemoveVocabularyMetadataRule(unittest.TestCase, VocabConfig):
    def setUp(self):
        # subprocess.check_call(['su', '-', VOCAB_AUTHOR])

        if os.path.exists(self.VOCAB_DIR):
            subprocess.call(['irm', self.VOCAB_NAME])
            shutil.rmtree(self.VOCAB_DIR)

        VocabConfig.copy_vocab_rules_file_to_etc_irods()

        VocabConfig.call_create_vocab_rule()
        VocabConfig.call_add_metadata_to_vocab_rule()

    def test_remove_vocab(self):
        """
        mlxRemoveVocabulary rule should be executed successfully in iRODS for vocabulary removal.
        """
        self.assertTrue(VocabConfig.call_remove_metadata_from_vocab_rule() == 0)

    def tearDown(self):
        VocabConfig.rm_rf_vocab_file()


class TestRemoveMetadataFromVocabularyDatabase(unittest.TestCase, VocabConfig):
    def setUp(self):
        # subprocess.call(['su', '-', 'irods'])
        VocabConfig.rm_rf_vocab_file()

        VocabConfig.copy_vocab_rules_file_to_etc_irods()

        VocabConfig.call_create_vocab_rule()
        VocabConfig.call_add_metadata_to_vocab_rule()
        VocabConfig.call_remove_metadata_from_vocab_rule()

    def test_remove_metadata_from_vocab_database(self):
        """
        mlxAddVocabMetadata rule should add metadata to the vocabulary database
        """
        conn = sqlite3.connect(os.path.join(self.VOCAB_DIR, self.IRODS_TEST_COLL_PATH, self.VOCAB_NAME))
        c = conn.cursor()
        c.execute("SELECT * FROM " + self.VOCAB_METADATA_TABLE_NAME)

        # no results found after deletion
        self.assertFalse(c.fetchall())

if __name__ == '__main__':
    unittest.main()
