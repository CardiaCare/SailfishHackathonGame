/**
 * @file kp_error.h
 * @author Alaxandr A. Lomov <lomov@cs.karelia.ru>
 * @date   20 December, 2009
 * @brief  Interface for error handling.
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
 *
 * @section DESCRIPTION
 *
 * Interface for error handling.
 * This file is part of PetrSU KP Library.
 *
 * Copyright (C) 2009 - Alexandr A. Lomov.
 */


#include "errors.h"
#include "errors_decl.h"

#include "bool.h"

#ifndef _SSLOG_ERRORS_INTERNAL_H
#define _SSLOG_ERRORS_INTERNAL_H


#ifdef	__cplusplus
extern "C" {
#endif


/******************************* Definitions **********************************/
/************************** Structs and constants *****************************/
/// @cond DOXY_INTERNAL_API

/** @brief Max length for error comment buffer. */
#define SSLOG_ERROR_MAX_COMMENT_LEN 255


/**
 * @brief Structure for error informations.
 */
typedef struct sslog_error_s {
    int code;   /**< Error code. */
    char text[SSLOG_ERROR_MAX_COMMENT_LEN + 1]; /**< Error comment, specifiers the error. */
} sslog_error_t;

/// @endcond



/******************************************************************************/
/****************************** Functions list ********************************/
/// @cond DOXY_INTERNAL_API

//TODO: make extern only for test
/**
 * @brief Sets error code and comment.
 * This function sets code and comment for the last error (#g_last_error) and for the
 * given error structure. If the given error structure is NULL, then it sets only #g_last_error.
 * @param error. Structure to store information about error (can be NULL).
 * @param error_code. Code of an error.
 * @param comment. Text that describes an error (can be NULL).
 * @return given error_code (useful for the end of function: return sslog_error_set(...)).
 */
SSLOG_EXTERN int sslog_error_set(sslog_error_t *error, int error_code, const char *comment);

//TODO: make extern only for test
/**
 * @brief Reset an error.
 * Sets code of error to SSLOG_ERROR_NO and comment to SSLOG_ERROR_TEXT_NO.
 * Function reset the last error (#g_last_error) and the given error structure.
 * If the given error structure is NULL, then only #g_last_error will be reseted.
 * @param error. Structure to reset information about error (can be NULL).
 * @return SSLOG_ERROR_NO code, useful for ending of function, if no errors (return sslog_error_reset(...)).
 */
SSLOG_EXTERN int sslog_error_reset(sslog_error_t *error);

/// @endcond

#ifdef	__cplusplus
}
#endif

#endif /* _SSLOG_ERRORS_INTERNAL_H */
