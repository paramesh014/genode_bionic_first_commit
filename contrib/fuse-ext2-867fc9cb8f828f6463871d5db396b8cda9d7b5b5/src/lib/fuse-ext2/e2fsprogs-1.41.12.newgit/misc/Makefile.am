root_sysconfdir= @root_sysconfdir@
DEFS = -DROOT_SYSCONFDIR=\"$(root_sysconfdir)\" @DEFS@

bin_PROGRAMS = \
	fuse-ext2.e2label \
	fuse-ext2.mke2fs

fuse_ext2_e2label_SOURCES = \
	e2label.c

fuse_ext2_mke2fs_SOURCES = \
	mke2fs.c \
	util.c \
	prof_err.c \
	profile.c \
	profile.h \
	default_profile.c

fuse_ext2_mke2fs_CFLAGS = \
	-I$(top_srcdir)/e2fsprogs-1.41.12.newgit

fuse_ext2_mke2fs_LDFLAGS = \
	../ext2fs/libext2fs.a	\
	../et/libcom_err.a \
	../e2p/libe2p.a \
	../blkid/libblkid.a \
	../uuid/libuuid.a
