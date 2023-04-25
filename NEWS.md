# Version 1.73.0 [2023-04-25]

## Notes

 * The version number was bumped for the Bioconductor release version,
   which is now Bioconductor 3.18 for R (>= 4.4.0).


# Version 1.72.0 [2023-04-25]

## Notes

 * The version number was bumped for the Bioconductor release version,
   which is now Bioconductor 3.17 for R (>= 4.3.0).


# Version 1.71.2 [2023-04-23]

### Bug Fixes

 * fix to `src/_mingw.h` provided by Tomas Kalibera.

# Version 1.71.1 [2023-04-04]

### Bug Fixes

 * Fix two instances of "watching polymorphic type 'class Except' by
   value [-Wcatch-value=]" compiler warnings.


# Version 1.71.0 [2022-11-01]

## Notes

 * The version number was bumped for the Bioconductor devel version,
   which is now Bioconductor 3.17 for R devel.


# Version 1.70.0 [2022-11-01]

## Miscellaneous

 * Remove extra backslash escaping in a few help pages.
 
## Notes

 * The version number was bumped for the Bioconductor release version,
   which is now Bioconductor 3.16 for R (>= 4.2.2).

 
# Version 1.69.1 [2022-04-28]
 
## Bug Fixes
 
 * Ported bug fix from **affxparser** 1.68.1.
 
 
# Version 1.69.0 [2022-04-26]

 
# Version 1.68.1 [2022-04-28]
 
## Bug Fixes
 
 * **affxparser** (>= 1.67.1) failed to install with R built with
   `-fpic` flag. The symptom was a linking error `ld: 000.init.o:
   relocation R_X86_64_32 against '.rodata' can not be used when
   making a shared object; recompile with -fPIC collect2: error: ld
   returned 1 exit status`.
 
 
# Version 1.68.0 [2022-04-26]

 
# Version 1.67.1 [2022-03-23]
 
## Significant Changes
 
 * This packages requires R (>= 4.0.0) when build on MS Windows. This
   is due to the added support for UCRT on MS Windows, which is
   required for the upcoming R 4.2.0.
 
## Software Quality
 
 * Updates to build package from source on MS Windows with UCRT.
   Thanks to Tomas Kalibera for the contribution.
 
 * Now registering native routines - apparently never happened before.
 
 
# Version 1.67.0 [2021-10-27]
 

# Version 1.66.0 [2021-10-27]

 
# Version 1.65.3 [2021-09-22]
 
## Software Quality
 
 * Making sure all pathnames are of length 100 or shorter.
 
 
# Version 1.65.2 [2021-09-22]
 
## Software Quality
 
 * Now properly registering native routines.
 
 
# Version 1.65.1 [2021-09-09]
 
## Bug Fixes
 
 * The package did not install on macOS with the M1 chip with error:
   `use of undeclared identifier 'finite'; did you mean
   'isfinite'?`. This issue goes back to 2014, when macOS produced
   `warning: 'finite' is deprecated: first deprecated in OS X 10.9
   [-Wdeprecated-declarations]. isOk = finite(x);`.  Patched by using
   `isfinite()` instead of `finite()`.
 
 
# Version 1.64.0 [2021-05-19]

 
# Version 1.62.0 [2020-10-27]

 
# Version 1.60.0 [2020-04-27]
 

# Version 1.58.0 [2019-10-29]

 
# Version 1.56.0 [2019-05-02]
 

# Version 1.55.0 [2018-10-30]
 

# Version 1.54.0 [2018-10-30]

 
# Version 1.53.2 [2018-10-22]
 
## Documentation
 
 * Link to Affx Fusion SDK archive on GitHub.
 
 * Spell corrections.
 
 
# Version 1.53.1 [2018-08-28]

 
# Version 1.53.0 [2018-04-30]
 

# Version 1.52.0 [2018-04-30]

 
# Version 1.51.0 [2017-10-30]
 

# Version 1.50.0 [2017-10-30]

 
# Version 1.49.0 [2017-04-23]
 

# Version 1.48.0 [2017-04-23]

 
# Version 1.47.0 [2016-10-18]
 
## Notes
 
 * The version number was bumped for the Bioconductor devel version,
   which is now Bioconductor 3.5 for R (>= 3.4.0).
 
 
# Version 1.46.0 [2016-10-18]
 
## Notes
 
 * The version number was bumped for the Bioconductor release version,
   which is now Bioconductor 3.4 for R (>= 3.3.1).
 
 
# Version 1.45.1 [2016-09-16]
 
## Code Refactoring
 
 * Using `c(x,y)` instead of `append(x,y)` internally.
 
 * CLEANUP: Dropped obsolete `src/R_affx_test.*cmdline.cpp` files.
 
 
# Version 1.45.0 [2015-05-03]
 
## Notes
 
 * The version number was bumped for the Bioconductor devel version,
   which is now Bioconductor 3.4 for R (>= 3.3.0).
 
 
# Version 1.44.0 [2015-05-03]
 
## Notes
 
 * The version number was bumped for the Bioconductor release version,
   which is now Bioconductor 3.3 for R (>= 3.3.0).
 
 
# Version 1.43.2 [2016-04-05]
 
## New Features
 
 * WINDOWS: Package now compiles with both the old gcc-4.6.3 toolchain
   as well as the new gcc-4.9.3 toolchain - introduced in R (>=
   3.3.0).  Thanks to Jim Hester and Dan Tenenbaum for help with this.
 
 
# Version 1.43.1 [2016-02-28]
 
## New Features
 
 * The DLL is now unloaded when the package is unloaded.
 
## Bug Fixes
 
 * Fixed a bug related to including `<R.h>` and extern C, reported by
   Brian Ripley.
 
 
# Version 1.43.0 [2015-10-23]
 
## Notes
 
 * The version number was bumped for the Bioconductor devel version,
   which is now Bioconductor 3.3 for R (>= 3.3.0).
 
 
# Version 1.42.0 [2015-10-13]
 
## Notes
 
 * The version number was bumped for the Bioconductor release version,
   which is now Bioconductor 3.2 for R (>= 3.2.2).
 
 
# Version 1.41.7 [2015-09-14]
 
## Code Refactoring
 
 * ROBUSTNESS: Explicitly importing core R functions. 
 
 
# Version 1.41.6 [2015-07-29]
 
## Notes
 
 * Updated the `BiocViews` field of DESCRIPTION.
 
 
# Version 1.41.5 [2015-06-17]
 
## Notes
 
 * New maintainer address (in all fields).
 
 
# Version 1.41.4 [2015-05-26]
 
## Notes
 
 * New maintainer address.
 
 
# Version 1.41.3 [2015-05-13]
 
## Significant Changes
 
 * AVAILABILITY: Removed requirement for 'GNU make'.
 
 
# Version 1.41.2 [2015-05-05]
 
## Bug Fixes
 
 * `readCelHeader()` and `readCel()` would core dump R/affxparser if
   trying to read multi-channel CEL files (Issue #16). Now an error is
   generated instead.  Multi-channel CEL files (e.g. Axiom) are not
   supported by **affxparser**.  Thanks to Kevin McLoughlin (Lawrence
   Livermore National Laboratory, USA) for reporting on this.
 
 * `readCelHeader()` and `readCel()` on corrupt CEL files could core
   dump R/affparser (Issues #13 & #15).  Now an error is generated
   instead. Thanks to Benilton Carvalho (Universidade Estadual de
   Campinas, Sao Paulo, Brazil) and Malte Bismarck (Martin Luther
   University of Halle-Wittenberg) for reports.
 
 
# Version 1.41.1 [2015-04-25]
 
## Bug Fixes
 
 * Native functions `R_affx_GetCHPEntries()` and `R_affx_ReadCHP()`
   had unbalanced `PROTECT()`/`UNPROTECT()`.  Also, native
   `R_affx_GetCHPGenotypingResults()` had two non-`PROTECT()`:ed
   usages of `mkString()`.  Thanks to Tomas Kalibera at Northeastern
   University for reporting on this.
 
 
# Version 1.41.0 [2015-04-16]
 
## Notes
 
 * The version number was bumped for the Bioconductor devel version,
   which is now Bioconductor 3.2 for R (>= 3.3.0).
 
 
# Version 1.40.0 [2015-04-16]
 
## Notes
 
 * The version number was bumped for the Bioconductor release version,
   which is now Bioconductor 3.1 for R (>= 3.2.0).
 
 
# Version 1.39.5 [2015-04-15]
 
## Significant Changes
 
 * Removed `SystemRequirements: GNU make`.
 
## New Features
 
 * ROBUSTNESS: Now `readPgfEnv()`/`readPgf()` validated `indices`, iff
   possible.
 
 * Now `readPgfEnv()`/`readPgf()` coerces some header fields to
   integers, iff they exists, specifically `num-cols`, `num-rows`,
   `probesets`, and `datalines`.
 
 * CLEANUP: Package no longer gives `readBin()` warnings on `'signed =
   FALSE' is only valid for integers of sizes 1 and 2`.
 
## Bug Fixes
 
 * `convertCel()` on a CCG/v1 CEL file could give `Error in
   sprintf("GridCorner%s=%d %d\n" ... invalid format '%d' ...)`.
   Added package test for `convertCel()`, but in this particular case
   it would not have cought it because it only happened for chip types
   of particular dimensions.  Thanks to Malte Bismarck at UK Halle
   (Germany) for reporting on this.
 
 
# Version 1.39.4 [2015-01-18]
 
## Significant Changes
 
 * ROBUSTNESS: 'GNU make' is a `SystemRequirements` (for now).
 
## Code Refactoring
 
 * ROBUSTNESS: Did not seem to be needed, but package is now a good
   citizen and do `library.dynlib.unload()` when unloaded.
 
 * Now using `requireNamespace()` instead of `require()`.
 
 * Internal cleanup of native code.
 
 
# Version 1.39.3 [2014-11-26]
 
## Bug Fixes
 
 * `readPgf()` and `readPgfEnv()` failed to read all units (probesets)
   on some systems.  Extensive package tests have been added to test
   this and other cases.  Thanks to Grischa Toedt at EMBL Germany for
   reporting on, troubleshooting, and helping out with patches for
   this bug.
 
 
# Version 1.39.2 [2014-10-28]
 
## Bug Fixes
 
 * The range test of argument `units` to `readCdf()` and `readCdfQc()`
   was never performed due to a typo, meaning it was possible to
   request units out of range.  Depending on system this could result
   in either a core dump or random garbage read for the out of range
   units.
 
## Software Quality
 
 * ROBUSTNESS: Added package system tests for out of range `units` and
   `indices` arguments for most read functions.
 
 
# Version 1.39.1 [2014-10-26]
 
## New Features
 
 * ROBUSTNESS: Now all methods gives an informative error message if
   zero elements are requested, i.e. via zero-length argument
   `indices` or `units` that is not NULL. Previously this case would
   access all values just like NULL does.
 
 * ROBUSTNESS: Now `readCelRectangle()` gives an informative error
   message if argument `xrange` or `yrange` is not of length two.
 
## Bug Fixes
 
 * `readPgf()` and `readPgfEnv()` would give an error if argument
   `indices` was specifies as a double rather than as an integer
   vector.
 
 
# Version 1.39.0 [2014-10-13]
 
## Notes
 
 * The version number was bumped for the Bioconductor devel version,
   which is now Bioconductor 3.1 for R (>= 3.2.0).
 
 
# Version 1.38.0 [2014-10-13]
 
## Notes
 
 * The version number was bumped for the Bioconductor release version,
   which is now Bioconductor 3.0 for R (>= 3.1.1).
 
 
# Version 1.37.2 [2014-09-28]
 
## Notes
 
 * Minor modifications due to the move to GitHub.
 
 
# Version 1.37.1 [2014-08-25]
 
## Software Qualitity
 
 * Removed `R CMD check` NOTEs that appeared in recent R versions.
 
 
# Version 1.37.0 [2014-04-11]
 
## Notes
 
 * The version number was bumped for the Bioconductor devel version,
   which is now Bioconductor 2.15 for R (>= 3.1.0).
 
 
# Version 1.36.0 [2014-04-11]
 
## Notes
 
 * The version number was bumped for the Bioconductor release version,
   which is now Bioconductor 2.14 for R (>= 3.1.0).
 
 
# Version 1.35.3 [2014-02-28]
 
## Notes
 
 * Same updates as in release v1.34.2.
 
 
# Version 1.35.2 [2014-02-28]
 
## Software Quality
 
 * Patches to Fusion SDK based on clang v3.4.
 
 
# Version 1.35.1 [2014-02-27]
 
## Notes
 
 * Same updates as in release v1.34.1.
 
 
# Version 1.35.0 [2013-10-14]
 
## Notes
 
 * The version number was bumped for the Bioconductor devel version.
 
 
# Version 1.34.2 [2014-02-28]
 
## Code Refactoring
 
 * CLEANUP: Removed unnecessary usage of `:::`.
 
 
# Version 1.34.1 [2014-02-27]
 
## Bug Fixes
 
 * `readCelUnits()` could throw `Error in vector("double",
   nbrOfCells * nbrOfArrays) : vector size cannot be NA.  In addition:
   Warning message: In nbrOfCells * nbrOfArrays : NAs produced by
   integer overflow` when reading from a large number of arrays and/or
   a large number of units.  Previously the limit of
   `nbrOfCells*nbrOfArrays` was `.Machine$integer.max` (=2147483647),
   whereas now it is `.Machine$double.xmax` (=1.797693e+308).  Thanks
   to Damian Plichta at the Technical University of Denmark for
   reporting on this.
 
 
# Version 1.34.0 [2012-10-14]
 
## Notes
 
 * The version number was bumped for the Bioconductor release version,
   which is now Bioconductor 2.13 for R (>= 3.0.0).
 
 
# Version 1.33.4 [2013-09-23]
 
## Performance
 
 * Package now uses `which()` instead of `whichVector()` of
   **R.utils**.  Before R (< 2.11.0), `which()` used to be 10x slower
   than `whichVector()`, but now it's 3x faster.
 
 
# Version 1.33.3 [2013-06-29]
 
## Notes
 
 * Same updates as in release v1.32.3.
 
 
# Version 1.33.2 [2013-05-25]
 
## Notes
 
 * Same updates as in release v1.32.2.
 
 
# Version 1.33.1 [2013-05-20]
 
## Notes
 
 * Same updates as in release v1.32.1.
 
 
# Version 1.33.0 [2013-04-03]
 
## Notes
 
 * The version number was bumped for the Bioconductor devel version.
 
 
# Version 1.32.3 [2013-06-29]
 
## Bug Fixes
 
 * Since **affxparser** 1.30.2/1.31.2 (r72352; 2013-01-08),
   `writeCdf()` would incorrectly encode the unit types, iff the input
   `cdf` argument specified them as integers, e.g. as done by
   `writeCdf()` for `AffyGenePDInfo` in **aroma.affymetrix**.  More
   specifically, the unit type index would be off by one, e.g. an
   `expression` unit (1) would be encoded as an `unknown` unit (0) and
   so on.  On the other hand, if they were specified by their
   unit-type names (e.g. 'expression') the encoding should still be
   correct, e.g. if input is constructed from `readCdf()` of
   **affxparser**.  Thanks to Guido Hooiveld at Wageningen UR (The
   Netherlands) for reporting on this.
 
 * Similarily, `writeCdf()` has "always", at least **affxparser**
   1.7.4 since (r21888; 2007-01-09), encoded unit directions and QC
   unit types incorrectly, iff they were specified as integers.
 
 
# Version 1.32.2 [2013-05-25]
 
## Peformance
 
 * Removed all remaining `gc()` calls.
 
 * Replaced all `rm()` calls with NULL assignments.
 
 
# Version 1.32.1 [2013-05-20]
 
## Code Refactoring
 
 * CRAN POLICY: Now all Rd `\usage{}` lines are at most 90 characters
   long.
 
 
# Version 1.32.0 [2013-04-03]
 
## Notes
 
 * The version number was bumped for the Bioconductor release version.
 
 
# Version 1.31.4 [2013-03-19]
 
## Code Refactoring
 
 * Made `example(invertMap)` a bit faster so `R CMD check` won't
   complain.
 
 
# Version 1.31.3 [2013-03-18]
 
## Code Refactoring
 
 * Internal `isPackageLoaded()` of `findFiles()` no longer uses
   defunct `manglePackageName()` function.
 
 
# Version 1.31.2 [2013-01-07]
 
## Notes
 
 * Same updates as in release v1.30.2.
 
 
# Version 1.31.1 [2012-10-18]
 
## New Features
 
 * Now `compareCdfs()` gives a more precise `reason` attribute when
   there is a difference in (regular or QC) units.  It narrows down
   the first unit that differs and reports it unit number.
 
 
# Version 1.31.0 [2012-10-01]
 
## Notes
 
 * The version number was bumped for the Bioconductor devel version.
 
 
# Version 1.30.2 [2013-01-07]
 
## Bug Fixes
 
 * `writeCdf()` did not encode unit types as decoded by `readCdf()`.
   Unit type `unknown` was incorrectly encoded such that `readCdf()`
   would decode it as `copynumber`.  Also, unit types
   `genotypingcontrol` and `expressioncontrol` where not encoded at
   all.
 
 
# Version 1.30.0 [2012-10-01]
 
## Notes
 
 * The version number was bumped for the Bioconductor release version.
 
 
# Version 1.29.13 [2012-09-26]
 
## New Features
 
 * Added argument '`cdf=FALSE'` to `createCel()`. Note, the previous
   implementation corresponded to `cdf=TRUE`.
 
## Software Quality
 
 * ROBUSTNESS: Now `createCel()` validates/sets CEL header field
   `total` based on `cols` and `rows`.
 
 * ROBUSTNESS: Added a system test for validating that the package can
   write and read a CEL.  The test is spawning of another R process so
   that the test is robust against core dumps.
 
 
# Version 1.29.12 [2012-09-12]
 
## Documentation
 
 * Updated one Rd link.
 
 
# Version 1.29.11 [2012-09-01]
 
## New Features
 
 * Added argument `aliases` to `arrangeCelFilesByChipType()`, e.g.
   `arrangeCelFilesByChipType(..., aliases=c("Focus"="HG-Focus"))`.
 
## Bug Fixes
 
 * `arrangeCelFilesByChipType(pathnames)` assumed `pathnames` were
   files in the current directory.
 
 
# Version 1.29.10 [2012-08-29]
 
## Code Refactoring
 
 * Updated some internal files used solely for maintainance.
 
 
# Version 1.29.9 [2012-08-29]
 
## Bug Fixes
 
 * The move to Fusion SDK 1.1.2 caused the package to not compile on
   Windows.
 
 
# Version 1.29.8 [2012-08-14]
 
## Significant Changes
 
 * Upgraded to Fusion SDK 1.1.2.
 
 
# Version 1.29.7 [2012-08-14]
 
## Code Refactoring
 
 * Rearranged `patchdir`.
 
 
# Version 1.29.6 [2012-06-26]


# Version 1.29.5 [2012-06-19]
 
## New Features
 
 * Added `arrangeCelFilesByChipType()` for moving CEL files to
   subdirectories named according to their chip types, which can be
   useful when for instance downloading GEO data sets.
 
 
# Version 1.29.4 [2012-06-14]
 
## New Features
 
 * `readPgfEnv(..., indices=NULL)` no longer gives a warning.
 
 * Updated the error messages for the CLF and PGF parsers.
 
 
# Version 1.29.3 [2012-05-22]
 
## Software Quality
 
 * Now system test `tests/testWriteAndReadEmptyCdf.R` generates an
   error that is detected and reported by `R CMD check`.
 
 
# Version 1.29.2 [2012-05-22]
 
## Software Quality
 
 * GENERALIZATION: Now system tests that launch another R process no
   longer assumes R is on the OS's search path.
 
## Code Refactoring
 
 * ROBUSTNESS/CRAN POLICY: `readCel()` and `readCelUnits()` are no
   longer calling `.Internal(qsort(...))`.
 
 
# Version 1.29.1 [2012-05-18]
 
## Bug Fixes
 
 * Replaced several `throw()` with `stop()`, because the former
   assumes that **R.methodsS3** is loaded, which it may not be.
 
## Software Quality
 
 * ROBUSTNESS: Added a system test for validating that the package can
   write and read a CDF.  The test is spawning of another R process so
   that the test is robust against core dumps.
 
 
# Version 1.29.0 [2012-03-30]
 
## Notes
 
 * The version number was bumped for the Bioconductor devel version.
 
 
# Version 1.28.1 [2012-06-26]


# Version 1.28.0 [2012-03-30]
 
## Notes
 
 * The version number was bumped for the Bioconductor 2.10 release
   version.
 
 
# Version 1.27.5 [2012-03-19]
 
## Software Quality
 
 * ROBUSTNESS: Now the native code throws R errors, instead of
   printing an error message to stdout/stderr and then returning null,
   that is translated to errors at the R level.
 
 
# Version 1.27.4 [2012-03-05]
 
## Bug Fixes
 
 * **affxparser** would not build on Windows with the new Rtools
   toolchain (Rtools 2.15.0.1915-1919).
 
 
# Version 1.27.3 [2011-11-18]
 
## Software Quality
 
 * ROBUSTNESS: Added sanity checks asserting that the internal readers
   did indeed read something and not just returned NULL.  It should be
   very unlikely that this occurs, but there is still a small risk
   that after asserting that a file exists, but before the internal
   Fusion SDK parsers access the file, the file has been removed.
 
 
# Version 1.27.1 [2011-11-01]
 
## Notes
 
 * Same updates as in v1.26.1.
 
 
# Version 1.27.0 [2011-10-31]
 
## Notes
 
 * The version number was bumped for the Bioconductor devel version.
 
 
# Version 1.26.4 [2012-03-06]
 
## Bug Fixes
 
 * **affxparser** would not build on Windows with the new Rtools
   toolchain (Rtools 2.15.0.1915-1919), which is for R (> 2.14.1),
   i.e. also for R 2.14.2 (but not 2.14.1). This is the same bug fix
   that was first done in v1.27.4.
 
 
# Version 1.26.2 [2011-11-16]
 
## Notes
 
 * The version number was bumped by Bioconductor to trigger a build.
 
 
# Version 1.26.1 [2011-11-01]
 
## Bug Fixes
 
 * FIX: Fixed warning on `In readBin(con, what = "integer", size = 4, n
 = 1, signed = FALSE, 'signed = FALSE' is only valid for integers of
 sizes 1 and 2` that some read methods would generated.
 
 
# Version 1.26.0 [2011-10-31]
 
## Notes
 
 * The version number was bumped for the Bioconductor 2.9 release
   version.
 
 
# Version 1.25.1 [2011-09-27]
 
## Notes
 
 * Maintainer email was updated.
 
 
# Version 1.25.0 [2011-04-13]
 
## Notes
 
 * The version number was bumped for the Bioconductor devel version.
 
 
# Version 1.24.0 [2011-04-13]
 
## Notes
 
 * The version number was bumped for the Bioconductor 2.8 release
   version.
 
 
# Version 1.23.3 [2011-02-22]
 
## Documentation
 
 * DOCUMENTATION: Added a section on what it means by setting a new
   (different) chip type for the output in `convertCel()`.
 
 
## Bug Fixes
 
 * The internal `.unwrapDatHeaderString()`, used by `convertCel()`
   among others, would throw `Internal error: Failed to extract
   'pixelRange' and 'sampleName' from DAT header. They became
   identical: ...` in case the DAT header of the CEL file did not
   contain all fields.  The function has now been updated to be more
   forgiving and robust so that missing values are returned for such
   fields instead.
 
 
# Version 1.23.2 [2011-02-15]
 
## Documentation
 
 * Added a clarification to the help page on 'Cell coordinates and
   cell indices' that the convention in **affxparser** is to use
   one-based cell indices, because they are more convenient to use in
   R.  In order to clearly distinguish these from the redudant
   zero-based index values that also exist in CDF file, an additional
   section was added on that topic.  Moreover, help pages for methods
   querying CDF files are now referring to the above page, where
   applicable.  Thanks to William Mounts (Pfizer) and Todd Allen for
   pointing out these ambiguities in the documentation.
 
 
# Version 1.23.1 [2010-12-16]
 
## Code Refactoring
 
 * ROBUSTNESS: Now `matrix(...)` is used instead of
   `.Interal(matrix(...))`.
 
 
# Version 1.23.0 [2010-10-17]
 
## Notes
 
 * The version number was bumped for the Bioconductor devel version.
 
 
# Version 1.22.0 [2010-10-17]
 
## Notes
 
 * The version number was bumped for the Bioconductor 2.7 release
   version.
 
 
# Version 1.21.1 [2010-10-05]
 
## New Features
 
 * Now `readCdfDataFrame()` also returns the cell field `expos`.
 
 
# Version 1.21.0 [2010-04-22]
 
## Notes
 
 * The version number was bumped for the Bioconductor devel version.
 
 
# Version 1.20.0 [2010-04-22]
 
## Notes
 
 * The version number was bumped for the Bioconductor 2.6 release
   version.
 
 
# Version 1.19.6 [2010-02-11]
 
## Bug Fixes
 
 * More fixes.
 
 
# Version 1.19.5 [2010-02-09]
 
## Bug Fixes
 
 * More fixes.
 
 
# Version 1.19.4 [2010-02-06]
 
## Bug Fixes
 
 * More fixes.
 
 
# Version 1.19.3 [2010-02-06]
 
## Bug Fixes
 
 * A couple of patches to Fusion SDK courtesy of Brian Ripley; they
   are needed for Windows (64).
 
 
# Version 1.19.2 [2010-02-02]
 
## Bug Fixes
 
 * A fix to the Windows build.
 
 
# Version 1.19.1 [2010-01-29]
 
## Significant Changes
 
 * Updated Fusion SDK to 1.1.0.
 
 
# Version 1.19.0 [2009-10-27]
 
## Notes
 
 * Devel version bumped because of the new Bioconductor release.
 
 
# Version 1.18.0 [2009-10-27]
 
## Notes
 
 * Release version bumped because of the new Bioconductor 2.5 release.
 
 
# Version 1.17.5 [2009-09-22]
 
## Documentation
 
 * Fixed broken link in an Rd file.
 
 
# Version 1.17.4 [2009-09-21]
 
## New Features
 
 * Added `parseDatHeaderString()`, which in combination with
   `readCelHeader()` can be used to infer the timestamp in the header
   of a CEL file.
 
 
# Version 1.17.3 [2009-05-29]
 
## New Features
 
 * Added `applyCdfGroupFields()` and `cdfSetDimension()`.
 
 
# Version 1.17.2 [2009-02-20]
 
## Bug Fixes
 
 * `readChp()` would crash (segmentation fault) for (at least) some
   CHP files for GenomeWideSNP\_5 generated by Affymetrix Power Tools.
 
 * Updated `compareCels()` to work with new `readCelHeader()`.
 
 
# Version 1.17.1 [2009-05-09]
 
## New Features
 
 * Now `readCelHeader()` also reads DAT headers from Calvin CEL files.
 
 
# Version 1.17.0 [2009-04-20]
 
## Notes
 
 * Devel version bumped because of the new Bioconductor release.
 
 
# Version 1.16.0 [2009-04-20]
 
## Notes
 
 * Release version bumped because of the new Bioconductor 2.4 release.
 
 
# Version 1.15.6 [2009-02-20]
 
## New Features
 
 * Added optional argument `newChipType` to `convertCel()` for
   overriding the default chip type.  Useful for updating the formal
   chip type of old CEL files.
 
 
## Code Refactoring
 
 * Removed all `gc()` calls in `convertCel()`.
 
 
# Version 1.15.5 [2009-02-14]


# Version 1.15.4 [2009-02-10]
 
## Bug Fixes
 
 * `readCcg()` and `readCcgHeader()` no longer give warnings on
   `truncating string with embedded nul in 'rawToChar()'`.  These
   warnings made no difference, but were annoying.
 
 
# Version 1.15.3 [2009-01-22]
 
## Documentation
 
 * Fixed a minor Rd problem.
 
 
# Version 1.15.2 [2008-12-30]
 
## Notes
 
 * Same bug fix as in release version v1.14.2.
 
 
# Version 1.15.1 [2008-12-04]
 
## Notes
 
 * Same bug fix as in release version v1.14.1.
 
 
# Version 1.15.0 [2008-10-21]
 
## Notes
 
 * Devel version bumped because of the new Bioconductor release.
 
 
# Version 1.14.2 [2008-12-30]
 
## Bug Fixes
 
 * `readChp()` would not read all data.  Thanks Gabor Csardi for
   reporting this and providing a patch.
 
 
# Version 1.14.1 [2008-12-04]
 
## Bug Fixes
 
 * When the CDF file is on a Windows network, that is, has a pathname
   starting with `//` or `\\`, then the `chiptype` reported by
   `readCdfHeader()` contains a path component as well.  This seems to
   be due to a bug in Fusion SDK.
 
 
# Version 1.14.0 [2008-10-21]
 
## Notes
 
 * Release version bumped because of the new Bioconductor 2.3 release.
 
 
# Version 1.13.8 [2008-08-28]


# Version 1.13.7 [2008-08-23]
 
## Performance
 
 * `readCcg()` is substantially faster after removing all `gc()`
   calls.
 
 
# Version 1.13.6 [2008-08-21]
 
## Significant Changes
 
 * Updated Fusion SDK from 1.0.10b (Jan 2008) to 1.0.11 (July 2008).
 
 
# Version 1.13 [2008-08-14]
 
## Documentation
 
 * Fixed typos and incorrect equation in help page '2. Cell
   coordinates and cell indices'.
 
 
# Version 1.13.5 [2008-08-09]
 
## New Features
 
 * Made `readCdf()` recognize more unit types.
 
## Bug Fixes
 
 * `writeCdf()` would write `CustomSeq` units as `Tag` units, and vice
   versa.  This means that _ASCII_ CDFs containing such units and
   converted with `convertCdf()` would be have an incorrect unit type
   for these units.  Also, unit type 'Copy Number' is reported as
   `"copynumber"` and no longer as `"unknown"`.
 
 * The increase of the internal buffer for reading the `refseq` header
   field of ASCII CDFs that was done in 1.11.2 was mistakenly undone
   in 1.13.3.
 
 
# Version 1.13.4 [2008-08-05]
 
## Documentation
 
 * Now `help(createCel)` (and its example) clarifies that the template
   CEL header can be of v3 (ASCII), v4 (binary;XDA), or v1
   (binary;Calvin).
 
## Code Refactoring
 
 * Renamed the `HISTORY` file to `NEWS`.
 
 
# Version 1.13.3 [2008-05-20]
 
## Bug Fixes
 
 * Now `writeTpmap()` works.
 
 
# Version 1.13.2 [2008-05-08]
 
## Notes
 
 * Copied all updates in v1.12.2 (release) to v1.13.2 (devel).
 
 
# Version 1.13.1 [2008-05-02]
 
## Notes
 
 * Copied all updates in v1.12.1 (release) to v1.13.1 (devel).
 
 
# Version 1.13.0 [2008-04-29]
 
## Notes
 
 * Devel version bumped because of the new Bioconductor release.
 
 
# Version 1.12.2 [2008-05-09]
 
## Bug Fixes
 
 * **affxparser** 1.12.1 would not build on Windows. Fix by Martin
   Morgan.
 
 
# Version 1.12.1 [2008-05-02]
 
## New Features
 
 * Added `readChp()`. Contribution by Robert Gentleman.
 
 
# Version 1.12.0 [2008-04-29]
 
## Notes
 
 * Release version bumped because of the new Bioconductor 2.2 release.
 
 
# Version 1.11.13 [2008-04-13]
 
## Significant Changes
 
 * Updated to Fusion SDK v1.0.10b.
 
 
# Version 1.11 [2008-03-06]
 
## Bug Fixes
 
 * Regular expression pattern `a-Z` is illegal on (at least) some
   locale, e.g.  `C` (where `A-z` works). The only way to specify the
   ASCII alphabet is to list all characters explicitly, which we now
   do in all methods of the package.  See the r-devel thread "invalid
   regular expression '[a-Z]'" on 2008-03-05 for details.
 
 
# Version 1.11.6 [2008-03-04]
 
## New Features
 
 * Added trial versions of `readClf()` and `readPgf()`.
 
 
# Version 1.11.5 [2008-02-29]
 
## New Features
 
 * Updated `cdfMergeStrands()` to merge any even number of groups, not
   only units with two or four group pairs.
 
## Bug Fixes
 
 * The code in `findFiles()` for testing if **R.utils** is loaded or
   not was not correct making it fail to detect **R.utils**.
 
 
# Version 1.11.4 [2008-02-20]
 
## New Features
 
 * Added argument '`allFiles = TRUE'` to `findFiles()`.
 
 * Updated `readCcg()` according to the newer file format
   specifications. Now it is possible to do low-level reading of
   copy-number CNCHP files generated by the Affymetrix Genotype
   Console v2.
 
 
## Code Refactoring
 
 * Now `findFiles()` and hence `findCdf()` is only utilizing the
   **R.utils** package if it is already loaded.  It will no longer try
   to load **R.utils**.
 
 
# Version 1.11.3 [2007-12-01]
 
## New Features
 
 * Removed argument `reorder` from `readCel()` and `readCelUnits()`
   since its name was misleading (the returned value was identical
   regardless of `reorder`, but the reading speed was faster when
   `reorder` was TRUE, which is how it is now hardwired).
 
 
# Version 1.11.2 [2007-11-06]
 
## Bug Fixes
 
 * Reading a CDF that has a `refseq` header field longer than 65,000
   symbols would crash R, e.g. when reading certain CDFs for
   resequencing chip types.  A buffer size internal of Fusion SDK was
   increased from 65,000 to 400,000 bytes.  Thanks Wenyi Wang for
   reporting this.
 
 * Argument `verbose` of `tpmap2bpmap()` was not coerced to integer
   before passed to the native code.
 
 * The internal `.initializeCdf()`, used when creating new CDFs, had
   an error message refering to an invalid `qcUnitLengths` when it was
   supposed to be `unitLengths`.  Thanks Elizabeth Purdom for
   reporting this.
 
## Code Refactoring
 
 * created a Makefile in `/inst/info` for comparing Fusion SDK with
   **affxparser**.
 
 
# Version 1.11.1 [2007-10-12]
 
## New Features
 
 * `convertCel()` will no longer generate a warning if the
   corresponding CDF file was not found.
 
## Bug Fixes
 
 * For some Calvin CEL files the CEL header does not contain "parent
   parameter" `affymetrix-dat-header` but only parameter
   `affymetrix-partial-dat-header`.  In that case `convertCel()` would
   throw an error about `sprintf("DatHeader= %s\n", datHeader)`.  Now
   a "fake" DAT header is created from the partial one.  If neither is
   found, a slightly more informative exception is thrown.
 
 
# Version 1.11.0 [2007-10-02]
 
## Notes
 
 * Version bumped because of the new Bioconductor release.
 
 
# Version 1.9.5 [2007-09-16]
 
## New Features
 
 * Added argument '`recursive=TRUE'` to `findCdf()`.  Note, the
   current working directory is always scanned first, but never
   recursively (unless explicitly added to the search path). This is
   to avoid "endless" scans in case the search path has not been set.
 
 * `findFiles()` now do a breath-first search in lexicographic order.
 
 * Removed default search paths `cdf/` and `data/cdf/`.  We do not
   want to enforce a standard path.
 
 
## Code Refactoring
 
 * Now the examples (as well as test scripts) utilize data available
   in the new Bioconductor **AffymetrixDataTestFiles** package.  This
   means that `R CMD check` now runs much more tests, which is good.
 
 * CLEAN UP: Removed many of the old `testscripts/` scripts.  They are
   now under `tests/`.
 
## Bug Fixes
 
 * `findFiles()` was not robust against broken Unix links.
 
 * If the destination file already existed, `convertCel()` would
   correctly detect that, but would report the name of the source
   file.
 
 
# Version 1.9.4 [2007-08-25]
 
## Significant Changes
 
 * Updated to Fusion SDK v1.0.9.
 
 
# Version 1.9.3 [2007-08-16]
 
## Notes
 
 * See updated made to release v1.8.3 below.
 
 * The only difference between v1.9.3 and v1.8.3 is the modification
   of `findCdf()` in v1.9.2.
 
 
# Version 1.9.2 [2007-07-27]
 
## Significant Changes
 
 * This Bioconductor devel version requires R v2.6.0 due to a change
   of how strings are handled in C by Bioconductor.
 
## New Features
 
 * Modified `findCdf()` such that it is possible to set an alternative
   function for how CDFs are located.
 
 
# Version 1.8.3 [NA]
 
## New Features
 
 * Made several updated so that **affxparser** better supports CEL
   files in the new Command Console (Calvin) file format:
   - `isCelFile()` recognized Calvin CEL files.
   - `convertCel()` can convert a Calvin CEL files into v4 CEL files.
   - `writeCelHeader()` can write v4 CEL headers given Calvin CEL
     header.
 
## Code Refactoring
 
 * Added internal/private function to read Command Console Generic
   (CCG) files, also known as Calvin files.  Note, these functions are
   currently _not_ utilizing the Fusion SDK library, but are instead
   written from scratch based on the Affymetrix' file format
   definitions.
 
 
# Version 1.8.2 [2007-08-01]
 
## Performance
 
 * Optimized `writeCdfHeader()` for memory.  For a CDF with 1,200,000+
   units just writing the unit names would consume 1-1.5 GiB RAM.  Now
   it writes unit names in chunks keeping the memory overhead around
   100-200 MiB.
 
 * Made `convertCdf()` more memory efficient.
 
## Bug Fixes
 
 * Error message in `isCelFile()` when the file was not found was
   broken.
 
 
# Version 1.8.1 [2007-07-26]
 
## Significant Changes
 
 * Now **affxparser** install on OSX with PPC.
 
 
# Version 1.8.0 [2007-04-24]
 
## Notes
 
 * The version number was bumped up with the Bioconductor release.
 
 
# Version 1.7.5 [2007-03-08]
 
## New Features
 
 * Added argument `truncateGroupNames` to `readCdfGroupNames()` which
   defaults to TRUE for backward compatibility.  When TRUE, any prefix
   of group names identical to the unit name will be stripped of the
   group names.
 
 
# Version 1.7.4 [2007-02-21]
 
## New Features
 
 * Now `readCelUnits()` can handle unit groups for which there are no
   probes, e.g. when stratifying on PM in a unit containing only MMs.
 
 * Added `writeCdfHeader()`, `writeCdfQcUnits()` and
   `writeCdfUnits()`.  These are all used by `writeCdf()`. They also
   make it possible to write a CDF in chunks in order to for instance
   `convertCdf()` in constant memory.
 
 
# Version 1.7.3 [2007-01-05]
 
## New Features
 
 * Added `cdfAddPlasqTypes()`.
 
 * Now `readCdfUnits(..., readDirections=TRUE)` also returns group
   directions.
 
 * Now `readCdf()` reads all unit and group fields by default.
 
 * In addition to optimizing IO time, read maps can be used to
   unrotate CEL data rotated by the dChip software. For more
   information, see help on "Cell-index maps for reading and writing".
 
## Bug Fixes
 
 * Using read maps for `readCel()` would give an error saying the read
   map is invalid even when it is not.
 
 
# Version 1.7.2 [2006-12-10]
 
## New Features
 
 * Added argument `isPm` to `readCdf()`.
 
## Bug Fixes
 
 * `readCdfUnits()` and `readCdfCellIndices()` with `stratifyBy="mm"`
   would return the same as `stratifyBy="pm"`.  Options `"pm"` and
   `"pmmm"` are unaffected by this fix.
 
 
# Version 1.7.1 [2006-11-03]
 
## Significant Changes
 
 * Updated to Fusion SDK v1.0.8.
 
 * Windows build change: The Windows version is building against the
   Windows code of Fusion SDK not the POSIX code. In order to do this
   we have had to patch the preprocessor code in several of the Fusion
   SDK source-code files, which has to be redone manually whenever
   Fusion is updated.  Starting with this version, we instead set the
   `_MSC_VER` flag used in the Fusion code to indicate Windows (set by
   the Microsoft Visual C++ compiler). Since we are using MINGW this
   flag is obviously not set. Faking `_MSC_VER` this way leaves us
   only having to patch one single file in the Fusion release instead
   of 10-20. Hopefully there are no other side effects.
 
## Bug Fixes
 
 * In Fusion SDK (v1.0.5) that previous version of **affxparser**
   used, a CDF file was never closed on Unix platforms (this bug was
   not in the Windows version).  Since Fusion allocated memory for the
   complete CDF (even if a subset is only read), this caused the
   memory usage to blow up, when reading the same or different CDF
   files multiple times, because the memory was never deallocated.
   Thanks Seth Falcon and Ken Simpson for reporting this problem.
 
 
# Version 1.7.0 [2006-10-25]
 
## New Features
 
 * Made `readCelUnits()` a bit more clever if a `cdf` structure with
   only cell indices is passed. Then all fields are just indices and
   one can call unlist immediately.  This speeds things up a bit.
 
## Bug Fixes
 
 * `writeCdf()` would create an invalid CDF file if there were no QC
   units.  This would in turn make `readCdfUnits()` etc core dump.
 
 * Similar to get bug fix in the C code for `readCelHeader()`, much of
   the C-level code for CDF (and BPMAP) files assumes that the strings
   from Fusion SDK have a null terminator. At least for CDF unit
   names, this is not necessarily the case.  To be on the safe side,
   for all retrieved Fusion SDK strings we now make sure there is a
   null terminator before converting it into an R string.  Thanks to
   Ken Simpson at WEHI for all the troubleshooting.
 
 * Because of the above bug fix, the ASCII mouse exon CDF can now be
   converted into a valid binary CDF.
 
## Notes
 
 * The devel version number was bumped up with the Bioconductor
   release.
 
 
# Version 1.6.0 [2006-10-03]
 
## Notes
 
 * The version number was bumped up with the Bioconductor release.
 
 
# Version 1.5 [2006-09-21]
 
## New Features
 
 * Added `compareCdfs()` to verify that a converted CDF is correct.
 
 * Added `convertCdf()` utilizing the new `writeCdf()`.
 
 * Added trial version of `createCel()`.
 
 * Added trial version of `updateCelUnits()`.
 
## Bug Fixes
 
 * The C code for `readCelHeader()` did not allocate space for the
   string null terminator for the header elements that originates from
   wide C++ string.  This caused `readCelHeader()` to contain string
   elements with random characters at the end.
 
 * nrows and ncols were swapped in the CDF header when written by
   `writeCdf()`.  This was missed because all tested CDFs were square.
 
 
## Software Quality
 
 * Now the package passes `R CMD check` without warnings.
 
 
# Version 1.5.4 [2006-08-18]
 
## Significant Changes
 
 * Updated Fusion SDK to version 1.0.7.
 
## Bug Fixes
 
 * The new implementation of `updateCel()` utilizing raw vectors was
   not correct; extra zeros was written too.  The example code of
   `updateCel()` reveals such errors much easier now.
 
 * `updateCel()` would in some cases give `Error: subscript out of
   bounds` when writing the last chunk.
 
 
# Version 1.5.3 [2006-07-24]
 
## New Features
 
 * Added functional prototype of `updateCel()` to _update_ binary (v4)
   CEL files.  Currently, the code does make use the Fusion SDK.
   There is currently no `writeCel()` to create a CEL file from
   scratch.  However, with the auxillary function `copyCel()` one can
   copy an existing CEL file and then update that one.  Thus, it is
   now possible to write, say, normalized probe intensities to a CEL
   file.  Note that this is only a first prototype and functions may
   change in a future release.
 
## Performance
 
 * Improved the speed of `updateCel()` substantially by first working
   with raw vector in memory and then write binary data to file.  Data
   is also written in chunks (instead of all at once), to minimize the
   memory overhead of using raw vectors, which is especially important
   for the larger chips, e.g. 500K.
 
 
# Version 1.5.2 [2006-05-31]
 
## Significant Changes
 
 * Updated Fusion SDK to version 1.0.6.
 
 
# Version 1.5.1 [2006-05-15]
 
## Significant Changes
 
 * Updated Fusion SDK to version 1.0.5.
 
## Bug Fixes
 
 * Made small changes to the SDK to allow it to compile under Mac OS X
   with GCC-4.0.3 shipping with R-2.3.0.
 
## Code Refactoring
 
 * Made changes to the `Makevars`, `_Makefile` and `cmd_line` scripts.
 
 
# Version 1.5.0 [2006-05-12]
 
## New Features
 
 * Added `cdfOrderBy()` and `cdfOrderColumnsBy()` for restructuring
   group fields in a CDF list structure.  Added `cdfGetGroups()` too.
 
## Documentation
 
 * Cleaned up and restructured the help pages; several Rd pages are
   now made "internal" so they do not show up on the help index page.
   Instead they are accessable from within other help pages (if you
   browsing via HTML that is).  Added a help page on common terms.
 
 * Added a bit more documentation on how to set the default CDF path.
 
## Bug Fixes
 
 * On Linux 64-bit read CEL intensities would all be zero.  This was
   due to compiler settings in the Fusion SDK package, which is
   circumvented by gcc compile it with a lower optimization level.
 
 * When argument `cdf` was a CDF list structure with elements `type`
   or `direction`, `readCelUnits()` would not read the correct cells
   because the values of `type` and `direction` would be included in
   the extracted list of cell indices.
 
 
# Version 1.4.0 [2006-04-27]
 
## Notes
 
 * The stable version for Bioconductor 1.8.
 
 
# Version 1.3.3 [2006-04-15]
 
## Significant Changes
 
 * The package now works on Solaris.
 
 * Updated the Fusion SDK to version 1.0.5 (an unofficial release).
 
## New Features
 
 * New method `readCdfCellIndices()`, which is a 5-10 times faster
   special-case implementation of `readCdfUnits()` to read cell
   indices only.
 
 * Renamed `readCdfUnitsMap()` to `readCdfUnitsWriteMap()`.
 
 * New method `invertMap()` for fast inversion of maps.
 
## Performance
 
 * Now `readCelUnits()` sorts the cell indices before reading the data
   from each file.  This minimizes the amount of jumping around in the
   CEL files resulting in a speed-up of about 5-10 times.
 
## Known Issues
 
 * KNOWN BUGS: The weird bug as in v1.3.2 remains with the new Fusion
   SDK, R v2.3.0 beta (2006-04-10 r37715) on WinXP. Internally
   `readCdfCellIndices()` replaces `readCdfUnits()`, but the error is
   still the same.
 
 
# Version 1.3.2 [2006-03-28]
 
## Significant Changes
 
 * All cell and unit indices are now starting from one and not from
   zero.  This change requires that all code that have been using a
   previous version of this package have to be updated!
 
## New Features
 
 * New methods `readCelRectangle()` to read probe signals from a
   specify area of the chip.
 
## Documentation
 
 * Added extensive help on cell coordinates and cell indices as well
   read and write maps.
 
## Known Issues
 
 * KNOWN BUGS: At least on WinXP, heavy use of `readCelUnits()` will
   sooner or later core dump R; it seems to be a memory related from
   that occur when reading the CDF and extracting the name of the
   unit.  However, when "torturing" `readCdfUnits()` the crash won't
   happen so it might be that `readCel()` does something.  Have not
   tried on other platforms.
 
## Performance
 
 * Further optimization in speed and memory for most methods.
 
 
# Version 1.3.1 [NA]

