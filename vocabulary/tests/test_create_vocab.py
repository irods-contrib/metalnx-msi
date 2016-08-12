"""
Test Vocabulary creation in iRODS.
"""

import os
import shutil
import subprocess
import unittest

from tests import VocabConfig


class TestCreateVocabularyRule(unittest.TestCase, VocabConfig):
    def setUp(self):
        # subprocess.call(['su', '-', self.TEST_VOCAB_AUTHOR])
        self.call_irm_vocab()

        if os.path.exists(self.VOCAB_DIR):
            shutil.rmtree(self.VOCAB_DIR)

    def test_create_valid_vocab_rule(self):
        """
        mlxCreateVocabulary rule should be executed successfully in iRODS when adding a vocabulary to
        a collection that does not have one.
        """

        self.assertTrue(self.call_create_vocab_rule(
            path=self.IRODS_TEST_COLL_ABS_PATH,
            resc=self.IRODS_TEST_RESC,
            vocab_name=self.TEST_VOCAB_NAME,
            vocab_author=self.TEST_VOCAB_AUTHOR
        ) == 0)

    def tearDown(self):
        self.rm_rf_vocab_file()


class TestCreateVocabularyInFileSystem(unittest.TestCase, VocabConfig):
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

    def test_create_valid_vocab_dir_tree(self):
        """
        mlxCreateVocabulary rule should create a vocabulary tree under the vocabularies directory
        in the local file system
        """
        self.assertTrue(os.path.exists(self.VOCAB_DIR))

    def test_create_valid_vocab_db_file(self):
        """
        mlxCreateVocabulary rule should create a *.vocab file in the file system
        """
        self.assertTrue(os.path.join(self.VOCAB_DIR, self.IRODS_TEST_COLL_REL_PATH, self.TEST_VOCAB_NAME))

    def tearDown(self):
        self.rm_rf_vocab_file()


class TestCreateVocabularyInIRODS(unittest.TestCase, VocabConfig):
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

    def test_create_valid_vocab_is_linked(self):
        """
        mlxCreateVocabulary rule should link the vocabulary database file to iRODS
        """

        self.assertIn(self.TEST_VOCAB_NAME, subprocess.check_output(['ils']))

    def tearDown(self):
        self.rm_rf_vocab_file()


if __name__ == '__main__':
    unittest.main()
