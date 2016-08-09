"""
Test Vocabulary creation in iRODS.
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


class TestVocabularyDatabase(unittest.TestCase):
    def setUp(self):
        # subprocess.call(['su', '-', VOCAB_AUTHOR])
        subprocess.call(['irm', VOCAB_NAME])

        if os.path.exists(VOCAB_DIR):
            shutil.rmtree(VOCAB_DIR)

        subprocess.call(CREATE_VOCAB_RULE_ARGS)
        self.conn = sqlite3.connect(os.path.join(VOCAB_DIR, IRODS_TEST_COLL_PATH, VOCAB_NAME))

    def test_vocab_table_schema(self):
        """
        mlxCreateVocabulary rule should create the VOCABULARIES table.
        """
        c = self.conn.cursor()
        c.execute("SELECT * FROM sqlite_master WHERE name =? and type='table'", (VOCAB_TABLE_NAME, ))

        self.assertTrue(c.fetchall())

    def test_vocab_metadata_table_schema(self):
        """
        mlxCreateVocabulary rule should create the VOCABULARY_METADATA table.
        """
        c = self.conn.cursor()
        c.execute("SELECT * FROM sqlite_master WHERE name =? and type='table'", (VOCAB_METADATA_TABLE_NAME,))

        self.assertTrue(c.fetchall())

    def test_vocab_auditing_table_schema(self):
        """
        mlxCreateVocabulary rule should create the VOCABULARY_AUDITING table.
        """
        pass

    def test_vocab_name_in_database(self):
        c = self.conn.cursor()
        c.execute("SELECT name FROM " + VOCAB_TABLE_NAME)

        self.assertEqual(c.fetchone()[0], VOCAB_NAME)

    def test_vocab_author_in_database(self):
        c = self.conn.cursor()
        c.execute("SELECT author FROM " + VOCAB_TABLE_NAME)

        self.assertEqual(c.fetchone()[0], VOCAB_AUTHOR)

    def test_vocab_created_at_in_database(self):
        c = self.conn.cursor()
        c.execute("SELECT created_at FROM " + VOCAB_TABLE_NAME)

        self.assertTrue(c.fetchone()[0])

    def test_vocab_modified_at_in_database(self):
        c = self.conn.cursor()
        c.execute("SELECT modified_at FROM " + VOCAB_TABLE_NAME)

        self.assertTrue(c.fetchone()[0])

    def tearDown(self):
        rm_rf_vocab_file()
        self.conn.close()


if __name__ == '__main__':
    unittest.main()
