METALNX MSI
===========

Installation Guide
-------------------------

Copyright © 2015-2017 Dell EMC.

This software is provided under the Software license provided in the [LICENSE](../LICENSE) file.

The information in this file is provided “as is.” Dell EMC makes no representations or warranties of any kind with respect to the information in this publication, and specifically disclaims implied warranties of merchantability or fitness for a particular purpose.

-------------------------------- 

#<a id="TOC"></a>TABLE OF CONTENTS
1. [Introduction](#introduction)
2. [Overview](#overview)
3. [Resolving Metalnx MSI dependencies](#dependencies)
  * [libxml2 and libexif](#dependencies-xml)
  * [irods-devel package](#dependencies-irods)
  * [libhts shared object](#dependencies-libhts)
4. [Installing The Metalnx Micro Services](#install)
  * [On RPM-Based systems](#rpm-install)
  * [On DEB-Based systems](#deb-install)
  * [Verifying installed files](#verify-install)
5. [Using Metalnx Microservices](#using)

----------------------------------

# <a id="introduction"></a>Introduction
Metalnx MSI is a set of microservices designed to work alongside the iRODS ([integrated Rule-Oriented Data System][irods]). It provides automatic metadata extraction funcionalities for different types of files.

This installation guide will provide information on how to install the components necessary to run Metalnx MSI along with your current Metalnx installation. 

## Assumptions

In this installation guide, to fully install Metalnx MSI package, we will:

- Show how to install the _Metalnx micro services package_ which provides automated metadata extraction for .jpeg, .bam, .cram, .vcf, and some **Illumina** manifest files

Metalnx MSI has been tested on the following Linux distributions as indicated:

- CentOS 7 – all functional testing performed.
- Ubuntu 14 – all functional testing performed.

Metalnx MSI will work with iRODS 4.1 or later.  It has been tested the most using iRODS 4.1.8.

[Back to: Table of Contents](#TOC)

# <a id="overview"></a> Metalnx MSI overview

Metalnx MSI package provides a few [iRODS microservices][irods-msi] implementations for automatic metadata extraction. The supported file types handled by this set of microservices are:
* Image files (JPG, JPEG, PNG)
* [SAM files][samtools] (SAM, BAM, CRAM)
* Illumina projects (entire Illumina project compressed file)
* [Variant call files][vcf-files] (VCF)

[Back to Table of Contents](#TOC)

# <a id="dependencies"></a>Resolving Metalnx MSI dependencies

At a high level Metalnx MSI is dependent on the following software components being available:

- Already installed instance of an iRODS grid (4.1+)
- iRODS dev package, available on [iRODS downloads page][irods-downloads]
- [Samtools][samtools] and [HTSlib][samtools] installed and configured
- [libxml2][libxml2] development package
- [libexif][libexif] development package

## <a id="dependencies-xml"></a>libxml2 and libexif

The `libxml2` package is required for make it possible for the microservices to read *xml* files such as some manifest files for genetic researches. These files usually contain important metadata that can be assigned to project files on an iRODS grid.

Image files also contain metadata that can be extracted and assigned to the original file residing inside an iRODS grid. Image metadata are standardized by the [EXIF][exif-info] model. The `libexif` library provides a way of extracting this information from files.

Both libraries are available on official repositories for CentOS7 and Ubuntu 14. On CentOS7 they can be installed with the following command:

```bash
$ sudo yum install -y libexif-devel libxml2-devel
```

On Ubuntu the same can be achieved with:

```bash
$ sudo apt-get -y install libexif-dev libxml2-dev
```

## <a id="dependencies-irods"></a>irods-devel package

The `irods-devel` package can be downloaded from the [iRODS download page][irods-downloads]. On CentOS 7 it can be installed with the following command:

```bash
$ sudo yum install irods-dev-4.1.8-centos7-x86_64.rpm
```

> __Note:__ The `yum` command accepts local RPM files as input. When using `yum` to install RPM packages, it will automatically fetch all the dependencies from the central repository. The `irods-dev-4.1.8-centos7-x86_64.rpm
` package used in the last command is a __local__ file.

YUM will resolve the dependencies for the `irods-devel` package and will fetch them from the central repository.

The same operation can be executed on Ubuntu 14  with the following commands:

```bash
$ sudo dpkg -i irods-dev-4.1.8-ubuntu14-x86_64.deb
$ sudo apt-get -f install
```

## <a id="dependencies-libhts"></a>Installing and configuring `htslib` and `samtools`

The `samtools` library allows Metalnx microservices to get information from `SAM`, `BAM` and `CRAM` files. These libraries binaries are not available on CentOS7 nor Ubuntu 14 offical repositories. It implies that they must be manually compiled, configured and installed.

> __Note:__ A C++ compiler must be present in the machine you are working on. If you do not have it, you can install it with the following command:
```bash
$ yum install -y gcc-c++    # CentOS
$ apt-get install -y g++    # Ubuntu
```

First of all, download the samtools from the official website:

```bash
$ wget https://github.com/samtools/samtools/releases/download/1.3.1/samtools-1.3.1.tar.bz2
```

Unzip it:

```bash
$ tar -xvf samtools-1.3.1.tar.bz2 
```

Inside the `samtools-1.3.1` folder, there is also the `htslib-1.3.1` source code. It means that you won't need to download the htslib separately.

Go inside the `htslib-1.3.1` directory and build the source files:

```bash
$ ./configure
$ make
$ sudo make install
```

The default installation process of the htslib copies the shared object `libhts.so.1.3.1` to the `/usr/local/lib` directory which is not the default search location for libraries. We need to configure linker to look for it on its installation path. To do so, use the following commands:

```bash
$ echo "/usr/local/lib" > /etc/ld.so.conf.d/htslib.conf
$ sudo ldconfig
```

To confirm that the operation was successfully executed:

```bash
$ sudo ldconfig -p | grep hts
        libhts.so.1 (libc6,x86-64) => /usr/local/lib/libhts.so.1
        libhts.so (libc6,x86-64) => /usr/local/lib/libhts.so
```

The next step is to install the `samtools` files. The installation is pretty straightforward:

```bash
$ cd samtools-1.3.1
$ ./configure --without-curses
$ make
$ sudo make install
$ sudo cp *.h /usr/local/include/
```

[Back to Table of Contents](#TOC)

# <a id="install"></a> Installing Metalnx Microservices

Now that all the dependencies are satisfied, we need to install the `metalnx-msi-plugins` package. This section explains how to install it on RPM and DEB-based platforms.

[Back to Table of Contents](#TOC)

## <a id="rpm-install"></a> Install procedure on RPM-based systems

In order to get the `metalnx-msi-plugins` package installed on CentOS 7, execute:

```bash
$ rpm -ivh --nodeps metalnx-msi-plugins-1.0-centos7.rpm	
```

> Notice that the **--nodeps** option is used. It happens because the `libhts.so` is only used at runtime. We don't need to configure the `rpm` installation process to search on the linker directories.

[Back to Table of Contents](#TOC)

## <a id="deb-install"></a> Install procedure on DEB-based systems

In order to get the `metalnx-msi-plugins` package installed on Ubuntu 14, execute:

```bash
$ sudo dpkg -i metalnx-msi-plugins-1.0.deb
```

[Back to Table of Contents](#TOC)

## <a id="verify-install"></a> Verifying installed files

Once installed, the `metalnx-msi-plugins` will copy all the shared objects implementations to the default iRODS microservices folder. To make sure the files have been copied to the target directory, `cd` to the `/var/lib/irods/plugins/microservices` directory and make sure you see the following files:

* `libmsiget_illumina_meta.so` 
* `libmsiobjjpeg_extract.so`
* `libmsiobjput_mdbam.so`
* `libmsiobjput_mdmanifest.so`
* `libmsiobjput_mdvcf.so`
* `libmsiobjput_populate.so`

[Back to Table of Contents](#TOC)

# <a id="using"></a>Using Metalnx Microservices

The microservices are triggered by file types during the **upload** function on the Metalnx UI. It means that you upload a file called `photo.jpg`, Metalnx will try to execute the `jpeg_extract` microservice to get metadata from the file. 

All the operations executed by the microservices are traceable by the iRODS logs. These logs can be accessed on the `/var/lib/irods/iRODS/server/log` directory and they have a name format `rodsLogs.YYYY.MM.DD`.

> Notice that if your infrastructure contains multiple iRODS servers (iCAT and resources) the `metalnx-msi-plugins´ package **must** be installed in **all** of the servers. The microservices are executed locally on each one of the machines. It means that if a user uploads a file to a resource server and it does not have the Metalnx MSI package installed, the metadata extraction will fail. 

[Back to Table of Contents](#TOC)

[irods]: http://www.irods.org
[irods-downloads]: http://irods.org/download/
[irods-msi]: https://docs.irods.org/master/doxygen/
[samtools]: http://www.htslib.org/
[vcf-files]: http://www.1000genomes.org/wiki/Analysis/vcf4.0
[libxml2]: http://xmlsoft.org/
[libexif]: http://libexif.sourceforge.net/
[exif-info]: https://en.wikipedia.org/wiki/Exchangeable_image_file_format
