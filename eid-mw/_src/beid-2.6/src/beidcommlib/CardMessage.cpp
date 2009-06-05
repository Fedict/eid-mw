// CardMessage.cpp: implementation of the CCardMessage class.
//
//////////////////////////////////////////////////////////////////////

#include "CardMessage.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NAMESPACE_BEGIN(EIDCommLIB)

CCardMessage::CCardMessage()
{
    m_oId = QUuid::createUuid();
}

CCardMessage::~CCardMessage()
{
    ItVecKeyValues it;
    for (it = m_oVecKeyValues.begin(); it != m_oVecKeyValues.end(); ++it)
    {
        QByteArray *pBytesBlock = (*it).second;
        if(pBytesBlock)
        {
            delete pBytesBlock;
        }
    }
    m_oVecKeyValues.clear();
}

string CCardMessage::GetMessageId()
{
    return m_oId.toString().ascii();
}


void CCardMessage::Set(const std::string & strKey, const std::string & strValue)
{
    Set(strKey, (byte *)strValue.c_str(), strValue.length());
}

void CCardMessage::Set(const std::string & strKey, long lValue)
{
    ostringstream streamTemp;
    streamTemp << lValue;
    Set(strKey, streamTemp.str());
}

void CCardMessage::Set(const std::string & strKey, const byte *pValue, unsigned int iSize)
{
    QByteArray *pBytesBlock = new QByteArray();
    pBytesBlock->duplicate((const char *)pValue, iSize);
    m_oVecKeyValues[strKey] = pBytesBlock;
}

bool CCardMessage::Get(const std::string & strKey, std::string & strValue)
{
    bool bRet = false;
    strValue.clear();
    ItVecKeyValues it = m_oVecKeyValues.find(strKey);
    if(it != m_oVecKeyValues.end())
    {
        QByteArray *pBytesBlock = m_oVecKeyValues[strKey];
        if(pBytesBlock && pBytesBlock->size() > 0)
        {
            unsigned int iLen = pBytesBlock->size();
            char *pBuffer = new char[iLen + 1];
            memset(pBuffer, 0, iLen + 1);
            QDataStream oData(*pBytesBlock, IO_ReadOnly);
            oData.readRawBytes(pBuffer,  iLen);
            strValue = pBuffer;
            if(pBuffer)
            {
                delete [] pBuffer;
            }
            bRet = true;
        }
    }
    return bRet;
}

bool CCardMessage::Get(const std::string & strKey, char *pszValue)
{
    bool bRet = false;
    if(pszValue)
    {
        std::string strValue;
        if(Get(strKey, strValue))
        {
            strcpy(pszValue, strValue.c_str());
            bRet = true;
        }
    }
    return bRet;
}

bool CCardMessage::Get(const std::string & strKey, long & lValue)
{
    bool bRet = false;
    lValue = 0;
    string strValue;
    Get(strKey, strValue);
    if(strValue.size() > 0)
    {
        istringstream streamTemp(strValue);
        streamTemp >> lValue;
        bRet = true;
    }
    return bRet;
}

bool CCardMessage::Get(const std::string & strKey, long *plValue)
{
    bool bRet = false;
    if(plValue)
    {
        *plValue = 0;
        string strValue;
        Get(strKey, strValue);
        if(strValue.size() > 0)
        {
            long lValue = 0;
            istringstream streamTemp(strValue);
            streamTemp >> lValue;
            *plValue = lValue;
            bRet = true;
        }
    }
    return bRet;
}

unsigned int CCardMessage::Get(const std::string & strKey, byte *pValue, unsigned int iSize)
{
    unsigned int iRet = 0;
    ItVecKeyValues it = m_oVecKeyValues.find(strKey);
    if(it != m_oVecKeyValues.end())
    {
        QByteArray *pBytesBlock = m_oVecKeyValues[strKey];
        if(pBytesBlock && pBytesBlock->size() > 0)
        {
            unsigned int iBlockSize = pBytesBlock->size();
            if(NULL == pValue)
            {
                return iBlockSize;
            }
            if(iBlockSize > 0 && iBlockSize <= iSize)
            {
                memcpy(pValue, pBytesBlock->data(), iBlockSize);
                iRet = iBlockSize;
            }
        }
    }
    return iRet;
}

unsigned int CCardMessage::GetSerializeSize()
{
    unsigned int iRet = 0;
    ItVecKeyValues it;
    QByteArray sink;
    QDataStream oStream(sink, IO_WriteOnly);
    oStream << m_oId;
    iRet += sink.size();

    for (it = m_oVecKeyValues.begin(); it != m_oVecKeyValues.end(); ++it)
    {
        QByteArray *pBytesBlock = (*it).second;
        string strKey = (*it).first;
        if(pBytesBlock->size() > 0)
        {
            iRet += sizeof(Q_UINT32);
            iRet += strKey.size();
            iRet += sizeof(Q_UINT32);
            iRet += pBytesBlock->size();
        }
    }
    return iRet;
}

unsigned int CCardMessage::Serialize(byte *pValue, unsigned int iSize)
{
    unsigned int iRet = 0;

    if(NULL == pValue)
    {
        return GetSerializeSize();
    }
    if (iSize < 16 || iSize > BEID_MAX_MESSAGE_SIZE)
    {
        return iRet;
    }

    ItVecKeyValues it;
    QByteArray sink;
    QDataStream stream(sink, IO_WriteOnly);

    // Add Message ID
    stream << m_oId;

    for (it = m_oVecKeyValues.begin(); it != m_oVecKeyValues.end(); ++it)
    {
        QByteArray *pBytesBlock = (*it).second;
        string strKey = (*it).first;
        if(pBytesBlock->size() > 0)
        {
            stream << (Q_UINT32) strKey.size();
            stream.writeRawBytes(strKey.c_str(), strKey.size());
            stream << (Q_UINT32) pBytesBlock->size();
            stream.writeRawBytes(pBytesBlock->data(), pBytesBlock->size());
        }
    }
    iRet = sink.size();
    if(iSize < iRet)
    {
        iRet = 0;
    }
    else
    {
        memcpy(pValue, sink.data(), iRet);
    }
    return iRet;
}

bool CCardMessage::Unserialize(const byte *pValue, unsigned int iSize)
{
    if (iSize < 16 || iSize > BEID_MAX_MESSAGE_SIZE)
    {
        return false;
    }
    ItVecKeyValues it;
    QByteArray sink;
    sink.setRawData((const char *)pValue, iSize);
    QDataStream stream(sink, IO_ReadOnly);
    bool bContinue = false;

    // Get Message ID
    stream >> m_oId;
    if(!m_oId.isNull() && !stream.atEnd())
    {
        do
        {
            bContinue = false;
            unsigned int iKey = 0;
            unsigned int iValue = 0;
            char *pBytesKey = NULL;
            char *pBytesValue = NULL;

            stream >> (Q_UINT32&)iKey;
            if(iKey > 0 && iKey < BEID_MAX_MESSAGE_SIZE)
            {
                pBytesKey = new char [iKey + 1];
                memset(pBytesKey, 0, iKey + 1);
                stream.readRawBytes(pBytesKey, iKey);
                stream >> (Q_UINT32&)iValue;
                if(iValue > 0 && iValue < BEID_MAX_MESSAGE_SIZE)
                {
                    pBytesValue = new char [iValue];
                    memset(pBytesValue, 0, iValue);
                    stream.readRawBytes(pBytesValue, iValue);
                    string strKey;
                    strKey = pBytesKey;
                    Set(strKey, (const byte *)pBytesValue, iValue);
                    delete [] pBytesValue;
                    bContinue = true;
                }
                delete [] pBytesKey;
            }
        }while(bContinue && !stream.atEnd());
    }
    sink.resetRawData((const char *)pValue, iSize);
    return true;
}


void CCardMessage::Clear()
{
    ItVecKeyValues it;
    for (it = m_oVecKeyValues.begin(); it != m_oVecKeyValues.end(); ++it)
    {
        QByteArray *pBytesBlock = (*it).second;
        if(pBytesBlock)
        {
            delete pBytesBlock;
        }
    }
    m_oVecKeyValues.clear();
}

NAMESPACE_END
