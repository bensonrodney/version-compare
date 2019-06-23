#ifndef _VERSION_COMPARE
#define _VERSION_COMPARE

// Comparison results
#define COMP_ERROR -1
#define COMP_TRUE 1
#define COMP_FALSE 0

typedef struct {
	uint8_t num_sections;
	int *sections;
} version_t;

/* The following functions are generally only used
 * internally. See the v_XX() functions below for
 * the more general string-only based comparison
 * functions.
 */
version_t *new_version();
int free_version(version_t *v);
version_t *str_to_ver(const char *verstr);
int min_sections(const version_t *v1, const version_t *v2);
int vt_eq(const version_t *v1, const version_t *v2);
int vt_ne(const version_t *v1, const version_t *v2);
int vt_gt(const version_t *v1, const version_t *v2);
int vt_ge(const version_t *v1, const version_t *v2);
int vt_lt(const version_t *v1, const version_t *v2);
int vt_le(const version_t *v1, const version_t *v2);

/* The following functions are the general purpose
 * comparison functions that only require strings as
 * input parameters.
 */
int v_eq(const char *version1, const char *version2);
int v_ne(const char *version1, const char *version2);
int v_gt(const char *version1, const char *version2);
int v_ge(const char *version1, const char *version2);
int v_lt(const char *version1, const char *version2);
int v_le(const char *version1, const char *version2);

#endif /* _VERSION_COMPARE */

