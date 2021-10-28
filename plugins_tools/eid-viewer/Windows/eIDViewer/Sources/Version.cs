using System.Xml;
using System;
using System.Windows;


namespace eIDViewer
{
    public static class Version
    {

        //try to jump to a subelement with:
        //a certain element name (elementName),
        //and a tagname(attributeName) with a value set to (attributeValue)
        //stop searching when end element (endElementName) is reached
        public static bool jumpToSubElement(ref XmlTextReader textReader, string elementName, string attributeName, string attributeValue, string endElementName)
        {
            while (textReader.Read())
            {
                //walk trough all the nodes untill we find the element, or untill we reach the endElement
                switch (textReader.NodeType)
                {
                    case XmlNodeType.Element:
                        //wait till you find the elementName tag with the attributeName attribute
                        if (String.Equals(textReader.Name, elementName, StringComparison.Ordinal))
                        {
                            while (textReader.MoveToNextAttribute())
                            {
                                if (String.Equals(textReader.Name, attributeName, StringComparison.OrdinalIgnoreCase))
                                {
                                    if (String.Equals(textReader.Value, attributeValue, StringComparison.OrdinalIgnoreCase))
                                    {
                                        //we found the element with the attribute we're looking for
                                        return true;
                                    }
                                }
                            }
                        }
                        break;
                    case XmlNodeType.Text:
                        break;

                    case XmlNodeType.EndElement:
                        if (String.Equals(textReader.Name, endElementName, StringComparison.Ordinal))
                            return false;
                        break;
                }
            }
            return false;
        }

        //try to jump to a subelement with:
        //a certain element name (elementName),
        //stop searching when end element (endElementName) is reached
        public static bool jumpToSubElement(ref XmlTextReader textReader, string elementName, string endElementName)
        {
            while (textReader.Read())
            {
                //walk trough all the nodes untill we find the element, or untill we reach the endElement
                switch (textReader.NodeType)
                {
                    case XmlNodeType.Element:
                        //wait till you find the "OS" tag with the "Windows" attribute
                        if (String.Equals(textReader.Name, elementName, StringComparison.Ordinal))
                        {
                            return true;
                        }
                        break;
                    case XmlNodeType.Text:
                        break;

                    case XmlNodeType.EndElement:
                        if (String.Equals(textReader.Name, endElementName, StringComparison.Ordinal))
                            return false;
                        break;
                }
            }
            return false;
        }


        public static void CheckAttributeValue(ref XmlTextReader textReader, string attributeName, ref string attributeValue)
        {
            attributeValue = "";
            if (textReader.MoveToAttribute(attributeName))
            {
                attributeValue = textReader.Value;
                textReader.MoveToElement();
            }
        }

        //returns -1 if the xml viewer version is lower then the running viewer version 
        //returns 1 if the xml viewer version is higher then the running viewer version 
        //returns 0 if the xml viewer version is equal to the running viewer version,
        //or when the comparison failed
        public static int CompareXMLToCurrentVersion(ref XmlTextReader textReader)
        {
            int major = -1;
            int minor = -1;
            int build = -1;

            while (textReader.MoveToNextAttribute())
            {
                if (String.Equals(textReader.Name, "major", StringComparison.Ordinal))
                {
                    if (!Int32.TryParse(textReader.Value, out major))
                    {
                        return 0; //we cannot compare without major number
                    }
                }
                else if (String.Equals(textReader.Name, "minor", StringComparison.Ordinal))
                {
                    if (!Int32.TryParse(textReader.Value, out minor))
                    {
                        return 0; //we will not compare against a misformatted value
                    }
                }
                else if (String.Equals(textReader.Name, "build", StringComparison.Ordinal))
                {
                    if (!Int32.TryParse(textReader.Value, out build))
                    {
                        return 0; //we will not compare against a misformatted value
                    }
                }
            }
            //we should have all needed version numbers now
            if (major > -1)
            {
                if (major < About.imajor)
                    return -1;
                if (major > About.imajor)
                    return 1;
            }
            else
                return 0;//we cannot compare without major number

            //majors are equal
            if (minor > -1)
            {
                if (minor < About.iminor)
                    return -1;
                if (minor > About.iminor)
                    return 1;
            }

            //majors and minors are equal
            if (build > -1)
            {
                if (build < About.ibuild)
                    return -1;
                if (build > About.ibuild)
                    return 1;
            }

            return 0;
        }

        //check if our version is within this upgrade range (min - max)
        public static bool InAutoUpgradeVersion(ref XmlTextReader textReader)
        {
            bool retVal = false;
            bool keepParsing = true;

            while (textReader.Read() && keepParsing)
            {
                switch (textReader.NodeType)
                {
                    case XmlNodeType.Element:
                        if (String.Equals(textReader.Name, "min", StringComparison.Ordinal))
                        {

                            if (CompareXMLToCurrentVersion(ref textReader) > 0)
                            {
                                //xml minimum version is higher then our version, so we are under this range
                                return false;
                            }
                            else
                            {
                                //we meet the min requirements, only a failed max requirement can fail the check now
                                retVal = true;
                            }
                        }
                        else if (String.Equals(textReader.Name, "max", StringComparison.Ordinal))
                        {
                            if (CompareXMLToCurrentVersion(ref textReader) < 0)
                            {
                                //xml maximum version is lower then our version, so we are above this range
                                return false;
                            }
                            else
                            {
                                //we meet the max requirement, only a failed min requirement can fail the check now
                                retVal = true;
                            }
                        }
                        break;

                    case XmlNodeType.EndElement:
                        //stop at the end of this sub-element ("auto-upgrade-version")
                        if (String.Equals(textReader.Name, "auto-upgrade-version", StringComparison.Ordinal))
                        {
                            keepParsing = false;
                        }
                        break;
                    default:
                        break;
                }
            }
            return retVal;
        }

        public static bool InAutoUpgradeVersions(ref XmlTextReader textReader)
        {
            bool keepParsing = true;

            while (textReader.Read() && keepParsing)
            {
                switch (textReader.NodeType)
                {
                    case XmlNodeType.Element:
                        if (String.Equals(textReader.Name, "auto-upgrade-version", StringComparison.Ordinal))
                        {
                            //check if our version is within this upgrade range
                            if (InAutoUpgradeVersion(ref textReader))
                            {
                                //no need to keep parsing all the version ranges, being in one range is enough
                                return true;
                            }
                        }
                        break;

                    case XmlNodeType.EndElement:
                        //stop at the end of this sub-element ("auto-upgrade-versions")
                        if (String.Equals(textReader.Name, "auto-upgrade-versions", StringComparison.Ordinal))
                        {
                            keepParsing = false;
                        }
                        break;
                    default:
                        break;
                }
            }
            return false;
        }

        /*
         * 
  <beid>
	<OS name="windows">
		<comp name="eid-viewer">
			<latest-version major="5" minor="1" build="20">
				<latest-url	url = "https://eid.belgium.be/" url_nl = "https://eid.belgium.be/nl" url_fr = "https://eid.belgium.be/fr" url_de = "https://eid.belgium.be/de" url_en = "https://eid.belgium.be/en"/>
				<release-notes url = "https://eid.belgium.be/sites/default/files/software/RN5020.pdf"/>
				<auto-upgrade-versions>
					<auto-upgrade-version>
						<min major="5" minor="0" build="6"/>
						<max major="5" minor="0" build="8"/>
					</auto-upgrade-version>
					<auto-upgrade-version>
						<min major="5" minor="0" build="10"/>
						<max major="5" minor="2" build="16"/>
					</auto-upgrade-version>
				</auto-upgrade-versions>
			</latest-version>
			<latest-supported-OS>
				<oldOS version = "6.1">
					<oldOS-latest-version major="4" minor="4" build="20">
						<oldOS-latest-url 	url = "https://eid.belgium.be/" url_nl = "https://eid.belgium.be/nl" url_fr = "https://eid.belgium.be/fr" url_de = "https://eid.belgium.be/de" url_en = "https://eid.belgium.be/en"/>
						<oldOS-release-notes url = "https://eid.belgium.be/sites/default/files/software/RN4420.pdf"/>
					</oldOS-latest-version>
					
				</oldOS>
				<oldOS version = "6.2">
					<oldOS-latest-version major="5" minor="1" build="2">
						<oldOS-latest-url 	url = "https://eid.belgium.be/" url_nl = "https://eid.belgium.be/nl" url_fr = "https://eid.belgium.be/fr" url_de = "https://eid.belgium.be/de" url_en = "https://eid.belgium.be/en"/>
						<oldOS-release-notes url = "https://eid.belgium.be/sites/default/files/software/RN502.pdf"/>
					</oldOS-latest-version>
				</oldOS>
			</latest-supported-OS>
		</eid-viewer>
	</OS>
	<OS name="macOS">
    </OS>
</beid>
         * 
         * */

        public static bool getUpdateUrl(out bool updateNeeded, string lang, ref string url, ref string releaseNotes)
        {
            XmlTextReader textReader = new XmlTextReader("https://eid.belgium.be/sites/default/files/software/eidversions.xml");
            updateNeeded = false;
            bool keepParsing = true;
            url = "";

            try
            {
                //jump to the <OS name="windows"> section
                if (!jumpToSubElement(ref textReader, "OS", "name", "windows", "beid"))
                    return false;

                //in the windows section, jump to the <comp name="eid-viewer"> section
                if (!jumpToSubElement(ref textReader, "comp", "name", "eid-viewer", "beid"))
                    return false;

                //within the <OS name="windows"> <comp name="eid-viewer"> element, search for the subelements <latest-version> and <latest-supported-OS>
                //walk trough all the nodes
                while (textReader.Read() && keepParsing)
                {
                    switch (textReader.NodeType)
                    {
                        case XmlNodeType.Element:
                            //check if this viewer's version is within the update range
                            //do not return when done, if the OS this viewer is running on is part of the latest-supported-OS,
                            //a different update (or none) is required
                            if (String.Equals(textReader.Name, "latest-version", StringComparison.Ordinal))
                            {
                                //get the latest version number from the attributes
                                //if this one equals the current version, stop parsing
                                if (CompareXMLToCurrentVersion(ref textReader) > 0)
                                {
                                    //there is a higher version number present, now fill in all data from the xml (url, release_notes)
                                    //verify if our version number is within an auto update range
                                    while (textReader.Read() && keepParsing)
                                    {                                        
                                        switch (textReader.NodeType)
                                        {
                                            case XmlNodeType.Element:

                                                //parse the multiple version ranges
                                                if (String.Equals(textReader.Name, "latest-url", StringComparison.Ordinal))
                                                {
                                                    //check if this element contains the url attribute, if so, store it in the url string
                                                    CheckAttributeValue(ref textReader, "url_" + lang, ref url);
                                                    //if no url of this viewer's language is specified, try getting the default url
                                                    if (url.Equals(""))
                                                    {
                                                        CheckAttributeValue(ref textReader,"url", ref url);
                                                    }
                                                }

                                                else if (String.Equals(textReader.Name, "release-notes", StringComparison.Ordinal))
                                                {
                                                    //check if this element contains the url attribute, if so, store it in the url string
                                                    CheckAttributeValue(ref textReader, "url", ref releaseNotes);
                                                }

                                                //parse the multiple version ranges
                                                else if (String.Equals(textReader.Name, "auto-upgrade-versions", StringComparison.Ordinal))
                                                {
                                                    if (InAutoUpgradeVersions(ref textReader))
                                                    {
                                                        updateNeeded = true;
                                                    }
                                                }

                                                break;

                                            case XmlNodeType.EndElement:
                                                //stop at the end of this sub-element ("latest-version")
                                                if (String.Equals(textReader.Name, "latest-version", StringComparison.Ordinal))
                                                {
                                                    keepParsing = false;
                                                }
                                                break;
                                            default:
                                                break;
                                        }
                                    }
                                    //do not drop out of the outer while loop
                                    keepParsing = true;
                                }
                                else
                                {
                                    //no need to update (no newer version) , no error
                                    return true;
                                }
                            }

                            /*currently we do not support this feature, due to the items below

                            //wait till you find the "OS" tag with the currentOS attribute
                            //beware, below .Net 5.0, Environment.OSversion may be wrong when running in Windows compatibility mode,
                            //also needs a manual manifest file in order to use deprecated version checks (to distinguish versions above 6.2)
                            //this feature is currently unused in the xml file, and no old release section currently exists on eid.belgium.be
                            
                            else if (String.Equals(textReader.Name, "latest-supported-OS", StringComparison.Ordinal))
                            {
                                string OSversion = Environment.OSVersion.Version.Major.ToString() + "." + Environment.OSVersion.Version.Minor.ToString();

                                //within the <OS name="windows"> <comp name="eid-viewer"> <latest-supported-OS> element
                                //try to find sub element version, stop searching when closing element "latest-supported-OS" is reached
                                if (jumpToSubElement(ref textReader, "oldOS", "version", OSversion, "latest-supported-OS"))
                                {
                                    if (jumpToSubElement(ref textReader, "oldOS-latest-version", "oldOS"))
                                    {
                                        //we found an "oldOS-latest-version" section for the current running OS (meaning this OS is no longer supported in the latest online version)
                                        //this means we need to ignore all data outside this section (urls, update needed, etc..)
                                        updateNeeded = false;
                                        url = "";
                                        releaseNotes = "";

                                        //check if a higher version is available for this OS specifically (latest supported version for this OS)
                                        if (CompareXMLToCurrentVersion(ref textReader) > 0)
                                        {
                                            updateNeeded = true;

                                            //parse the oldOS subsection
                                            while (textReader.Read() && keepParsing)
                                            {
                                                switch (textReader.NodeType)
                                                {
                                                    case XmlNodeType.Element:
                                                        if (String.Equals(textReader.Name, "oldOS-latest-url", StringComparison.Ordinal))
                                                        {
                                                            //check if this element contains the url attribute, if so, store it in the url string
                                                            CheckAttributeValue(ref textReader, "url_" + lang, ref url);
                                                            //if no url of this viewer's language is specified, try getting the default url
                                                            if (url.Equals(""))
                                                            {
                                                                CheckAttributeValue(ref textReader, "url", ref url);
                                                            }
                                                        }
                                                        else if (String.Equals(textReader.Name, "oldOS-release-notes", StringComparison.Ordinal))
                                                        {
                                                            //check if this element contains the url attribute, if so, store it in the url string
                                                            CheckAttributeValue(ref textReader, "url", ref releaseNotes);
                                                        }
                                                        break;

                                                    case XmlNodeType.EndElement:
                                                        //stop at the end of this sub-element ("latest-version")
                                                        if (String.Equals(textReader.Name, "oldOS-latest-version", StringComparison.Ordinal))
                                                        {
                                                            keepParsing = false;
                                                        }
                                                        break;
                                                    default:
                                                        break;
                                                }
                                            }
                                        }
                                        //we can stop parsing the entire xml file, as we have parsed a "oldOS-latest-version" section for this OS
                                        keepParsing = false;
                                    }
                                }
                            }    
                            */
                            
                            break;

                        case XmlNodeType.EndElement:
                            //when we reach the end of the "OS" Windows section, no use to keep parsing the macOS and linux sections
                            if (String.Equals(textReader.Name, "OS", StringComparison.Ordinal))
                            {
                                keepParsing = false;
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
            catch (System.Xml.XmlException e)
            {
                eIDViewer.BackendDataViewModel theBackendData = (BackendDataViewModel)(App.Current.Resources["eIDViewerBackendObj"]);
                theBackendData.WriteLog("error parsing xml version file: \n", eid_vwr_loglevel.EID_VWR_LOG_NORMAL);
                theBackendData.WriteLog(e.ToString() + "\n", eid_vwr_loglevel.EID_VWR_LOG_DETAIL);
                return false; //xml corrupt?
            }
            catch
            {
                return false; //xml corrupt?
            }
            return true; //no error
        }
    }
}

