MSI Tests
=========

Tests for the Metalnx micro-services. This python module tests metadata extraction from Illumina, JPEG, Bam, Manifest and VCF files. In addition, it also tests creation of random 
metadata and MSI package version.

Requirements
------------

Before running the tests, make sure you have both Python 2.7.5 and iRODS 4.1.8+ installed in your environment.


How to run the tests?
---------------------

To run the tests use (from outside the *tests* directory):

```console
python -m unittest discover
```
