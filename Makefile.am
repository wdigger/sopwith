AUX_DIST_GEN =                          \
        $(ac_aux_dir)/config.guess      \
        $(ac_aux_dir)/config.sub        \
        $(ac_aux_dir)/install-sh        \
        $(ac_aux_dir)/missing           \
        $(ac_aux_dir)/mkinstalldirs

# extra distributed MSVC build files
MSVC_DIST= \
	msvc/README  msvc/config.h msvc/sopwith.vcproj

EXTRA_DIST = $(AUX_DIST_GEN) $(MSVC_DIST)

MAINTAINERCLEANFILES =  $(AUX_DIST_GEN)

docdir=$(prefix)/share/doc/@PACKAGE@
SUBDIRS=src doc

blah:
