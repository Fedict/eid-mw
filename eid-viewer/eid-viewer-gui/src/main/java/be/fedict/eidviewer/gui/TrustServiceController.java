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
import java.security.PublicKey;
import java.security.cert.CertificateEncodingException;
import java.security.cert.X509Certificate;
import java.util.ArrayList;
import java.util.List;
import java.util.Observable;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.logging.Level;
import java.util.logging.Logger;

//trust.ta.belgium.be
//trust.services.belgium.be

/**
 *
 * @author Frank Marien
 */
public class TrustServiceController extends Observable implements Runnable
{
    private final static String                                 XKMS2_REASONURI_PREFIX="http://www.w3.org/2002/03/xkms#";
    private String                                              trustServiceURL;
    private XKMS2Client                                         trustServiceClient;
    private LinkedBlockingQueue<X509CertificateChainAndTrust>   chainsToBeValidated;
    private boolean                                             running,validating;

    public TrustServiceController(String trustServiceURL)
    {
        this.trustServiceURL=trustServiceURL;
        trustServiceClient=new XKMS2Client(this.trustServiceURL);
        chainsToBeValidated=new LinkedBlockingQueue<X509CertificateChainAndTrust>();
        validating=false;
    }

    public TrustServiceController setServicePublicKey(PublicKey publicKey)
    {
        trustServiceClient.setServicePublicKey(publicKey);
        return this;
    }

    public TrustServiceController setServerCertificate(X509Certificate serverCertificate)
    {
        trustServiceClient.setServerCertificate(serverCertificate);
        return this;
    }

    public TrustServiceController setProxy(String proxyHost, int proxyPort)
    {
        Logger.getLogger(TrustServiceController.class.getName()).log(Level.SEVERE, "Set Proxy Host To {0}", proxyHost);
        Logger.getLogger(TrustServiceController.class.getName()).log(Level.SEVERE, "Set Proxy Port To {0}", proxyPort);
        trustServiceClient.setProxy(proxyHost, proxyPort);
        return this;
    }

    public synchronized TrustServiceController validateLater(X509CertificateChainAndTrust certificateChain) throws InterruptedException
    {
        chainsToBeValidated.put(certificateChain);
        return this;
    }

    public synchronized TrustServiceController clear()
    {
        chainsToBeValidated.clear();
        return this;
    }

    public TrustServiceController start()
    {
         Thread me=new Thread(this);
                me.setDaemon(true);
                me.start();
        return this;
    }

    public void stop()
    {
        running=false;
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
                X509CertificateChainAndTrust chain = chainsToBeValidated.take();
                
                try
                {
                    validating=true;
                    chain.setValidating();
                    trustServiceClient.validate(chain.getTrustDomain(), chain.getCertificates(), true);
                    chain.setTrusted();
                }
                catch(CertificateEncodingException ex)
                {
                    Logger.getLogger(TrustServiceController.class.getName()).log(Level.SEVERE, null, ex);
                    chain.setValidationException(ex);     
                }
                catch(TrustDomainNotFoundException ex)
                {
                    Logger.getLogger(TrustServiceController.class.getName()).log(Level.SEVERE, null, ex);
                    chain.setValidationException(ex);
                }
                catch(RevocationDataNotFoundException ex)
                {
                    Logger.getLogger(TrustServiceController.class.getName()).log(Level.SEVERE, null, ex);
                    chain.setValidationException(ex);
                    chain.setInvalidReasons(trustServiceClient.getInvalidReasons());
                }
                catch(ValidationFailedException ex)
                {
                    Logger.getLogger(TrustServiceController.class.getName()).log(Level.SEVERE, null, ex);
                    chain.setValidationException(ex);
                    chain.setRevocationValues(trustServiceClient.getRevocationValues());
                    chain.setInvalidReasons(trimInvalidReasons(trustServiceClient.getInvalidReasons()));
                }
                catch(Exception ex)
                {
                    Logger.getLogger(TrustServiceController.class.getName()).log(Level.SEVERE, null, ex);
                    chain.setTrustServiceException(ex);
                }

                validating=false;
                setChanged();
                notifyObservers(chain);
            }
            catch (InterruptedException ex)
            {
                Logger.getLogger(TrustServiceController.class.getName()).log(Level.SEVERE, null, ex);
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
