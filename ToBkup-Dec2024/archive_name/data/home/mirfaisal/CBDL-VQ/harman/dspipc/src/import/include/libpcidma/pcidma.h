/*!
** \file    pcidma.h
** \brief   QNX PCI-DMA library (declarations).
** \author  Dr. Bruno Achauer, EEH-B
**
** See the main (index) page for details.
**
** \note    QNX PCI-DMA library\n
**          Harman/Becker Automotive Systems GmbH\n
**          (c) Copyright 2004, 2005\n
**          All rights reserved
**
** \date    2004-04-30 Initial version.
** \date    2004-10-26 Administrative routines (init, fini, module info) added;
**                     extensible transfer option handling.
** \date    2005-04-12 Provide API to request burst transfers (on SH7780);
**                     add support for initiator inside HB FPGA device.
** \date    2005-06-03 API to request channel options, query PCI address type.
** \date    2006-01-12 Add (target independent) "lightweight" channel option.
** \date    2009-02-05 Add byte-spap transfer option, time-out variants of
**                     transfer functions.
*/

#ifndef _PCI_DMA_H
#define _PCI_DMA_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

struct sigevent;

#define PCIDMALIB_VERSION_MAJOR 1 /*!< Major version number */
#define PCIDMALIB_VERSION_MINOR 4 /*!< Minor version number */
#define PCIDMALIB_REVISION 0      /*!< Interface revision */

/*!
**  Initialize the library (for the on-chip controller only).
**
**  \param log_code  slog opcode to be used when sending messages to the
**                   system logger.  This parameter should be constructed with
**                   the _SLOG_SETCODE(major, minor) macro; cf <sys/slog.h>
**                   and/or the slog() documentation for details.
**  \param log_level highest severity level that will be actually be logged
**                   (i.e. messages of lower severity will be discarded).
**                   Recommended value for this parameter: _SLOG_NOTICE.
**
**  \returns 0 on success, or errno code.
**
**  This function (or ::pciDma_init_fpga) must be called before any
**  other library entry point!
*/
extern int
pciDma_init(unsigned log_code, unsigned log_level);

/*!
**  Initialize the library (for both the on-chip and FPGA DMA controllers).
**
**  \param log_code  slog opcode to be used when sending messages to the
**                   system logger; cf. ::pciDma_init for details.
**  \param log_level highest severity level that will be actually be logged;
**                   cf. ::pciDma_init for details.
**  \param vid       PCI vendor ID of FPGA device.
**  \param did       PCI device ID of FPGA device.
**  \param bar       PCI base address register (memory space) to use for
**                   accessing the FPGA DMA controller.
**  \param aoff      Address offset of DMA controller within FPGA device.
**
**  \returns 0 on success, or errno code.
**
**  This function (or ::pciDma_init) must be called before any
**  other library entry point!
*/
extern int
pciDma_init_fpga(unsigned log_code, unsigned log_level,
    unsigned vid, unsigned did, unsigned bar, unsigned aoff);

/*!
**  Release all resources held.
**
**  This function should be called when the library is no longer needed,
**  e.g. when the driver using the library is about to be unloaded.
*/
extern void
pciDma_fini(void);

/*!
**  Structure describing both the module and the resources maintained.
*/
typedef struct
{
        uint8_t pcidma_version_major; /*!< major version of interface */
        uint8_t pcidma_version_minor; /*!< minor version of interface */
        uint8_t pcidma_revision;      /*!< interface revision */
        uint8_t library_revision;     /*!< implementation revision. */

        const struct _dma_controller /*!< controllers present and available. */
        {
                const char* description; /*!< description of module */
                uint32_t num_channels;   /*!< number of DMA channels available */
        } * controller[0];               /*!< variable length; end: 0 || ->desc == 0 */
} pciDma_moduleInfo_t;

/*!
**  Get module information.
**
**  \returns a pointer to the module information structure.
*/
extern const pciDma_moduleInfo_t*
pciDma_moduleInfo(void);

/*!
**  Cookie (contains the internal state of a PCI-DMA channel).
*/
typedef struct _pci_dma_channel pciDma_Channel_t;

/*!
**  Callback to determine size and addresses when (re-) starting a chained DMA
**  transfer; cf. pciDma_startChain() and pciDma_transferChain() for details.
*/
typedef uint32_t (*pciDma_reload_fcn)(void*, volatile uint32_t* la, volatile uint32_t* pa);

/*!
**  FPGA controller channel address space.
*/
#define PCIDMA_FPGA_CHAN(ch) (0x100 + (ch))

/*!
**  Allocate a PCI DMA channel.
**
**  \param ch  number of channel to allocate.
**
**  \returns Pointer to channel structure, or NULL on error (code in errno).
*/
extern pciDma_Channel_t*
pciDma_alloc(unsigned ch);

/*!
**  Allocate a PCI DMA channel with (target-dependent) options.
**
**  \param ch    number of channel to allocate;
**  \param opts  options requested for channel.
**
**  \returns Pointer to channel structure, or NULL on error (code in errno).
**
**  This function allocates a DMA channel and requests some (possibly target-
**  dependent) options of the channel to be activated.
**
**  A zero value for the \c opts parameter requests no special options (this
**  is identical to ::pciDma_alloc); otherwise, bits 27..0 of the \c opts
**  parameter denote controller-specific options (cf. below) and bits 31..28
**  encode target-independent options. Currently, one common option is supported:
**  - LIGHTWEIGHT requests that no interrupt handler should be attached to the
**    channel.  Consequently, operations that can be performed on the channel
**    are restricted as follows:
**    - ::pciDma_startChain, ::pciDma_transfer and ::pciDma_transferChain
**      are not available.
**    - For ::pciDma_start the \c finished parameter must be zero.
**
**  At present, the following controller-specific options are supported:
**  - for the SH7780 and the SH7785, the transfer request mode can be set as follows:
**    - 0 denotes auto request (as with ::pciDma_alloc).
**    - 0x01xxxxxx means external request mode. In this case, bits 23..0
**      contain values for the DO, RL, AM, AL, DL and DS fields of the CHCR
**      to configure the DREQ, DACK and DRAK lines. For details, consult the
**      SH7780 Hardware Manual.
**  - for the SH7751R and the H/B FPGA device, no options are supported.
*/
extern pciDma_Channel_t*
pciDma_alloc_opts(unsigned ch, unsigned opts);

/*!
**  Values for common PCI DMA channel options:
*/
enum {
        PCIDMA_CHOPT_LIGHTWEIGHT = 0x80000000, /*!< don't attach interrupt handler */

        PCIDMA_CHOPT_common = 0xF0000000, /*!< mask for common (target-independent) options. */
};

/*!
**  Release the PCI DMA channel.
*/
extern void
pciDma_release(pciDma_Channel_t*);

/*!
**  Flags to specify PCI DMA transfer options:
*/
enum {
        PCIDMA_XFER_space = 0x0f,  /*!< direction and address space options: */
        PCIDMA_XFER_IN = 0x00,     /*!< transfer in (from PCI to local bus) */
        PCIDMA_XFER_OUT = 0x01,    /*!< transfer out (from local to PCI bus) */
        PCIDMA_XFER_MEMORY = 0x00, /*!< transfer from/to PCI memory space */
        PCIDMA_XFER_IO = 0x02,     /*!< transfer from/to PCI I/O space */

        PCIDMA_XFER_LA_mask = 0x30,         /*!< local (CPU) address options: */
        PCIDMA_XFER_LA_NO_INCREMENT = 0x10, /*!< don't increment address */
        PCIDMA_XFER_LA_DECREMENT = 0x20,    /*!< decrement address */

        PCIDMA_XFER_PA_mask = 0xc0,         /*!< PCI bus address options: */
        PCIDMA_XFER_PA_NO_INCREMENT = 0x40, /*!< don't increment address */
        PCIDMA_XFER_PA_DECREMENT = 0x80,    /*!< decrement address */

        PCIDMA_XFER_SIZE_mask = 0x700,    /*!< transfer size options: */
        PCIDMA_XFER_SIZE_DEFAULT = 0x000, /*!< use most efficient size (always supported) */
        PCIDMA_XFER_SIZE_1 = 0x100,       /*!< 1 bytes (for Jacinto peripherals) */
        PCIDMA_XFER_SIZE_4 = 0x200,       /*!< 4 bytes (always supported) */
        PCIDMA_XFER_SIZE_8 = 0x300,       /*!< 8 bytes */
        PCIDMA_XFER_SIZE_16 = 0x400,      /*!< 16 bytes */
        PCIDMA_XFER_SIZE_32 = 0x500,      /*!< 32 bytes */
        PCIDMA_XFER_SIZE_64 = 0x600,      /*!< 64 bytes */
        PCIDMA_XFER_SIZE_128 = 0x700,     /*!< 128 bytes */

        PCIDMA_XFER_LB_SWAP = 0x1000, /*!< byte-swap 4-byte entities during xfer */

        PCIDMA_XFER_DEBUG_mask = 0xF0000000,      /*!< Debug Flag Mask */
        PCIDMA_XFER_DEBUG_QE = 0x10000000,        /*!< Print Event Queue Entry Registers */
        PCIDMA_XFER_DEBUG_DMAREGION = 0x20000000, /*!< Print DMA Region Access Register */

        PCIDMA_XFER_reserved = ~0x000017f3 /*!< available for future extensions. */
};

/*!
**  Check whether transfer options are valid.
**
**  \param flags   DMA transfer options.
**
**  \returns 0 if the options (as specified by the flags) are supported by the
**           channel, errno code otherwise.
*/
extern int
pciDma_checkFlags(pciDma_Channel_t*,
    int flags);

/*!
**  Determine type of PCI bus address required by the transfer functions.
**
**  \returns 0 iff a pci_dev_info.PciBaseAddress should be used for the PCI
**           bus address parameter, otherwise, use a .CpuBaseAddress.
*/
extern int
pciDma_useCpuBaseAddr(pciDma_Channel_t*);

/*!
**  Initiate a DMA transfer on the channel (explicit size and addresses).
**
**  \param flags   DMA transfer options
**  \param la      local bus address
**  \param pa      PCI bus address
**  \param cnt     number of bytes to transfer (SH7751: <= 64MB)
**  \param finished event to return from ISR after the transfer is done, or NULL.
**
**  \returns 0 on success, or errno code.
**
**  Note that the ISR is attached to the thread that has allocated the channel;
**  this implies that:
**  - the finished event might not be signalled if that thread has terminated;
**  - when using SIGEV_INTR, it will be directed to (awaited by) that thread.
*/
extern int
pciDma_start(pciDma_Channel_t*,
    int flags,
    uint32_t la,
    uint32_t pa,
    uint32_t cnt,
    const struct sigevent* finished);

/*!
**  Initiate a DMA transfer on the channel (using callback function).
**
**  \param flags   DMA transfer options
**  \param reload  callback function to determine transfer size and addresses
**  \param r_ctxt  passed to reload function
**  \param finished event to return from ISR after the transfer has finished
**                 (and the reload function returned zero), or NULL.
**
**  \returns 0 on success, or errno code.
**
**  This functions initiates a DMA transfer on the given channel, where transfer
**  addresses and sizes are specified indirectly by a callback function.  The
**  callback is invoked with r_ctxt as first argument.  It either returns:
**  - zero to indicate that there is nothing more to transfer, or
**  - the size of the next section to transfer (after writing the local and PCI
**    addresses to the la and pa pointers, respectively).
**
**  Note that the callback function will be invoked from ISR context, and the
**  following restrictions apply:
**  - is is not allowed to make any kernel calls;
**  - it should return as quickly as possible;
**  - invalid values are \e not detected (examples are alignment or transfer
**    size restrictions imposed by the hardware);
**  - the thread that has allocated the DMA channel must be still alive.
**  The library assumes that the values returned by the first invocation
**  specify the minimum alignment restrictions; subseqent invocations must
**  return not values that are less aligned.  For instance, an initial
**  transfer length of 16 followed by a length of 4 is illegal; the inverse
**  case (4 then 16) is allowed.
**
**  See the ::pciDma_start documentation of the restrictions associated with
**  the \c finished event parameter.
*/
extern int
pciDma_startChain(pciDma_Channel_t*,
    int flags,
    pciDma_reload_fcn reload, void* r_ctxt,
    const struct sigevent* finished);

/*!
**  Check current status of the channel.
**
**  \retval =0  transfer is still in progress
**  \retval >0  finished normally (successful)
**  \retval <0  abnormal termination (error or forcibly terminated).
*/
extern int
pciDma_status(pciDma_Channel_t*);

/*!
**  Forcibly terminate the current transfer on the channel.
*/
extern void
pciDma_abort(pciDma_Channel_t*);

/*!
**  Re-initialize the channel to its initial condition.
*/
extern void
pciDma_reinit(pciDma_Channel_t*);

/*!
**  Await termination of the current transfer on the channel.
**
**  \param to  number of milliseconds to wait (0: indefinitely).
**
**  \returns 0 on success (transfer (ab) normally terminated, status),
**           or errno code.
**
**  Note that this function is implemented by polling for pciDma_status().
*/
extern int
pciDma_await(pciDma_Channel_t*, unsigned to);

/*!
**  Perform a DMA transfer on the channel (explicit size and addresses).
**
**  \param flags   DMA transfer options
**  \param la      local bus address
**  \param pa      PCI bus address
**  \param cnt     number of bytes to transfer (SH7751: <= 64MB)
**
**  \returns 0 on success, or errno code.
**
**  This function \e must be called from the thread that allocated the channel!
*/
extern int
pciDma_transfer(pciDma_Channel_t*,
    int flags,
    uint32_t la,
    uint32_t pa,
    uint32_t cnt);

/*!
**  Perform a DMA transfer with time-out with time-out on the channel (explicit size and addresses).
**
**  \param flags   DMA transfer options
**  \param la      local bus address
**  \param pa      PCI bus address
**  \param cnt     number of bytes to transfer (SH7751: <= 64MB)
**  \param to      number of milliseconds to wait (0: indefinitely).
**
**  \returns 0 on success, or errno code.
**
**  This function \e must be called from the thread that allocated the channel!
**
**  \note A side effect of this function is to set a time-out on the CLOCK_REALTIME clock.
**        Another timeout that is already set for this clock will be lost.
*/
extern int
pciDma_transfer_to(pciDma_Channel_t*,
    int flags,
    uint32_t la,
    uint32_t pa,
    uint32_t cnt,
    unsigned to);

/*!
**  Perform a DMA transfer on the channel (using callback function).
**
**  \param flags   DMA transfer options
**  \param reload  callback function, or NULL
**  \param r_ctxt  passed to reload function.
**
**  \returns 0 on success, or errno code.
**
**  This function \e must be called from the thread that allocated the channel!
**  See the discussion at ::pciDma_startChain for restrictions related to the
**  callback function.
*/
extern int
pciDma_transferChain(pciDma_Channel_t*,
    int flags,
    pciDma_reload_fcn reload, void* r_ctxt);

/*!
**  Perform a timed DMA transfer with time-out on the channel (using callback function).
**
**  \param flags   DMA transfer options
**  \param reload  callback function, or NULL
**  \param r_ctxt  passed to reload function.
**  \param to      number of milliseconds to wait (0: indefinitely).
**
**  \returns 0 on success, or errno code.
**
**  This function \e must be called from the thread that allocated the channel!
**  See the discussion at ::pciDma_startChain for restrictions related to the
**  callback function.
**
**  \note A side effect of this function is to set a time-out on the CLOCK_REALTIME clock.
**        Another timeout that is already set for this clock will be lost.
*/
extern int
pciDma_transferChain_to(pciDma_Channel_t*,
    int flags,
    pciDma_reload_fcn reload, void* r_ctxt,
    unsigned to);

/*!
**  Dump the internal state of the channel to the slog.
**
**  \param msg  string to include in the log message.
**
**  This function is provided for debugging purposes.
*/
extern void
pciDma_dumpState(pciDma_Channel_t*,
    const char* msg);

#if defined(__cplusplus)
}
#endif

#endif
