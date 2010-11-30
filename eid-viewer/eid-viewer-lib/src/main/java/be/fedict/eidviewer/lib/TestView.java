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

package be.fedict.eidviewer.lib;

import be.fedict.eid.applet.DiagnosticTests;
import be.fedict.eid.applet.Status;
import be.fedict.eid.applet.View;
import java.awt.Component;
import be.fedict.eid.applet.Messages;

public class TestView implements View
{

    public void addDetailMessage(String detailMessage)
    {
        System.err.println("addDetailMessage [" + detailMessage + "]");
    }

    public Component getParentComponent()
    {
        System.err.println("getParentComponent");
        return null;
    }

    public boolean privacyQuestion(boolean includeAddress, boolean includePhoto, String identityDataUsage)
    {
        System.err.println("privacyQuestion [" + identityDataUsage + "]");
        return true;
    }

    public void setStatusMessage(Status status, Messages.MESSAGE_ID messageId)
    {
        System.err.println("setStatusMessage [" + status.name() + "]");
    }

    public void addTestResult(DiagnosticTests diagnosticTest, boolean success, String description)
    {
        System.err.println("addTestResult [" + description + "]");
    }

    public void setProgressIndeterminate()
    {
        System.err.println("setProgressIndeterminate");
    }

    public void resetProgress(int max)
    {
        System.err.println("resetProgress");
    }

    public void increaseProgress()
    {
        System.err.println("increaseProgress");
    }
}
