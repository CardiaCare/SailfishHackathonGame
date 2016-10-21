/**
 * @file subscription.h
 * @author Aleksandr A. Lomov <lomov@cs.karelia.ru>
 * @date   05 December, 2009
 * @brief  Interface for the subscription.
 *
 *
 * @section LICENSE
 *
 * SmartSlog KP Library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * SmartSlog KP Library is distributed in the hope that it will be useful,
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
 * subscription.h - <b>Interface for the subscription.</b> <br />
 *
 * The subscription is used to track changes of data in the smart space.
 * You can subscribe to individual's properties and classes.
 * If you subscribe to properties, then you will get notification when
 * properties change in the smart space.
 *
 * The subscription to classes means that you will get notification when
 * a new individual is inserted in to the smart space or some individual
 * is removed from the smart space. <br />
 * It is the subscription to triple: <b> '* - rdf:type - class_uri' </b>
 *
 * The subscription can be synchnronous or asynchronous type. To start checking
 * notification for the synchronous subscription you need to call
 * the function #sbcr_wait. It waits the notification from the smart space.
 * The asynchronous subscription works and performs all actions in
 * the background thread. The asynchronous subscription is needed to be
 * subscribed to start working.
 *
 * To work with the subscription you need to do the following steps: <br />
 * 1 - Create a subscription (#sslog_new_subscription); <br />
 * 2 - Add data to subscription: individual-properties pairs, classes
 * (#sbcr_add_individual, #sslog_sbcr_add_class); <br />
 * 3 - Set callbacks, if you need it
 * (#sbcr_set_changed_handler...); <br />
 * 4 - Subscribe the subscription (#sslog_sbcr_subscribe); <br />
 * 5 - Working with subscription (handle callbacks,
 * checking synchronous subscription using #sslog_sbcr_wait); <br />
 * 6 - Unsubscribe subscription then it is no longer needed
 * (#sslog_sbcr_unsubscribe); <br />
 * 7 - Release the subscription (#sslog_free_subscription). 
 *
 * When the subscription is subscribed, it is in the active state. While
 * the subscription is active, then you can't modify the subscription
 * (add data to the subscription, change the type and etc.)
 *
 * To track the changes of subscribed data or an unsubscribing subscription,
 * you can use callbacks functions (#sbcr_set_changed_handler,
 * #sslog_sbcr_set_unsubscription_handler). Callbacks can be used for synchronous
 * and asynchronous subscriptions.<br />
 *
 * This file is part of SmartSlog project
 *
 * Copyright (C) 2015 by PetrSU SmartSlog team.
 */


#include <stdio.h>

#include "subscription_changes.h"

#include "entity.h"
#include "session.h"
#include "triple.h"
#include "utils/list.h"


#ifndef _SSLOG_SUBSCRIPTION_H
#define	_SSLOG_SUBSCRIPTION_H

#if !defined(SSLOG_EXTERN)
#if defined(SSLOG_EXPORT)
#define SSLOG_EXTERN __declspec(dllexport)
#elif defined (SSLOG_IMPORT)
#define SSLOG_EXTERN __declspec(dllimport)
#else
#define SSLOG_EXTERN
#endif
#endif

#ifdef	__cplusplus
extern "C" {
#endif


/******************************************************************************/
/****************************** Structures list *******************************/

// It is needed only for doxygen, to generate stract docs without
// internal fields and internal *.h files.
#ifdef DOXY_EXTERNAL_ONLY_DATA
/**
 * @brief Contains data for the subscription.
 * @see sslog_subscription_t
 */
struct sslog_subscription_s {
};
#endif

/**
 * @brief Type that represets the subscription structure.
 * @see sslog_subscription_s
 */
typedef struct sslog_subscription_s sslog_subscription_t;


/******************************************************************************/
/****************************** Functions list ********************************/

/// @cond DOXY_EXTERNAL_API

/**
 * @brief Creates a new subscription.
 *
 * Bsed on information from node about smart space appropriate messages for smart spaces
 * are created by subscription.
 * If the node leaves the smart space all subscription will be unsubscribed too.
 * Also it is possible subscribe/unsubscribe all subscription linked with some node.
 *
 * The subscriptioon can be synhronous or asynchronous. In the first case
 * you need to call #sbcr_wait to start checking notification from
 * the smart space.
 * In the second case, all operations are made in the background thread.
 *
 * This function sets a global error if it can't perfome operation,
 * you can get an error code using #sslog_get_error_code function.
 *
 * @param[in] node. Node to link with thw subscrioption.
 * Can be NULL, but must be set with #sslog_sbcr_set_node before subscribe.
 * @param[in] is_asynchronous. The flag of the asynchronous type.
 *
 * @return new created subscription on success or NULL otherwise.
 */
SSLOG_EXTERN sslog_subscription_t* sslog_new_subscription(sslog_node_t *node, bool is_asynchronous);

/**
 * @brief Free a subscription.
 *
 * This function does't release active subscriptions.
 *
 * This function sets a global error if it can't perfome operation,
 * you can get an error code using #sslog_get_error_code function.
 *
 * @param[in] subscription subscription container for free.
 */
SSLOG_EXTERN void sslog_free_subscription(sslog_subscription_t *subscription);


/**
 * @brief Checks an active state of the subscription.
 *
 * Subscription is active, when it was successfully subscribed.
 * You can't change type of the subscription (synchronous or asynchronous) or
 * add entities to the subcription when it has active state.
 * When you unsubscribe the subscription or from the smart space receives
 * unsubscribe indication, then the subscription changes the state to inactive.
 *
 * This function set global error if it can't perfome operation,
 * you can get a error code using #sslog_get_error_code function.
 *
 * @param[in] subscription subcription to check the state.
 *
 * @return true if the subscription has active state or false otherwise (also if
 * argument is incorrect).
 */
SSLOG_EXTERN bool sslog_sbcr_is_active(sslog_subscription_t *subscription);

/**
 * @brief Gets a subscription identifier.
 *
 * When subscription is in active state (it is subscribed), then it has an
 * identifier. While subscription is in inactive state, then the identifier is
 * empty string.<br />
 * <b>Don't free or modify returned string.</b>
 *
 * This function sets a global error if it can't perfome operation,
 * you can get an error code using #sslog_get_error_code function.
 *
 * @param[in] subscription subscription to get the indentifier.
 *
 * @return Subscription identifier or NULL if function can't perfome operation.
 */
SSLOG_EXTERN const char* sslog_sbcr_get_id(sslog_subscription_t *subscription);

/**
 * @brief Gets a subscription's last changes.
 *
 * When subscribed data are changed, then information about last changes is
 * updated. You can use this information to track what data have been chenged
 * after last notification.
 *
 * <b>Be careful using the asynchronous subscription</b>.<br />
 * If you get the changes not in the handler, then it possible what they
 * will be freed if the new notification for current subscription will be
 * received while you are working with them. <br />
 * It's better to make a copy of data or make all needed actions with changes
 * in the handler.
 *
 * This function sets a global error if it can't perfome operation,
 * you can get an error code using #sslog_get_error_code function.
 *
 * @param[in] subscription subscription to get last changes.
 *
 * @return information about last changes on success or NULL otherwise.
 */
SSLOG_EXTERN sslog_sbcr_changes_t *sslog_sbcr_get_changes_last(sslog_subscription_t *subscription);


/**
 * @brief Sets a handler that will be called after changing of subscribed data.
 *
 * Subscribed data could be changed after indications, after updating local data
 * this handler will be called. You can get last changes using
 * #sbcr_get_last_changes.
 *
 * You can't change the handler when the subscription is active.
 *
 * The handler is called synchronously.
 * It is important for asynchronous subscription, because the calling of
 * the handler stops checking indications for other subscription.
 *
 * This function sets a global error if it can't perfome operation,
 * you can get an error code using #sslog_get_error_code function.
 *
 * @param[in] subscription subscription to set handler.
 * @param[in] handler pointer to hadler or NULL to reset/disable it.
 */
SSLOG_EXTERN void sslog_sbcr_set_changed_handler(sslog_subscription_t *subscription,
        void (*handler)(sslog_subscription_t *));


/**
     * @brief Sets a handler that will be called after changing of subscribed data.
     *
     * Subscribed data could be changed after indications, after updating local data
     * this handler will be called. You can get last changes using
     * #sbcr_get_last_changes.
     *
     * You can't change the handler when the subscription is active.
	 *
     * The handler is called synchronously.
     * It is important for asynchronous subscription, because the calling of
     * the handler stops checking indications for other subscription.
	 * The handler can return value: 
	 *  0 - no actions;
	 *  1 - unsubscribe current subscription.
     *
     * This function sets a global error if it can't perfome operation,
     * you can get an error code using #sslog_get_error_code function.
     *
     * @param[in] subscription subscription to set handler.
     * @param[in] handler pointer to hadler or NULL to reset/disable it.
     */
SSLOG_EXTERN void sslog_sbcr_set_error_handler(sslog_subscription_t *subscription,
                                                   void (*handler)(sslog_subscription_t *, int sslog_errno));
    

/**
 * @brief Sets handler that will be called after receiving an unsubscription
 * indication.
 *
 * The smart space can unsubscribe a subscription. It sends the unsubscription
 * indication to an agent, after receiving this indication and processed it this
 * hendler will be called.
 *
 * The state of the subscription changes to inactive after unsubscribing
 * indication and you can subscribe it again.
 *
 * This handler did not called if you unsubscribe the subscription by youself
 * with function #sbcr_unsubscribe.
 *
 * The handler is called synchronously.
 * It is important for asynchronous subscription, because the calling of
 * the handler stops checking indications for other subscription.
 *
 * You can't change the handler when the subscription is active.
 *
 * This function sets a global error if it can't perfome operation,
 * you can get an error code using #sslog_get_error_code function.
 *
 * @param[in] subscription subscription to set handler.
 * @param[in] handler pointer to hadler or NULL to reset/disable it.
 */
SSLOG_EXTERN void sslog_sbcr_set_unsubscription_handler(sslog_subscription_t *subscription,
        void (*handler)(sslog_subscription_t *));


/**
 * @brief Sets node for the subscription.
 * Bsed on information from node about smart space appropriate messages for smart spaces
 * are created by subscription.
 * If the node leaves the smart space all subscription will be unsubscribed too.
 * Also it is possible subscribe/unsubscribe all subscription linked with some node.
 * @param subscription
 * @param node
 */
SSLOG_EXTERN void sslog_sbcr_set_node(sslog_subscription_t *subscription,
        sslog_node_t *node);


/**
 * @brief Adds a individual-properties pair to the subscription.
 *
 * You can subscribe to properties of the individual. To make this subscription
 * you need the individual and a list with properties.
 * You can add many pairs to the one subscription. <br />
 * If you pass NULL instead the properties list, then all
 * properties of the individual's parent class will be used for subscribing.
 *
 * This function sets a global error if it can't perfome operation,
 * you can get an error code using #sslog_get_error_code function.
 *
 * @param[in] subscription subscription to add new data for subscribing.
 * @param[in] individual individual to add.
 * @param[in] properties list of properties to subscribe or NULL for all properties.
 *
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_sbcr_add_individual(sslog_subscription_t *subscription,
        sslog_individual_t *individual, list_t *properties);

/**
 * @brief Adds a class to the subscription.
 *
 * You can subscribe to the class of individuals. This means that you will get
 * notifications evry time when RDF-triple that describes a type of an individual
 * (uuid - rdf:type - classtype) will be inseted or removed to/from
 * the smart space.
 * New individuals will be created automatically using indication data, but
 * the subscripton doesn't remove individuals when they are removed from the
 * smart space.
 *
 * Function sets information about errors (#errors.h).
 *
 * @param[in] subscription. Subscription to add new data for subscribing.
 * @param[in] ont_class. Class to add.
 *
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_sbcr_add_class(sslog_subscription_t *subscription, sslog_class_t *ont_class);


/**
 * @brief Adds a triple-template to the subscription.
 * Function makes a copy of triple-template and triples_templste must be freed manually
 * (if the triple is detached).
 *
 * Function sets information about errors (#errors.h).
 *
 * @param[in] subscription.Subscription to add new triple-template for subscribing.
 * @param triple_template. Triple-template to add.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_sbcr_add_triple_template(sslog_subscription_t *subscription, sslog_triple_t *triple_template);


/**
 * @brief Adds list if triple-templates to the subscription.
 * Function makes a copy of triple-templates and the list and triples must be freed manually.
 *
 * Function sets information about errors (#errors.h).
 *
 * @param[in] subscription.Subscription to add new triple-templates for subscribing.
 * @param triples_templates. List with triples-templates to add.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_sbcr_add_triples_templates(sslog_subscription_t *subscription, list_t *triples_templates);


/**
 * @brief Adds one SPARQL SELECT query to subscription.
 * Now only one query can be added and
 * subscription can be based only on SPARQL or on triples (classes, individual, triple-templates).
 * If the subscription contains some triples to subscribe, then this function
 * does not adds a query.
 *
 * To store triples in the local store it is need to set vriables for triples
 * (simple form of CONSTRUCT).
 * For example 5 variables are selected: 'SELECT ?a ?b ?c ?d ?e ...' and you wont to
 * store triples by using variables '?a ?c ?d' and '?b ?c ?e'. Set triples_template
 * parameters as 'a c d . b c e' (dot is used as a separator of triples).
 * In this way function gets variables from each row and constructs 2 triples to store.
 *
 * Function sets information about errors (#errors.h).
 *
 * @param[in] subscription. Subscription to add SPARQL SELECT query for subscribing.
 * @param[in] select_query. SPARQL SELECT query to add.
 * @param[in] select_triples_templates. Template to construct and store triples from vriables
 * in a form '?a ?b ?c . ?d ?e ?f . ?q ?w ?e'. If the parameter is NULL then
 * no triples will be constructed and stored in the local store.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_sbcr_add_sparql_select(sslog_subscription_t *subscription, const char *select_query, const char *select_triples_templates);


//TODO: To high level API
/**
 * @brief Subscribes a subscription.
 *
 * On success, the subscription changes status to active.
 * After this you can't change the subscription:  add new data for subscribing,
 * change a type (synchronous or asynchronous) and etc.
 * Also this function performs a first synchronization:
 * the smart space sends all data that are corresponds to subscribed data.
 * This data is used to set a starting state of the local data to track
 * further changes.
 *
 * To start checking notification from the smart space for
 * the synchronous subscription you need to call the #sbcr_wait function.
 *
 * You can track local data chnages using callback function. This callback will
 * be called evry time then local data is changed.
 * Use #sbcr_set_changed_handler function to set this callback.
 *
 * You can manually unsubscribe the subscription using #sbcr_unsubscribe
 * function, but sometimes the smart space can unsubscribe it. In this case
 * will be called the unsubscription callback.
 * Use #sbcr_set_unsubscription_handler function to set this callback.
 *
 * This function sets a global error if it can't perfome operation,
 * you can get an error code using #sslog_get_error_code function.
 *
 * @param[in] subscription subscription to subscribe.
 *
 * @return SSLOG_ERROR_NO on seccess or error code otherwise.
 */
SSLOG_EXTERN int sslog_sbcr_subscribe(sslog_subscription_t *subscription);

/**
 * @brief Unsubscribes a subscription.
 *
 * The function sends a unsubscribe message to the smart space and
 * waits a unsubscribing confirmation from the smart space.
 * This operation may take some time if a link with the smart space is bad or
 * the smart space doesn't send the confirmation message.
 * If the confirmation message is received or a timeout is ended,
 * then the subscription is considered as unsubscribed and
 * it's status is changed to inactive.
 *
 * It is possible that before the unsubscription message will be received
 * one or more indications, this indications will be handled as usual:
 * with updating local data and with calling handlers.
 *
 * <b>Remember: </b> if you unsubscribe from the subscription,
 * then you need to free it by yourself using #free_subscription
 * if it does not needed.
 *
 * This function sets a global error if it can't perfome operation,
 * you can get an error code using #sslog_get_error_code function.
 *
 * @param[in] subscription subscription to unsubscribe.
 *
 * @return SSLOG_ERROR_NO on seccess or error code otherwise.
 */
SSLOG_EXTERN int sslog_sbcr_unsubscribe(sslog_subscription_t *subscription);

/**
 * @brief Unsubscribes all subscriptions (synchronous and asynchronous).
 *
 * This function is similar to #sbcr_unsubscribe, but works with all
 * subscriptions and can free subscriptions while working.
 *
 * <b>Remembe:</b> if you pass flag to the function as 'true',
 * then you do not need to 'free' subscription by youself.
 * Also do not use this function if you are waiting synchronous subscription
 * with #sbcr_wait function. You need to stop (#sslog_sbcr_stop) synchronous
 * sybscription before calling this function.
 *
 * @param with_free flag to indicate a need to free subscriptions.
 */
SSLOG_EXTERN void sslog_sbcr_unsubscribe_all(sslog_node_t *node, bool with_free);


/**
 * @brief Waits an indication for the subscription from the smart space.
 *
 * This function works only with synchronous subscriptions. It checks
 * is there any notifications from the smart space for the current subscription.
 * If the notification is received then it updates subscribed data and ends.
 *
 * If the smart space sends a unsubscription message, then it changes
 * the subscription state to inactive and ends.
 *
 * With synchronous subscription you also can use callbacks function to
 * track the changing subscribed data and the unsubscription.
 *
 * To stop waiting you can use #sbcr_stop function.
 *
 * <b>Remember:</b> do not use #sbcr_unsubscribe or #sbcr_unsubscribe_all
 * functions while you are waiting. Call the #sbcr_stop function and when
 * the #sbcr_wait ends working, then you can unsubscribe subscription.
 *
 * This function sets a global error if it can't perfome operation,
 * you can get an error code using #sslog_get_error_code function.
 *
 * @param[in] subscription subscription to wait notifications.
 *
 * @return SSLOG_ERROR_NO on seccess (notification, unsubscribe message is received
 * or subscription was stopped with #sslog_sbcr_stop) or error code otherwise.
 */
SSLOG_EXTERN int sslog_sbcr_wait(sslog_subscription_t *subscription);


/**
 * @brief Indicates what checking of the subscription must be stopped.
 *
 * This function works in pair with #sbcr_wait and works only with synchronous
 * subscriptions.<br />
 * If you want to stop waiting, then you call this function.
 * It only indicates, but not really stops checking, you need to wait while
 * the #sbcr_wait ends, and then make other actions under subscription.
 *
 * This function sets a global error if it can't perfome operation,
 * you can get an error code using #sslog_get_error_code function.
 *
 * @param[in] subscription subscription to stop.
 *
 * @return SSLOG_ERROR_NO on seccess or error code otherwise.
 */
SSLOG_EXTERN int sslog_sbcr_stop(sslog_subscription_t *subscription);

/**
 * @brief Indicates what synchronous checking of subscriptions must be stopped.
 *
 * This function works in pair with #sbcr_wait and it is similar with
 * #sbcr_stop but indicates stop checking for all synchrnous subscriptions.
 *
 * If you want to stop waiting, then you call this function.
 * It only indicates, but not really stops checking, you need to wait while
 * the #sbcr_wait ends, and then make other actions under subscriptions.
 *
 * This function sets a global error if it can't perfome operation,
 * you can get an error code using #sslog_get_error_code function.
 */
SSLOG_EXTERN void sslog_sbcr_stop_all();

/// @endcond
// DOXY_EXTERNAL_API

#ifdef	__cplusplus
}
#endif

#endif	/* _SSLOG_SUBSCRIPTION_H */
