import os
import subprocess


def _check_call_output(*args, **kwargs):
    return subprocess.check_output(list(args), **kwargs)

def _call(*args, **kwargs):
    """
    Makes a subprocess call using the args given
    :param args: command
    :return: result of the command call
    """
    with open(os.devnull, 'w') as os_devnull:
        return subprocess.call(list(args), stdout=os_devnull, stderr=os_devnull, **kwargs)

def iput(*args, **kwargs):
    """
    put a file into the iRODS data grid
    :param args: should contain the file which will be uploaded
    :param kwargs:
    :return:
    """
    return _call('iput', *args, **kwargs)

def irm(*args, **kwargs):
    """
    Removes a path from irods
    :param path:
    :param args:
    :param kwargs:
    :return:
    """
    return _call('irm', *args, **kwargs)

def imeta_ls(*args, **kwargs):
    """
        List metadatas for a data grid object
        :param args: should contain name of the object which we want a metadata list
        :param kwargs:
        :return:
        """
    return _check_call_output('imeta', 'ls', *args, **kwargs)

class MetaDataExtractConfig:

    IRODS_USER = 'rods'
    IRODS_RESC = 'demoResc'
    IRODS_ZONE = 'tempZone'
    BAM_FILE_NAME = 'chrom20.ILLUMINA.bwa.JPT.low_coverage.bam'
    BAM_OBJ_PATH = '/{}/home/{}/{}'.format(IRODS_ZONE, IRODS_USER, BAM_FILE_NAME)
    BAM_FILE_PATH = '/var/lib/irods/iRODS/Vault/home/{}/{}'.format(IRODS_USER, BAM_FILE_NAME)
    EXTRACT_METADATA_FOR_BAM = 'mlxExtractMetaDataBam.r'

    RULE_HEADERS_PATH = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'rules')

    def __init__(self):
        pass

    def call_rule_from_file(self, call_function, rule_filename, *args, **kwargs):
        self.build_rule_file(rule_filename, *args, **kwargs)
        return call_function('irule', '-F', rule_filename)

    def call_extract_metadata_for_bam(self, check_output=False, *args, **kwargs):
        call_function = _check_call_output if check_output else _call
        return self.call_rule_from_file(call_function, self.EXTRACT_METADATA_FOR_BAM, *args, **kwargs)

    def build_rule_file(self, rule_filename, *args, **kwargs):
        path_header_file = os.path.join(self.RULE_HEADERS_PATH, rule_filename)

        with open(path_header_file, 'r') as header_file:
            header_content = header_file.read()

        input_params = ','.join('*{}="{}"'.format(k, v) for k, v in kwargs.iteritems()) if kwargs else 'null'
        output_params = '*{}'.format(',*'.join(args)) if args else 'ruleExecOut'

        rule_content = '{}\n{}\n{}'.format(
            header_content,
            'INPUT {}'.format(input_params),
            'OUTPUT {}'.format(output_params)
        )

        rule_filename = os.path.basename(path_header_file)
        with open(rule_filename, 'w') as rule_file:
            rule_file.write(rule_content)
