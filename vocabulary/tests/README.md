Vocabulary Tests
================

Tests for the Metalnx Vocabulary micro-services. It also tests the database schema created to handle vocabularies under control of iRODS. This python module tests the following rules:

1. mlxCreateVocabulary
2. mlxAddVocabMetadata
3. mlxRemoveVocabMetadata
4. mlxRemoveVocabulary

Requirements
------------

Before running the tests, make sure you have both Python 2.7.5 and iRODS 4.1+ installed in your environment.


How to run the tests?
---------------------

To run all the tests use:

```console
python -m unittest discover
```

If you want to run tests separately, run the following command from inside the tests directory:

```console
python <name-of-the-test-file> # python test_create_vocab.py
```
