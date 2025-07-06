/* net/package.h
 *
 * Header for packages to be sent between client and server
 *
 */

#ifndef MANDELBROT_NET_PACKAGE_H_INCLUDED
#define MANDELBROT_NET_PACKAGE_H_INCLUDED

#include <stdint.h>

#include <cutil/std/stdbool.h>

#include <net/connection.h>

/**
 * vtable for different types of packages
 */
typedef struct {
    const char *const name;
    void *(*const init)(const void *params);
    void (*const free)(void *pkgdata);
    void (*const set)(void *pkgdata, const void *data);
    void (*const get)(const void *pkgdata, void *data);
    uint64_t (*const hash)(const void *pkgdata);
    bool (*const send)(const void *pkgdata, const Connection *conn);
    bool (*const recv)(void *pkgdata, const Connection *conn);
} PackageType;

/**
 * Opaque package type
 */
typedef struct _package Package;

/**
 * Creates a new Package object of type `type` that can safely be freed.
 * Allocates and initializes memory inside Package according to `params`. To be
 * used inside constructors for the different Package types where checks for
 * correct types should also happen.
 *
 * @param[in] type PackageType of newly created Package
 * @param[in] params pointer to parameter object to initialize Package with
 *
 * @return new Package object of type `type` initialized with `params`
 */
Package *
Package_create(const PackageType *type, const void *params);

/**
 * Destroys Package object `pkg` and frees memory.
 *
 * @param[in] pkg Package object to be destroyed
 */
void
Package_free(Package *pkg);

/**
 * Updates data inside Package according to `data`. To be used inside setter
 * functions for the different Package types where checks for correct types
 * should also happen.
 *
 * @param[in] pkg Package to set data of
 * @param[in] data pointer to data object to update data inside Package with
 */
void
Package_set_data(Package *pkg, const void *data);

/**
 * Writes data inside Package to `data`. To be used inside getter functions for
 * the different Package types where checks for correct types should also
 * happen.
 *
 * @param[in] pkg Package to get data of
 * @param[in] data pointer to data object to write data from inside the Package
 * to
 */
void
Package_get_data(const Package *pkg, void *data);

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
 * Returns the hash of the data inside the Package.
 *
 * @param[in] pkg Package to return hash of
 *
 * @return hash of the data inside the Package
 */
uint64_t
Package_get_hash(const Package *pkg);

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
