from unittest import TestCase, main

from tests import MetaDataExtractConfig


class TestGetVersion(TestCase, MetaDataExtractConfig):
    def test_msi_api_version(self):
        rule_output = self.call_get_version_rule('version')

        self.assertIn("version", rule_output)
        self.assertIn("1", rule_output)


if __name__ == '__main__':
    main()
