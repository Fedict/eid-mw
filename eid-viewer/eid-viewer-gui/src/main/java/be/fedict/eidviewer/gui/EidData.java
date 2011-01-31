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

import be.fedict.eid.applet.service.Address;
import be.fedict.eid.applet.service.Identity;

/**
 *
 * @author Frank Marien
 */
public interface EidData
{
    public EidData  setIdentity(Identity identity);
    public EidData  setAddress(Address address);
    public EidData  setPhoto(byte[] photo);
    public EidData  setAuthCertChain(X509CertificateChainAndTrust authCertChain);
    public EidData  setSignCertChain(X509CertificateChainAndTrust signCertChain);
    public EidData  setRRNCertChain(X509CertificateChainAndTrust rrnCertChain);

    public Identity                     getIdentity();
    public Address                      getAddress();
    public byte[]                       getPhoto();
    public X509CertificateChainAndTrust getAuthCertChain();
    public X509CertificateChainAndTrust getSignCertChain();
    public X509CertificateChainAndTrust getRRNCertChain();

    public boolean hasAddress();
    public boolean hasIdentity();
    public boolean hasPhoto();
    public boolean hasAuthCertChain();
    public boolean hasSignCertChain();
    public boolean hasRRNCertChain();
}
