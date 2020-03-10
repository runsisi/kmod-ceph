// SPDX-License-Identifier: GPL-2.0
/*
 * bus.h - the bus-specific portions of the driver model
 *
 * Copyright (c) 2001-2003 Patrick Mochel <mochel@osdl.org>
 * Copyright (c) 2004-2009 Greg Kroah-Hartman <gregkh@suse.de>
 * Copyright (c) 2008-2009 Novell Inc.
 * Copyright (c) 2012-2019 Greg Kroah-Hartman <gregkh@linuxfoundation.org>
 * Copyright (c) 2012-2019 Linux Foundation
 *
 * See Documentation/driver-api/driver-model/ for more information.
 */

#ifndef _DEVICE_BUS_H_
#define _DEVICE_BUS_H_

#define BUS_ATTR_WO(_name) \
	struct bus_attribute bus_attr_##_name = __ATTR_WO(_name)

#endif
