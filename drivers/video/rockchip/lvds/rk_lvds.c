#include <linux/kernel.h>
#include <linux/string.h>
#include <asm/io.h>
#include <mach/io.h>
#include <linux/rk_screen.h>
#include "rk_lvds.h"

static void rk_output_lvds(rk_screen *screen)
{
        printk("%s: %x\n", __func__, m_PDN_CBG(1)|m_PD_PLL(0)|m_PDN(1)|m_OEN(0) 	\
					|m_DS(DS_10PF)|m_MSBSEL(DATA_D0_MSB) 	\
					|m_OUT_FORMAT(screen->hw_format) 		\
					|m_LCDC_SEL(FROM_LCDC0));
	LVDSWrReg(m_PDN_CBG(1)|m_PD_PLL(0)|m_PDN(1)|m_OEN(0) 	\
					|m_DS(DS_10PF)|m_MSBSEL(DATA_D0_MSB) 	\
					|m_OUT_FORMAT(screen->hw_format) 		\
					|m_LCDC_SEL(FROM_LCDC0));

        printk("%s: reg = 0x%x\n",  __func__, LVDSRdReg());
}

static void rk_output_lvttl(rk_screen *screen)
{
	LVDSWrReg(m_PD_PLL(0)|m_PD_PLL(1)|m_PDN(0)|m_OEN(1) 	\
					|m_DS(DS_10PF)|m_MSBSEL(DATA_D0_MSB) 	\
					|m_OUT_FORMAT(screen->hw_format) 		\
					|m_LCDC_SEL(FROM_LCDC0));
        printk("%s: reg = 0x%x\n",  __func__, LVDSRdReg());
}

static void rk_output_disable(void)
{
	LVDSWrReg(m_PD_PLL(0)|m_PD_PLL(0)|m_PDN(0)|m_OEN(0));
        printk("%s: reg = 0x%x\n",  __func__, LVDSRdReg());
}

static int rk_lvds_set_param(rk_screen *screen,bool enable )
{
	if(OUT_ENABLE == enable){
		switch(screen->type){
			case SCREEN_LVDS:
					rk_output_lvds(screen);
                                        
					break;
			case SCREEN_RGB:
					rk_output_lvttl(screen);
					break;
			default:
				printk("%s>>>>LVDS not support this screen type %d,power down LVDS\n",__func__,screen->type);
					rk_output_disable();
					break;
		}
	}else{
		rk_output_disable();
	}
	return 0;
}

int rk_lvds_register(rk_screen *screen)
{
	if(screen->sscreen_set == NULL)
		screen->sscreen_set = rk_lvds_set_param;

	rk_lvds_set_param(screen , OUT_ENABLE);

	return 0;
}