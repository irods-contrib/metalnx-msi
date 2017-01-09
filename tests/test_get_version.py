from unittest import TestCase, main

from tests import MetadataExtractConfig


class TestGetVersion(TestCase, MetadataExtractConfig):
    def setUp(self):
        version = self.MSI_PACKAGE_VERSION
        self.msi_api_version = version[:version.index('.')]

    def test_msi_api_version(self):
        rule_output = self.call_get_version_rule('version')
        self.assertTrue(rule_output.startswith('*version = {}'.format(self.msi_api_version)))


if __name__ == '__main__':
    main()
