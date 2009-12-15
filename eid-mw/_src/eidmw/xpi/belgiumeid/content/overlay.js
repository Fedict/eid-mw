/*
 * eID Middleware Project.
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



var belgiumeid = {
  onLoad: function() {
    // initialization code
    this.initialized = true;
    //this.strings = document.getElementById("belgiumeid-strings");
    
    beidPKCS11 = new BelgiumEidPKCS11();
    beidPKCS11.removeModuleIfNotAvailable();
    if (!beidPKCS11.registerModule() && !beidPKCS11.pkcs11ModuleAvailable() && beidPKCS11.shouldShowModuleNotFoundNotification() ) {
      beidPKCS11.notifyModuleNotFound();
//      beidPKCS11.shouldShowModuleNotFoundNotification(false);
    }
    
  },
};
window.addEventListener("load", function(e) { belgiumeid.onLoad(e); }, false);
