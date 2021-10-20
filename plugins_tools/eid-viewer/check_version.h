#ifndef CHECK_VERSION_H
#define CHECK_VERSION_H
#include <eid-viewer/macros.h>
#include <stdbool.h>

struct version_triplet {
	int major;
	int minor;
	int build;
};

struct upgrade_info {
	bool have_upgrade;
	char *upgrade_url;
	char *relnotes_url;
	struct version_triplet new_version;
};

DllExport struct upgrade_info *eid_vwr_upgrade_info(char *xml, size_t xmllen, char *osname, char *osversion, int major, int minor, int build);
#endif
