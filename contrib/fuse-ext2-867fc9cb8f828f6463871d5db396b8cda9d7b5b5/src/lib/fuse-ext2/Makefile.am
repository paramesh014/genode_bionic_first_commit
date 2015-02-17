if LINUX
if STATIC_E2FS
SUBDIRS = e2fsprogs-1.41.12.newgit
else
SUBDIRS = 
endif
SUBDIRS += fuse-ext2
SUBDIRS += test/pjd-fstest-20080816
else
SUBDIRS = e2fsprogs-1.41.12.newgit
SUBDIRS += fuse-ext2
SUBDIRS += tools
SUBDIRS += test/pjd-fstest-20080816
endif

EXTRA_DIST = \
	fuse-ext2.pc.in

pkgconfigdir = @pkgconfigdir@
pkgconfig_DATA = fuse-ext2.pc

$(pkgconfig_DATA): config.status

if DARWIN
package:
	$(SHELL) $(top_srcdir)/tools/macosx/make-pkg.sh "$(VERSION)" "$(top_builddir)" "$(top_srcdir)/tools/macosx/"
endif
if LINUX
package:
	echo "You can use checkinstall or some other equivalent tool to generate install package for your distribution."
endif
