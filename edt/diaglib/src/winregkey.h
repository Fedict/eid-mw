#include <windows.h>
#include <iostream>
#include <string>
#include <exception>

using namespace std;

class WinRegKey
{
public:
	WinRegKey(HKEY,const wstring);
	~WinRegKey() throw();
	const wstring operator()(const wstring) const;
	const HKEY rootkey(void) const;
	const wstring subkey(void) const;
private:
	HKEY	m_rootkey;
	wstring	m_subkey;
	HKEY	m_key;

};

class WinRegKeyException : public exception
{
public:
	WinRegKeyException(LONG, HKEY, wstring, wstring);
	virtual ~WinRegKeyException() throw();
	const LONG errorcode(void) const;
	const wstring key(void) const;
private:			
	LONG	m_errorcode;
	wstring	m_key;
};

