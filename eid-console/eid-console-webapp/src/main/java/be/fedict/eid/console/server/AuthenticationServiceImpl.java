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

package be.fedict.eid.console.server;

import java.security.cert.X509Certificate;
import java.util.List;

import be.fedict.eid.applet.service.spi.AuthenticationService;

public class AuthenticationServiceImpl implements AuthenticationService {

	public void validateCertificateChain(
			List<X509Certificate> authnCertificateChain)
			throws SecurityException {
		/*
		 * No need to do a PKI validation here. We're only interested in doing a
		 * PIN verification. For PKI validation we use the eID Trust Service.
		 */
	}
}
