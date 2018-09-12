/*
filename: occur.cpp
author: Sherri Harms
last modified: 7/20/01
description: Class occur implementatino file for episodes.
  	Values of this type are a int start and int stop times.
******************************************************************************
Known bugs:
  The constraints are all disjuctive singular occurs.
  input validation is not done
*******************************************************************************
Assumptions:
See the documentation.cpp file for system documentation.
*******************************************************************************
Local Data Structures:
Local variables: int start, int stop
*****************************************************************************
occur operations include:
input, output, constructors, 
relational operators, accessor functions
*******************************************************************************/
#include "occur.h"
#include <iostream>
#include <fstream>
using namespace std;

occur::occur(const occur& o1)
{
	if (&o1 != this)
	{
		start = o1.start;
		stop = o1.stop;
	}
}

const occur& occur::operator = (const occur& o1)
{
	if (&o1 != this)
	{
		start = o1.start;
		stop = o1.stop;
	}
	return *this;
}

/*occur::~occur()
{
	delete this;
}*/

int occur::get_start() const
{return start;}

int occur::get_stop() const
{return stop;}

