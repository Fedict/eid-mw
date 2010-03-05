/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */

#include <algorithm>
#include "repository.h"
#include "util.h"
#include "MiddleWareFilesIntegrityRule.h"

MiddleWareFilesIntegrityRule::MiddleWareFilesIntegrityRule()
:MetaRule(L"middleware.files.integrity",L"Are the Middleware files all present and unaltered as compared to the ones we installed?"),m_sys_files(),m_app_files()

// list expected middleware md5 sums (only 3.5.3 for now)
{
	m_sys_files.insert(make_pair(L"beid35applayer_dll",					L"ad95f692c90673a3f893d83f43852c2c"));
	m_sys_files.insert(make_pair(L"beid35cardlayer_dll",				L"c7df01c6b62400d31c16b6fb91675712"));
	m_sys_files.insert(make_pair(L"beid35common_dll",					L"7953a843ef109416dbb281e9bd323b1b"));
	m_sys_files.insert(make_pair(L"beid35dlgswin32_dll",				L"1c20dc4c27e828282fafc0aac2ec2609"));
	m_sys_files.insert(make_pair(L"libeay32_0_9_8g_dll",				L"e03b64b9be9cb341bdc16b16954ecaae"));
	m_sys_files.insert(make_pair(L"ssleay32_0_9_8g_dll",				L"099b6b991321ae4a97ff11d879722dda"));
	m_sys_files.insert(make_pair(L"xerces-c_2_8_dll",					L"2736fa1553d38b07b1ffb3a9d18d4274"));
	m_sys_files.insert(make_pair(L"beidcsp_dll",						L"d8dc44b28e4def1923d262a7d3ed7897"));
	m_sys_files.insert(make_pair(L"beidcsplib_dll",						L"05cb389401d1cc3bfe04bc97d9853a03"));
	m_sys_files.insert(make_pair(L"beidpkcs11_dll",						L"4a78b1700e16736f255a57732013fec1"));
	m_sys_files.insert(make_pair(L"belgium_identity_card_pkcs11_dll",	L"4a78b1700e16736f255a57732013fec1"));

	m_app_files.insert(make_pair(L"beid35libcpp_dll",					L"b736c0074dc06496948c917f074f4bcb"));
	m_app_files.insert(make_pair(L"beid35gui_exe",						L"c78437693b5aafacf42d8ce28f139b91"));
	m_app_files.insert(make_pair(L"eidmw_en_qm",						L"a39db5a36aacdf68df256d7314374602"));
	m_app_files.insert(make_pair(L"eidmw_nl_qm",						L"7e7d17e274ecfbcbb62fb5f151a4d33c"));
	m_app_files.insert(make_pair(L"eidmw_fr_qm",						L"f9e4ceb126d822e40dc2fd6f2ebfeb33"));
	m_app_files.insert(make_pair(L"eidmw_de_qm",						L"fd1773cc5dae3410b948a2b54f5120c1"));
	m_app_files.insert(make_pair(L"qtcore4_dll",						L"ecd1e6a6c7aa2a8adf17d5fed5170dc4"));
	m_app_files.insert(make_pair(L"qtgui4_dll",							L"6aa72e28888ef013eea042471b7db7e0"));
	m_app_files.insert(make_pair(L"imageformats\\qjpeg4_dll",			L"b49c7183d7352ab7dbd51fae7f593b02"));
}

MiddleWareFilesIntegrityRule::~MiddleWareFilesIntegrityRule() throw()
{
	m_app_files.clear();	
	m_sys_files.clear();
}

void MiddleWareFilesIntegrityRule::testCategory(Repository evidence, std::wstring category, FileMD5Map data, FileNameSet& missing, FileNameSet& corrupt) const
{
	for(FileMD5Map::const_iterator file=data.begin();file!=data.end();file++)
	{
		if(!evidence.exists(L"middleware_files." + category + L"." + file->first + L".md5"))
			missing.insert(file->first);
		else
		{
			std::wstring md5_found=evidence.value(L"middleware_files." + category + L"." + file->first + L".md5");
			if(md5_found.compare(file->second)!=0)
				corrupt.insert(file->first);
		}
	}
}

MetaRuleVerdict MiddleWareFilesIntegrityRule::verdict(Repository evidence) const
{
	FileNameSet missing,corrupt;
	testCategory(evidence,L"system",m_sys_files,missing,corrupt);
	testCategory(evidence,L"application",m_app_files,missing,corrupt);

	if(missing.empty() && corrupt.empty())	// if no missing or corrupted files, judge "not guilty"
		return MetaRuleVerdict(false);
	else														// if any are some, expose them.
	{
		std::wstringstream curetext;
		std::wstringstream diagtext;
						   diagtext << L"There are problems with the eID Middleware installation:\n";

		if(!corrupt.empty())
		{
			diagtext << L"The following file" << (corrupt.size()==1?L" is":L"s are") << L" corrupted: [" << join<std::wstring,FileNameSet>(corrupt,L",") << L"]\n";
			curetext << L"- Make sure your system is free of Malware before proceeding!\n";
		}

		if(!missing.empty())
			diagtext << L"The following file" << (missing.size()==1?L" is":L"s are") << L" missing: [" + join<std::wstring,FileNameSet>(missing,L",") << L"]\n";
			
		curetext << L"- Re-install eID Middleware from http://eid.belgium.be/\n";

		return MetaRuleVerdict(true,diagtext.str(),curetext.str());
	}
}