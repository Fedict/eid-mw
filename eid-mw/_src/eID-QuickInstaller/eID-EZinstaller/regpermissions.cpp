// regpermissions.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <string>
#include <aclapi.h>
#include "Tlhelp32.h" 
#include <psapi.h>

#include "Sddl.h"

BOOL SCardSvrAsLS();
int LSCheckAndSetReadperm(HKEY hRootKey, const wchar_t *wzKey);


BOOL IsLocalhostSid(PSID SidStart)
{
	BOOL retval = FALSE;
	if (FALSE == IsValidSid(SidStart))
	{
		wprintf(L"Invalid Sid given, cannot parse\n");
		return retval;
	}
	LPTSTR stringSid = NULL;
	if(FALSE == ConvertSidToStringSid(SidStart,&stringSid))
	{
		wprintf(L"Could not convert SID to SIDString\n");
		return retval;
	}
	wprintf(L"SID = %s\n",stringSid);

	PSID_IDENTIFIER_AUTHORITY sidia = GetSidIdentifierAuthority(SidStart);

	PUCHAR pSubAuthorityCount = GetSidSubAuthorityCount(SidStart);

	UCHAR counter = 0;
	for(; counter < *pSubAuthorityCount ; counter++)
	{
		PDWORD pSidSubAuthority = GetSidSubAuthority(SidStart,counter);
		BYTE nullauthorityValue[6] = SECURITY_NULL_SID_AUTHORITY;
		BYTE worldauthorityValue[6] = SECURITY_WORLD_SID_AUTHORITY;
		BYTE localauthorityValue[6] = SECURITY_LOCAL_SID_AUTHORITY;
		BYTE creatorauthorityValue[6] = SECURITY_CREATOR_SID_AUTHORITY;
		BYTE ntauthorityValue[6] = SECURITY_NT_AUTHORITY;

		if( (memcmp(sidia->Value,nullauthorityValue,6) == 0) ||
			(memcmp(sidia->Value,worldauthorityValue,6) == 0) ||
			(memcmp(sidia->Value,localauthorityValue,6) == 0) ||
			(memcmp(sidia->Value,creatorauthorityValue,6) == 0) ||
			(memcmp(sidia->Value,ntauthorityValue,6) == 0) )
		{
			if( (*pSidSubAuthority) ==  SECURITY_LOCAL_SERVICE_RID )
			{
				wprintf(L"LOCAL_SERVICE\n");
				retval = true;
			}
		}
	}
	return retval;
}

BOOL ReadAccesSetInMask(DWORD aceMask)
{
	BOOL retval = FALSE;
	//we also need the rigts to query the subkeys, so READ_CONTROL or GENERIC_READ on their own are not enough

	if((KEY_READ & aceMask)==KEY_READ)
		retval = TRUE;

	return retval;
}

BOOL ReadAccessForLocalHost(PACL pDacl)
{
	BOOL retval = FALSE;
	DWORD i = 0;
	LPVOID pAce = NULL;
	PACE_HEADER pAceheader=NULL;

	for(;i<pDacl->AceCount;i++)
	{
		GetAce(pDacl,i,&pAce);
		pAceheader = (PACE_HEADER)pAce;
		pAceheader->AceType;
		pAceheader->AceSize;
		wprintf(L"-------------------\n",pAceheader->AceFlags);
		wprintf(L"AceFlags are 0x%.2x\n",pAceheader->AceFlags);
		//RegistrywprintfAceFlags(pAceheader->AceFlags);
		wprintf(L"AceSize  is  0x%.2x\n",pAceheader->AceSize);
		wprintf(L"AceType  is  0x%.2x\n",pAceheader->AceType);
		switch(pAceheader->AceType)
		{
		case ACCESS_ALLOWED_ACE_TYPE:
		case ACCESS_DENIED_ACE_TYPE:
		case SYSTEM_AUDIT_ACE_TYPE:
		case SYSTEM_ALARM_ACE_TYPE:
			//case SYSTEM_MANDATORY_LABEL_ACE:
			{
				PACCESS_ALLOWED_ACE pAccessAllowedAce = (PACCESS_ALLOWED_ACE)pAce;
				if (IsLocalhostSid( &(pAccessAllowedAce->SidStart)))
				{
					wprintf(L"Localhost");
					if(ReadAccesSetInMask(pAccessAllowedAce->Mask))
					{
						retval = TRUE;
					}
				}
			}
			break;
		case ACCESS_ALLOWED_OBJECT_ACE_TYPE:
		case ACCESS_DENIED_OBJECT_ACE_TYPE:
		case SYSTEM_AUDIT_OBJECT_ACE_TYPE:
		case SYSTEM_ALARM_OBJECT_ACE_TYPE:
			{
				PACCESS_ALLOWED_OBJECT_ACE pAccessAllowedAce = (PACCESS_ALLOWED_OBJECT_ACE)pAce;
				if (IsLocalhostSid( &(pAccessAllowedAce->SidStart)))
				{
					wprintf(L"Localhost");
					if(ReadAccesSetInMask(pAccessAllowedAce->Mask))
					{
						wprintf(L"has read access\n");
						retval = TRUE;
					}
					else
					{
						wprintf(L"does not have read access\n");
					}
				}
			}
			break;

		default:
			wprintf(L"AceType unknow\n");
			wprintf(L"Don't know the ACE type, cannot parse the ACE");
			break;
		};
	}

	return retval;
}

BOOL AddReadAccessForLocalHost(PACL pOldDacl, PACL *pNewDacl)
{
	BOOL retval = FALSE;
//	DWORD i = 0;
//	LPVOID pAce = NULL;
//	PACE_HEADER pAceheader=NULL;
	TRUSTEE trusteeLs ;
	PSID pSidLs = NULL;
	EXPLICIT_ACCESS readAccessls;
	int err = ERROR_SUCCESS;

	if ( ConvertStringSidToSidW(L"S-1-5-19",&pSidLs) != 0 )
	{
		BuildTrusteeWithSid(&trusteeLs,pSidLs);
		//this one sets the trustee form to TRUSTEE_IS_NAME , while its TRUSTEE_IS_SID
		//BuildExplicitAccessWithName(&readAccessls,trusteeLs.ptstrName,KEY_READ,GRANT_ACCESS,SUB_CONTAINERS_AND_OBJECTS_INHERIT);
		
		readAccessls.grfAccessPermissions = KEY_READ;
		readAccessls.grfAccessMode = GRANT_ACCESS ;
		readAccessls.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
		readAccessls.Trustee = trusteeLs;

		err = SetEntriesInAcl(1, &readAccessls,pOldDacl,pNewDacl);
		if ( err != ERROR_SUCCESS )
		{
			wprintf(L"SetEntriesInAcl failed with %d\n",err);
		}
		else
		{
			retval = TRUE;
		}
		LocalFree(pSidLs);
	}
	else
	{
		err = GetLastError();
		switch (err)
		{
		case ERROR_INVALID_SID:
			wprintf(L"ERROR_INVALID_SID\n");
			break;
		case ERROR_INVALID_PARAMETER:
			wprintf(L"ERROR_INVALID_PARAMETER\n");
			break;
		}
	}

	return retval;
}

int LSCheckAndSetReadperm(HKEY hRootKey, const wchar_t *wzKey)
{
	int		retval = 0;

	int err = ERROR_SUCCESS;

	HKEY hRegKey;

	if(ERROR_SUCCESS != (err = RegOpenKeyExW(hRootKey, wzKey, 0L, KEY_ALL_ACCESS , &hRegKey)))
	{
		if(err != ERROR_FILE_NOT_FOUND)
		{
			return -1;
		}
		else
		{
			return -2;
		}
	}
	PSECURITY_DESCRIPTOR pSecurityDescriptor = NULL;
	DWORD lcbSecurityDescriptor = 0;

	if(ERROR_INSUFFICIENT_BUFFER == (err = RegGetKeySecurity(hRegKey, DACL_SECURITY_INFORMATION, pSecurityDescriptor, &lcbSecurityDescriptor)))
	{     
		pSecurityDescriptor = (PSECURITY_DESCRIPTOR)malloc(lcbSecurityDescriptor);

		if(ERROR_SUCCESS != (err = RegGetKeySecurity(hRegKey, DACL_SECURITY_INFORMATION, pSecurityDescriptor, &lcbSecurityDescriptor)))
		{
			retval = -3;
		}
		else
		{
			BOOL bDaclPresent = FALSE;
			BOOL bDaclDefaulted = FALSE;
//			BOOL bReadAccessLH = FALSE;
			PACL pDacl = NULL;

			GetSecurityDescriptorDacl(pSecurityDescriptor, &bDaclPresent, &pDacl, &bDaclDefaulted);
			if(bDaclPresent == TRUE)
			{
				if(pDacl == NULL)
				{
					wprintf(L"A NULL discretionary access control list (DACL) found \nA NULL DACL implicitly allows all access to an object.\n");
				}
				else
				{
					wprintf(L"A discretionary access control list (DACL) was found with Length = %d\n",pDacl->AclSize);
					wprintf(L"Number of Access Control Elements (ACE's): %d\n",pDacl->AceCount);
					if (ReadAccessForLocalHost(pDacl) == FALSE)
					{
						PACL newDacl = NULL;
						if (AddReadAccessForLocalHost(pDacl, &newDacl) == TRUE)
						{
							err = SetSecurityInfo(hRegKey,SE_REGISTRY_KEY,DACL_SECURITY_INFORMATION,NULL,NULL,newDacl,NULL);
							if( err != ERROR_SUCCESS )
							{
								retval = -4;
								wprintf(L"SetSecurityInfo failed with %s/n",err);
							}
						}


						/*
						LPTSTR StringNewSecurityDescriptor = NULL;
						ULONG ulStringNewSecurityDescriptorLen = 0;
						LPTSTR StringOldSecurityDescriptor = NULL;
						ULONG ulStringOldSecurityDescriptorLen = 0;
						PSECURITY_DESCRIPTOR pNewSecurityDescriptor = NULL;
						ULONG ulNewSecurityDescriptorSize = 0;

						if (ConvertSecurityDescriptorToStringSecurityDescriptor(pSecurityDescriptor,SDDL_REVISION_1,DACL_SECURITY_INFORMATION,
							&StringOldSecurityDescriptor,&ulStringOldSecurityDescriptorLen) != NULL)
						{
							wprintf(L"Old: %s\n",(const wchar_t*)StringOldSecurityDescriptor);
							ulStringNewSecurityDescriptorLen = ulStringOldSecurityDescriptorLen + 16;
							StringNewSecurityDescriptor = (LPTSTR)malloc(ulStringNewSecurityDescriptorLen * sizeof (WCHAR));
							if (StringNewSecurityDescriptor != NULL)
							{
								wcscpy(StringNewSecurityDescriptor,StringOldSecurityDescriptor);
								wcscpy(StringNewSecurityDescriptor + wcslen(StringOldSecurityDescriptor),TEXT("(A;OICI;GR;;;LS)") );
								//					TEXT("(A;OICI;GR;;;LS)") // Allow general read permission to local host
								//OBJECT_INHERIT_ACE : Non-container child objects inherit the ACE as an effective ACE.
								//CONTAINER_INHERIT_ACE : The ACE has an effect on child namespaces as well as the current namespace.
								wprintf(L"New %s\n",(const wchar_t*)StringNewSecurityDescriptor);

								if (ConvertStringSecurityDescriptorToSecurityDescriptor( StringNewSecurityDescriptor,SDDL_REVISION_1,
									&pNewSecurityDescriptor,&ulNewSecurityDescriptorSize) != NULL)
								{
									return 1;
									//parse info see http://msdn.microsoft.com/en-us/library/aa379570%28v=vs.85%29.aspx
									if( ERROR_SUCCESS != (err = RegSetKeySecurity(hRegKey,DACL_SECURITY_INFORMATION,pNewSecurityDescriptor)) )
									{
										retval = -4;
										wprintf(L"RegSetKeySecurity failed with %s/n",err);
									}
									else
									{										
										LocalFree(pNewSecurityDescriptor);
									}									
								}
								free(StringNewSecurityDescriptor);
							}
							else
							{
								wprintf(L"malloc failed\n");
							}
							LocalFree(StringOldSecurityDescriptor);
						}*/
					}
				}
			}
			else
			{
				wprintf(L"No discretionary access control list (DACL) found \n");
			}
		}
		free(pSecurityDescriptor);
	}
	else
	{
		wprintf(L"RegGetKeySecurity failed",err);
	}

	if(ERROR_SUCCESS != (err = RegCloseKey(hRegKey)))
	{
		wprintf(L"RegCloseKey failed",err);
	}

	return retval;
}

int LSAddReadperm(HKEY hRootKey, const wchar_t *wzKey)
{
	int		retval = 0;

	int err = ERROR_SUCCESS;

	HKEY hRegKey;

	if(ERROR_SUCCESS != (err = RegOpenKeyExW(hRootKey, wzKey, 0L, KEY_ALL_ACCESS , &hRegKey)))
	{
		if(err != ERROR_FILE_NOT_FOUND)
		{
			return -1;
		}
		else
		{
			return -2;
		}
	}
	PSECURITY_DESCRIPTOR pSecurityDescriptor = NULL;
	DWORD lcbSecurityDescriptor = 0;

	if(ERROR_INSUFFICIENT_BUFFER == (err = RegGetKeySecurity(hRegKey, DACL_SECURITY_INFORMATION, pSecurityDescriptor, &lcbSecurityDescriptor)))
	{     
		pSecurityDescriptor = (PSECURITY_DESCRIPTOR)malloc(lcbSecurityDescriptor);

		if(ERROR_SUCCESS != (err = RegGetKeySecurity(hRegKey, DACL_SECURITY_INFORMATION, pSecurityDescriptor, &lcbSecurityDescriptor)))
		{
			retval = -3;
		}
		else
		{
			BOOL bDaclPresent = FALSE;
			BOOL bDaclDefaulted = FALSE;
			PACL pDacl = NULL;

			GetSecurityDescriptorDacl(pSecurityDescriptor, &bDaclPresent, &pDacl, &bDaclDefaulted);
			if(bDaclPresent == TRUE)
			{
				if(pDacl == NULL)
				{
					wprintf(L"A NULL discretionary access control list (DACL) found \nA NULL DACL implicitly allows all access to an object.\n");
				}
				else
				{
					wprintf(L"A discretionary access control list (DACL) was found with Length = %d\n",pDacl->AclSize);
					wprintf(L"Number of Access Control Elements (ACE's): %d\n",pDacl->AceCount);
					ReadAccessForLocalHost(pDacl);
				}
			}
			else
			{
				wprintf(L"No discretionary access control list (DACL) found \n");
			}

			//LPTSTR StringSecurityDescriptor;
//			ULONG StringSecurityDescriptorLen;

			//ConvertSecurityDescriptorToStringSecurityDescriptor(pSecurityDescriptor,SDDL_REVISION_1,DACL_SECURITY_INFORMATION,
			//	&StringSecurityDescriptor,&StringSecurityDescriptorLen);


			//wprintf(L"%s\n",(const wchar_t*)StringSecurityDescriptor);
			//parse info see http://msdn.microsoft.com/en-us/library/aa379570%28v=vs.85%29.aspx

			//LocalFree(StringSecurityDescriptor);

		}
		free(pSecurityDescriptor);
	}
	else
	{
		wprintf(L"RegGetKeySecurity failed",err);
	}

	if(ERROR_SUCCESS != (err = RegCloseKey(hRegKey)))
	{
		wprintf(L"RegCloseKey failed",err);
	}

	return retval;
}

BOOL SCardSvrAsLS()
{
	BOOL			retval = FALSE;
	SC_HANDLE		schManager=0;
	SC_HANDLE		schService=0;
	DWORD			dwBytesNeeded=0;

	schManager = OpenSCManager( NULL, SERVICES_ACTIVE_DATABASE,GENERIC_READ );
	if(schManager == NULL)
	{
		return FALSE;
	}

	schService = OpenServiceW(schManager,L"SCardSvr",GENERIC_READ);
	if(schService == NULL)
	{
		CloseServiceHandle(schManager);
		return FALSE;
	}

	if (QueryServiceConfig(schService,NULL,0,&dwBytesNeeded) == 0)
	{
		if ( GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			LPQUERY_SERVICE_CONFIGW lpServiceConfig = (LPQUERY_SERVICE_CONFIGW) malloc(dwBytesNeeded);
			if(lpServiceConfig != 0)
			{
				if (QueryServiceConfigW(schService, lpServiceConfig,dwBytesNeeded,&dwBytesNeeded) )
				{
					LPWSTR lpwprintfonAs = lpServiceConfig->lpServiceStartName;
					wprintf(lpServiceConfig->lpServiceStartName);
					wprintf(L"\n");
					if(wcslen(lpServiceConfig->lpServiceStartName) > wcslen(L"LocalService"))
					{
						lpwprintfonAs+=wcslen(lpServiceConfig->lpServiceStartName)- wcslen(L"LocalService");
						if (wcscmp(lpwprintfonAs,L"LocalService") == 0)
						{
							wprintf(L"SCardSvr is running as LocalService");
							retval = TRUE;
						}
					}
				}
				free(lpServiceConfig);
			}
		}
	}
	CloseServiceHandle(schService);
	CloseServiceHandle(schManager);
	return retval;
}
