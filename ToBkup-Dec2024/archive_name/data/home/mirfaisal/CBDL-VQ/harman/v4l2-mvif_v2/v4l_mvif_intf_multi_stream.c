#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/slab.h>


#include "global.h"
#include "iukMVIFRoles.h"


struct mvif_ops *mvifops;

//My changes Start
enum eMVIFStreamState_t MVIFStream_CurrentState = MVIF_SST_ERROR_PARAM;
enum eMVIFStreamState_t MVIFStream_PrvState = MVIF_SST_IDLE;
//My changes end

static int set_packet_size(unsigned int x)
{
	printk("%s %d\n",__func__, x);
	return 0;
}

static int update_mac(unsigned int mac_low, unsigned int mac_high)
{
	printk("%s\n",__func__);
	printk("high:0x%x low:0x%x\n",mac_high, mac_low);

        //My changes start
        // Check, If state is Idle then Call the API and set the MAC address else return error code:: Need to be changed.
        //mvif_Set_destMacAddressTalker(pMVIFCommon, 0, mac_high, mac_low); // supplying index as 0 becoz of 1 source.
        if(MVIF_SST_IDLE != getMVIFStreamStateTalker(0))
        {
            printk("State is not idle\n");
            return -1;
        }
        setMVIFDestAddressTalker(0,mac_high,mac_low);
        // My chnages end
	return 0;
}

static int update_streamid(unsigned int streamid_low,unsigned int streamid_high)
{
	printk("%s\n",__func__);
	printk("high:0x%x low:0x%x\n",streamid_high, streamid_low);

        // My changes start
        // Check, if State is idle then call the API and set the stream ID else return error code:: Need to be changed.
	// mvif_Set_streamIdTalker(pMVIFCommon, 0, streamid_high, streamid_low); // suppplying index as 0 becoz of 1 source.
        

        if(MVIF_SST_IDLE != getMVIFStreamStateTalker(0))
            return -1;

        setMVIFStreamIdTalker(0,streamid_high,streamid_low);
        // My changes end.
        return 0;
}

int create_video_interface(unsigned int rolecfg,
	unsigned int memaddr, unsigned int memsize)
{
	printk("%s\n",__func__);
	if (-1 == createVideoIFace(rolecfg, memaddr, memsize))
	{
		printk("MVIF: can't create TV Tuner MVIF\n");
		return -1;
	}

        // My changes start
	/* idle here */
        //setMVIFStreamStateTalker(0, MVIF_SST_IDLE);
	return 0;
        // My changes end
}
int free_entries(unsigned int indx)
{
        return (get_freeentries(indx));
}
int push_entry(unsigned int indx,
	unsigned int *arrayTsPtr, unsigned int numTsPtr)
{
	int free_entries;
	/* printk("%s\n",__func__); */
	free_entries = pushMVIFTsPtrEntriesTalker(indx, arrayTsPtr, numTsPtr);
	return free_entries;
}


int init_v4l2_mvif_module(void)
{
	mvifops = (struct mvif_ops*) kzalloc(sizeof(struct mvif_ops),GFP_KERNEL);

	mvifops->set_packet_size		= set_packet_size;
	mvifops->update_mac				= update_mac;
	mvifops->update_streamid		= update_streamid;
	mvifops->create_video_interface	= create_video_interface;
	mvifops->push_entry				= push_entry;

	if ( -1 == mvifops->create_video_interface(MVIF_CFG_OWNER,
			MVIF_ADDR_TVTUNER, MVIF_SIZE_TVTUNER))
	{
		printk("MVIF: can't create TV Tuner MVIF\n");
		goto err;
	}

       //setMVIFDestAddressTalker( 0, 0x91E0, 0xF000FE00 );
       //setMVIFStreamIdTalker(0,0x91E0F000, 0xFE000006);

       //setMVIFStreamStateTalker( 0, MVIF_SST_TALKING );
       //setMVIFTsPacketSizeTalker(0,188);

	vsink_multi_stream_init_module(13);
	vsink_register_mvif(mvifops);
	vsink_multi_stream_init_module(14);
	vsink_register_mvif(mvifops);
	return 0;

err:
	kfree(mvifops);
	return -1;

}

module_init(init_v4l2_mvif_module);
void cleanup_v4l2_mvif_module(void)
{
	if(mvifops)
		kfree(mvifops);
	vsink_cleanup_module();
}
module_exit(cleanup_v4l2_mvif_module);

