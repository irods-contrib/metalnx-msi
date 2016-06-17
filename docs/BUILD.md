<font color="#3892CF"> EMC METALNX MSI BUILD GUIDE
==================================================

<font color="#3892CF"> Build From Source Instructions
=====================================================

<font color="#A6A6A6"> <font size=+2> Revision 1.0

6/2016 </font>

----------------------------------

<font color="#000000">
Copyright © 2015-16 EMC Corporation.

This software is provided under [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0).

The information in this file is provided “as is.” EMC Corporation makes no representations or warranties of any kind with respect to the information in this publication, and specifically disclaims implied warranties of merchantability or fitness for a particular purpose. 

-------------------------------- 

<br>
<font color="#0066CC"> <font size=+2> __INTRODUCTION__ </font>

<font color="#000000"> <a name="Introduction"></a>

This tutorial guides you through the Metalnx MSI compilation pipeline. It explains how to build Metalnx MSI from scratch.

## Version
1.0
 
## Building Metalnx MSI RPM and DEB packages

The Metalnx MSI package is an optional package that provides a set of iRODS micro services allowing the data grid to automatically extract metadata from certain kinds of files. It relies on the irods-dev library provided by  [iRODS][irods-dev-download].

This package also provides automatic metadata extraction for genetic research files (BAM, SAM, CRAM, VCF and Illumina project manifest). In order to allow iRODS and Metalnx to understand these files formats, you must install additional libraries:
    * [HTSlib][htslib-download]
	* [samtools][samtools-download]

### Installing the irods-dev Package

In order to install the `irods-dev` package, you must satisfy its dependencies. The only package you'll need to install `irods-dev` is `openssl-dev` (on DEB systems) or `openssl-devel` (on RPM systems). Once this dependency is satisfied, you can install `irods-dev` with the command:

    $ rpm -ivh irods-dev-4.1.8-centos7-x86_64.rpm         # On RPM-based system
	
or

    $ dpkg -i irods-dev-4.1.8-ubuntu14-x86_64.deb         # On DEB-based systems

### Installing HTSlib

On DEB-based or RPM-based systems, you will need the basic development tools in order to download, compile and install HTSlib (C++ compiler,   make):

    $ wget https://github.com/samtools/htslib/releases/download/1.3.1/htslib-1.3.1.tar.bz2    # Getting sources from GitHub
    $ tar -xvf htslib-1.3.1.tar.bz2															  # Extracting tarball
    $ cd htslib-1.3.1																		  # Entering sources directory
    $ ./configure																			  # Configuring compilation pipeline
    $ make																					  # Compiling sources
    $ sudo make install																		  # Installing compiled binary and headers

### Installing samtools

On DEB-based or RPM-based systems, you will need the basic development tools in order to download, compile and install samtools (C++ compiler, make):

    $ wget https://github.com/samtools/samtools/releases/download/1.3.1/samtools-1.3.1.tar.bz2    # Getting sources from GitHub
    $ tar -xvf samtools-1.3.1.tar.bz2															  # Extracting tarball
    $ cd samtools-1.3.1																			  # Entering sources directory
    $ ./configure																				  # Configuring compilation pipeline
    $ make																						  # Compiling source code
    $ sudo make install																			  # Installing compiled binary
    $ sudo cp *.h /usr/local/include/															  # Installing headers

### Building packages

Once the previous requirements are satisfied, you can build the MSI package. The format of the package will depend on which platform you are executing these commands on. That happens because the Metalnx MSI packing system is based on EPM (Enterprise Package Manager), which is part of the iRODS compilation pipeline. The EPM automatically manages the package creation depending on which platform it is running on.

The first step is make sure you have the `metalnx-msi` sources downloaded and a terminal session opened on the source tree root:

    $ cd metalnx-msi
	
Then you have to make sure your Linux user has execution permission on the `build.sh` script:

    $ chmod u+x packaging/build.sh
	
The last step is to execute the `build.sh` script. 

**NOTE:** This command must be executed from the source tree root, otherwise it will NOT work.":

    $ ./packaging/build.sh

Once completed, your resulting package can be found at `<metalnx_msi_root>/linux-X.XX-<arch>/RPMS/<arch>metalnx-msi-plugins-1.0.0.<arch>.<rpm/deb>`.

[irods-dev-download]: http://irods.org/download/
[htslib-download]: https://github.com/samtools/htslib/releases/download/1.3.1/htslib-1.3.1.tar.bz2
[samtools-download]: https://github.com/samtools/samtools/releases/download/1.3.1/samtools-1.3.1.tar.bz2
