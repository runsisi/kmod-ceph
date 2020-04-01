// SPDX-License-Identifier: GPL-2.0-or-later
/* Filesystem parameter parser.
 *
 * Copyright (C) 2018 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 */

#include <linux/ceph/ceph_debug.h>

#include <linux/export.h>
#include <linux/fs_context.h>
#include <linux/fs_parser.h>
#include <linux/slab.h>
#include <linux/security.h>
#include <linux/namei.h>

static const struct constant_table bool_names[] = {
	{ "0",		false },
	{ "1",		true },
	{ "false",	false },
	{ "no",		false },
	{ "true",	true },
	{ "yes",	true },
	{ },
};

static const struct constant_table *
__lookup_constant(const struct constant_table *tbl, const char *name)
{
	for ( ; tbl->name; tbl++)
		if (strcmp(name, tbl->name) == 0)
			return tbl;
	return NULL;
}

/**
 * lookup_constant - Look up a constant by name in an ordered table
 * @tbl: The table of constants to search.
 * @name: The name to look up.
 * @not_found: The value to return if the name is not found.
 */
int lookup_constant(const struct constant_table *tbl, const char *name, int not_found)
{
	const struct constant_table *p = __lookup_constant(tbl, name);

	return p ? p->value : not_found;
}
//EXPORT_SYMBOL(lookup_constant);

static inline bool is_flag(const struct fs_parameter_spec *p)
{
	return p->type == NULL;
}

static const struct fs_parameter_spec *fs_lookup_key(
	const struct fs_parameter_spec *desc,
	struct fs_parameter *param, bool *negated)
{
	const struct fs_parameter_spec *p, *other = NULL;
	const char *name = param->key;
	bool want_flag = param->type == fs_value_is_flag;

	*negated = false;
	for (p = desc; p->name; p++) {
		if (strcmp(p->name, name) != 0)
			continue;
		if (likely(is_flag(p) == want_flag))
			return p;
		other = p;
	}
	if (want_flag) {
		if (name[0] == 'n' && name[1] == 'o' && name[2]) {
			for (p = desc; p->name; p++) {
				if (strcmp(p->name, name + 2) != 0)
					continue;
				if (!(p->flags & fs_param_neg_with_no))
					continue;
				*negated = true;
				return p;
			}
		}
	}
	return other;
}

/*
 * fs_parse - Parse a filesystem configuration parameter
 * @fc: The filesystem context to log errors through.
 * @desc: The parameter description to use.
 * @param: The parameter.
 * @result: Where to place the result of the parse
 *
 * Parse a filesystem configuration parameter and attempt a conversion for a
 * simple parameter for which this is requested.  If successful, the determined
 * parameter ID is placed into @result->key, the desired type is indicated in
 * @result->t and any converted value is placed into an appropriate member of
 * the union in @result.
 *
 * The function returns the parameter number if the parameter was matched,
 * -ENOPARAM if it wasn't matched and @desc->ignore_unknown indicated that
 * unknown parameters are okay and -EINVAL if there was a conversion issue or
 * the parameter wasn't recognised and unknowns aren't okay.
 */
int __fs_parse(struct p_log *log,
	     const struct fs_parameter_spec *desc,
	     struct fs_parameter *param,
	     struct fs_parse_result *result)
{
	const struct fs_parameter_spec *p;

	result->uint_64 = 0;

	p = fs_lookup_key(desc, param, &result->negated);
	if (!p)
		return -ENOPARAM;

	if (p->flags & fs_param_deprecated)
		warn_plog(log, "Deprecated parameter '%s'", param->key);

	/* Try to turn the type we were given into the type desired by the
	 * parameter and give an error if we can't.
	 */
	if (is_flag(p)) {
		if (param->type != fs_value_is_flag)
			return inval_plog(log, "Unexpected value for '%s'",
				      param->key);
		result->boolean = !result->negated;
	} else  {
		int ret = p->type(log, p, param, result);
		if (ret)
			return ret;
	}
	return p->opt;
}
//EXPORT_SYMBOL(__fs_parse);

int fs_param_bad_value(struct p_log *log, struct fs_parameter *param)
{
	return inval_plog(log, "Bad value for '%s'", param->key);
}

int fs_param_is_bool(struct p_log *log, const struct fs_parameter_spec *p,
		     struct fs_parameter *param, struct fs_parse_result *result)
{
	int b;
	if (param->type != fs_value_is_string)
		return fs_param_bad_value(log, param);
	b = lookup_constant(bool_names, param->string, -1);
	if (b == -1)
		return fs_param_bad_value(log, param);
	result->boolean = b;
	return 0;
}
//EXPORT_SYMBOL(fs_param_is_bool);

int fs_param_is_u32(struct p_log *log, const struct fs_parameter_spec *p,
		    struct fs_parameter *param, struct fs_parse_result *result)
{
	int base = (unsigned long)p->data;
	if (param->type != fs_value_is_string ||
	    kstrtouint(param->string, base, &result->uint_32) < 0)
		return fs_param_bad_value(log, param);
	return 0;
}
//EXPORT_SYMBOL(fs_param_is_u32);

int fs_param_is_s32(struct p_log *log, const struct fs_parameter_spec *p,
		    struct fs_parameter *param, struct fs_parse_result *result)
{
	if (param->type != fs_value_is_string ||
	    kstrtoint(param->string, 0, &result->int_32) < 0)
		return fs_param_bad_value(log, param);
	return 0;
}
//EXPORT_SYMBOL(fs_param_is_s32);

int fs_param_is_u64(struct p_log *log, const struct fs_parameter_spec *p,
		    struct fs_parameter *param, struct fs_parse_result *result)
{
	if (param->type != fs_value_is_string ||
	    kstrtoull(param->string, 0, &result->uint_64) < 0)
		return fs_param_bad_value(log, param);
	return 0;
}
//EXPORT_SYMBOL(fs_param_is_u64);

int fs_param_is_enum(struct p_log *log, const struct fs_parameter_spec *p,
		     struct fs_parameter *param, struct fs_parse_result *result)
{
	const struct constant_table *c;
	if (param->type != fs_value_is_string)
		return fs_param_bad_value(log, param);
	c = __lookup_constant(p->data, param->string);
	if (!c)
		return fs_param_bad_value(log, param);
	result->uint_32 = c->value;
	return 0;
}
//EXPORT_SYMBOL(fs_param_is_enum);

int fs_param_is_string(struct p_log *log, const struct fs_parameter_spec *p,
		       struct fs_parameter *param, struct fs_parse_result *result)
{
	if (param->type != fs_value_is_string || !*param->string)
		return fs_param_bad_value(log, param);
	return 0;
}
//EXPORT_SYMBOL(fs_param_is_string);

int fs_param_is_blob(struct p_log *log, const struct fs_parameter_spec *p,
		     struct fs_parameter *param, struct fs_parse_result *result)
{
	if (param->type != fs_value_is_blob)
		return fs_param_bad_value(log, param);
	return 0;
}
//EXPORT_SYMBOL(fs_param_is_blob);

int fs_param_is_fd(struct p_log *log, const struct fs_parameter_spec *p,
		  struct fs_parameter *param, struct fs_parse_result *result)
{
	switch (param->type) {
	case fs_value_is_string:
		if (kstrtouint(param->string, 0, &result->uint_32) < 0)
			break;
		if (result->uint_32 <= INT_MAX)
			return 0;
		break;
	case fs_value_is_file:
		result->uint_32 = param->dirfd;
		if (result->uint_32 <= INT_MAX)
			return 0;
		break;
	default:
		break;
	}
	return fs_param_bad_value(log, param);
}
//EXPORT_SYMBOL(fs_param_is_fd);

int fs_param_is_blockdev(struct p_log *log, const struct fs_parameter_spec *p,
		  struct fs_parameter *param, struct fs_parse_result *result)
{
	return 0;
}
//EXPORT_SYMBOL(fs_param_is_blockdev);

int fs_param_is_path(struct p_log *log, const struct fs_parameter_spec *p,
		     struct fs_parameter *param, struct fs_parse_result *result)
{
	return 0;
}
//EXPORT_SYMBOL(fs_param_is_path);
