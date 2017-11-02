# Test Card Settings Generator for Hickok Cardmatic tube tester 

Last updated: November 1, 2017 

---

This repository contains files to generate test card settings for the Hickok 
Cardmatic tube tester.

---

## Required Libraries
The files are implemented using C++11, so when not using GCC, set your compiler
flags accordingly.  The `sqlite3` library is also needed for compiling files 
inside the `sql` folder.  To install `sqlite3` in Ubuntu, open up a bash 
terminal and type
```
sudo apt-get update
sudo apt-get install sqlite3 libsqlite3-dev
```

---

## Background

As Cardmatic testers utilize punch cards to control tube test settings, a tube 
cannot be tested when the associated test card is lost or damaged. In the former
case, a new test card can be hand-punched according to the programming 
instructions detailed in the [RD Instruments KS-15874 instructions manual](http://bama.edebris.com/download/hickok/ks15874l2/ks15874l2.djvu ), 
using test parameters listed in manufacturer data sheets or tube data books.

Hickok and the US Navy also issued *Tube Test Conditions* for the KS-15784 and
the USM118 testers, listing test parameters for almost 2000 tubes.
Unfortunately this document is not available freely on the web, and only
reprints are available from a few eBay sellers.  I have checked out the extracts
from their listings.  Tube pin data are left out from those booklets, 
meaning that half the required information needed to program the test
card is missing from those manuals.  Also a cheapstake like myself cannot
justify spending $50 for reprints plus $25 shipping.

Later I came up with the idea of extracting tube test conditions from another
well-documented tube tester and translate them to Cardmatic test card settings.
A few searches on Google led me to the [AVO VCM163 "Valve" Tester manual](https://frank.pocnet.net/instruments/AVO/MF/AVO_VDM_23RD_Valve_Data-Manual-OCR-20150222.pdf).
The AVO documentation turns out to be a good candidate given someone from Sweden 
already OCRed the manual in searchable pdf.  It contains both tube pin data and 
test parameters, useful information needed to program Cardmatic test cards.

---

## The data conversion pipeline

Programming instructions in **pages 24-32** of the KS-15874 manual are coded
using C++ and STL containers.  The implementation is spread across the files
`cardmatic_cardpos.h`, `cardmatic_cardpos.cpp`, `cardmatic_globals.h` and 
`cardmatic_tube.h`.  The file `test_main.cpp` contains a test case for ECC83.
You should modify the main function accordingly, choosing the proper functions
that match with the type of tube for testing.

To compile those source files in Linux:

1. Open up a bash terminal.
2. cd into the directory where you save the files. 
3. Type in the following line

```
make
```

Once compiled, type the following to run the program:
```
./cardmatic
```

The folder `sql` contains all the files necessary to expedite the test card
making process.  The AVO data is extracted, reformatted as SQL tables and views, 
and cleaned using a few SQL queries.  The resulting file called 
`cardmatic.sqlite` has all unnecessary attributes, redundant and untestable 
tubes removed.  The SQLite3 wrapper files `cardmatic_sql.cpp` and 
`cardmatic_sql.h` contain the functions required to extract data from the
database. The results extracted from the query can then be forwarded to the 
data converter for further processing.  The file `test_main.cpp` shows an 
example how the AVO data is parsed and translated to a set of Cardmatic switches
that should be actuated. 

To compile those source files in Linux/Ubuntu:

1. Open up a bash terminal.
2. cd into the directory where you save the files. 
3. Type in the following line

```
make
```

Once compiled, type the following to run the program:
```
./cardmaticsql **Tube ID**
```
where **Tube ID** is the ID of the tube, ex. `ECC83`, `KT66` etc.

---
 
## TODOs
* Write more Test Cases
* Fix functions in `cardmatic_dataconver.cpp`
* Fix `cardmatic.sqlite` datatypes
* Do unit testing
* Write a GUI
* More to list...
