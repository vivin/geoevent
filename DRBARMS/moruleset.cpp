/*
filename: moruleset.cpp
author: Sherri Harms
last modified: 7/23/01
description: Class moruleset implementation file for MINEPI REAR rules 
  	Values of this type are vectors of rules,	where a morule is a composite of 
	an antecedent episode, and a consequent episode, with a confidence;
******************************************************************************
Known bugs:
  The constraints are all disjuctive singular events.
  step 5 from alg 5 generate candidatges has not been implemented
  input validation is not done
*******************************************************************************
Local Data Structures:
Local variables:
*****************************************************************************
moruleset operations include:
input, output, constructors, 
relational operators, accessor functions
and operations to adds rules, etc
*******************************************************************************/

#pragma warning(disable:4786)

#include "event.h"
#include "morule.h"
#include "moruleset.h"
#include <vector>
#include <algorithm>
#include <iomanip>
#include <iostream>
using namespace std;

void moruleset::cleanup()
{
	try
	{
		p_rule_set::iterator r = rs.begin();
		while (r != rs.end())
		{
			r->clear();
//			delete r; r=0; //items are inserted into vectors, without using new, so we don't delete
			r++;
		}
		clear();
	} //end try
	catch(...)
	{
//		cout<<"exception occurred"<<endl;
	}

}


void moruleset::clear()
{
	rs.erase(rs.begin(), rs.end());
	count = 0;
}

int moruleset::AccessCount() const
{
	return count;
}

void moruleset::insertrule(const morule& in_rule)
{
	rs.push_back(in_rule);
	count++;
}

morule moruleset::get_first_rule() const
{
	morule i = *rs.begin();
	return i;
}

bool moruleset::empty() const
{
	return rs.empty();
}

bool moruleset::contain(const morule& in_rule) const
{
	bool found = false;
	p_rule_set::const_iterator r = rs.begin();
	while (r != rs.end() && !found)
	{
		if (in_rule == *r)
			found = true;
		r++;
	}
	return found;
}

void moruleset::output(ostream& outs,bool rar) const
{
	outs<<endl;
	outs<<"| ";
	if (rar)
		outs<<setw(14)<<"Representative";
	else
		outs<<setw(14)<<"All";
	outs<<" Parallel Rules";
		//modified 3/21/02 last occurrence
	outs<<" | "<<"Last Occurred"
		<<" | "<<"Confidence"
		<<" | "<<"Support"
		<<" | "<<"Coverage"
		<<" | "<<"Cons Freq"
		<<" | "<<"J-Measure"
		<<" |"<<endl;
	outs<<"---------------------------------------------------------------------------------------------------------"<<endl;

	p_rule_set::const_iterator i= rs.begin();
	while (i != rs.end())
	{
		(*i).output(outs);
		++i;
	}
	outs<<"---------------------------------------------------------------------------------------------------------"<<endl;
}

string moruleset::outputXML(char type) const
//Purpose: To add all of the rules generated by the algorithm to an
//		XML string for output
//Author: Matt Culver
//Last Modified: 6 July 2003
//type - The type of episode that the algorthm was looking for
{
	string ruleListXML;
	
	ruleListXML = "<RuleList>";
	p_rule_set::const_iterator i= rs.begin();
	while (i != rs.end())
	{
		ruleListXML += (*i).outputXML(type);
		++i;
	}
	ruleListXML += "</RuleList>";

	return ruleListXML;
}

// Dan Li, Oct29
void moruleset::output(ostream& outs,char type, bool rar) const
{
	outs<<endl;
	outs<<"| ";
	if (rar)
		outs<<setw(14)<<"Representative";
	else
		outs<<setw(14)<<"All";
	outs<<" Parallel Rules";
		//modified 3/21/02 last occurrence
	outs<<" | "<<"Last Occurred"
		<<" | "<<"Confidence"
		<<" | "<<"Support"
		<<" | "<<"Coverage"
		<<" | "<<"Cons Freq"
		<<" | "<<"J-Measure"
		<<" |"<<endl;
	outs<<"---------------------------------------------------------------------------------------------------------"<<endl;

	p_rule_set::const_iterator i= rs.begin();
	while (i != rs.end())
	{
		(*i).output(outs, type);
		++i;
	}
	outs<<"---------------------------------------------------------------------------------------------------------"<<endl;
}


moruleset::moruleset( )
{
	morule empty;
	rs.push_back(empty);
	rs.clear();
	count = 0;
}
    //Constructor.



