#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned char u8;
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <unistd.h>

#define IGB_REGS_LEN 		555

struct ethtool_if_user_ioctl_data {
	struct ethtool_regs     *regs;
	void                    *data;
};

static struct ethtool_regs e_regs;
static u32 RegsDump[IGB_REGS_LEN] = {0};

int set_mii(int skfd, char *ifname,int dev_addr,int reg,int value)
{
	struct ifreq ifr;
	u16 *data, mii_val;
	int phy_id;

	/* Get the vitals from the interface. */
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGMIIPHY, &ifr) < 0)
	{
		fprintf(stderr, "SIOCGMIIPHY on %s failed: %s\n", ifname,
				strerror(errno));
		(void) close(skfd);
		return 2;
	}

	data = (u16 *)(&ifr.ifr_data);
	phy_id = data[0];
	phy_id = (phy_id | (dev_addr << 8));
	data[0] = phy_id;
	data[1] = reg;
	data[2] = value;
	printf("phy_id is 0x%x reg is 0x%x value is 0x%x\n", data[0],data[1],data[2]);

	if (ioctl(skfd, SIOCSMIIREG, &ifr) < 0)
	{
		fprintf(stderr, "SIOCGMIIREG on %s failed: %s\n", ifr.ifr_name,
				strerror(errno));
		return 2;
	}

	mii_val = data[3];

	return 0 ;
}

static int get_mac_reg_data(int skfd, char *ifname)
{
	struct ifreq ifr;
	struct ethtool_if_user_ioctl_data user_data = {
		&e_regs,
		RegsDump,
	};

	ifr.ifr_data = (void *)&user_data;

	/* Get the vitals from the interface. */
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFMAP, &ifr) < 0)
	{
		fprintf(stderr, "SIOCGIFMAP on %s failed: %s\n", ifname,
				strerror(errno));
		(void) close(skfd);
		return 2;
	}

	return 0;
}

int detect_mii(int skfd, char *ifname,int dev_addr, int reg, int page_no)
{
	struct ifreq ifr;
	u16 *data, mii_val;
	int phy_id;

	/* Get the vitals from the interface. */
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGMIIPHY, &ifr) < 0)
	{
		fprintf(stderr, "SIOCGMIIPHY on %s failed: %s\n", ifname,
				strerror(errno));
		(void) close(skfd);
		return 2;
	}

	data = (u16 *)(&ifr.ifr_data);
	phy_id = data[0];
	phy_id = (phy_id | (dev_addr << 8));
	data[0] = phy_id;
	data[1] = reg;
	data[2] = page_no;

	if (ioctl(skfd, SIOCGMIIREG, &ifr) < 0)
	{
		fprintf(stderr, "SIOCGMIIREG on %s failed: %s\n", ifr.ifr_name,
				strerror(errno));
		return 2;
	}

	mii_val = data[3];
	printf("phy_id\t0x%x | page\t%d | reg\t0x%x | data\t0x%x\n", data[0], data[2], data[1], data[3]);

	return 0;
}

int detect_ethtool(int skfd, char *ifname)
{
	struct ifreq ifr;
	struct ethtool_value edata;

	memset(&ifr, 0, sizeof(ifr));
	edata.cmd = ETHTOOL_GLINK;

	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name)-1);
	ifr.ifr_data = (char *) &edata;

	if (ioctl(skfd, SIOCETHTOOL, &ifr) == -1)
	{
		printf("ETHTOOL_GLINK failed: %s\n", strerror(errno));
		return 2;
	}

	return (edata.data ? 0 : 1);
}

int main(int argc, char **argv)
{
	int skfd = -1;
	char *ifname;
	int retval, index = 0;

	if( argc > 1 )
		ifname = argv[1];
	else {
		printf("usage:\n %s  <intf>  <device addr>  [phy_add in hex]  [Value to write in hex] [page number]\n",argv[0]);
		printf("example: %s eth0 3 FA1C F8 0\n",argv[0]);
		return -1;
	}

	/* Open a socket. */
	if (( skfd = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 )
	{
		printf("socket error\n");
		return -1;
	}

	retval = detect_ethtool(skfd, ifname);

	if (!retval)
	{
		printf("Link is Up\n");
	}

	retval = get_mac_reg_data(skfd, ifname);

	for (index = 0; index < IGB_REGS_LEN; index++)
	{
		printf("Reg %x | data %x\n", index, RegsDump[index]);
	}

	close(skfd);

	if (retval == 2)
		printf("Could not determine status\n");

	return retval;
}
