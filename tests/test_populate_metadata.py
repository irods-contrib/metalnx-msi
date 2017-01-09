import os
from unittest import TestCase, main

from tests import MetaDataExtractConfig, iput, irm, imeta_ls


class TestPopulate(TestCase, MetaDataExtractConfig):
    def setUp(self):
        self.obj_path = os.path.join(self.IRODS_HOME_PATH, self.POPULATE_FILE_NAME)

        irm('-rf', self.obj_path)
        iput(self.POPULATE_FILE_PATH)

    def test_populate(self):
        self.call_populate_metadata(obj_path=self.obj_path)

        imeta_output = imeta_ls('-d', self.obj_path)

        self.assertIn("ATTRIBUTE01", imeta_output)
        self.assertIn("VALUE1", imeta_output)
        self.assertIn("chars", imeta_output)

        self.assertIn("ATTRIBUTE02", imeta_output)
        self.assertIn("2", imeta_output)
        self.assertIn("ints", imeta_output)

        self.assertIn("ATTRIBUTE03", imeta_output)
        self.assertIn("2, 3, 4, 5", imeta_output)

        self.assertIn("ATTRIBUTE04", imeta_output)
        self.assertIn("VALUE-1235", imeta_output)

        self.assertIn("ATTRIBUTE05", imeta_output)
        self.assertIn("1865", imeta_output)
        self.assertIn("bytes", imeta_output)

        self.assertIn("ATTRIBUTE06", imeta_output)
        self.assertIn("False", imeta_output)
        self.assertIn("bool", imeta_output)

        self.assertIn("ATTRIBUTE07", imeta_output)
        self.assertIn("_TAG-12342_", imeta_output)

        self.assertIn("ATTRIBUTE08", imeta_output)
        self.assertIn("['value1', 'value2, 'value3']", imeta_output)
        self.assertIn("as", imeta_output)

        self.assertIn("ATTRIBUTE09", imeta_output)
        self.assertIn("MLX", imeta_output)

        self.assertIn("ATTRIBUTE10", imeta_output)
        self.assertIn("NULL", imeta_output)

    def tearDown(self):
        irm('-rf', self.obj_path)

if __name__ == '__main__':
    main()
