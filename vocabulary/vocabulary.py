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
        pass

    def update_metadata(self, metadata):
        # metadata = AVU + AVU_ID
        # look for the metadata whose ID = AVU_ID
        # replace current A by the new A
        # replace current V by the new V
        # replace current U by the new U

        pass

    def del_metadata(self, metadata):
        # look for the metadata whose ID = AVU_ID
        # delete from the Vocabulary_Metadata table

        pass


class VocabularyCtrl:

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
