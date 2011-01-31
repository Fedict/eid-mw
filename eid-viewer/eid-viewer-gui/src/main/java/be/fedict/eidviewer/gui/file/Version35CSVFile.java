/*
 * eID Middleware Project.
 * Copyright (C) 2010 FedICT.
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
 */
package be.fedict.eidviewer.gui.file;

import be.fedict.eid.applet.service.Address;
import be.fedict.eid.applet.service.DocumentType;
import be.fedict.eid.applet.service.Gender;
import be.fedict.eid.applet.service.Identity;
import be.fedict.eid.applet.service.impl.tlv.DataConvertorException;
import be.fedict.eid.applet.service.impl.tlv.DateOfBirthDataConvertor;
import be.fedict.eidviewer.gui.EidData;
import be.fedict.eidviewer.gui.X509CertificateChainAndTrust;
import be.fedict.trust.client.TrustServiceDomains;
import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.GregorianCalendar;
import java.util.LinkedList;
import java.util.List;
import java.util.StringTokenizer;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.apache.commons.codec.binary.Base64;

/**
 *
 * @author Frank Marien
 */
public class Version35CSVFile
{
    private static final Logger logger=Logger.getLogger(Version35CSVFile.class.getName());
    private static final int    FILEVERSION                 =1;
    private static final int    FILEMAGIC                   =2;
    private static final int    DOCUMENTTYPE                =3;
    private static final int    FIRSTNAMES                  =4;
    private static final int    LASTNAME                    =5;
    private static final int    GENDER                      =6;
    private static final int    BIRTHDATE                   =7;
    private static final int    PLACEOFBIRTH                =8;
    private static final int    NATIONALITY                 =9;
    private static final int    NATIONALNUMBER              =10;
    private static final int    CARDNUMBER                  =12;
    private static final int    CARDCHIPNUMBER              =13;
    private static final int    CARDVALIDFROM               =14;
    private static final int    CARDVALIDUNTIL              =15;
    private static final int    CARDISSUINGMUNICIPALITY     =16;
    private static final int    STREETANDNUMBER             =17;
    private static final int    ZIP                         =18;
    private static final int    MUNICIPALITY                =19;
    private static final int    PHOTO                       =25;
    private static final int    RRNCERTIFICATE              =48;
    private static final int    AUTHCERTIFICATE             =53;
    private static final int    SIGNCERTIFICATE             =57;
    private static final int    CITIZENCACERTIFICATE        =61;
    private static final int    ROOTCERTIFICATE             =65;
    
    private CertificateFactory  certificateFactory  = null;
    private Address             address             = null;
    private Identity            identity            = null;
    private X509Certificate     rootCert            = null;
    private X509Certificate     citizenCert         = null;
    private X509Certificate     authenticationCert  = null;
    private X509Certificate     signingCert         = null;
    private X509Certificate     rrnCert             = null;
    private EidData             eidData;
    DateFormat                  dateFormat;

    public static void load(File file, EidData eidData) throws CertificateException, IOException
    {
        Version35CSVFile v35CVSFile=new Version35CSVFile(eidData);
                         v35CVSFile.load(file);
    }

    public Version35CSVFile(EidData eidData)
    {
        this.eidData=eidData;
        dateFormat=new SimpleDateFormat("dd.MM.yyyy");
    }

    public void load(File file) throws CertificateException, FileNotFoundException, IOException
    {
        logger.fine("Loading Version 35X CSV File");
        
        StringTokenizer     tokenizer   = null;
        int                 tokenNumber = 0;
        String              line,token  =null;
        
        certificateFactory = CertificateFactory.getInstance("X.509");
        InputStreamReader inputStreamReader=new InputStreamReader(new FileInputStream(file),"utf-8");
        BufferedReader bufferedReader = new BufferedReader(inputStreamReader);
        line=bufferedReader.readLine();
        if(line!=null)
        {
            System.out.println(line);
            tokenizer = new StringTokenizer(line, ";");
            while(tokenizer.hasMoreTokens())
            {
                token=tokenizer.nextToken();
                tokenNumber++;
                
                try
                {
                    handleToken(tokenNumber, token);
                }
                catch (UnsupportedEncodingException ex)
                {
                    logger.log(Level.SEVERE, "Unsupported Encoding for Token " + tokenNumber , ex);
                }
                catch (DataConvertorException ex)
                {
                    logger.log(Level.SEVERE, "Couldn't Convert Date Of Birth \"" + token + "\" in Token " + tokenNumber, ex);
                }
            }

            if(identity!=null)
                eidData.setIdentity(identity);

            if(address!=null)
                eidData.setAddress(address);
        }

        if(rootCert != null && citizenCert != null)
        {
            logger.fine("Certificates were gathered");
            
            if (authenticationCert != null)
            {
                logger.fine("Setting Authentication Certificate Chain");
                List authChain = new LinkedList<X509Certificate>();
                authChain.add(authenticationCert);
                authChain.add(citizenCert);
                authChain.add(rootCert);
                eidData.setAuthCertChain(new X509CertificateChainAndTrust(TrustServiceDomains.BELGIAN_EID_AUTH_TRUST_DOMAIN, authChain));
            }

            if (signingCert != null)
            {
                logger.fine("Setting Signing Certificate Chain");
                List signChain = new LinkedList<X509Certificate>();
                signChain.add(signingCert);
                signChain.add(citizenCert);
                signChain.add(rootCert);
                eidData.setSignCertChain(new X509CertificateChainAndTrust(TrustServiceDomains.BELGIAN_EID_NON_REPUDIATION_TRUST_DOMAIN, signChain));
            }

            if (rrnCert != null)
            {
                logger.fine("Setting RRN Certificate Chain");
                List rrnChain = new LinkedList<X509Certificate>();
                rrnChain.add(rrnCert);
                rrnChain.add(rootCert);
                eidData.setRRNCertChain(new X509CertificateChainAndTrust(TrustServiceDomains.BELGIAN_EID_NATIONAL_REGISTRY_TRUST_DOMAIN, rrnChain));
            }
        }
    }

    private void handleToken(int tokenNumber, String token) throws UnsupportedEncodingException, DataConvertorException
    {
        logger.log(Level.FINEST, "token #{0} : [{1}]", new Object[]{tokenNumber, token});
        switch(tokenNumber)
        {
            case DOCUMENTTYPE:
            identity=new Identity();
            identity.documentType=DocumentType.toDocumentType(token.getBytes("utf-8"));
            if(identity.documentType==null)
                logger.log(Level.SEVERE, "Unknown Document Type \"{0}\"", token);
            break;

            case FIRSTNAMES:
            String[] nameParts=token.split(" ");
            switch(nameParts.length)
            {
                case 1:
                logger.finest("First Name: One Token -> firstname, no middleName");
                identity.firstName=nameParts[0];
                break;

                case 2:
                logger.finest("First Name: Two Tokens -> one in firstName, second in middleName");
                identity.firstName=nameParts[0];
                identity.middleName=nameParts[1];
                break;

                default:
                {
                    logger.finest("First Name: More Than Two Tokens -> all but last part in firstname, last part in middleName");
                    StringBuilder firstName=new StringBuilder();
                    for(int i=0;i<=nameParts.length-2;i++)
                    {
                        firstName.append(nameParts[i]);
                        firstName.append(' ');
                    }
                    
                    identity.firstName=firstName.toString().trim();
                    identity.middleName=nameParts[nameParts.length-1];
                }
                break;
            }

            case LASTNAME:
            identity.name=token;
            break;

            case GENDER:
            identity.gender=token.equals("M")?Gender.MALE:Gender.FEMALE;
            break;

            case BIRTHDATE:
            {
                logger.fine("field BIRTHDATE");
                DateOfBirthDataConvertor dateOfBirthConvertor=new DateOfBirthDataConvertor();
                identity.dateOfBirth=dateOfBirthConvertor.convert(token.getBytes("utf-8"));
            }
            break;

            case PLACEOFBIRTH:
            identity.placeOfBirth=token;
            break;

            case NATIONALITY:
            identity.nationality=token;
            break;
            
            case NATIONALNUMBER:
            identity.nationalNumber=token;
            break;

            case CARDNUMBER:
            identity.cardNumber=token;
            break;

            case CARDCHIPNUMBER:
            identity.chipNumber=token;
            break;

            case CARDVALIDFROM:
            {
                GregorianCalendar validityBeginCalendar=new GregorianCalendar();
                try
                {
                    validityBeginCalendar.setTime(dateFormat.parse(token));
                    identity.cardValidityDateBegin=validityBeginCalendar;
                }
                catch (ParseException ex)
                {
                    logger.log(Level.SEVERE, "Failed to parse Card Validity Start Date \"" + token + "\"", ex);
                }  
            }
            break;

            case CARDVALIDUNTIL:
                GregorianCalendar validityEndCalendar=new GregorianCalendar();
                try
                {
                    validityEndCalendar.setTime(dateFormat.parse(token));
                    identity.cardValidityDateEnd=validityEndCalendar;
                }
                catch (ParseException ex)
                {
                    logger.log(Level.SEVERE, "Failed to parse Card Validity End Date \"" + token + "\"", ex);
                }
            break;

            case CARDISSUINGMUNICIPALITY:
            identity.cardDeliveryMunicipality=token;
            break;

            case STREETANDNUMBER:
            address=new Address();
            address.streetAndNumber=token;
            break;

            case ZIP:
            address.zip=token;
            break;

            case MUNICIPALITY:
            address.municipality=token;
            break;

            case PHOTO:
            eidData.setPhoto(Base64.decodeBase64(token));
            break;

            case AUTHCERTIFICATE:
                logger.finer("Gathering Authentication Certificate");
                    try
                    {
                        authenticationCert  = (X509Certificate) certificateFactory.generateCertificate(new ByteArrayInputStream(Base64.decodeBase64(token)));
                    }
                    catch (CertificateException ex)
                    {
                        logger.log(Level.SEVERE, "Failed to Convert Authentication Certificate", ex);
                    }
            break;

            case SIGNCERTIFICATE:
                logger.finer("Gathering Signing Certificate");
                    try
                    {
                        signingCert  = (X509Certificate) certificateFactory.generateCertificate(new ByteArrayInputStream(Base64.decodeBase64(token)));
                    }
                    catch (CertificateException ex)
                    {
                        logger.log(Level.SEVERE, "Failed to Convert Signing Certificate", ex);
                    }
            break;

            case CITIZENCACERTIFICATE:
                 logger.finer("Gathering Citizen CA Certificate");
                    try
                    {
                        citizenCert  = (X509Certificate) certificateFactory.generateCertificate(new ByteArrayInputStream(Base64.decodeBase64(token)));
                    }
                    catch (CertificateException ex)
                    {
                        logger.log(Level.SEVERE, "Failed to Convert Citizen CA Certificate", ex);
                    }
            break;

            case ROOTCERTIFICATE:
                 logger.finer("Gathering Belgian Root Certificate");
                    try
                    {
                        rootCert  = (X509Certificate) certificateFactory.generateCertificate(new ByteArrayInputStream(Base64.decodeBase64(token)));
                    }
                    catch (CertificateException ex)
                    {
                        logger.log(Level.SEVERE, "Failed to Belgian Root Certificate", ex);
                    }
            break;

            case RRNCERTIFICATE:
                 logger.finer("Gathering RRN Certificate");
                    try
                    {
                        rrnCert  = (X509Certificate) certificateFactory.generateCertificate(new ByteArrayInputStream(Base64.decodeBase64(token)));
                    }
                    catch (CertificateException ex)
                    {
                        logger.log(Level.SEVERE, "Failed to RRN Certificate", ex);
                    }
            break;

        }
    }
}
