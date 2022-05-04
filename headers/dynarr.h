/** @file dynarr.h
 *  @brief Dynamic array.
 *  @warning Use before initialization will cause undefined behaviour.
 *  @warning It is the users responsibility to know what datatype is stored in the array.
 *  @version 1.0
 *  @date 2022/04/14
 *  @author Adam Spilchen
 */

#ifndef DYNARR_H
#define DYNARR_H

#include <stddef.h>

struct Dynarr {
	void *data;
	size_t size;
	size_t itemSize;
	size_t capacity;
} typedef Dynarr;


/// @brief Initialize dynamic array.
/// @param dynarr Pointer to Dynarr being initialized.
/// @param itemSize Size of items to be stored.
/// @return Returns 0 on success. Returns -1 on failure.
int dynarr_init(Dynarr *const dynarr, size_t itemSize)
	__attribute__((nonnull));


/// @brief Destroy dynamic array.
/// @param dynarr Pointer to Dynarr being destroyed.
void dynarr_destroy(Dynarr *const dynarr)
	__attribute__((nonnull));


/// @brief Change capacity of dynamic array.
/// @param dynarr Pointer to Dynarr being changed.
/// @param newCapacity Desired capacity.
/// @return Returns 0 on success. Returns -1 on failure.
/// @warning Data will be lost if nweCapacity is smaller than the number of items in the array.
int dynarr_setCapacity(Dynarr *const dynarr, size_t newCapacity)
	__attribute__((nonnull));


/// @brief Put item from source into back of array, dynamically resizes if needed.
/// @param source Pointer to destination Dynarr.
/// @param destination Address to value being stored.
/// @return Returns 0 on success. Returns -1 on failure.
int dynarr_pushback(Dynarr *const dynarr, void *source)
	__attribute((nonnull));


/// @brief Put item from source into dynar at index.
/// @param source Pointer to destination Dynarr.
/// @param destination Address to value being stored.
/// @param index Desired array index.
/// @return Returns 0 on success. Returns -1 on failure.
int dynarr_put(Dynarr *const dynarr, void *source, unsigned int index)
	__attribute((nonnull));


/// @brief Get item from dynarr at index and store in destination.
/// @param source Pointer to source Dynarr.
/// @param destination Address to store value.
/// @param index Index of item in array.
/// @return Returns 0 on success. Returns -1 on failure.
int dynarr_get(const Dynarr *const dynarr, void *destination, unsigned int index)
	__attribute((nonnull));


/// @brief Get number of items stored in array.
/// @param dynarr Pointer to Dynarr being checked.
/// @return Number of items being stored in array.
size_t dynarr_getSize(const Dynarr *const dynarr)
	__attribute((nonnull));




#endif
