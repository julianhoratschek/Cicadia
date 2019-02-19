# Cicadia
Provides a small, fast Tool for Managing Temperature Data mainly gathered by 1-WIRE-Devices, to perform Cosinor-Analysis on them and to finally plot them in SVG-Files.

## Getting Started

Unfortunately, there are no Mirrors (yet) to download fully functioning Versions of *Cicadia*. However, using the latest Code provided on Github you can easily compile your own Version.

### Prerequisites

* *Cicadia* is built on Qt 5.12, you will have to install the correct Framework according to the Machine you want to run *Cicadia* on. You can download Qt from [Here](https://www.qt.io/download).
* To plot Data *Cicadia* uses the beautiful Qt-based Library QCustomPlot, which is released under GNU GPL. *Cicadia* is currently using the Library in Version 2.0.1. You can get the latest Version from [Here](https://www.qcustomplot.com/index.php/download).
* Matrix Multiplication relies on the Eigen-Library Version 3.3.7, which can be downloaded from [Here](http://eigen.tuxfamily.org/index.php?title=Main_Page).
* Statistical Analysis is performed using *StatLib* by *Keith O'Hara* in Version 2.2.0. The Library is published under Apache 2.0 Licence and can be acquired [Here](https://github.com/kthohr/stats).

### Setup Folder Structure

* **Setup Qt**: Qt is well documented, you probably won't have Trouble finding step-by-step Instructions on setting it up on your specific System.
* **Setup QCustomPlot**:
    * Download and unpack *QCustomPlot*.
    * You will only need the Files *qcustomplot.cpp* and *qcustomplot.h*. Copy both Files into *Cicadia/UI/*.
* **Setup Eigen**:
    * Download and unpack *Eigen Library*.
    * Copy the Folder "eigen-eigen-versionnumber/Eigen" into *Cicadia/include*
* **Setup StatsLib**:
    * Download and unpack *StatsLib*.
    * Rename the Folder "stats" to "StatsLib"
    * Save it in *Cicadia/include*

The final Structure of the *Cicadia*-Folder prior to Compiling should look like (Files not shown):

```
Cicadia
|-Algorithms
|-include
|--Eigen                  <- User-inserted
|---...
|---src
|--StatsLib               <- User-inserted
|---...
|---include
|---notebooks
|---tests
|-Models
|-UI
|--...
|--qcustomplot.cpp        <- User-inserted
|--qcustomplot.h          <- User-inserted
```

### Compiling

Load *Cicadia/Cicadia.pro* with a Qt-Building Environment, e.g. QtCreator and compile. No additional Steps should be necessary.

## Built With

* [Qt 5.12](https://www.qt.io/download) - Used as Base Framework for GUI
* [QCustomPlot 2.0.1](https://www.qcustomplot.com/index.php/download) - Qt-based Plotting-Library
* [Eigen 3.3.7](http://eigen.tuxfamily.org/index.php?title=Main_Page) - Matrix (and Vector) processing Library
* [StatsLib 2.2.0](https://github.com/kthohr/stats) - Statistics Library


## Versioning

[SemVer](http://semver.org/) was used for Versioning

## Authors

* **Julian Horatschek**

## License

This Project is licensed under the Apache 2.0 Licence - see the [LICENSE](LICENSE) File for Details

