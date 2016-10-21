/**
 * @file subscription.c
 * @author Aleksandr A. Lomov <lomov@cs.karelia.ru>
 * @date   05 DEcember, 2009
 * @brief  Functions to work with the subscription.
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
 * subscription.c - <b>Functions to work with the subscribtion.</b> <br />
 * This file is part of SmartSlog KP Library.
 *
 * <b>Copyright (C) SmartSlog Team (Aleksandr A. Lomov).
 * All rights reserved.</b> <br />
 * Mail-list: smartslog@cs.karelia.ru
 */

#include <string.h>
#include <stdlib.h>

#ifdef MTENABLE
#include <pthread.h>
#endif

#include "kpi_interface.h"

#include "utils/debug.h"
#include "utils/errors.h"
#include "utils/errors_internal.h"
#include "utils/util_func.h"

#include "entity_internal.h"
#include "subscription_internal.h"
#include "subscription_changes_internal.h"
#include "session_internal.h"
#include "low_api_internal.h"


#if defined(WIN32) || defined(WIN) || defined (WINCE) || defined(SSLOG_WIN)
#include <winsock2.h>

void usleep(unsigned int usecs)
{
  unsigned int msecs = usecs / 1000;
  if (msecs < 1)
    Sleep (1);
  else
    Sleep (msecs);
}

#else
#include <unistd.h> /* usleep() */
#endif


/******************************************************************************/
/****************************** Structures list *******************************/
/// @cond INTERNAL_STRUCTURES

/** @brief The state of the tread. */
typedef enum thread_state_s {
    THREAD_UNINITIALIZED = -1, /**< Thread is not created. */
    THREAD_RUNNING = 1, /**< Thread is working. */
    THREAD_ENDED = 0 /**< Thread is ended. */
} thread_state;

/** @brief Contains data for asynchronous subscription thread. */
typedef struct async_sbcr_thread_s {
#ifdef MTENABLE
    pthread_t thread; /**< Thread desctiptor. */
#endif

    int status; /**< State of the thraed. */
} async_sbcr_thread_t;

/// @endcond
/******************************************************************************/



/******************************************************************************/
/************************ Static and global entities **************************/
/// @cond INTERNAL_STRUCTURES

/** @brief List for synchronous subscriptions.  */
static list_t g_sync_subscriptions = {NULL, LIST_HEAD_INIT(g_sync_subscriptions.links)};

//// For asynchronous sybscription
#ifdef MTENABLE
/** @brief List for asyncronous subscriptions.  */
static list_t g_async_subscriptions = {NULL, LIST_HEAD_INIT(g_async_subscriptions.links)};

/** @brief Mutex to manage the work with asynchronous subscription. */
static pthread_mutex_t g_async_subscription_mutex = PTHREAD_MUTEX_INITIALIZER;

/** @brief The flag to stop thread that works with asynchronous subscriptions. */
static bool g_is_async_process_need_to_stopped = false;

/** 
 * @brief The flag to indicate what is need to start working with first
 * asynchronous subscripton.
 *
 * When the asynchronous subscription is removed from the global list,
 * then it needs to continue processing from the first element in the list.
 * Otherwise it is possible, that the asynchronous process will be work
 * with the incorrect node.
 */
static bool g_to_first_async_subscription = false;

/**
 *  @brief Information about asynchronus subscription thread.
 *
 * This thread starts working after first asynchronous subscription will
 * be added to list with asynchronous subscriptions.
 *
 * It works while there are asynchronous subscriptions in the list or
 * while #g_is_async_process_need_to_stopped is false.
 */
static async_sbcr_thread_t async_sbcr_thread_info = {0, -1};
#endif

/// @endcond
/*****************************************************************************/



/******************************* Definitions *********************************/
/**************************** Static functions *******************************/
static void add_subscription(sslog_subscription_t *container);
static void remove_subscription(sslog_subscription_t *subscription);
static int process_subscription(sslog_subscription_t *subscription, int timeout);
static int wait_unsubscription(sslog_subscription_t *subscription,
        int attempts_number);

static void update_subscription(sslog_subscription_t *subscription,
        list_t *old_triples, list_t *new_triples);
static void update_sparql_subscription(sslog_subscription_t *subscription,
                                       sslog_sparql_result_t *old_result, sslog_sparql_result_t *new_result);
// TODO: Now comments, late rewrite or remove
// static void update_individual(sslog_node_t *node, sslog_triple_t *individual_triple,
        // list_t *old_triples, list_t *new_triples,
        // sslog_sbcr_changes_t *changes);

static list_t* convert_triples_to_list_by_filter(list_t *triples,
        char *subject, char *predicate, char *object);
static sslog_action_type next_triples(list_t *new_triples, list_t *old_triples,
        sslog_triple_t **new_triple, sslog_triple_t **old_triple);

static sslog_action_type get_action_type(sslog_triple_t *new_triple, sslog_triple_t *old_triple);
static sslog_individual_t *get_or_create_individual_with_uuid(sslog_node_t *node, char *uri);

static void copy_ss_info(ss_info_t *destination, const ss_info_t *source);

static void remove_node(sslog_subscription_t *subscription);

//// For asynchronous sybscription
#ifdef MTENABLE
static void start_async_sbrc_process();
static void stop_async_sbrc_process();
static bool is_async_sbrc_propcess_need_to_stoped();
static void *propcess_async_subscription(void *data);
#endif
/*****************************************************************************/


/****************************** Implementations ******************************/
/**************************** External functions *****************************/
/// @cond EXTERNAL_FUNCTIONS

/**
 * @brief Creates a new subscription.
 *
 * The subscriptioon can be synhronous or asynchronous. In the first case
 * you need to call #sbcr_wait to start checking notification from
 * the smart space.
 * In the second case, all operations are made in the background thread.
 *
 * This function sets a global error if it can't perfome operation,
 * you can get an error code using #sslog_get_error_code function.
 *
 * @param[in] is_asynchronous flag of the asynchronous type.
 *
 * @return new created subscription on success or NULL otherwise.
 */
SSLOG_EXTERN sslog_subscription_t* sslog_new_subscription(sslog_node_t *node, bool is_asynchronous)
{
    sslog_error_reset(NULL);

    sslog_subscription_t *container =
            (sslog_subscription_t *) malloc(sizeof (sslog_subscription_t));

    if (container == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_OUT_OF_MEMORY);
        sslog_error_set(NULL, SSLOG_ERROR_OUT_OF_MEMORY, SSLOG_ERROR_TEXT_OUT_OF_MEMORY);
        return NULL;
    }

    container->linked_node = NULL;

    container->kpi.socket = 0;
    container->subs_info.id[0] = '\0';


    INIT_LIST_HEAD(&container->sbrc_data.links);
    INIT_LIST_HEAD(&container->sbrc_classes.links);
    INIT_LIST_HEAD(&container->sbrc_triples.links);
    container->sbcr_select = NULL;
    container->select_triple_template = NULL;
    container->select_triples_templates = NULL;


    container->is_asynchronous = is_asynchronous;
    container->is_active = false;
    container->is_stopped = false;



    container->changed_handler = NULL;
    container->unsubscription_handler = NULL;
    container->onerror_handler = NULL;

    container->last_changes = new_subscription_changes();

    if (container->last_changes == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_OUT_OF_MEMORY);
        sslog_error_set(NULL, SSLOG_ERROR_OUT_OF_MEMORY, SSLOG_ERROR_TEXT_OUT_OF_MEMORY);
        sslog_free_subscription(container);
        return NULL;
    }

    sslog_sbcr_set_node(container, node);

    return container;
}

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
SSLOG_EXTERN void sslog_free_subscription(sslog_subscription_t *subscription)
{
    //TODO: stop a subscription
    if (sslog_sbcr_is_active(subscription) == true) {
        //sslog_error_set(NULL, SSLOG_ERROR_SUBSCRIPTION_IS_ACTIVE);
        //return;
    }

    if (subscription->linked_node != NULL) {
        list_del_and_free_nodes_with_data(&subscription->linked_node->subscriptions, subscription, NULL);
    }

    list_head_t *list_walker = NULL;

    list_for_each(list_walker, &subscription->sbrc_data.links) {
        list_t *list = list_entry(list_walker, list_t, links);
        subscription_data_t *sbcr_data = (subscription_data_t *) list->data;

        free_subscription_data(sbcr_data, true);
    }

    list_del_and_free_nodes(&subscription->sbrc_data, NULL);
    list_del_and_free_nodes(&subscription->sbrc_classes, NULL);
    list_del_and_free_nodes(&subscription->sbrc_triples, LIST_CAST_TO_FREE_FUNC sslog_free_triple_force);

    /* Free SPARQL data */
    free(subscription->sbcr_select);
    free(subscription->select_triple_template);
    list_free_with_nodes(subscription->select_triples_templates, LIST_CAST_TO_FREE_FUNC sslog_free_triple_force);

    //remove_node(subscription);

    subscription->changed_handler = NULL;
    subscription->unsubscription_handler = NULL;

    free_subscription_changes(subscription->last_changes);
    subscription->last_changes = NULL;

    free(subscription);
}

/**
 * @brief Checks an active state of the subscription.
 *
 * Subscription is active, when it was successfully subscribed.
 * You can't change type of the subscription (synchronous or asynchronous) or
 * add entities to the subcription when it has active state.
 * When you unsubscribe the subscription or from the smart space receives
 * unsubscribe indication, then the subscription changes the state to inactive.
 *
 * @param[in] subscription subcription to check the state.
 *
 * @return true if the subscription has active state or false otherwise (also if
 * argument is incorrect).
 */
SSLOG_EXTERN bool sslog_sbcr_is_active(sslog_subscription_t *subscription)
{
    if (subscription == NULL) {
        return false;
    }

    return subscription->is_active;
}

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
 * @return Subscript
 * ion identifier or NULL if function can't perfome operation.
 */
SSLOG_EXTERN const char* sslog_sbcr_get_id(sslog_subscription_t *subscription)
{
    if (subscription == NULL) {
        sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "subscription");
        return NULL;
    }

    if (subscription->is_active == false) {
        return NULL;
    }

    return subscription->subs_info.id;
}

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
 * @param[in] subscription subscription to get last changes.
 *
 * @return information about last changes on success or NULL otherwise.
 */
SSLOG_EXTERN sslog_sbcr_changes_t *sslog_sbcr_get_changes_last(sslog_subscription_t *subscription)
{
    if (subscription == NULL) {
        return NULL;
    }

    return subscription->last_changes;
}

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
        void (*handler)(sslog_subscription_t *))
{
    sslog_error_reset(NULL);

    if (subscription == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "subscription");
    }

    if (sslog_sbcr_is_active(subscription) == true) {
        sslog_error_set(NULL, SSLOG_ERROR_SUBSCRIPTION_IS_ACTIVE, SSLOG_ERROR_TEXT_SUBSCRIPTION_IS_ACTIVE);
        return;
    }

    subscription->changed_handler = handler;
}



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
SSLOG_EXTERN void sslog_sbcr_set_error_handler(sslog_subscription_t *subscription,
        void (*handler)(sslog_subscription_t *, int sslog_errno))
{
    sslog_error_reset(NULL);

    if (subscription == NULL) {
        sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "subscription");
        return;
    }

    if (sslog_sbcr_is_active(subscription) == true) {
        sslog_error_set(NULL,SSLOG_ERROR_SUBSCRIPTION_IS_ACTIVE, NULL);
        return;
    }

	subscription->onerror_handler = handler;
}



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
        void (*handler)(sslog_subscription_t *))
{
    sslog_error_reset(NULL);

    if (subscription == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "subscription");
        return;
    }

    if (sslog_sbcr_is_active(subscription) == true) {
        sslog_error_set(NULL, SSLOG_ERROR_SUBSCRIPTION_IS_ACTIVE,
                        SSLOG_ERROR_TEXT_SUBSCRIPTION_IS_ACTIVE);
        return;
    }

    subscription->unsubscription_handler = handler;
}


void sslog_sbcr_set_node(sslog_subscription_t *subscription, sslog_node_t *node)
{
    if (subscription == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "subscription.");
        return;
    }

    if (node == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "node.");
        return;
    }

    if (sslog_sbcr_is_active(subscription) == true) {
        sslog_error_set(NULL, SSLOG_ERROR_SUBSCRIPTION_IS_ACTIVE,
                        SSLOG_ERROR_TEXT_SUBSCRIPTION_IS_ACTIVE);
        return;
    }

    remove_node(subscription);

    // Set links with new node.
    subscription->linked_node = node;

    list_add_data(&node->subscriptions, subscription);
}


SSLOG_EXTERN int sslog_sbcr_add_triple_template(sslog_subscription_t *subscription, sslog_triple_t *triple_template)
{
    if (triple_template == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "triple_template");
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT,
                               SSLOG_ERROR_TEXT_NULL_ARGUMENT "triple_template");
    }

    list_t *triples = list_new();
    list_add_data(triples, triple_template);

    int result = sslog_sbcr_add_triples_templates(subscription, triples);

    list_free_with_nodes(triples, NULL);

    return result;
}


SSLOG_EXTERN int sslog_sbcr_add_triples_templates(sslog_subscription_t *subscription, list_t *triples_templates)
{
    sslog_error_reset(NULL);

    if (subscription == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "subscription");
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT,
                               SSLOG_ERROR_TEXT_NULL_ARGUMENT "subscription");
    }

    if (sslog_sbcr_is_active(subscription) == true) {
        SSLOG_DEBUG_FUNC("Subscription '%s' is active.", subscription->subs_info.id);
        return sslog_error_set(NULL, SSLOG_ERROR_SUBSCRIPTION_IS_ACTIVE,
                               SSLOG_ERROR_TEXT_SUBSCRIPTION_IS_ACTIVE);
    }

    if (list_is_null_or_empty(triples_templates) == true) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "triples_templates");
        return sslog_error_set(NULL, SSLOG_ERROR_INCORRECT_ARGUMENT,
                               SSLOG_ERROR_TEXT_INCORRECT_ARGUMENT "triples_templates is null or empty");
    }


    list_head_t *list_walker = NULL;
    list_for_each(list_walker, &triples_templates->links) {
        list_t *node = list_entry(list_walker, list_t, links);
        sslog_triple_t *triple = (sslog_triple_t *) node->data;

        sslog_triple_t *copy_triple = sslog_new_triple_detached(triple->subject, triple->predicate, triple->object,
			(sslog_rdf_type) triple->subject_type, (sslog_rdf_type) triple->object_type);

        list_add_data(&subscription->sbrc_triples, copy_triple);
    }

    SSLOG_DEBUG_FUNC("Added %d triples to subscription.", list_count(&subscription->sbrc_triples));

    return sslog_error_reset(&subscription->last_error);
}

int sslog_sbcr_add_sparql_select(sslog_subscription_t *subscription, const char *select_query, const char *select_triples_templates)
{
    if (subscription == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "subscription");
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT,
                               SSLOG_ERROR_TEXT_NULL_ARGUMENT "subscription.");
    }

    if (sslog_sbcr_is_active(subscription) == true) {
        SSLOG_DEBUG_FUNC("Subscription '%s' is active.", subscription->subs_info.id);
        return sslog_error_set(NULL, SSLOG_ERROR_SUBSCRIPTION_IS_ACTIVE,
                               SSLOG_ERROR_TEXT_SUBSCRIPTION_IS_ACTIVE);
    }

    if (sslog_is_str_null_empty(select_query) == true) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "select_query");
        return sslog_error_set(NULL, SSLOG_ERROR_INCORRECT_ARGUMENT,
                               SSLOG_ERROR_TEXT_INCORRECT_ARGUMENT "select_query is null or empty.");
    }

    if (list_is_empty(&subscription->sbrc_triples) == 0) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_INCORRECT_SUBSCRIPTION "subscription already contains %i triples.", list_count(&subscription->sbrc_triples));
        return sslog_error_set(NULL, SSLOG_ERROR_INCORRECT_SUBSCRIPTION,
                               SSLOG_ERROR_TEXT_INCORRECT_SUBSCRIPTION "subscription already contains triples, select query cannot be added.");
    }

    subscription->sbcr_select = sslog_strndup(select_query, SSLOG_SELECT_QUERY_MAX_LEN);

    sslog_free_triples(subscription->select_triples_templates);
    free(subscription->select_triple_template);

    if (select_triples_templates != NULL) {
        subscription->select_triple_template = sslog_strndup(select_triples_templates, SSLOG_SELECT_QUERY_MAX_LEN);
        subscription->select_triples_templates = sslog_sparql_template_to_triples(select_triples_templates);
    } else {
        subscription->select_triple_template = NULL;
        subscription->select_triples_templates = NULL;
    }




    return sslog_error_reset(&subscription->last_error);
}


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
        sslog_individual_t *individual, list_t *properties)
{
    sslog_error_reset(NULL);

    if (subscription == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "subscription");
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT,
                               SSLOG_ERROR_TEXT_NULL_ARGUMENT "subscription");
    }

    if (sslog_sbcr_is_active(subscription) == true) {
        SSLOG_DEBUG_FUNC("Subscription '%s' is active.", subscription->subs_info.id);
        return sslog_error_set(NULL, SSLOG_ERROR_SUBSCRIPTION_IS_ACTIVE,
                               SSLOG_ERROR_TEXT_SUBSCRIPTION_IS_ACTIVE);
    }

    if (individual == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "individual");
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT,
                               SSLOG_ERROR_TEXT_NULL_ARGUMENT "subscription");
    }


    // Convert individuals and properties to triples.
    list_head_t *list_walker = NULL;
    list_for_each(list_walker, &properties->links) {
        list_t *node = list_entry(list_walker, list_t, links);
        sslog_property_t *property = (sslog_property_t *) node->data;

        sslog_triple_t *triple = sslog_new_triple_detached(individual->entity.uri, property->entity.uri, SSLOG_TRIPLE_ANY,
                                                           SSLOG_RDF_TYPE_URI, SSLOG_RDF_TYPE_URI);

        list_add_data(&subscription->sbrc_triples, triple);
    }

    subscription_data_t *data = new_subscription_data(individual, properties);

    if (data == NULL) {
        return sslog_error_set(&subscription->last_error, SSLOG_ERROR_OUT_OF_MEMORY, SSLOG_ERROR_TEXT_OUT_OF_MEMORY);
    }

    list_add_data(&subscription->sbrc_data, data);

    return SSLOG_ERROR_NO;
}

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
 * This function sets a global error if it can't perfome operation,
 * you can get an error code using #sslog_get_error_code function.
 *
 * @param[in] subscription subscription to add new data for subscribing.
 * @param[in] ont_class class to add.
 *
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_sbcr_add_class(sslog_subscription_t *subscription, sslog_class_t *ont_class)
{
    sslog_error_reset(NULL);

    if (subscription == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "subscription");
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT,
                               SSLOG_ERROR_TEXT_NULL_ARGUMENT "subscription");
    }

    if (sslog_sbcr_is_active(subscription) == true) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_SUBSCRIPTION_IS_ACTIVE "%s", subscription->subs_info.id);
        return sslog_error_set(NULL, SSLOG_ERROR_SUBSCRIPTION_IS_ACTIVE,
                               SSLOG_ERROR_TEXT_SUBSCRIPTION_IS_ACTIVE);
    }

    if (ont_class == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "ont_class");
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT,
                               SSLOG_ERROR_TEXT_NULL_ARGUMENT "ont_class");
    }

    list_add_data(&subscription->sbrc_classes, ont_class);

    sslog_triple_t *template_triple = sslog_new_triple_detached(SSLOG_TRIPLE_ANY, SSLOG_TRIPLE_RDF_TYPE, ont_class->entity.uri,
                                                                SSLOG_RDF_TYPE_URI, SSLOG_RDF_TYPE_URI);

    list_add_data(&subscription->sbrc_triples, template_triple);

    return sslog_error_reset(NULL);
}

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
SSLOG_EXTERN int sslog_sbcr_subscribe(sslog_subscription_t *subscription)
{
    if (subscription == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "subscription");
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT,
                               SSLOG_ERROR_TEXT_NULL_ARGUMENT "subscription");
    }

    if (sslog_sbcr_is_active(subscription) == true) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_SUBSCRIPTION_IS_ACTIVE "%s", subscription->subs_info.id);
        return sslog_error_set(NULL, SSLOG_ERROR_SUBSCRIPTION_IS_ACTIVE,
                               SSLOG_ERROR_TEXT_SUBSCRIPTION_IS_ACTIVE);
    }

#ifndef MTENABLE
    if (subscription->is_asynchronous == true) {
        SSLOG_DEBUG_FUNC("Asynchronous subscription doesn't support (threads are disabled).");
        return sslog_error_set(NULL, SSLOG_ERROR_INCORRECT_SUBSCRIPTION_TYPE,
                               SSLOG_ERROR_TEXT_INCORRECT_SUBSCRIPTION_TYPE "asynchronous subscription are disabled.");
    }
#endif


    if (subscription->linked_node == NULL) {
        SSLOG_DEBUG_FUNC("The node for the subscription is not set. Operation cannot be done.).");
        return sslog_error_set(NULL, SSLOG_ERROR_INCORRECT_SUBSCRIPTION,
                               SSLOG_ERROR_TEXT_INCORRECT_SUBSCRIPTION "node is not set.");
    }

    copy_ss_info(&subscription->kpi, subscription->linked_node->kpi);

    list_t *result_triples = NULL;
    sslog_sparql_result_t *sparql_result = NULL;

    int result = SSLOG_ERROR_NO;

    // Subscribe to triples or to SPARQL SELECT query.
    if (sslog_sbcr_is_sparql(subscription)) {
        result = sslog_kpi_subscribe_sparql_select(&subscription->kpi, &subscription->subs_info, subscription->sbcr_select, &sparql_result);
    } else {
        result = sslog_kpi_subscribe_triples(&subscription->kpi, &subscription->subs_info,
                                             &subscription->sbrc_triples, &result_triples);
    }

    if (result != SSLOG_ERROR_NO) {
         SSLOG_DEBUG_FUNC("Can't subscribe, KPI failed: %s.", sslog_kpi_get_error_text(result));
         return sslog_error_set(&subscription->last_error, result, sslog_kpi_get_error_text(result));
    }

    subscription->is_active = true;

    // This step called first synchronization:
    // all triples that were receiveved after subscription are set as new.
    // Create an empty list  with old triples to update subscription.
    if (sslog_sbcr_is_sparql(subscription)) {
        //FIXME: In CKPI number of bindings return only on subscription and no on indivation (bug in CKPI).
        subscription->bindings_count = (sparql_result == NULL) ? 0 : sparql_result->bindings_count;
        update_sparql_subscription(subscription, NULL, sparql_result);
    } else {
        update_subscription(subscription, NULL, result_triples);
    }


    // Add container to one of two lists depends from subscription's type.
    add_subscription(subscription);

    return sslog_error_reset(&subscription->last_error);
}

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
SSLOG_EXTERN int sslog_sbcr_unsubscribe(sslog_subscription_t *subscription)
{
    SSLOG_DEBUG_FUNC("Unsibscribing: '%s'.", (subscription == NULL) ? "NULL" : subscription->subs_info.id);

    sslog_error_reset(NULL);

    if (subscription == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "subscription");
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT,
                               SSLOG_ERROR_TEXT_NULL_ARGUMENT "subscription");
    }

    if (sslog_sbcr_is_active(subscription) == false) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_SUBSCRIPTION_IS_INACTIVE "%s", subscription->subs_info.id);
        return sslog_error_set(NULL, SSLOG_ERROR_SUBSCRIPTION_IS_INACTIVE,
                               SSLOG_ERROR_TEXT_SUBSCRIPTION_IS_INACTIVE);
    }


    remove_subscription(subscription);

    int error_code = SSLOG_ERROR_NO;

    if (sslog_kpi_unsubscribe(&subscription->kpi, &subscription->subs_info) == 0) {
        error_code = wait_unsubscription(subscription, KPLIB_SBCR_UNSUBSCRIBE_ATTEMPTS);
    } else {
        SSLOG_DEBUG_FUNC("KPI operation fails for subscription: %s", subscription->subs_info.id);
        error_code = SSLOG_ERROR_CANT_UNSUBSCRIBE;
    }

    subscription->is_active = false;

    return sslog_error_set(NULL, error_code, NULL);
}

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
 * @param node with subscription to unsubscribe.
 * @param with_free flag to indicate a need to free subscriptions.
 */
void sslog_sbcr_unsubscribe_all(sslog_node_t *node, bool with_free)
{

    list_head_t *list_walker = NULL;
    list_head_t *cur_pos = NULL;

#ifdef MTENABLE
	stop_async_sbrc_process();

    list_for_each_safe(list_walker, cur_pos, &g_async_subscriptions.links) {
           list_t *list_node = list_entry(list_walker, list_t, links);
           sslog_subscription_t *subscription = (sslog_subscription_t *) list_node;

           if (subscription->linked_node != node) {
               continue;
           }

           sslog_sbcr_unsubscribe(subscription);

           if (with_free == true) {
               remove_node(subscription);
               //list_del_and_free_node(list_node, LIST_CAST_TO_FREE_FUNC sslog_free_subscription);
               sslog_free_subscription(subscription);
           }
       }


//    while (true) {

//        list_t *node = list_get_first_prev_node(&g_async_subscriptions);
//        if (node == NULL) {
//            break;
//        }

//        sslog_subscription_t *subscription = (sslog_subscription_t *) node->data;

//        if (subscription.linked_node != node) {
//            continue;
//        }

//        sslog_sbcr_unsubscribe(subscription);

//        if (with_free == true) {
              //sslog_free_subscription(subscription);
//            //list_del_and_free_nodes(&node->subscriptions, LIST_CAST_TO_FREE_FUNC sslog_free_subscription);
//        }
//    }
    //list_del_and_free_nodes(&g_async_subscriptions,
    //        NULL);
#endif

    list_walker = NULL;
    cur_pos = NULL;

    list_for_each_safe(list_walker, cur_pos, &g_sync_subscriptions.links) {
           list_t *list_node = list_entry(list_walker, list_t, links);
           sslog_subscription_t *subscription = (sslog_subscription_t *) list_node->data;

           if (subscription->linked_node != node) {
               continue;
           }

           sslog_sbcr_unsubscribe(subscription);

           if (with_free == true) {
               remove_node(subscription);
               sslog_free_subscription(subscription);
               //remove_node(subscription);
               //list_del_and_free_node(list_node, LIST_CAST_TO_FREE_FUNC sslog_free_subscription);
           }
       }



//    while (true) {
//        list_t *node = list_get_first_prev_node(&g_sync_subscriptions);
//        if (node == NULL) {
//            break;
//        }

//        sslog_subscription_t *subscription = (sslog_subscription_t *) node->data;

//        sslog_sbcr_unsubscribe(subscription);

//        if (with_free == true) {
//            sslog_free_subscription(subscription);
//        }
//    }

//    list_del_and_free_nodes(&g_sync_subscriptions, NULL);
}

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
int sslog_sbcr_wait(sslog_subscription_t *subscription)
{
    if (subscription == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "subscription.");
        return sslog_error_set(&subscription->last_error, SSLOG_ERROR_NULL_ARGUMENT,
                               SSLOG_ERROR_TEXT_NULL_ARGUMENT "subscription.");
    }

    if (subscription->is_asynchronous == true) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_INCORRECT_SUBSCRIPTION_TYPE "Subscription is asynchronous.");
        return sslog_error_set(&subscription->last_error, SSLOG_ERROR_INCORRECT_SUBSCRIPTION_TYPE,
                               SSLOG_ERROR_TEXT_INCORRECT_SUBSCRIPTION_TYPE "Subscription is asynchronous.");
    }

    if (sslog_sbcr_is_active(subscription) == false) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_SUBSCRIPTION_IS_INACTIVE);
        return sslog_error_set(&subscription->last_error, SSLOG_ERROR_SUBSCRIPTION_IS_INACTIVE,
                               SSLOG_ERROR_TEXT_SUBSCRIPTION_IS_INACTIVE);
    }

    subscription->is_stopped = false;

    while (sslog_sbcr_is_active(subscription) == true
            && subscription->is_stopped == false) {

        int status = process_subscription(subscription,
                KPLIB_SBCR_SYNC_WAITING_TIMEOUT);

        if (status == 0) { // Timeout
            continue;
        } else if (status < 0) { // Error

            SSLOG_DEBUG_FUNC("Processing subscription failed: %s (%s)", subscription->subs_info.id,
                             sslog_kpi_get_error_text(subscription->kpi.ss_errno));


            if (subscription->onerror_handler != NULL) {
                subscription->onerror_handler(subscription, subscription->kpi.ss_errno);
			}

            return sslog_error_set(&subscription->last_error,
                                   sslog_kpi_get_error(subscription->kpi.ss_errno),
                                   sslog_kpi_get_error_text(subscription->kpi.ss_errno));
        } else { // Unsubscription or indication
            break;
        }
    }

    SSLOG_DEBUG_END

    return SSLOG_ERROR_NO;
}

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
 */
SSLOG_EXTERN int sslog_sbcr_stop(sslog_subscription_t *subscription)
{
    sslog_error_reset(NULL);

    if (subscription == NULL) {
       return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT,
                        SSLOG_ERROR_TEXT_NULL_ARGUMENT "subscription");
    }

    if (subscription->is_asynchronous == true) {
        return sslog_error_set(NULL, SSLOG_ERROR_INCORRECT_SUBSCRIPTION_TYPE, SSLOG_ERROR_TEXT_INCORRECT_SUBSCRIPTION_TYPE);
    }

    if (sslog_sbcr_is_active(subscription) == false) {
        return sslog_error_set(NULL, SSLOG_ERROR_SUBSCRIPTION_IS_INACTIVE,
                        SSLOG_ERROR_TEXT_SUBSCRIPTION_IS_INACTIVE "subscription cannot be stopped.");
    }

    subscription->is_stopped = true;

    return sslog_error_reset(&subscription->last_error);
}

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
SSLOG_EXTERN void sslog_sbcr_stop_all()
{
    sslog_error_reset(NULL);

    list_head_t *list_walker = NULL;

    list_for_each(list_walker, &g_sync_subscriptions.links)
    {
        list_t *node = list_entry(list_walker, list_t, links);

        if (node->data == NULL) {
            continue;
        }

        sslog_subscription_t *subscription = (sslog_subscription_t *) node->data;
        subscription->is_stopped = true;
    }

}

/// @endcond
/******************************************************************************/



/******************************************************************************/
/***************************** Internal functions *****************************/
/// @cond INTERNAL_FUNCTIONS

/**
 * @brief Create a new data for the subscription.
 *
 * Subscription data contains individual and list with properties
 * or NULL for all individual's properties.
 *
 * @param individual individual.
 * @param properties list with properties for subscribe
 * or NULL for all properties.
 *
 * @return new subscription data on success or NULL otherwise.
 */
subscription_data_t* new_subscription_data(sslog_individual_t *individual, list_t *properties)
{
    if (individual == NULL) {
        return NULL;
    }

    subscription_data_t *data =
            (subscription_data_t *) malloc(sizeof (subscription_data_t));

    if (data == NULL) {
        return NULL;
    }

    data->individual_triple = sslog_triple_copy(individual->entity.linked_triple);

    if (properties == NULL) {
        data->properties_triples = properties;
        return data;
    }

    data->properties_triples = list_new();

    list_head_t *list_walker = NULL;
    list_for_each (list_walker, &properties->links) {
        list_t *entry = list_entry(list_walker, list_t, links);
        sslog_property_t *property = (sslog_property_t *) entry->data;
        sslog_triple_t *triple = sslog_triple_copy(property->entity.linked_triple);

        list_add_data(data->properties_triples, triple);
    }

    list_free_with_nodes(properties, NULL);

    return data;
}

/**
 * @brief Free subscription data.
 *
 * Free subscription data structure, individual and properties are not freed.
 * You can free properties list (only list structure will be freed, not properties),
 * using parameter free_prop_list.
 *
 * @param data subscription data to free.
 * @param free_prop_list if it is 'true' - list structure will be freed.
 */
void free_subscription_data(subscription_data_t *data, bool free_prop_list)
{
    if (data == NULL) {
        return;
    }

    sslog_free_triple(data->individual_triple);

    if (free_prop_list == true) {
        list_free_with_nodes(data->properties_triples, LIST_CAST_TO_FREE_FUNC sslog_free_triple);
    }
    data->properties_triples = NULL;

    free(data);
}

void sslog_sbcr_shutdown()
{
    stop_async_sbrc_process();
}


bool sslog_sbcr_is_sparql(sslog_subscription_t *subscription)
{
    if (subscription == NULL) {
        return false;
    }

    if (subscription->sbcr_select != NULL) {
        return true;
    }

    return false;
}

/// @endcond
/******************************************************************************/



/******************************************************************************/
/***************************** Static functions *******************************/

/**
 * @brief Adds a subscription to the list.
 *
 * Depending of the subscription type adds the subscription to list and
 * starts asynchronous subscription process if the subscription has
 * the asynchronous type.
 *
 * @param[in] subscription subscription to add.
 */
static void add_subscription(sslog_subscription_t *subscription)
{
    if (subscription == NULL) {
        return;
    }

    if (subscription->is_asynchronous == false) {
        list_add_data(&g_sync_subscriptions, subscription);
        return;
    }

#ifdef MTENABLE

    pthread_mutex_lock(&g_async_subscription_mutex);

    list_add_data(&g_async_subscriptions, subscription);

    start_async_sbrc_process();

    pthread_mutex_unlock(&g_async_subscription_mutex);
#endif
}

/**
 * Removes subscription from the list.
 * 
 * If the subscription is asynchronous, then it lock mutex 
 * #g_async_subscription_mutex and remove the subscription. 
 * If the subscription is removed it sets #g_to_first_async_subscription to
 * true.
 *
 * @param[in] subscription subscription to remove.
 */
static void remove_subscription(sslog_subscription_t *subscription)
{
    if (subscription == NULL) {
        return;
    }

    // Remove synchronous subscription.
    if (subscription->is_asynchronous == false) {
        list_del_and_free_nodes_with_data(&g_sync_subscriptions, subscription, NULL);
        return;
    }


#ifdef MTENABLE // Remove asynchronous subscription.
    pthread_mutex_lock(&g_async_subscription_mutex);

    list_t *del_node = NULL;
    list_head_t *list_walker = NULL;

    // Try to find given subscription and remove it from the asynchronous list.
    // Set a global variable to start processing from the first element if the
    // subscription is removed.

    list_for_each(list_walker, &g_async_subscriptions.links)
    {
        list_t *node = list_entry(list_walker, list_t, links);

        if (node->data == subscription) {
            del_node = node;
        }

        if (del_node != NULL) {
            list_del_and_free_node(del_node, NULL);

            SSLOG_DEBUG_FUNC("Async list is empty = %i",
                             list_is_empty(&g_async_subscriptions));
            g_to_first_async_subscription = true;

            break;
        }
    }
    pthread_mutex_unlock(&g_async_subscription_mutex);
#endif
}

/**
 * @brief Checks notification for the subscription.
 *
 * The function calls KPI function to check notification from the smart space.
 * The timeout sets maximum time that will be used for waiting notifications.
 *
 * If the notification received then subscribed data will be updated.
 *
 * If unsubscription message received then the current subscription will be
 * removed from the subscriptions list and it state will be changed to inactive.
 * In this case unsubscription callback will be called.
 *
 * @param[in] subscription subscription to process.
 * @param[in] timeout maximum time for waiting notifications.
 *
 * @return 0 - time is over, 1 - indication, 2 - unsubscription message, -1 - error.
 */
static int process_subscription(sslog_subscription_t *subscription, int timeout)
{
    if (subscription == NULL) {
        return -1;
    }

    list_t * n_val = NULL;
    list_t * o_val = NULL;

    sslog_sparql_result_t *new_result = NULL;
    sslog_sparql_result_t *old_result = NULL;

    int status = -1;

    if (sslog_sbcr_is_sparql(subscription) == true) {
        status = sslog_kpi_sparql_subscribe_indication(&subscription->kpi, &subscription->subs_info,
                                                       &new_result, &old_result, subscription->bindings_count, timeout);
    } else {
        status = sslog_kpi_subscribe_indication(&subscription->kpi,
                                                &subscription->subs_info, &n_val, &o_val, timeout);
    }

    switch (status) {
        case 1: // Indication
            SSLOG_DEBUG_FUNC("Indication received for subscription: '%s'.", subscription->subs_info.id);
            if (sslog_sbcr_is_sparql(subscription) == true) {
                update_sparql_subscription(subscription, old_result, new_result);
            } else {
                update_subscription(subscription, o_val, n_val);
            }
            break;

        case 2: // Unsubscription
           remove_subscription(subscription);
           subscription->is_active = false;

           if (subscription->unsubscription_handler != NULL) {
               subscription->unsubscription_handler(subscription);
           }

        break;

    case 0: // Timeout
        break;

    default: // Error: -1
        SSLOG_DEBUG_FUNC("Indication error for the subscription '%s'", subscription->subs_info.id);
        break;
    }

//    list_free_with_nodes(n_val, LIST_CAST_TO_FREE_FUNC sslog_free_triple);
//    list_free_with_nodes(o_val, LIST_CAST_TO_FREE_FUNC sslog_free_triple);

    return status;
}

/**
 * @brief Waits unsubscription message from the smart space.
 * 
 * To confirm unsubscription, it is needed to get notification from
 * the smart space. This function try to get this notification.
 * It possible that the indication will be received before
 * the unsubscription message, in this case this notification will be processed
 * as usual.
 *
 * It tries to wait notification only a few times.
 *
 * @param[in] subscription subscription to wait notification.
 * @param[in] attempts_number number to attempts to try get notification.
 *
 * @return SSLOG_ERROR_NO on success or error_code otherwise.
 */
static int wait_unsubscription(sslog_subscription_t *subscription,
        int attempts_number)
{
    if (subscription == NULL) {
        return SSLOG_ERROR_INCORRECT_SUBSCRIPTION;
    }

    bool is_unsubscribed = false;

    while (attempts_number > 0) {

        list_t *n_val = NULL;
        list_t *o_val = NULL;
        sslog_sparql_result_t *new_result = NULL;
        sslog_sparql_result_t *old_result = NULL;

        int status = -1;

        if (sslog_sbcr_is_sparql(subscription) == true) {
            status = sslog_kpi_sparql_subscribe_indication(&subscription->kpi,
                                                           &subscription->subs_info, &new_result, &old_result,
                                                           subscription->bindings_count,
                                                           KPLIB_SBCR_UNSBCR_WAITING_TIMEOUT);
        } else {
            status = sslog_kpi_subscribe_indication(&subscription->kpi,
                                                    &subscription->subs_info, &n_val, &o_val,
                                                    KPLIB_SBCR_UNSBCR_WAITING_TIMEOUT);
        }
		
        switch (status) {
        case 1: // Indication

            SSLOG_DEBUG_FUNC("Indication received for subscription: '%s'.", subscription->subs_info.id);
            if (sslog_sbcr_is_sparql(subscription) == true) {
                update_sparql_subscription(subscription, old_result, new_result);
            } else {
                update_subscription(subscription, o_val, n_val);
            }

            break;
        case 2: // Unsubscribde indication
            attempts_number = -1;
            is_unsubscribed = true;
            SSLOG_DEBUG_FUNC("Unsubscription indication received for subscription.");
            break;
        case 0: // Timeout
            SSLOG_DEBUG_FUNC("Wait unsubscription for %s (%d)", subscription->subs_info.id, attempts_number);
            break;
        default: // Error
            SSLOG_DEBUG_FUNC("Indication fails for subscription: %s", subscription->subs_info.id);
            break;
        }

        n_val = NULL;
        n_val = NULL;
        new_result = NULL;
        old_result = NULL;

        --attempts_number;
    }

    if (is_unsubscribed == false) {
        SSLOG_DEBUG_FUNC("Unsubscription indication does not received for subscription: %s",
                         subscription->subs_info.id);

#ifdef KPI_LOW
		// KPI_Low doesn't export close function, we can't use it in the Windows.
#if !(defined(WIN32) || defined (WINCE))
        ss_close(subscription->info.socket);
#endif
        subscription->info.socket = -1;
        subscription->info.id[0] = '\0';
#else                 
        sslog_kpi_close_subscription(&subscription->subs_info);
#endif        

        return SSLOG_ERROR_CANT_UNSUBSCRIBE;
    }

    return SSLOG_ERROR_NO;

}


static void update_sparql_subscription(sslog_subscription_t *subscription,
                                       sslog_sparql_result_t *old_result, sslog_sparql_result_t *new_result)
{
    list_t *new_triples = NULL;
    list_t *old_triples = NULL;

    sslog_free_sparql_result(subscription->last_changes->old_result);
    sslog_free_sparql_result(subscription->last_changes->new_result);
    subscription->last_changes->new_result = NULL;
    subscription->last_changes->old_result = NULL;

    if (old_result != NULL) {
           old_triples = sslog_sparql_result_to_triples(old_result, subscription->select_triples_templates);
           subscription->last_changes->old_result = old_result;
    }

    if (new_result != NULL) {
           new_triples = sslog_sparql_result_to_triples(new_result, subscription->select_triples_templates);
           subscription->last_changes->new_result = new_result;
    }

    update_subscription(subscription, old_triples, new_triples);

}


/**
 * @brief Updates a subscription using triples.
 *
 * Updates local classes and then all individuals.
 * Calls changed handler of the subscription.
 *
 * @param[in] subscription subscription to update.
 * @param[in] old_triples triples that were removed from the smart space.
 * @param[in] new_triples triples that were inserted to the smart space.
 */
static void update_subscription(sslog_subscription_t *subscription,
        list_t *old_triples, list_t *new_triples)
{
    // Clean data about changes
    // and increment the sequence number of the changes.
    clean_changes_data(subscription->last_changes);
    inc_sequence_number(subscription->last_changes);

    if (old_triples == NULL && new_triples == NULL) {
        SSLOG_DEBUG_FUNC("No triple to update subscription '%s'.", subscription->subs_info.id);
        return;
    }

    if (old_triples == NULL) {
        old_triples = list_new();
    }

    if (new_triples == NULL) {
        new_triples = list_new();
    }


    sslog_store_update_triples(sslog_session_get_default()->store, old_triples, new_triples);

    list_t *copy_list = sslog_copy_triples(old_triples);
    list_add_list(copy_list , &subscription->last_changes->removed_triples);
    list_free(copy_list);
    sslog_free_triples(old_triples);

    copy_list = sslog_copy_triples(new_triples);
    list_add_list(copy_list , &subscription->last_changes->inserted_triples);
    list_free(copy_list);
    sslog_free_triples(new_triples);

    list_head_t *list_walker = NULL;

    list_walker = NULL;
    list_for_each(list_walker, &subscription->last_changes->inserted_triples.links) {
        list_t *node = list_entry(list_walker, list_t, links);
        sslog_triple_t *triple = (sslog_triple_t *) node->data;

        if (sslog_triple_is_individual(triple) == true) {
            list_add_data(&subscription->last_changes->inserted_individuals, triple->subject);
        } else if (sslog_triple_is_class(triple) == true) {
            list_add_data(&subscription->last_changes->inserted_classes, triple->subject);
        } else if (sslog_triple_is_property(triple) == true) {
            list_add_data(&subscription->last_changes->inserted_props, triple->subject);
        }

      }


    list_walker = NULL;
    list_for_each(list_walker, &subscription->last_changes->removed_triples.links) {
        list_t *node = list_entry(list_walker, list_t, links);
        sslog_triple_t *triple = (sslog_triple_t *) node->data;

        if (sslog_triple_is_individual(triple) == true) {
            list_add_data(&subscription->last_changes->removed_individuals, triple->subject);
        } else if (sslog_triple_is_class(triple) == true) {
            list_add_data(&subscription->last_changes->removed_classes, triple->subject);
        } else if (sslog_triple_is_property(triple) == true) {
            list_add_data(&subscription->last_changes->removed_props, triple->subject);
        }

      }

}


/**
 * @brief Updates an individual.
 *
 * This function try to found triples for current individuals by UUID and
 * updates individual's properties using this triples.
 *
 * It also update information about changes that were done.
 *
 * @param[in] individual individual to update.
 * @param[in] old_triples triples that were removed from the smart space.
 * @param[in] new_triples triples that were inserted to the smart space.
 * @param[in] changes information about changes.
 */

 static void update_individual(sslog_node_t *node, sslog_triple_t *individual_triple,
                              list_t *old_triples, list_t *new_triples,
                              sslog_sbcr_changes_t *changes)
{    

    // Gets triples by individual's UUID.
    list_t *new_triples_list = convert_triples_to_list_by_filter(new_triples, individual_triple->subject, NULL, NULL);
    list_t *old_triples_list = convert_triples_to_list_by_filter(old_triples, individual_triple->subject, NULL, NULL);

    while (list_is_null_or_empty(new_triples_list) != 1 || list_is_null_or_empty(old_triples_list) != 1) {

        sslog_triple_t *new_triple = NULL;
        sslog_triple_t *old_triple = NULL;

        // Get action type for triples (inserted, removed, updated), depends on
        // this set active triples and ork  with them.
        sslog_action_type current_action = next_triples(new_triples_list, old_triples_list, &new_triple, &old_triple);
        sslog_triple_t *active_triple = (current_action == SSLOG_ACTION_REMOVE) ? old_triple : new_triple;

        // Get property using uri from the smart space.
        char *prop_name = active_triple->predicate;

        sslog_property_t *prop_type = sslog_store_get_property(sslog_session_get_default()->store, prop_name);

        if (prop_type == NULL) {
            SSLOG_DEBUG_FUNC("No property - %s for individual %s",
                    prop_name, individual_triple->subject);
            continue;
        }

        void *object = NULL;

        // Prepare a value for the property depending of the property type.
        if (sslog_property_is_object(prop_type) == true) {
            char *uri = active_triple->object;
            object = get_or_create_individual_with_uuid(node, uri);
        } else {
            object = active_triple->object;
        }

        // Change property. Triple was removed.
        if (current_action == SSLOG_ACTION_REMOVE && object != NULL) {

            //sslog_remove_property_with_value(individual, prop_type, object);
            sslog_individual_t *ind = sslog_new_individual_from_triple(individual_triple);
            add_property_to_changes(changes, ind,
                    prop_type, NULL, object, SSLOG_ACTION_REMOVE);

            // New triple was added.
        } else if (current_action == SSLOG_ACTION_INSERT && object != NULL) {

            sslog_individual_t *ind = sslog_new_individual_from_triple(individual_triple);
            add_property_to_changes(changes, ind,
                    prop_type, object, NULL, SSLOG_ACTION_INSERT);

        } else if (object != NULL) { // Triple was changed.
            // To update we need to get an old value of the property.
            void *old_data = old_triple->object;

            if (sslog_property_is_object(prop_type) == true) {
                old_data = sslog_store_get_individual(sslog_session_get_default()->store, old_triple->object);
            }

            sslog_individual_t *ind = sslog_new_individual_from_triple(individual_triple);
            add_property_to_changes(changes, ind, prop_type, object, old_data, SSLOG_ACTION_UPDATE);
        } else {

            SSLOG_DEBUG_FUNC("can't process property(%s)", prop_type->entity.uri);
        }
    }

    list_free_with_nodes(new_triples_list, NULL);
    list_free_with_nodes(old_triples_list, NULL);

}


/**
 * @brief Convers triple to list with triples using filter.
 *
 * Do not free data in the result list,
 * the function doesn't creates new triples.
 *
 * Filter for triples element: subject AND predicate AND object <br>
 * NULL is used as any value.
 *
 * @param[in] triples triples to convert.
 * @param[in] subject filter for the subject of triples.
 * @param[in] predicate filter for the predicate of triples.
 * @param[in] object filter for the object of triples.
 *
 * @return new list with triples on success, or NULL otherwise.
 */
static list_t *convert_triples_to_list_by_filter(list_t *triples,
                                                 char *subject, char *predicate, char *object)
{
    //TODO: remove
    if (false) {
    update_individual(NULL, NULL, NULL, NULL, NULL);
    }

    list_t *triples_list = list_new();

    if (list_is_null_or_empty(triples)) {
        return triples_list;
    }

    list_head_t *list_walker = NULL;
    list_for_each(list_walker, &triples->links) {
        list_t *node = list_entry(list_walker, list_t, links);
        sslog_triple_t *triple = (sslog_triple_t *) node->data;

        if ((subject == NULL || strncmp(triple->subject,
                subject, SSLOG_TRIPLE_SUBJECT_LEN) == 0)

                && (predicate == NULL || strncmp(triple->predicate,
                predicate, SSLOG_TRIPLE_PREDICATE_LEN) == 0)

                && (object == NULL || strncmp(triple->object,
                object, SSLOG_TRIPLE_OBJECT_LEN) == 0)) {

            list_add_data(triples_list, triple);
        }
    }

    return triples_list;
}


/**
 * @brief Gets nodes and remove them from the lists.
 *
 * @param[in] old_triples list with old triples, that were removed.
 * @param[in] new_triples list with new triples, that were inserted.
 * @param[out] new_triple next new triple.
 * @param[out] old_triple newxt old triple.
 *
 * @return action that was performed (insert, remove, update) under triples.
 */
static sslog_action_type next_triples(list_t *new_triples, list_t *old_triples,
        sslog_triple_t **new_triple, sslog_triple_t **old_triple)
{
    // Get first nodes
    list_t *new_first_triple = list_get_first_prev_node(new_triples);
    list_t *old_first_triple = list_get_first_prev_node(old_triples);

    // Remove one of the node.
    if (new_first_triple != NULL) {

        *new_triple = (sslog_triple_t *) new_first_triple->data;
        list_del_and_free_node(new_first_triple, NULL);

    } else if (old_first_triple != NULL) {

        *old_triple = (sslog_triple_t *) old_first_triple->data;
        list_del_and_free_node(old_first_triple, NULL);

    }

    // If there are only old triples, then get the action and return it.
    // Old triple was removed from the smart space.
    if (*old_triple != NULL) {
        return get_action_type(NULL, *old_triple);
    }

    list_head_t *list_walker = NULL;
    list_head_t *cur_pos = NULL;

    // If there is new triple, then try to find triple with the
    // same predicate in old triples - triples were updated.

    list_for_each_safe(list_walker, cur_pos, &old_triples->links)
    {
        list_t *node = list_entry(list_walker, list_t, links);
        sslog_triple_t *triple = (sslog_triple_t *) node->data;

        if (strncmp(triple->predicate, (*new_triple)->predicate, SS_PREDICATE_MAX_LEN) == 0) {
            *old_triple = triple;
            list_del_and_free_node(node, NULL);

            break;
        }
    }

    return get_action_type(*new_triple, *old_triple);
}

/**
 * @brief Determines an action.
 *
 * When triples has come from the smart space we can get a status of operation
 * (insert, delete, update) checking triples: <br />
 * - there are new and old values - triple was updated;  <br />
 * - there is only new value - triple was inserted;  <br />
 * - there is only old value - triple was removed.  
 *
 * @param new_triple triple that was inserted to the smart space.
 * @param old_triple triple that was removed from the smart space.
 *
 * @return return type of the action.
 */
static sslog_action_type get_action_type(sslog_triple_t *new_triple, sslog_triple_t *old_triple)
{
    if (new_triple != NULL && old_triple != NULL) {
        return SSLOG_ACTION_UPDATE;
    } else if (new_triple == NULL && old_triple != NULL) {
        return SSLOG_ACTION_REMOVE;
    } else if (new_triple != NULL && old_triple == NULL) {
        return SSLOG_ACTION_INSERT;
    }

    return SSLOG_ACTION_UNKNOWN;
}

/**
 * @brief Tries to find individual with given UUID or creates new individual.
 *
 * @param uuid UUID of the individual.
 *
 * @return individual (founded or new) on success, or NULL otherwise.
 */
static sslog_individual_t *get_or_create_individual_with_uuid(sslog_node_t *node, char *uri)
{
    SSLOG_DEBUG_START

    sslog_individual_t *ind = sslog_store_get_individual(sslog_session_get_default()->store, uri);

    if (ind == NULL) {

        list_t *triples = NULL;
        sslog_triple_t * triple_req = sslog_new_triple_detached(uri, SSLOG_TRIPLE_RDF_TYPE, SS_RDF_SIB_ANY,
                SSLOG_RDF_TYPE_URI, SSLOG_RDF_TYPE_URI);

        // Try to get triple with rdf:type: uuid - rdf:type - *
        sslog_kpi_query_triple(node->kpi, triple_req, &triples);

        sslog_free_triple(triple_req);

        // There is no information about type in the smart space.
        if (triples == NULL) {
            return NULL;
        }

         sslog_triple_t *rdftype = (sslog_triple_t *) list_get_first_prev_node(triples)->data;

         ind = sslog_new_individual_from_triple(rdftype);
    }

    SSLOG_DEBUG_END

    return ind;
}

static void copy_ss_info( ss_info_t *destination, const ss_info_t *source)
{
    destination->free = source->free;
    destination->transaction_id = source->transaction_id;
    destination->socket = source->socket;

    destination->ssap_msg[0] = '\0';
    destination->ss_errno = 0;
    
    strncpy(destination->node_id, source->node_id, SS_NODE_ID_MAX_LEN);
    strncpy(destination->space_id, source->space_id, SS_SPACE_ID_MAX_LEN);

#ifdef ACCESS_NOTA
    strncpy(destination->address.sid, source->address.sid, MAX_SID_LEN);
#else
    strncpy(destination->address.ip, source->address.ip, MAX_IP_LEN);
#endif

    destination->address.port = source->address.port;
}


#ifdef MTENABLE

/** @brief Starts (if it is needed) asynchronous subscription process. */
static void start_async_sbrc_process()
{
	fflush(stdout);

    if (async_sbcr_thread_info.status == THREAD_RUNNING) {
		g_is_async_process_need_to_stopped = false;
        return;
    }

    if (async_sbcr_thread_info.status == THREAD_ENDED) {
        // Join to relese resources, that were used for the thread.
        pthread_join(async_sbcr_thread_info.thread, NULL);
    }

    // Set flag to get permission for asynchronous subscription to work.
    g_is_async_process_need_to_stopped = false;

    if (pthread_create(&async_sbcr_thread_info.thread, NULL,
            propcess_async_subscription, (void *) &g_async_subscriptions) == 0) {
        async_sbcr_thread_info.status = THREAD_RUNNING;
    } else {
        async_sbcr_thread_info.status = THREAD_UNINITIALIZED;
    }

    SSLOG_DEBUG_FUNC("Start async propcess, code = %i",async_sbcr_thread_info.status );
}

/** @brief Stops asynchronous subscription process. */
static void stop_async_sbrc_process()
{
    if (async_sbcr_thread_info.status == THREAD_UNINITIALIZED) {
        return;
    }

    // Set flag to stop asynchronous function.
    g_is_async_process_need_to_stopped = true;

    // Wait while asynchronous subscription working (only from other threads).
	if (pthread_equal(pthread_self(), async_sbcr_thread_info.thread) == 0) {
		pthread_join(async_sbcr_thread_info.thread, NULL);
		async_sbcr_thread_info.status = THREAD_UNINITIALIZED;
	}

    SSLOG_DEBUG_FUNC("Async propcess has stopped.");
}

/**
 * @brief Checks: asynchronous subscription process
 * need to be stopped or not.
 *
 * @return true if process need to be stopped.
 */
static bool is_async_sbrc_propcess_need_to_stoped()
{
    return g_is_async_process_need_to_stopped;
}

/** @brief Process asynchronous subscriptions..
 *
 * @param data information for process.
 *
 * @return NULL, it is for pthread.
 */
static void *propcess_async_subscription(void *data)
{
    SSLOG_DEBUG_START

    list_t *subscriptions = (list_t*) data;
    list_t *list_walker = NULL;

    // Checking flag, sometimes it usefull to stop subscription...
    while (true) {
        pthread_mutex_lock(&g_async_subscription_mutex);

        if (is_async_sbrc_propcess_need_to_stoped() == true) {
            async_sbcr_thread_info.status = 0;
            pthread_mutex_unlock(&g_async_subscription_mutex);
            break;
        }

        if (g_to_first_async_subscription == true) {
            g_to_first_async_subscription = false;
            list_walker = NULL;
        }

        sslog_subscription_t *sbcr = NULL;

        // Get next subscription from the global list.
        list_walker = list_get_next_node(subscriptions, list_walker,
                (void **) &sbcr);

        // When there are no containers: break
        if (list_walker == NULL) {
            async_sbcr_thread_info.status = 0;
            pthread_mutex_unlock(&g_async_subscription_mutex);
            break;
        }

        int status = process_subscription(sbcr, KPLIB_SBCR_ASYNC_WAITING_TIMEOUT);

        pthread_mutex_unlock(&g_async_subscription_mutex);

		// Error
		if (status == -1 && sbcr->onerror_handler != NULL) {
            sbcr->onerror_handler(sbcr, sslog_error_get_last_code());
		} else if (status == 1) { // Indication
			if (sbcr->changed_handler != NULL) {
				sbcr->changed_handler(sbcr);
			}
		}

     //TODO: sleep or NOT
     //   usleep(KPLIB_SBCR_ASYNC_PROCESS_SLEEP);
    }

    SSLOG_DEBUG_END

	pthread_exit(NULL);

	return NULL;
}

#endif



static void remove_node(sslog_subscription_t *subscription)
{
    // Remove subscription from node.
    if (subscription->linked_node != NULL) {
        list_del_and_free_nodes_with_data(&subscription->linked_node->subscriptions, subscription, NULL);
    }

    subscription->linked_node = NULL;
}






