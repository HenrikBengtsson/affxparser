

<div id="badges"><!-- pkgdown markup -->
<a href="https://bioconductor.org/packages/affxparser/"><img border="0" src="https://bioconductor.org/shields/years-in-bioc/affxparser.svg" alt="Bioconductor since badge"/></a> <a href="https://bioconductor.org/checkResults/release/bioc-LATEST/affxparser/"><img border="0" src="https://bioconductor.org/shields/build/release/bioc/affxparser.svg" alt="Bioconductor release build status"/></a> <a href="https://bioconductor.org/checkResults/devel/bioc-LATEST/affxparser/"><img border="0" src="https://bioconductor.org/shields/build/devel/bioc/affxparser.svg" alt="Bioconductor devel build status"/></a> <a href="https://github.com/HenrikBengtsson/affxparser/actions?query=workflow%3AR-CMD-check"><img border="0" src="https://github.com/HenrikBengtsson/affxparser/workflows/R-CMD-check/badge.svg?branch=develop" alt="Build status"/></a>  <a href="https://ci.appveyor.com/project/HenrikBengtsson/affxparser"><img border="0" src="https://ci.appveyor.com/api/projects/status/github/HenrikBengtsson/affxparser?svg=true" alt="Build status"/></a> <a href="https://codecov.io/gh/HenrikBengtsson/affxparser"><img border="0" src="https://codecov.io/gh/HenrikBengtsson/affxparser/branch/develop/graph/badge.svg" alt="Coverage Status"/></a> 
</div>

# affxparser: Affymetrix File Parsing SDK 


## Installation
R package affxparser is available on [Bioconductor](https://www.bioconductor.org/packages/devel/bioc/html/affxparser.html) and can be installed in R as:

```r
install.packages("BiocManager")
BiocManager::install("affxparser")
```


### Pre-release version

To install the pre-release version that is available in Git branch `develop` on GitHub, use:
```r
remotes::install_github("HenrikBengtsson/affxparser", ref="develop")
```
This will install the package from source.  Because of this and because this package also compiles native code, Windows users need to have [Rtools](https://cran.r-project.org/bin/windows/Rtools/) installed and macOS users need to have [Xcode](https://developer.apple.com/xcode/) installed.


<!-- pkgdown-drop-below -->


## Contributing

To contribute to this package, please see [CONTRIBUTING.md](CONTRIBUTING.md).

