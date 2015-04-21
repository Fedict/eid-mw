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

    @IBAction func do_pinop(sender: AnyObject) {
        var alert = NSAlert()
        alert.messageText = "Pin operations not yet implemented"
        alert.runModal()
    }
    @IBAction func open_file(sender: AnyObject) {
        NSOperationQueue.mainQueue().addOperationWithBlock() {
            var alert = NSAlert()
            alert.messageText = "Opening files not yet implemented"
            alert.runModal()
        }
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
    func newstringdata(data: String!, withLabel label: String!) {
        NSOperationQueue.mainQueue().addOperationWithBlock() {
            var aView : NSView
            for aView in self.window.contentView.subviews {
                if (aView.identifier.isEqualToString(label) && aView.type == "NSTextView") {
                    var tv : NSTextView = aView as! NSTextView
                    tv.setValue(data)
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
