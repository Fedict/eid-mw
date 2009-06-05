#if !defined(AFX_PROCESSAPI_H__1A1965AD_3827_47AB_ADCA_1CF84A266F16__INCLUDED_)
#define AFX_PROCESSAPI_H__1A1965AD_3827_47AB_ADCA_1CF84A266F16__INCLUDED_

#include "psapi.h"
#include <tlhelp32.h>
#include <tchar.h>
#include <vector>

//--------------------------------------------------------------------------------------------
// CProcessApi class
//

class CProcessApi
{
public:
  typedef struct tProcessInfo
  {
    DWORD pid;
    TCHAR FileName[MAX_PATH];
  };

  typedef struct tModuleInfo
  {
    LPVOID ImageBase;
    DWORD  ImageSize;
    TCHAR  FileName[MAX_PATH];
  };

private:
    typedef std::vector<tProcessInfo> tProcessesList;
    typedef std::vector<tModuleInfo> tModulesList;

  typedef struct tProcessesData
  {
    DWORD Pos;
    tProcessesList *pl;
  };

  typedef struct tModulesData
  {
    DWORD Pos;
    tModulesList *ml;
  };

  // PSAPI.DLL functions prototype
  typedef BOOL (WINAPI *t_psapi_EnumProcesses)(
    DWORD *lpidProcess,  // array of process identifiers
    DWORD cb,            // size of array
    DWORD *cbNeeded      // number of bytes returned
  );

  typedef BOOL (WINAPI *t_psapi_EnumProcessModules)(
    HANDLE hProcess,      // handle to process
    HMODULE *lphModule,   // array of module handles
    DWORD cb,             // size of array
    LPDWORD lpcbNeeded    // number of bytes required
  );

  typedef DWORD (WINAPI *t_psapi_GetModuleFileNameEx)(
    HANDLE hProcess,    // handle to process
    HMODULE hModule,    // handle to module
    LPTSTR lpFilename,  // path buffer
    DWORD nSize         // maximum characters to retrieve
  );

  typedef BOOL (WINAPI *t_psapi_GetModuleInformation)(
    HANDLE hProcess,
    HMODULE hModule,
    LPMODULEINFO lpmodinfo,
    DWORD cb
  );
  
  // functions instances
  t_psapi_GetModuleFileNameEx       psapi_GetModuleFileNameEx;
  t_psapi_EnumProcessModules        psapi_EnumProcessModules;
  t_psapi_EnumProcesses             psapi_EnumProcesses;
  t_psapi_GetModuleInformation      psapi_GetModuleInformation;

  // TOOLHELP functions prototype
  typedef HANDLE (WINAPI *t_tlhlp_CreateToolhelp32Snapshot)(
    DWORD dwFlags,
    DWORD th32ProcessID
  );

  typedef BOOL (WINAPI *t_tlhlp_Process32First)(
    HANDLE hSnapshot,
    LPPROCESSENTRY32 lppe
  );
  
  typedef BOOL (WINAPI *t_tlhlp_Process32Next)(
    HANDLE hSnapshot,
    LPPROCESSENTRY32 lppe
  );

  typedef BOOL (WINAPI *t_tlhlp_Module32First)(
    HANDLE hSnapshot,
    LPMODULEENTRY32 lpme
  );

  typedef BOOL (WINAPI *t_tlhlp_Module32Next)(
    HANDLE hSnapshot,
    LPMODULEENTRY32 lpme
  );

  // functions instances
  t_tlhlp_CreateToolhelp32Snapshot tlhlp_CreateToolhelp32Snapshot;
  t_tlhlp_Process32First           tlhlp_Process32First;
  t_tlhlp_Process32Next            tlhlp_Process32Next; 
  t_tlhlp_Module32First            tlhlp_Module32First;
  t_tlhlp_Module32Next             tlhlp_Module32Next;

  // Private member variables
	HMODULE   m_hPsApi;
	HMODULE   m_hTlHlp;
  bool      m_bPsApi;
  bool      m_bToolHelp;

  bool Load_TlHlp();
    bool Load_PsApi();

  DWORD ProcessesPopulatePsApi(tProcessesData *pd);
  DWORD ProcessesPopulateToolHelp(tProcessesData *pd);

  DWORD ModulesPopulatePsApi(DWORD pid, tModulesData *md);
  DWORD ModulesPopulateToolHelp(DWORD pid, tModulesData *md);

public:
  // CProcessApi error enum
  enum
  {
    paeSuccess = 0,                     // No error
    paeNoApi,                           // No process API helper dll found
    paeNoEntryPoint,                    // One needed entrypoint not found in helper dll
    paeNoMem,                           // Not enough memory
    paeNoSnap,                          // Could not get a snapshot
    paeNoMore,                          // List contains no more items
    paeOutOfBounds,                     // Tried to access list w/ an invalid index
    paeYYY
  };

  DWORD LastError; // Holds the last error

	CProcessApi();
  ~CProcessApi();

	bool Init(bool bPsApiFirst = true);

  DWORD ProcessesGetList();
  bool  ProcessesWalk(DWORD lid, tProcessInfo *pi, DWORD Pos = -1);
  DWORD ProcessesCount(DWORD lid) const;
  void  ProcessesFreeList(DWORD lid);

  DWORD ModulesGetList(DWORD ProcessID);
  bool  ModulesWalk(DWORD lid, tModuleInfo *mi, DWORD Pos = -1);
  DWORD ModulesCount(DWORD lid) const;
  void  ModulesFreeList(DWORD lid);
};

#endif // !defined(AFX_PROCESSAPI_H__1A1965AD_3827_47AB_ADCA_1CF84A266F16__INCLUDED_)