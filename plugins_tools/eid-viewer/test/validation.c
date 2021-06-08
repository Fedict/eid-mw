#include <unix.h>
#include <pkcs11.h>
#include <testlib2.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "../dataverify.h"

TEST_FUNC(validation) {
	FILE* f;
	uint8_t data[512]; size_t datas;
	uint8_t dsig[512]; size_t dsigs;
	uint8_t addr[512]; size_t addrs;
	uint8_t asig[512]; size_t asigs;
	uint8_t cert[2048]; size_t certs;
	uint8_t phot[8192]; size_t phots;
	uint8_t hash[0x30]; size_t hashs;
	uint8_t odata[512]; 
	uint8_t odsig[512]; 
	uint8_t oaddr[512]; 
	uint8_t oasig[512]; 
	uint8_t ocert[2048]; 
	uint8_t ophot[8192]; 
	uint8_t ohash[0x30]; 
	uint8_t *ptr;
	int offset;

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
	//happy flow
	if (!eid_vwr_check_data_validity(phot, phots, hash, hashs, data, datas, dsig, dsigs, addr, addrs, asig, asigs, cert, certs)){
   		printf("happy flow\n");
   		return TEST_RV_FAIL;
	}
	
	//unhappy flow
	int now = time(NULL); 
	//int now = 1622620131;
	srand(now);
	//data change
	if ( rand () % 2){
		offset = rand() % datas;
		printf("data->seed: %d, offset: %d\n",now , offset);
		memcpy(odata, data, datas);
		odata[offset]= rand() % 256;
		//printf("old data: %d, new data: %d\n",data[offset], odata[offset]);
		if (eid_vwr_check_data_validity(phot, phots, hash, hashs, odata, datas, dsig, dsigs, addr, addrs, asig, asigs, cert, certs)){
	   		return TEST_RV_FAIL;
		}
	}
	if ( rand () % 2){ 
		offset = rand() % dsigs;
		printf("dsig->seed: %d, offset: %d\n",now , offset);
		memcpy(odsig, dsig, dsigs);
		odsig[offset]= rand() % 256;
		//printf("old data: %d, new data: %d\n",dsig[offset], odsig[offset]);
		if (eid_vwr_check_data_validity(phot, phots, hash, hashs, data, datas, odsig, dsigs, addr, addrs, asig, asigs, cert, certs)){
	   		return TEST_RV_FAIL;
		}
	}
	if ( rand () % 2){ 
		offset = rand() % addrs;
		printf("addr->seed: %d, offset: %d\n",now , offset);
		memcpy(oaddr, addr, addrs);
		oaddr[offset]= rand() % 256;
		//printf("old data: %d, new data: %d\n",addr[offset], oaddr[offset]);
		if (eid_vwr_check_data_validity(phot, phots, hash, hashs, data, datas, dsig, dsigs, oaddr, addrs, asig, asigs, cert, certs)){
	   		return TEST_RV_FAIL;
		}
	}
	if ( rand () % 2){
		offset = rand() % asigs;
		printf("asig->seed: %d, offset: %d\n",now , offset);
		memcpy(oasig, asig, asigs);
		oasig[offset]= rand() % 256;
		//printf("old data: %d, new data: %d\n",asig[offset], oasig[offset]);
		if (eid_vwr_check_data_validity(phot, phots, hash, hashs, data, datas, dsig, dsigs, addr, addrs, asig, asigs, cert, certs)){
	   		return TEST_RV_FAIL;
		}
	}
	if ( rand () % 2){ 
		offset = rand() % certs;
		printf("cert->seed: %d, offset: %d\n",now , offset);
		memcpy(ocert, cert, certs);
		ocert[offset]= rand() % 256;
		//printf("old data: %d, new data: %d\n",cert[offset], ocert[offset]);
		if (eid_vwr_check_data_validity(phot, phots, hash, hashs, data, datas, dsig, dsigs, addr, addrs, asig, asigs, ocert, certs)){
	   		return TEST_RV_FAIL;
		}
	}
	if ( rand () % 2){ 
		offset = rand() % phots;
		printf("photo->seed: %d, offset: %d\n",now , offset);
		memcpy(ophot, phot, phots);
		ophot[offset]= rand() % 256;
		//printf("old data: %d, new data: %d\n",phot[offset], ophot[offset]);
		if (eid_vwr_check_data_validity(ophot, phots, hash, hashs, data, datas, dsig, dsigs, addr, addrs, asig, asigs, cert, certs)){
	   		return TEST_RV_FAIL;
		}
	}
	if ( rand () % 2){ 
		offset = rand() % hashs;
		printf("hash->seed: %d, offset: %d\n",now , offset);
		memcpy(ohash, hash, hashs);
		ohash[offset]= rand() % 256;
		//printf("old data: %d, new data: %d\n",hash[offset], ohash[offset]);
		if (eid_vwr_check_data_validity(phot, phots, ohash, hashs, data, datas, dsig, dsigs, addr, addrs, asig, asigs, cert, certs)){
	   		return TEST_RV_FAIL;
		}
	}
	return TEST_RV_OK;
}
