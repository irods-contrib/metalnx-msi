import os
from unittest import TestCase, main

from tests import MetadataExtractConfig, iput, irm, imeta_ls


class TestMetadataIlluminaExtraction(TestCase, MetadataExtractConfig):
    def setUp(self):
        iput(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'samples', self.ILLUMINA_FILE_NAME))

    def test_extract_illumina(self):
        illumina_args = {'objPath': self.ILLUMINA_OBJ_PATH,
                         'targetPath': self.IRODS_HOME_PATH,
                         'destResc': self.IRODS_RESC}

        if not self.IRODS_42:
            illumina_args['status'] = 'null'

        self.call_extract_metadata_for_illumina(**illumina_args)

        # '-d' option is used when metadatas are get from a file and not a collection
        rule_output = imeta_ls('-d', self.ILLUMINA_METADATA_FILE)
        self.assertNotEqual(rule_output, self.IMETA_LS_NONE.format(self.ILLUMINA_METADATA_FILE))

    def tearDown(self):
        irm('-rf', self.ILLUMINA_OBJ_PATH)
        irm('-rf', self.ILLUMINA_FOLDER_PATH)


if __name__ == '__main__':
    main()
