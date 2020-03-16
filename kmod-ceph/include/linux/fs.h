/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _KC_LINUX_FS_H
#define _KC_LINUX_FS_H

#include_next <linux/fs.h>
#include <linux/errseq.h>

struct fs_context;

static inline unsigned int i_blocksize(const struct inode *node)
{
	return (1 << node->i_blkbits);
}

/*
 * sb->s_flags.  Note that these mirror the equivalent MS_* flags where
 * represented in both.
 */
#define SB_RDONLY	 1	/* Mount read-only */
#define SB_NOSUID	 2	/* Ignore suid and sgid bits */
#define SB_NODEV	 4	/* Disallow access to device special files */
#define SB_NOEXEC	 8	/* Disallow program execution */
#define SB_SYNCHRONOUS	16	/* Writes are synced at once */
#define SB_MANDLOCK	64	/* Allow mandatory locks on an FS */
#define SB_DIRSYNC	128	/* Directory modifications are synchronous */
#define SB_NOATIME	1024	/* Do not update access times. */
#define SB_NODIRATIME	2048	/* Do not update directory access times */
#define SB_SILENT	32768
#define SB_POSIXACL	(1<<16)	/* VFS does not apply the umask */
#define SB_KERNMOUNT	(1<<22) /* this is a kern_mount call */
#define SB_I_VERSION	(1<<23) /* Update inode I_version field */
#define SB_LAZYTIME	(1<<25) /* Update the on-disk [acm]times lazily */

/* These sb flags are internal to the kernel */
//#define SB_SUBMOUNT     (1<<26)
//#define SB_FORCE    	(1<<27)
#define SB_NOSEC	(1<<28)
#define SB_BORN		(1<<29)
#define SB_ACTIVE	(1<<30)
#define SB_NOUSER	(1<<31)

#ifdef CONFIG_COMPAT
extern long compat_ptr_ioctl(struct file *file, unsigned int cmd,
					unsigned long arg);
#else
#define compat_ptr_ioctl NULL
#endif

struct dir_context {
	filldir_t actor;
	loff_t pos;
};

int set_anon_super_fc(struct super_block *s, struct fs_context *fc);

extern void discard_new_inode(struct inode *);

static inline bool dir_emit(struct dir_context *ctx,
			    const char *name, int namelen,
			    u64 ino, unsigned type)
{
	return ctx->actor(ctx, name, namelen, ctx->pos, ino, type) == 0;
}
static inline bool dir_emit_dot(struct file *file, struct dir_context *ctx)
{
	return ctx->actor(ctx, ".", 1, ctx->pos,
			  file->f_path.dentry->d_inode->i_ino, DT_DIR) == 0;
}
static inline bool dir_emit_dotdot(struct file *file, struct dir_context *ctx)
{
	return ctx->actor(ctx, "..", 2, ctx->pos,
			  parent_ino(file->f_path.dentry), DT_DIR) == 0;
}
static inline bool dir_emit_dots(struct file *file, struct dir_context *ctx)
{
	if (ctx->pos == 0) {
		if (!dir_emit_dot(file, ctx))
			return false;
		ctx->pos = 1;
	}
	if (ctx->pos == 1) {
		if (!dir_emit_dotdot(file, ctx))
			return false;
		ctx->pos = 2;
	}
	return true;
}

#endif /* _KC_LINUX_FS_H */
