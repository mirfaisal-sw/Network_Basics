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
#include <stdlib.h>

#define READ			"R"
#define WRITE			"W"

int detect_mii(int skfd, char *ifname, int dev_addr, int reg);

static unsigned int atoh( char const *string )
{
	unsigned int value = 0;
	char ch = 0;
	unsigned char digit = 0;

	// loop until the string has ended
	while ( ( ch = *(string++) ) != 0 ) 
	{
		// service digits 0 - 9
		if ( ( ch >= '0' ) && ( ch <= '9' ) ) 
		{
			digit = ch - '0';
		}
		// and then lowercase a - f
		else if ( ( ch >= 'a' ) && ( ch <= 'f' ) ) 
		{
			digit = ch - 'a' + 10;
		}
		// and uppercase A - F
		else if ( ( ch >= 'A' ) && ( ch <= 'F' ) ) 
		{
			digit = ch - 'A' + 10;
		}
		// stopping where we are if an inapproprate value is found
		else 
		{
			break;
		}
		// and build the value now, preparing for the next pass
		value = (value << 4) + digit;
	}

	return value;
}

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

	if (ioctl(skfd, SIOCSMIIREG, &ifr) < 0)
	{
		fprintf(stderr, "SIOCGMIIREG on %s failed: %s\n", ifr.ifr_name,
				strerror(errno));
		return 2;
	}

	mii_val = data[3];

	printf("Write:\t Dev ID 0x%x | Reg 0x%x | Data 0x%x\n", dev_addr, data[1], data[2]);
	detect_mii(skfd, ifname, dev_addr, reg);
	return 0 ;
}

int detect_mii(int skfd, char *ifname, int dev_addr, int reg)
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

	if (ioctl(skfd, SIOCGMIIREG, &ifr) < 0)
	{
		fprintf(stderr, "SIOCGMIIREG on %s failed: %s\n", ifr.ifr_name,
				strerror(errno));
		return 2;
	}

	mii_val = data[3];
	printf("Read:\t Dev ID 0x%x | Reg 0x%x | Data 0x%x\n", dev_addr, data[1], data[3]);

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
	char *ifname, *operation = NULL;
	int retval = -1, dev_no = 0, phy_addr = 0, phy_data = 0;

	if( argc > 4 )
	{
		ifname = argv[1];
		dev_no = atoi(argv[2]);
		phy_addr = atoh(argv[3]);
		operation = argv[4];
		phy_data = atoh(argv[5]);
	}
	else 
	{
		printf("usage:\n %s  <intf>  <device addr>  [phy_add in hex] [R/W]  [Value to write in hex]\n",argv[0]);
		printf("example read:\t %s eth0 3 FA1C R 0\n", argv[0]);
		printf("example write:\t %s eth0 3 FA1C W F8\n", argv[0]);
		return -1;
	}

	if (!strcmp(operation, WRITE) || !strcmp(operation, READ))
		retval = 0;

	if (retval < 0)
	{
		printf("Error: Invalid Operation R/W\n");
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

	if (!strcmp(operation, READ))
		retval = detect_mii(skfd, ifname, dev_no, phy_addr);
	else
		retval = set_mii(skfd, ifname, dev_no, phy_addr, phy_data);

	close(skfd);

	if (retval == 2)
		printf("Could not determine status\n");

	return retval;
}
