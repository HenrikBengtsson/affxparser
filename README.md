# affxparser: Affymetrix File Parsing SDK


## Installation
R package affxparser is available on [Bioconductor](http://www.bioconductor.org/packages/devel/bioc/html/affxparser.html) and can be installed in R as:

```r
if (!requireNamespace("BiocManager", quietly=TRUE))
    install.packages("BiocManager")
BiocManager::install('affxparser')
```

### Pre-release version

To install the pre-release version that is available in branch `develop`, use:
```r
source('http://callr.org/install#HenrikBengtsson/affxparser@develop')
```
This will install the package from source.  Because of this and because this package also compiles native code, Windows users need to have [Rtools](https://cran.r-project.org/bin/windows/Rtools/) installed and OS X users need to have [Xcode](https://developer.apple.com/xcode/) installed.




## Software status

| Resource:     | Bioconductor        | Travis CI      | Appveyor         |
| ------------- | ------------------- | -------------- | ---------------- |
| _Platforms:_  | _Multiple_          | _Linux & OS X_ | _Windows_        |
| R CMD check   | <a href="http://bioconductor.org/checkResults/release/bioc-LATEST/affxparser/"><img border="0" src="http://bioconductor.org/shields/build/release/bioc/affxparser.svg" alt="Build status"></a> (release)</br><a href="http://bioconductor.org/checkResults/devel/bioc-LATEST/affxparser/"><img border="0" src="http://bioconductor.org/shields/build/devel/bioc/affxparser.svg" alt="Build status"></a> (devel) | <a href="https://travis-ci.org/HenrikBengtsson/affxparser"><img src="https://travis-ci.org/HenrikBengtsson/affxparser.svg" alt="Build status"></a>  | <a href="https://ci.appveyor.com/project/HenrikBengtsson/affxparser"><img src="https://ci.appveyor.com/api/projects/status/github/HenrikBengtsson/affxparser?svg=true" alt="Build status"></a> |
| Test coverage |                     | <a href="https://codecov.io/gh/HenrikBengtsson/affxparser"><img src="https://codecov.io/gh/HenrikBengtsson/affxparser/branch/develop/graph/badge.svg" alt="Coverage Status"/></a>    |                  |
