import os
from unittest import TestCase, main

from tests import MetadataExtractConfig, iput, irm, imeta_ls


class TestMetadataVcfExtraction(TestCase, MetadataExtractConfig):
    def setUp(self):
        iput(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'samples', self.VCF_FILE_NAME))

    def test_extract_vcf(self):
        self.call_extract_metadata_for_vcf(
            inObjPathParam=self.VCF_OBJ_PATH,
            inFilePathParam=self.VCF_FILE_PATH
        )

        # '-d' option is used when metadatas are get from a file and not a collection
        rule_output = imeta_ls('-d', self.VCF_FILE_NAME)
        self.assertNotEqual(rule_output, self.IMETA_LS_NONE.format(self.VCF_FILE_NAME))

    def tearDown(self):
        irm('-rf', self.VCF_OBJ_PATH)


if __name__ == '__main__':
    main()
