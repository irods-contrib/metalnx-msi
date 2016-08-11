"""
Test Vocabulary removal.
"""

import os
import shutil
import unittest

from tests import VocabConfig


class TestRemoveVocabularyRule(unittest.TestCase, VocabConfig):
    def setUp(self):
        # subprocess.check_call(['su', '-', VOCAB_AUTHOR])
        self.call_irm_vocab()

        if os.path.exists(self.VOCAB_DIR):
            shutil.rmtree(self.VOCAB_DIR)

        self.copy_vocab_rules_file_to_etc_irods()

        self.call_create_vocab_rule()

    def test_remove_vocab(self):
        """
        mlxRemoveVocabulary rule should be executed successfully in iRODS for vocabulary removal.
        """
        self.assertTrue(self.call_remove_vocab_rule(self.IRODS_TEST_COLL_PATH, self.IRODS_TEST_RESC) == 0)


if __name__ == '__main__':
    unittest.main()
