// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2014-2021, The Linux Foundation. All rights reserved.
 */
#include <linux/compat.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/msm_ion.h>
#include <linux/slab.h>

#include "adsprpc_compat.h"
#include "adsprpc_shared.h"

#define COMPAT_FASTRPC_IOCTL_INVOKE \
		_IOWR('R', 1, struct compat_fastrpc_ioctl_invoke)
#define COMPAT_FASTRPC_IOCTL_MMAP \
		_IOWR('R', 2, struct compat_fastrpc_ioctl_mmap)
#define COMPAT_FASTRPC_IOCTL_MUNMAP \
		_IOWR('R', 3, struct compat_fastrpc_ioctl_munmap)
#define COMPAT_FASTRPC_IOCTL_INVOKE_FD \
		_IOWR('R', 4, struct compat_fastrpc_ioctl_invoke_fd)
#define COMPAT_FASTRPC_IOCTL_INIT \
		_IOWR('R', 6, struct compat_fastrpc_ioctl_init)
#define COMPAT_FASTRPC_IOCTL_INVOKE_ATTRS \
		_IOWR('R', 7, struct compat_fastrpc_ioctl_invoke_attrs)
#define COMPAT_FASTRPC_IOCTL_INIT_ATTRS \
		_IOWR('R', 10, struct compat_fastrpc_ioctl_init_attrs)
#define COMPAT_FASTRPC_IOCTL_INVOKE_CRC \
		_IOWR('R', 11, struct compat_fastrpc_ioctl_invoke_crc)
#define COMPAT_FASTRPC_IOCTL_CONTROL \
		_IOWR('R', 12, struct compat_fastrpc_ioctl_control)
#define COMPAT_FASTRPC_IOCTL_MMAP_64 \
		_IOWR('R', 14, struct compat_fastrpc_ioctl_mmap_64)
#define COMPAT_FASTRPC_IOCTL_MUNMAP_64 \
		_IOWR('R', 15, struct compat_fastrpc_ioctl_munmap_64)
#define COMPAT_FASTRPC_IOCTL_GET_DSP_INFO \
		_IOWR('R', 17, \
			struct compat_fastrpc_ioctl_capability)
#define COMPAT_FASTRPC_IOCTL_INVOKE2 \
			 _IOWR('R', 18, struct compat_fastrpc_ioctl_invoke2)
#define COMPAT_FASTRPC_IOCTL_MEM_MAP \
		_IOWR('R', 19, struct compat_fastrpc_ioctl_mem_map)
#define COMPAT_FASTRPC_IOCTL_MEM_UNMAP \
		_IOWR('R', 20, struct compat_fastrpc_ioctl_mem_unmap)
#define COMPAT_FASTRPC_IOCTL_INVOKE_PERF \
		_IOWR('R', 21, struct compat_fastrpc_ioctl_invoke_perf)
#define COMPAT_FASTRPC_IOCTL_DSPSIGNAL_CREATE \
		_IOWR('R', 23, struct fastrpc_ioctl_dspsignal_create)
#define COMPAT_FASTRPC_IOCTL_DSPSIGNAL_DESTROY \
		_IOWR('R', 24, struct fastrpc_ioctl_dspsignal_destroy)
#define COMPAT_FASTRPC_IOCTL_DSPSIGNAL_SIGNAL \
		_IOWR('R', 25, struct fastrpc_ioctl_dspsignal_signal)
#define COMPAT_FASTRPC_IOCTL_DSPSIGNAL_WAIT \
		_IOWR('R', 26, struct fastrpc_ioctl_dspsignal_wait)
#define COMPAT_FASTRPC_IOCTL_DSPSIGNAL_CANCEL_WAIT \
		_IOWR('R', 27, struct fastrpc_ioctl_dspsignal_cancel_wait)

struct compat_remote_buf {
	compat_uptr_t pv;	/* buffer pointer */
	compat_size_t len;	/* length of buffer */
};

union compat_remote_arg {
	struct compat_remote_buf buf;
	compat_uint_t h;
};

struct compat_fastrpc_ioctl_invoke {
	compat_uint_t handle;	/* remote handle */
	compat_uint_t sc;	/* scalars describing the data */
	compat_uptr_t pra;	/* remote arguments list */
};

struct compat_fastrpc_ioctl_invoke_fd {
	struct compat_fastrpc_ioctl_invoke inv;
	compat_uptr_t fds;	/* fd list */
};

struct compat_fastrpc_ioctl_invoke_attrs {
	struct compat_fastrpc_ioctl_invoke inv;
	compat_uptr_t fds;	/* fd list */
	compat_uptr_t attrs;	/* attribute list */
};

struct compat_fastrpc_ioctl_invoke_crc {
	struct compat_fastrpc_ioctl_invoke inv;
	compat_uptr_t fds;	/* fd list */
	compat_uptr_t attrs;	/* attribute list */
	compat_uptr_t crc;	/* crc list */
};

struct compat_fastrpc_ioctl_invoke_perf {
	struct compat_fastrpc_ioctl_invoke inv;
	compat_uptr_t fds;
	compat_uptr_t attrs;
	compat_uptr_t crc;
	compat_uptr_t perf_kernel;
	compat_uptr_t perf_dsp;
};

struct compat_fastrpc_ioctl_invoke_async {
	struct compat_fastrpc_ioctl_invoke inv;
	compat_uptr_t fds;			/* fd list */
	compat_uptr_t attrs;		/* attribute list */
	compat_uptr_t crc;			/* crc list */
	compat_uptr_t perf_kernel;	/* Kernel perf data pointer */
	compat_uptr_t perf_dsp;		/* DSP perf data pointer */
	compat_uptr_t job;			/* Async job */
};
struct compat_fastrpc_ioctl_invoke_async_no_perf {
	struct compat_fastrpc_ioctl_invoke inv;
	compat_uptr_t fds;			/* fd list */
	compat_uptr_t attrs;		/* attribute list */
	compat_uptr_t crc;			/* crc list */
	compat_uptr_t job;			/* Async job */
};

struct compat_fastrpc_ioctl_invoke2 {
	compat_uint_t req;		/* type of invocation request */
	compat_uptr_t invparam;	/* invocation request param */
	compat_uint_t size;		/* size of invocation param */
	compat_int_t  err;		/* reserved */
};
struct compat_fastrpc_ioctl_async_response {
	compat_u64 jobid;			 /* job id generated by user */
	compat_int_t result;		 /* result from DSP */
	compat_uptr_t perf_kernel;	 /* Kernel perf data pointer */
	compat_uptr_t perf_dsp;		 /* DSP perf data pointer */
	compat_uint_t handle;
	compat_uint_t sc;
};

struct compat_fastrpc_ioctl_notif_rsp {
	compat_int_t domain;		 /* Domain of User PD */
	compat_int_t session;		 /* Session ID of User PD */
	compat_uint_t status;		 /* Status of the process */
};

struct compat_fastrpc_mem_map {
	compat_int_t fd;	/* ion fd */
	compat_int_t offset;	/* buffer offset */
	compat_uint_t flags;	/* flags to control memory map */
	compat_uint_t attrs;	/* buffer attributes used for SMMU mapping */
	compat_uptr_t vaddrin;	/* virtual address */
	compat_size_t length;	/* buffer length */
	compat_u64 vaddrout;	/* dsp virtual address */
};

struct compat_fastrpc_ioctl_mem_map {
	compat_int_t version;
	union {
		struct compat_fastrpc_mem_map m;
		compat_int_t reserved[MAP_RESERVED_NUM];
	};
};

struct compat_fastrpc_mem_unmap {
	compat_int_t fd;		/* ion fd */
	compat_u64 vaddr;		/* dsp virtual address */
	compat_size_t length;		/* buffer length */
};

struct compat_fastrpc_ioctl_mem_unmap {
	compat_int_t version;
	union {
		struct compat_fastrpc_mem_unmap um;
		compat_int_t reserved[UNMAP_RESERVED_NUM];
	};
};

struct compat_fastrpc_ioctl_mmap {
	compat_int_t fd;	/* ion fd */
	compat_uint_t flags;	/* flags for dsp to map with */
	compat_uptr_t vaddrin;	/* optional virtual address */
	compat_size_t size;	/* size */
	compat_uptr_t vaddrout;	/* dsps virtual address */
};

struct compat_fastrpc_ioctl_mmap_64 {
	compat_int_t fd;	/* ion fd */
	compat_uint_t flags;	/* flags for dsp to map with */
	compat_u64 vaddrin;	/* optional virtual address */
	compat_size_t size;	/* size */
	compat_u64 vaddrout;	/* dsps virtual address */
};

struct compat_fastrpc_ioctl_munmap {
	compat_uptr_t vaddrout;	/* address to unmap */
	compat_size_t size;	/* size */
};

struct compat_fastrpc_ioctl_munmap_64 {
	compat_u64 vaddrout;	/* address to unmap */
	compat_size_t size;	/* size */
};

struct compat_fastrpc_ioctl_init {
	compat_uint_t flags;	/* one of FASTRPC_INIT_* macros */
	compat_uptr_t file;	/* pointer to elf file */
	compat_int_t filelen;	/* elf file length */
	compat_int_t filefd;	/* ION fd for the file */
	compat_uptr_t mem;	/* mem for the PD */
	compat_int_t memlen;	/* mem length */
	compat_int_t memfd;	/* ION fd for the mem */
};

struct compat_fastrpc_ioctl_init_attrs {
	struct compat_fastrpc_ioctl_init init;
	compat_int_t attrs;	/* attributes to init process */
	compat_int_t siglen;	/* test signature file length */
};

#define FASTRPC_CONTROL_LATENCY		(1)
struct compat_fastrpc_ctrl_latency {
	compat_uint_t enable;	/* latency control enable */
	compat_uint_t latency;	/* target latency in us */
};

#define FASTRPC_CONTROL_KALLOC		(3)
struct compat_fastrpc_ctrl_kalloc {
	compat_uint_t kalloc_support; /* Remote memory allocation from kernel */
};

struct compat_fastrpc_ctrl_wakelock {
	compat_uint_t enable;	/* wakelock control enable */
};

struct compat_fastrpc_ctrl_pm {
	compat_uint_t timeout;	/* timeout(in ms) for PM to keep system awake */
};

struct compat_fastrpc_ioctl_control {
	compat_uint_t req;
	union {
		struct compat_fastrpc_ctrl_latency lp;
		struct compat_fastrpc_ctrl_kalloc kalloc;
		struct compat_fastrpc_ctrl_wakelock wp;
		struct compat_fastrpc_ctrl_pm pm;
	};
};

struct compat_fastrpc_ioctl_capability {
	/*
	 * @param[in]: DSP domain ADSP_DOMAIN_ID,
	 * SDSP_DOMAIN_ID, or CDSP_DOMAIN_ID
	 */
	compat_uint_t domain;
	/*
	 * @param[in]: One of the DSP attributes
	 * from enum remote_dsp_attributes
	 */
	compat_uint_t attribute_ID;
	/*
	 * @param[out]: Result of the DSP
	 * capability query based on attribute_ID
	 */
	compat_uint_t capability;
};

static int compat_get_fastrpc_ioctl_invoke(
			struct compat_fastrpc_ioctl_invoke_async __user *inv32,
			struct fastrpc_ioctl_invoke_async *inv,
			unsigned int cmd, unsigned int sc)
{
	compat_uint_t u = 0;
	compat_size_t s;
	compat_uptr_t p, k;
	union compat_remote_arg *pra32;
	union remote_arg *pra;
	int err = 0, len = 0, j = 0;

	len = REMOTE_SCALARS_LENGTH(sc);

	pra = (union remote_arg *)(inv + 1);
	memcpy(&inv->inv.pra, &pra, sizeof(pra));
	memcpy(&inv->inv.sc, &sc, sizeof(sc));
	err |= get_user(u, &inv32->inv.handle);
	memcpy(&inv->inv.handle, &u, sizeof(u));
	err |= get_user(p, &inv32->inv.pra);
	if (err)
		return err;
	pra32 = compat_ptr(p);
	pra = (union remote_arg *)(inv + 1);
	for (j = 0; j < len; j++) {
		err |= get_user(p, &pra32[j].buf.pv);
		memcpy((uintptr_t *)&pra[j].buf.pv, &p, sizeof(p));
		err |= get_user(s, &pra32[j].buf.len);
		memcpy(&pra[j].buf.len, &s, sizeof(s));
	}

	inv->fds = NULL;
	if (cmd != COMPAT_FASTRPC_IOCTL_INVOKE) {
		err |= get_user(p, &inv32->fds);
		memcpy(&inv->fds, &p, sizeof(p));
	}
	inv->attrs = NULL;
	if ((cmd == COMPAT_FASTRPC_IOCTL_INVOKE_ATTRS) ||
		(cmd == COMPAT_FASTRPC_IOCTL_INVOKE_CRC) ||
		(cmd == COMPAT_FASTRPC_IOCTL_INVOKE_PERF) ||
		(cmd == FASTRPC_INVOKE2_ASYNC)) {
		err |= get_user(p, &inv32->attrs);
		memcpy(&inv->attrs, &p, sizeof(p));
	}
	inv->crc = NULL;
	if ((cmd == COMPAT_FASTRPC_IOCTL_INVOKE_CRC) ||
		(cmd == COMPAT_FASTRPC_IOCTL_INVOKE_PERF)) {
		err |= get_user(p, &inv32->crc);
		memcpy(&inv->crc, &p, sizeof(p));
	}
	inv->job = NULL;
	if (cmd == FASTRPC_INVOKE2_ASYNC) {
		err |= get_user(p, &inv32->job);
		memcpy(&inv->job, &p, sizeof(p));
	}
	inv->perf_kernel = NULL;
	inv->perf_dsp = NULL;

	if ((cmd == COMPAT_FASTRPC_IOCTL_INVOKE_PERF) || (cmd == FASTRPC_INVOKE2_ASYNC)) {
		err |= get_user(k, &inv32->perf_kernel);
		err |= get_user(p, &inv32->perf_dsp);
		memcpy(&inv->perf_kernel, &k, sizeof(k));
		memcpy(&inv->perf_dsp, &p, sizeof(p));
	}
	return err;
}

static int compat_fastrpc_ioctl_invoke(struct file *filp,
		unsigned int cmd, unsigned long arg)
{
	struct compat_fastrpc_ioctl_invoke_async __user *inv32;
	struct fastrpc_ioctl_invoke_async *inv = NULL;
	compat_uint_t sc = 0;
	int err = 0, len = 0;
	struct fastrpc_file *fl = (struct fastrpc_file *)filp->private_data;

	inv32 = compat_ptr(arg);
	err = get_user(sc, &inv32->inv.sc);
	if (err)
		return err;
	len = REMOTE_SCALARS_LENGTH(sc);
	VERIFY(err, NULL != (inv = kmalloc(
		sizeof(*inv) + len * sizeof(union remote_arg), GFP_KERNEL)));
	if (err)
		return -EFAULT;

	VERIFY(err, 0 == compat_get_fastrpc_ioctl_invoke(inv32,
						inv, cmd, sc));
	if (err) {
		kfree(inv);
		return err;
	}
	VERIFY(err, 0 == (err = fastrpc_internal_invoke(fl,
						fl->mode, COMPAT_MSG, inv)));
	kfree(inv);
	return err;
}

static int compat_get_fastrpc_ioctl_invoke2(
			struct compat_fastrpc_ioctl_invoke2 __user *inv32,
			struct fastrpc_ioctl_invoke2 **inva,
			unsigned int cmd)
{
	int err = 0;
	compat_uptr_t pparam, p;
	compat_uint_t req, size, ref_size = 0;
	struct fastrpc_ioctl_invoke2 *inv2_user = NULL;
	struct fastrpc_ioctl_invoke_async *asyncinv_user;

	err = get_user(req, &inv32->req);
	err |= get_user(pparam, &inv32->invparam);
	err |= get_user(size, &inv32->size);
	if (err)
		goto bail;

	switch (req) {
	case FASTRPC_INVOKE2_ASYNC:
	{
		struct compat_fastrpc_ioctl_invoke_async __user *lasync32;
		struct compat_fastrpc_ioctl_invoke_async_no_perf __user *lasync32_old;
		compat_uint_t sc = 0;
		int len = 0;

		VERIFY(err, size <= sizeof(*lasync32));
		if (err) {
			err = -EBADE;
			goto bail;
		}
		lasync32 = compat_ptr(pparam);
		err = get_user(sc, &lasync32->inv.sc);
		if (err)
			goto bail;
		len = REMOTE_SCALARS_LENGTH(sc);
		VERIFY(err, NULL != (inv2_user = kmalloc(
				sizeof(*inv2_user) + sizeof(*asyncinv_user) +
					len * sizeof(union remote_arg), GFP_KERNEL)));
		if (err) {
			err = -EFAULT;
			goto bail;
		}
		asyncinv_user =
		(struct fastrpc_ioctl_invoke_async *)(inv2_user + 1);
		if (size < sizeof(struct compat_fastrpc_ioctl_invoke_async)) {
			lasync32_old = compat_ptr(pparam);
			VERIFY(err, 0 == compat_get_fastrpc_ioctl_invoke(lasync32,
					asyncinv_user, COMPAT_FASTRPC_IOCTL_INVOKE_CRC, sc));
			if (err)
				goto bail;

			asyncinv_user->job = NULL;
			err |= get_user(p, &lasync32_old->job);
			memcpy(&asyncinv_user->job, &p, sizeof(p));
			asyncinv_user->perf_kernel = NULL;
			asyncinv_user->perf_dsp = NULL;
		} else {
			VERIFY(err, 0 == compat_get_fastrpc_ioctl_invoke(lasync32,
							asyncinv_user, req, sc));
		}
		if (err)
			goto bail;
		memcpy(&inv2_user->req, &req, sizeof(req));
		inv2_user->invparam = (uintptr_t)asyncinv_user;
		inv2_user->size = sizeof(*asyncinv_user);
		if (err)
			goto bail;
		break;
	}
	case FASTRPC_INVOKE2_ASYNC_RESPONSE:
		ref_size = sizeof(struct compat_fastrpc_ioctl_async_response);
		VERIFY(err, size <= ref_size);
		if (err) {
			err = -EBADE;
			goto bail;
		}
		fallthrough;
	case FASTRPC_INVOKE2_KERNEL_OPTIMIZATIONS:
		if (!ref_size) {
			ref_size = sizeof(uint32_t);
			VERIFY(err, size == ref_size);
			if (err) {
				err = -EBADE;
				goto bail;
			}
		}
		fallthrough;
	case FASTRPC_INVOKE2_STATUS_NOTIF:
	{
		if (!ref_size) {
			ref_size = sizeof(struct compat_fastrpc_ioctl_notif_rsp);
			VERIFY(err, size == ref_size);
			if (err) {
				err = -EBADE;
				goto bail;
			}
		}
		VERIFY(err, NULL != (inv2_user = kmalloc(
							sizeof(*inv2_user), GFP_KERNEL)));
		if (err) {
			err = -EFAULT;
			goto bail;
		}
		memcpy(&inv2_user->req, &req, sizeof(req));
		memcpy(&inv2_user->invparam, &pparam, sizeof(pparam));
		memcpy(&inv2_user->size, &size, sizeof(size));
		if (err)
			goto bail;
		break;
	}
	default:
		err = -ENOTTY;
		break;
	}
	*inva = inv2_user;
bail:
	return err;
}

static int compat_fastrpc_ioctl_invoke2(struct file *filp,
		unsigned int cmd, unsigned long arg)
{
	struct compat_fastrpc_ioctl_invoke2 __user *inv32;
	struct fastrpc_ioctl_invoke2 *inv = NULL;
	int err = 0;
	struct fastrpc_file *fl = (struct fastrpc_file *)filp->private_data;

	inv32 = compat_ptr(arg);
	VERIFY(err, 0 == compat_get_fastrpc_ioctl_invoke2(inv32,
							&inv, cmd));
	if (err) {
		kfree(inv);
		return err;
	}

	VERIFY(err, 0 == (err = fastrpc_internal_invoke2(fl, inv, true)));
	kfree(inv);
	return err;
}

static int compat_get_fastrpc_ioctl_mem_map(
			struct compat_fastrpc_ioctl_mem_map __user *map32,
			struct fastrpc_ioctl_mem_map *map)
{
	compat_uint_t u = 0;
	compat_int_t i = 0;
	compat_size_t s = 0;
	compat_uptr_t p = 0;
	int err;

	err = get_user(i, &map32->version);
	if (err || i != 0)
		return -EINVAL;

	memcpy(&map->version, &i, sizeof(i));
	err |= get_user(i, &map32->m.fd);
	memcpy(&map->m.fd, &i, sizeof(i));
	err |= get_user(i, &map32->m.offset);
	memcpy(&map->m.offset, &i, sizeof(i));
	err |= get_user(u, &map32->m.flags);
	memcpy(&map->m.flags, &u, sizeof(u));
	err |= get_user(p, &map32->m.vaddrin);
	memcpy(&map->m.vaddrin, &s, sizeof(s));
	err |= get_user(s, &map32->m.length);
	memcpy(&map->m.length, &s, sizeof(s));
	err |= get_user(u, &map32->m.attrs);
	memcpy(&map->m.attrs, &u, sizeof(u));

	return err;
}

static int compat_put_fastrpc_ioctl_mem_map(
			struct compat_fastrpc_ioctl_mem_map __user *map32,
			struct fastrpc_ioctl_mem_map *map)
{
	compat_u64 p;
	int err = 0;

	memcpy(&p, &map->m.vaddrout, sizeof(map->m.vaddrout));
	err |= put_user(p, &map32->m.vaddrout);

	return err;
}

static int compat_get_fastrpc_ioctl_mem_unmap(
			struct compat_fastrpc_ioctl_mem_unmap __user *unmap32,
			struct fastrpc_ioctl_mem_unmap __user *unmap)
{
	compat_int_t i;
	compat_size_t s;
	compat_u64 p;
	int err;

	err = get_user(i, &unmap32->version);
	if (err || i != 0)
		return -EINVAL;

	memcpy(&unmap->version, &i, sizeof(i));
	err |= get_user(i, &unmap32->um.fd);
	memcpy(&unmap->um.fd, &i, sizeof(i));
	err |= get_user(p, &unmap32->um.vaddr);
	memcpy(&unmap->um.vaddr, &p, sizeof(p));
	err |= get_user(s, &unmap32->um.length);
	memcpy(&unmap->um.length, &s, sizeof(s));

	return err;
}

static int compat_get_fastrpc_ioctl_mmap(
			struct compat_fastrpc_ioctl_mmap __user *map32,
			struct fastrpc_ioctl_mmap __user *map)
{
	compat_uint_t u;
	compat_int_t i;
	compat_size_t s;
	compat_uptr_t p;
	int err;

	err = get_user(i, &map32->fd);
	memcpy(&map->fd, &i, sizeof(i));
	err |= get_user(u, &map32->flags);
	memcpy(&map->flags, &u, sizeof(u));
	err |= get_user(p, &map32->vaddrin);
	memcpy((uintptr_t *)&map->vaddrin, &p, sizeof(p));
	err |= get_user(s, &map32->size);
	memcpy(&map->size, &s, sizeof(s));

	return err;
}

static int compat_get_fastrpc_ioctl_mmap_64(
			struct compat_fastrpc_ioctl_mmap_64 __user *map32,
			struct fastrpc_ioctl_mmap __user *map)
{
	compat_uint_t u;
	compat_int_t i;
	compat_size_t s;
	compat_u64 p;
	int err;

	err = get_user(i, &map32->fd);
	memcpy(&map->fd, &i, sizeof(i));
	err |= get_user(u, &map32->flags);
	memcpy(&map->flags, &u, sizeof(u));
	err |= get_user(p, &map32->vaddrin);
	memcpy(&map->vaddrin, &p, sizeof(p));
	err |= get_user(s, &map32->size);
	memcpy(&map->size, &s, sizeof(s));

	return err;
}

static int compat_put_fastrpc_ioctl_mmap(
			struct compat_fastrpc_ioctl_mmap __user *map32,
			struct fastrpc_ioctl_mmap __user *map)
{
	compat_uptr_t p;
	int err = 0;

	memcpy(&p, &map->vaddrout, sizeof(p));
	err |= put_user(p, &map32->vaddrout);

	return err;
}

static int compat_put_fastrpc_ioctl_mmap_64(
			struct compat_fastrpc_ioctl_mmap_64 __user *map32,
			struct fastrpc_ioctl_mmap __user *map)
{
	compat_u64 p;
	int err = 0;

	memcpy(&p, &map->vaddrout, sizeof(map->vaddrout));
	err |= put_user(p, &map32->vaddrout);

	return err;
}

static int compat_get_fastrpc_ioctl_munmap(
			struct compat_fastrpc_ioctl_munmap __user *unmap32,
			struct fastrpc_ioctl_munmap __user *unmap)
{
	compat_uptr_t p;
	compat_size_t s;
	int err;

	err = get_user(p, &unmap32->vaddrout);
	memcpy(&unmap->vaddrout, &p, sizeof(p));
	err |= get_user(s, &unmap32->size);
	memcpy(&unmap->size, &s, sizeof(s));

	return err;
}

static int compat_get_fastrpc_ioctl_munmap_64(
			struct compat_fastrpc_ioctl_munmap_64 __user *unmap32,
			struct fastrpc_ioctl_munmap *unmap)
{
	compat_u64 p;
	compat_size_t s;
	int err;

	err = get_user(p, &unmap32->vaddrout);
	memcpy(&unmap->vaddrout, &p, sizeof(p));
	err |= get_user(s, &unmap32->size);
	memcpy(&unmap->size, &s, sizeof(s));

	return err;
}

static int compat_get_fastrpc_ioctl_control(
			struct compat_fastrpc_ioctl_control __user *ctrl32,
			struct fastrpc_ioctl_control *ctrl)
{
	compat_uptr_t p;
	int err;

	err = get_user(p, &ctrl32->req);
	memcpy(&ctrl->req, &p, sizeof(p));
	if ((p == FASTRPC_CONTROL_LATENCY) || (p == FASTRPC_CONTROL_RPC_POLL)) {
		err |= get_user(p, &ctrl32->lp.enable);
		memcpy(&ctrl->lp.enable, &p, sizeof(p));
		err |= get_user(p, &ctrl32->lp.latency);
		memcpy(&ctrl->lp.latency, &p, sizeof(p));
	} else if (p == FASTRPC_CONTROL_WAKELOCK) {
		err |= get_user(p, &ctrl32->wp.enable);
		memcpy(&ctrl->wp.enable, &p, sizeof(p));
	} else if (p == FASTRPC_CONTROL_PM) {
		err |= get_user(p, &ctrl32->pm.timeout);
		memcpy(&ctrl->pm.timeout, &p, sizeof(p));
	}

	return err;
}

static int compat_get_fastrpc_ioctl_init(
			struct compat_fastrpc_ioctl_init_attrs __user *init32,
			struct fastrpc_ioctl_init_attrs *init,
			unsigned int cmd)
{
	compat_uint_t u;
	compat_uptr_t p;
	compat_int_t i;
	int err;

	err = get_user(u, &init32->init.flags);
	memcpy(&init->init.flags, &u, sizeof(u));
	err |= get_user(p, &init32->init.file);
	memcpy(&init->init.file, &p, sizeof(p));
	err |= get_user(i, &init32->init.filelen);
	memcpy(&init->init.filelen, &i, sizeof(i));
	err |= get_user(i, &init32->init.filefd);
	memcpy(&init->init.filefd, &i, sizeof(i));
	err |= get_user(p, &init32->init.mem);
	memcpy(&init->init.mem, &p, sizeof(p));
	err |= get_user(i, &init32->init.memlen);
	memcpy(&init->init.memlen, &i, sizeof(i));
	err |= get_user(i, &init32->init.memfd);
	memcpy(&init->init.memfd, &i, sizeof(i));

	init->attrs = 0;
	if (cmd == COMPAT_FASTRPC_IOCTL_INIT_ATTRS) {
		err |= get_user(i, &init32->attrs);
		memcpy((compat_uptr_t *)&init->attrs, &i, sizeof(i));
	}

	init->siglen = 0;
	if (cmd == COMPAT_FASTRPC_IOCTL_INIT_ATTRS) {
		err |= get_user(i, &init32->siglen);
		memcpy((compat_uptr_t *)&init->siglen, &i, sizeof(i));
	}

	return err;
}

static int compat_put_fastrpc_ioctl_get_dsp_info(
	struct compat_fastrpc_ioctl_capability __user *info32,
	struct fastrpc_ioctl_capability __user *info)
{
	compat_uint_t u;
	int err = 0;

	memcpy(&u, &info->capability, sizeof(u));
	err |= put_user(u, &info32->capability);
	return err;
}

static int compat_fastrpc_control(struct fastrpc_file *fl,
		unsigned long arg)
{
	int err = 0;
	struct compat_fastrpc_ioctl_control __user *ctrl32;
	struct fastrpc_ioctl_control *ctrl = NULL;
	compat_uptr_t p;

	ctrl32 = compat_ptr(arg);
	VERIFY(err, NULL != (ctrl = kmalloc(
						sizeof(*ctrl), GFP_KERNEL)));
	if (err)
		return -EFAULT;
	VERIFY(err, 0 == compat_get_fastrpc_ioctl_control(ctrl32,
						ctrl));
	if (err)
		goto bail;
	VERIFY(err, 0 == (err = fastrpc_internal_control(fl, ctrl)));
	if (err)
		goto bail;
	err = get_user(p, &ctrl32->req);
	if (err)
		goto bail;
	if (p == FASTRPC_CONTROL_KALLOC) {
		memcpy(&p, &ctrl->kalloc.kalloc_support, sizeof(ctrl->kalloc.kalloc_support));
		err |= put_user(p, &ctrl32->kalloc.kalloc_support);
	}
bail:
	kfree(ctrl);
	return err;
}

static int compat_fastrpc_get_dsp_info(struct fastrpc_file *fl,
		unsigned long arg)
{
	struct compat_fastrpc_ioctl_capability __user *info32 = NULL;
	struct fastrpc_ioctl_capability *info = NULL;
	compat_uint_t u;
	int err = 0;
	size_t info_size = 0;

	info32 = compat_ptr(arg);
	VERIFY(err, NULL != (info = kmalloc(
				sizeof(*info), GFP_KERNEL)));
	info_size = sizeof(*info);
	if (err) {
		ADSPRPC_ERR("allocation failed for size 0x%zx\n", info_size);
		return err;
	}
	err = get_user(u, &info32->domain);
	if (err)
		goto bail;
	memcpy(&info->domain, &u, sizeof(info->domain));

	err = get_user(u, &info32->attribute_ID);
	if (err)
		goto bail;
	memcpy(&info->attribute_ID, &u, sizeof(info->attribute_ID));

	err = fastrpc_get_info_from_kernel(info, fl);
	if (err)
		goto bail;

	err = compat_put_fastrpc_ioctl_get_dsp_info(info32, info);
bail:
	kfree(info);
	return err;
}

static inline long compat_fastrpc_mmap_device_ioctl(struct fastrpc_file *fl,
		unsigned int cmd, unsigned long arg)
{
	int err = 0;

	switch (cmd) {
	case COMPAT_FASTRPC_IOCTL_MEM_MAP:
	{
		struct compat_fastrpc_ioctl_mem_map __user *map32;
		struct fastrpc_ioctl_mem_map *map = NULL;

		map32 = compat_ptr(arg);
		VERIFY(err, NULL != (map = kmalloc(
			sizeof(*map), GFP_KERNEL)));
		if (err)
			return -EFAULT;

		err = compat_get_fastrpc_ioctl_mem_map(map32, map);
		if (err) {
			kfree(map);
			return err;
		}

		VERIFY(err, 0 == (err = fastrpc_internal_mem_map(fl,
						map)));
		if (err) {
			kfree(map);
			return err;
		}
		VERIFY(err, 0 == compat_put_fastrpc_ioctl_mem_map(map32, map));
		kfree(map);
		return err;
	}
	case COMPAT_FASTRPC_IOCTL_MEM_UNMAP:
	{
		struct compat_fastrpc_ioctl_mem_unmap __user *unmap32;
		struct fastrpc_ioctl_mem_unmap *unmap = NULL;

		unmap32 = compat_ptr(arg);
		unmap = kmalloc(sizeof(*unmap), GFP_KERNEL);
		if (unmap == NULL)
			return -EFAULT;

		err = compat_get_fastrpc_ioctl_mem_unmap(unmap32, unmap);
		if (err) {
			kfree(unmap);
			return err;
		}

		VERIFY(err, 0 == (err = fastrpc_internal_mem_unmap(fl,
						unmap)));
		kfree(unmap);
		return err;
	}
	case COMPAT_FASTRPC_IOCTL_MMAP:
	{
		struct compat_fastrpc_ioctl_mmap __user *map32;
		struct fastrpc_ioctl_mmap *map = NULL;

		map32 = compat_ptr(arg);
		VERIFY(err, NULL != (map = kmalloc(
							sizeof(*map), GFP_KERNEL)));
		if (err)
			return -EFAULT;
		VERIFY(err, 0 == compat_get_fastrpc_ioctl_mmap(map32, map));
		if (err) {
			kfree(map);
			return err;
		}

		VERIFY(err, 0 == (err = fastrpc_internal_mmap(fl, map)));

		VERIFY(err, 0 == compat_put_fastrpc_ioctl_mmap(map32, map));
		kfree(map);
		return err;
	}
	case COMPAT_FASTRPC_IOCTL_MMAP_64:
	{
		struct compat_fastrpc_ioctl_mmap_64  __user *map32;
		struct fastrpc_ioctl_mmap *map = NULL;

		map32 = compat_ptr(arg);
		VERIFY(err, NULL != (map = kmalloc(
							sizeof(*map), GFP_KERNEL)));
		if (err)
			return -EFAULT;
		VERIFY(err, 0 == compat_get_fastrpc_ioctl_mmap_64(map32, map));
		if (err) {
			kfree(map);
			return err;
		}
		VERIFY(err, 0 == (err = fastrpc_internal_mmap(fl, map)));
		VERIFY(err, 0 == compat_put_fastrpc_ioctl_mmap_64(map32, map));
		kfree(map);
		return err;
	}
	case COMPAT_FASTRPC_IOCTL_MUNMAP:
	{
		struct compat_fastrpc_ioctl_munmap __user *unmap32;
		struct fastrpc_ioctl_munmap *unmap = NULL;

		unmap32 = compat_ptr(arg);
		VERIFY(err, NULL != (unmap = kmalloc(
							sizeof(*unmap), GFP_KERNEL)));
		if (err)
			return -EFAULT;
		VERIFY(err, 0 == compat_get_fastrpc_ioctl_munmap(unmap32,
							unmap));
		if (err) {
			kfree(unmap);
			return err;
		}
		VERIFY(err, 0 == (err = fastrpc_internal_munmap(fl,
							unmap)));
		kfree(unmap);
		return err;
	}
	default:
		return -ENOIOCTLCMD;
	}
}

static long compat_fastrpc_dspsignal_create(struct fastrpc_file *fl, unsigned long arg)
{
	struct fastrpc_ioctl_dspsignal_create __user *uc = compat_ptr(arg);
	struct fastrpc_ioctl_dspsignal_create c;
	int err = 0;

	err = copy_from_user(&c, uc, sizeof(c));
	if (err)
		return -EFAULT;
	return fastrpc_dspsignal_create(fl, &c);
}

static long compat_fastrpc_dspsignal_destroy(struct fastrpc_file *fl, unsigned long arg)
{
	struct fastrpc_ioctl_dspsignal_destroy __user *uc = compat_ptr(arg);
	struct fastrpc_ioctl_dspsignal_destroy c;
	int err = 0;

	err = copy_from_user(&c, uc, sizeof(c));
	if (err)
		return -EFAULT;
	return fastrpc_dspsignal_destroy(fl, &c);
}

static long compat_fastrpc_dspsignal_signal(struct fastrpc_file *fl, unsigned long arg)
{
	struct fastrpc_ioctl_dspsignal_signal __user *uc = compat_ptr(arg);
	struct fastrpc_ioctl_dspsignal_signal c;
	int err = 0;

	err = copy_from_user(&c, uc, sizeof(c));
	if (err)
		return -EFAULT;
	return fastrpc_dspsignal_signal(fl, &c);
}

static long compat_fastrpc_dspsignal_wait(struct fastrpc_file *fl, unsigned long arg)
{
	struct fastrpc_ioctl_dspsignal_wait __user *uc = compat_ptr(arg);
	struct fastrpc_ioctl_dspsignal_wait c;
	int err = 0;

	err = copy_from_user(&c, uc, sizeof(c));
	if (err)
		return -EFAULT;
	return fastrpc_dspsignal_wait(fl, &c);
}

static long compat_fastrpc_dspsignal_cancel_wait(struct fastrpc_file *fl, unsigned long arg)
{
	struct fastrpc_ioctl_dspsignal_cancel_wait __user *uc = compat_ptr(arg);
	struct fastrpc_ioctl_dspsignal_cancel_wait c;
	int err = 0;

	err = copy_from_user(&c, uc, sizeof(c));
	if (err)
		return -EFAULT;
	return fastrpc_dspsignal_cancel_wait(fl, &c);
}

long compat_fastrpc_device_ioctl(struct file *filp, unsigned int cmd,
				unsigned long arg)
{
	int err = 0;
	struct fastrpc_file *fl = (struct fastrpc_file *)filp->private_data;

	if (!filp->f_op || !filp->f_op->unlocked_ioctl)
		return -ENOTTY;

	switch (cmd) {
	case COMPAT_FASTRPC_IOCTL_INVOKE:
	case COMPAT_FASTRPC_IOCTL_INVOKE_FD:
	case COMPAT_FASTRPC_IOCTL_INVOKE_ATTRS:
	case COMPAT_FASTRPC_IOCTL_INVOKE_CRC:
	case COMPAT_FASTRPC_IOCTL_INVOKE_PERF:
	{
		return compat_fastrpc_ioctl_invoke(filp, cmd, arg);
	}
	case COMPAT_FASTRPC_IOCTL_INVOKE2:
	{
		return compat_fastrpc_ioctl_invoke2(filp, cmd, arg);
	}
	case COMPAT_FASTRPC_IOCTL_MUNMAP_64:
	{
		struct compat_fastrpc_ioctl_munmap_64 __user *unmap32;
		struct fastrpc_ioctl_munmap *unmap = NULL;

		unmap32 = compat_ptr(arg);
		VERIFY(err, NULL != (unmap = kmalloc(
							sizeof(*unmap), GFP_KERNEL)));

		if (err)
			return -EFAULT;
		VERIFY(err, 0 == compat_get_fastrpc_ioctl_munmap_64(unmap32,
							unmap));
		if (err) {
			kfree(unmap);
			return err;
		}

		VERIFY(err, 0 == (err = fastrpc_internal_munmap(fl,
							unmap)));

		kfree(unmap);
		return err;
	}
	case COMPAT_FASTRPC_IOCTL_INIT:
		fallthrough;
	case COMPAT_FASTRPC_IOCTL_INIT_ATTRS:
	{
		struct compat_fastrpc_ioctl_init_attrs __user *init32;
		struct fastrpc_ioctl_init_attrs *init = NULL;

		init32 = compat_ptr(arg);
		VERIFY(err, NULL != (init = kmalloc(
							sizeof(*init), GFP_KERNEL)));
		if (err)
			return -EFAULT;
		VERIFY(err, 0 == compat_get_fastrpc_ioctl_init(init32,
							init, cmd));
		if (err) {
			kfree(init);
			return err;
		}
		VERIFY(err, 0 == (err = fastrpc_init_process(fl, init)));

		kfree(init);
		return err;

	}
	case FASTRPC_IOCTL_GETINFO:
	{
		compat_uptr_t __user *info32;
		uint32_t *info = NULL;
		compat_uint_t u;

		info32 = compat_ptr(arg);
		VERIFY(err, NULL != (info = kmalloc(
							sizeof(*info), GFP_KERNEL)));
		if (err)
			return -EFAULT;
		err = get_user(u, info32);
		memcpy(info, &u, sizeof(u));
		if (err) {
			kfree(info);
			return err;
		}
		VERIFY(err, 0 == (err = fastrpc_get_info(fl, info)));
		memcpy(&u, info, sizeof(*info));
		err |= put_user(u, info32);
		kfree(info);
		return err;
	}
	case FASTRPC_IOCTL_SETMODE:
		return fastrpc_setmode(arg, fl);
	case COMPAT_FASTRPC_IOCTL_CONTROL:
	{
		return compat_fastrpc_control(fl, arg);
	}
	case COMPAT_FASTRPC_IOCTL_GET_DSP_INFO:
	{
		return compat_fastrpc_get_dsp_info(fl, arg);
	}
	case COMPAT_FASTRPC_IOCTL_MEM_MAP:
		fallthrough;
	case COMPAT_FASTRPC_IOCTL_MEM_UNMAP:
		fallthrough;
	case COMPAT_FASTRPC_IOCTL_MMAP:
		fallthrough;
	case COMPAT_FASTRPC_IOCTL_MMAP_64:
		fallthrough;
	case COMPAT_FASTRPC_IOCTL_MUNMAP:
		return compat_fastrpc_mmap_device_ioctl(fl, cmd, arg);
	case COMPAT_FASTRPC_IOCTL_DSPSIGNAL_CREATE:
		return compat_fastrpc_dspsignal_create(fl, arg);
	case COMPAT_FASTRPC_IOCTL_DSPSIGNAL_DESTROY:
		return compat_fastrpc_dspsignal_destroy(fl, arg);
	case COMPAT_FASTRPC_IOCTL_DSPSIGNAL_SIGNAL:
		return compat_fastrpc_dspsignal_signal(fl, arg);
	case COMPAT_FASTRPC_IOCTL_DSPSIGNAL_WAIT:
		return compat_fastrpc_dspsignal_wait(fl, arg);
	case COMPAT_FASTRPC_IOCTL_DSPSIGNAL_CANCEL_WAIT:
		return compat_fastrpc_dspsignal_cancel_wait(fl, arg);
	default:
		return -ENOTTY;
	}
}
