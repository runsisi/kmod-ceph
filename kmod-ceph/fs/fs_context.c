// SPDX-License-Identifier: GPL-2.0-or-later
/* Provide a way to create a superblock configuration context within the kernel
 * that allows a superblock to be set up prior to mounting.
 *
 * Copyright (C) 2017 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 */

#include <linux/ceph/ceph_debug.h>

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/module.h>
#include <linux/fs_context.h>
#include <linux/fs_parser.h>
#include <linux/fs.h>
#include <linux/mount.h>
#include <linux/nsproxy.h>
#include <linux/slab.h>
#include <linux/magic.h>
#include <linux/security.h>
#include <linux/mnt_namespace.h>
#include <linux/pid_namespace.h>
#include <linux/user_namespace.h>
#include <net/net_namespace.h>
#include <asm/sections.h>

static const struct constant_table common_set_sb_flag[] = {
	{ "dirsync",	SB_DIRSYNC },
	{ "lazytime",	SB_LAZYTIME },
	{ "mand",	SB_MANDLOCK },
	{ "posixacl",	SB_POSIXACL },
	{ "ro",		SB_RDONLY },
	{ "sync",	SB_SYNCHRONOUS },
	{ },
};

static const struct constant_table common_clear_sb_flag[] = {
	{ "async",	SB_SYNCHRONOUS },
	{ "nolazytime",	SB_LAZYTIME },
	{ "nomand",	SB_MANDLOCK },
	{ "rw",		SB_RDONLY },
	{ "silent",	SB_SILENT },
	{ },
};

static const char *const forbidden_sb_flag[] = {
	"bind",
	"dev",
	"exec",
	"move",
	"noatime",
	"nodev",
	"nodiratime",
	"noexec",
	"norelatime",
	"nostrictatime",
	"nosuid",
	"private",
	"rec",
	"relatime",
	"remount",
	"shared",
	"slave",
	"strictatime",
	"suid",
	"unbindable",
};

/*
 * Check for a common mount option that manipulates s_flags.
 */
static int vfs_parse_sb_flag(struct fs_context *fc, const char *key)
{
	unsigned int token;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(forbidden_sb_flag); i++)
		if (strcmp(key, forbidden_sb_flag[i]) == 0)
			return -EINVAL;

	token = lookup_constant(common_set_sb_flag, key, 0);
	if (token) {
		fc->sb_flags |= token;
		fc->sb_flags_mask |= token;
		return 0;
	}

	token = lookup_constant(common_clear_sb_flag, key, 0);
	if (token) {
		fc->sb_flags &= ~token;
		fc->sb_flags_mask |= token;
		return 0;
	}

	return -ENOPARAM;
}

/**
 * vfs_parse_fs_param - Add a single parameter to a superblock config
 * @fc: The filesystem context to modify
 * @param: The parameter
 *
 * A single mount option in string form is applied to the filesystem context
 * being set up.  Certain standard options (for example "ro") are translated
 * into flag bits without going to the filesystem.  The active security module
 * is allowed to observe and poach options.  Any other options are passed over
 * to the filesystem to parse.
 *
 * This may be called multiple times for a context.
 *
 * Returns 0 on success and a negative error code on failure.  In the event of
 * failure, supplementary error information may have been set.
 */
int vfs_parse_fs_param(struct fs_context *fc, struct fs_parameter *param)
{
	int ret;

	if (!param->key)
		return invalf(fc, "Unnamed parameter\n");

	ret = vfs_parse_sb_flag(fc, param->key);
	if (ret != -ENOPARAM)
		return ret;

//	ret = security_fs_context_parse_param(fc, param);
//	if (ret != -ENOPARAM)
//		/* Param belongs to the LSM or is disallowed by the LSM; so
//		 * don't pass to the FS.
//		 */
//		return ret;

	if (fc->ops->parse_param) {
		ret = fc->ops->parse_param(fc, param);
		if (ret != -ENOPARAM)
			return ret;
	}

	/* If the filesystem doesn't take any arguments, give it the
	 * default handling of source.
	 */
//	if (strcmp(param->key, "source") == 0) {
//		if (param->type != fs_value_is_string)
//			return invalf(fc, "VFS: Non-string source");
//		if (fc->source)
//			return invalf(fc, "VFS: Multiple sources");
//		fc->source = param->string;
//		param->string = NULL;
//		return 0;
//	}

	return invalf(fc, "%s: Unknown parameter '%s'",
		      fc->fs_type->name, param->key);
}
//EXPORT_SYMBOL(vfs_parse_fs_param);

/**
 * vfs_parse_fs_string - Convenience function to just parse a string.
 */
int vfs_parse_fs_string(struct fs_context *fc, const char *key,
			const char *value, size_t v_size)
{
	int ret;

	struct fs_parameter param = {
		.key	= key,
		.type	= fs_value_is_flag,
		.size	= v_size,
	};

	if (value) {
		param.string = kmemdup_nul(value, v_size, GFP_KERNEL);
		if (!param.string)
			return -ENOMEM;
		param.type = fs_value_is_string;
	}

	ret = vfs_parse_fs_param(fc, &param);
	kfree(param.string);
	return ret;
}
//EXPORT_SYMBOL(vfs_parse_fs_string);

/**
 * generic_parse_monolithic - Parse key[=val][,key[=val]]* mount data
 * @ctx: The superblock configuration to fill in.
 * @data: The data to parse
 *
 * Parse a blob of data that's in key[=val][,key[=val]]* form.  This can be
 * called from the ->monolithic_mount_data() fs_context operation.
 *
 * Returns 0 on success or the error returned by the ->parse_option() fs_context
 * operation on failure.
 */
int generic_parse_monolithic(struct fs_context *fc, void *data)
{
	char *options = data, *key;
	int ret = 0;

	if (!options)
		return 0;

//	ret = security_sb_eat_lsm_opts(options, &fc->security);
//	if (ret)
//		return ret;

	while ((key = strsep(&options, ",")) != NULL) {
		if (*key) {
			size_t v_len = 0;
			char *value = strchr(key, '=');

			if (value) {
				if (value == key)
					continue;
				*value++ = 0;
				v_len = strlen(value);
			}
			ret = vfs_parse_fs_string(fc, key, value, v_len);
			if (ret < 0)
				break;
		}
	}

	return ret;
}
//EXPORT_SYMBOL(generic_parse_monolithic);

int parse_monolithic_mount_data(struct fs_context *fc, void *data)
{
	int (*monolithic_mount_data)(struct fs_context *, void *);

	monolithic_mount_data = fc->ops->parse_monolithic;
	if (!monolithic_mount_data)
		monolithic_mount_data = generic_parse_monolithic;

	return monolithic_mount_data(fc, data);
}
