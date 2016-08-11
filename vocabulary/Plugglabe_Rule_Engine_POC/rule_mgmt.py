import os
from ConfigParser import RawConfigParser as CP
from urllib import urlretrieve as get


def clean_ruleset_dir(ruleset_dir):
    filelist = [f for f in os.listdir(ruleset_dir)]
    for f in filelist:
        os.remove(os.path.join(ruleset_dir, f))


def assemble_py(ruleset_dir):
    core_content = open('/etc/irods/rule_mgmt.py')
    filelist = [f for f in os.listdir(ruleset_dir) if f.endswith('py')]

    rule_contents = core_content.readlines()
    for f in filelist:
        rule_contents.append('\n\n\n')
        r = open(ruleset_dir + '/' + f)
        rule_contents.extend(r.readlines())
        r.close()

    core_content.close()
    core_content = open('/etc/irods/core.py', 'w+')
    for l in rule_contents:
        core_content.write(l)
    core_content.close()


def assemble_re(ruleset_dir):
    filelist = [f for f in os.listdir(ruleset_dir) if f.endswith('r')]

    rule_contents = []
    for f in filelist:
        rule_contents.append('\n\n')
        r = open(ruleset_dir + '/' + f)
        rule_contents.extend(r.readlines())
        r.close()

    core_content = open('/etc/irods/rulebase_r.re', 'w+')
    for l in rule_contents:
        core_content.write(l)
    core_content.close()


def rule_sync(rule_args, callback):
    log = callback.writeLine
    cp = CP()
    cp.read('/etc/irods/rules.manifest')

    log('serverLog', '[RULE MANAGEMENT] Cleaning local ruleset cache...')
    clean_ruleset_dir('/etc/irods/ruleset')

    server_base = cp.get('conf', 'server_base_url')

    for s in [x for x in cp.sections() if x != 'conf']:
        log('serverLog', '[RULE MANAGEMENT] Handling .{} rules'.format(s))
        for rule in cp.options(s):
            if cp.get(s, rule).lower() in ['true', 'yes', 'y', 'install', 'installed']:
                log('serverLog', '[RULE MANAGEMENT] Syncing rule {}...'.format(rule))
                get('{}/{}.{}'.format(server_base, rule, s), '/etc/irods/ruleset/{}.{}'.format(rule, s))
            else:
                log('serverLog', '[RULE MANAGEMENT] Skipping rule {} in the [{}] section'.format(rule, s))

    log('serverLog', '[RULE MANAGEMENT] Finished syncing rules')

    log('serverLog', '[RULE MANAGEMENT] Assembling ruleset for Python files.')
    assemble_py('/etc/irods/ruleset')

    log('serverLog', '[RULE MANAGEMENT] Assembling ruleset for RE files.')
    assemble_re('/etc/irods/ruleset')
