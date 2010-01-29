## Usual workflow
##   first make cleanfusion
##   then make compare
##     check for files and directories that ought to be deleted
##   then copy

fusion=~/Work/packages/affy/sdk
affxparser_base=~/Work/packages/affxparser
affxparser_fusion=${affxparser_base}/src/fusion_sdk
affxparser_patchdir=${affxparser_base}/inst/info

compare :
	diff -rq ${fusion} ${affxparser_fusion} | grep Only | grep -v svn | cat

cleanfusion :
	rm -Rf	 \
	${fusion}/TestDataFiles \
	${fusion}/file_formats \
	${fusion}/interface_docs \
	${fusion}/java \
	${fusion}/libraries_and_tools \
	${fusion}/parser_examples \
	${fusion}/sample_data \
	${fusion}/*.html \
	${fusion}/*.gif \
	${fusion}/portability/CPPTest \
	${fusion}/file/CPPTest \
	${fusion}/file/readme.html \
	${fusion}/calvin_files/makefile.g5 \
	${fusion}/calvin_files/template/test \
	${fusion}/calvin_files/array/test \
	${fusion}/calvin_files/converters \
	${fusion}/calvin_files/data/test calvin_files/data/data \
	${fusion}/calvin_files/exception/test \
	${fusion}/calvin_files/fusion/test calvin_files/fusion/data \
	${fusion}/calvin_files/fusion/com \
	${fusion}/calvin_files/fusion/swig-cs/calvin_files/fusion/swig-perl \
	${fusion}/calvin_files/parameter/test \
	${fusion}/calvin_files/parsers/test calvin_files/parsers/data \
	${fusion}/calvin_files/portability/test \
	${fusion}/calvin_files/utils/test calvin_files/utils/data \
	${fusion}/calvin_files/writers/test calvin_files/writers/data \
	${fusion}/calvin_files/data/data \
	${fusion}/calvin_files/fusion/data \
	${fusion}/calvin_files/fusion/swig-perl \
	${fusion}/calvin_files/parsers/data \
	${fusion}/calvin_files/utils/data \
	${fusion}/calvin_files/writers/data \
	${fusion}/calvin_files/lib-calvin-devnet.vcproj \
	${fusion}/calvin_files/lib-fusion-apt.vcproj \
	${fusion}/file/file.vcproj \
	${fusion}/file/file.sln \
	${fusion}/file/lib-file-apt.vcproj \
	${fusion}/style.css \

patch:
	patch ${affxparser_fusion}/file/CDFFileData.cpp ${affxparser_patchdir}/CDFFileData.cpp.patch

