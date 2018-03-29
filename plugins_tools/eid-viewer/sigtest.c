#include <stdio.h>
#include <stdint.h>
#include "dataverify.h"

uint8_t photohash[] = { 0x67, 0xf4, 0x46, 0x1c, 0x77, 0x55, 0x1d, 0xef, 0x62, 0x44, 0xc3, 0xda, 0x75, 0x51, 0x2f, 0x1e, 0x9b, 0x4f, 0xb5, 0xa3 };

int main(void) {
	FILE* f;
	uint8_t data[256]; size_t datas;
	uint8_t dsig[256]; size_t dsigs;
	uint8_t addr[256]; size_t addrs;
	uint8_t asig[256]; size_t asigs;
	uint8_t cert[2048]; size_t certs;
	uint8_t phot[8192]; size_t phots;

	f = fopen("3F00_DF01_4031", "rb");
	datas = fread(data, 1, sizeof data, f);
	fclose(f);
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
	return !eid_vwr_check_data_validity(phot, phots, photohash, sizeof photohash, data, datas, dsig, dsigs, addr, addrs, asig, asigs, cert, certs);
}
