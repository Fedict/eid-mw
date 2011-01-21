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

package be.fedict.eidviewer.gui;

import be.fedict.trust.client.XKMS2Client;
import be.fedict.trust.client.exception.RevocationDataNotFoundException;
import be.fedict.trust.client.exception.TrustDomainNotFoundException;
import be.fedict.trust.client.exception.ValidationFailedException;
import com.sun.xml.ws.client.ClientTransportException;
import java.security.PublicKey;
import java.security.cert.CertificateEncodingException;
import java.security.cert.X509Certificate;
import java.util.ArrayList;
import java.util.List;
import java.util.Observable;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Frank Marien
 */
public class TrustServiceController extends Observable implements Runnable
{
    private static final Logger                                 logger=Logger.getLogger(TrustServiceController.class.getName());
    private final static String                                 XKMS2_REASONURI_PREFIX="http://www.w3.org/2002/03/xkms#";
    private String                                              trustServiceURL;
    private XKMS2Client                                         trustServiceClient;
    private LinkedBlockingQueue<X509CertificateChainAndTrust>   chainsToBeValidated;
    private boolean                                             running,validating;
    private Thread                                              worker;

    public TrustServiceController(String trustServiceURL)
    {
        this.trustServiceURL=trustServiceURL;
        trustServiceClient=new XKMS2Client(this.trustServiceURL);
        chainsToBeValidated=new LinkedBlockingQueue<X509CertificateChainAndTrust>();
        validating=false;
    }

    public TrustServiceController setServicePublicKey(PublicKey publicKey)
    {
        logger.fine("Setting Service Public Key");
        trustServiceClient.setServicePublicKey(publicKey);
        return this;
    }

    public TrustServiceController setServerCertificate(X509Certificate serverCertificate)
    {
        logger.fine("Setting Server Certificate");
        trustServiceClient.setServerCertificate(serverCertificate);
        return this;
    }

    public TrustServiceController setProxy(String proxyHost, int proxyPort)
    {
        if(proxyHost!=null)
            logger.log(Level.INFO, "Set Proxy To {0}:{1}", new Object[]{proxyHost, proxyPort});
        else
            logger.log(Level.INFO, "Removing Proxy");
        trustServiceClient.setProxy(proxyHost, proxyPort);   
        return this; 
    }

    public synchronized TrustServiceController validateLater(X509CertificateChainAndTrust certificateChain)
    {
        logger.log(Level.FINEST,"Enqueueing {0} for validation",new Object[] {certificateChain.toString()});
        
        try
        {
            chainsToBeValidated.put(certificateChain);
            logger.log(Level.FINEST,"Enqueued {0} successfully",new Object[] {certificateChain.toString()});
        }
        catch (InterruptedException iex)
        {
            logger.log(Level.SEVERE, "TrustServiceController Interruped while adding certificates to be validated", iex);
        }
        
        return this;
    }

    public synchronized TrustServiceController clear()
    {
        logger.fine("Clearing");
        chainsToBeValidated.clear();
        return this;
    }

    public TrustServiceController start()
    {
        logger.fine("Starting");
        worker=new Thread(this);
        worker.setDaemon(true);
        worker.start();
        return this;
    }

    public void stop()
    {
        logger.fine("Stopping..");
        running=false;
        worker.interrupt();
    }

    public boolean isValidating()
    {
        return validating || (!chainsToBeValidated.isEmpty());
    }

    public void run()
    {
        running=true;
        while(running)
        {
            try
            {
                logger.finest("Sleeping until validation requested");
                X509CertificateChainAndTrust chain = chainsToBeValidated.take();
                logger.finest("Validation requested");
                
                try
                {
                    validating=true;
                    chain.setValidating();
                    logger.log(Level.INFO,"Validating {0}", new Object[] {chain.toString()});
                    trustServiceClient.validate(chain.getTrustDomain(), chain.getCertificates(), true);
                    logger.log(Level.INFO,"Trusted");
                    chain.setTrusted();
                }
                catch(CertificateEncodingException ex)
                {
                    logger.log(Level.SEVERE, "Certificate Encoding Exception", ex);
                    chain.setValidationException(ex);     
                }
                catch(TrustDomainNotFoundException ex)
                {
                    logger.log(Level.SEVERE, "Trust Domain Not Found", ex);
                    chain.setValidationException(ex);
                }
                catch(RevocationDataNotFoundException ex)
                {
                    logger.log(Level.WARNING, "Revocation Data Not Found", ex);
                    chain.setValidationException(ex);
                    chain.setInvalidReasons(trustServiceClient.getInvalidReasons());
                }
                catch(ValidationFailedException ex)
                {
                    logger.log(Level.INFO, "Validation Failed", ex);
                    chain.setValidationException(ex);
                    chain.setRevocationValues(trustServiceClient.getRevocationValues());
                    chain.setInvalidReasons(trimInvalidReasons(trustServiceClient.getInvalidReasons()));
                }
                catch(ClientTransportException ex)
                {
                    logger.log(Level.SEVERE, "Transport Exception Trying to Validate Certificate Chain",ex);
                    logger.log(Level.SEVERE, "Check the Proxy Settings, DNS Availability and Trust Service Accessibility",ex);
                    chain.setTrustServiceException(ex);
                }
                catch(Exception ex)
                {
                    logger.log(Level.SEVERE, "General Exception Trying to Validate Certificate Chain",ex);
                    chain.setTrustServiceException(ex);
                }

                validating=false;
                setChanged();
                notifyObservers(chain);
            }
            catch(InterruptedException ex)
            {
                logger.log(running?Level.SEVERE:Level.INFO, "TrustServiceController Worker Interrupted",ex);
                running=false;
            }
        }
    }

    private List<String> trimInvalidReasons(List<String> reasons)
    {
        List<String> trimmed=new ArrayList<String>(reasons.size());
        for(String reason : reasons)
            if(reason.startsWith(XKMS2_REASONURI_PREFIX))
                trimmed.add(reason.substring(XKMS2_REASONURI_PREFIX.length()));
        return trimmed;
    }
}
