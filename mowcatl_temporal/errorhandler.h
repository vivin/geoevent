#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <vector>
#include <string>

using namespace std;

class ErrorHandler{
public:
	ErrorHandler();
	ErrorHandler(string rTag);

	//string convertitoa(int input);
	void addError(string message, int code);
	bool hasError();
	string toXML();
	void setRootTag(string rTag);

private:
	bool error;
	vector<string> errorMsgs;
	vector<int> errorCodes;
	string rootTag;
	//error:	Whether an error has occured
	//errorMsgs:	A vector of error messages for each error that has occured
	//errorCodes:	A vector of error codes for each error that has occured
	//rootTag:		The outermost tag when the contents of this object are writen as an XML string
};

#endif
