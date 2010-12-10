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
import java.util.Observable;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author frank
 */
public class TrustServiceController extends Observable implements Runnable
{   
    private XKMS2Client                                         trustServiceClient;
    private LinkedBlockingQueue<X509CertificateChainAndTrust>   chainsToBeValidated;
    private boolean                                             running;

    public TrustServiceController(String url)
    {
        trustServiceClient=new XKMS2Client(url);
        chainsToBeValidated=new LinkedBlockingQueue<X509CertificateChainAndTrust>();
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
        trustServiceClient.setProxy(proxyHost, proxyPort);
        return this;
    }

    public synchronized TrustServiceController validateLater(X509CertificateChainAndTrust certififateChain) throws InterruptedException
    {
        chainsToBeValidated.put(certififateChain);
        return this;
    }

    public TrustServiceController start()
    {
        new Thread(this).start();
        return this;
    }

    void stop()
    {
        running=false;
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
                    chain.setInvalidReasons(trustServiceClient.getInvalidReasons());
                }

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
}
