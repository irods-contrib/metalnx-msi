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
        VocabConfig.call_irm_vocab()

        if os.path.exists(self.VOCAB_DIR):
            shutil.rmtree(self.VOCAB_DIR)

        VocabConfig.copy_vocab_rules_file_to_etc_irods()

        VocabConfig.call_create_vocab_rule()

    def test_remove_vocab(self):
        """
        mlxRemoveVocabulary rule should be executed successfully in iRODS for vocabulary removal.
        """
        self.assertTrue(VocabConfig.call_remove_vocab_rule() == 0)


if __name__ == '__main__':
    unittest.main()
