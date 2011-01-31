/*
 * eID Applet Project.
 * Copyright (C) 2008-2009 FedICT.
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
package be.fedict.eidviewer.lib;

import be.fedict.eid.applet.service.Address;
import be.fedict.eid.applet.service.Identity;
import java.awt.Image;
import java.security.cert.X509Certificate;
import java.util.List;
import java.util.Observer;

/**
 * Belgian Electronic ID Card Operations
 *
 */
public interface Eid
{
    /**
     * Gives back a list of all available smart card readers.
     *
     * @return
     */
    List<String> getReaderList() throws Exception;

    byte[] readFile(byte[] fileId) throws Exception;

    Address getAddress() throws Exception;
    
    Identity getIdentity() throws Exception;

    Image getPhotoImage() throws Exception;
    byte[] getPhotoJPEG() throws Exception;

    void close() throws Exception;

    /**
     * Checks whether there is an eID card present in one of the available smart
     * card readers.
     *
     * @return
     * @throws Exception
     */
    boolean isEidPresent() throws Exception;

    /**
     * Checks whether there is a smart card reader available on the system.
     *
     * @return
     * @throws Exception
     */
    boolean hasCardReader() throws Exception;

    /**
     * Wait until a smart card reader has been connected to the system.
     *
     * @throws Exception
     */
    void waitForCardReader() throws Exception;

    /**
     * Waits for an eID card to be present in one of the available smart card
     * readers.
     *
     * @throws Exception
     */
    void waitForEidPresent() throws Exception;

    /**
     * Waits until (some) smart card has been removed from some smart card
     * reader.
     *
     * @throws Exception
     */
    void removeCard() throws Exception;

    /**
     * Change the PIN code of the eID card. The PIN will be queried to the
     * citizen via some GUI dialog.
     *
     * @throws Exception
     */

    boolean isCardStillPresent() throws Exception;

    public void yieldExclusive(boolean yield) throws Exception;
    
    void changePin() throws Exception;

    /**
     * Change the PIN code of the eID card. The PIN will be queried to the
     * citizen via some GUI dialog.
     *
     * @param requireSecureReader
     * @throws Exception
     */
    void changePin(boolean requireSecureReader) throws Exception;

    void verifyPin(boolean requireSecureReader) throws Exception;


    public boolean isIdentityTrusted();
    public boolean isAddressTrusted();

    List<X509Certificate> getRRNCertificateChain() throws Exception;
    
    List<X509Certificate> getAuthnCertificateChain() throws Exception;

    List<X509Certificate> getSignCertificateChain() throws Exception;

    /**
     * Adds an observer for eID file readout progress monitoring.
     *
     * @param observer
     */
    void addObserver(Observer observer);

    
}
