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

obj-m := rbd.o

deps-y := ../../mm/util.o ../../block/blk-core.o \
	../../fs/fs_parser.o ../../fs/fs_context.o
deps-$(CONFIG_LOCKDEP) += ../../kernel/locking/lockdep.o

rbd-y := _rbd.o $(deps-y)

ccflags-y += -DCONFIG_CEPH_LIB_PRETTYDEBUG

endif
