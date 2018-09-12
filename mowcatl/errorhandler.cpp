#include "errorhandler.h"
#include "conversions.h"


ErrorHandler::ErrorHandler(){
	error = false;
}

ErrorHandler::ErrorHandler(string rTag){
	error = false;
	rootTag = rTag;
}

void ErrorHandler::addError(string message, int code){
	errorMsgs.push_back(message);
	errorCodes.push_back(code);
	error = true;
}

bool ErrorHandler::hasError(){
	return error;
}

string ErrorHandler::toXML(){
	string errorXML, rTag, errorCode;
	int i;

	if(rootTag == "")
		rTag = "Results";
	else
		rTag = rootTag;

	errorXML = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
	errorXML += "<"+rTag+"><Status>FAILURE</Status><Errors>";

	for(i=0;i<errorMsgs.size();i++){
		errorCode = convertitoa(errorCodes[i]);

		errorXML += "<Error>";
		errorXML += "<Message>"+ errorMsgs[i] + "</Message>";
		errorXML += "<Code>" + errorCode + "</Code>";
		errorXML += "</Error>";
	}

	errorXML += "</Errors></"+rTag+">";

	return errorXML;
}

void ErrorHandler::setRootTag(string rTag){
	rootTag = rTag;
}

