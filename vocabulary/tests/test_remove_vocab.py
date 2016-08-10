"""
Test Vocabulary removal.
"""

import unittest
import subprocess
import shutil
import os

VOCAB_AUTHOR = 'rods'

VOCAB_TABLE_NAME = 'VOCABULARIES'
VOCAB_METADATA_TABLE_NAME = 'VOCABULARY_METADATA'

IRODS_TEST_COLL_PATH = 'msiZone/home/rods'

CREATE_VOCAB_RULE_ARGS = ['irule', 'mlxCreateVocabulary', '"null"', '"null"']

VOCAB_NAME = 'test.vocab'

VOCAB_DIR = '/etc/irods/vocabularies'


def copy_vocab_rules_file_to_etc_irods():
    """
    Copy the vocabulary_rules.re file to /etc/irods
    """
    shutil.copyfile('./vocabulary_rules.re', '/etc/irods/vocabulary_rules.re')


class TestRemoveVocabularyRule(unittest.TestCase):
    def setUp(self):
        # subprocess.check_call(['su', '-', VOCAB_AUTHOR])
        subprocess.call(['irm', VOCAB_NAME])

        if os.path.exists(VOCAB_DIR):
            shutil.rmtree(VOCAB_DIR)

        copy_vocab_rules_file_to_etc_irods()

        subprocess.call(CREATE_VOCAB_RULE_ARGS)

    def test_remove_vocab(self):
        """
        mlxRemoveVocabulary rule should be executed successfully in iRODS for vocabulary removal.
        """
        self.assertTrue(subprocess.call(['irule', 'mlxRemoveVocabulary', '"null"', '"null"']) == 0)


if __name__ == '__main__':
    unittest.main()
