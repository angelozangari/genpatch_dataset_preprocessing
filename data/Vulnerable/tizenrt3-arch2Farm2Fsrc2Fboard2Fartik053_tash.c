/****************************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/
/****************************************************************************
 * arch/arm/src/artik053/src/artik053_tash.c
 *
 *   Copyright (C) 2010 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <tinyara/config.h>

#include <stdio.h>
#include <syslog.h>
#include <errno.h>

#include <tinyara/board.h>
#include <tinyara/fs/mtd.h>
#include <tinyara/fs/ioctl.h>
#include <chip.h>

#include "s5j_adc.h"
#include "s5j_rtc.h"
#include "s5j_mct.h"
#include "up_internal.h"

#include <apps/shell/tash.h>

#include "artik053.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: artik053_adc_setup
 *
 * Description:
 *   Initialize ADC and register the ADC driver.
 *
 ****************************************************************************/
int artik053_adc_setup(void)
{
#ifdef CONFIG_S5J_ADC
	int ret;
	struct adc_dev_s *adc;
	uint8_t chanlist[] = {
		adc_channel_0,
		adc_channel_1,
		adc_channel_2,
		adc_channel_3,
	};

	/* Get an instance of the ADC interface */
	adc = s5j_adc_initialize(chanlist, sizeof(chanlist));
	if (adc == NULL) {
		return -ENODEV;
	}

	/* Register the ADC driver at "/dev/adc0" */
	ret = adc_register("/dev/adc0", adc);
	if (ret < 0) {
		return ret;
	}
#endif /* CONFIG_S5J_ADC */

	return OK;
}

static void artik053_configure_partitions(void)
{
#if defined(CONFIG_ARTIK053_FLASH_PART)
	int partno;
	int partoffset;
	const char *parts = CONFIG_ARTIK053_FLASH_PART_LIST;
	const char *types = CONFIG_ARTIK053_FLASH_PART_TYPE;
#if defined(CONFIG_MTD_PARTITION_NAMES)
	const char *names = CONFIG_ARTIK053_FLASH_PART_NAME;
#endif
	FAR struct mtd_dev_s *mtd;
	FAR struct mtd_geometry_s geo;

	mtd = progmem_initialize();
	if (!mtd) {
		lldbg("ERROR: progmem_initialize failed\n");
		return;
	}

	if (mtd->ioctl(mtd, MTDIOC_GEOMETRY, (unsigned long)&geo) < 0) {
		lldbg("ERROR: mtd->ioctl failed\n");
		return;
	}

	partno = 0;
	partoffset = 0;

	while (*parts) {
		FAR struct mtd_dev_s *mtd_part;
		int partsize;

		partsize = strtoul(parts, NULL, 0) << 10;

		if (partsize < geo.erasesize) {
			lldbg("ERROR: Partition size is lesser than erasesize\n");
			return;
		}

		if (partsize % geo.erasesize != 0) {
			lldbg("ERROR: Partition size is not multiple of erasesize\n");
			return;
		}

		mtd_part = mtd_partition(mtd, partoffset, partsize / geo.blocksize, partno);
		partoffset += partsize / geo.blocksize;

		if (!mtd_part) {
			lldbg("ERROR: failed to create partition.\n");
			return;
		}
#if defined(CONFIG_MTD_FTL)
		if (!strncmp(types, "ftl,", 4)) {
			if (ftl_initialize(partno, mtd_part)) {
				lldbg("ERROR: failed to initialise mtd ftl errno :%d\n", errno);
			}
		} else
#endif
#if defined(CONFIG_MTD_CONFIG)
		if (!strncmp(types, "config,", 7)) {
			mtdconfig_register(mtd_part);
		} else
#endif
#if defined(CONFIG_MTD_SMART) && defined(CONFIG_FS_SMARTFS)
		if (!strncmp(types, "smartfs,", 8)) {
			char partref[4];
			snprintf(partref, sizeof(partref), "p%d", partno);
			smart_initialize(CONFIG_ARTIK053_FLASH_MINOR, mtd_part, partref);
		} else
#endif
#if defined(CONFIG_FS_ROMFS) && defined(CONFIG_FS_SMARTFS)
		if (!strncmp(types, "romfs,", 6)) {
			ftl_initialize(partno, mtd_part);
		} else

#endif
		{
		}

#if defined(CONFIG_MTD_PARTITION_NAMES)
		if (strcmp(names, "")) {
			mtd_setpartitionname(mtd_part, names);
		}

		while (*names != ',' && *names) {
			names++;
		}
		if (*names == ',') {
			names++;
		}
#endif

		while (*parts != ',' && *parts) {
			parts++;
		}
		if (*parts == ',') {
			parts++;
		}

		while (*types != ',' && *types) {
			types++;
		}
		if (*types == ',') {
			types++;
		}

		partno++;
	}
#endif /* CONFIG_ARTIK053_FLASH_PART */
}

static void scsc_wpa_ctrl_iface_init(void)
{
#ifdef CONFIG_SCSC_WLAN
	int ret;

	ret = mkfifo("/dev/wpa_ctrl_req", 666);
	if (ret != 0 && ret != -EEXIST) {
		lldbg("mkfifo error ret:%d\n", ret);
		return;
	}

	ret = mkfifo("/dev/wpa_ctrl_cfm", 666);
	if (ret != 0 && ret != -EEXIST) {
		lldbg("mkfifo error ret:%d\n", ret);
		return;
	}

	ret = mkfifo("/dev/wpa_monitor", 666);
	if (ret != 0 && ret != -EEXIST) {
		lldbg("mkfifo error ret:%d\n", ret);
		return;
	}
#endif
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: board_app_initialize
 *
 * Description:
 *   Perform architecture specific initialization
 *
 ****************************************************************************/
int board_app_initialize(void)
{
	int ret;
#if defined(CONFIG_RAMMTD) && defined(CONFIG_FS_SMARTFS)
	int bufsize = CONFIG_RAMMTD_ERASESIZE * CONFIG_ARTIK053_RAMMTD_NEBLOCKS;
	static uint8_t *rambuf;
	struct mtd_dev_s *mtd;
#endif /* CONFIG_RAMMTD */

	artik053_configure_partitions();

#ifdef CONFIG_ARTIK053_AUTOMOUNT_ROMFS
	ret = mount(CONFIG_ARTIK053_AUTOMOUNT_ROMFS_DEVNAME, CONFIG_ARTIK053_AUTOMOUNT_ROMFS_MOUNTPOINT, "romfs", 0, NULL);

	if (ret != OK) {
		lldbg("ERROR: mounting '%s'(ROMFS) failed\n",
			  CONFIG_ARTIK053_AUTOMOUNT_ROMFS_DEVNAME);
	}
#endif

#ifdef CONFIG_ARTIK053_AUTOMOUNT_USERFS
	/* Initialize and mount user partition (if we have) */
	ret = mksmartfs(CONFIG_ARTIK053_AUTOMOUNT_USERFS_DEVNAME, false);
	if (ret != OK) {
		lldbg("ERROR: mksmartfs on %s failed\n", CONFIG_ARTIK053_AUTOMOUNT_USERFS_DEVNAME);
	} else {
		ret = mount(CONFIG_ARTIK053_AUTOMOUNT_USERFS_DEVNAME, CONFIG_ARTIK053_AUTOMOUNT_USERFS_MOUNTPOINT, "smartfs", 0, NULL);
		if (ret != OK) {
			lldbg("ERROR: mounting '%s' failed\n", CONFIG_ARTIK053_AUTOMOUNT_USERFS_DEVNAME);
		}
	}
#endif /* CONFIG_ARTIK053_AUTOMOUNT_USERFS */

#ifdef CONFIG_FS_PROCFS
	/* Mount the procfs file system */
	ret = mount(NULL, ARTIK053_PROCFS_MOUNTPOINT, "procfs", 0, NULL);
	if (ret < 0) {
		lldbg("Failed to mount procfs at %s: %d\n", ARTIK053_PROCFS_MOUNTPOINT, ret);
	}
#endif

#if defined(CONFIG_RAMMTD) && defined(CONFIG_FS_SMARTFS)
	rambuf = (uint8_t *)malloc(bufsize);

	mtd = rammtd_initialize(rambuf, bufsize);
	if (!mtd) {
		lldbg("ERROR: FAILED TO CREATE RAM MTD INSTANCE\n");
		free(rambuf);
	} else {
		if (smart_initialize(CONFIG_ARTIK053_RAMMTD_DEV_NUMBER, mtd, NULL) < 0) {
			lldbg("ERROR: FAILED TO smart_initialize\n");
			free(rambuf);
		} else {
			(void)mksmartfs(CONFIG_ARTIK053_RAMMTD_DEV_POINT, false);

			ret = mount(CONFIG_ARTIK053_RAMMTD_DEV_POINT, CONFIG_ARTIK053_RAMMTD_MOUNT_POINT,
						"smartfs", 0, NULL);
			if (ret < 0) {
				lldbg("ERROR: Failed to mount the SMART volume: %d\n", errno);
				free(rambuf);
			}
		}
	}
#endif /* CONFIG_RAMMTD */

#if defined(CONFIG_RTC_DRIVER)
	{
		struct rtc_lowerhalf_s *rtclower;

		rtclower = s5j_rtc_lowerhalf();
		if (rtclower) {
			ret = rtc_initialize(0, rtclower);
			if (ret < 0) {
				lldbg("Failed to register the RTC driver: %d\n", ret);
			}
		}
	}
#endif /* CONFIG_RTC_DRIVER */

#ifdef CONFIG_TIMER
	{
		int  i;
		char path[CONFIG_PATH_MAX];

		for (i = 0; i < CONFIG_S5J_MCT_NUM; i++) {
			snprintf(path, sizeof(path), "/dev/timer%d", i);
			s5j_timer_initialize(path, i);
		}
	}
#endif

	artik053_adc_setup();

	scsc_wpa_ctrl_iface_init();

	UNUSED(ret);

	return OK;
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 184,4;184,11

