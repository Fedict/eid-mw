//
//  AppDelegate.swift
//  eID-Viewer
//
//  Created by buildslave on 15/04/15.
//  Copyright (c) 2015 Fedict. All rights reserved.
//

import Cocoa

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate, eIDOSLayerUI {
    @IBOutlet weak var window: NSWindow!
    @IBOutlet var logItem: NSTextView!
    @IBOutlet weak var logLevel: NSPopUpButton!
    @IBOutlet weak var pinopControls: NSSegmentedControl!
    @IBOutlet weak var IdentityTab: NSView!
    @IBOutlet weak var CardPinTab: NSView!
    @IBOutlet weak var CertificatesTab: NSView!

    @IBAction func do_pinop(sender: AnyObject) {
        var send : NSSegmentedControl = sender as! NSSegmentedControl
        var alert = NSAlert()
        var sel = send.selectedSegment
        var which : eIDPinOp
        switch(sel) {
        case 0:
            which = eIDPinOp.Test
        case 1:
            which = eIDPinOp.Change
        default:
            return // we shouldn't get here
        }
        eIDOSLayerBackend.pinop(which)
    }

    @IBAction func open_file(sender: AnyObject) {
        var panel = NSOpenPanel()
        
        panel.beginWithCompletionHandler( { (Int result) -> Void in
            if(result == NSFileHandlingPanelOKButton) {
                eIDOSLayerBackend.deserialize(panel.URLs[0] as! NSURL)
            }
        })
    }
    
    func log(line: String!, withLevel level: eIDLogLevel) {
        NSOperationQueue.mainQueue().addOperationWithBlock() {
            var output : String
            switch(level) {
            case eIDLogLevel.Detail:
                output = "D: " + line
            case eIDLogLevel.Normal:
                output = "N: " + line
            case eIDLogLevel.Coarse:
                output = "C: " + line
            case eIDLogLevel.Error:
                output = "E: " + line
                var alert = NSAlert()
                alert.messageText = "Error:" + line;
                alert.runModal()
            }
            self.logItem.insertText(output + "\n")
        }
    }
    
    func newsrc(which: eIDSource) {
    }
    func newbindata(data: NSData!, withLabel label: String!) {
    }
    func newstate(state: eIDState) {
    }
    func searchView(from: AnyObject, name: String) -> AnyObject? {
        var fromId : NSUserInterfaceItemIdentification? = from as? NSUserInterfaceItemIdentification
        if (fromId == nil) {
            return nil
        }
        if (fromId?.identifier == name) {
            return from
        }
        for aView in from.subviews {
            var retval: AnyObject? = searchView(aView as! NSView, name: name)
            if(retval != nil) {
                return retval
            }
        }
        return nil
    }
    func newstringdata(data: String!, withLabel label: String!) {
        NSOperationQueue.mainQueue().addOperationWithBlock() {
            for tab in [self.IdentityTab, self.CardPinTab, self.CertificatesTab] {
                var aView = self.searchView(tab, name: label) as? NSTextField
                aView?.stringValue = data
                if(aView != nil) {
                    return
                }
            }
        }
    }

    func applicationDidFinishLaunching(aNotification: NSNotification) {
        // Insert code here to initialize your application
    }

    func applicationWillTerminate(aNotification: NSNotification) {
        // Insert code here to tear down your application
    }
    
    override func awakeFromNib() {
        eIDOSLayerBackend.setUi(self)
        eIDOSLayerBackend.mainloop_thread()
    }
}
