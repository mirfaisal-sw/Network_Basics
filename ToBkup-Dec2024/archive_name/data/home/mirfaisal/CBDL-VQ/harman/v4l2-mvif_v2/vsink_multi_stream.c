/*
 * Video-Out/Sink driver for video sink
 * that can be interfaced to other modules
 * in the kernel
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/random.h>
#include <linux/version.h>
#include <linux/kthread.h>
#include <linux/list.h>
#include <linux/dma-mapping.h>
#include <linux/videodev2.h>
#include <media/v4l2-async.h>
#include <media/v4l2-common.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/v4l2-event.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-mem2mem.h>
#include <media/videobuf2-core.h>
#include <media/videobuf2-dma-contig.h>
#include <media/videobuf2-memops.h>
#include <media/v4l2-of.h>
#include <media/videobuf2-vmalloc.h>
#include <linux/v4l2-dv-timings.h>
#include <media/v4l2-fh.h>
#include <asm/cacheflush.h>
#include <linux/ktime.h>
#include <linux/sysfs.h>
#include <linux/wait.h>
#include <linux/workqueue.h>
#include <linux/time.h>
#include "global.h"
#include "iukMVIFRoles.h"

#define VIVID_MODULE_NAME 			"VSink"
#define VSINK_CID_VSINK_CLASS   	(0x00F00000)
#define	PACKET_SIZE_ID				(1)
#define MAC_LOW_ID					(2)
#define	MAC_HIGH_ID					(3)
#define	STREAMID_LOW_ID				(4)
#define	STREAMID_HIGH_ID			(5)
#define VSINK_CID_PACKET_SIZE   	(VSINK_CID_VSINK_CLASS + PACKET_SIZE_ID)
#define VSINK_CID_MAC_LOW       	(VSINK_CID_VSINK_CLASS + MAC_LOW_ID)
#define VSINK_CID_MAC_HIGH      	(VSINK_CID_VSINK_CLASS + MAC_HIGH_ID)
#define VSINK_CID_STREAMID_LOW		(VSINK_CID_VSINK_CLASS + STREAMID_LOW_ID)
#define VSINK_CID_STREAMID_HIGH		(VSINK_CID_VSINK_CLASS + STREAMID_HIGH_ID)
#define VSINK_MAX_BUFFER_SIZE		(21 * 188)
#define VSINK_DEFAULT_PACKET_SIZE	(188)
#define MAX_NUM_BUFFER			64
#define MIN_NUM_BUFFER			8 
#define NUM_TS_PTR_ENTRIES		252
/* #define DEBUG */

#ifdef DEBUG
	#define vsink_dbg(LOG, ...) do { \
		printk("%s: %d: " LOG, "Vsink",__LINE__, ##__VA_ARGS__); \
	} while(0);
	#define vsink_dbg_r 		printk
#else
	#define vsink_dbg(LOG, ...) do {} while(0)
	#define vsink_dbg_r(LOG, ...) do {} while(0)
#endif
static DECLARE_WAIT_QUEUE_HEAD(v4l2_wq);
atomic_t flag;
/* buffer for one video frame */
struct virtvid_buffer {
    /* common v4l buffer stuff -- must be first */
    struct vb2_buffer   vb;
    struct list_head    list;
};


struct virtvid {
	unsigned			inst;
	struct v4l2_device		v4l2_dev;
	struct video_device		vid_out_dev;
	struct v4l2_format		vid_fmt;
	unsigned			num_outputs;
	u32				vid_out_caps;
	/* video output */
	struct v4l2_fract		timeperframe_vid_out;
	struct v4l2_rect		fmt_out_rect;
	struct vb2_queue		vb_vid_out_q;
	struct list_head		vid_out_active;
	struct task_struct		*kthread_vid_out;
	/* controls */
	union {
		unsigned int			ctrls[8];
		struct ctrl {
			unsigned int		packet_size;
			unsigned int		mac_low;
			unsigned int		mac_high;
			unsigned int		stream_id_low;
			unsigned int		stream_id_high;
		} s_ctrl;
	} u_ctrl;
	/* Locks */
	spinlock_t			slock;
	struct mutex			mutex;

	unsigned int			*packet_addr;
	void				*mvif_data;
	/* Flags */
	union {
		int				c_flags;
		struct {
			unsigned int		mac_low_updated : 1;
			unsigned int		mac_high_updated : 1;
			unsigned int		streamid_low_updated : 1;
			unsigned int		streamid_high_updated : 1;
		} s_ctrl_flags;
	} u_ctrl_flags;

	unsigned int			mvif_inited : 1;
	unsigned long long		packet_cnt;
};

struct virtvid *dev;
static struct video_device *vfd[2];
static unsigned long long input_packet_count;
static unsigned long long output_packet_count;
int number_of_entries;
int number_of_buffer;
struct vb2_kmalloc_buf {
    void                *vaddr;
	unsigned long        pfn_start;
	unsigned int 		size;
	atomic_t            refcount;
    struct vm_area_struct *vma;
	struct vb2_vmarea_handler   handler;
};
static const struct v4l2_queryctrl vsink_ctrls[] = {
	{
		.flags = V4L2_CTRL_FLAG_READ_ONLY | V4L2_CTRL_FLAG_WRITE_ONLY,
		.id = VSINK_CID_VSINK_CLASS,
		.name = "Vsink-Ctrl-Class",
		.type = V4L2_CTRL_TYPE_CTRL_CLASS,
	},
	{
		.flags = V4L2_CTRL_FLAG_READ_ONLY | V4L2_CTRL_FLAG_WRITE_ONLY,
		.id = VSINK_CID_PACKET_SIZE,
		.name = "packet-size",
		.minimum = 0,
		.maximum = 0x7FFFFFFF,
		.step = 0,
		.type = V4L2_CTRL_TYPE_INTEGER,
	},
	{
		.flags = V4L2_CTRL_FLAG_READ_ONLY | V4L2_CTRL_FLAG_WRITE_ONLY,
		.id = VSINK_CID_MAC_LOW,
		.name = "mac-low",
		.minimum = 0x80000000,
		.maximum = 0x7FFFFFFF,
		.step = 0,
		.type = V4L2_CTRL_TYPE_INTEGER,
	},
	{
		.flags = V4L2_CTRL_FLAG_READ_ONLY | V4L2_CTRL_FLAG_WRITE_ONLY,
		.id = VSINK_CID_MAC_HIGH,
		.name = "mac-high",
		.minimum = 0x80000000,
		.maximum = 0x7FFFFFFF,
		.step = 0,
		.type = V4L2_CTRL_TYPE_INTEGER,
	},
	{
		.flags = V4L2_CTRL_FLAG_READ_ONLY | V4L2_CTRL_FLAG_WRITE_ONLY,
		.id = VSINK_CID_STREAMID_LOW,
		.name = "stream-id-low",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.minimum = 0x80000000,
		.maximum = 0x7FFFFFFF,
		.step = 0,
	},
	{
		.flags = V4L2_CTRL_FLAG_READ_ONLY | V4L2_CTRL_FLAG_WRITE_ONLY,
		.id = VSINK_CID_STREAMID_HIGH,
		.name = "stream-id-high",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.minimum = 0x80000000,
		.maximum = 0x7FFFFFFF,
		.step = 0,
	}
};

static void vb2_kmalloc_put(void *buf_priv)
{
    struct vb2_kmalloc_buf *buf = buf_priv;
	if (atomic_dec_and_test(&buf->refcount)) {
        kfree(buf->vaddr);
        kfree(buf);
	}
}

static void *vb2_kmalloc_alloc(void *alloc_ctx, unsigned long size,
                 enum dma_data_direction dma_dir, gfp_t gfp_flags)
{
    struct vb2_kmalloc_buf *buf;
    buf = kzalloc(sizeof(*buf), GFP_KERNEL);
    if (!buf)
        return NULL;
    buf->size = size;
    buf->vaddr = kzalloc(buf->size, GFP_KERNEL | GFP_DMA);
	buf->handler.refcount = &buf->refcount;
    buf->handler.put = vb2_kmalloc_put;
    buf->handler.arg = buf;

    if (!buf->vaddr) {
        vsink_dbg("kmalloc of size %u failed\n", buf->size);
        kfree(buf);
        return NULL;
    } else {
        vsink_dbg("Allocated buffer, size %d\n",(unsigned int)buf->size);
    }
	atomic_inc(&buf->refcount);
    return buf;
}

static void *vb2_kmalloc_vaddr(void *buf_priv)
{
    struct vb2_kmalloc_buf *buf = buf_priv;

    if (!buf->vaddr) {
        vsink_dbg("Address of an unallocated plane requested "
               "or cannot map\n");
        return NULL;
    }

    return buf->vaddr;
}
static unsigned int vb2_kmalloc_num_users(void *buf_priv)
{
    struct vb2_kmalloc_buf *buf = buf_priv;
    return atomic_read(&buf->refcount);
}

static int vb2_kmalloc_mmap(void *buf_priv, struct vm_area_struct *vma)
{
    struct vb2_kmalloc_buf *buf = buf_priv;
	long length = vma->vm_end - vma->vm_start;
    int ret;

    if (!buf) {
        vsink_dbg("No memory to map\n");
        return -EINVAL;
    }

	//vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	ret = remap_pfn_range(vma, vma->vm_start,
	 virt_to_phys((void *)buf->vaddr) >> PAGE_SHIFT, length, vma->vm_page_prot);
    /*
     * Make sure that vm_areas for 2 buffers won't be merged together
     */
    vma->vm_flags       |= VM_DONTEXPAND;

    /*
     * Use common vm_area operations to track buffer refcount.
     */
    vma->vm_private_data    = &buf->handler;
    vma->vm_ops     = &vb2_common_vm_ops;
    return 0;
}

static int virt_vbridge_vid_out(void *data)
{
	struct virtvid *dev = data;
	struct virtvid_buffer *vid_out_buf = NULL;
	for (;;) {
		if (kthread_should_stop())
			break;
		if (!list_empty(&dev->vid_out_active)) {	
		       usleep_range(1400, 1450);
                       spin_lock(&dev->slock);
                       vid_out_buf = list_entry(dev->vid_out_active.next,
                               struct virtvid_buffer, list);
                       list_del(&vid_out_buf->list);
                       spin_unlock(&dev->slock);
                       if (vid_out_buf) {
                               vb2_buffer_done(&vid_out_buf->vb,VB2_BUF_STATE_DONE);
                               vid_out_buf = NULL;
                       }
		}
		else {	
			atomic_set(&flag,0);
                        wait_event_interruptible_timeout(v4l2_wq, atomic_read(&flag) == 1, jiffies_to_msecs(5));
		}	
	}
	vsink_dbg("Video out thread end\n");
	return 0;
}


/* ------------------------------------------------------------------
    Videobuf operations
   ------------------------------------------------------------------*/

static int vid_out_queue_setup(struct vb2_queue *vq, const void *parg,
		       unsigned *nbuffers, unsigned *nplanes,
		       unsigned sizes[], void *alloc_ctxs[])
{
	struct virtvid *dev = vb2_get_drv_priv(vq);
    struct v4l2_format *viddev_fmt = &dev->vid_fmt;
	unsigned size = viddev_fmt->fmt.pix.sizeimage;

	*nplanes = 1;
	sizes[0] = size;
	vsink_dbg("%s, count=%d, size=%u\n", __func__,	*nbuffers, sizes[0]);
	return 0;
}

static int vid_out_buf_prepare(struct vb2_buffer *vb)
{
	//vsink_dbg("%s\n", __func__);
	return 0;
}

static void vid_out_buf_queue(struct vb2_buffer *vb)
{
	int i=0;
	void *vbuf = NULL;
	struct virtvid *dev = vb2_get_drv_priv(vb->vb2_queue);
	struct virtvid_buffer *buf = container_of(vb, struct virtvid_buffer, vb);
	unsigned int num_packets = 0;
	unsigned int bytes_used = 0;
	unsigned int buffer_phy_base = 0;
	int freeEntries = -1;
	unsigned int *phy_addr = dev->packet_addr;
	int pack_id = 0;
	unsigned int inst_id = 0;
	static DEFINE_RATELIMIT_STATE(v4l2_rl, V4L2_RATELIMIT_INTERVAL,
                                       V4L2_RATELIMIT_BURST);
	struct mvif_ops *mvifops = (struct mvif_ops*)dev->mvif_data;
	vsink_dbg("%s: VIDIOC_QBUF buf %d plane off %08x \n",
			__func__,vb->index,vb->planes[0].m.offset);
	vsink_dbg("%-40s %d\n", "vb->v4l2_planes[0].bytesused",
			vb->planes[0].bytesused);
	vsink_dbg("%-40s %d\n", "vb->v4l2_planes[0].length",
			vb->planes[0].length);
	if (vb->planes[0].bytesused > vb->planes[0].length)
	{
		vsink_dbg("Dropping buffer %d, bytes_used > buffer_length\n",
				vb->index);
		return;
	}
	vbuf = vb2_plane_vaddr(vb, 0);
	__cpuc_flush_dcache_area(vbuf, vb->planes[0].length);
	for(i=0;i<10;i++)
		vsink_dbg_r("v4l2 :~ %x",*((char*)vbuf+i));
	vsink_dbg_r("\n");
	if (buf) {
		vbuf                    = vb2_plane_vaddr(&buf->vb, 0);
		buffer_phy_base         = (unsigned int)virt_to_phys(vbuf);
		bytes_used              = buf->vb.planes[0].bytesused;
		num_packets             = bytes_used / dev->u_ctrl.s_ctrl.packet_size;
		vsink_dbg("v4l2 :~ Post %d packets\n",num_packets);

		for (pack_id = 0; pack_id < num_packets; pack_id++)
		{
			phy_addr[pack_id] =
				buffer_phy_base + (pack_id * dev->u_ctrl.s_ctrl.packet_size);
		}
		inst_id = (dev->inst == 13) ? 0 : 1;  
		freeEntries = mvifops->push_entry(inst_id, phy_addr, num_packets);
		if (freeEntries <= 0) {
			if (0 == freeEntries )
			{
				if(___ratelimit(&v4l2_rl, "virt_vbridge_vid_out")) {
					printk(KERN_INFO "%s: V4L2_ERROR: IF 0 result pushMVIFTsPtrEntriesTalker(): \
					overwrite condition, TS packets are lost\n","MVIF");
				}
			}
			else {
				if(___ratelimit(&v4l2_rl, "virt_vbridge_vid_out")) {
					printk(KERN_INFO " %s: V4L2_ERROR:IF negative result \
					pushMVIFTsPtrEntriesTalker(): init problem \n", "MVIF");
				}
			}
		}		
		dev->packet_cnt += num_packets;
		output_packet_count += num_packets;
		spin_lock(&dev->slock);
		list_add_tail(&buf->list, &dev->vid_out_active);
		spin_unlock(&dev->slock);
		buf = NULL;
		if(!(atomic_read(&flag))) {
			atomic_set(&flag,1);
			wake_up_interruptible(&v4l2_wq);
		}
	}

}
/* vid_out_start_start streaming :
	* set MVIF state to TALKING so that avb will start reading packets from
	  MVIF
	* Start  thread to release a buffer back to user-space
*/
static int vid_out_start_streaming(struct vb2_queue *vq, unsigned count)
{
	struct virtvid *dev = vb2_get_drv_priv(vq);
	if (!dev->mvif_inited) {
		printk("MVIF uninitialized..err\n");
		return -1;
	}
	//vsink_dbg("%s\n", __func__);
	//printk(KERN_ALERT "%s:v4l2_input_pkt_count: %llu\t v4l2_out_pkt_count: %llu\n",__func__,input_packet_count,output_packet_count);
	//setup_timer(&v4l2_timer, packet_analyze_log, 0);
	//mod_timer( &v4l2_timer, jiffies + msecs_to_jiffies(v4l2_log_delay));	
	printk(KERN_INFO "Stream on : Number of Entries = %d\t Number of Buffers = %d number of TS entries = %d\n"
							  ,number_of_entries,number_of_buffer,NUM_TS_PTR_ENTRIES);
	if(number_of_entries && number_of_buffer) {
		if((number_of_entries * number_of_buffer) > NUM_TS_PTR_ENTRIES)
			return -EFBIG;
	}
	setMVIFStreamStateTalker(0, MVIF_SST_TALKING);
	dev->kthread_vid_out = kthread_run(virt_vbridge_vid_out, dev,
			"%s-vid-out", dev->v4l2_dev.name);
	return 0;
}

/* vid_out_stop_streaming :
		* Set a MVIF state to IDLE so that AVB will stop reading packets from MVIF
		* stop the buffer release kernel thread
		* Release all the queued buffers back to userspace
 */
void vid_out_stop_streaming(struct vb2_queue *vq)
{
	struct virtvid *dev = vb2_get_drv_priv(vq);
	struct virtvid_buffer *buf, *tmp;
	usleep_range(2000, 3000);
	setMVIFStreamStateTalker(0, MVIF_SST_IDLE);
	atomic_set(&flag,1);
	wake_up(&v4l2_wq);
	kthread_stop(dev->kthread_vid_out);
	printk(KERN_ALERT "%s:v4l2_input_pkt_count: %llu\t v4l2_out_pkt_count: %llu\n",__func__,input_packet_count,output_packet_count);
	//del_timer_sync(&v4l2_timer);
	output_packet_count = 0;
	input_packet_count = 0;
	spin_lock(&dev->slock);
	list_for_each_entry_safe(buf, tmp, &dev->vid_out_active, list) {
		list_del(&buf->list);
		vb2_buffer_done(&buf->vb, VB2_BUF_STATE_DONE);
	}
	spin_unlock(&dev->slock);
}

const struct vb2_ops virt_vid_out_qops = {
	.queue_setup		= vid_out_queue_setup,
	.buf_prepare		= vid_out_buf_prepare,
	.buf_queue			= vid_out_buf_queue,
	.start_streaming	= vid_out_start_streaming,
	.stop_streaming		= vid_out_stop_streaming,
	.wait_prepare		= vb2_ops_wait_prepare,
	.wait_finish		= vb2_ops_wait_finish,
};


/* ------------------------------------------------------------------
    IOCTL vidioc handling
   ------------------------------------------------------------------*/
static int vidioc_querycap(struct file *file, void  *priv,
                    struct v4l2_capability *cap)
{
	struct virtvid *dev = video_drvdata(file);

    strcpy(cap->driver, VIVID_MODULE_NAME);
    strcpy(cap->card, VIVID_MODULE_NAME);
    snprintf(cap->bus_info, sizeof(cap->bus_info), "platform:%s",
		dev->v4l2_dev.name);
	cap->device_caps  = V4L2_CAP_VIDEO_OUTPUT;
    cap->capabilities = V4L2_CAP_VIDEO_OUTPUT;
    return 0;
}


static int vidioc_g_fmt_vid_out(struct file *file, void *priv,
					struct v4l2_format *f)
{
	struct virtvid *dev = video_drvdata(file);
	struct v4l2_format *viddev_fmt = &dev->vid_fmt;
	f->fmt.pix.width        = viddev_fmt->fmt.pix.width;
	f->fmt.pix.height       = viddev_fmt->fmt.pix.height;
	f->fmt.pix.sizeimage 	= viddev_fmt->fmt.pix.sizeimage;

	return 0;
}

int vidioc_s_fmt_vid_out(struct file *file, void *priv,
            struct v4l2_format *f)
{

    struct virtvid *dev = video_drvdata(file);
	struct v4l2_format *viddev_fmt = &dev->vid_fmt;
	vsink_dbg("%s\n", __func__);
	if(f->fmt.pix.sizeimage <= VSINK_MAX_BUFFER_SIZE &&
		f->fmt.pix.sizeimage >= dev->u_ctrl.s_ctrl.packet_size)
	{
		viddev_fmt->fmt.pix.sizeimage 	= f->fmt.pix.sizeimage;
		viddev_fmt->fmt.pix.width 		= f->fmt.pix.width;
		viddev_fmt->fmt.pix.height 		= f->fmt.pix.height;
		viddev_fmt->fmt.pix.field		= f->fmt.pix.field;
		vsink_dbg("pix fmt %d X %d size = %d\n",
			f->fmt.pix.width, f->fmt.pix.height, f->fmt.pix.sizeimage);
	}
	else
		return -EFBIG;
	return 0;
}

static int vidioc_g_ext_ctrls(struct file *file, void *fh,
	struct v4l2_ext_controls *ec)
{
	struct virtvid *dev = video_drvdata(file);
	int i = 0, id = 0;

	vsink_dbg("%s\n",__func__);
	if (ec->ctrl_class != VSINK_CID_VSINK_CLASS || ec->count == 0)
		return -EINVAL;
	for (i = 0; i < ec->count; i++) {
		if (ec->controls[i].id > vsink_ctrls[(ARRAY_SIZE(vsink_ctrls)-1)].id ||
		(V4L2_CTRL_ID2CLASS(ec->controls[i].id) != VSINK_CID_VSINK_CLASS) ||
		ec->controls[i].id == VSINK_CID_VSINK_CLASS)
			return -EINVAL;
		else {
			id = ec->controls[i].id & 0x7;
			switch (id) {
				case PACKET_SIZE_ID:
					ec->controls[i].value = dev->u_ctrl.ctrls[0];
					break;
				case MAC_LOW_ID:
					ec->controls[i].value = dev->u_ctrl.ctrls[1];
					break;
				case MAC_HIGH_ID:
					ec->controls[i].value = dev->u_ctrl.ctrls[2];
					break;
				case STREAMID_LOW_ID:
					ec->controls[i].value = dev->u_ctrl.ctrls[3];
					break;
				case STREAMID_HIGH_ID:
					ec->controls[i].value = dev->u_ctrl.ctrls[4];
					break;
				default:
					break;
			}
		}
	}
	return 0;
}
static int vidioc_s_ext_ctrls(struct file *file, void *fh,
	struct v4l2_ext_controls *ec)
{
	struct virtvid *dev = video_drvdata(file);
	struct mvif_ops *mvifops = (struct mvif_ops*)dev->mvif_data;
	int i = 0;
	int id = 0;
	int ret = -EPERM;

	vsink_dbg("%s %d ctrls\n",__func__, ec->count);
	if (ec->ctrl_class != VSINK_CID_VSINK_CLASS || ec->count == 0)
		return -EINVAL;
	if (ec->controls[i].id > vsink_ctrls[(ARRAY_SIZE(vsink_ctrls)-1)].id)
		return -EINVAL;
	for (i = 0; i < ec->count; i++) {
		id = ec->controls[i].id & 0x7;
		switch (id) {
			case 0:
				vsink_dbg("Control Class, ro\n");
				break;
			case PACKET_SIZE_ID:
				if (ec->controls[i].value >= VSINK_DEFAULT_PACKET_SIZE) {
					dev->u_ctrl.s_ctrl.packet_size = ec->controls[i].value;
					vsink_dbg("Packet Size %d\n",
						dev->u_ctrl.s_ctrl.packet_size);
					mvifops->set_packet_size(dev->u_ctrl.s_ctrl.packet_size);
				} else {
					vsink_dbg("Control value lesser than default\n");
					return -EINVAL;
				}
				break;
			case MAC_LOW_ID:
				dev->u_ctrl.s_ctrl.mac_low = ec->controls[i].value;
				dev->u_ctrl_flags.s_ctrl_flags.mac_low_updated = 1;
				vsink_dbg("MAC low:%06x\n",dev->u_ctrl.s_ctrl.mac_low);
				break;
			case MAC_HIGH_ID:
				dev->u_ctrl.s_ctrl.mac_high = ec->controls[i].value;
				dev->u_ctrl_flags.s_ctrl_flags.mac_high_updated = 1;
				vsink_dbg("MAC high:%06x\n",dev->u_ctrl.s_ctrl.mac_high);
				break;
			case STREAMID_LOW_ID:
				dev->u_ctrl.s_ctrl.stream_id_low = ec->controls[i].value;
				dev->u_ctrl_flags.s_ctrl_flags.streamid_low_updated = 1;
				vsink_dbg("Stream id low:%08x\n",
					dev->u_ctrl.s_ctrl.stream_id_low);
				break;
			case STREAMID_HIGH_ID:
				dev->u_ctrl.s_ctrl.stream_id_high = ec->controls[i].value;
				dev->u_ctrl_flags.s_ctrl_flags.streamid_high_updated = 1;
				vsink_dbg("Stream id high:%08x\n",
					dev->u_ctrl.s_ctrl.stream_id_high);
				break;
			default:
				break;
		}
	}
	if (dev->u_ctrl_flags.s_ctrl_flags.mac_low_updated &&
		dev->u_ctrl_flags.s_ctrl_flags.mac_high_updated) {
		/* update mac in mvif and clear the flags */
		vsink_dbg("mac-low:0x%x mac-high:0x%x\n",
			dev->u_ctrl.s_ctrl.mac_low, dev->u_ctrl.s_ctrl.mac_high);
		dev->u_ctrl_flags.s_ctrl_flags.mac_low_updated = 0;
		dev->u_ctrl_flags.s_ctrl_flags.mac_high_updated = 0;
		ret = mvifops->update_mac(dev->u_ctrl.s_ctrl.mac_low,
			dev->u_ctrl.s_ctrl.mac_high);
	}
	if (dev->u_ctrl_flags.s_ctrl_flags.streamid_low_updated &&
		dev->u_ctrl_flags.s_ctrl_flags.streamid_high_updated) {
		/* update streamid in mvif and clear the flags */
		vsink_dbg("streamid-low:0x%x streamid-high:0x%x\n",
			dev->u_ctrl.s_ctrl.stream_id_low,
			dev->u_ctrl.s_ctrl.stream_id_high);
		dev->u_ctrl_flags.s_ctrl_flags.streamid_low_updated = 0;
		dev->u_ctrl_flags.s_ctrl_flags.streamid_high_updated = 0;
		ret = mvifops->update_streamid(dev->u_ctrl.s_ctrl.stream_id_low,
			dev->u_ctrl.s_ctrl.stream_id_high);
	}
	return ret;
}

static int vidioc_queryctrl(struct file *file, void *priv,
	struct v4l2_queryctrl *qc)
{
	int i;
	if (V4L2_CTRL_ID2CLASS(qc->id) != VSINK_CID_VSINK_CLASS) {
		if (!(qc->id | V4L2_CTRL_FLAG_NEXT_CTRL))
			return -EINVAL;
		else {
			memcpy(qc, &(vsink_ctrls[0]),sizeof(*qc));
			return 0;
		}
	}

	if(qc->id > vsink_ctrls[(ARRAY_SIZE(vsink_ctrls)-1)].id)
		return -EINVAL;

	for (i = 0; i < ARRAY_SIZE(vsink_ctrls); i++)
		if (qc->id && qc->id == vsink_ctrls[i].id) {
			memcpy(qc, &(vsink_ctrls[i]),sizeof(*qc));
			vsink_dbg("%s return qctrl[%d] %s\n",
				__func__, i, vsink_ctrls[i].name);
			return (0);
		}
	return -EINVAL;
}

static int vb2_wrapper_qbuf(struct file *file, void *priv,
		struct v4l2_buffer *p)
{

	input_packet_count += ((p->bytesused)/VSINK_DEFAULT_PACKET_SIZE);
	return(vb2_ioctl_qbuf(file,priv,p));

}
static int vb2_wrapper_reqbuffer(struct file *file, void *priv,
		struct v4l2_requestbuffers *p)
{
	number_of_buffer = p->count;
	if(p->count > MAX_NUM_BUFFER || p->count < MIN_NUM_BUFFER)
		return -ENOMEM;
	return(vb2_ioctl_reqbufs(file,priv,p));

}
static const struct v4l2_ioctl_ops virt_vid_ioctl_ops = {
	.vidioc_querycap      		= vidioc_querycap,
	.vidioc_s_fmt_vid_out   	= vidioc_s_fmt_vid_out,
	.vidioc_g_fmt_vid_out       = vidioc_g_fmt_vid_out,
	.vidioc_reqbufs       		= vb2_wrapper_reqbuffer,
	.vidioc_create_bufs   		= vb2_ioctl_create_bufs,
	.vidioc_prepare_buf   		= vb2_ioctl_prepare_buf,
	.vidioc_querybuf      		= vb2_ioctl_querybuf,
	.vidioc_qbuf                       = vb2_wrapper_qbuf,
	.vidioc_dqbuf         		= vb2_ioctl_dqbuf,
	.vidioc_streamon      		= vb2_ioctl_streamon,
	.vidioc_streamoff     		= vb2_ioctl_streamoff,
	.vidioc_subscribe_event 	= v4l2_ctrl_subscribe_event,
	.vidioc_unsubscribe_event 	= v4l2_event_unsubscribe,
	.vidioc_s_ext_ctrls      	= vidioc_s_ext_ctrls,
	.vidioc_g_ext_ctrls      	= vidioc_g_ext_ctrls,
	.vidioc_queryctrl     		= vidioc_queryctrl,
};

static const struct v4l2_file_operations vivi_fops = {
	.owner      	= THIS_MODULE,
	.open           = v4l2_fh_open,
	.release        = vb2_fop_release,
	.unlocked_ioctl = video_ioctl2, /* V4L2 ioctl handler */
	.mmap           = vb2_fop_mmap,
};


static struct video_device vivi_template = {
	.name       	= VIVID_MODULE_NAME,
	.fops           = &vivi_fops,
	.ioctl_ops  	= &virt_vid_ioctl_ops,
	.release    	= video_device_release_empty,
};

const struct vb2_mem_ops vb2_kmalloc_memops = {
	.alloc      = vb2_kmalloc_alloc,
	.put        = vb2_kmalloc_put,
	.vaddr      = vb2_kmalloc_vaddr,
	.mmap       = vb2_kmalloc_mmap,
	.num_users  = vb2_kmalloc_num_users,
};

int vsink_register_mvif(struct mvif_ops *mvifops)
{
	if (!mvifops)
		return -1;
	else if (!mvifops->set_packet_size || !mvifops->update_mac ||
	!mvifops->update_streamid || !mvifops->push_entry)
		return -1;

	dev->mvif_data = (void*)mvifops;
	dev->mvif_inited = 1;
	return 0;
}

static ssize_t vsink_show(struct device *dev,
    struct device_attribute *attr, char *buf)
{
	struct virtvid *vdev = (struct virtvid *) dev_get_drvdata(dev);
	sprintf(buf, "%s: %llu\n", "packets", vdev->packet_cnt);
	dev_info(dev, "%llu", vdev->packet_cnt);
	return strlen(buf);
}

static DEVICE_ATTR(packet_cnt, S_IRUSR, vsink_show, NULL);

static struct attribute *vsink_attrs[] = {
    &dev_attr_packet_cnt.attr,
    NULL,
};

static const struct attribute_group vsink_attr_group = {
    .attrs = vsink_attrs,
};

int vsink_multi_stream_init_module(unsigned int inst_id)
{
    static int i = 0;
    struct v4l2_format *viddev_fmt;
    struct vb2_queue *q;
    int ret;

    dev = (struct virtvid *) kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;
	dev->u_ctrl.s_ctrl.packet_size = VSINK_DEFAULT_PACKET_SIZE;
	dev->packet_addr =
				(unsigned int*) kzalloc((sizeof(unsigned int)) *
				((VSINK_MAX_BUFFER_SIZE / dev->u_ctrl.s_ctrl.packet_size)+1),
				GFP_KERNEL);
	if(!dev->packet_addr) {
		kfree(dev);
		printk("Memory alloc failed %s\n", "dev->packet_addr");
		return -ENOMEM;
	}
	dev->inst = inst_id;
	snprintf(dev->v4l2_dev.name, sizeof(dev->v4l2_dev.name),
			"%s-%03d", VIVID_MODULE_NAME, 1);
	printk(KERN_INFO "V4L2 driver info : Version V2\nRegistering v4l2 /dev/video%u device\n",dev->inst);
	ret = v4l2_device_register(NULL, &dev->v4l2_dev);
	if (ret)
		goto free_dev;

	viddev_fmt = &dev->vid_fmt;
    viddev_fmt->fmt.pix.width = 64;
    viddev_fmt->fmt.pix.height = 64;
	viddev_fmt->fmt.pix.sizeimage =
		viddev_fmt->fmt.pix.width * viddev_fmt->fmt.pix.height;
    //viddev_fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    viddev_fmt->fmt.pix.field   = V4L2_FIELD_NONE;

	dev->num_outputs = 1;
	dev->vid_out_caps = V4L2_CAP_VIDEO_OUTPUT;

	/* initialize locks */
	spin_lock_init(&dev->slock);
	mutex_init(&dev->mutex);
	INIT_LIST_HEAD(&dev->vid_out_active);

	/* initialize vid_out queue */
	q = &dev->vb_vid_out_q;
	q->type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	q->io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF | VB2_WRITE;
	q->drv_priv = dev;
	q->buf_struct_size = sizeof(struct virtvid_buffer);
	q->ops = &virt_vid_out_qops;
	//q->mem_ops = &vb2_vmalloc_memops;
	q->mem_ops = &vb2_kmalloc_memops;
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
    q->gfp_flags = GFP_DMA | __GFP_COLD;
	//q->timestamp_type = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->lock = &dev->mutex;

	ret = vb2_queue_init(q);
	if (ret)
		goto unreg_dev;

	vfd[i] = &dev->vid_out_dev;
	*vfd[i] = vivi_template;
	//vfd[i]->debug = 0;
	vfd[i]->v4l2_dev = &dev->v4l2_dev;
	vfd[i]->vfl_dir = VFL_DIR_TX;
	vfd[i]->queue = q;
	vfd[i]->lock = &dev->mutex;
	video_set_drvdata(vfd[i], dev);
	ret = video_register_device(vfd[i], VFL_TYPE_GRABBER, dev->inst);
    if (ret < 0)
        goto unreg_dev;
	dev_info(&vfd[i]->dev, "V4L2 device registered\n");
	atomic_set(&flag,1);
	ret = sysfs_create_group(&vfd[i]->dev.kobj, &vsink_attr_group);
    if (ret)
        dev_err(&vfd[i]->dev, "Failed to create sysfs group %d\n", ret);
	i++;
	if(i > 1)
	    i = 0;
	return 0;

unreg_dev:
    v4l2_device_unregister(&dev->v4l2_dev);
free_dev:
    kfree(dev);
    return ret;
}

void vsink_cleanup_module(void)
{
	video_unregister_device(vfd[0]);
        video_unregister_device(vfd[1]);
	v4l2_device_unregister(&dev->v4l2_dev);
	kfree(dev->packet_addr);
    kfree(dev);
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("V4l2 Video-Sink");
