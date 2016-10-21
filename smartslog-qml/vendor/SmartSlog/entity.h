/**
 * @file    entity.h
 * @author  Aleksandr A. Lomov <lomov@cs.karelia.ru>
 * @date    03 March, 2015
 * @brief   Describes ontology entites.
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
 * Describes ontology entities (class, property. individual).
 *
 * This file is part of the SmartSlog project.
 *
 * <b>Copyright (C) 2015 by PetrSU SmartSlog team.</b>
 */

#include "triple.h"
#include "utils/bool.h"


#ifndef _SSLOG_ENTITY_H
#define	_SSLOG_ENTITY_H

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

/******************************** Definitions ********************************/
/******************* Defines, structures, constatnts and etc.*****************/
/** @brief Types of actions that are performed under triples/entites. */
typedef enum sslog_action_type_enum {
    SSLOG_ACTION_UNKNOWN = 0,   /**< Error action. */
    SSLOG_ACTION_UPDATE = 1,    /**< Triple/entity was updated. */
    SSLOG_ACTION_REMOVE = 2,    /**< Triple/entity was deleted. */
    SSLOG_ACTION_INSERT = 4     /**< New triple/entity was inserted. */
} sslog_action_type;

/**
 * @brief Property types.
 * Property can be data or object.
 * Data property links individual with some raw data (value of color, years, number of weeks and etc.).
 * Object property links individual with other individuals.
 */
typedef enum sslog_property_type_enum {
    SSLOG_PROPERTY_TYPE_OBJECT = SSLOG_RDF_TYPE_URI,    /**< Object-type property. */
    SSLOG_PROPERTY_TYPE_DATA = SSLOG_RDF_TYPE_LIT       /**< Data-type property. */
} sslog_property_type;

/** @brief Types of entities. */
typedef enum sslog_entity_type_e {
    SSLOG_ENTITY_UNKNOWN = -1,      /**< Error type */
    SSLOG_ENTITY_INDIVIDUAL = 1,    /**< Individual type. */
    SSLOG_ENTITY_CLASS,             /**< Class type. */
    SSLOG_ENTITY_PROPERTY           /**< Property type. */
} sslog_entity_type;



// It is needed only for doxygen, to generate struct docs without
// internal fields and internal *.h files.
#ifdef DOXY_DATA
/**
 * @brief Structure for representing entity.
 * Entity can be a class, property or individual.
 * Each entity is based on triple.
 * When the triple is deleted, then delete linked with this triple entity too.
 * @see sslog_class_t
 * @see sslog_property_t
 * @see sslog_individual_t
 */
struct sslog_entity_s {
};

/** @brief Structure for representing a class. */
struct sslog_class_s {
};

/**
 * @brief Structure for representing individual.
 * Individual is an instance of class.
 * Individual are used with properties.
 * With properties some data or links are defined for individual.
 */
struct sslog_individual_s {
};

/**
 * @brief Structure for representing a property.
 * Properties are used with individuals to define some row data (data-properties)
 * or some links with other individuals (object-properties).
 */
struct sslog_property_s {
};
#endif

/**
 * @brief Type that represets the entity structure.
 * @see sslog_entity_s
 */
typedef struct sslog_entity_s sslog_entity_t;

/**
 * @brief Type that represets the class structure.
 * @see sslog_class_s
 */
typedef struct sslog_class_s sslog_class_t;

/**
 * @brief Type that represets the individual structure.
 * @see sslog_individual_s
 */
typedef struct sslog_individual_s sslog_individual_t;

/**
 * @brief Type that represets the property structure.
 * @see sslog_property_s
 */
typedef struct sslog_property_s sslog_property_t;


/************************ Entities typecasts macroses ************************/
/**
 * @defgroup Entities-typecasts Typecasts macros for entites
 * Such macroses are used to simplify typecasting of entities.
 * @{
 */
/** @brief Casts entity as individual. */
#define sslog_entity_as_individual(entity) ((sslog_individual_t *) entity)

/** @brief Casts entity as calss. */
#define sslog_entity_as_class(entity) ((sslog_class_t *) entity)

/** @brief Casts entity as property. */
#define sslog_entity_as_property(entity) ((sslog_property_t *) entity)


/** @brief Casts any object (for example void *) as entity. */
#define sslog_object_as_entity(object) ((sslog_entity_t *) object)

/** @brief Casts property as entity. */
#define sslog_property_as_entity(property) ((sslog_entity_t *) property)

/** @brief Casts class as entity. */
#define sslog_class_as_entity(sslog_class) ((sslog_entity_t *) sslog_class)

/** @brief Casts individual as entity. */
#define sslog_individual_as_entity(individual) ((sslog_entity_t *) individual)


/** @brief Casts object to varialbe with entity type. */
#define sslog_object_to_entity(object, variable) sslog_entity_t *variable = (sslog_entity_t *) object;

/** @brief Casts property to varialbe with entity type. */
#define sslog_property_to_entity(property, variable) sslog_entity_t *variable = (sslog_entity_t *) property;

/** @brief Casts class to varialbe with entity type. */
#define sslog_class_to_entity(sslog_class, variable) sslog_entity_t *variable = (sslog_entity_t *) sslog_class;

/** @brief Casts individual to varialbe with entity type. */
#define sslog_individual_to_entity(individual, variable) sslog_entity_t *variable = (sslog_entity_t *) individual;
/** @} */ // end of Entities-typecasts
/*****************************************************************************/


/******************************* Definitions *********************************/
/**************************** External functions *****************************/
/// @cond DOXY_EXTERNAL_API

/******************** Constructors ('_new_' functions) ***********************/
/**
 * @defgroup Constructors-Destructores Constructors/destructors ('new'/'free' functions)
 * @{
 */
/**
 *
 * @brief Creates a class.
 * Try to find information about class in the local store by given URI and returns
 * founded class. If the is not informations then a new triple created and based on it
 * created a new class.
 *
 * Each class are represented in the local store with triple:
 * URI - http://www.w3.org/1999/02/22-rdf-syntax-ns#type - http://www.w3.org/2000/01/rdf-schema#Class
 *
 * Function sets information about errors (#errors.h).
 *
 * @param uri. URI of the class.
 * @return class on success or NULL otherwise (error or no information in the local store).
 */
SSLOG_EXTERN sslog_class_t *sslog_new_class(const char *uri);


/**
 * @brief Creates a property.
 * Try to find information about property in the local store by given URI and returns
 * founded property. If the is not informations then a new triple created and based on it
 * created a new property.
 *
 * Each class are represented in the local store with triple:
 * URI - http://www.w3.org/1999/02/22-rdf-syntax-ns#type - http://www.w3.org/1999/02/22-rdf-syntax-ns#Property
 *
 * Function sets information about errors (#errors.h).
 *
 * @param uri. URI of the property.
 * @return class on success or NULL otherwise (error or no information in the local store).
 */
SSLOG_EXTERN sslog_property_t *sslog_new_property(const char *uri, sslog_property_type type);


/**
 * @brief Creates an individual.
 * Try to find information about individual in the local store by given URI and class.
 * If the indormation is founded then an individual will be returned.
 * If the is not informations then a new triple created and based on it
 * created a new individual.
 *
 * Each class are represented in the local store with triple:
 * URI - http://www.w3.org/1999/02/22-rdf-syntax-ns#type - Class-URI
 *
 * Function sets information about errors (#errors.h).
 *
 * @param parent_class. Parent class to create an individual.
 * @param uri. URI of the individual.
 * @return class on success or NULL otherwise (error or no information in the local store).
 */
SSLOG_EXTERN sslog_individual_t *sslog_new_individual(const sslog_class_t *parent_class, const char *uri);
/** @} */ // end of Entities-typecasts

/***************************** Setters/Getters *******************************/
/**
 * @brief Gets a type of entity.
 * @param entity. Entity to get a type.
 * @return type of the entity on success or SSLOG_ACTION_UNKNOWN if the entity is NULL.
 */
SSLOG_EXTERN sslog_entity_type sslog_entity_get_type(sslog_entity_t *entity);


/**
 * @brief Gets URI of the entity: class, property or individual.
 * @param entity. Entity to get URI.
 * @return URI of the entity on success or NULL otherwise.
 */
SSLOG_EXTERN const char* sslog_entity_get_uri(void *entity);


/**
 * @brief Cheks is a property has an object type.
 * @param property. Property to check type.
 * @return true if the property is object or false otherwise (property has a data type or property is NULL).
 */
SSLOG_EXTERN bool sslog_property_is_object(sslog_property_t *property);


//TODO: Semantic checking: property domain: Needs to decide is it needed or not.
/// @cond NO_DOCS
/**
 * @brief Sets a domain of property (now domain is not used now).
 * @param[in] property. Property to set a domain.
 * @param[in] uri.URI of the domain.
 */
/// @endcond NO_DOCS
//SSLOG_EXTERN void sslog_property_set_domain(sslog_property_t *property, const char *uri);


//TODO: Semantic checking: cardinality: Needs to decide is it needed or not.
/// @cond NO_DOCS
/**
 * @brief Sets cardinality of the property (now cardinality is not used now).
 * Minimum cardinality means how many values of property must be set for individual.
 * It is impossible to remove properties if count of properties <= min cardinality.
 *
 * Maximal cardinality means how many maximum values of property can be set for individual.
 * It is impossible to add properties if count of properties >= max cardinality.
 * @param[in] property. Property to set cardinality.
 * @param[in] min. Minimal cardinality (if value < 0 then min cardinality is not used).
 * @param[in] max. Maximal cardinality (if value < 0 then max cardinality is not used).
 */
/// @endcond NO_DOCS
//SSLOG_EXTERN void sslog_property_set_cardinality(sslog_property_t *property, int min, int max);

/// @endcond
//  endcond of DOXY_EXTERNAL_API


#ifdef	__cplusplus
}
#endif


#endif	/* _SSLOG_ENTITY_H */


