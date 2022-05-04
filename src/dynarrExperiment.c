#include <string.h>
#include <stdlib.h>

#include "../headers/dynarrExperiment.h"

/******************************************************************************
**              FUNCTION DEFINITIONS                                           
******************************************************************************/
void dynarr__init__(struct DynarrMeta *meta, void *data, size_t itemSize)
{
	meta->data = data;
	*meta->data = malloc(itemSize);
	meta->size = 0;
	meta->itemSize = itemSize;
	meta->capacity = 1;
}

void dynarr__destroy__(struct DynarrMeta *meta)
{
	free(*(meta->data));
	*(meta->data) = NULL;
	meta->size = 0;
	meta->capacity = 0;
}

void dynarr__pushBack__(struct DynarrMeta *meta)
{
	meta->size += 1;
}

void dynar__expandIfFull__(struct DynarrMeta *meta)
{
	if(meta->size < meta->capacity) {
		return;
	}

	size_t newCapacity = meta->capacity * 2;
	void* data = malloc(newCapacity * meta->itemSize);
	memcpy(data, *meta->data, meta->size * meta->itemSize);
	free(*meta->data);
	*meta->data = data;
	meta->capacity = newCapacity;
}

void dynar__setSize__(struct DynarrMeta *meta, size_t size)
{
	void* data = malloc(size * meta->itemSize);
	memcpy(data, *meta->data, meta->size * meta->itemSize);
	free(*meta->data);
	*meta->data = data;
	meta->capacity = size;
	meta->size = size;
}