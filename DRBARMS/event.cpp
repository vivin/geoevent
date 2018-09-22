/*
filename: event.cpp
author: Sherri Harms
last modified: 7/16/01
description: Class event implementatino file for episodes.
  	Values of this type are a string.
******************************************************************************
Known bugs:
  The constraints are all disjuctive singular events.
  step 5 from alg 5 generate candidates has not been implemented (constraints with conjunctions)
  Time lag is not completed.
  input validation is not done
*******************************************************************************
Assumptions:
See the documentation.cpp file for system documentation.
*******************************************************************************
Local Data Structures:
Local variables: string e
*****************************************************************************
event operations include:
input, output, constructors, 
relational operators, accessor functions, get length, and matchB
*******************************************************************************/
#include "event.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;


event::event(const event& e1)
{
	if (&e1 != this)
		e = e1.e;
}

const event& event::operator = (const event& e1)
{
	if (&e1 != this)
		e = e1.e;
	return *this;
}

void event::clear()
{
	e="";
}

string event::getEvent() const{
	return e;
}

int event::get_length() const
{
	return e.length();
}

/*event::~event()
{delete this;}
*/

bool event::matchB(const event_set& B) const
{
	bool constraint = true;
		if (!binary_search(B.begin(), B.end(), *this) )
		{	//found an event that does not meet the constraints
			constraint = false;
		} //if
return constraint;
} //matchB
