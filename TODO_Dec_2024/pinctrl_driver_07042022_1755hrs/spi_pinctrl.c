
#include "spi_pinctrl.h"

struct spi_pin_ctrl *myspi_pin_ctrl;

void spi_pin_writel(struct spi_pin_ctrl *mspi_pin_ctrl, u32 reg, u32 val)
{
         writel(val, mspi_pin_ctrl->base + reg);
}

u32 spi_pin_readl(struct spi_pin_ctrl *mspi_pin_ctrl, u32 reg)
{
         return readl(mspi_pin_ctrl->base + reg);
}

int read_control_module_pin_register(struct spi_pin_ctrl *mspi_pin_ctrl, pin_name_t m_pin)
{
        int ret;
        u32 pin_reg;

        switch(m_pin)
        {
                case pin_spi0_sclk:
                        pin_reg = CONF_SPI0_SCLK;
                        break;

                case pin_spi0_d0:
                        pin_reg = CONF_SPI0_D0;
                        break;

                case pin_spi0_d1:
                        pin_reg = CONF_SPI0_D1;
                        break;

                case pin_spi0_cs0:
                        pin_reg = CONF_SPI0_CS0;
                        break;

                case pin_spi0_cs1:
                        pin_reg = CONF_SPI0_CS1;
                        break;

                default:
                        pr_err("Wrong pin selected\n");
                        ret = -EINVAL;
                        goto err;
                        break;
        }

	/*Read control module pin register*/
         mspi_pin_ctrl->conf_module_reg.reg_word = spi_pin_readl(mspi_pin_ctrl, pin_reg);
	 mspi_pin_ctrl->value_read =  mspi_pin_ctrl->conf_module_reg.reg_word;

        if(!( mspi_pin_ctrl->conf_module_reg.reg_bits.pud_en)){

                pr_info("Pin - %s , %s\n",mspi_pin_ctrl->pin_names[m_pin], 
		        ((mspi_pin_ctrl->conf_module_reg.reg_bits.pu_typesel) ? \
			 "pullup enabled" : "pulldown enabled"));

        }
        else{
                pr_info("Pin - %s, pullup pulldown disabled\n", mspi_pin_ctrl->pin_names[m_pin]);
        }

        return 0;

err:
	pr_info("Error in reading spi0 pinctrl hardware registers\n");
        return ret;
}

void read_spi0_pinctrl_reg_for_debug(struct spi_pin_ctrl *mspi_pin_ctrl, u8 debug)
{
	int i_pin;
	u8 num_pins_toread = mspi_pin_ctrl->num_of_pins_to_config;

	if(debug){
		
		for(i_pin = 0; i_pin < num_pins_toread;i_pin++){ 
			read_control_module_pin_register(mspi_pin_ctrl, i_pin);
		}
	}
}

int program_internal_pullup_pulldown(struct spi_pin_ctrl *mspi_pin_ctrl, pin_name_t m_pin)
{
        u32 pin_reg;
        u32 write_val = 0;
        int ret;
	pud_mode_t mode;

        switch(m_pin)
        {
                case pin_spi0_sclk:
                        pin_reg = CONF_SPI0_SCLK;
                        break;

                case pin_spi0_d0:
                        pin_reg = CONF_SPI0_D0;
                        break;

                case pin_spi0_d1:
                        pin_reg = CONF_SPI0_D1;
                        break;

                case pin_spi0_cs0:
                        pin_reg = CONF_SPI0_CS0;
                        break;

                case pin_spi0_cs1:
                        pin_reg = CONF_SPI0_CS1;
                        break;

                default:
                        pr_err("Wrong pin selected\n");
                        ret = -EINVAL;
                        goto err;
                        break;
        }

        /*Read control module pin register*/
        mspi_pin_ctrl->conf_module_reg.reg_word = spi_pin_readl(mspi_pin_ctrl, pin_reg);
	write_val = mspi_pin_ctrl->conf_module_reg.reg_word;
	mode = mspi_pin_ctrl->pud_mode;

	if(mode == PULL_UP_ENABLE){

                /*Enable pullup or pulldown*/
                write_val &= ~CONF_PUD_EN;
                /*Select pullup*/
                write_val |= CONF_PU_TYPESEL;

        }else if(mode == PULL_DOWN_ENABLE){

                /*Enable pullup or pulldown*/
                write_val &= ~CONF_PUD_EN;
                /*Select pulldown*/
                write_val &= ~CONF_PU_TYPESEL;

        }else if( mode == PULL_UPDOWN_DISABLE){

                /*Disable pullup or pulldown*/
                write_val |= CONF_PUD_EN;
                write_val &= ~CONF_PU_TYPESEL;

        }else{
                pr_err("Wrong mode selected\n");
		ret = -EINVAL;
                goto err;
        }

        /*Write into control module pin register*/
        spi_pin_writel(mspi_pin_ctrl, pin_reg, write_val);

	return 0;

err:
        return ret;
}

int program_spi0_internal_pud_pins(struct spi_pin_ctrl* mspi_pin_ctrl)
{
	int ret;
	int i_pin;

	for(i_pin = 0; i_pin < mspi_pin_ctrl->num_of_pins_to_config; i_pin++){

		ret = program_internal_pullup_pulldown(mspi_pin_ctrl, i_pin);
        	if(ret)
                	return ret;
	}

	return 0;
}

int init_spi0_pinctrl(struct spi_pin_ctrl *mspi_pin_ctrl)
{
	int ret;

	mspi_pin_ctrl->base = ioremap(CONTROL_MODULE_BASE_REGISTER, CONTROL_MODULE_MAP_SIZE);
        if (!mspi_pin_ctrl->base)
        {
                ret = -ENOMEM;
                goto fail;
        }

	mspi_pin_ctrl->num_of_pins_to_config = (MAX_SPI0_PINS -1);

	/*Parameter to set internal pull up behaviour*/
	mspi_pin_ctrl->pud_mode = PULL_UP_ENABLE;

	mspi_pin_ctrl->pin_names[pin_spi0_sclk] = "SPI0_CLK_PIN";
	mspi_pin_ctrl->pin_names[pin_spi0_d0] = "SPI0_D0_PIN";
	mspi_pin_ctrl->pin_names[pin_spi0_d1] = "SPI0_D1_PIN";
	mspi_pin_ctrl->pin_names[pin_spi0_cs0] = "SPI0_CS0_PIN";
	mspi_pin_ctrl->pin_names[pin_spi0_cs1] = "SPI0_CS1_PIN";

	return 0;

fail:
        return ret;

}

void deinit_spi0_pinctrl(struct spi_pin_ctrl *mspi_pin_ctrl)
{
	if (mspi_pin_ctrl->base)
		iounmap(mspi_pin_ctrl->base);
}


