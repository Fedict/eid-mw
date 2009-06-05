// test.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eidlib.h"

#ifdef _WIN32
#include <tchar.h>
#include <windows.h>
#ifdef _DEBUG
        #pragma comment( lib, "beidlibd" )
#else
        #pragma comment( lib, "beidlib" )
#endif
#endif

#define PRINT( label_, data_ )    printf("%-25s: %s\n", label_, data_)
#ifdef _WIN32
#define PRINTU( label_, data_ )    _tprintf(_T("%-25s: %s\n"), label_, data_)
#else
#define _T
#define PRINTU( label_, data_ )    printf(_T("%-25s: %s\n"), label_, data_)
#endif

void PrintIDData(BEID_ID_Data *pData)
{
    PRINT("Card Number", pData->cardNumber);
    PRINT("Chip Number", pData->chipNumber);
    printf("%-25s: %s - %s\n", "Validity", pData->validityDateBegin, pData->validityDateEnd);
    PRINTU(_T("Delivery Municipality"), pData->municipality);
    PRINT("National Number", pData->nationalNumber);
    PRINTU(_T("Name"), pData->name);
    PRINTU(_T("First name 1"), pData->firstName1);
    PRINTU(_T("First name 2"), pData->firstName2);
    PRINTU(_T("First name 3"), pData->firstName3);
    PRINT("Nationality", pData->nationality);
    PRINTU(_T("Birthplace"), pData->birthLocation);
    PRINT("Birthdate", pData->birthDate);
    PRINT("Gender", pData->sex);
    PRINTU(_T("Noble Condition"), pData->nobleCondition);
    printf("%-25s: %d\n", "Document Type", pData->documentType);
    printf("Special Status: Whitecane: %s, Yellowcane: %s, Extendedminority: %s\n",  pData->whiteCane ? "TRUE" : "FALSE", 
            pData->yellowCane ? "TRUE" : "FALSE", pData->extendedMinority ? "TRUE" : "FALSE");
    printf("\n");
}

void PrintAddressData(BEID_Address *pData)
{
    PRINTU(_T("Street"), pData->street);
    PRINT("Number", pData->streetNumber);
    PRINT("Box", pData->boxNumber);
    PRINT("Zip", pData->zip);
    PRINTU(_T("Municipality"), pData->municipality);
    PRINT("Country", pData->country);
    printf("\n");
}


#define PRINTBYTES( label_, data_, length_ )    { printf("%-25s: ", label_ ); for (int i = 0; i < length_; i++) printf("%02X", data_[i]); printf("\n"); }
#define PRINTBYTE( label_, data_ )              printf("%-25s: %02X\n", label_ , data_);

void PrintVersionInfo(BEID_VersionInfo *pInfo)
{
    PRINTBYTES("Serial Number",  pInfo->SerialNumber, 16 );
    PRINTBYTE("ComponentCode", pInfo->ComponentCode);
    PRINTBYTE("OSNumber", pInfo->OSNumber);
    PRINTBYTE("OSVersion", pInfo->OSVersion);
    PRINTBYTE("SoftmaskNumber", pInfo->SoftmaskNumber);
    PRINTBYTE("SoftmaskVersion", pInfo->SoftmaskVersion);
    PRINTBYTE("AppletVersion", pInfo->AppletVersion);
    printf("%-25s: %d\n", "GlobalOSVersion", pInfo->GlobalOSVersion);
    PRINTBYTE("AppletInterfaceVersion", pInfo->AppletInterfaceVersion);
    PRINTBYTE("PKCS1Support", pInfo->PKCS1Support);
    PRINTBYTE("KeyExchangeVersion", pInfo->KeyExchangeVersion);
    PRINTBYTE("ApplicationLifeCycle", pInfo->ApplicationLifeCycle);
    PRINTBYTE("GraphPerso", pInfo->GraphPerso);
    PRINTBYTE("ElecPerso", pInfo->ElecPerso);
    PRINTBYTE("ElecPersoInterface", pInfo->ElecPersoInterface);
    printf("\n");
}


void PrintStatus( const char title[], BEID_Status tStatus )
{
    printf( "\n === %s ===\n", title );

    if ( BEID_OK == tStatus.general ) return;

    printf( " *** Return codes: general=%d, system=%d, PC/SC=%d, Card SW=%02X%02X\n",
            tStatus.general, tStatus.system, tStatus.pcsc, tStatus.cardSW[0], tStatus.cardSW[1]
          );

}

const char *VerifyCertErrorString(long n)
{
	switch ((int)n)
	{
	case BEID_CERTSTATUS_CERT_VALIDATED_OK:
		return("Valid");
	case BEID_CERTSTATUS_CERT_NOT_VALIDATED:
		return("Not validated");
	case BEID_CERTSTATUS_UNABLE_TO_GET_ISSUER_CERT:
		return("Unable to get issuer certificate");
	case BEID_CERTSTATUS_UNABLE_TO_GET_CRL:
		return("Unable to get certificate CRL");
	case BEID_CERTSTATUS_UNABLE_TO_DECRYPT_CERT_SIGNATURE:
		return("Unable to decrypt certificate's signature");
	case BEID_CERTSTATUS_UNABLE_TO_DECRYPT_CRL_SIGNATURE:
		return("Unable to decrypt CRL's signature");
	case BEID_CERTSTATUS_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY:
		return("Unable to decode issuer public key");
	case BEID_CERTSTATUS_CERT_SIGNATURE_FAILURE:
		return("Certificate signature failure");
	case BEID_CERTSTATUS_CRL_SIGNATURE_FAILURE:
		return("CRL signature failure");
	case BEID_CERTSTATUS_CERT_NOT_YET_VALID:
		return("Certificate is not yet valid");
	case BEID_CERTSTATUS_CRL_NOT_YET_VALID:
		return("CRL is not yet valid");
	case BEID_CERTSTATUS_CERT_HAS_EXPIRED:
		return("Certificate has expired");
	case BEID_CERTSTATUS_CRL_HAS_EXPIRED:
		return("CRL has expired");
	case BEID_CERTSTATUS_ERR_IN_CERT_NOT_BEFORE_FIELD:
		return("Format error in certificate's notBefore field");
	case BEID_CERTSTATUS_ERR_IN_CERT_NOT_AFTER_FIELD:
		return("Format error in certificate's notAfter field");
	case BEID_CERTSTATUS_ERR_IN_CRL_LAST_UPDATE_FIELD:
		return("Format error in CRL's lastUpdate field");
	case BEID_CERTSTATUS_ERR_IN_CRL_NEXT_UPDATE_FIELD:
		return("Format error in CRL's nextUpdate field");
	case BEID_CERTSTATUS_OUT_OF_MEM:
		return("Out of memory");
	case BEID_CERTSTATUS_DEPTH_ZERO_SELF_SIGNED_CERT:
		return("Self signed certificate");
	case BEID_CERTSTATUS_SELF_SIGNED_CERT_IN_CHAIN:
		return("Self signed certificate in certificate chain");
	case BEID_CERTSTATUS_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
		return("Unable to get local issuer certificate");
	case BEID_CERTSTATUS_UNABLE_TO_VERIFY_LEAF_SIGNATURE:
		return("Unable to verify the first certificate");
	case BEID_CERTSTATUS_CERT_CHAIN_TOO_LONG:
		return("Certificate chain too long");
	case BEID_CERTSTATUS_CERT_REVOKED:
		return("Certificate revoked");
	case BEID_CERTSTATUS_INVALID_CA:
		return ("Invalid CA certificate");
	case BEID_CERTSTATUS_PATH_LENGTH_EXCEEDED:
		return ("Path length constraint exceeded");
	case BEID_CERTSTATUS_INVALID_PURPOSE:
		return ("Unsupported certificate purpose");
	case BEID_CERTSTATUS_CERT_UNTRUSTED:
		return ("Certificate not trusted");
	case BEID_CERTSTATUS_CERT_REJECTED:
		return ("Certificate rejected");
	case BEID_CERTSTATUS_SUBJECT_ISSUER_MISMATCH:
		return("Subject issuer mismatch");
	case BEID_CERTSTATUS_AKID_SKID_MISMATCH:
		return("Authority and subject key identifier mismatch");
	case BEID_CERTSTATUS_AKID_ISSUER_SERIAL_MISMATCH:
		return("Authority and issuer serial number mismatch");
	case BEID_CERTSTATUS_KEYUSAGE_NO_CERTSIGN:
		return("Key usage does not include certificate signing");
	case BEID_CERTSTATUS_UNABLE_TO_GET_CRL_ISSUER:
		return("Unable to get CRL issuer certificate");
	case BEID_CERTSTATUS_UNHANDLED_CRITICAL_EXTENSION:
		return("Unhandled critical extension");

	default:
        return "Unknown status";
		}
}


void PrintCertifCheck( BEID_Certif certif )
{
    printf( " *** Certificate \"%s\" status: %d, (%s)\n", certif.certifLabel, certif.certifStatus, VerifyCertErrorString(certif.certifStatus) );
}


void PrintSignCheck( BEID_Certif_Check tCheck )
{
    static char *resultStr[] = { "System error",
                                     "Valid",
                                     "Invalid",
                                     "Valid & wrong RRN certificate",
                                     "Invalid & wrong RRN certificate"
                                    };
    static char *resultStrPol[] = { "None",
                                     "OCSP",
                                     "CRL",
                                     "Both"
                                    };


    printf( " *** Signature result: %d, (%s)\n", tCheck.signatureCheck, resultStr[tCheck.signatureCheck + 1] );
    
    printf( " *** Certificate checking used policy: %d, (%s)\n", tCheck.usedPolicy, resultStrPol[tCheck.usedPolicy] );
    for ( int i = 0; i < tCheck.certificatesLength; i++ ) PrintCertifCheck( tCheck.certificates[i] );
}


int main(int argc, char* argv[])
{
    // Alround test buffer
    BYTE buffer[4096] = {0};
    BEID_Bytes tBytes = {0};
    tBytes.length = 4096;
    tBytes.data = buffer;

    BEID_Status tStatus = {0};
    BEID_ID_Data idData = {0};
    BEID_Address adData = {0};
    BEID_Certif_Check tCheck = {0};
    BEID_VersionInfo tVersion = {0};
    BEID_Raw tRawData = {0};

    long lLeft = 0;
    // Application ID
    BEID_Bytes tAID = {(unsigned char *)"\xA0\x00\x00\x01\x77\x50\x4B\x43\x53\x2D\x31\x35", 12};
    // RN Certificate
    BEID_Bytes tFileID = {(unsigned char *)"\x50\x38", 2};

    long lHandle = 0;
    //BEID_OCSP_CRL_MANDATORY
    //BEID_OCSP_CRL_OPTIONAL
    //BEID_OCSP_CRL_NOT_USED
    bool bCont = true;
    //while(bCont)
    {
    tStatus = BEID_Init(""/*"VIRTUAL"*/, 0, 0, &lHandle);

    // tStatus = BEID_GetRawData(&tRawData);
  /*  PrintStatus( "Get Raw Data", tStatus );


    if(tStatus.general == BEID_OK)
    {
        char szRawFile[64] = {0};
        FILE *pfraw = NULL;
    
        sprintf(szRawFile, "id.bin");
        pfraw = fopen(szRawFile, "w+b");
        if(pfraw != NULL)
        {
            fwrite(tRawData.idData, sizeof(unsigned char), tRawData.idLength, pfraw);
            fclose(pfraw);
            pfraw = NULL;
        }

        sprintf(szRawFile, "idsig.bin");
        pfraw = fopen(szRawFile, "w+b");
        if(pfraw != NULL)
        {
            fwrite(tRawData.idSigData, sizeof(unsigned char), tRawData.idSigLength, pfraw);
            fclose(pfraw);
            pfraw = NULL;
        }

        sprintf(szRawFile, "addr.bin");
        pfraw = fopen(szRawFile, "w+b");
        if(pfraw != NULL)
        {
            fwrite(tRawData.addrData, sizeof(unsigned char), tRawData.addrLength, pfraw);
            fclose(pfraw);
            pfraw = NULL;
        }

        sprintf(szRawFile, "addrSig.bin");
        pfraw = fopen(szRawFile, "w+b");
        if(pfraw != NULL)
        {
            fwrite(tRawData.addrSigData, sizeof(unsigned char), tRawData.addrSigLength, pfraw);
            fclose(pfraw);
            pfraw = NULL;
        }

        sprintf(szRawFile, "pic.bin");
        pfraw = fopen(szRawFile, "w+b");
        if(pfraw != NULL)
        {
            fwrite(tRawData.pictureData, sizeof(unsigned char), tRawData.pictureLength, pfraw);
            fclose(pfraw);
            pfraw = NULL;
        }

        sprintf(szRawFile, "carddata.bin");
        pfraw = fopen(szRawFile, "w+b");
        if(pfraw != NULL)
        {
            fwrite(tRawData.cardData, sizeof(unsigned char), tRawData.cardDataLength, pfraw);
            fclose(pfraw);
            pfraw = NULL;
        }

        sprintf(szRawFile, "tokeninfo.bin");
        pfraw = fopen(szRawFile, "w+b");
        if(pfraw != NULL)
        {
            fwrite(tRawData.tokenInfo, sizeof(unsigned char), tRawData.tokenInfoLength, pfraw);
            fclose(pfraw);
            pfraw = NULL;
        }

        sprintf(szRawFile, "certrn.bin");
        pfraw = fopen(szRawFile, "w+b");
        if(pfraw != NULL)
        {
            fwrite(tRawData.certRN, sizeof(unsigned char), tRawData.certRNLength, pfraw);
            fclose(pfraw);
            pfraw = NULL;
        }

        sprintf(szRawFile, "challenge.bin");
        pfraw = fopen(szRawFile, "w+b");
        if(pfraw != NULL)
        {
            fwrite(tRawData.challenge, sizeof(unsigned char), tRawData.challengeLength, pfraw);
            fclose(pfraw);
            pfraw = NULL;
        }

        sprintf(szRawFile, "response.bin");
        pfraw = fopen(szRawFile, "w+b");
        if(pfraw != NULL)
        {
            fwrite(tRawData.response, sizeof(unsigned char), tRawData.responseLength, pfraw);
            fclose(pfraw);
            pfraw = NULL;
        }
    }
    BEID_Exit();

    memset(&tRawData, 0, sizeof(BEID_Raw));

    tStatus = BEID_Init("VIRTUAL", 0, 0, &lHandle);

    if(tStatus.general == BEID_OK)
    {
        unsigned char szTemp[1024] = {0};
        char szRawFile[64] = {0};
        FILE *pfraw = NULL;
        int iRead = 0;
    
        sprintf(szRawFile, "id.bin");
        pfraw = fopen(szRawFile, "r+b");
        if(pfraw != NULL)
        {
           while( !feof( pfraw ) )
           {
              iRead = fread( szTemp, sizeof(unsigned char), sizeof(szTemp), pfraw );
              if(iRead > 0)
              {
                memcpy(tRawData.idData + tRawData.idLength, szTemp,  iRead);
                tRawData.idLength += iRead;
              }
           }
            fclose(pfraw);
            pfraw = NULL;
        }

        sprintf(szRawFile, "idsig.bin");
        pfraw = fopen(szRawFile, "r+b");
        if(pfraw != NULL)
        {
           while( !feof( pfraw ) )
           {
              iRead = fread( szTemp, sizeof(unsigned char), sizeof(szTemp), pfraw );
              if(iRead > 0)
              {
                memcpy(tRawData.idSigData + tRawData.idSigLength, szTemp,  iRead);
                tRawData.idSigLength += iRead;
              }
           }
            fclose(pfraw);
            pfraw = NULL;
        }

        sprintf(szRawFile, "addr.bin");
        pfraw = fopen(szRawFile, "r+b");
        if(pfraw != NULL)
        {
           while( !feof( pfraw ) )
           {
              iRead = fread( szTemp, sizeof(unsigned char), sizeof(szTemp), pfraw );
              if(iRead > 0)
              {
                memcpy(tRawData.addrData + tRawData.addrLength, szTemp,  iRead);
                tRawData.addrLength += iRead;
              }
           }         
            fclose(pfraw);
            pfraw = NULL;
        }

        sprintf(szRawFile, "addrSig.bin");
        pfraw = fopen(szRawFile, "r+b");
        if(pfraw != NULL)
        {
           while( !feof( pfraw ) )
           {
              iRead = fread( szTemp, sizeof(unsigned char), sizeof(szTemp), pfraw );
              if(iRead > 0)
              {
                memcpy(tRawData.addrSigData + tRawData.addrSigLength, szTemp,  iRead);
                tRawData.addrSigLength += iRead;
              }
           }         
            fclose(pfraw);
            pfraw = NULL;
        }

        sprintf(szRawFile, "pic.bin");
        pfraw = fopen(szRawFile, "r+b");
        if(pfraw != NULL)
        {
           while( !feof( pfraw ) )
           {
              iRead = fread( szTemp, sizeof(unsigned char), sizeof(szTemp), pfraw );
              if(iRead > 0)
              {
                memcpy(tRawData.pictureData + tRawData.pictureLength, szTemp,  iRead);
                tRawData.pictureLength += iRead;
              }
           }         
            fclose(pfraw);
            pfraw = NULL;
        }

        sprintf(szRawFile, "carddata.bin");
        pfraw = fopen(szRawFile, "r+b");
        if(pfraw != NULL)
        {
           while( !feof( pfraw ) )
           {
              iRead = fread( szTemp, sizeof(unsigned char), sizeof(szTemp), pfraw );
              if(iRead > 0)
              {
                memcpy(tRawData.cardData + tRawData.cardDataLength, szTemp,  iRead);
                tRawData.cardDataLength += iRead;
              }
           }        
            fclose(pfraw);
            pfraw = NULL;
        }

        sprintf(szRawFile, "tokeninfo.bin");
        pfraw = fopen(szRawFile, "r+b");
        if(pfraw != NULL)
        {
           while( !feof( pfraw ) )
           {
              iRead = fread( szTemp, sizeof(unsigned char), sizeof(szTemp), pfraw );
              if(iRead > 0)
              {
                memcpy(tRawData.tokenInfo + tRawData.tokenInfoLength, szTemp,  iRead);
                tRawData.tokenInfoLength += iRead;
              }
           }        
            fclose(pfraw);
            pfraw = NULL;
        }

        sprintf(szRawFile, "certrn.bin");
        pfraw = fopen(szRawFile, "r+b");
        if(pfraw != NULL)
        {
           while( !feof( pfraw ) )
           {
              iRead = fread( szTemp, sizeof(unsigned char), sizeof(szTemp), pfraw );
              if(iRead > 0)
              {
                memcpy(tRawData.certRN + tRawData.certRNLength, szTemp,  iRead);
                tRawData.certRNLength += iRead;
              }
           }        
            fclose(pfraw);
            pfraw = NULL;
        }

        sprintf(szRawFile, "challenge.bin");
        pfraw = fopen(szRawFile, "r+b");
        if(pfraw != NULL)
        {
           while( !feof( pfraw ) )
           {
              iRead = fread( szTemp, sizeof(unsigned char), sizeof(szTemp), pfraw );
              if(iRead > 0)
              {
                memcpy(tRawData.challenge + tRawData.challengeLength, szTemp,  iRead);
                tRawData.challengeLength += iRead;
              }
           }   
            fclose(pfraw);
            pfraw = NULL;
        }

        sprintf(szRawFile, "response.bin");
        pfraw = fopen(szRawFile, "r+b");
        if(pfraw != NULL)
        {
           while( !feof( pfraw ) )
           {
              iRead = fread( szTemp, sizeof(unsigned char), sizeof(szTemp), pfraw );
              if(iRead > 0)
              {
                memcpy(tRawData.response + tRawData.responseLength, szTemp,  iRead);
                tRawData.responseLength += iRead;
              }
           }   
            fclose(pfraw);
            pfraw = NULL;
        }
    }
    
    tStatus = BEID_SetRawData(&tRawData);
    PrintStatus( "Set Raw Data", tStatus );
*/

////////////// High Level Test //////////////////////
    // Read ID Data


    tStatus = BEID_GetID(&idData, &tCheck);
    PrintStatus( "Get Identity Data", tStatus );
    if(BEID_OK == tStatus.general)
    {
#ifdef WIN32
        int ol = MultiByteToWideChar(CP_UTF8, 0, idData.name, strlen(idData.name), 0, 0);
        WCHAR *tOut = new WCHAR[ol];
        MultiByteToWideChar(CP_UTF8, 0, idData.name, strlen(idData.name), tOut, ol);
#endif
        PrintIDData(&idData);
        PrintSignCheck(tCheck);
        for (int j = 0; j < tCheck.certificatesLength; j++)
        {
            char szFile[64] = {0};
            sprintf(szFile, "%s.der", tCheck.certificates[j].certifLabel);
            FILE *pfcert = fopen(szFile, "w+b");
            if(pfcert != NULL)
            {
                fwrite(tCheck.certificates[j].certif, sizeof(unsigned char), tCheck.certificates[j].certifLength, pfcert);
                fclose(pfcert);
            }
        }
    }
    memset(&tCheck, 0, sizeof(BEID_Certif_Check));

/*
    // Read Address Data
    tStatus = BEID_GetAddress(&adData, &tCheck);
    PrintStatus( "Get Address", tStatus );
    if(BEID_OK == tStatus.general)
    {
        PrintAddressData(&adData);
    }
    memset(&tCheck, 0, sizeof(BEID_Certif_Check));

    // Read Picture Data
    tStatus = BEID_GetPicture(&tBytes, &tCheck);
    PrintStatus( "Get Picture", tStatus );
    if(BEID_OK == tStatus.general)
    {
        FILE *pf = fopen("photo.jpg", "w+b");
        if(pf != NULL)
        {
            fwrite(tBytes.data, sizeof(unsigned char), tBytes.length, pf);
            fclose(pf);
        }
        printf("Picture written in \"photo.jpg\". Launching picture file ...\n");
     //  system("start photo.jpg");
    }
    memset(&tCheck, 0, sizeof(BEID_Certif_Check));
    // Re use buffer
    memset(buffer, 0, sizeof(buffer));
    tBytes.length = 4096;

    ////////////// Mid Level Test //////////////////////

*/
 /*  
    BEID_Pin tPin = {0};
    tPin.id = 0x01;
    tPin.pinType = BEID_PIN_TYPE_PKCS15;
    tPin.usageCode = BEID_USAGE_AUTH;

    // Verify Pin
    // NULL means:  Pinpad -> input on pinpad or No pinpad -> dialogbox popup
    // NO NULL means : Pinpad->input on pinpad or No pinpad -> no input asked
  
    tStatus = BEID_VerifyPIN(&tPin, NULL, &lLeft);
    PrintStatus( "Verify PIN", tStatus );
*/
    // Change Pin
   // tStatus = BEID_ChangePIN(&tPin, NULL, NULL, &lLeft); 


/*
    // Read RN Certificate
    tStatus = BEID_SelectApplication(&tAID);
    PrintStatus( "Select Application", tStatus );
    tStatus = BEID_ReadFile(&tFileID, &tBytes, &tPin);
    PrintStatus( "Read File", tStatus );
    printf( " RRN certificate length=%ld\n", tBytes.length );

    // Re use buffer
    memset(buffer, 0, sizeof(buffer));
    tBytes.length = 4096;
*/

    /*tStatus = BEID_SelectApplication(&tAID);
    PrintStatus( "Select Application", tStatus );
    tStatus = BEID_WriteFile(&tFileID, &tBytes, &tPin);
*/
    // Get version
    tStatus = BEID_GetVersionInfo(&tVersion, 0, &tBytes);
    PrintStatus( "Version", tStatus );
    if(BEID_OK == tStatus.general)
    {
        PrintVersionInfo(&tVersion);
    }
    // Re use buffer
    memset(buffer, 0, sizeof(buffer));
    tBytes.length = 4096;

/*
    // PIN Status
    tStatus = BEID_GetPINStatus(&tPin, &lLeft, 0, &tBytes);
    PrintStatus( "Get PIN status", tStatus );
    if ( BEID_OK == tStatus.general ) printf( " Number of PIN tries=%ld", lLeft );
    
    // Re use buffer
    memset(buffer, 0, sizeof(buffer));
    tBytes.length = 4096;


////////////// Low Level Test //////////////////////

*/

    /*
    // Low level test
//    tStatus = BEID_BeginTransaction();
   // if(BEID_OK == tStatus.general)
    //{
    BEID_Pin tPin = {0};
    tPin.id = 0x01;
    tPin.pinType = BEID_PIN_TYPE_PKCS15;
    tPin.usageCode = BEID_USAGE_AUTH;
    int iCount = 0;
    bool bCont = true;
    while (iCount < 100 && bCont)
    {
        // Send select SGNID
        BEID_Bytes tSendBytes = {(unsigned char *)"\x00\xA4\x08\x0C\x06\x3F\x00\xDF\x01\x40\x31", 11}; 
        BEID_Bytes tRespBytes = {0};
        BYTE respbuffer[256] = {0};
        tRespBytes.data = respbuffer;
        tRespBytes.length = 256;
        tStatus = BEID_SendAPDU(&tSendBytes, &tPin, &tRespBytes);
        if(tStatus.general == BEID_OK)
        {
            BEID_Bytes tSendBytesRead = {(unsigned char *)"\x00\xB0\x00\x00\xF4", 5}; 
            tRespBytes.length = 256;       
            tStatus = BEID_SendAPDU(&tSendBytesRead, &tPin, &tRespBytes);
            if(tStatus.general != BEID_OK)
                bCont = false;
        }
        else
        {
            bCont = false;
        }
        iCount++;
        printf("OK\n");
    }
        //tStatus = BEID_EndTransaction();
    //}
    // FlushCache
    //tStatus = BEID_FlushCache();

*/
    
    // Read AUTH Certificate
    BEID_Bytes tFileIDAUTH = {(unsigned char *)"\x50\x38", 2};
    // Re use buffer
    memset(buffer, 0, sizeof(buffer));
    tBytes.length = 4096;
    tStatus = BEID_ReadBinary(&tFileIDAUTH, 0, 5000, &tBytes);
    PrintStatus( "Read File", tStatus );
    printf( " AUTH certificate length=%ld\n", tBytes.length );
    // Re use buffer
    memset(buffer, 0, sizeof(buffer));
    tBytes.length = 4096;
    tStatus = BEID_ReadBinary(&tFileIDAUTH, 800, 50, &tBytes);
    printf( " AUTH certificate length=%ld\n", tBytes.length );

    BEID_Exit();
}
    return 0;
}

