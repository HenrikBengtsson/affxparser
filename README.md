<div id="badges"><!-- pkgdown markup -->
<a href="https://bioconductor.org/packages/affxparser/"><img border="0" src="https://bioconductor.org/shields/years-in-bioc/affxparser.svg" alt="Bioconductor since badge"/></a> <a href="https://bioconductor.org/checkResults/release/bioc-LATEST/affxparser/"><img border="0" src="https://bioconductor.org/shields/build/release/bioc/affxparser.svg" alt="Bioconductor build status"/></a> <a href="https://bioconductor.org/checkResults/devel/bioc-LATEST/affxparser/"><img border="0" src="https://bioconductor.org/shields/build/devel/bioc/affxparser.svg" alt="Bioconductor build status"/></a> <a href="https://github.com/HenrikBengtsson/affxparser/actions?query=workflow%3AR-CMD-check"><img border="0" src="https://github.com/HenrikBengtsson/affxparser/actions/workflows/R-CMD-check.yaml/badge.svg?branch=develop" alt="R CMD check status"/></a>     <a href="https://app.codecov.io/gh/HenrikBengtsson/affxparser"><img border="0" src="https://codecov.io/gh/HenrikBengtsson/affxparser/branch/develop/graph/badge.svg" alt="Coverage Status"/></a> 
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


## Development workflow

The Bioconductor _development_ version of the **affxparser** package
is available at
<https://bioconductor.org/packages/devel/bioc/html/affxparser.html>. Bioconductor
builds and checks this development version based on the source code in
the `devel` branch of the package Git repository available at
<https://code.bioconductor.org/browse/affxparser/devel/>.

The `master` branch of _this_ GitHub repository is is manually kept in
sync with the `devel` branch on the Bioconductor Git repository.  If
there's an update made directly to the Bioconductor `devel` branch, we
merge it into the `master` branch on GitHub. Vice versa, if there are
updates to the GitHub `master` branch, we merge those into the `devel`
branch on Bioconductor.

The most common, and recommend workflow is to develop toward GitHub
instead of directly toward the Bioconductor servers.  By working
toward GitHub first, we get instant checking of the code across
different operating systems and R versions on GitHub Actions.  This
way, we can validate the code immediately, instead of having to way
24-36 hours for it to be validated on the Bioconductor check servers.


<!-- pkgdown-drop-below -->


## Contributing

To contribute to this package, please see
[CONTRIBUTING.md](CONTRIBUTING.md).

