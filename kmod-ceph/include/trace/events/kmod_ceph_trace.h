/*
 * Copyright (C) 2017 runsisi <runsisi@hust.edu.cn>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; only
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#undef TRACE_SYSTEM
#define TRACE_SYSTEM kmod_ceph

#if !defined(_TRACE_KMOD_CEPH_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_KMOD_CEPH_H

#include <linux/tracepoint.h>

TRACE_EVENT(
        rbd_img_request_submit,
        TP_PROTO(void *req, uint64_t offset, uint64_t length),
        TP_ARGS(req, offset, length),
        TP_STRUCT__entry(
                __field(void *, req)
                __field(uint64_t, offset)
                __field(uint64_t, length)
        ),
        TP_fast_assign(
                entry->req = req;
                entry->offset = offset;
                entry->length = length;
        ),
        TP_printk("rbd img request submit = %p, offset = %llu, length = %llu",
                __entry->req, __entry->offset, __entry->length)
);

TRACE_EVENT(
        rbd_img_request_complete,
        TP_PROTO(void *req),
        TP_ARGS(req),
        TP_STRUCT__entry(
                __field(void *, req)
        ),
        TP_fast_assign(
                entry->req = req;
        ),
        TP_printk("rbd img request complete = %p", __entry->req)
);

TRACE_EVENT(
        osd_client_submit_request,
        TP_PROTO(void *req),
        TP_ARGS(req),
        TP_STRUCT__entry(
                __field(void *, req)
        ),
        TP_fast_assign(
                entry->req = req;
        ),
        TP_printk("osd client request submit = %p", __entry->req)
);

TRACE_EVENT(
        osd_client_complete_request,
        TP_PROTO(void *req),
        TP_ARGS(req),
        TP_STRUCT__entry(
                __field(void *, req)
        ),
        TP_fast_assign(
                entry->req = req;
        ),
        TP_printk("osd client request complete = %p", __entry->req)
);

TRACE_EVENT(
        osd_client_send_request,
        TP_PROTO(uint64_t tid, bool linger),
        TP_ARGS(tid, linger),
        TP_STRUCT__entry(
                __field(uint64_t, tid)
                __field(bool, linger)
        ),
        TP_fast_assign(
                entry->tid = tid;
                entry->linger = linger;
        ),
        TP_printk("osd client request send tid = %llu, linger = %d",
                __entry->tid, __entry->linger)
);

TRACE_EVENT(
        osd_client_handle_reply,
        TP_PROTO(uint64_t tid, bool linger),
        TP_ARGS(tid, linger),
        TP_STRUCT__entry(
                __field(uint64_t, tid)
                __field(bool, linger)
        ),
        TP_fast_assign(
                entry->tid = tid;
                entry->linger = linger;
        ),
        TP_printk("osd client handle reply tid = %llu, linger = %d",
                __entry->tid, __entry->linger)
);

#endif /* _TRACE_KMOD_CEPH_H */

#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH .
#define TRACE_INCLUDE_FILE kmod_ceph_trace
/* this part must be outside protection */
#include <trace/define_trace.h>
