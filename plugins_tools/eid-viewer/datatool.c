#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "dataverify.h"
#include <eid-viewer/oslayer.h>
#include <errno.h>

struct option longopt[] = {
	{"identity",		required_argument, NULL, 'i'},
	{"identity-signature",	required_argument, NULL, 'I'},
	{"address",		required_argument, NULL, 'a'},
	{"address-signature",	required_argument, NULL, 'A'},
	{"photo",		required_argument, NULL, 'p'},
	{"rrncert",		required_argument, NULL, 'r'},
	{0,			0,		   NULL, 0}
};

void read_file(void** filedata, int* filelen, char *filename) {
	FILE *fp = fopen(filename, "rb");
        if(!fp) {
                perror("fopen");
                exit(EXIT_FAILURE);
        }
	fseek(fp, 0, SEEK_END);
	*filelen = ftell(fp);
        errno=0;
	rewind(fp);
        if(errno != 0) {
                perror("rewind");
                exit(EXIT_FAILURE);
        }
        if(*filelen == 0) {
                fprintf(stderr, "file %s is empty\n", filename);
                exit(EXIT_FAILURE);
        }
	*filedata = realloc(filedata, *filelen);
        if(filedata == NULL) {
                perror("realloc");
                exit(EXIT_FAILURE);
        }
	fread(*filedata, (size_t)(*filelen), 1, fp);
	fclose(fp);
}

void print_log(enum eid_vwr_loglevel level, const EID_CHAR *line) {
	if(level < EID_VWR_LOG_COARSE) {
		return;
	}
	printf("%s\n", line);
}

int main(int argc, char**argv) {
	struct eid_vwr_ui_callbacks *cb = eid_vwr_cbstruct();
	cb->log = print_log;
	eid_vwr_createcallbacks(cb);
	int c, uninteresting;
	void *photo, *photohash, *datafile, *datasig, *addrfile, *addrsig, *rrncert;
	photo = photohash = datafile = datasig = addrfile = addrsig = rrncert = NULL;
	int plen, hashlen, datfilelen, datsiglen, addfilelen, addsiglen, certlen;
	while((c=getopt_long(argc, argv, "i:I:a:A:p:r:", longopt,&uninteresting)) >= 0) {
		switch(c) {
			case 'i':
				read_file(&datafile, &datfilelen, optarg);
				break;
			case 'I':
				read_file(&datasig, &datsiglen, optarg);
				break;
			case 'a':
				read_file(&addrfile, &addfilelen, optarg);
				break;
			case 'A':
				read_file(&addrsig, &addsiglen, optarg);
				break;
			case 'p':
				read_file(&photo, &plen, optarg);
				break;
			case 'r':
				read_file(&rrncert, &certlen, optarg);
				break;
			default:
				fprintf(stderr, "E: unknown argument '%c'\n", c);
				exit(EXIT_FAILURE);
		}
	}
	if(photo == NULL || datafile == NULL || datasig == NULL || addrfile == NULL || addrsig == NULL || rrncert == NULL) {
		fprintf(stderr, "E: all arguments are required.\n");
		exit(EXIT_FAILURE);
	}
	char *tag = datafile;
	char *len = datafile + 1;
	while(*tag != 0x11 /* photo hash tag */) {
		assert(*len < 255);
		tag += *len + 2;
		len = tag + 1;
	}
	hashlen = *len;
	photohash = malloc(hashlen);
	memcpy(photohash, len + 1, hashlen);
	// all done now, do the actual verification
	printf("%s\n", (eid_vwr_check_data_validity(photo, plen, photohash, hashlen, datafile, datfilelen, datasig, datsiglen, addrfile, addfilelen, addrsig, addsiglen, rrncert, certlen) == 1) ? "Data verifies" : "Data verification FAILED");
	return 0;
}
