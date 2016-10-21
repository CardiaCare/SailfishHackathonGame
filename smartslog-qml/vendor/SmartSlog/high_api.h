/**
 * @file    high_api.h
 * @author  Aleksandr A. Lomov <lomov@cs.karelia.ru>
 * @date    2 June, 2015
 * @brief   Defines API functions to implement high-level access to smart space and for local store.
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
 * <b>Copyright (C) 2015 by PetrSU SmartSlog team.</b>
 */

#include "entity.h"
#include "kpi_api.h"

#ifndef _SSLOG_HIGH_API_H
#define	_SSLOG_HIGH_API_H


/// @cond NO_DOCS
#if !defined(SSLOG_EXTERN)
#if defined(SSLOG_EXPORT)
#define SSLOG_EXTERN __declspec(dllexport)
#elif defined (SSLOG_IMPORT)
#define SSLOG_EXTERN __declspec(dllimport)
#else
#define SSLOG_EXTERN
#endif
#endif
/// @endcond NO_DOCS


#ifdef	__cplusplus
extern "C" {
#endif


/******************************* Definitions *********************************/
/**************************** External functions *****************************/
/// @cond DOXY_EXTERNAL_API

/**
 * @defgroup Individual Functions for individuals
 * Functions for individuals properties and for whole individuals.
 * Local functions make changes only in local store, Node (remote) functions
 * make changes in the smart space and in the local store.
 */

/******************* Entity-triple converting functions **********************/
/**
 * @defgroup Entity-triple-converting Entity-triple converting functions
 * Functions are used to get base-triple of entity or get entity that is represented by a triple.
 * @{
 */

/**
 * @brief Gets an entity (class, property, individual) from triple.
 *
 * If the triple describes a class (http://www.w3.org/2000/01/rdf-schema#Class)
 * then class structure will be returned.
 *
 * If the triple describes a property (http://www.w3.org/1999/02/22-rdf-syntax-ns#Property)
 * then property structure will be returned. 
 *
 * If the triple describes an individual (URI - http://www.w3.org/1999/02/22-rdf-syntax-ns#type - classURI)
 * then the individual structure will be returned.
 *
 * If the triple describes a property value of some individuals (URI - propertyURI - URI/Data)
 * then the owner (individual) of this property will be returned. In this way the individual
 * can be returned if there is a rdf:type triple for the individual with 'URI'.
 *
 * @param triple. Triple to convert.
 * @return entity structure on success or NULL otherwise (incorrect triple).
 */
SSLOG_EXTERN sslog_entity_t *sslog_entity_from_triple(sslog_triple_t *triple);


/**
 * @brief Gets individual from triple.
 *
 * If the triple describes an individual (URI - http://www.w3.org/1999/02/22-rdf-syntax-ns#type - classURI)
 * then the individual structure will be returned.
 *
 * If the triple describes a property value of some individuals (URI - propertyURI - URI/Data)
 * then the owner (individual) of this property will be returned. In this way the individual
 * can be returned if there is a rdf:type triple for the individual with 'URI'.
 *
 * @param triple. Triple to convert.
 * @return individual on success or NULL otherwise (triple not describes individual, no rdf:type for individual).
 */
SSLOG_EXTERN sslog_individual_t* sslog_individual_from_triple(sslog_triple_t *triple);


/**
 * @brief Gets class from triple.
 *
 * If the triple describes a class
 * (ClassURI - http://www.w3.org/1999/02/22-rdf-syntax-ns#type - http://www.w3.org/2000/01/rdf-schema#Class)
 * or individual (IndividualURI - http://www.w3.org/1999/02/22-rdf-syntax-ns#type - classURI)
 * then the property structure will be returned.
 *
 * @param triple. Triple to convert.
 * @return class on success or NULL otherwise (triple not describes class).
 */
SSLOG_EXTERN sslog_class_t *sslog_class_from_triple(sslog_triple_t *triple);


/**
 * @brief Gets property from triple.
 *
 * If the triple describes a property
 * (PropertyURI - http://www.w3.org/1999/02/22-rdf-syntax-ns#type - http://www.w3.org/1999/02/22-rdf-syntax-ns#Property)
 * or property value (IndividualURI - PropertyURI - Object/Data)
 * then the property structure will be returned.
 *
 * @param triple. Triple to convert.
 * @return property on success or NULL otherwise (triple not describes property).
 */
SSLOG_EXTERN sslog_property_t *sslog_property_from_triple(sslog_triple_t *triple);


/**
 * @brief Gets rdf:type triple of entity (class, property, individual).
 *
 * If the entity is a class then class rdf:type triple
 * (ClassURI - http://www.w3.org/1999/02/22-rdf-syntax-ns#type - http://www.w3.org/2000/01/rdf-schema#Class)
 * will be returned.
 *
 * If the entity is a property then property rdf:type triple
 * (PropertyURI - http://www.w3.org/1999/02/22-rdf-syntax-ns#type - http://www.w3.org/1999/02/22-rdf-syntax-ns#Property)
 * will be returned.
 *
 * If the entity is an individual then individual rdf:type triple
 * (IndividualURI - http://www.w3.org/1999/02/22-rdf-syntax-ns#type - ClassURI)
 * will be returned.
 *
 * @param triple. Triple to convert.
 * @return entity structure on success or NULL otherwise (incorrect entity).
 */
SSLOG_EXTERN sslog_triple_t* sslog_entity_to_triple(sslog_entity_t *entity);


/**
 * @brief Gets rdf:type triple of individual.
 *
 * Individual is described with rdf:type triple (URI - http://www.w3.org/1999/02/22-rdf-syntax-ns#type - classURI)).
 *
 * @param individual. Individual to convert.
 * @return triple on success or NULL otherwise (individual is NULL).
 */
SSLOG_EXTERN sslog_triple_t * sslog_individual_to_triple(sslog_individual_t *individual);


/**
 * @brief Gets rdf:type triple of property.
 * Rdf:type triple of property:
 * PropertyURI - http://www.w3.org/1999/02/22-rdf-syntax-ns#type - http://www.w3.org/1999/02/22-rdf-syntax-ns#Property.
 * @param individual. Property to convert.
 * @return triple on success or NULL otherwise (property is NULL).
 */
SSLOG_EXTERN sslog_triple_t * sslog_property_to_triple(sslog_property_t *property);


/**
 * @brief Gets rdf:type triple of class.
 *
 * Rdf:type triple of class:
 * ClassURI - http://www.w3.org/1999/02/22-rdf-syntax-ns#type - http://www.w3.org/2000/01/rdf-schema#Class.
 *
 * @param individual. Property to convert.
 * @return triple on success or NULL otherwise (property is NULL).
 */
SSLOG_EXTERN sslog_triple_t * sslog_class_to_triple(sslog_class_t *ont_class);
/** @} */ // End of Entity-triple-converting group.
/*****************************************************************************/



/******************* Functions for individual properties *********************/
/**
 * @defgroup Individual-local-property Local functions for individual properties
 * @ingroup Individual
 * Functions are used to local opeation (in local store) with individual properties.
 * @{
 */

/**
 * @brief Gets individial property value (data or object).
 * If the property has an data-type, then the string will be returned,
 * if the property has an object-type, then an individual will be returned.
 * If the individual has several propertirs for given property when a first founded value
 * will be returned.
 *
 * Function sets information about errors (#errors.h).
 *
 * @param individual. Individual to get property value.
 * @param property. Property to get value.
 * @return string (char *) for data-property or individual for object-property on success and
 * NULL otherwise (error or value not found).
 */
SSLOG_EXTERN const void *sslog_get_property(sslog_individual_t *individual, sslog_property_t *property);


/**
 * @brief Gets all values for individual property.
 * If the property has an data-type, then the strings will be returned,
 * if the property has an object-type, then an individuals will be returned.
 *
 * Returned list MUST be deleted without deleting datas (use #list_free_with_nodes).
 *
 * Function sets information about errors (#errors.h).
 *
 * @param individual. Individual to get property value.
 * @param property. Property to get value.
 * @return list with strings (char *) for data-property or list with individuals for object-property on success,
 * empty list when there are no values for property and NULL otherwise (error).
 */
SSLOG_EXTERN list_t *sslog_get_properties(sslog_individual_t *individual, sslog_property_t *property);


/**
 * @brief Adds a property for individual.
 * Property can be data or object, for data property value must be a string (char *),
 * for object proeprty value must be an individual. If the inividual has a given property
 * with the same value, then function does nothing.
 *
 * Function sets information about errors (#errors.h).
 *
 * @param individual. Individual to add a property.
 * @param property. Property to add.
 * @param value. Value of property. String for data-property, individual for object-property.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_insert_property(sslog_individual_t *individual, sslog_property_t *property, void *value);


/**
 * @brief Removes value of individual property.
 * Remoives data or object property with the given value.
 * For data-property value is a string (char *), for object-property is an individual.
 * If the inividual hasn't a property with the given value, then function does nothing.
 *
 * Function sets information about errors (#errors.h).
 *
 * @param individual. Individual to remove a property.
 * @param property. Property to remove.
 * @param value. Value of property. String for data-property, individual for object-property.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_remove_property(sslog_individual_t *individual, sslog_property_t *property, void *value);


/**
 * @brief Removes all values if individual property.
 * Remoives all values of data or object property.
 * If the inividual hasn't a given property values, then function does nothing.
 *
 * Function sets information about errors (#errors.h).
 *
 * @param individual. Individual to remove a property values.
 * @param property. Property to remove.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_remove_properties(sslog_individual_t *individual, sslog_property_t *property);


/**
 * @brief Updates an individual property value.
 * For data-property values are a strings (char *), for object-property are individuals.
 * If the current_value is NULL, then function works as #sslog_add_property.
 * If the new_value is NULL, then function works as #sslog_remove_property.
 *
 * Function sets information about errors (#errors.h).
 *
 * @param individual. Individual to update property value.
 * @param property. Property to update.
 * @param current_value. Current value of the property.
 * @param new_value. New value of the property.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_update_property(sslog_individual_t *individual, sslog_property_t *property, void *current_value, void *new_value);
/** @} */ // End of Individual-local-property group.
/*****************************************************************************/


/**************** Node functions for individual properties *******************/
/**
 * @ingroup Individual
 * @defgroup Individual-node-property Node functions for individual properties
 * Functions are used to remote opeations (in smart space) with individual properties.
 * @{
 */

/**
 * @brief Gets one property from the smart space and inserts it to local store.
 * Function gets only one first founded property. Founded property (  triple ) will be stored too.
 * If the object property is getting,   then the rdf:type triple for individual (  property value )
 * must be in the local store or in the smart space,   otherwise individual can't be created and
 * function returns NULL.
 *
 * Function sets an information about errors (  see #errors.h ).
 *
 * @param[in] node. Node to work with the smart space.
 * @param[in] individual. Individual to get property.
 * @param[in] property. Property to get (  object ot data ).
 * @return value of property (string for data-property/individual for object) on success or NULL otherwise (error or not found).
 */
SSLOG_EXTERN const void* sslog_node_get_property(sslog_node_t *node, sslog_individual_t *individual, sslog_property_t *property);


/**
 * @brief Inserts property to the smart space and to the local store.
 * Function converts property to triple and inserts it into smart space and store triple in the local store.
 *
 * Function sets an information about errors (see #errors.h).
 *
 * @param node. Node to work with the smart space.
 * @param individual. Individual to set property.
 * @param property. Property of individual to set.
 * @param value. Value of the property, can be a string (for data-properties) or individual (for object-properties).
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_node_insert_property(sslog_node_t *node, sslog_individual_t *individual, sslog_property_t *property,
                                            void *value);


/**
 * @brief Removes property from the smart space nd from local store.
 * Function converts property to triple and removes it from smart space and from the local store.
 * If the value of property is NULL then all such properties will be removed.
 *
 * Function sets an information about errors (see #errors.h).
 *
 * @param node. Node to work with the smart space.
 * @param individual. Individual to remove property.
 * @param property. Property of individual to remove.
 * @param value. Value of the property,
 * can be a string (for data-properties), individual (for object-properties)
 * or NULL for removing all such properties.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_node_remove_property(sslog_node_t *node, sslog_individual_t *individual, sslog_property_t *property,
                                            void *value);


/**
 * @brief Updates property in the smart space and in the local store.
 * Function converts property to triple and updates value from current to new in the smart space and in the local store.
 *
 * Function sets an information about errors (see #errors.h).
 *
 * @param node. Node to work with the smart space.
 * @param individual. Individual to update property.
 * @param property. Property to update.
 * @param current_value. Current value of the property, this value will be removed.
 * @param new_value. New value of the property, this value will be inserted to the smart space and added to local store.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_node_update_property(sslog_node_t *node, sslog_individual_t *individual, sslog_property_t *property,
                                            void *current_value, void *new_value);
/** @} */ // End of Individual-node-property group.
/*****************************************************************************/


/********************** Functions for whole individual ***********************/
/**
 * @defgroup Individual-local-whole Local functions for whole individual
 * @ingroup Individual
 * Functions are used to local opeations (in local store) with whole individual.
 * @{
 */

/**
 * @brief Gets an individual by URI.
 * @param[in] uri. URI of the individual.
 * @return individual on success or NULL otherwise (error, no triple that describes individual).
 */
SSLOG_EXTERN sslog_individual_t *sslog_get_individual(const char *uri);


/**
 * @brief Removes individual from the local store.
 * Functiosn converts individual to triples and removes it.
 * Individual structure CANNOT BE used any more.
 *
 * Function sets information about errors (#errors.h).
 *
 * @param individual. Individual to remove.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_remove_individual(sslog_individual_t *individual);
/** @} */ // End of Individual-local-whole group.
/*****************************************************************************/


/******************** Node functions for whole individual ********************/
/**
 * @defgroup Individual-node-whole Node functions for whole individual
 * @ingroup Individual
 * Functions are used to remote opeations (in smart space) with whole individual.
 * @{
 */

/**
 * @brief Get all individual by uri.
 * Function get individual with given URI (rdf:type triple with object equals URI).
 *
 * Function sets an information about errors (see #errors.h).*
 *
 * @param node. Node to work with the smart space.
 * @param uri. URI of the individual.
 * @return individual on success or NULL otherwise (error or not found).
 */
SSLOG_EXTERN sslog_individual_t * sslog_node_get_individual_by_uri(sslog_node_t *node, const char *uri);


/**
 * @brief Get all individual by class.
 * Function get all individuals with rdf:type where object equals class URI.
 *
 * Function sets an information about errors (see #errors.h).*
 *
 * @param node. Node to work with the smart space.
 * @param ont_class. Base class of individuals.
 * @return list with individuals on success or NULL otherwise.
 */
SSLOG_EXTERN list_t* sslog_node_get_individuals_by_class(sslog_node_t *node, sslog_class_t *ont_class);


/**
 * @brief Gets all properties about individual.
 * The function gets triples that are represent an individual and properties.
 * If the individual has object properties
 * (links to other individuals) then only rdf:type triple of linked individual
 * will be queried from the smart space.
 * All triples will be stored locally.
 *
 * Function sets an information about errors (see #errors.h).
 *
 * @param[in] node. Node to work with the smart space.
 * @param[in] individual. Individual to populate.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_node_populate(sslog_node_t *node, sslog_individual_t *individual);


/**
 * @brief Inserts the individual to the smart space.
 * The function gets triples that are represent an individual and properties and
 * inserts it to the smart space. If the individual has object properties
 * (links to other individuals) then only rdf:type triple of linked individual
 * will be inserted.
 *
 * Function sets an information about errors (see #errors.h).
 *
 * @param[in] node. Node to work with the smart space.
 * @param[in] individual. Individual to insert.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_node_insert_individual(sslog_node_t *node, sslog_individual_t *individual);


/**
 * @brief Removes the individual from the smart space and all properties from local store.
 * The function gets triples that are represent an individual and properties and
 * removes it from the smart space.
 * This function does not remove rdf:type triple (i.e. individual)
 * from the local store and individual structure can be used after calling function.
 * To fully remove individual from the smart space and local store use
 * #sslog_node_remove_individual_with_local function.
 *
 * Linked individuals with object properties will not be removed from the smart space.
 *
 * Function sets an information about errors (see #errors.h).
 *
 * @param[in] node. Node to work with the smart space.
 * @param[in] individual. Individual to insert.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 *
 * @see sslog_node_remove_individual_with_local
 */
SSLOG_EXTERN int sslog_node_remove_individual(sslog_node_t *node, sslog_individual_t *individual);


/**
 * @brief Removes the individual from the smart space and from local store.
 * The function gets triples that are represent an individual and properties and
 * removes it from the smart space and local store.
 * Individual structure CANNOT BE used any more.
 *
 * Linked individuals with object properties will not be removed from the smart space.
 *
 * Function sets an information about errors (see #errors.h).
 *
 * @param[in] node. Node to work with the smart space.
 * @param[in] individual. Individual to insert.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_node_remove_individual_with_local(sslog_node_t *node, sslog_individual_t *individual);
/** @} */ // End of Individual-node-whole group.
/*****************************************************************************/

/// @endcond
//  endcond of DOXY_EXTERNAL_API


#ifdef	__cplusplus
}
#endif

#endif	/* _SSLOG_HIGH_API_H */
