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
 * \file sskp_errno.h
 *
 * \brief header provides system error number for error that may occur in the Smart Space
 *        transactions.
 *
 * Author: Jussi Kiljander, VTT Technical Research Centre of Finland
 */

#include "ckpi.h"
/*
*****************************************************************************
*  MACROS
*****************************************************************************
*/

#define  SS_OK                       0

/* SSAP message format errors */
#define  SS_ERROR_SSAP_MSG_FORMAT    1
#define  SS_ERROR_TRANSACTION_TYPE   2
#define  SS_ERROR_MESSAGE_TYPE       3
#define  SS_ERROR_UNKNOWN_SS         4
#define  SS_ERROR_UNKNOWN_NODE       5
#define  SS_ERROR_TRANSACTION_FAILED 6
#define  SS_ERROR_NO_SUBSCRIBE_ID    7
#define  SS_ERROR_WRONG_SUBSCRIBE_ID 8

/* Access errors */
#define  SS_ERROR_SOCKET_OPEN        9
#define  SS_ERROR_SOCKET_SEND        10
#define  SS_ERROR_SOCKET_RECV        11
#define  SS_ERROR_RECV_TIMEOUT       12
#define  SS_ERROR_SOCKET_CLOSE       13

/*
*****************************************************************************
* EXPORTED FUNCTIONS
*****************************************************************************
*/


void ss_perror(int error);
