#include "backend.h"

#include <check_version.h>

#include <libxml/encoding.h>
#include <libxml/xmlreader.h>
#include <stdbool.h>

#define check_xml(call) if((rc = call) < 0) { \
	be_log(EID_VWR_LOG_DETAIL, "Error while parsing version information (calling '%s'): %d", #call, rc); \
	goto out; \
}

enum parse_result {
	CONTINUE_PARSING = 0, // no interesting data found
	VERSION_FOUND = 1, // a version number was found
	CONSTRAINT_MATCHED = 2, // an upgrade constraint matched our version
	NOT_INTERESTING = 4, // this element does not apply to us, skip it
	RELNOTES_URL = 8, // a release notes URL was found
	UPDATE_URL = 16, // an update URL was found
	NO_UPGRADE = 32, // any previously found upgrades should be ignored
	LATEST_SUPPORTED = 64, // this is the latest supported version, cannot be overridden anymore
	PARSER_ERROR = 65536, // error parsing the file
};

struct rv {
	int parse_result;
	struct version_triplet version;
	char *rnurl;
	char *uurl;
};

struct context {
	const char *osname;
	const char *osversion;
	struct version_triplet version;
};

struct element_desc {
	char *name;
	struct rv(*parse)(xmlTextReaderPtr, struct context *);
};

static struct upgrade_info *upgrade_info;

static struct rv parse_name(xmlTextReaderPtr ctx, const char * const expect) {
	struct rv rc = {
		.parse_result = CONTINUE_PARSING,
	};
	if(xmlTextReaderHasAttributes(ctx) != 1) {
		rc.parse_result = PARSER_ERROR;
		return rc;
	}
	xmlChar *value = xmlTextReaderGetAttribute(ctx, (const xmlChar*) "name");
	if(strcmp(value, expect)) {
		rc.parse_result = NOT_INTERESTING;
	}
	xmlFree(value);
	return rc;
}

static enum parse_result parse_version(xmlTextReaderPtr ctx, struct version_triplet *version) {
	char *major = xmlTextReaderGetAttribute(ctx, (const xmlChar*) "major");
	char *minor = xmlTextReaderGetAttribute(ctx, (const xmlChar*) "minor");
	char *build = xmlTextReaderGetAttribute(ctx, (const xmlChar*) "build");
	if(major == NULL || minor == NULL || build == NULL) {
		return PARSER_ERROR;
	}
	version->major = strtol(major, NULL, 10);
	version->minor = strtol(minor, NULL, 10);
	version->build = strtol(build, NULL, 10);
	return VERSION_FOUND;
}

static struct rv parse_os(xmlTextReaderPtr ctx, struct context *whoami) {
	return parse_name(ctx, whoami->osname);
}

static struct rv parse_comp(xmlTextReaderPtr ctx, struct context *whoami) {
	return parse_name(ctx, "eid-viewer");
}

static int cmp_version(struct version_triplet *a, struct version_triplet *b) {
	if(a == b) return 0;
	if(a->major != b->major) return a->major - b->major;
	if(a->minor != b->minor) return a->minor - b->minor;
	if(a->build != b->build) return a->build - b->build;
	return 0;
}

static struct rv parse_latest_version(xmlTextReaderPtr ctx, struct context *whoami) {
	struct rv rv;
	if(xmlTextReaderHasAttributes(ctx) != 1) {
		rv.parse_result = PARSER_ERROR;
		return rv;
	}
	if(parse_version(ctx, &(rv.version)) == PARSER_ERROR) {
		rv.parse_result = PARSER_ERROR;
		return rv;
	}
	rv.parse_result = VERSION_FOUND;
	return rv;
}

static void parse_url(xmlTextReaderPtr ctx, char **urlloc, struct rv *rv) {
	if(xmlTextReaderHasAttributes(ctx) != 1) {
		rv->parse_result = PARSER_ERROR;
		return;
	}
	if(*urlloc) {
		xmlFree(*urlloc);
	}
	*urlloc = xmlTextReaderGetAttribute(ctx, (const xmlChar *)"url");
	if(!urlloc) {
		rv->parse_result = PARSER_ERROR;
	}
}

static struct rv parse_release_notes(xmlTextReaderPtr ctx, struct context *whoami) {
	struct rv rv = { .parse_result = RELNOTES_URL };
	parse_url(ctx, &(rv.rnurl), &rv);
	return rv;
}

static struct rv parse_latest_url(xmlTextReaderPtr ctx, struct context *whoami) {
	struct rv rv = { .parse_result = UPDATE_URL };
	parse_url(ctx, &(rv.uurl), &rv);
	return rv;
}

static struct rv parse_upgrade_version(xmlTextReaderPtr ctx, struct context *whoami) {
	struct rv rv = { .parse_result = CONTINUE_PARSING };
	struct version_triplet min = { 0, 0, 0 };
	struct version_triplet max = { INT_MAX, 0, 0 };
	int rc;
	bool done = false;
	do {
		rc = xmlTextReaderRead(ctx);
		if(rc < 0) {
			rv.parse_result = PARSER_ERROR;
			return rv;
		}
		const xmlChar *curnode = xmlTextReaderConstLocalName(ctx);
		switch(xmlTextReaderNodeType(ctx)) {
			case XML_READER_TYPE_ELEMENT:
				if(!strcmp(curnode, "min")) {
					if(parse_version(ctx, &min) != VERSION_FOUND) {
						rv.parse_result = PARSER_ERROR;
						return rv;
					}
				} else if(!strcmp(curnode, "max")) {
					if(parse_version(ctx, &max) != VERSION_FOUND) {
						rv.parse_result = PARSER_ERROR;
						return rv;
					}
				}
				break;
			case XML_READER_TYPE_END_ELEMENT:
				if(!strcmp(curnode, "auto-upgrade-version")) {
					done = true;
				}
				break;
			default:
				// nothing to do
				break;
		}
	} while(!done);
	if((cmp_version(&min, &(whoami->version)) <= 0) && (cmp_version(&(whoami->version), &max) <= 0)) {
		rv.parse_result = CONSTRAINT_MATCHED;
	}
	return rv;
}

static struct rv parse_old_os(xmlTextReaderPtr ctx, struct context *whoami) {
	struct rv rv = { .parse_result = LATEST_SUPPORTED };
	char *version = xmlTextReaderGetAttribute(ctx, (const xmlChar*) "version");
	if(strcmp(whoami->osversion, version)) {
		rv.parse_result = NOT_INTERESTING;
		return rv;
	}
	int rc;
	do {
		rc = xmlTextReaderRead(ctx);
	} while(rc > 0 && xmlTextReaderNodeType(ctx) != XML_READER_TYPE_ELEMENT && strcmp(xmlTextReaderConstLocalName(ctx), "oldOS-latest-version"));
	if(parse_version(ctx, &(rv.version)) != VERSION_FOUND) {
		rv.parse_result = PARSER_ERROR;
		return rv;
	}
	int res = cmp_version(&(whoami->version), &(rv.version));
	rv.parse_result = VERSION_FOUND | LATEST_SUPPORTED;
	if(res < 0) {
		rv.parse_result |= CONSTRAINT_MATCHED;
	} else {
		rv.parse_result |= NO_UPGRADE;
	}
	return rv;
}

struct element_desc elements[] = {
	{ "OS", parse_os },
	{ "comp", parse_comp },
	{ "latest-version", parse_latest_version },
	{ "latest-url", parse_latest_url },
	{ "release-notes", parse_release_notes },
	{ "auto-upgrade-version", parse_upgrade_version },
	{ "oldOS", parse_old_os },
};

struct upgrade_info *eid_vwr_upgrade_info(char *xml, size_t xmllen, char *osname, char *osversion, int major, int minor, int build) {
	xmlTextReaderPtr reader = xmlReaderForDoc((const xmlChar *)xml, NULL, NULL, 0);
	int rc;
	if(upgrade_info != NULL) return upgrade_info;
	upgrade_info = calloc(sizeof(struct upgrade_info), 1);
	upgrade_info->have_upgrade = false;

	if(reader == NULL) {
		be_log(EID_VWR_LOG_COARSE, "Could not check for most recent version: could not parse XML");
		rc = -1;
		goto out;
	}
	struct context whoami = { .osname = osname, .osversion = osversion, .version = {.major = major, .minor = minor, .build = build}};
	while((rc = xmlTextReaderRead(reader)) > 0) {
		const xmlChar *curnode = xmlTextReaderConstLocalName(reader);
		if(xmlTextReaderNodeType(reader) != XML_READER_TYPE_ELEMENT) {
			continue;
		}
		bool found = false;
		for(int i = 0; i < (sizeof(elements) / sizeof(elements[0])) && !found; i++) {
			if(!strcmp(elements[i].name, curnode)) {
				found = true;
				struct rv rv = elements[i].parse(reader, &whoami);
				if(rv.parse_result & VERSION_FOUND) {
					upgrade_info->new_version = rv.version;
				}
				if(rv.parse_result & CONSTRAINT_MATCHED) {
                                        if(cmp_version(&(whoami.version), &(upgrade_info->new_version)) < 0) {
                                                upgrade_info->have_upgrade = true;
                                        }
				}
				if(rv.parse_result & NOT_INTERESTING) {
					xmlTextReaderNext(reader);
				}
				if(rv.parse_result & RELNOTES_URL) {
					upgrade_info->relnotes_url = rv.rnurl;
				}
				if(rv.parse_result & UPDATE_URL) {
					upgrade_info->upgrade_url = rv.uurl;
				}
				if(rv.parse_result & NO_UPGRADE) {
					upgrade_info->have_upgrade = false;
				}
				if(rv.parse_result & LATEST_SUPPORTED) {
					goto out;
				}
				if(rv.parse_result & PARSER_ERROR) {
					be_log(EID_VWR_LOG_COARSE, "Could not parse version information: parse error");
					rc = -1;
					upgrade_info->have_upgrade = false;
					goto out;
				}
			}
		}
	}
out:
	if(rc < 0) {
		xmlError *err = xmlGetLastError();
		if(err != NULL) {
			be_log(EID_VWR_LOG_DETAIL, "Error parsing version file: %s", err->message);
		}
	}
	if(reader) {
		xmlFreeTextReader(reader);
	}
	return upgrade_info;
}
