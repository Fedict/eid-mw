/* ****************************************************************************
 
 * eID Middleware Project.
 * Copyright (C) 2014 FedICT.
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
 
 **************************************************************************** */


#include <CoreFoundation/CoreFoundation.h>

CFDictionaryRef CreateDictionaryFromPlist(CFStringRef, SInt32 *errorCode);
void StoreDictionaryInPlist(CFPropertyListRef propListRef, CFStringRef stringPlist, SInt32 *errorCode);
void MergeValueIntoArray(CFArrayRef arrayRef, CFStringRef stringRef, SInt32 *errorCode);

//the combination of values in ifdVendorID,ifdProductID and ifdFriendlyName we call a readerentry
//CheckReaderEntryPressence checks if the readerentry at index in sourceDict can be found in destDict
Boolean CheckValuePresence(CFIndex index,CFArrayRef pArrayDestifdVendorID,CFArrayRef pArraySrcifdVendorID,SInt32 *errorCode);

Boolean CheckReaderEntryPresence(CFIndex index, CFDictionaryRef destDictRef,CFDictionaryRef sourceDictRef, SInt32 *errorCode);

void AddReaderEntry(CFIndex index, CFDictionaryRef destDictRef,CFDictionaryRef sourceDictRef, SInt32 *errorCode);


int main(int argc, const char * argv[])
{
    SInt32 errorCode;
    
    CFArrayRef pArraySrcVID; //array of Vendor ID's in the source plist (plist to copy from)
    CFArrayRef pArrayDestVID; //array of Vendor ID's in the destination plist (plist to copy to)
    CFMutableArrayRef pArrayNewVID;//new array of Vendor ID's containing the merge of source and destination
    
    CFArrayRef pArraySrcPID;  //array of Product ID's in the source plist (plist to copy from)
    CFArrayRef pArrayDestPID;  //array of Product ID's in the destination plist (plist to copy to)
    CFMutableArrayRef pArrayNewPID; //new array of Product ID's containing the merge of source and destination
    
    CFArrayRef pArraySrcFN;   //array of Friendly names in the source plist (plist to copy from)
    CFArrayRef pArrayDestFN;   //array of Friendly names in the destination plist (plist to copy to)
    CFMutableArrayRef pArrayNewFN;   //new array of Friendly names containing the merge of source and destination
    
    //the new Dictionary has been added by the installer
    //the original one will be checked against the new one, to see if all old devices are still present in the list
    
    CFDictionaryRef pDictDestOrg = CreateDictionaryFromPlist(CFSTR("./Info_org.plist"), &errorCode);
    //CreateDictionaryFromPlist(CFSTR("/usr/libexec/SmartCardServices/drivers/ifd-ccid.bundle/Contents/Info.plist"), &errorCode);
    CFMutableDictionaryRef pDictDest = NULL;
    if (pDictDestOrg != NULL) {
        pDictDest = CFDictionaryCreateMutableCopy(kCFAllocatorDefault, 0, pDictDestOrg);
    }
    CFDictionaryRef pDictSrc = CreateDictionaryFromPlist(CFSTR("./Info.plist"), &errorCode);
    
    if( (pDictSrc != NULL) && (pDictDest != NULL) )
    {
        //get the 3 arrays per library that we need to search into
        //we create mutable copies for the destination arrays, as they might need to be expanded
        Boolean bArrayPresent = false;
        Boolean bAllArraysPresent = false;
        bArrayPresent = CFDictionaryGetValueIfPresent (pDictDest,CFSTR("ifdVendorID"),(const void **)&pArrayDestVID);
        if (bArrayPresent)
        {
            //create a mutable (changeable) copy
            pArrayNewVID = CFArrayCreateMutableCopy(kCFAllocatorDefault, 0, pArrayDestVID);
            bArrayPresent = CFDictionaryGetValueIfPresent (pDictSrc,CFSTR("ifdVendorID"),(const void **)&pArraySrcVID);
            if (bArrayPresent)
            {
                bArrayPresent = CFDictionaryGetValueIfPresent (pDictDest,CFSTR("ifdProductID"),(const void **)&pArrayDestPID);
                if (bArrayPresent)
                {
                    pArrayNewPID = CFArrayCreateMutableCopy(kCFAllocatorDefault, 0, pArrayDestPID);
                    bArrayPresent = CFDictionaryGetValueIfPresent (pDictSrc,CFSTR("ifdProductID"),(const void **)&pArraySrcPID);
                    if (bArrayPresent)
                    {
                        bArrayPresent = CFDictionaryGetValueIfPresent (pDictDest,CFSTR("ifdFriendlyName"),(const void **)&pArrayDestFN);
                        if (bArrayPresent)
                        {
                            pArrayNewFN = CFArrayCreateMutableCopy(kCFAllocatorDefault, 0, pArrayDestFN);
                            bArrayPresent = CFDictionaryGetValueIfPresent (pDictSrc,CFSTR("ifdFriendlyName"),(const void **)&pArraySrcFN);
                            if (bArrayPresent)
                            {
                                bAllArraysPresent = true;
                            }
                        }
                    }
                }
            }

        if (!bAllArraysPresent) {
            CFShow(CFSTR("Cannot merge\nNot all arrays are present in the plists\n"));
            return 0;
        }
        
        CFIndex indexSrcTotalCount = CFArrayGetCount (pArraySrcVID);
        CFIndex indexDestTotalCount = CFArrayGetCount (pArrayDestVID);
        
        //go through the array of the source plist, and add entries that are missing in the dest array
        CFIndex index = 0;
        Boolean bEntryFound;
        Boolean bStopSearch;
        CFRange range;

        CFStringRef pStringSrcVID;
        CFStringRef pStringSrcPID;
        CFStringRef pStringSrcFN;
        CFStringRef pStringTemp;
        
        for (index = 0; index < indexSrcTotalCount; index++)
        {
            pStringSrcVID = CFArrayGetValueAtIndex (pArraySrcVID, index);
            pStringSrcPID = CFArrayGetValueAtIndex (pArraySrcPID, index);
            pStringSrcFN = CFArrayGetValueAtIndex (pArraySrcFN, index);
            bStopSearch = false;
            bEntryFound = false;
            
            range.location = 0;
            range.length = indexDestTotalCount;
            pStringSrcVID = CFArrayGetValueAtIndex (pArraySrcVID, index);
            
            CFIndex found = CFArrayGetFirstIndexOfValue (pArrayDestVID,range,pStringSrcVID);
            while ((found != -1)&&(!bStopSearch)) {
                //We found a match for VID's at Destination index 'found'
                //Now check if at that index, the PID's also match
                pStringTemp = CFArrayGetValueAtIndex (pArrayDestPID,found);
                
                if (kCFCompareEqualTo == CFStringCompare(pStringTemp, pStringSrcPID, kCFCompareCaseInsensitive))
                {
                    //the PID's also match at Destination index 'found'
                    //Now check if Friendly names also match
                    pStringTemp = CFArrayGetValueAtIndex (pArrayDestFN,found);
                    if (kCFCompareEqualTo == CFStringCompare(pStringTemp, pStringSrcFN, kCFCompareCaseInsensitive))
                    {
                        bEntryFound = true;
                        bStopSearch = true;
                    }
                }
                if(bEntryFound == false)
                {
                    //search if the VID is also present further down the array
                    if ((found+1) < indexDestTotalCount)
                    {
                        range.length = indexDestTotalCount - (found+1);
                        range.location = found+1;
                        found = CFArrayGetFirstIndexOfValue (pArrayDestVID,range,pStringSrcVID);
                    }
                    else
                    {
                        bStopSearch = true;
                    }
                }
            }
            //the destination plist has been searched for the entry at 'index' of the source plist
            if(bEntryFound == false)
            {
                range.location = 0;
                range.length = CFArrayGetCount (pArrayNewVID);
                CFIndex vIDFound = CFArrayGetFirstIndexOfValue (pArrayNewVID,range,pStringSrcVID);
                //if the entry was not found, we add it to the destination arrays now
                //add it up front in the subarray with the same vendorID's if the vendorid was already present
                //If its a new vendorid, add it to the back of the arrays
                if(vIDFound != -1)
                {
                    CFArrayInsertValueAtIndex (pArrayNewVID, vIDFound,pStringSrcVID);
                    CFArrayInsertValueAtIndex (pArrayNewPID, vIDFound,pStringSrcPID);
                    CFArrayInsertValueAtIndex (pArrayNewFN, vIDFound,pStringSrcFN);
                }
                else
                {
                    CFArrayAppendValue(pArrayNewVID, pStringSrcVID);
                    CFArrayAppendValue(pArrayNewPID, pStringSrcPID);
                    CFArrayAppendValue(pArrayNewFN, pStringSrcFN);
                }
            }
            
        }//end of for (entire plist searched)
        
        //now put the merged arrays into the destination dictionary
        CFDictionaryReplaceValue (pDictDest,CFSTR("ifdVendorID"),pArrayNewVID);
        CFDictionaryReplaceValue (pDictDest,CFSTR("ifdProductID"),pArrayNewPID);
        CFDictionaryReplaceValue (pDictDest,CFSTR("ifdFriendlyName"),pArrayNewFN);
        //write changes into the new plist
        StoreDictionaryInPlist(pDictDest, CFSTR("./Info_merged.plist"), &errorCode);
        
        //release all created variables
        CFRelease(pDictDest);
        }
        CFShow(CFSTR("Merge completed\n"));
    }
    else{
        CFShow(CFSTR("Merge skipped\n"));
    }
    return 0;
}

CFDictionaryRef CreateDictionaryFromPlist(CFStringRef stringPlist, SInt32 *errorCode)
{
    CFDataRef dataRef;
    CFStringRef errorStringRef;
    
    CFURLRef fileURLRef = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,stringPlist,kCFURLPOSIXPathStyle,false);
    Boolean status = CFURLCreateDataAndPropertiesFromResource(kCFAllocatorDefault,fileURLRef,&dataRef,NULL,NULL,errorCode);
    if (!status) {
        //error
    }
    CFDictionaryRef dictRef = CFPropertyListCreateFromXMLData(kCFAllocatorDefault,dataRef,
                                                              kCFPropertyListMutableContainersAndLeaves,&errorStringRef);
    if(fileURLRef != NULL)
        CFRelease(fileURLRef);
    if(dataRef != NULL)
        CFRelease(dataRef);
    return dictRef;
}

void StoreDictionaryInPlist(CFPropertyListRef propListRef, CFStringRef stringPlist, SInt32 *errorCode)
{
    CFErrorRef errorRef;
    CFURLRef fileURLRef = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,stringPlist,kCFURLPOSIXPathStyle,false);
    
    CFDataRef dataRef = CFPropertyListCreateData(kCFAllocatorDefault, propListRef, kCFPropertyListXMLFormat_v1_0, 0, &errorRef);
    // Write the plist to the file.
    Boolean status = CFURLWriteDataAndPropertiesToResource(fileURLRef, dataRef, NULL, errorCode);
    if (!status) {
        //error
    }
    if(fileURLRef != NULL)
        CFRelease(fileURLRef);
    if(dataRef != NULL)
        CFRelease(dataRef);
}




