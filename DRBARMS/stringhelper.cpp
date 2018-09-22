#include "stringhelper.h"
#include <string.h>
using namespace std;

string StringHelper::extractXML(string XML, string tag, int occurance, bool unwrap){
//Purpose - Extracts the portion of an XML string between XML tags
//Author:	Matt Culver
//Returns:	String - The data that was contained in the specified tags.  The empty
//			string if the specified tags were not found
//XML - The XML string to parse.
//tag - The tag that we want the information from.
//occurance - The instance of the specified tag that is returned.
//unwrap - Whether to unwrap the text from its tags.
	string startTag, endTag, returnStr="";
	int sTLen, eTLen, startIndex=0, endIndex, i;
	bool notFound = false;
	//startTag:	The XML start tag that we are looking for
	//endTag:	The XML end tag that we are looking for
	//returnStr:	The string that is returned that contains the data that was 
	//				between the specified tags.  Wraped in tags if unwrap is false
	//sTLen:	The number of characters in the start tag
	//eTLen:	The number of characters in the end tag
	//startIndex:	The index of the start tag in the XML string
	//endIndex:	The index of the end tag in the XML string
	//i:	An integer for iterating through for loops
	//notFound:	Whether the specified tags were found

	//remove any white space, angle brackets or slashes from the tag that we are looking for
	StringHelper::trim(tag," \t\n<>/");

	//make the start and end tags that we will look for
	startTag = "<"+tag+">";
	endTag = "</"+tag+">";
	//get the length of the start and end tags
	sTLen = startTag.length();
	eTLen = endTag.length();
	
	//look for the specefied occurance of the start tag
	for(i=0;i<occurance;i++){
		startIndex = XML.find(startTag,startIndex);
		if(startIndex == string::npos){
			notFound = true;
			break;
		}
		startIndex++;
	}

	//if the start tag was found look for its matching end tag
	if(!notFound){
		startIndex += sTLen-1;
		endIndex = XML.find(endTag,startIndex);
		//if the end tag was found get that data that is enclsed in the tags
		if(endIndex != string::npos)
			returnStr = XML.substr(startIndex,endIndex-startIndex);
	}

	//trim the result 
	StringHelper::trim(returnStr);

	//if we don't want to unwrap the data from its tags then put them back on
	if(!unwrap){
		returnStr = startTag+returnStr+endTag;
	}
	
	return returnStr;
}

string StringHelper::extractXML(string XML, int &start, string tag, bool unwrap){
//Purpose - Extracts the portion of an XML string between XML tags
//Author:	Matt Culver
//Returns:	String - The data that was contained in the specified tags.  The empty
//			string if the specified tags were not found
//XML - The XML string to parse.
//tag - The tag that we want the information from.
//start - The location in the string to start looking for the tag
//unwrap - Whether to unwrap the text from its tags.
	string startTag, endTag, returnStr="";
	int sTLen, eTLen, startIndex, endIndex, end;
	bool notFound = false;
	//startTag:	The XML start tag that we are looking for
	//endTag:	The XML end tag that we are looking for
	//returnStr:	The string that is returned that contains the data that was 
	//				between the specified tags.  Wraped in tags if unwrap is false
	//sTLen:	The number of characters in the start tag
	//eTLen:	The number of characters in the end tag
	//startIndex:	The index of the start tag in the XML string
	//endIndex:	The index of the end tag in the XML string
	//end:		The index in the XML string where the end tag ends
	//notFound:	Whether the specified tags were found

	//remove any white space, angle brackets or slashes from the tag that we are looking for
	StringHelper::trim(tag," \t\n<>/");

	//make the start and end tags that we will look for
	startTag = "<"+tag+">";
	endTag = "</"+tag+">";
	//get the length of the start and end tags
	sTLen = startTag.length();
	eTLen = endTag.length();

	//get the location of the start tag
	startIndex = XML.find(startTag,start);
	if(startIndex == string::npos){
		notFound = true;
	}
	startIndex++;

	//if the start tag was found the find the end tag
	if(!notFound){
		startIndex += sTLen-1;
		endIndex = XML.find(endTag,startIndex);
		//if the end tag was found then get the data that is encapsulated by the tags
		if(endIndex != string::npos){
			returnStr = XML.substr(startIndex,endIndex-startIndex);
			end = endIndex + eTLen;
			StringHelper::trim(returnStr);
		}else
			end = string::npos;
	}else
		end = string::npos;

	//if we don't want to unwrap the data put the tags back on
	if(!unwrap){
		returnStr = startTag+returnStr+endTag;
	}

	//set the starting location to the end location so we know where to start looking
	//the next time this method is called
	start = end;

	return returnStr;
}

void StringHelper::trim(string &target){
//Purpose - To trim all white space off of the begining and end of a string
//Author:	Matt Culver
//Returns:	Nothing - The target string is passed by reference
//target - The string to trim
	StringHelper::trim(target," \t\n");
}

void StringHelper::trim(string &target, string charlist){
//Purpose - To trim all characters specified in charlist from the 
//	begining and end of the target string
//Author:	Matt Culver
//Returns:	Nothing - The target string is passed by reference
//target - The string to trim
//charlist - A list of the characters that need to be trimed from the target
	int begin, length, index, i;
	bool done = false, charFound = false;
	//begin:	The begining index of the data that we want to keep from the target string
	//lenght:	The lenght of the data that we want to keep from the beging position
	//index:	The index in the target string that we are looking at
	//i:		An integer for iterating through for loops
	//done:		Whether we are done looking for the start or end of the data that we want 
	//			to keep
	//charFound:	Whether a character in the target string matches one of the characters
	//				in the charlist

	index = 0;

	//look for the location of the first charater we want to keep in the target string 
	while(!done){
		for(i=0;i<charlist.length();i++){
			//look at each character and determine whether it is in the list of 
			//characters to trim
			if(target[index] == charlist[i]){
				charFound = true;
			}
		}
		//if the character was found in the charlist move to the next character
		//if it is not found then get out of the loop.  This is the begin position
		if(charFound)
			index++;
		else
			done = true;
		charFound = false;
	}
	begin = index;
	done = false;
	index = target.length()-1;
	
	//now start at the end of the string working in reverse to find the last charater
	//in the target string that we want to keep
	while(!done){
		for(i=0;i<charlist.length();i++){
			if(target[index] == charlist[i]){
				charFound = true;
			}
		}
		if(charFound)
			index--;
		else
			done = true;
		charFound = false;
	}
	length = index - begin + 1;

	//get the portion of the target string that we want to keep and 
	//reassign it to the target string
	target = target.substr(begin,length);
}

int StringHelper::occurs(string target, string lookfor){
//Purpose - To count how many times the lookfor string occurs in the target string
//Author:	Matt Culver
//Returns:	integer - The number of time the lookfor string occured in the target string
//target - The string that we are looking for the lookfor string in.
//lookfor - The string that we are counting how many times it occurs in the target string
	int count = 0, index = 0;
	bool atEnd = false;
	//count:	Keeps track of the number of times we found the lookfor string in 
	//			the target string '
	//index:	The index in the target string that we are at
	//atEnd:	Whether we have reached the end of the target string 

	while(!atEnd){
		//look for an occurance of the target string after the current index
		index = target.find(lookfor,index);

		//if it was found increment the counter.  Other wise get out of the loop
		//and return the count
		if(index != string::npos){
			index++;
			count++;
		}else{
			atEnd = true;
		}
	}
	return count;
}

vector<string> StringHelper::parse(string str, string delim){
//Purpose - To parse a delimited string into a vector of strings
//Author:	Matt Culver
//Returns:	A string vector that holds all of the elements that were paresed from
//			the passed in string 
//str - The string to parse
//delim - The list of delimiters

	char* tokenPtr;
	vector<string> strings;
	char* cstr = (char *)str.c_str();	
	char* cdelim = (char*)delim.c_str();
	string thestr;
	//todenPtr:	character array used to tokenize the string
	//strings:	The vector of strings that holds the elements that were parse from 
	//			the passed in delimited string
	//cstr:		A charcter array of the passed in delimited string
	//cdelim:	A character array of the passed in string of delimiters
	//thestr:	A single element that is parsed from the delimited string

	tokenPtr=strtok(cstr,cdelim);
	while(tokenPtr!=NULL){
		thestr = *new string(tokenPtr);
		strings.push_back(thestr);	
		tokenPtr=strtok(NULL,cdelim);
	}
	return strings;
}
