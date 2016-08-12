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
        self.copy_vocab_rules_file_to_etc_irods()

        self.call_create_vocab_rule(
            path=self.IRODS_TEST_COLL_ABS_PATH,
            resc=self.IRODS_TEST_RESC,
            vocab_name=self.TEST_VOCAB_NAME,
            vocab_author=self.TEST_VOCAB_AUTHOR
        )

    def test_add_metadata_to_vocab_rule(self):
        """
        mlxAddVocabMetadata.r rule should be executed successfully in iRODS when adding a new metadata to a vocabulary.
        """

        self.assertTrue(
            self.call_add_metadata_to_vocab_rule(path=self.IRODS_TEST_COLL_ABS_PATH,
                                                 resc=self.IRODS_TEST_RESC,
                                                 attr_name=self.TEST_ATTR_NAME,
                                                 attr_unit=self.TEST_ATTR_UNIT,
                                                 attr_type=self.TEST_ATTR_TYPE) == 0
        )

    def tearDown(self):
        self.rm_rf_vocab_file()


class TestAddMetadataToVocabularyDatabase(unittest.TestCase, VocabConfig):
    def setUp(self):
        # subprocess.call(['su', '-', 'irods'])
        self.rm_rf_vocab_file()
        self.copy_vocab_rules_file_to_etc_irods()

        self.call_create_vocab_rule(
            path=self.IRODS_TEST_COLL_ABS_PATH,
            resc=self.IRODS_TEST_RESC,
            vocab_name=self.TEST_VOCAB_NAME,
            vocab_author=self.TEST_VOCAB_AUTHOR
        )

        self.call_add_metadata_to_vocab_rule(path=self.IRODS_TEST_COLL_ABS_PATH,
                                             resc=self.IRODS_TEST_RESC,
                                             attr_name=self.TEST_ATTR_NAME,
                                             attr_unit=self.TEST_ATTR_UNIT,
                                             attr_type=self.TEST_ATTR_TYPE)

    def test_add_metadata_to_vocab_database(self):
        """
        mlxAddVocabMetadata rule should add metadata to the vocabulary database
        """

        conn = sqlite3.connect(os.path.join(self.VOCAB_DIR, self.IRODS_TEST_COLL_REL_PATH, self.TEST_VOCAB_NAME))
        c = conn.cursor()
        c.execute("SELECT * FROM " + self.VOCAB_METADATA_TABLE_NAME)

        for item in c.fetchone():
            self.assertTrue(item in (self.TEST_ATTR_NAME, self.TEST_ATTR_UNIT, self.TEST_ATTR_TYPE))

    def tearDown(self):
        self.rm_rf_vocab_file()


class TestAddDuplicatedMetadataToVocabularyDatabase(unittest.TestCase, VocabConfig):
    def setUp(self):
        self.rm_rf_vocab_file()
        self.copy_vocab_rules_file_to_etc_irods()

        self.call_create_vocab_rule(
            path=self.IRODS_TEST_COLL_ABS_PATH,
            resc=self.IRODS_TEST_RESC,
            vocab_name=self.TEST_VOCAB_NAME,
            vocab_author=self.TEST_VOCAB_AUTHOR
        )

        self.call_add_metadata_to_vocab_rule(path=self.IRODS_TEST_COLL_ABS_PATH,
                                             resc=self.IRODS_TEST_RESC,
                                             attr_name=self.TEST_ATTR_NAME,
                                             attr_unit=self.TEST_ATTR_UNIT,
                                             attr_type=self.TEST_ATTR_TYPE)

    def test_add_duplicated_metadata_to_vocab(self):
        """
        Adding a metadata that already exists to a vocab should not succeed (return != 0)
        """
        self.assertTrue(
            self.call_add_metadata_to_vocab_rule(path=self.IRODS_TEST_COLL_ABS_PATH,
                                                 resc=self.IRODS_TEST_RESC,
                                                 attr_name=self.TEST_ATTR_NAME,
                                                 attr_unit=self.TEST_ATTR_UNIT,
                                                 attr_type=self.TEST_ATTR_TYPE) != 0)

    def tearDown(self):
        self.rm_rf_vocab_file()


if __name__ == '__main__':
    unittest.main()
