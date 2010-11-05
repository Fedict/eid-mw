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
