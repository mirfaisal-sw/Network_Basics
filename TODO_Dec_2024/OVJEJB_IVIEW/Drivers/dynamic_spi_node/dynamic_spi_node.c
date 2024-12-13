#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spi/spi.h>

static unsigned short mode;
module_param(mode, ushort, 0);

MODULE_PARM_DESC(mode, "[mode = 0] for SWDL/HDR spi node, [mode = 1] for DAB spi node,\n"
	               "[mode = 2] for Mercury spi node,  [mode = 3] for PNP DAB spi node,\n"
		       "[mode = 4] for PNP HD spi node with CS-Per-Byte");

enum chip_id {
	SWDL_OR_HDR = 0,
	DAB = 1,
	MERCURY = 2,
	PNP_DAB = 3, /* Tuner Part No. - PAIOS2-AD */
	PNP_HD_WITH_CS_PER_BYTE = 4, /* Tuner Part No. - PN3034HT */
	INVALID_PARAM = 6,
};

struct spi_contoller_config_data{

        unsigned turbo_mode:1;
        /* toggle chip select after every word */
        unsigned cs_per_word:1;
	/* mask force bit for cs per word or if protocol driver toggles CS-GPIO*/
        unsigned mask_force_bit:1;
};

static struct spi_contoller_config_data m_spi_controller_data = {
        .turbo_mode = 0,
        .cs_per_word = 1,
	.mask_force_bit = 1,
};

/* Dynamic SPI node for SWDL/HD radio*/
static struct spi_board_info spi_device_template[INVALID_PARAM] = {

	[SWDL_OR_HDR] = {
		.max_speed_hz = 1500000, /* 1.5MHz */
		.bus_num = 1,
		.chip_select = 0,
		.mode = SPI_MODE_1,
		.modalias = "spidev",
	},

	[DAB] = {
		.max_speed_hz = 20000000, /* 20MHz */
	        .bus_num = 1,
	        .chip_select = 0,
	        .mode = SPI_MODE_1,
	        .modalias = "dabplugin",
	},

	[MERCURY] = {
		.max_speed_hz = 1500000, /* 1.5MHz */
	        .bus_num = 1,
        	.chip_select = 0,
        	.mode = SPI_MODE_1,
     		.modalias = "spidev",
	},

	[PNP_DAB] = {
		.max_speed_hz = 1500000, /* 1.5MHz */
	        .bus_num = 1,
        	.chip_select = 0,
	        .mode = SPI_MODE_0,
        	.modalias = "pnp_dabplugin",
	},

	[PNP_HD_WITH_CS_PER_BYTE] = {
		.max_speed_hz = 1500000, /* 1.5MHz */
		.bus_num = 1,
		.chip_select = 0,
		.mode = SPI_MODE_0,
		.modalias = "spidev",
		.controller_data = &m_spi_controller_data,
	},

	{ },

};

static struct spi_device *spi_device;

static int spi_init(void)
{
	struct spi_master *master;
	int ret;
	struct spi_board_info spidev_info;

	switch (mode) {
	case SWDL_OR_HDR:
		spidev_info = spi_device_template[SWDL_OR_HDR];
		break;

	case DAB:
		spidev_info = spi_device_template[DAB];
		break;

	case MERCURY:
		spidev_info = spi_device_template[MERCURY];
		break;

	case PNP_DAB:
		spidev_info = spi_device_template[PNP_DAB];
		break;

	case PNP_HD_WITH_CS_PER_BYTE:
		spidev_info = spi_device_template[PNP_HD_WITH_CS_PER_BYTE];
		break;

	default:
		pr_err("module param \"mode\" not correct. Insert driver with correct mode\n");
		return -EPERM;
	}
	pr_info("bus_num:%d, mode:%d, chip_select:%d, max_speed_hz:%d =>\n",
		spidev_info.bus_num, spidev_info.mode,
		spidev_info.chip_select,
		spidev_info.max_speed_hz);

	master = spi_busnum_to_master(spidev_info.bus_num);
	if (!master)
		return -ENODEV;

	spi_device = spi_new_device(master, &spidev_info);
	if (!spi_device)
		return -ENODEV;

	ret = spi_setup(spi_device);
	if (ret)
		spi_unregister_device(spi_device);

	return ret;
}
module_init(spi_init);

static void spi_exit(void)
{
	spi_unregister_device(spi_device);
}
module_exit(spi_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vikram N <vikram.narayanarao@harman.com>");
MODULE_DESCRIPTION("Driver for dynamic SPI device node switching");
