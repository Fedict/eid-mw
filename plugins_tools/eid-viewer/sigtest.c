#include <stdio.h>
#include <stdint.h>
#include "dataverify.h"

int main(void) {
	FILE* f;
	uint8_t data[512]; size_t datas;
	uint8_t dsig[512]; size_t dsigs;
	uint8_t addr[512]; size_t addrs;
	uint8_t asig[512]; size_t asigs;
	uint8_t cert[2048]; size_t certs;
	uint8_t phot[8192]; size_t phots;
	uint8_t hash[0x30]; size_t hashs;
	uint8_t *ptr;

	f = fopen("3F00_DF01_4031", "rb");
	datas = fread(data, 1, sizeof data, f);
	fclose(f);
	ptr = data;
	while(*ptr != 0x11) {
		ptr++;
		ptr += (*ptr);
		ptr++;
		if(ptr > (data + datas)) {
			exit(EXIT_FAILURE);
		}
	}
	ptr++;
	hashs = (size_t)(*ptr);
	ptr++;
	memcpy(hash, ptr, hashs);
	f = fopen("3F00_DF01_4032", "rb");
	dsigs = fread(dsig, 1, sizeof dsig, f);
	fclose(f);
	f = fopen("3F00_DF01_4033", "rb");
	addrs = fread(addr, 1, sizeof addr, f);
	fclose(f);
	f = fopen("3F00_DF01_4034", "rb");
	asigs = fread(asig, 1, sizeof asig, f);
	fclose(f);
	f = fopen("3F00_DF01_4035", "rb");
	phots = fread(phot, 1, sizeof phot, f);
	fclose(f);
	f = fopen("3F00_DF00_503C", "rb");
	certs = fread(cert, 1, sizeof cert, f);
	fclose(f);
	return !eid_vwr_check_data_validity(phot, phots, hash, hashs, data, datas, dsig, dsigs, addr, addrs, asig, asigs, cert, certs);
}
