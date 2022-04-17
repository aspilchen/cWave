#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "dynarr.h"

/******************************************************************************
**              INTERNAL MACRO DEFINITIONS                                     
******************************************************************************/
#define RETURN_BAD_IF_NULL(ptr) {if(!(ptr)) {return -1;}}
#define RETURN_BAD_IF_FALSE(boolean) {if(!(boolean)) {return -1;}}

/******************************************************************************
**              INTERNAL FUNCTION PROTOTYPES                                   
******************************************************************************/
static int dynarr_initImpl(Dynarr *const dynarr, size_t itemSize);
static void dynarr_destroyImpl(Dynarr *const dynarr);
static int dynarr_setCapacityImpl(Dynarr *const dynarr, size_t newCapacity);
static int dynarr_putImpl(Dynarr *const dynarr, void *source, unsigned int index);
static int dynarr_pushbackImpl(Dynarr *const dynarr, void *source);
static size_t dynarr_sizeImpl(const Dynarr *const dynarr);
static int dynarr_getImpl(const Dynarr *const dynarr, void *destination, unsigned int index);

/******************************************************************************
**              INTERNAL VARIABLES                                             
******************************************************************************/

/******************************************************************************
**              FUNCTION DEFINITIONS                                           
******************************************************************************/
int dynarr_init(Dynarr *const dynarr, size_t itemSize)
{
	assert(itemSize > 0);
	RETURN_BAD_IF_FALSE(itemSize > 0);
	return dynarr_initImpl(dynarr, itemSize);
}

void dynarr_destroy(Dynarr *const dynarr)
{
	void *data = dynarr->data;
	assert(data);
	if(data) {
		return;
	}

	dynarr_destroyImpl(dynarr);
}
int dynarr_setCapacity(Dynarr *const dynarr, size_t newCapacity)
{
	return dynarr_setCapacityImpl(dynarr, newCapacity);
}

int dynarr_put(Dynarr *const dynarr, void *source, unsigned int index)
{
	void *data = dynarr->data;
	assert(data);
	RETURN_BAD_IF_NULL(data);
	RETURN_BAD_IF_FALSE(index < dynarr->size);
	return dynarr_putImpl(dynarr, source, index);
}

int dynarr_pushback(Dynarr *const dynarr, void *source)
{
	void *data = dynarr->data;
	assert(data);
	RETURN_BAD_IF_NULL(data);
	return dynarr_pushbackImpl(dynarr, source);
}

size_t dynarr_size(const Dynarr *const dynarr)
{
	return dynarr_sizeImpl(dynarr);
}

int dynarr_get(const Dynarr *const dynarr, void *destination, unsigned int index)
{
	void *data = dynarr->data;
	assert(index < dynarr->size);
	assert(data);
	RETURN_BAD_IF_NULL(data);
	RETURN_BAD_IF_FALSE(index < dynarr->size);
	return dynarr_getImpl(dynarr, destination, index);
}



/******************************************************************************
**              INTERNAL FUNCTION DEFINITIONS                                  
******************************************************************************/
static int dynarr_initImpl(Dynarr *const dynarr, size_t itemSize)
{
	const size_t DEFAULT_CAPACITY = 8;
	void *arr = malloc(DEFAULT_CAPACITY * itemSize);
	RETURN_BAD_IF_NULL(arr);
	dynarr->data = arr;
	dynarr->capacity = DEFAULT_CAPACITY;
	dynarr->itemSize = itemSize;
	dynarr->size = 0;
	return 0;
}

static void dynarr_destroyImpl(Dynarr *const dynarr)
{
	free(dynarr->data);
	dynarr->data = 0;
	dynarr->size = 0;
	dynarr->itemSize = 0;
	dynarr->capacity = 0;
}

static int dynarr_setCapacityImpl(Dynarr *const dynarr, size_t newCapacity)
{
	size_t newCapInBytes = dynarr->itemSize * newCapacity;
	char *data = realloc(dynarr->data, newCapInBytes);
	RETURN_BAD_IF_NULL(data);
	dynarr->data = data;
	dynarr->capacity = newCapacity;

	if(newCapacity < dynarr->size) {
		dynarr->size = newCapacity;
	}

	return 0;
}

static int dynarr_putImpl(Dynarr *const dynarr, void *source, unsigned int index)
{
	char *data = dynarr->data;
	data += index * dynarr->itemSize;
	memcpy(data, source, dynarr->itemSize);
	return 0;
}

static int dynarr_pushbackImpl(Dynarr *const dynarr, void *source)
{
	if(dynarr->size >= dynarr->capacity) {
		size_t newCap = dynarr->capacity * 2;
		dynarr_setCapacityImpl(dynarr, newCap);
	}

	size_t currSize = dynarr->size;
	dynarr->size += 1;
	return dynarr_putImpl(dynarr, source, currSize);
}

static size_t dynarr_sizeImpl(const Dynarr *const dynarr)
{
	return dynarr->size;
}

static int dynarr_getImpl(const Dynarr *const dynarr, void *destination, unsigned int index)
{
	char *data = dynarr->data;
	data += index * dynarr->itemSize;
	memcpy(destination, data, dynarr->itemSize);
	return 0;
}