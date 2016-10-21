/**
 * @file error.c
 * @author Alaksandr A. Lomov <lomov@cs.karelia.ru>
 * @date   20 December, 2009
 * @brief  Functions to work with errors.
 *
 * This file is part of SmartSlog library.
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
 * along with PetrSU KP Library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 *
 *
 * @section DESCRIPTION
 *
 * Contains functions to work with errors.
 *
 * Copyright (C) 2015 by PetrSU.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors_internal.h"
#include "errors_decl.h"
#include "util_func.h"

#include "../session_internal.h"


/******************************* Definitions **********************************/
/************************ Static and global entities **************************/
/** @brief Contains information about last error. */
static sslog_error_t g_last_error = { 0, "\0" };


/******************************* Definitions **********************************/
/***************************** Static functions *******************************/
static inline void sslog_copy_comment(char* buffer, const char *comment);


/****************************** Implementation ********************************/
/**************************** External functions ******************************/
/// @cond DOXY_EXTERNAL_API

SSLOG_EXTERN int sslog_error_get_last_code()
{
    return g_last_error.code;
}


SSLOG_EXTERN const char* sslog_error_get_last_text()
{
    return g_last_error.text;
}

/**
 * @brief Returns code of the node last error.
 * @param node. Node to get last error.
 * @return code of error.
 */
SSLOG_EXTERN int sslog_error_get_code(sslog_node_t *node)
{
    return node->last_error.code;
}


/**
 * @brief Return error message.
 *
 * @return error description.
 */
SSLOG_EXTERN const char* sslog_error_get_text(sslog_node_t *node)
{
    return node->last_error.text;
}

/// @endcond



/****************************** Implementation ********************************/
/**************************** Internal functions ******************************/
/// @cond DOXY_INTERNAL_API

int sslog_error_set(sslog_error_t *error, int error_code, const char *comment)
{
    g_last_error.code = error_code;
    sslog_copy_comment(g_last_error.text, comment);

    if (error != NULL) {
        error->code = error_code;
        sslog_copy_comment(error->text, comment);
    }

    return error_code;
}


int sslog_error_reset(sslog_error_t *error)
{
    g_last_error.code = SSLOG_ERROR_NO;
    strncpy(g_last_error.text, SSLOG_ERROR_TEXT_NO, SSLOG_ERROR_MAX_COMMENT_LEN);

    if (error != NULL) {
        error->code = SSLOG_ERROR_NO;
        strncpy(error->text, SSLOG_ERROR_TEXT_NO, SSLOG_ERROR_MAX_COMMENT_LEN);
    }

    return SSLOG_ERROR_NO;
}

/// @endcond





/****************************** Implementation ********************************/
/***************************** static functions *******************************/

/**
 * @brief Update buffer with given comment.
 * Copies comment to buffer, if the comment is null, then the buffer will be empty
 * string (first element will null-terminated char - '\0').
 * @param[in,out] buffer to copy comment.
 * @param[in] comment to copy.
 */
static inline void sslog_copy_comment(char* buffer, const char *comment)
{
    if (comment == NULL) {
        buffer[0] = '\0';
    } else {
        strncpy(buffer, comment, SSLOG_ERROR_MAX_COMMENT_LEN);
    }
}











