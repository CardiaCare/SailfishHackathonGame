/**
 * @file    debug.h
 * @author  Alaxandr A. Lomov <lomov@cs.karelia.ru>
 * @date    11 February, 2010
 * @brief   Debug macroses and defines.
 *
 *
 * @section LICENSE
 *
 * SmartSlog is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * SmartSlog library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PetrSU KP library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 *
 *
 * @section DESCRIPTION
 *
 * Contains macroses and defines to debug SmartSlog library.
 * This file is part of SmartSlog library.
 *
 */



#ifndef _SSLOG_DEBUG_H
#define	_SSLOG_DEBUG_H

#ifdef	__cplusplus
extern "C" {
#endif

/******************************************************************************/
/********************************** Defines ***********************************/
/// @cond INTERNAL_STRUCTURES

/** @defgroup DebugLevels Debug levels */
/*@{*/
#define KPLIB_DEBUG_LEVEL 10        /**< Debug level of library */
#define KPLIB_DEBUG_LEVEL_HIGH 1    /**< Highest debug level */
#define KPLIB_DEBUG_LEVEL_AMED 3    /**< Above medium debug level */
#define KPLIB_DEBUG_LEVEL_BMED 7    /**< Below medium debug level */
#define KPLIB_DEBUG_LEVEL_MED 5     /**< Medium debug level */
#define KPLIB_DEBUG_LEVEL_LOW 10    /**< Lowes debug level */
/*@}*/


#define SSLOG_DEBUG_PREFIX "\t==[SmartSlog] - "


/** @brief Debug flag */
/** Uncomment below string with #define to turn on debug mode */
//#define SSLOG_DEBUG_ON 1

//#define SSLOG_DEBUG_CODE_ON 1

#ifndef SSLOG_DEBUG_ON
#undef SSLOG_DEBUG_CODE_ON
#endif

#ifdef SSLOG_DEBUG_CODE_ON
#define SSLOG_DEBUG_CODE(code) code
#else
#define SSLOG_DEBUG_CODE(code)
#endif


//#if defined (  )
#define __func__ __FUNCTION__
//#endif

#ifdef SSLOG_DEBUG_ON
/** @brief Debug macros */
#define SSLOG_DEBUG(...) printf("%s", SSLOG_DEBUG_PREFIX); printf(__VA_ARGS__); printf(__VA_ARGS__); printf("\n"); fflush(stdout);
#define SSLOG_DEBUG_PRINT(...) printf(__VA_ARGS__);
#define SSLOG_DEBUG_FUNC(...) printf("%s%s - ", SSLOG_DEBUG_PREFIX, __func__); printf(__VA_ARGS__); printf("\n"); fflush(stdout);
#define SSLOG_DEBUG_START printf(SSLOG_DEBUG_PREFIX "%s START.\n", __func__);
#define SSLOG_DEBUG_END printf(SSLOG_DEBUG_PREFIX "%s ENDS.\n", __func__);
#else
#define SSLOG_DEBUG(...)
#define SSLOG_DEBUG_PRINT(...)
#define SSLOG_DEBUG_FUNC(...)
#define SSLOG_DEBUG_START
#define SSLOG_DEBUG_END
#endif 


/** @defgroup DebugPrefixes Defines of debug prefixes for output */
/*@{*/
/** @brief Debug prefix for output (classes.[c|h]) */
#define KPLIB_DEBUG_CLASSES_PREFIX    "-- [SmartSlog] CLASSES:"

/** @brief Debug prefix for output (ss_classes.[c|h]) */
#define KPLIB_DEBUG_SS_CLASSES_PREFIX "-- [SmartSlog] SS_CLASSES:"

/** @brief Debug prefix for output (patterns.[c|h]) */
#define KPLIB_DEBUG_PATTERNS_PREFIX    "-- [SmartSlog] PATTERNS:"

/** @brief Debug prefix for output (repository.[c|h]) */
#define KPLIB_DEBUG_REPO_PREFIX       "-- [SmartSlog] REPOSITORY:"

/** @brief Debug prefix for output (kpi_interface.[c|h]) */
#define KPLIB_DEBUG_KPIINTER_PREFIX   SSLOG_DEBUG_PREFIX"KPI_INTERFACE:"

/** @brief Debug prefix for output (subscribtion.[c|h]) */
#define KPLIB_DEBUG_SS_SBCR_PREFIX    "-- [SmartSlog] SS_SBCR:"

/** @brief Debug prefix for output (properties.[c|h]) */
#define KPLIB_DEBUG_PROP_PREFIX       "-- [SmartSlog] PROPS:"

/** @brief Debug prefix for output (ss_properties.[c|h]) */
#define KPLIB_DEBUG_SS_PROP_PREFIX    "-- [SmartSlog] SS_PROPS:"

/** @brief Debug prefix for output (property_changes.[c|h]) */
#define KPLIB_DEBUG_PROP_CH_PREFIX    "-- [SmartSlog] PROP_CHANGES:"

/** @brief Debug prefix for output (subscribtion_changes.[c|h]) */
#define KPLIB_DEBUG_SBCR_CH_PREFIX    "-- [SmartSlog] SBCR_CHANGES:"

/** @brief Debug prefix for output (utils/check_func.[c|h]) */
#define KPLIB_DEBUG_CHECKS_PREFIX     "-- [SmartSlog] CHECKS:"
	
/** @brief Debug prefix for output (ss_populate.[c|h]) */
#define KPLIB_DEBUG_POPULATE_PREFIX     "-- [SmartSlog] POPULATE:"
/*@}*/

/// @endcond

#ifdef	__cplusplus
}
#endif

#endif	/* _SSLOG_DEBUG_H */

