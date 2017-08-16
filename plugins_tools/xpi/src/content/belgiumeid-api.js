if(!be) var be={};
if(!be.fedict) be.fedict={};
if(!be.fedict.belgiumeid) be.fedict.belgiumeid={};
if(!be.fedict.belgiumeid.exists) be.fedict.belgiumeid.exists = true;
if(!be.fedict.belgiumeid.getVersion) be.fedict.belgiumeid.getVersion = function() {
	// TODO: use content scripts and similar things to query version from
	// actual add-on. Later.
	return "1.0.24";
};
