

#include <linux/types.h>

#include <linux/slab.h>
#include <asm/io.h>

/* 
 * TI DRA 604 Control module hardware address range:-
 * Start address/offset = 800h,
 * End address/offset = a34h
 * Control Module size = 565 ~= 568.
 */

#define CONTROL_MODULE_BASE_REGISTER  0x44E10000
#define CONTROL_MODULE_MAP_SIZE       568U

#define CONF_SPI0_SCLK                0x950
#define CONF_SPI0_D0                  0x954
#define CONF_SPI0_D1                  0x958
#define CONF_SPI0_CS0                 0x95C
#define CONF_SPI0_CS1                 0x960

/* Per-register bitmasks: */
#define CONF_PIN_MODE                 BIT(0)
#define CONF_PUD_EN                   BIT(3)
#define CONF_PU_TYPESEL               BIT(4)
#define CONF_RX_ACTIVE                BIT(5)
#define CONF_SLEW_CTRL                BIT(6)

#define MAX_SPI0_PINS		      5U

#define DEBUG_ON		      1U
#define DEBUG_OFF                     0U

typedef enum{
        pin_spi0_sclk = 0,
        pin_spi0_d0,
        pin_spi0_d1,
        pin_spi0_cs0,
        pin_spi0_cs1,

}pin_name_t;

typedef enum{
        PULL_UP_ENABLE = 1,
        PULL_DOWN_ENABLE,
        PULL_UPDOWN_DISABLE,

}pud_mode_t;

typedef union {

        struct conf_module_reg_bitfield
        {
                u8 pin_mode:3;    /*Bit[0:2]*/
                u8 pud_en:1;      /*Bit 3*/
                u8 pu_typesel:1;  /*Bit 4*/
                u8 rx_active:1;   /*Bit 5*/
                u8 slew_ctrl:1;
                u32 reserved_bits:25; /*Bit 7*/

        }reg_bits;

        uint32_t reg_word;

}conf_module_pin_register_t;

struct spi_pin_ctrl {
	void __iomem *base;
        conf_module_pin_register_t conf_module_reg;
	char *pin_names[MAX_SPI0_PINS];
	pud_mode_t pud_mode;
	u8 num_of_pins_to_config;
	u32 value_read;
};

extern struct spi_pin_ctrl *myspi_pin_ctrl;

extern void spi_pin_writel(struct spi_pin_ctrl *mspi_pin_ctrl, u32 reg, u32 val);
extern u32 spi_pin_readl(struct spi_pin_ctrl *mspi_pin_ctrl, u32 reg);
extern int read_control_module_pin_register(struct spi_pin_ctrl *mspi_pin_ctrl, pin_name_t m_pin);
extern void read_spi0_pinctrl_reg_for_debug(struct spi_pin_ctrl *mspi_pin_ctrl, u8 debug);
extern int program_internal_pullup_pulldown(struct spi_pin_ctrl *mspi_pin_ctrl, pin_name_t m_pin);
extern int program_spi0_internal_pud_pins(struct spi_pin_ctrl* mspi_pin_ctrl);
extern int init_spi0_pinctrl(struct spi_pin_ctrl* mspi_pin_ctrl);
extern void deinit_spi0_pinctrl(struct spi_pin_ctrl* mspi_pin_ctrl);


