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
remotes::install_github("HenrikBengtsson/affxparser@develop")
```
This will install the package from source.  Because of this and because this package also compiles native code, Windows users need to have [Rtools](https://cran.r-project.org/bin/windows/Rtools/) installed and macOS users need to have [Xcode](https://developer.apple.com/xcode/) installed.




## Contributions

This Git repository uses the [Git Flow](http://nvie.com/posts/a-successful-git-branching-model/) branching model (the [`git flow`](https://github.com/petervanderdoes/gitflow-avh) extension is useful for this).  The [`develop`](https://github.com/HenrikBengtsson/affxparser/tree/develop) branch contains the latest contributions and other code that will appear in the next release, and the [`master`](https://github.com/HenrikBengtsson/affxparser) branch contains the code of the latest release, which is exactly what is currently on [Bioconductor](https://www.bioconductor.org/packages/devel/bioc/html/affxparser.html).

Contributing to this package is easy.  Just send a [pull request](https://help.github.com/articles/using-pull-requests/).  When you send your PR, make sure `develop` is the destination branch on the [affxparser repository](https://github.com/HenrikBengtsson/affxparser).  Your PR should pass `R CMD check --as-cran`, which will also be checked by <a href="https://travis-ci.org/HenrikBengtsson/affxparser">Travis CI</a> and <a href="https://ci.appveyor.com/project/HenrikBengtsson/affxparser">AppVeyor CI</a> when the PR is submitted.


## Software status

| Resource:     | Bioconductor        | Travis CI       | Appveyor         |
| ------------- | ------------------- | --------------- | ---------------- |
| _Platforms:_  | _Multiple_          | _Linux & macOS_ | _Windows_        |
| R CMD check   | <a href="https://bioconductor.org/checkResults/release/bioc-LATEST/affxparser/"><img border="0" src="https://bioconductor.org/shields/build/release/bioc/affxparser.svg" alt="Build status"></a> (release)</br><a href="https://bioconductor.org/checkResults/devel/bioc-LATEST/affxparser/"><img border="0" src="https://bioconductor.org/shields/build/devel/bioc/affxparser.svg" alt="Build status"></a> (devel) | <a href="https://travis-ci.org/HenrikBengtsson/affxparser"><img src="https://travis-ci.org/HenrikBengtsson/affxparser.svg" alt="Build status"></a>   | <a href="https://ci.appveyor.com/project/HenrikBengtsson/affxparser"><img src="https://ci.appveyor.com/api/projects/status/github/HenrikBengtsson/affxparser?svg=true" alt="Build status"></a> |
| Test coverage |                     | <a href="https://codecov.io/gh/HenrikBengtsson/affxparser"><img src="https://codecov.io/gh/HenrikBengtsson/affxparser/branch/develop/graph/badge.svg" alt="Coverage Status"/></a>     |                  |
