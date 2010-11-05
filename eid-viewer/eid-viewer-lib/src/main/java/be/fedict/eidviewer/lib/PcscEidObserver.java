/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package be.fedict.eidviewer.lib;

import java.util.Observable;
import java.util.Observer;

/**
 *
 * @author frank
 */
class PcscEidObserver implements Observer
{
    public PcscEidObserver()
    {
    }

    public void update(Observable o, Object o1)
    {
        String oStr=(o!=null?o.toString():"null");
        String o1Str=(o1!=null?o1.toString():"null");
        System.err.println("update [" + oStr + "," + o1Str + "]");
    }
}
