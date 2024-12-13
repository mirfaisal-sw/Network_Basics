#
# Makefile for kernel IOC-IPC driver.
#

ccflags-y := -I$(srctree)/drivers/harman/fs3ipc_uart/fs3ipc/ -I$(srctree)/drivers/harman/fs3ipc_uart/fs3ipc/cfg/ -I$(srctree)/drivers/harman/fs3ipc_uart/orrlist/ -I$(srctree)/drivers/harman/fs3ipc_uart/vlqueue/ -I$(srctree)/drivers/harman/fs3ipc_uart/include/ -I$(srctree)/drivers/harman/fs3ipc_uart/ipc/ -I$(srctree)/drivers/harman/fs3ipc_uart/cobs/ -DLINUX_KERNEL_INC

obj-$(CONFIG_FS3IPC_UART) += fs3ipc-uart.o
fs3ipc-uart-objs :=  fs3ipc/cfg/fs3ipc_cfg.o fs3ipc/fs3ipc_app.o fs3ipc/fs3ipc_flowControl.o fs3ipc/fs3ipc_hdlc.o fs3ipc/fs3ipc_os.o fs3ipc/fs3ipc_phys_uart.o fs3ipc/fs3ipc_timer.o ipc/ipc.o ipc/ipc_cdev.o ipc/ipc_debug.o ipc/log.o orrlist/orrlist.o vlqueue/vlqueue.o cobs/cobs.o
