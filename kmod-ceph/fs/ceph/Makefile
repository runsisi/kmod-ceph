# If KERNELRELEASE is defined, the make command using this Makefile has
# been invoked by the kernel build system and so can use its language.
# Otherwise, if KERNELRELEASE is null, a make command was issued from
# the command line. So invoke the kernel build system.

ifeq ($(KERNELRELEASE),)

    # KVERSION should be set in the environment if this
    # build is not for the currently running kernel.
    KVERSION ?= $(shell uname -r)

    # BUILD_DIR should be set in the environment if a
    # subdirectory of /lib/modules/ is not appropriate.
    BUILD_DIR ?= /lib/modules/${KVERSION}/build

    PWD := $(shell pwd)

    export NOSTDINC_FLAGS
    NOSTDINC_FLAGS += -I$(PWD)/../../arch/x86/include
    NOSTDINC_FLAGS += -I$(PWD)/../../include

modules:
	echo $(PWD)
	$(MAKE) -C $(BUILD_DIR) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(BUILD_DIR) M=$(PWD) modules_install

clean:
	$(MAKE) -C $(BUILD_DIR) M=$(PWD) clean

.PHONY: modules modules_install clean

else

# Called from kernel build system -- just declare the module(s).

obj-m := ceph.o

deps-y := ../../mm/util.o ../../fs/fs_parser.o ../../fs/fs_context.o \
	../../fs/inode.o ../../fs/dcache.o ../../fs/ioctl.o \
	../../fs/super.o ../../fs/posix_acl.o \
	../../lib/errseq.o ../../lib/refcount.o ../../lib/iov_iter.o \
	../../mm/backing-dev.o

ceph-y := super.o inode.o dir.o file.o locks.o addr.o ioctl.o \
	export.o caps.o snap.o xattr.o quota.o io.o \
	mds_client.o mdsmap.o strings.o ceph_frag.o \
	debugfs.o util.o $(deps-y)

ifdef CONFIG_FSCACHE
ccflags-y += -DCONFIG_CEPH_FSCACHE
ceph-y += cache.o
endif

ifdef CONFIG_FS_POSIX_ACL
ccflags-y += -DCONFIG_CEPH_FS_POSIX_ACL
ceph-y += acl.o
endif

ccflags-y += -DCONFIG_CEPH_LIB_PRETTYDEBUG

endif
