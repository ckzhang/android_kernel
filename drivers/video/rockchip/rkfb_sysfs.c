/*
 * linux/drivers/video/rockchip/rkfb-sysfs.c
 *
 * Copyright (C) 2012 Rockchip Corporation
 * Author: yxj<yxj@rock-chips.com>
 *
 * Some code and ideas taken from 
 *drivers/video/omap2/omapfb/omapfb-sys.c
 *driver by Tomi Valkeinen.
 *
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/fb.h>
#include <linux/sysfs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <asm/div64.h>
#include "../display/screen/screen.h"
#include <linux/rk_fb.h>


static ssize_t show_screen_info(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct fb_info *fbi = dev_get_drvdata(dev);
	struct rk_lcdc_device_driver * dev_drv = 
		(struct rk_lcdc_device_driver * )fbi->par;
	rk_screen * screen = dev_drv->screen;
	int fps;
	u64 ft = (u64)(screen->upper_margin + screen->lower_margin + screen->y_res +screen->vsync_len)*
		(screen->left_margin + screen->right_margin + screen->x_res + screen->hsync_len)*
		(dev_drv->pixclock);       // one frame time ,(pico seconds)
	fps = div64_u64(1000000000000llu,ft);
	return snprintf(buf, PAGE_SIZE,"xres:%d\nyres:%d\nfps:%d\n",
		screen->x_res,screen->y_res,fps);
}

static ssize_t show_disp_info(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct fb_info *fbi = dev_get_drvdata(dev);
	struct rk_lcdc_device_driver * dev_drv = 
		(struct rk_lcdc_device_driver * )fbi->par;
	int layer_id = get_fb_layer_id(&fbi->fix);
	if(dev_drv->get_disp_info)
		dev_drv->get_disp_info(dev_drv,layer_id);
}

static ssize_t show_phys(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct fb_info *fbi = dev_get_drvdata(dev);
	return snprintf(buf, PAGE_SIZE, "0x%lx-----0x%x\n",
		fbi->fix.smem_start,fbi->fix.smem_len);
}

static ssize_t show_virt(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct fb_info *fbi = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "0x%p-----0x%x\n",
		fbi->screen_base,fbi->fix.smem_len);
}

static struct device_attribute rkfb_attrs[] = {
	__ATTR(phys_addr, S_IRUGO, show_phys, NULL),
	__ATTR(virt_addr, S_IRUGO, show_virt, NULL),
	__ATTR(disp_info, S_IRUGO, show_disp_info, NULL),
	__ATTR(screen_info, S_IRUGO, show_screen_info, NULL),
};

int rkfb_create_sysfs(struct fb_info *fbi)
{
	int r;
	int t;
	for (t = 0; t < ARRAY_SIZE(rkfb_attrs); t++)
	{
		r = device_create_file(fbi->dev,&rkfb_attrs[t]);
		if (r)
		{
			dev_err(fbi->dev, "failed to create sysfs "
					"file\n");
			return r;
		}
	}
	

	return 0;
}

void rkfb_remove_sysfs(struct rk_fb_inf *inf)
{
	int i, t;

	for (i = 0; i < inf->num_fb; i++) {
		for (t = 0; t < ARRAY_SIZE(rkfb_attrs); t++)
			device_remove_file(inf->fb[i]->dev,
					&rkfb_attrs[t]);
	}
}
