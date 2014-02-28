## Usual workflow
##   first make cleanfusion
##   then make compare
##     check for files and directories that ought to be deleted
##   then copy

fusion=~/Work/packages/FusionSDK/affy/sdk
affxparser_base=~/Work/packages/affxparser
affxparser_fusion=${affxparser_base}/src/fusion_sdk
affxparser_patchdir_1.1.0=${affxparser_base}/inst/info/fusion_1.1.0_patches
affxparser_patchdir_1.1.2=${affxparser_base}/inst/info/fusion_1.1.2_patches

compare_only :
	diff -rq ${fusion} ${affxparser_fusion} | grep Only | grep -v svn | cat

compare :
	diff -rq ${fusion} ${affxparser_fusion}

make_patches

cleanfusion :
	rm -Rf	 \
	${fusion}/TestDataFiles \
	${fusion}/file_formats \
	${fusion}/interface_docs \
	${fusion}/java \
	${fusion}/libraries_and_tools \
	${fusion}/parser_examples \
	${fusion}/sample_data \
	${fusion}/images \
	${fusion}/css \
	${fusion}/*.html \
	${fusion}/*.gif \
	${fusion}/portability/CPPTest \
	${fusion}/file/CPPTest \
	${fusion}/file/readme.html \
	${fusion}/file/TsvFile/*.vcproj \
	${fusion}/file/TsvFile/*.csv \
	${fusion}/file/TsvFile/*.pgf \
	${fusion}/file/TsvFile/*.tsv \
	${fusion}/file/TsvFile/*.txt \
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
	${fusion}/util/lib-util-devnet.vcproj \
	${fusion}/style.css \


patch_1.1.0:
	patch -N ${affxparser_fusion}/portability/affy-base-types.h ${affxparser_patchdir_1.1.0}/affy-base-types.h-patch
	patch -N ${affxparser_fusion}/util/AffxConv.cpp ${affxparser_patchdir_1.1.0}/AffxConv.cpp-patch
	patch -N ${affxparser_fusion}/util/AffxByteArray.cpp ${affxparser_patchdir_1.1.0}/AffxByteArray.cpp-patch
	patch -N ${affxparser_fusion}/calvin_files/portability/src/AffymetrixBaseTypes.h ${affxparser_patchdir_1.1.0}/AffymetrixBaseTypes.h-patch

patch_1.1.2:
	patch -N ${affxparser_fusion}/file/CDFFileData.cpp ${affxparser_patchdir_1.1.2}/CDFFileData.cpp-patch
	patch -N ${affxparser_fusion}/portability/affy-base-types.h ${affxparser_patchdir_1.1.2}/affy-base-types.h-patch
	patch -N ${affxparser_fusion}/util/AffxConv.cpp ${affxparser_patchdir_1.1.2}/AffxConv.cpp-patch
	patch -N ${affxparser_fusion}/util/AffxByteArray.cpp ${affxparser_patchdir_1.1.2}/AffxByteArray.cpp-patch
	patch -N ${affxparser_fusion}/calvin_files/portability/src/AffymetrixBaseTypes.h ${affxparser_patchdir_1.1.2}/AffymetrixBaseTypes.h-patch
	patch -N ${affxparser_fusion}/calvin_files/utils/src/FileUtils.cpp ${affxparser_patchdir_1.1.2}/FileUtils.cpp-patch
	patch -N ${affxparser_fusion}/util/Fs.cpp ${affxparser_patchdir_1.1.2}/Fs.cpp-patch
	patch -N ${affxparser_fusion}/util/Verbose.cpp ${affxparser_patchdir_1.1.2}/Verbose.cpp-patch
	patch -N ${affxparser_fusion}/calvin_files/data/src/CHPMultiDataData.cpp ${affxparser_patchdir_1.1.2}/CHPMultiDataData.cpp-patch
	patch -N ${affxparser_fusion}/file/CELFileData.h ${affxparser_patchdir_1.1.2}/CELFileData.h-patch
	patch -N ${affxparser_fusion}/file/TsvFile/TsvFile.cpp ${affxparser_patchdir_1.1.2}/TsvFile.cpp-patch
	patch -N ${affxparser_fusion}/util/AffxConv.cpp ${affxparser_patchdir_1.1.2}/AffxConv.cpp-patch2
	patch -N ${affxparser_fusion}/util/Util.cpp ${affxparser_patchdir_1.1.2}/Util.cpp-patch
