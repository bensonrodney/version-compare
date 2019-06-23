/* version_compare.c - Jason Milen 2019
 * Demo for use as a version string equality, inequality,
 * greater than, less than, greater or equal, less or equal,
 * or combos of those. See the assertion tests in main().
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "version_compare.h"


version_t *new_version() {
	version_t *v = (version_t *)malloc(sizeof(version_t));
	if (!v) return NULL;
	v->num_sections = 0;
	v->sections = NULL;
	return v;
}

int free_version(version_t *v) {
	if (!v) return 1;
	if (v->sections) free(v->sections);
	free(v);
	return 1;
}

version_t *str_to_ver(const char *verstr) {
	version_t *v = new_version();
	char *dup = strdup(verstr);
	char *remain = dup;
	char *token = NULL;
	int subv = 0;
	int sr = 0;

	while ((token = strtok_r(remain, ".", &remain))) {
		sr = sscanf(token, "%d", &subv);
		if (sr != 1) {
			free_version(v);
			v = NULL;
			goto cleanup;
		}
		if (v->num_sections > 0) {
			v->sections = (int *) realloc(v->sections, (v->num_sections + 1) * sizeof(int));
		} else {
			v->sections = (int *) calloc(1, sizeof(int));
		}

		v->sections[v->num_sections++] = subv;
	}
cleanup:
	if (dup) free (dup);
	return v; 
}

int min_sections(const version_t *v1, const version_t *v2) {
	if (!v1 || !v2) return 0;
	return v1->num_sections < v2->num_sections ? v1->num_sections : v2->num_sections; 
}


/* version_t BASED COMPARISON FUNCTIONS */
int vt_eq(const version_t *v1, const version_t *v2) {
	int rv = COMP_FALSE;
	int sections = min_sections(v1, v2);
	if (v1->num_sections != v2->num_sections) return COMP_FALSE;
	for (int i=0; i < sections; i++) {
		if (v1->sections[i] != v2->sections[i]) return COMP_FALSE;
	}
	return COMP_TRUE;
}

int vt_ne(const version_t *v1, const version_t *v2) {
	int rv = vt_eq(v1, v2);
	if (rv == COMP_ERROR) return rv;
	return rv == COMP_TRUE ? COMP_FALSE : COMP_TRUE;
}

int vt_gt(const version_t *v1, const version_t *v2) {
	int sections = min_sections(v1, v2);
	for (int i=0; i < sections; i++) {
		if (v1->sections[i] > v2->sections[i]) return COMP_TRUE;
		if (v1->sections[i] < v2->sections[i]) return COMP_FALSE;
	}
	if (v1->num_sections > v2->num_sections) return COMP_TRUE;
	return COMP_FALSE;
}

int vt_ge(const version_t *v1, const version_t *v2) {
	int rv = vt_eq(v1, v2);
	if (rv == COMP_ERROR) return COMP_ERROR;
	if (rv == COMP_TRUE) return COMP_TRUE;
	return vt_gt(v1, v2);
}

int vt_lt(const version_t *v1, const version_t *v2) {
	return vt_gt(v2, v1);
}

int vt_le(const version_t *v1, const version_t *v2) {
	int rv = vt_eq(v1, v2);
	if (rv == COMP_ERROR) return COMP_ERROR;
	if (rv == COMP_TRUE) return COMP_TRUE;
	return vt_lt(v1, v2);
}

/* STRING BASED COMPARISON FUNCTIONS */

// used at the start of a comparison function that takes
// two strings as the versions
#define CONVERT_VERSION_STRINGS \
	version_t *v1 = str_to_ver(version1); \
	version_t *v2 = str_to_ver(version2); \
	if (!v1 || !v2) { \
		free_version(v1); \
		free_version(v2); \
		return COMP_ERROR; \
	} \

// used at the end of a comparison funtion that takes two
// strings as the versions
#define COMP_FUNC_END \
cleanup: \
	free_version(v1); \
	free_version(v2); \
	return rv; \

int v_eq(const char *version1, const char *version2) {
	CONVERT_VERSION_STRINGS
	int rv = vt_eq(v1, v2);
COMP_FUNC_END
}

int v_ne(const char *version1, const char *version2) {
	CONVERT_VERSION_STRINGS
	int rv = vt_ne(v1, v2);
COMP_FUNC_END
}

int v_gt(const char *version1, const char *version2) {
	CONVERT_VERSION_STRINGS
	int rv = vt_gt(v1, v2);
COMP_FUNC_END
}

int v_ge(const char *version1, const char *version2) {
	CONVERT_VERSION_STRINGS
	int rv = vt_ge(v1, v2);
COMP_FUNC_END
}

int v_lt(const char *version1, const char *version2) {
	return v_gt(version2, version1);
}

int v_le(const char *version1, const char *version2) {
	CONVERT_VERSION_STRINGS
	int rv = vt_le(v1, v2);
COMP_FUNC_END
}

int main(char *argv[], int argc) {
	/* tests for the code above, should move to a separate
	 * test file later.
	 */
	assert(v_eq("2.13.4.2.4", "4.2.13") == COMP_FALSE);
	assert(v_eq("2.13.4.2.4", "2.2.13") == COMP_FALSE);
	assert(v_eq("2.13.4.2.4", "2.13.4") == COMP_FALSE);
	assert(v_eq("2.13.4", "2.13.4.2.4") == COMP_FALSE);
	assert(v_eq("2.13.4", "2.13.4") == COMP_TRUE);
	assert(v_eq("2.13.4", "2.13.2") == COMP_FALSE);

	assert(v_ne("2.13.4.2.4", "4.2.13") == COMP_TRUE);
	assert(v_ne("2.13.4.2.4", "2.2.13") == COMP_TRUE);
	assert(v_ne("2.13.4.2.4", "2.13.4") == COMP_TRUE);
	assert(v_ne("2.13.4", "2.13.4.2.4") == COMP_TRUE);
	assert(v_ne("2.13.4", "2.13.4") == COMP_FALSE);
	assert(v_ne("2.13.4", "2.13.2") == COMP_TRUE);

	assert(v_gt("2.13.4.2.4", "4.2.13") == COMP_FALSE);
	assert(v_gt("2.13.4.2.4", "2.2.13") == COMP_TRUE);
	assert(v_gt("2.13.4.2.4", "2.13.4") == COMP_TRUE);
	assert(v_gt("2.13.4.3.4", "2.13.4.2.28") == COMP_TRUE);
	assert(v_gt("2.13.4", "2.13.4.2.4") == COMP_FALSE);
	assert(v_gt("2.13.4", "2.13.4") == COMP_FALSE);
	assert(v_gt("2.13.4", "2.3.4") == COMP_TRUE);

	assert(v_ge("2.13.4.2.4", "4.2.13") == COMP_FALSE);
	assert(v_ge("2.13.4.2.4", "2.2.13") == COMP_TRUE);
	assert(v_ge("2.13.4.2.4", "2.13.4") == COMP_TRUE);
	assert(v_ge("2.13.4.3.4", "2.13.4.2.28") == COMP_TRUE);
	assert(v_ge("2.13.4", "2.13.4.2.4") == COMP_FALSE);
	assert(v_ge("2.13.4", "2.13.4") == COMP_TRUE);
	assert(v_ge("2.13.4", "2.3.4") == COMP_TRUE);

	assert(v_lt("2.13.4.2.4", "4.2.13") == COMP_TRUE);
	assert(v_lt("2.13.4.2.4", "2.2.13") == COMP_FALSE);
	assert(v_lt("2.13.4.2.4", "2.13.4") == COMP_FALSE);
	assert(v_lt("2.13.4.3.4", "2.13.4.2.28") == COMP_FALSE);
	assert(v_lt("2.13.4", "2.13.4.2.4") == COMP_TRUE);
	assert(v_lt("2.13.4", "2.13.4") == COMP_FALSE);
	assert(v_lt("2.3.4", "2.13.4") == COMP_TRUE);

	assert(v_le("2.13.4.2.4", "4.2.13") == COMP_TRUE);
	assert(v_le("2.13.4.2.4", "2.2.13") == COMP_FALSE);
	assert(v_le("2.13.4.2.4", "2.13.4") == COMP_FALSE);
	assert(v_le("2.13.4", "2.13.4.2.4") == COMP_TRUE);
	assert(v_le("2.13.4", "2.13.4") == COMP_TRUE);
	assert(v_le("2.3.4", "2.13.4") == COMP_TRUE);

	assert(v_eq("apple78pear.4.5", "0.4.5") == COMP_ERROR);
	assert(v_eq("3.4.5", "0.apple.5") == COMP_ERROR);

	return 0;
}
