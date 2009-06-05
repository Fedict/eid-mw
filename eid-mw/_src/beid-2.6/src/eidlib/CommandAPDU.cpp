// CommandAPDU.cpp: implementation of the CCommandAPDU class.
//
//////////////////////////////////////////////////////////////////////

#include "CommandAPDU.h"
#include <opensc/opensc.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommandAPDU::CCommandAPDU()
{
    m_CLA = 0x00;
    m_INS = 0x00;
    m_P1 = 0x00;
    m_P2 = 0x00;
    m_Le = 0x00;
    m_Case = 0;
}

CCommandAPDU::~CCommandAPDU()
{

}

/**
* Constructs an empty ISO 7816-4 APDU command as a
* <code>CCommandAPDU</code> object.
* This APDU can be build after creation using SetClassByte,
* SetInstructionByte... This <code>CCommandAPDU</code>
* must then be validated using the <code>Flush()</code> method.
*
* @param   ulSize
*          Size of the buffer
**/
CCommandAPDU::CCommandAPDU(unsigned long ulSize) : CAPDU(ulSize)
{

}


/**
 * Constructs an ISO 7816-4 case 1 APDU command
 *
 * @param   cla
 *          CLASS byte
 *
 * @param   ins
 *          INSTRUCTION byte
 *
 * @param   p1
 *          PARAMETER1 byte
 *
 * @param   p2
 *          PARAMETER2 byte
 **/
CCommandAPDU::CCommandAPDU(unsigned char cla, unsigned char ins, unsigned char p1, unsigned char p2)
{
        m_Case = SC_APDU_CASE_1;
        SetLength(0);

        // Class byte
        m_CLA = cla; 
        Append(cla);

        // Instruction byte
        m_INS = ins;
        Append(ins);

        // Parameters bytes
        m_P1 = p1;
        Append(p1);
        m_P2 = p2; 
        Append(p2);
}

/**
 * Constructs an ISO 7816-4 case 2 APDU command
 *
 * @param   cla
 *          CLASS byte
 *
 * @param   ins
 *          INSTRUCTION byte
 *
 * @param   p1
 *          PARAMETER1 byte
 *
 * @param   p2
 *          PARAMETER2 byte
 *
 * @param   le
 *          Length of expected data
 **/
CCommandAPDU::CCommandAPDU(unsigned char cla, unsigned char ins, unsigned char p1, unsigned char p2, unsigned char le)
{
        m_Case = SC_APDU_CASE_2_SHORT;
        SetLength(0);

        // Class byte
        m_CLA = cla; 
        Append(cla);

        // Instruction byte
        m_INS = ins;
        Append(ins);

        // Parameters bytes
        m_P1 = p1;
        Append(p1);
        m_P2 = p2; 
        Append(p2);

        // Length expected
        m_Le = le;
        Append(le);
}


/**
 * Constructs an ISO 7816-4 case 3 APDU command
 *
 * @param   cla
 *          CLASS byte
 *
 * @param   ins
 *          INSTRUCTION byte
 *
 * @param   p1
 *          PARAMETER1 byte
 *
 * @param   p2
 *          PARAMETER2 byte
 *
 * @param   pData
 *          Input data field
 *
 * @param   ulDataLen
 *          Length of input data field
 **/
CCommandAPDU::CCommandAPDU(unsigned char cla, unsigned char ins, unsigned char p1, unsigned char p2, unsigned char *pData, unsigned long ulDataLen)
{
        m_Case = SC_APDU_CASE_3_SHORT;
        SetLength(0);

        // Class byte
        m_CLA = cla; 
        Append(cla);

        // Instruction byte
        m_INS = ins;
        Append(ins);

        // Parameters bytes
        m_P1 = p1;
        Append(p1);
        m_P2 = p2; 
        Append(p2);

        m_Data.Append(pData, ulDataLen);

        // Input data length byte
        Append((int)ulDataLen);
        
        // Input data bytes
        Append(pData, ulDataLen);
}

/**
* Constructs an ISO 7816-4 case 4 APDU command
*
* @param   cla
*          CLASS byte
*
* @param   ins
*          INSTRUCTION byte
*
* @param   p1
*          PARAMETER1 byte
*
* @param   p2
*          PARAMETER2 byte
*
* @param   data
*          Input data field
*
* @param   pData
*          Input data field
*
* @param   ulDataLen
*          Length of input data field
*
* @param   le
*          Length of expected data
**/
CCommandAPDU::CCommandAPDU(unsigned char cla, unsigned char ins, unsigned char p1, unsigned char p2, unsigned char *pData, unsigned long ulDataLen, unsigned char le)
{
        m_Case = SC_APDU_CASE_4_SHORT;
        SetLength(0);

        // Class byte
        m_CLA = cla; 
        Append(cla);

        // Instruction byte
        m_INS = ins;
        Append(ins);

        // Parameters bytes
        m_P1 = p1;
        Append(p1);
        m_P2 = p2; 
        Append(p2);

        m_Data.Append(pData, ulDataLen);

        // Input data length byte
        Append((int)ulDataLen);
        
        // Input data bytes
        Append(pData, ulDataLen);

        // Length expected
        m_Le = le; 
        Append(le);
}

/**
* Set the CLASS byte
* The method <code>Flush()</code> must be used when
* CCommandAPDU is OK.
*
* @param   cla
*          value to set for CLASS byte
**/
void CCommandAPDU::SetClassByte(unsigned char cla)
{
    m_CLA = cla;
}

/**
* Return the CLASS byte
**/
unsigned char CCommandAPDU::GetClassByte()
{
    return m_CLA;
}

/**
* Set the INSTRUCTION byte
* The method <code>Flush()</code> must be used when
* CCommandAPDU is OK.
*
* @param   ins
*          value to set for INSTRUCTION byte
**/
void CCommandAPDU::SetInstructionByte(unsigned char ins)
{
    m_INS = ins;
}

/**
* Return the INSTRUCTION byte
**/
unsigned char CCommandAPDU::GetInstructionByte()
{
    return m_INS;
}

/**
* Set the PARAMETER1 byte
* The method <code>Flush()</code> must be used when CCommandAPDU
* is OK.
*
* @param   p1
*          value to set for PARAMETER1 byte
**/
void CCommandAPDU::SetParameter1Byte(unsigned char p1)
{
    m_P1 = p1;
}

/**
* Return the PARAMETER1 byte
**/
unsigned char CCommandAPDU::GetParameter1Byte()
{
    return m_P1;
}

/**
* Set the PARAMETER2 byte
* The method <code>Flush()</code> must be used when CCommandAPDU
* is OK.
*
* @param   p2
*          value to set for PARAMETER2 byte
**/
void CCommandAPDU::SetParameter2Byte(unsigned char p2)
{
    m_P2 = p2;
}

/**
* Return the PARAMETER2 byte
**/
unsigned char CCommandAPDU::GetParameter2Byte()
{
    return m_P2;
}

unsigned char CCommandAPDU::GetLeByte()
{
    return m_Le;
}

void CCommandAPDU::SetLeByte(unsigned char Le)
{
    m_Le = Le;
}

/**
 * Set the input data
 * The method <code>Flush()</code> must be used when CCommandAPDU
 * is OK.
 *
 * @param   pdata
 *          value to set for input data field
 * @param   ulDataLen
 *          length of input data field
 **/
void CCommandAPDU::SetInputDataBytes(unsigned char *pData, unsigned long ulDataLen)
{
    m_Data.SetSize(ulDataLen);
    m_Data.Append(pData, ulDataLen);
}

/**
 * Return the input data
 **/
unsigned char *CCommandAPDU::GetInputDataBytes()
{
    return m_Data.GetData();
}

/**
 * Return the input data length
 **/
unsigned long CCommandAPDU::GetInputDataBytesLen()
{
    return m_Data.GetSize();
}

/**
 * Validate the modifications applied on this
 * <code>CCommandAPDU</code> object.
 **/
void CCommandAPDU::Flush()
{
    // Clear buffer
    ClearBuffer();

    // Class byte
    Append(m_CLA);

    // Instruction byte
    Append(m_INS);

    // Parameters bytes
    Append(m_P1);
    Append(m_P2);

    // Input data bytes
    if (m_Data.GetSize() > 0)
    {
        // Input data length byte
        Append((int)m_Data.GetSize());
        
        // Input data bytes
        Append(m_Data.GetData(), m_Data.GetSize());
    }

    // Length expected
    Append(m_Le);
}

short CCommandAPDU::GetCase()
{
    return m_Case;
}

void CCommandAPDU::SetCase(short sCase)
{
    m_Case = sCase;
}
