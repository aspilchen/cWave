/** @file dynarrExperiment.h
 *  @brief Experimenting with macros to create a generic dynamic array.
 * 
 *  This is an experiment to create easy to use generic dynamic arrays.
 *  Currently must typedef any structs or pointers being used in this. I need
 *  to do some adjustments so it doesnt get mad when declaring a struct or a pointer 
 *  directly.
 * 
 *  @version 1.0
 *  @date 2022/04/18
 *  @author Adam Spilchen
 */

#ifndef DYNARR_EXPERIMENTAL_H
#define DYNARR_EXPERIMENTAL_H

struct DynarrMeta {
	void **data;
	size_t size;
	size_t itemSize;
	size_t capacity;
};

/******************************************************************************
**              Interface
******************************************************************************/

// Interface is done with macros, allowing the compiler to dynamically create new data types as needed.

#define Dynarr(TYPE) dynarr__ ## TYPE ## __
// #define DynarrPTRS(TYPE) dynarr__ptrs ## TYPE ## __

/** @brief Declares a dynamic array type.
 *  @warning User must use include gaurds to prevent redifining types. Please use the format
 *  #ifndef DYNARR__<type>__ 
 *  #define DYNARR__<type>__ 
 *  dynarr_declare(<type>)
 *  #endif
 */
#define dynarr_declare(TYPE) struct Dynarr(TYPE) {struct DynarrMeta meta; TYPE *data;} typedef Dynarr(TYPE)

/** @brief Initialize dynamic array.
 *  @arg PTR Pointer to dynamic array being initialized.
 */
#define dynarr_init(PTR) dynarr__init__(&arrmeta(PTR), &arrdata(PTR), arrtypesize(PTR))

#define dynarr_destroy(PTR) dynarr__destroy__(&arrmeta(PTR))

#define dynarr_put(PTR, INDEX, VALUE) (arrdata(PTR))[INDEX] = VALUE

#define dynarr_get(PTR, INDEX) arrdata(PTR)[INDEX]

#define dynarr_pushBack(PTR, VALUE) dynarr__push__back__(PTR, VALUE)

#define dynarr_setSize(PTR, SIZE) dynarr_setSize(&arrmeta(PTR), SIZE)



/******************************************************************************
**              Helpers
******************************************************************************/

#define dynarr__push__back__(PTR, VALUE) {dynar__expandIfFull__(&arrmeta(PTR)); dynarr_put(PTR, arrsize(PTR), VALUE); arrsize(PTR)++;}

// Make these safe in the future by preventing users from changing data using them.

#define arrdata(PTR)     (PTR)->data
#define arrmeta(PTR)     (PTR)->meta
#define arrmdata(PTR)    arrmeta(PTR).data
#define arrsize(PTR)     arrmeta(PTR).size
#define arrtypesize(PTR) sizeof(*arrdata(PTR))
#define arrcapacity(PTR) arrmeta(PTR).capacity


void dynarr__init__(struct DynarrMeta *meta, void *data, size_t itemSize);
void dynarr__destroy__(struct DynarrMeta *meta);
void dynar__expandIfFull__(struct DynarrMeta *meta); 
void dynarr__pushBack__(struct DynarrMeta *meta);
void dynar__setSize__(struct DynarrMeta *meta, size_t size); 

#endif