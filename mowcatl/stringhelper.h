#ifndef StringHelper_H
#define StringHelper_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class StringHelper
//Purpose:	To provide some static functions that allow for easy string manipulation
//Author:	Matt Culver
//Last Modified:	26 July 2003
{
public:
	StringHelper(){}
	//Constructor
	static string extractXML(string XML, string tag, int occurance, bool unwrap);
	//Purpose - Extracts the portion of an XML string between XML tags
	//Author:	Matt Culver
	//Returns:	String - The data that was contained in the specified tags.  The empty
	//			string if the specified tags were not found
	//XML - The XML string to parse.
	//tag - The tag that we want the information from.
	//occurance - The instance of the specified tag that is returned.
	//unwrap - Whether to unwrap the text from its tags.
	static string extractXML(string XML, int &start, string tag, bool unwrap);
	//Purpose - Extracts the portion of an XML string between XML tags
	//Author:	Matt Culver
	//Returns:	String - The data that was contained in the specified tags.  The empty
	//			string if the specified tags were not found
	//XML - The XML string to parse.
	//tag - The tag that we want the information from.
	//start - The location in the string to start looking for the tag
	//unwrap - Whether to unwrap the text from its tags.
	static void trim(string &target);
	//Purpose - To trim all white space off of the begining and end of a string
	//Author:	Matt Culver
	//Returns:	Nothing - The target string is passed by reference
	//target - The string to trim
	static void trim(string &target, string charlist);
	//Purpose - To trim all characters specified in charlist from the 
	//	begining and end of the target string
	//Author:	Matt Culver
	//Returns:	Nothing - The target string is passed by reference
	//target - The string to trim
	//charlist - A list of the characters that need to be trimed from the target
	static int occurs(string target, string lookfor);
	//Purpose - To count how many times the lookfor string occurs in the target string
	//Author:	Matt Culver
	//Returns:	integer - The number of time the lookfor string occured in the target string
	//target - The string that we are looking for the lookfor string in.
	//lookfor - The string that we are counting how many times it occurs in the target string
	static vector<string> parse(string str, string delim);
	//Purpose - To parse a delimited string into a vector of strings
	//Author:	Matt Culver
	//Returns:	A string vector that holds all of the elements that were paresed from
	//			the passed in string 
	//str - The string to parse
	//delim - The list of delimiters
};

#endif

