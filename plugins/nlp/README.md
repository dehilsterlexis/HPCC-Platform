NLP Plugin
================

This plugin exposes nlp-engine to ECL.  It is a wrapper around VisualText's nlp-engine:
* [GitHub](https://github.com/VisualText/nlp-engine)


Installation and Dependencies
------------------------------

The nlp plugin has a dependency on https://github.com/VisualText/nlp-engine which has been added to the HPCC-Platform repository as a git submodule.  To install:
```c
git submodule update --init --recursive
```

Quick Start
------------

Import the nlp plugin library and calculate a NLP index at the specified lat/long and at maximum resolution (15):
```c
IMPORT nlp from lib_nlp; 

inputstr := 'The quick brown fox jumped over the lazy dog';
analyzer := 'taiparse';
outputstr := nlp.RunAnalyzer(inputstr,analyzer);
```

API
----

### Type Declarations
```c
```

### NLP++ Functions

#### RunAnalyzer

```c
STRING RunAnalyzer(CONST VARSTRING strInput, CONST VARSTRING strAnalyzer)
```

Takes an input string and processes it with an NLP++ analyzer and returns the output string.

Returns an output string from the nlp analyzer.
