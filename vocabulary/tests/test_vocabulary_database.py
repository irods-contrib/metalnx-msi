"""
Test Vocabulary database schema.
"""

import os
import shutil
import sqlite3
import unittest

from tests import VocabConfig


class TestVocabularyDatabase(unittest.TestCase, VocabConfig):
    def setUp(self):
        # subprocess.call(['su', '-', TEST_VOCAB_AUTHOR])
        self.call_irm_vocab()

        if os.path.exists(self.VOCAB_DIR):
            shutil.rmtree(self.VOCAB_DIR)

        self.call_create_vocab_rule(
            path=self.IRODS_TEST_COLL_ABS_PATH,
            resc=self.IRODS_TEST_RESC,
            vocab_name=self.TEST_VOCAB_NAME,
            vocab_author=self.TEST_VOCAB_AUTHOR
        )

        self.conn = sqlite3.connect(os.path.join(self.VOCAB_DIR, self.IRODS_TEST_COLL_REL_PATH, self.TEST_VOCAB_NAME))

    def test_vocab_table_schema(self):
        """
        mlxCreateVocabulary rule should create the VOCABULARIES table.
        """
        c = self.conn.cursor()
        c.execute("SELECT * FROM sqlite_master WHERE name =? and type='table'", (self.VOCAB_TABLE_NAME,))

        self.assertTrue(c.fetchall())

    def test_vocab_metadata_table_schema(self):
        """
        mlxCreateVocabulary rule should create the VOCABULARY_METADATA table.
        """
        c = self.conn.cursor()
        c.execute("SELECT * FROM sqlite_master WHERE name =? and type='table'", (self.VOCAB_METADATA_TABLE_NAME,))

        self.assertTrue(c.fetchall())

    def test_vocab_name_in_database(self):
        c = self.conn.cursor()
        c.execute("SELECT name FROM " + self.VOCAB_TABLE_NAME)

        self.assertEqual(c.fetchone()[0], self.TEST_VOCAB_NAME)

    def test_vocab_author_in_database(self):
        c = self.conn.cursor()
        c.execute("SELECT author FROM " + self.VOCAB_TABLE_NAME)

        self.assertEqual(c.fetchone()[0], self.TEST_VOCAB_AUTHOR)

    def test_vocab_created_at_in_database(self):
        c = self.conn.cursor()
        c.execute("SELECT created_at FROM " + self.VOCAB_TABLE_NAME)

        self.assertTrue(c.fetchone()[0])

    def test_vocab_modified_at_in_database(self):
        c = self.conn.cursor()
        c.execute("SELECT modified_at FROM " + self.VOCAB_TABLE_NAME)

        self.assertTrue(c.fetchone()[0])

    def tearDown(self):
        self.rm_rf_vocab_file()
        self.conn.close()


if __name__ == '__main__':
    unittest.main()
