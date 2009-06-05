#include "StdAfx.h"
#include "processapi.h"

//--------------------------------------------------------------------------------------------
// This function returns a list id (list id) that can be used w/ Walking functions
// in order to navigate through the process list
// This function decides what to use from the helper apis
DWORD CProcessApi::ModulesGetList(DWORD pid)
{
  tModulesData *md = new tModulesData;
  if (!md)
    return (LastError = paeNoMem, NULL);

  // create the list
  md->ml = new tModulesList;
  if (!md->ml)
  {
    delete md;
    return (LastError = paeNoMem, NULL);
  }

  // decide what to use
  if (m_bPsApi)
    LastError = ModulesPopulatePsApi(pid, md);
  else if (m_bToolHelp)
    LastError = ModulesPopulateToolHelp(pid, md);

  return (DWORD) md;
}

//--------------------------------------------------------------------------------------------
// Populates the modules of a process using ToolHelp api
DWORD CProcessApi::ModulesPopulateToolHelp(DWORD pid, tModulesData *md)
{
  MODULEENTRY32 me32        = {sizeof(MODULEENTRY32), 0}; 
  tModuleInfo   mi = {0};

  // Take a snapshot of all modules in the specified process. 
  HANDLE hModuleSnap = tlhlp_CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid); 

  if (hModuleSnap == INVALID_HANDLE_VALUE) 
    return paeNoSnap; 
 
  // Start walking
  BOOL bMore = tlhlp_Module32First(hModuleSnap, &me32);
  do
  {
    _tcscpy(mi.FileName, me32.szExePath);
    mi.ImageBase = me32.modBaseAddr;
    mi.ImageSize = me32.modBaseSize;

    // save item
    md->ml->push_back(mi);

    // search next
    bMore = tlhlp_Module32Next(hModuleSnap, &me32);
  } while (bMore);

  CloseHandle (hModuleSnap); 
  md->Pos = 0;
  return paeSuccess;
} 


//--------------------------------------------------------------------------------------------
// Populates the modules of a process using PsApi api
DWORD CProcessApi::ModulesPopulatePsApi(DWORD pid, tModulesData *md)
{
  DWORD nModules, nCount = 4096;
  HANDLE hProcess;

  // allocate memory for modules
  HMODULE *modules   = new HMODULE[nCount];

  // open process for querying only
  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
  if (!hProcess)
    return paeNoSnap;
      
  // now try to enum all modules
  if (!psapi_EnumProcessModules(hProcess, modules, nCount * sizeof(DWORD), &nModules))    
  {
    CloseHandle(hProcess);
    return paeNoSnap;
  }

  // because nModules returned from EnumProcessModules() is in bytes, I divid by 4 to return n DWORDs
  nModules /= 4;

  tModuleInfo mi = {0};
  MODULEINFO psapiMi = {0};

  for (DWORD i=0; i < nModules; i++)
  {
    // get module name
    psapi_GetModuleFileNameEx(hProcess, modules[i], mi.FileName, sizeof(mi.FileName));    

    // get module information
    psapi_GetModuleInformation(hProcess, modules[i], &psapiMi, sizeof(MODULEINFO));

    // get relevant data
    mi.ImageBase = psapiMi.lpBaseOfDll;
    mi.ImageSize = psapiMi.SizeOfImage;

    // save item
    md->ml->push_back(mi);
  }

  md->Pos = 0;
  CloseHandle(hProcess);
  delete modules;
  return paeSuccess;
}

//--------------------------------------------------------------------------------------------
// It frees a modules list by its ID
void CProcessApi::ModulesFreeList(DWORD lid)
{
  tModulesData *md = reinterpret_cast<tModulesData *>(lid);
  delete md->ml;
  delete md;
}

//--------------------------------------------------------------------------------------------
// This function allows you to retrieve information about a certain module in a process
// You can either use this function using the Pos parameter controlled by the ModulesCount()
// Or you can keep calling it till it returns false
bool CProcessApi::ModulesWalk(DWORD lid, tModuleInfo *mi, DWORD Pos)
{
  tModulesData *md = reinterpret_cast<tModulesData *>(lid);

  // auto position ?
  if (Pos == -1)
    Pos = md->Pos;

  // out of bounds?
  if (Pos < 0 || Pos > md->ml->size())
    return (LastError = paeOutOfBounds, false);

  // end reached ?
  else if (Pos == md->ml->size())
    return (LastError = paeNoMore, false);
    
  // copy information to user buffer
  *mi = md->ml->at(Pos);

  // advance position to next item
  md->Pos++;

  return (LastError = paeSuccess, true);
}


//--------------------------------------------------------------------------------------------
// This function allows you to retrieve information about a certain process in the list
// You can either use this function using the Pos parameter controlled by the ProcessesCount()
// Or you can keep calling it till it returns false
bool CProcessApi::ProcessesWalk(DWORD lid, tProcessInfo *pi, DWORD Pos)
{
  tProcessesData *pd = reinterpret_cast<tProcessesData *>(lid);

  // auto position ?
  if (Pos == -1)
    Pos = pd->Pos;

  // out of bounds?
  if (Pos < 0 || Pos > pd->pl->size())
    return (LastError = paeOutOfBounds, false);
  // end reached ?
  else if (Pos == pd->pl->size())
    return (LastError = paeNoMore, false);
    
  // copy information to user buffer
  *pi = pd->pl->at(Pos);

  // advance position to next item
  pd->Pos++;
  return (LastError = paeSuccess, true);
}


//--------------------------------------------------------------------------------------------
// This function returns a list id (list id) that can be used w/ Walking functions
// in order to navigate through the process list
// This function decides what to use from the helper apis
DWORD CProcessApi::ProcessesGetList()
{
  tProcessesData *pd = new tProcessesData;
  if (!pd)
    return (LastError = paeNoMem, NULL);

  // create the list
  pd->pl = new tProcessesList;
  if (!pd->pl)
  {
    delete pd;
    return (LastError = paeNoMem, NULL);
  }

  // decide what to use
  if (m_bPsApi)
    LastError = ProcessesPopulatePsApi(pd);
  else if (m_bToolHelp)
    LastError = ProcessesPopulateToolHelp(pd);

  return (DWORD) pd;
}

//--------------------------------------------------------------------------------------------
// It frees a process list by its ID
void CProcessApi::ProcessesFreeList(DWORD lid)
{
  tProcessesData *pd = reinterpret_cast<tProcessesData *>(lid);
  delete pd->pl;
  delete pd;
}

//--------------------------------------------------------------------------------------------
// Dynamically loads the PsApi functions
bool CProcessApi::Load_PsApi()
{
  m_hPsApi = LoadLibrary(_T("psapi.dll"));
  if (!m_hPsApi)
    return FALSE; 

  #ifdef _UNICODE
    #define Modifier "W"
  #else
    #define Modifier "A"
  #endif
   ;
  PVOID p;
  
  // custom macro to allow me to load functions dynamically
  #define DynamicGetProcAddress(modname, Mod) \
    p = GetProcAddress(m_hPsApi, #modname Mod); \
    if (!p) { FreeLibrary(m_hPsApi); return (LastError = paeNoEntryPoint, false); } \
    psapi_##modname = (t_psapi_##modname)p;

  DynamicGetProcAddress(GetModuleFileNameEx, Modifier);
  DynamicGetProcAddress(EnumProcessModules, _T(""));
  DynamicGetProcAddress(EnumProcesses, _T(""));
  DynamicGetProcAddress(GetModuleInformation, _T(""));

  #undef DynamicGetProcAddress
  #undef Modifier

  m_bPsApi = true;
  return true;
}


//--------------------------------------------------------------------------------------------
// Dynamically loads the ToolHelp functions
bool CProcessApi::Load_TlHlp()
{
  m_hTlHlp = LoadLibrary(_T("kernel32.dll"));
  if (!m_hTlHlp)
    return false; 

  #ifdef _UNICODE
    #define Modifier "W"
  #else
    #define Modifier "A"
  #endif
   ;
  PVOID p;
  
  // custom macro to allow me to load functions dynamically
  #define DynamicGetProcAddress(modname, Mod) \
    p = GetProcAddress(m_hTlHlp, #modname Mod); \
    if (!p) { FreeLibrary(m_hTlHlp); return (LastError = paeNoEntryPoint, false);  } \
    tlhlp_##modname = (t_tlhlp_##modname)p;

  DynamicGetProcAddress(CreateToolhelp32Snapshot, _T(""));
  DynamicGetProcAddress(Process32First, _T(""));
  DynamicGetProcAddress(Process32Next, _T(""));
  DynamicGetProcAddress(Module32First, _T(""));
  DynamicGetProcAddress(Module32Next, _T(""));

  #undef DynamicGetProcAddress
  #undef Modifier

  m_bToolHelp = true;
  return true;
}

//--------------------------------------------------------------------------------------------
// The constructor only initializes some internal variables
CProcessApi::CProcessApi()
{
  m_hPsApi = NULL;
  m_bToolHelp = m_bPsApi = false;
  LastError = paeSuccess;
}

//--------------------------------------------------------------------------------------------
// This is the destructor. It frees any process helper api that has been used
CProcessApi::~CProcessApi()
{
  if (m_bPsApi)
    FreeLibrary(m_hPsApi);
  if (m_bToolHelp)
    FreeLibrary(m_hTlHlp);
}

//--------------------------------------------------------------------------------------------
// Used to initialize the CProcessApi class
bool CProcessApi::Init(bool bPsApiFirst)
{
  bool loaded;
  
  if (bPsApiFirst)
  {
    loaded = Load_PsApi();
    if (!loaded)
      loaded = Load_TlHlp();
  }
  else
  {
    loaded = Load_TlHlp();
    if (!loaded)
      loaded = Load_PsApi();
  }
  return (loaded ? (LastError = paeSuccess, true) : (LastError = paeNoApi, false));
}


//--------------------------------------------------------------------------------------------
// Populates a tProcessesList with the help of ToolHelp API
// Returns an error code paeXXXX
DWORD CProcessApi::ProcessesPopulateToolHelp(tProcessesData *pd)
{
  // create a process snapshot
  HANDLE hSnap = tlhlp_CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnap == INVALID_HANDLE_VALUE)
    return paeNoSnap;

  BOOL bMore;
  tProcessInfo pi = {0};

  PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32), 0};

  // clear the list
  pd->pl->clear();

  // initialize position
  pd->Pos = 0;

  bMore = tlhlp_Process32First(hSnap, &pe32);
  while (bMore)
  {
    // convert from PROCESSENTRY32 to my unified tProcessInfo struct
    pi.pid = pe32.th32ProcessID;
    _tcscpy(pi.FileName, pe32.szExeFile);

    pd->pl->push_back(pi);
    bMore = tlhlp_Process32Next(hSnap, &pe32);
  }

  CloseHandle(hSnap);
  return paeSuccess;
}


//--------------------------------------------------------------------------------------------
// Populates the list using PsApi functions
DWORD CProcessApi::ProcessesPopulatePsApi(tProcessesData *pd)
{
  DWORD nProcess, // number of processes returned
        nCount(4096); // maximum number of processes (defined by me)

  // Dynamic array for storing returned processes IDs
  DWORD *processes = new DWORD[nCount];

  // enum all processes
  if (!psapi_EnumProcesses(processes, nCount * sizeof(DWORD), &nProcess))
  {
    delete processes;
    return paeNoSnap;
  }

  // convert fron bytes count to items count
  nProcess /= 4;

  tProcessInfo pi = {0};

  // walk in process list
  for (DWORD i=0; 
       (i < nProcess);
       i++)
  {
    HANDLE hProcess;

    // open process for querying only
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i]);
    
    if (!hProcess)
      continue;

    // get the process's image name by getting first module
    DWORD nmod;
    HMODULE mod1;
    
    if (!psapi_EnumProcessModules(hProcess, &mod1, sizeof(mod1), &nmod))
      _tcscpy(pi.FileName, _T("-"));
    else
      psapi_GetModuleFileNameEx(hProcess, mod1, pi.FileName, sizeof(pi.FileName));

    // get the process ID
    pi.pid = processes[i];

    // store in the list
    pd->pl->push_back(pi);
     
    CloseHandle(hProcess);
  }

  // reposition list to 0
  pd->Pos = 0;
  delete processes;
  return paeSuccess;
}

//--------------------------------------------------------------------------------------------
// Returns the count in the processes list
DWORD CProcessApi::ProcessesCount(DWORD lid) const
{
  return (reinterpret_cast<tProcessesData *>(lid))->pl->size();
}

//--------------------------------------------------------------------------------------------
// Returns the count in the modules list
DWORD CProcessApi::ModulesCount(DWORD lid) const
{
  return (reinterpret_cast<tModulesData *>(lid))->ml->size();
}
