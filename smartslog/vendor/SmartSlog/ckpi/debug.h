/*
 * Copyright (C) 2009 Nokia Corporation
 * All rights reserved
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * Author(s): Mikko Hurskainen
 *
 * $Id$
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

#define D_MIN         0
#define D_MAJOR_ERROR 64
#define D_STATE       128
#define D_MINOR_ERROR 192
#define D_REQUEST     224
#define D_ALL         255

#define DEBUG_LEVEL  D_MINOR_ERROR


#ifdef DEBUG
#include <stdio.h>
#define D(level, x) do { if((level)<= DEBUG_LEVEL){x;} } while(0)
#else
#define D(level, x) do {} while (0)
#endif /*DEBUG*/

#endif /*__DEBUG_H__*/


