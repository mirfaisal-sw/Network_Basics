/*
* silabs_spidev.h
* Copyright (C) 2021 Harman International Ltd,
* Author: Sneha Mohan <Sneha.Mohan@harman.com>
* Created on: 30-July-2021
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License v2.0 as published by
* the Free Software Foundation
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*/

#ifndef _SILABS_SPIDEV_H
#define _SILABS_SPIDEV_H

#define SPI_IOC_BUS_LOCK                _IO(SPI_IOC_MAGIC, 6)
#define SPI_IOC_BUS_UNLOCK              _IO(SPI_IOC_MAGIC, 7)

#endif
