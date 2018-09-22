/*
filename: morule.h
author: Sherri Harms
last modified: 08/06/01
description: Class morule interface file for parallel CTMINEPI episode rules.
  	Values of this type are a composite of 
	an episode antecedant and an episode consequent
	This implementation uses a vector of events for an episode.
******************************************************************************
 
Local Data Structures:
Local variables:
*****************************************************************************
morule operations include:
input, output, constructors, copy constructor, assignment operator, 
destructor, and relational operators, accessor functions
and operations to separate the antecedant and consequence
*******************************************************************************/
#ifndef moRULE_H
#define moRULE_H

#include "event.h"
#include "occur.h"
#include "stringhelper.h"
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

typedef vector<event> event_set;

class morule
{
public:

    void clear();
	double get_j_measure() const;

	// Dan Li, Oct29
	double get_j_measure(char type) const;

	void insert_ant(const event& e);
	//insert event e into the antecedant

	void insert_cons(const event& e);
	//insert event e into the consequence

	void insert(const event_set& a, double a_support, const event_set& c, double rule_support, const occur last, double c_support, double density);
	//create a new morule from the antecedent a, and consequence c
	//with frequency = rule_support
	//and confidence = rule_support/a_support

//	void remove_ant_from_cons();
	//remove the antecedant from the consequence

	bool ant_contains_event(const event& A) const;
	//true if the antecedent already contains event A

	bool cons_contains_event(const event& A) const;
	//true if the consequence already contains event A

	bool cons_empty() const;
	//true if the cons is empty

	string outputXML(char type) const;
	//Purpose: To output the rule as an XML string
	//Author: Matt Culver
	//Last Modified: 6 July 2003
	//type - The type of episode that the algorithm was looking for

	void output(ostream& outs) const;
	// Dan Li, Oct29
	void output(ostream& outs, char type) const;

	string output_antXML() const;
	//Purpose: To output the anticedents of this rule as an XML string
	//Author: Matt Culver
	//Last Modified: 6 July 2003

	void output_ant(ostream& outs) const;

	string output_consXML() const;
	//Purpose: To output the consequents of this rule as an XML string
	//Author: Matt Culver
	//Last Modified: 6 July 2003

	void output_cons(ostream& outs) const;

friend	bool operator ==(const morule& r1, const morule& r2)
//Returns true if i1 and i2 represent the same morule; 
//otherwise, returns false.
{
		bool equal = true;

	int i =0;
	if ( r1.ant.size() != r2.ant.size() || r1.cons.size() != r2.cons.size())
		equal = false;

	while (i<r1.ant.size() && equal)
	{
		if (r1.ant[i] != r2.ant[i])
			equal = false;
		else
			i++;
	}

	i = 0;
	while (i<r1.cons.size() && equal)
	{
		if (r1.cons[i] != r2.cons[i])
			equal = false;
		else
			i++;
	}
return equal;
/*
	for (event_set::const_iterator i = r1.ant.begin(); i != r1.ant.end(); i++)
	{
		event E = *i;
		if (!r2.ant_contains_event(E))
			return false;
	}
	for (i = r1.cons.begin(); i != r1.cons.end(); i++)
	{
		event E = *i;
		if (!r2.cons_contains_event(E))
			return false;
	}
	for (i = r2.ant.begin(); i != r2.ant.end(); i++)
	{
		event E = *i;
		if (!r1.ant_contains_event(E))
			return false;
	}
	for (i = r2.cons.begin(); i != r2.cons.end(); i++)
	{
		event E = *i;
		if (!r1.cons_contains_event(E))
			return false;
	}
	return true;*/
}

friend bool operator !=(const morule& r1, const morule& r2)
//Returns true if i1 != i2 ; otherwise, returns false.
{return !(r1==r2);}


friend istream& operator >>(istream& ins, morule& the_object)
    //Overloads the >> operator for input values of type morule.
    //Precondition: If ins is a file input stream, then ins has 
    //already been connected to a file. 
{
	return ins;
}

friend ostream& operator << (ostream& outs, const morule& the_object)
    //Overloads the << operator for output values of type morule.
    //Precondition: If outs is a file output stream, then outs as 
    //already been connected to a file.
{
	the_object.output_ant(outs);
	outs<<"  ==>  ";
	the_object.output_cons(outs);
	outs.setf(ios::showpoint);
	outs.setf(ios::fixed);
	outs.precision(0);
	outs<<", support = ";
	outs<<int(the_object.support);
	outs.precision(2);
	outs<<", confidence = ";
	outs<<the_object.confidence;
	return outs;
}


    morule( );
    //Constructor.

private:

	event_set ant;
	event_set cons;
	double confidence;
	double support;
	double coverage;
	double consequent_support;
	occur last_occurrence;
	double density;
};


#endif //rule_H



