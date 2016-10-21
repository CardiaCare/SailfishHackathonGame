/*

  Copyright (c) 2009, VTT Technical Research Center of Finland
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
  * Neither the name of the VTT Technical Research Center of Finland nor the
  names of its contributors may be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ''AS IS'' AND ANY
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

/**
 * \file sskp_errno.c
 *
 * Author: Jussi Kiljander, VTT Technical Research Centre of Finland
 */

#include <stdio.h>
#include "sskp_errno.h"

#if DEBUG
char *ss_error_str[]= {"SS_OK",
                       "SS_ERROR_SSAP_MSG_FORMAT",
                       "SS_ERROR_TRANSACTION_TYPE",
                       "SS_ERROR_MESSAGE_TYPE",
                       "SS_ERROR_UNKNOWN_SS",
                       "SS_ERROR_UNKNOWN_NODE",
                       "SS_ERROR_TRANSACTION_FAILED",
                       "SS_ERROR_NO_SUBSCRIBE_ID",
                       "SS_ERROR_WRONG_SUBSCRIBE_ID",
                       "SS_ERROR_SOCKET_OPEN",
                       "SS_ERROR_SOCKET_SEND",
                       "SS_ERROR_SOCKET_RECV",
                       "SS_ERROR_RECV_TIMEOUT",
                       "SS_ERROR_SOCKET_CLOSE"};

void ss_perror(int error)
{
  fprintf(stderr,"%s\n", ss_error_str[error]);
}
#endif
