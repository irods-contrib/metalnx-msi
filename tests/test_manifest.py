import os
from unittest import TestCase, main

from tests import MetadataExtractConfig, iput, irm, imeta_ls


class TestManifest(TestCase, MetadataExtractConfig):
    def setUp(self):
        self.obj_path = os.path.join(self.IRODS_HOME_PATH, self.MANIFEST_FILE_NAME)
        self.file_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'samples', self.MANIFEST_FILE_NAME)

        irm('-rf', self.obj_path)
        iput(self.file_path)

    def test_manifest(self):
        self.call_manifest_rule(
            obj_path=self.obj_path,
            file_path=self.file_path,
            manifest_path=self.file_path
        )

        imeta_output = imeta_ls('-d', self.obj_path)

        self.assertIsNotNone(imeta_output)
        self.assertNotEqual(imeta_output, self.IMETA_LS_NONE.format(self.obj_path))

    def tearDown(self):
        irm('-rf', self.obj_path)

if __name__ == '__main__':
    main()
