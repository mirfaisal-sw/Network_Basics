/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) Copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         orrlist.h
 * @ingroup      FS3IPC Component
 * @author       David Rogala <david.rogala@harman.com>
 *
 * Implements a ordered-linked-list with round-robin scheduling of items with
 * identical priority
 *****************************************************************************/

#ifndef ORRLIST_H
#define ORRLIST_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#ifdef LINUX_KERNEL_INC
#include <linux/types.h>
#else
#include <stdint.h>
#endif

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
#ifndef NULL
#define NULL ((void *)0)
#endif

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/
struct __orrlist_node_t {
	struct __orrlist_node_t *previous;
	struct __orrlist_node_t *next;
	struct __orrlist_node_t *adjacent;
	void *data;
	unsigned char orderValue;
	unsigned char adjacentDepth;
	unsigned char adjacentCount;
};
typedef struct __orrlist_node_t orrlist_node_t;

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/
void orrlist_init_node(orrlist_node_t *node, void *data,
		       unsigned int orderValue);
void orrlist_insert(orrlist_node_t *head, orrlist_node_t *newnode);
void orrlist_next(orrlist_node_t **node);

#ifdef __cplusplus
}
#endif

#endif /* ORRLIST_H */
