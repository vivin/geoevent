/*
 *  Mowcatl.cpp
 *  Mowcatl
 *
 *  Created by UNK, Sherri Harms, harmssk@unk.edu Wed Feb 09 2005.
 *  Copyright (c) 2004 __MyCompanyName__. All rights reserved.
 *
 */
#include <iostream>
#include "mowcatl.h"
#include "xmlintface.h"
using namespace std;

string Mowcatl::getMowcatlResults( string inputXML )
{
	 XMLIntFace XMLIF;
	return XMLIF.runMo(inputXML);
//	cout<<results<<endl;
//	return inputXML;

}

