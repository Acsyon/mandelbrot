/* net/package.h
 *
 * Header for packages to be sent between client and server
 *
 */

#ifndef MANDELBROT_NET_PACKAGE_H_INCLUDED
#define MANDELBROT_NET_PACKAGE_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>

#include <net/connection.h>

/**
 * vtable for different types of packages
 */
typedef struct {
    const char *name;
    void *(*init)(const void *params);
    void (*update)(void *data, uint64_t size, const void *params);
    uint64_t (*size)(const void *data);
    uint64_t (*hash)(const void *data);
    bool supports_move;
} PackageType;

/**
 * Opaque package type
 */
typedef struct Package Package;

/**
 * Creates a new Package object of type `type` that can safely be freed. Memory
 * inside the Package has not yet been initialized (this must be done with
 * 'Package_init').
 *
 * @param[in] type PackageType of newly created Package
 *
 * @return new Package object of type `type`
 */
Package *
Package_create(const PackageType *type);

/**
 * Destroys Package object `pkg` and frees memory.
 *
 * @param[in] pkg Package object to be destroyed
 */
void
Package_free(Package *pkg);

/**
 * Returns the type of the Package.
 *
 * @param[in] pkg Package to return type of
 *
 * @return type of the Package
 */
const PackageType *
Package_get_type(const Package *pkg);

/**
 * Returns the size of the Package in bytes.
 *
 * @param[in] pkg Package to return size of
 *
 * @return size of the Package in bytes
 */
uint64_t
Package_get_size(const Package *pkg);

/**
 * Returns a pointer to the data in the Package.
 *
 * @param[in] pkg Package to return data of
 *
 * @return pointer to the data in the Package
 */
const void *
Package_get_data(const Package *pkg);

/**
 * Returns the hash of the data inside the Package.
 *
 * @param[in] pkg Package to return hash of
 *
 * @return hash of the data inside the Package
 */
uint64_t
Package_get_hash(const Package *pkg);

/**
 * Allocates and initializes memory inside Package according to `params`. To be
 * used inside constructors for the different Package types where checks for
 * correct types should also happen.
 *
 * @param[in] pkg Package to initialize
 * @param[in] params pointer to parameter object to initialize Package with
 */
void
Package_init(Package *pkg, const void *params);

/**
 * Takes ownership of `data`, i.e., data inside the package will become `data`
 * and it will be freed by 'Package_free'. To be used inside constructors for
 * the different Package types where checks for correct types should also
 * happen.
 *
 * @param[in] pkg Package to initialize
 * @param[in] data pointer to data object to initialize Package with
 *
 * @note May not be supported by all types.
 */
void
Package_move_init(Package *pkg, void *data);

/**
 * Updates memory (without reallocating) inside Package according to `params`.
 * This serves to minimize the number of memory allocations. To be used inside
 * update methods for the different Package types where checks for correct types
 * should also happen.
 *
 * @param[in] pkg Package to update
 * @param[in] params pointer to parameter object to update memory inside Package
 * with
 *
 * @note May not be supported by all types.
 */
void
Package_update(Package *pkg, const void *params);

/**
 * Returns whether or not the hash inside the Package corresponds to the data.
 * This serves to check if a Package has been transmitted correctly.
 *
 * @param[in] pkg Package to check integrity of
 *
 * @return Does the hash inside the Package correspond to the data?
 */
bool
Package_verify(const Package *pkg);

/**
 * Sends Package via Connection and returns whether transmission was succesful.
 *
 * @param[in] pkg Package to send
 * @param[in] conn Connection to send Package via
 *
 * @return was transmission succesful?
 */
bool
Package_send(const Package *pkg, const Connection *conn);

/**
 * Receives Package via Connection and returns whether transmission was
 * succesful. The PackageType of `pkg` has to correspond to the received
 * package to be succesful.
 *
 * @param[in, out] pkg Package to write data to
 * @param[in] conn Connection to send Package via
 *
 * @return was transmission succesful?
 */
bool
Package_receive(Package *pkg, const Connection *conn);

#endif /* MANDELBROT_NET_PACKAGE_H_INCLUDED */
