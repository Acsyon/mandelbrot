/* util/json.h
 *
 * Header for handling JSON files
 *
 */

#ifndef MANDELBROT_UTIL_JSON_H_INCLUDED
#define MANDELBROT_UTIL_JSON_H_INCLUDED

#include <stdint.h>

#include <cjson/cJSON.h>

#include <cutil/std/stdbool.h>
#include <cutil/std/stdio.h>

/**
 * JSON object type (wrapper around cJSON)
 */
typedef cJSON Json;

/**
 * Creates newly malloc'd Json object.
 *
 * @return newly malloc'd Json object
 */
Json *
Json_create(void);

/**
 * Creates newly malloc'd Json object whose contents are read from string `str`.
 *
 * @param[in] str string to read contents of Json object from
 *
 * @return newly malloc'd Json object with contents read from string
 */
Json *
Json_from_string(const char *str);

/**
 * Destroys Json object `json` and frees memory.
 *
 * @param[in] newly Json object to be destroyed
 */
void
Json_free(Json *json);

/**
 * Writes contents of `json` to newly malloc'd string.
 *
 * @param[in] json Json object to write
 *
 * @return newly malloc'd string with contents of `json`
 */
char *
Json_to_string(const Json *json);

/**
 * Compares the Json objects `lhs` and `rhs` by either taking case sensitivity
 * into account or not (depending on `case_sensitive`) and returns if the
 * objects are equal. Invalid and NULL input always compares to false.
 *
 * @param[in] lhs left-hand side of comparison
 * @param[in] rhs right-hand side of comparison
 * @param[in] case_sensitive Should comparison be performed case-sensitively?
 *
 * @return Does `lhs` equal `rhs`?
 */
bool
Json_compare(const Json *lhs, const Json *rhs, bool case_sensitive);

/**
 * Reads element `elem` from `json` interpreted as an int and writes it to
 * memory pointed to by `ptr`. Returns error code (EXIT_SUCESS or EXIT_FAILURE).
 *
 * @param[in] json Json object to read from
 * @param[in] elem name of element to read
 * @param[out] ptr pointer to memory to write to
 *
 * @return error code (EXIT_SUCESS or EXIT_FAILURE).
 */
int
Json_elem_to_int(const Json *json, const char *elem, int *ptr);

/**
 * Reads element `elem` from `json` interpreted as a uint16_t and writes it to
 * memory pointed to by `ptr`. Returns error code (EXIT_SUCESS or EXIT_FAILURE).
 *
 * @param[in] json Json object to read from
 * @param[in] elem name of element to read
 * @param[out] ptr pointer to memory to write to
 *
 * @return error code (EXIT_SUCESS or EXIT_FAILURE).
 */
int
Json_elem_to_uint16_t(const Json *json, const char *elem, uint16_t *ptr);

/**
 * Reads element `elem` from `json` interpreted as a size_t and writes it to
 * memory pointed to by `ptr`. Returns error code (EXIT_SUCESS or EXIT_FAILURE).
 *
 * @param[in] json Json object to read from
 * @param[in] elem name of element to read
 * @param[out] ptr pointer to memory to write to
 *
 * @return error code (EXIT_SUCESS or EXIT_FAILURE).
 */
int
Json_elem_to_size_t(const Json *json, const char *elem, size_t *ptr);

/**
 * Reads element `elem` from `json` interpreted as a double and writes it to
 * memory pointed to by `ptr`. Returns error code (EXIT_SUCESS or EXIT_FAILURE).
 *
 * @param[in] json Json object to read from
 * @param[in] elem name of element to read
 * @param[out] ptr pointer to memory to write to
 *
 * @return error code (EXIT_SUCESS or EXIT_FAILURE).
 */
int
Json_elem_to_double(const Json *json, const char *elem, double *ptr);

/**
 * Reads element `elem` from `json` interpreted as a string and writes it to
 * memory pointed to by `ptr`. Returns error code (EXIT_SUCESS or EXIT_FAILURE).
 *
 * @param[in] json Json object to read from
 * @param[in] elem name of element to read
 * @param[out] ptr pointer to memory to write to
 *
 * @return error code (EXIT_SUCESS or EXIT_FAILURE).
 */
int
Json_elem_to_str(const Json *json, const char *elem, char **ptr);

/**
 * Adds int `var` as element with name `elem` to Json object pointed to by
 * `json`. Returns error code (EXIT_SUCESS or EXIT_FAILURE).
 *
 * @param[in] json Json object to add element to
 * @param[in] elem name of element to add
 * @param[in] var value of element to add
 *
 * @return error code (EXIT_SUCESS or EXIT_FAILURE).
 */
int
Json_add_int(Json *json, const char *elem, int var);

/**
 * Adds uint16_t `var` as element with name `elem` to Json object pointed to by
 * `json`. Returns error code (EXIT_SUCESS or EXIT_FAILURE).
 *
 * @param[in] json Json object to add element to
 * @param[in] elem name of element to add
 * @param[in] var value of element to add
 *
 * @return error code (EXIT_SUCESS or EXIT_FAILURE).
 */
int
Json_add_uint16_t(Json *json, const char *elem, uint16_t var);

/**
 * Adds size_t `var` as element with name `elem` to Json object pointed to by
 * `json`. Returns error code (EXIT_SUCESS or EXIT_FAILURE).
 *
 * @param[in] json Json object to add element to
 * @param[in] elem name of element to add
 * @param[in] var value of element to add
 *
 * @return error code (EXIT_SUCESS or EXIT_FAILURE).
 */
int
Json_add_size_t(Json *json, const char *elem, size_t var);

/**
 * Adds double `var` as element with name `elem` to Json object pointed to by
 * `json`. Returns error code (EXIT_SUCESS or EXIT_FAILURE).
 *
 * @param[in] json Json object to add element to
 * @param[in] elem name of element to add
 * @param[in] var value of element to add
 *
 * @return error code (EXIT_SUCESS or EXIT_FAILURE).
 */
int
Json_add_double(Json *json, const char *elem, double var);

/**
 * Adds string `var` as element with name `elem` to Json object pointed to by
 * `json`. Returns error code (EXIT_SUCESS or EXIT_FAILURE).
 *
 * @param[in] json Json object to add element to
 * @param[in] elem name of element to add
 * @param[in] var value of element to add
 *
 * @return error code (EXIT_SUCESS or EXIT_FAILURE).
 */
int
Json_add_str(Json *json, const char *elem, const char *var);

/**
 * Typedef for JSON reading callback function
 *
 * @param[out] obj object to write values into
 * @param[in] json Json object to read from
 */
typedef void
JsonUtilReadCallback(void *obj, const Json *json);

/**
 * Fills object `obj` with the values in the JSON config file whose contents are
 * stored in string `str`. Reading is performed by the callback function
 * `callback`.
 *
 * @param[out] obj object to write values into
 * @param[in] str contents of config file to read values from
 * @param[in] callback callback function for reading
 */
void
JsonUtil_fill_from_string(
  void *obj, const char *str, JsonUtilReadCallback *callback
);

/**
 * Fills object `obj` with the values in the JSON config file opened in file
 * stream `in`. Reading is performed by the callback function `callback`.
 *
 * @param[out] obj object to write values into
 * @param[in] in stream of config file to read values from
 * @param[in] callback callback function for reading
 */
void
JsonUtil_fread(void *obj, FILE *in, JsonUtilReadCallback *callback);

/**
 * Fills object `obj` with the values in the JSON config file in `fname`.
 * Reading is performed by the callback function `callback`.
 *
 * @param[out] obj object to write values into
 * @param[in] fname file name of config file to read values from
 * @param[in] callback callback function for reading
 */
void
JsonUtil_read(void *obj, const char *fname, JsonUtilReadCallback *callback);

/**
 * Typedef for JSON writing callback function
 *
 * @param[in] obj object to read values from
 *
 * @return Json object to write data to
 */
typedef Json *
JsonUtilWriteCallback(const void *obj);

/**
 * Creates newly malloc'd string containing `obj` as a JSON object. Writing is
 * performed by the callback function `callback`.
 *
 * @param[in] obj object to convert to string
 * @param[in] callback callback function for writing
 *
 * @return newly malloc'd string containing `obj` as a JSON object
 */
char *
JsonUtil_to_string(const void *obj, JsonUtilWriteCallback *callback);

/**
 * Writes `obj` as a JSON object to output stream `out`. Writing is performed by
 * the callback function `callback`.
 *
 * @param[in] obj object to write
 * @param[out] out output stream to write into
 * @param[in] callback callback function for writing
 */
void
JsonUtil_fwrite(const void *obj, FILE *out, JsonUtilWriteCallback *callback);

/**
 * Writes `obj` as a JSON object to file `fname`. Writing is performed by the
 * callback function `callback`.
 *
 * @param[in] obj object to write
 * @param[in] out name of file to write into
 * @param[in] callback callback function for writing
 */
void
JsonUtil_write(
  const void *obj, const char *fname, JsonUtilWriteCallback *callback
);

#endif /* MANDELBROT_UTIL_JSON_H_INCLUDED */
