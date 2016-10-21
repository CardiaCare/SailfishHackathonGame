/**
 * @file    high_api.h
 * @author  Aleksandr A. Lomov <lomov@cs.karelia.ru>
 * @date    2 June, 2015
 * @brief   Implements internal (for SmartSlog using) part of high-level API.
 *
 *
 * @section LICENSE
 *
 * SmartSlog is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * SmartSlog is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SmartSlog KP Library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 *
 *
 * @section DESCRIPTION
 *
 * The high API based on entities (classes, individual, properies).
 * These entities are used as wrapper for triples.
 * All operations on entities are performed on triples on local store
 * and in smart spcae.
 *
 * This file is part of the SmartSlog project.
 *
 * Copyright (C) 2015 by PetrSU SmartSlog team.
 */


#ifndef _SSLOG_HIGH_API_INTERNAL_H
#define	_SSLOG_HIGH_API_INTERNAL_H

#include "high_api.h"

#if defined(SSLOG_EXPORT)
#define SSLOG_EXTERN __declspec(dllexport)
#elif defined (SSLOG_IMPORT)
#define SSLOG_EXTERN __declspec(dllimport)
#else
#define SSLOG_EXTERN
#endif

/// @cond DOXY_INTERNAL

#ifdef	__cplusplus
extern "C" {
#endif

/****************************** Implementations *********************************/
/***************************** Internal functions *******************************/

/**
 * @brief Gets an individual by URI from the smart space.
 * @param node. Node to work with the smart space.
 * @param uri. Uri of the individual.
 * @return new individual on success or NULL otherwise (error, not found).
 */
sslog_individual_t * sslog_node_get_individual_by_uri(sslog_node_t *node, const char *uri);


/// @endcond
//  endcond of DOXY_INTERNAL

#ifdef	__cplusplus
}
#endif

#endif	/* _SSLOG_HIGH_API_INTERNAL_H */
