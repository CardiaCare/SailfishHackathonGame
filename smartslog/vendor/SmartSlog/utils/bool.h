/**
 * @file kp_bool.h
 * @Author Alaxandr A. Lomov <lomov@cs.karelia.ru>
 * @date   11 February, 2010
 * @brief  Define bool type.
 *
 *
 * @section LICENSE
 *
 * PetrSU KP Library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * PetrSU KP Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PetrSU KP Library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 *
 * @section DESCRIPTION
 *
 * Described bool type.
 * This file is part of PetrSU KP Library.
 *
 */


/*
 * ATTENTION:
 * _Bool is a build-in type (bool expectedly resolves to _Bool), it predefined in c99.
 * Header stdbool.h  has some macros(you can see it in the standard).
 * If your compiler is not support _Bool type,
 * or you have not stdbool.h, you can change this file to suit your needs.
 * Use bool for supportings c++.
 */

#if !(defined(WIN32) || defined (WINCE))
#include <stdbool.h>
#endif

#ifndef _KP_BOOL_H
#define	_KP_BOOL_H


#if !(defined(__bool_true_false_are_defined) || defined(__cplusplus))

/** @brief bool type.
 *
 * _Bool is a build-in type (bool expectedly resolves to _Bool), it predefined in c99.
 */
#define bool int
/** @brief true for bool type. */
#define true 1
/** @brief false for bool type. */
#define false 0

#endif

#endif	/* _KP_BOOL_H */

