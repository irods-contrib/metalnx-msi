import os
from unittest import TestCase, main
from tests import MetaDataExtractConfig, iput, irm, imeta_ls


class TestMetaDataBamExtraction(TestCase, MetaDataExtractConfig):
    def setUp(self):
        iput(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'samples', 'chrom20.ILLUMINA.bwa.JPT.low_coverage.bam'))

    def test_extract_bam(self):
        self.call_extract_metadata_for_bam(
            inObjPathParam=self.BAM_OBJ_PATH,
            inFilePathParam=self.BAM_FILE_PATH
        )

        # '-d' option is used when metadatas are get from a file and not a collection
        rule_output = imeta_ls('-d', self.BAM_FILE_NAME)
        self.assertIsNotNone(rule_output)

    def tearDown(self):
        irm('-rf', self.BAM_OBJ_PATH)


if __name__ == '__main__':
    main()
