"""
Test creation of duplicated vocabularies.
"""

import unittest
import subprocess
import shutil
import os

from tests import VocabConfig


class TestCreateDuplicatedVocabulary(unittest.TestCase, VocabConfig):
    def setUp(self):
        # subprocess.check_call(['su', '-', 'irods'])
        subprocess.call(['irm', self.VOCAB_NAME])

        if os.path.exists(self.VOCAB_DIR):
            shutil.rmtree(self.VOCAB_DIR)

        VocabConfig.copy_vocab_rules_file_to_etc_irods()

        VocabConfig.call_create_vocab_rule()

    def test_create_duplicated_vocab(self):
        """
        mlxCreateVocabulary rule should throw an error when adding a vocabulary to a collection that already has one.
        """

        self.assertTrue(VocabConfig.call_create_vocab_rule() != 0)

    def tearDown(self):
        VocabConfig.rm_rf_vocab_file()


if __name__ == '__main__':
    unittest.main()
