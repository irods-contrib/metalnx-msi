from unittest import TestCase, main

from tests import MetadataExtractConfig


class TestGetMicroservices(TestCase, MetadataExtractConfig):
    def test_msis_installed(self):
        rule_output = self.call_get_microservices_rule('msis')

        for irods_msi in self.IRODS_MSIS_INSTALLED:
            self.assertIn(irods_msi, rule_output)

        for mlx_msi in self.METALNX_MSIS_INSTALLED:
            self.assertIn(mlx_msi, rule_output)


if __name__ == '__main__':
    main()
