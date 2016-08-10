"""
Test Add Metadata to Vocabulary.
"""
import os
import sqlite3
import unittest

from tests import VocabConfig


class TestAddMetadataToVocabularyRule(unittest.TestCase, VocabConfig):
    def setUp(self):
        # subprocess.call(['su', '-', 'irods'])
        self.rm_rf_vocab_file()

        VocabConfig.copy_vocab_rules_file_to_etc_irods()

        VocabConfig.call_create_vocab_rule()

    def test_add_metadata_to_vocab_rule(self):
        """
        mlxAddVocabMetadata rule should be executed successfully in iRODS when adding a new metadata to a vocabulary.
        """

        self.assertTrue(VocabConfig.call_add_metadata_to_vocab_rule() == 0)

    def tearDown(self):
        VocabConfig.rm_rf_vocab_file()


class TestAddMetadataToVocabularyDatabase(unittest.TestCase, VocabConfig):
    def setUp(self):
        # subprocess.call(['su', '-', 'irods'])
        VocabConfig.rm_rf_vocab_file()

        VocabConfig.copy_vocab_rules_file_to_etc_irods()

        VocabConfig.call_create_vocab_rule()
        VocabConfig.call_add_metadata_to_vocab_rule()

    def test_add_metadata_to_vocab_database(self):
        """
        mlxAddVocabMetadata rule should add metadata to the vocabulary database
        """
        conn = sqlite3.connect(os.path.join(self.VOCAB_DIR, self.IRODS_TEST_COLL_PATH, self.VOCAB_NAME))
        c = conn.cursor()
        c.execute("SELECT * FROM " + self.VOCAB_METADATA_TABLE_NAME)

        for item in c.fetchone():
            self.assertTrue(item in ('ATTR_NAME', 'UNIT_NAME', 'TEXT'))

    def tearDown(self):
        VocabConfig.rm_rf_vocab_file()


if __name__ == '__main__':
    unittest.main()
