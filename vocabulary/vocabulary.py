#!/usr/bin/python
#	Copyright (c) 2015-2016, EMC Corporation
#
#	Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

import sqlite3
import os
from glob import glob
from datetime import datetime


class Vocabulary:
    def __init__(self, location):
        self.conn = sqlite3.connect(glob(os.path.join(location, '*.vocab'))[0])

    def _close_vocab(self):
        self.conn.close()

    def update_name(self, new_name):
        pass

    def update_location(self, new_location):
        pass

    def show_info(self):
        c = self.conn.cursor()
        c.execute('SELECT * FROM vocabularies')
        print c.fetchone()

    def show_metadata(self):
        c = self.conn.cursor()
        c.execute('SELECT * FROM vocabulary_metadata')
        print c.fetchall()

    def add_metadata(self, attr, value, unit, type=''):
        c = self.conn.cursor()
        c.execute('INSERT INTO vocabulary_metadata VALUES (?, ?, ?, ?)', (attr, value, unit, type))
        c.execute('UPDATE vocabularies SET modify_date = ?', (datetime.strftime(datetime.now(), '%Y-%m-%d %H:%M:%S'),))
        self.conn.commit()

    def update_metadata(self, metadata):
        # metadata = AVU + AVU_ID
        # look for the metadata whose ID = AVU_ID
        # replace current A by the new A
        # replace current V by the new V
        # replace current U by the new U
        pass

    def remove_metadata(self, attr, value, unit):
        c = self.conn.cursor()
        c.execute('DELETE FROM vocabulary_metadata WHERE attr=? AND value=? AND unit=?', (attr, value, unit))
        c.execute('UPDATE vocabularies SET modify_date = ?', (datetime.strftime(datetime.now(), '%Y-%m-%d %H:%M:%S'),))
        self.conn.commit()

    def is_metadata_defined(self, attr, value, unit):
        c = self.conn.cursor()
        c.execute('SELECT * FROM vocabulary_metadata WHERE attr=? AND value=? AND unit=?', (attr, value, unit))

        if c.fetchone() is None:
            return False

        return True


class VocabularyCtrl:
    def __init__(self):
        self.conn = None

    def create_vocabulary(self, name, author, location):
        vocab_db_path = os.path.join(location, name)

        if os.path.exists(vocab_db_path):
            raise Exception("A vocabulary already exists under " + location)

        os.close(os.open(vocab_db_path, os.O_CREAT))

        self.conn = sqlite3.connect(vocab_db_path)

        self._create_vocab_schema()
        self._create_vocab_metadata_schema()
        self._create_vocab_change_schema()

        self._create_vocab(name, author)

        self.conn.close()

    def _create_vocab(self, name, author):
        curr_time = datetime.strftime(datetime.now(), '%Y-%m-%d %H:%M:%S')
        c = self.conn.cursor()
        c.execute('INSERT INTO vocabularies VALUES (?, ?, ?, ?)', (name, author, curr_time, curr_time))
        self.conn.commit()

    def _create_vocab_schema(self):
        c = self.conn.cursor()
        c.execute('CREATE TABLE vocabularies (name text, author text, creation_date text, modify_date text)')
        self.conn.commit()

    def _create_vocab_metadata_schema(self):
        c = self.conn.cursor()
        c.execute('CREATE TABLE vocabulary_metadata (attr text, value text, unit text, type text)')
        self.conn.commit()

    def _create_vocab_change_schema(self):
        pass


def mlx_create_vocabulary(rule_args, callback):
    callback.writeLine("serverLog", "Creating Vocabulary...")
    VocabularyCtrl().create_vocabulary('test.vocab', 'arthur', '/etc/irods')
    callback.writeLine("serverLog", "Vocabulary created.")


def mlx_show_vocabulary_info(rule_args, callback):
    callback.writeLine("serverLog", "Showing vocabulary info...")
    vocab = Vocabulary('/etc/irods')
    vocab.show_info()
    callback.writeLine("serverLog", "Vocabulary info showed.")


def mlx_remove_vocabulary(rule_args, callback):
    callback.writeLine("serverLog", "Delete Vocabulary")


def mlx_update_vocabulary_location(rule_args, callback):
    callback.writeLine("serverLog", "Update Vocabulary Location")


def mlx_update_vocabulary_name(rule_args, callback):
    callback.writeLine("serverLog", "Update Vocabulary Name")


def mlx_add_metadata_to_vocabulary(rule_args, callback):
    callback.writeLine("serverLog", "Adding Metadata to Vocabulary...")
    vocab = Vocabulary('/etc/irods')
    vocab.add_metadata('vocab_attr1', 'vocab_value1', 'vocab_unit1')
    vocab.add_metadata('vocab_attr2', 'vocab_value2', 'vocab_unit2')
    vocab.add_metadata('vocab_attr3', 'vocab_value3', 'vocab_unit3')
    callback.writeLine("serverLog", "Metadata added to vocabulary.")


def mlx_show_metadata_of_vocabulary(rule_args, callback):
    callback.writeLine("serverLog", "Showing Metadata of Vocabulary...")
    Vocabulary('/etc/irods').show_metadata()
    callback.writeLine("serverLog", "Metadata of Vocabulary showed.")


def mlx_remove_metadata_from_vocabulary(rule_args, callback):
    callback.writeLine("serverLog", "Removing Metadata from Vocabulary...")
    Vocabulary('/etc/irods').remove_metadata('vocab_attr1', 'vocab_value1', 'vocab_unit1')
    callback.writeLine("serverLog", "Metadata of Vocabulary removed.")


def mlx_update_metadata_of_vocabulary(rule_args, callback):
    callback.writeLine("serverLog", "Update Metadata of a Vocabulary")


def acPreProcForModifyAVUMetadata(rule_args, callback):
    callback.writeLine('serverLog', 'overloading acPreProcForModifyAVUMetadata')

    if not rule_args:
        raise Exception('Could not read rule_args from rule')

    path, attr, val, unit = rule_args[2], rule_args[3], rule_args[4], rule_args[5]

    callback.writeLine('serverLog', 'Path = ' + path)
    callback.writeLine('serverLog', 'Attr = ' + attr)
    callback.writeLine('serverLog', 'Val = ' + val)
    callback.writeLine('serverLog', 'unt = ' + unit)

    path = '/etc/irods'

    if Vocabulary(path).is_metadata_defined(attr, val, unit):
        callback.writeLine('serverLog', 'AVU defined by vocabulary /etc/irods/test.vocab')
    else:
        callback.writeLine('serverLog', 'AVU NOT defined by vocabulary /etc/irods/test.vocab')


def main():
    VocabularyCtrl().create_vocabulary('test.vocab', 'arthur', '.')

    vocab = Vocabulary('.')
    vocab.show_info()
    vocab.add_metadata('vocab_attr1', 'vocab_value1', 'vocab_unit1')
    vocab.add_metadata('vocab_attr2', 'vocab_value2', 'vocab_unit2')
    vocab.add_metadata('vocab_attr3', 'vocab_value3', 'vocab_unit3')
    vocab.show_metadata()
    vocab.show_info()


if __name__ == '__main__':
    main()
