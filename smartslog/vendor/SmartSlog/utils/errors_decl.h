/*
 * errors_decl.h - describe errors, then used in enum and array.
 * This file is part of SmartSlog ANSI C KP Library.
 *
 * Copyright (C) 2009-2015 - SmartSlog team. All rights reserved.
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
 * along with SmartSlog KP Library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */


#ifndef _SSLOG_ERRORS_TEXT_H
#define _SSLOG_ERRORS_TEXT_H


/** @defgroup SmartSlogErrorsText Defines text of errors. */
/*@{*/

#define SSLOG_ERROR_TEXT_NO "\0"
#define SSLOG_ERROR_TEXT_UNKNOWN "Unknown error: "
#define SSLOG_ERROR_TEXT_INCORRECT_ARGUMENT "Incorrect argument: "
#define SSLOG_ERROR_TEXT_NULL_ARGUMENT "Argument is null: "
#define SSLOG_ERROR_TEXT_NOT_IMPLEMENTED "Not implemented."
#define SSLOG_ERROR_TEXT_NOT_FOUND "Not found: "

/* Memory errors */
#define SSLOG_ERROR_TEXT_OUT_OF_MEMORY "Out of memory."

/* Errors for OWL-entities */
#define SSLOG_ERROR_TEXT_INCORRECT_CLASS "Incorrect class structure: "
#define SSLOG_ERROR_TEXT_INCORRECT_ENTITY "Incorrect entity structure: "
#define SSLOG_ERROR_TEXT_INCORRECT_CLASSTYPE "Incorrect type of class: "
#define SSLOG_ERROR_TEXT_INCORRECT_RTTI "Incorrect RTTI type: "
#define SSLOG_ERROR_TEXT_INCORRECT_PROPERTY "Incorrect property structure: "
#define SSLOG_ERROR_TEXT_SS_NO_INDIVIDUAL "No individual in SS, no triples described it: "

/* Errors associated with individuals */
#define SSLOG_ERROR_TEXT_INCORRECT_INDIVIDUAL "Incorrect individual structure: "
#define SSLOG_ERROR_TEXT_INCORRECT_INDIVIDUAL_PARENTCLASS "Incorrect individual's parent class: "
#define SSLOG_ERROR_TEXT_CANT_CHANGED_SUBSCRIBED_INDIVIDUAL "Unable to changed subscribed individual manually: "

/* Errors associated with properties */
#define SSLOG_ERROR_TEXT_INCORRECT_PROPERTY_NAME "Incorrect name of property: "
#define SSLOG_ERROR_TEXT_INCORRECT_PROPERTY_ABOUT "Incorrect about field of property: "
#define SSLOG_ERROR_TEXT_INCORRECT_PROPERTY_VALUE "Incorrect property value: "
#define SSLOG_ERROR_TEXT_INCORRECT_PROPERTY_TYPE "Wrong property type: "
#define SSLOG_ERROR_TEXT_CANT_FIND_PROPERTY "No property in the class or individual: "

/* Cardinality errors */
#define SSLOG_ERROR_TEXT_MINCARDINALITY_VIOLATED "Minimal cardinality violated: "
#define SSLOG_ERROR_TEXT_MAXCARDINALITY_VIOLATED "Maximal cardinality violated: "
#define SSLOG_ERROR_TEXT_INCORRECT_CARDINALITY "Incorrect set cardinality restriction: "

/* Triples errors */
#define SSLOG_ERROR_TEXT_INCORRECT_TRIPLE_SUBJECT "Incorrect subjet of triple: "
#define SSLOG_ERROR_TEXT_INCORRECT_TRIPLE_OBJECT "Incorrect object of triple: "
#define SSLOG_ERROR_TEXT_INCORRECT_TRIPLE_PREDICATE "Incorrect predicate of triple: "

/* Subscription errors */
#define SSLOG_ERROR_TEXT_INCORRECT_SUBSCRIPTION "Incorrect subscription: "
#define SSLOG_ERROR_TEXT_SUBSCRIPTION_IS_INACTIVE "Inactive subscription: "
#define SSLOG_ERROR_TEXT_SUBSCRIPTION_IS_ACTIVE "Subscription is active. "
#define SSLOG_ERROR_TEXT_INCORRECT_SUBSCRIPTION_TYPE "Incorrect type or type is not suported. "
#define SSLOG_ERROR_TEXT_CANT_SUBSCRIBE "Can't subscribe: "
#define SSLOG_ERROR_TEXT_CANT_UNSUBSCRIBE "Can't unsubscribe: "

/* Network errors (throw KPI) */
//#define SSLOG_ERROR_TEXT_SOCKET_OPEN "Error occurred while opening socket."
//#define SSLOG_ERROR_TEXT_SOCKET_SEND "Error occurred while sending message."
//#define SSLOG_ERROR_TEXT_SOCKET_RECV "Error occurred while receiving message."
//#define SSLOG_ERROR_TEXT_RECV_TIMEOUT "Receive timeout."
#define SSLOG_ERROR_TEXT_SOCKET_CLOSE "Error occurred while closing socket."

/* KPI ERRORS */
#define SSLOG_ERROR_TEXT_KPI_UNKNOWN "Unknown error from KPI library: "
#define SSLOG_ERROR_TEXT_KPI "Error from KPI: "
//#define SSLOG_ERROR_TEXT_KPI_WRONG_SUBSCRIBE_ID "Wrong subscribe ID: error in mediator (KPI) library."
//#define SSLOG_ERROR_TEXT_KPI_NO_SUBSCRIBE_ID "No subscribe ID: error in mediator (KPI) library."
//#define SSLOG_ERROR_TEXT_KPI_TRANSACTION_FAILED "Transaction failed: error in mediator (KPI) library."
//#define SSLOG_ERROR_TEXT_KPI_UNKNOWN_NODE "Unknown node: error in mediator (KPI) library."
//#define SSLOG_ERROR_TEXT_KPI_UNKNOWN_SS "Unknown SS: error in mediator (KPI) library."
//#define SSLOG_ERROR_TEXT_KPI_SS_ERROR_MESSAGE_TYPE "Wrong message type: error in mediator (KPI) library".
//#define SSLOG_ERROR_TEXT_KPI_TRANSACTION_TYPE "Wrong transaction type: error in mediator (KPI) library".
//#define SSLOG_ERROR_TEXT_KPI_SSAP_MSG_FORMAT "Wrong SSAP message format: error in mediator (KPI) library".
/*@}*/ // Group ends


#endif //_SSLOG_ERRORS_TEXT_H
