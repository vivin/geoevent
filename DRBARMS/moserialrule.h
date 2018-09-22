/*
filename: moserialrule.h
author: Sherri Harms
last modified: 07/13/01
description: Class mos_rule interface file for serial episode rules.
  	Values of this type are a composite of 
	an episode antecedant and an episode consequent
	This implementation uses a timestamp vector for episode.
******************************************************************************
Local Data Structures:
Local variables:
*****************************************************************************
mos_rule operations include:
input, output, constructors, copy constructor, assignment operator, 
destructor, and relational operators, accessor functions
and operations to separate the antecedant and consequence
*******************************************************************************/

#ifndef mosRULE_H
#define mosRULE_H

#include "event.h"
#include "timestamp.h"
#include "occur.h"
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

typedef vector<event> event_set;
typedef vector<time_stamp> t_stamp_vec;
typedef vector<time_stamp>::iterator  t_stamp_vec_ITR;
typedef vector<time_stamp>::const_iterator  const_t_stamp_vec_ITR;

class mos_rule
{
public:

    void clear();
	double get_j_measure() const;
	// Dan Li Oct29
	double get_j_measure(char type) const;

	void insert_ant(const time_stamp& t);
	//insert time_stamp t into the antecedant

	void insert_cons(const time_stamp& t);
	//insert time_stamp t into the consequence

	void insert(const t_stamp_vec& a, double a_support, const t_stamp_vec& c, double rule_support, const occur last, double c_support);
	//create a new mos_rule from the antecedent a, and consequence c
	//with frequency = rule_support
	//and confidence = rule_support/a_support

//	void remove_ant_from_cons();
	//remove the antecedant from the consequence

	bool ant_contains_time_stamp(const time_stamp& A) const;
	//true if the antecedent already contains time_stamp A

	bool cons_contains_time_stamp(const time_stamp& A) const;
	//true if the consequence already contains time_stamp A

	bool cons_empty() const;
	//true if the cons is empty

	string outputXML(char type) const;
	//Purpose: To output the rule as an XML string
	//Author: Matt Culver
	//Last Modified: 7 July 2003
	//type - The type of episode that the algorithm was looking for

	void output(ostream& outs) const;

	// Dan Li Oct29
	void output(ostream& outs, char type) const;

	string output_antXML() const;
	//Purpose: To output the anticedents of this rule as an XML string
	//Author: Matt Culver
	//Last Modified: 7 July 2003

	void output_ant(ostream& outs) const;

	string output_consXML() const;
	//Purpose: To output the consequents of this rule as an XML string
	//Author: Matt Culver
	//Last Modified: 7 July 2003

	void output_cons(ostream& outs) const;

friend bool operator ==(const mos_rule& r1, const mos_rule& r2)
//Returns true if i1 and i2 represent the same mos_rule; 
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
}

friend bool operator !=(const mos_rule& r1, const mos_rule& r2)
//Returns true if i1 != i2 ; otherwise, returns false.
{return !(r1==r2);}


    mos_rule( );
    //Constructor.

 //   friend istream& operator >>(istream& ins, mos_rule& the_object);
    //Overloads the >> operator for input values of type mos_rule.
    //Precondition: If ins is a file input stream, then ins has 
    //already been connected to a file. 

	 friend ostream& operator << (ostream& outs, const mos_rule& the_object)
    //Overloads the << operator for output values of type mos_rule.
    //Precondition: If outs is a file output stream, then outs as 
    //already been connected to a file.
{
	the_object.output_ant(outs);
	outs<<"  ==>  ";
	the_object.output_cons(outs);
	outs.setf(ios::showpoint);
	outs.setf(ios::fixed);
	outs.precision(0);
	outs<<", frequency = ";
	outs<<int(the_object.support);
	outs.precision(2);
	outs<<", confidence = ";
	outs<<the_object.confidence;
	return outs;
}

private:

	t_stamp_vec ant;
	t_stamp_vec cons;
	double confidence;
	double support;
	double coverage;
	double consequent_support;
	occur last_occurrence;
};


#endif //rule_H



