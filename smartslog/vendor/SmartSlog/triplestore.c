/**
 * @file repository.c
 * @Author Alexandr A. Lomov <lomov@cs.karelia.ru>
 * @Author Pavel I. Vanag <vanag@cs.karelia.ru>
 * @date   05 December, 2012
 * @brief  Contains functions for work with repository.
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
 * Contains functions for work with reository.
 * This file is part of SmartSlog KP Library.
 *
 * Copyright (C) 2009 - SmartSlog. All rights reserved.
 *
 */

#include "triplestore.h"

#include <stdlib.h>
#include <string.h>

#include "kpi_api.h"
#include "entity_internal.h"
#include "triple_internal.h"

#include "utils/errors.h"
#include "utils/list.h"
#include "utils/util_func.h"


#define SSLOG_STORE_CAST (list_t *)
#define SSLOG_FREE_FUNC_CAST (void (*)(void*))

#define sslog_store_to_list(store, variable) list_t *variable = (list_t *) store;


static bool sslog_store_match_templates(sslog_triple_t *triple, list_t *triples_templates);

/*****************************************************************************/
/**************************** External functions *****************************/
/// @cond EXTERNAL_FUNCTIONS


/****************************** Implementations *********************************/
/***************************** External functions *******************************/
/// @cond DOXY_EXTERNAL_API
list_t *sslog_store_get_individual_triples(sslog_store_t *store, sslog_individual_t *individual, bool with_rdftype)
{
    sslog_store_to_list(store, store_list);

    list_t *ind_triples = sslog_store_query_triples_by_data(store_list, individual->entity.uri, SSLOG_TRIPLE_ANY, SSLOG_TRIPLE_ANY,
                                                           SSLOG_RDF_TYPE_URI, SSLOG_RDF_TYPE_URI, -1);

    if (ind_triples == NULL) {
        SSLOG_DEBUG_FUNC("Can't query triples for individual: '%s'.", individual->entity.uri);
        return NULL;
    }


    // Find object properties: triples that link to other triples (individuals),
    // and add rdf:type triples of linked individuals to result triples list.
    // Property-Triples  must have URI type for object. Object is a URI to other entity.
    list_head_t *list_walker = NULL;
    list_for_each(list_walker, &ind_triples->links) {
        list_t *node = list_entry(list_walker, list_t, links);
        sslog_triple_t *triple = (sslog_triple_t *) node->data;

        // Skip the tripele if the triple is not represent an property value of individual
        // or type of the object is not URI.
        if (sslog_triple_is_property_value(triple) != true
                || triple->object_type != SSLOG_RDF_TYPE_URI) {
            continue;
        }

        // Get a individual rdf:type triple.
        sslog_triple_t *type_triple = NULL;

        if (with_rdftype == true) {
            type_triple = sslog_store_get_rdftype(store, triple->object);
        }

        if (type_triple != NULL) {
            list_add_data(ind_triples, type_triple);
        }
    }

    return ind_triples;
}

/// @endcond
// DOXY_EXTERNAL_API


///**
// * @brief Get class from repository by given classtype.
// *
// * @param classtype name of classtype.
// *
// * @return class from repository or NULL if it not found.
// */
//SSLOG_EXTERN const class_t* sslog_repo_get_class_by_classtype(const char *classtype)
//{
//    list_t *repository = get_global_repository_by_rtti(RTTI_CLASS);

//    if (is_str_null_or_empty(classtype) == true
//            || list_is_null_or_empty(repository) == 1) {
//        return NULL;
//    }

//    // Gets classes and checks their classtypes.
//    list_head_t *list_walker = NULL;
//    list_for_each (list_walker, &repository->links) {
//         list_t *list = list_entry(list_walker, list_t, links);
//         class_t *test_class = (class_t *) list->data;

//         if (strcmp(test_class->classtype, classtype) == 0) {
//            return test_class;
//        }
//    }

//    return NULL;
//}

///**
// * @brief Remove entity from repository.
// *
// * @param entity individual, class or property.
// */
//SSLOG_EXTERN void sslog_repo_remove_entity(void *entity)
//{
//    int rtti = sslog_get_rtti_type(entity);

//    if (rtti <= RTTI_MIN_VALUE) {
//        return;
//    }

//    list_t *repository = get_global_repository_by_rtti(rtti);

//    list_del_and_free_nodes_with_data(repository, entity, NULL);
//}


//void sslog_repo_clean_all()
//{
//    list_free_with_nodes(g_individuals_repository, (void (*)(void *)) sslog_free_individual);
//    g_individuals_repository = NULL;

//    list_free_with_nodes(g_classes_repository, (void (*)(void *)) sslog_free_class);
//    g_classes_repository = NULL;

//    list_free_with_nodes(g_properties_repository, (void (*)(void *)) sslog_free_property);
//    g_properties_repository = NULL;
//}

/// @endcond


/******************************************************************************/
/***************************** Static functions *******************************/







sslog_store_t* repo_new()
{
    return list_new();
}


sslog_store_t *sslog_store_new()
{
    return list_new();
}


void sslog_store_free(sslog_store_t *store)
{
    list_t *store_list = SSLOG_STORE_CAST(store);

    list_free_with_nodes(store_list, SSLOG_FREE_FUNC_CAST sslog_free_triple_force);

//    list_head_t *list_walker = NULL;
//    list_head_t *position = NULL;
//    list_for_each_safe(list_walker, position, &store_list->links) {
//        list_t *node = list_entry(list_walker, list_t, links);
//        sslog_triple_t *triple = (sslog_triple_t *) node->data;
//        list_del_and_free_node(node, SSLOG_FREE_FUNC_CAST sslog_free_triple_force);
//    }

//    list_free(store_list);

    //list_free_with_nodes(SSLOG_STORE_CAST store, SSLOG_FREE_FUNC_CAST );
}


int sslog_store_add_triple(sslog_store_t *store, sslog_triple_t *triple)
{
    list_t *list = SSLOG_STORE_CAST(store);

    if (list_has_data_with_equals(list, triple, LIST_CAST_TO_EQUAL_BOOL_FUNC sslog_equal_triples) == true) {
        return SSLOG_ERROR_ALREADY_EXISTS;
    }
    
    list_add_data(list, triple);
    sslog_triple_stored(triple, true);
    
    SSLOG_DEBUG_FUNC("New triple: %s - %s - %s (%d)", triple->subject, triple->predicate, triple->object, triple->object_type);
    SSLOG_DEBUG_FUNC("Stored triples: %d", list_count(list));

    return SSLOG_ERROR_NO;
}



sslog_triple_t* sslog_store_add_get_triple(sslog_store_t *store, sslog_triple_t *triple)
{
    list_t *list = SSLOG_STORE_CAST(store);

    list_t *node = list_get_node_with_equals(list, triple, LIST_CAST_TO_EQUAL_BOOL_FUNC sslog_equal_triples);

    if (node != NULL) {
        SSLOG_DEBUG_FUNC("Triple already stored: %s - %s - %s (%d)", triple->subject, triple->predicate, triple->object, triple->object_type);
        return (sslog_triple_t *)node->data;
    }

    list_add_data(list, triple);
    sslog_triple_stored(triple, true);

    SSLOG_DEBUG_FUNC("New triple: %s - %s - %s (%d)", triple->subject, triple->predicate, triple->object, triple->object_type);
    SSLOG_DEBUG_FUNC("Stored triples: %d", list_count(list));

    return triple;
}


sslog_triple_t *sslog_store_get_triple_by_data(sslog_store_t *store, const char *subject, const char *predicate, const char *object,
                                               sslog_rdf_type subject_type, sslog_rdf_type object_type) {
    list_t *store_list = SSLOG_STORE_CAST(store);

    sslog_triple_t *triple_data = sslog_new_triple_detached(subject, predicate, object, subject_type, object_type);

    list_head_t *list_walker = NULL;
    list_for_each(list_walker, &store_list->links) {
        list_t *node = list_entry(list_walker, list_t, links);
        sslog_triple_t *triple = (sslog_triple_t *) node->data;

        if (sslog_compare_triple_with_any(triple_data, triple, SSLOG_TRIPLE_ANY) == true) {
            sslog_free_triple(triple_data);
            return triple;
        }
    }

    sslog_free_triple(triple_data);
    return NULL;
}


static bool sslog_store_match_templates(sslog_triple_t *triple, list_t *triples_templates)
{
    list_head_t *list_walker = NULL;
    list_for_each(list_walker, &triples_templates->links) {
        list_t *node = list_entry(list_walker, list_t, links);
        sslog_triple_t *triple_template = (sslog_triple_t *) node->data;

        if (sslog_compare_triple_with_any(triple, triple_template, SSLOG_TRIPLE_ANY) == true) {
            return true;
        }
    }

    return false;
}


int sslog_store_update_triples(sslog_store_t *store, list_t *current_triples, list_t *new_triples)
{
    sslog_store_remove_triples(store, current_triples);
    sslog_store_add_triples(store, new_triples);

    return SSLOG_ERROR_NO;
}



int sslog_store_add_triples(sslog_store_t *store, list_t *triples)
{
    list_t *list = SSLOG_STORE_CAST(store);

    int unstored_count = 0;

    list_head_t *list_walker = NULL;
    list_for_each(list_walker, &triples->links) {
        list_t *node = list_entry(list_walker, list_t, links);
        sslog_triple_t *new_triple = (sslog_triple_t *) node->data;

        if (sslog_triple_is_stored(new_triple) == true) {
            continue;
        }

        if (sslog_triple_is_template(new_triple) == true) {
            ++unstored_count;
            continue;
        }

//        list_t *founded_node = list_get_node_with_equals(list, new_triple, LIST_CAST_TO_EQUAL_BOOL_FUNC sslog_equal_triples);

//        if (founded_node == NULL) {
//            list_add_data(list, new_triple);
//            sslog_triple_stored(new_triple, true);
//        } else {
//            sslog_internal_triple_t *old_triple = (sslog_internal_triple_t *) founded_node->data;

//            if (old_triple->linked_entity != NULL) {
//                sslog_entity_set_link(old_triple->linked_entity, sslog_triple_as_internal(new_triple));
//                old_triple->linked_entity = NULL;
//            }

//            founded_node->data = node->data;
//            sslog_free_triple_force((sslog_triple_t *)old_triple);
//            sslog_triple_stored(new_triple, true);
//        }

        if (sslog_store_exists(list, new_triple) != true) {
            list_add_data(list, new_triple);
            sslog_triple_stored(new_triple, true);
        } else {
            ++unstored_count;
        }

        //sslog_triple_t *old_triple = (founded_node == NULL) ? NULL : (sslog_triple_t *) founded_node->data;

        // The store already has a triple (old-triple) with same data of new-triple:
        // - checks: new and old triple are different pointers;
        // - make a link from node (triples list) to old-triple;
        // - remove new-triple.
        // If the new-triple is not in the store then add it.
        //if ((old_triple != NULL) && (old_triple != new_triple)) {
           // node->data = old_triple;
           // sslog_free_triple(new_triple);
       // } else {
//            list_add_data(list, new_triple);
        //}
    }

    SSLOG_DEBUG_FUNC("Stored triples: %i", list_count(list));

    return unstored_count;
}



int sslog_store_add_triples_with_replace(sslog_store_t *store, list_t *triples)
{
    list_t *list = SSLOG_STORE_CAST(store);

    list_head_t *list_walker = NULL;
    list_for_each(list_walker, &triples->links) {
        list_t *node = list_entry(list_walker, list_t, links);
        sslog_triple_t *new_triple = (sslog_triple_t *) node->data;

        if (sslog_triple_is_stored(new_triple) == true) {
            continue;
        }

        if (sslog_triple_is_template(new_triple) == true) {
            continue;
        }

        list_t *founded_node = list_get_node_with_equals(list, new_triple, LIST_CAST_TO_EQUAL_BOOL_FUNC sslog_equal_triples);

        if (founded_node == NULL) {
            list_add_data(list, new_triple);
            sslog_triple_stored(new_triple, true);
        } else {
            sslog_internal_triple_t *old_triple = (sslog_internal_triple_t *) founded_node->data;

            if (old_triple->linked_entity != NULL) {
                sslog_entity_set_link(old_triple->linked_entity, sslog_triple_as_internal(new_triple));
                old_triple->linked_entity = NULL;
            }

            founded_node->data = node->data;
            sslog_free_triple_force((sslog_triple_t *)old_triple);
            sslog_triple_stored(new_triple, true);
        }

//        if (sslog_store_exists(list, new_triple) != true) {
//            list_add_data(list, new_triple);
//            sslog_triple_stored(new_triple, true);
//        }

        //sslog_triple_t *old_triple = (founded_node == NULL) ? NULL : (sslog_triple_t *) founded_node->data;

        // The store already has a triple (old-triple) with same data of new-triple:
        // - checks: new and old triple are different pointers;
        // - make a link from node (triples list) to old-triple;
        // - remove new-triple.
        // If the new-triple is not in the store then add it.
        //if ((old_triple != NULL) && (old_triple != new_triple)) {
           // node->data = old_triple;
           // sslog_free_triple(new_triple);
       // } else {
//            list_add_data(list, new_triple);
        //}
    }

    SSLOG_DEBUG_FUNC("Stored triples: %i", list_count(list));

    return SSLOG_ERROR_NO;
}



int sslog_store_remove_triples(sslog_store_t *store, list_t *triples_templates)
{
    if (list_is_empty(triples_templates) == true) {
        SSLOG_DEBUG_FUNC("Not riples templates.");
        return SSLOG_ERROR_NO;
    }

    list_t *list_store = SSLOG_STORE_CAST(store);

    list_head_t *current_position = NULL;
    list_head_t *list_walker = NULL;

    list_for_each_safe(list_walker, current_position, &list_store->links) {
        list_t *node = list_entry(list_walker, list_t, links);
        sslog_triple_t *triple = (sslog_triple_t *) node->data;

        if (sslog_store_match_templates(triple, triples_templates) == true) {

            // Check is the list with templates contain a fouded triple.
            if (list_has_data(triples_templates, triple) == 1 ) {
                sslog_triple_stored(triple, false);
                sslog_free_entity(sslog_triple_as_internal(triple)->linked_entity);
                sslog_triple_as_internal(triple)->linked_entity = NULL;
                list_del_and_free_node(node, NULL);
            } else {
                list_del_and_free_node(node, SSLOG_FREE_FUNC_CAST sslog_free_triple_force);
            }
        }

//        list_t *founded_node = list_get_node_with_equals(list_store, remove_triple, LIST_CAST_TO_EQUAL_BOOL_FUNC sslog_equal_triples);

//        sslog_triple_stored(founded_node->data, true);

        // If the pointers of triples are the same,
        // then detach a triple (remove from internal store only).
        // If poiters are different (only triples data are same),
        // then remove node from list and free triple.
//        if (founded_node->data == remove_triple) {
//            list_del_and_free_node(node, NULL);
//        } else {
//            list_del_and_free_node(node, LIST_CAST_TO_FREE_FUNC sslog_free);
//        }
    }

    SSLOG_DEBUG_FUNC("Stored triples: %i", list_count(list_store));

    return SSLOG_ERROR_NO;
}





int sslog_store_update_triples_old(sslog_store_t *store, list_t *current_triples, list_t *new_triples)
{
    list_t *list_store = SSLOG_STORE_CAST(store);

    list_t *insert_triples = list_new();

    list_head_t *current_position = NULL;
    list_head_t *list_walker = NULL;

    list_for_each_safe(list_walker, current_position, &current_triples->links) {
        list_t *node = list_entry(list_walker, list_t, links);
        sslog_triple_t *remove_triple = (sslog_triple_t *) node->data;

        list_t *founded_node = list_get_node_with_equals(list_store, remove_triple, LIST_CAST_TO_EQUAL_BOOL_FUNC sslog_equal_triples);

        sslog_triple_stored((sslog_triple_t *) founded_node->data, true);

        // If the pointers of triples are the same,
        // then detach a triple (remove from internal store only).
        // If poiters are different (only triples data are same),
        // then remove node from list and free triple.
        if (founded_node->data == remove_triple) {
            list_del_and_free_node(node, NULL);
        } else {
            //TODO: was free_triple
            list_del_and_free_node(node, LIST_CAST_TO_FREE_FUNC sslog_free);
        }
    }

    list_walker = NULL;
    list_for_each(list_walker, &new_triples->links) {
        list_t *node = list_entry(list_walker, list_t, links);
        sslog_triple_t *triple = (sslog_triple_t *) node->data;

        if (list_has_data_with_equals(list_store, triple,
                                      LIST_CAST_TO_EQUAL_BOOL_FUNC sslog_equal_triples) != true) {
            list_add_data(insert_triples, triple);
        }
    }




    return 0;
}



bool sslog_store_exists(sslog_store_t *store, sslog_triple_t *triple)
{
    list_t *list_store = SSLOG_STORE_CAST(store);

    return list_has_data_with_equals(list_store, triple, LIST_CAST_TO_EQUAL_BOOL_FUNC sslog_equal_triples);
}


bool sslog_store_exists_template_data(sslog_store_t *store, const char *subject, const char *predicate, const char *object,
                                               sslog_rdf_type subject_type, sslog_rdf_type object_type)
{
	sslog_triple_t *triple = sslog_store_get_triple_by_data(store, subject, predicate, object, subject_type, object_type);

	return (triple == NULL) ? false : true;
}


bool sslog_store_exists_pointer(sslog_store_t *store, sslog_triple_t *triple)
{
    list_t *list_store = SSLOG_STORE_CAST(store);

    return list_has_data_with_equals(list_store, triple, NULL);
}


list_t *sslog_store_query_triples(sslog_store_t *store, list_t *triples_templates)
{
    list_t *list_store = SSLOG_STORE_CAST(store);

    list_t *query_triples = list_new();

    list_head_t *list_walker = NULL;

    list_for_each(list_walker, &list_store->links) {
        list_t *node = list_entry(list_walker, list_t, links);
        sslog_triple_t *triple = (sslog_triple_t *) node->data;

        if (sslog_store_match_templates(triple, triples_templates) == true) {
            list_add_data(query_triples, triple);
        }
    }

    SSLOG_DEBUG_FUNC("Stored triples: %i", list_count(list_store));

    return query_triples;
}


list_t *sslog_store_query_triples_by_data(sslog_store_t *store,
                                          const char *subject, const char *predicate, const char *object,
                                          sslog_rdf_type subject_type, sslog_rdf_type object_type,
                                          int max_triples_count)
{
    if (max_triples_count == 0) {
        return list_new();
    }

    list_t *list_store = SSLOG_STORE_CAST(store);

    list_t *query_triples = list_new();

    list_head_t *list_walker = NULL;

    int counter = 0;

    sslog_triple_t *triple_template = sslog_new_triple_detached(subject, predicate, object, subject_type, object_type);

    list_for_each(list_walker, &list_store->links) {
        list_t *node = list_entry(list_walker, list_t, links);
        sslog_triple_t *triple = (sslog_triple_t *) node->data;

        if (sslog_compare_triple_with_any(triple, triple_template, SSLOG_TRIPLE_ANY) == true) {
            list_add_data(query_triples, triple);
            ++counter;
        }

        if (max_triples_count > 0 && counter >= max_triples_count) {
            break;
        }
    }

    sslog_free_triple(triple_template);

    return query_triples;
}


sslog_triple_t *sslog_store_get_triple(sslog_store_t *store, list_t *triples_templates)
{
    list_t *list_store = SSLOG_STORE_CAST(store);

    list_head_t *list_walker = NULL;

    list_for_each(list_walker, &list_store->links) {
        list_t *node = list_entry(list_walker, list_t, links);
        sslog_triple_t *triple = (sslog_triple_t *) node->data;

        if (sslog_store_match_templates(triple, triples_templates) == true) {
            return triple;
        }
    }

    return NULL;
}




sslog_triple_t *sslog_store_get_rdftype(sslog_store_t *store, const char *uri)
{
    return sslog_store_get_triple_by_data(store, uri, SSLOG_TRIPLE_RDF_TYPE, SSLOG_TRIPLE_ANY,
                                   SSLOG_RDF_TYPE_URI, SSLOG_RDF_TYPE_URI);
}






sslog_individual_t *sslog_store_get_individual(sslog_store_t *store, const char *uri)
{
    if (uri == NULL) {
        return NULL;
    }

    sslog_internal_triple_t *type_triple = (sslog_internal_triple_t *) sslog_store_get_rdftype(store, uri);

    if (type_triple == NULL) {
        return NULL;
    }

    if (type_triple->linked_entity != NULL) {
        return sslog_entity_as_individual(type_triple->linked_entity);
    }

    return sslog_new_individual_from_triple((sslog_triple_t *) type_triple);
}


int sslog_store_remove_individual(sslog_store_t *store, sslog_individual_t *individual)
{
    list_t *triples = list_new();

    sslog_triple_t *triple = sslog_new_triple_detached(individual->entity.uri, SSLOG_TRIPLE_ANY, SSLOG_TRIPLE_ANY,
                                                       SSLOG_RDF_TYPE_URI, SSLOG_RDF_TYPE_URI) ;
    list_add_data(triples, triple);

    // Triple to remove properties, that is link to individual.
    triple = sslog_new_triple_detached(SSLOG_TRIPLE_ANY, SSLOG_TRIPLE_ANY, individual->entity.uri,
                                       SSLOG_RDF_TYPE_URI, SSLOG_RDF_TYPE_URI) ;

    list_add_data(triples, triple);

    sslog_store_remove_triples(store, triples);

    list_free_with_nodes(triples, LIST_CAST_TO_FREE_FUNC sslog_free_triple);

    return SSLOG_ERROR_NO;
}

sslog_property_t *sslog_store_get_property(sslog_store_t *store, const char *uri)
{
    if (uri == NULL) {
        return NULL;
    }

    sslog_internal_triple_t *type_triple = (sslog_internal_triple_t *) sslog_store_get_rdftype(store, uri);

    if (type_triple == NULL) {
        return NULL;
    }

    if (sslog_triple_is_property((sslog_triple_t *) type_triple) == false) {
        SSLOG_DEBUG_FUNC("Triple is not property. Given URI: %s", uri);
        return NULL;
    }

    if (type_triple->linked_entity == NULL) {
        SSLOG_DEBUG_FUNC("Property with URI '%s' is not created.", uri);
        return NULL;
    }

    return sslog_entity_as_property(type_triple->linked_entity);
}


sslog_class_t *sslog_store_get_class(sslog_store_t *store, const char *uri)
{
    if (uri == NULL) {
        return NULL;
    }

    sslog_internal_triple_t *type_triple = (sslog_internal_triple_t *) sslog_store_get_rdftype(store, uri);

    if (type_triple == NULL) {
        return NULL;
    }

    if (sslog_triple_is_class((sslog_triple_t *) type_triple) == false) {
        SSLOG_DEBUG_FUNC("Triple is not property. Given URI: %s", uri);
        return NULL;
    }

    if (type_triple->linked_entity == NULL) {
        SSLOG_DEBUG_FUNC("Property with URI '%s' is not created.", uri);
        return NULL;
    }

    return sslog_entity_as_class(type_triple->linked_entity);
}



