/*
filename: mos_rule.cpp
author: Sherri Harms
last modified: 08/01/01
description: Class mos_rule implementation file for serial episode rules.
  	Values of this type are a composite of 
	an episode antecedant and an episode consequent
	This implementation uses a vector of time_stamps as an episode.
******************************************************************************
Local Data Structures:
Local variables:
*****************************************************************************
mos_rule operations include:
input, output, constructors, copy constructor, assignment operator, 
destructor, and relational operators, accessor functions
and operations to separate the antecedant and consequence
*******************************************************************************/
#include "moserialrule.h"
#include "moitemset.h"
#include "conversions.h"
#include <vector>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <math.h>
using namespace std;

void mos_rule::clear()
{	t_stamp_vec_ITR i = ant.begin();

	while(i != ant.end())
	{
		(*i).clear();
		i++;
	}
	t_stamp_vec_ITR c = cons.begin();

	while(c != cons.end())
	{
		(*c).clear();
		c++;
	}

	ant.clear();
	cons.clear();
}

void mos_rule::insert_ant(const time_stamp& t)
{
	ant.push_back(t);
}

void mos_rule::insert_cons(const time_stamp& t)
{
	cons.push_back(t);
}

/*void mos_rule::remove_ant_from_cons()
{
	time_stamp empty;
	t_stamp_vec newcons;
	const_t_stamp_vec_ITR aitr = ant.begin();
	const_t_stamp_vec_ITR citr = cons.begin();
	const_t_stamp_vec_ITR cenditr = cons.end();
	const_t_stamp_vec_ITR location = cons.begin();
	do
	{
		location = find(citr, cenditr, *aitr); 
		//starting location of the current time_stamp in the antecedent
		//with serial items, the location of the current time_stamp must follow the
		//location of the previous time_stamp
		//with parallel items, the events are lexigraphically sorted, so again the
		// location of the current time_stamp must follow the location of the previous time_stamp
		if (location != cons.end())
		{
			for (const_t_stamp_vec_ITR i = citr; i < location; i++)
				newcons.push_back(*i);
			citr = location;
			citr++;
			aitr++;
		}
		else
		{
			aitr++;
		}
	}	while (aitr != ant.end());

	//move the remaining events to the new cons
	for (const_t_stamp_vec_ITR i = citr; i < cons.end(); i++)
				newcons.push_back(*i);
	
	cons = newcons;
}  //end remove_ant_from_cons
*/

void mos_rule::insert(const t_stamp_vec& a, double a_support, const t_stamp_vec& c, double rule_support, const occur last, double c_support)
{
	for (const_t_stamp_vec_ITR i = c.begin(); i != c.end(); i++)
	{
		cons.push_back(*i);
	}

	for (const_t_stamp_vec_ITR j = a.begin(); j != a.end(); j++)
	{
		ant.push_back(*j);
	}
//	if (type == 'p' || type == 'q')
//		remove_ant_from_cons();

	support= rule_support;
	coverage = a_support;
	consequent_support = c_support;
	last_occurrence = last;
	if (a_support !=0)
		confidence = rule_support/a_support;
	else
	{
		cerr<<"Division by zero in computing the confidence of the mos_rule."<<endl;
		exit(1);
	}
}

/*void mos_rule::insert(const t_stamp_vec& a, double a_support, const t_stamp_vec& c, double rule_support, char type, const event_set& C)
{//Tsegaye said to insert the mos_rule if ANY of the antecedent constraints were met
	//if all antecedent constraints are not met, do not insert the mos_rule
	for (const_t_stamp_vec_ITR i = c.begin(); i != c.end(); i++)
	{
		if ((*i).matchB(C))
			cons.push_back(*i);
	}

	for (const_t_stamp_vec_ITR j = a.begin(); j != a.end(); j++)
	{
		ant.push_back(*j);
	}
	if (type == 'p' || type == 'q')
		remove_ant_from_cons();

	support= rule_support;
	coverage = a_support;
	if (a_support !=0)
		confidence = rule_support/a_support;
	else
	{
		cerr<<"Division by zero in computing the confidence of the mos_rule."<<endl;
		exit(1);
	}
}
*/

bool mos_rule::ant_contains_time_stamp(const time_stamp& A) const
{
	for (const_t_stamp_vec_ITR i = ant.begin(); i != ant.end(); i++)
	{
		if (A == (*i))
			return true;
	}
	return false;
}

bool mos_rule::cons_contains_time_stamp(const time_stamp& A) const
{
	for (const_t_stamp_vec_ITR i = cons.begin(); i != cons.end(); i++)
	{
		if (A == (*i))
			return true;
	}
	return false;
}

bool mos_rule::cons_empty() const
{
	return (cons.empty());
}


void mos_rule::output(ostream& outs) const
{
	outs.setf(ios::showpoint);
	outs.setf(ios::fixed);
	outs<<"| "; //mos_rule
	output_ant(outs);
	outs<<"  ==>  ";
	output_cons(outs);
	//modified 3/21/02
	outs<<" | "<<setw(8);  //last occurrence
	outs<<last_occurrence;
	outs<<" | "<<setw(9); //Confidence
	outs.precision(2);
	outs<<confidence;
	outs<<" | "<<setw(7); //Support
	outs.precision(0);
	outs<<int(support);
	outs<<" | "<<setw(8); //Coverage
	outs<<int(coverage);
	outs<<" | "<<setw(9); //Cons Freq
	outs<<int(consequent_support);
// Dan Li modify 9/10/2001	
	outs<<" | "<<setw(8); //J-measure
	outs.precision(5);
	outs<<get_j_measure();
	outs<<" |"<<endl;
}

string mos_rule::outputXML(char type) const
//Purpose: To output the rule as an XML string
//Author: Matt Culver
//Last Modified: 7 July 2003
//type - The type of episode that the algorithm was looking for
{
	string ruleXML;
	string convertedNum, convertedDouble = "";
	//char convertedInt[30];
	//int sign, decimal,i;
	
	ruleXML = "<Rule>";
	
	//add the anticedent list
	ruleXML += output_antXML();
	
	//add the consequent list
	ruleXML += output_consXML();

	//Last occurance
	ruleXML += "<LastOccur>";
	ruleXML += "<Start>";
	convertedNum = convertitoa(last_occurrence.get_start());
	StringHelper::trim(convertedNum," \0");
	ruleXML += convertedNum;
	ruleXML += "</Start>";
	ruleXML += "<Stop>";
	convertedNum = convertitoa(last_occurrence.get_stop());
	StringHelper::trim(convertedNum," \0");
	ruleXML += convertedNum;
	ruleXML += "</Stop>";
	ruleXML += "</LastOccur>";

	//confidence
	ruleXML += "<Confidence>";
	convertedNum = convertfcvt(confidence);
	ruleXML += convertedNum;
	ruleXML += "</Confidence>";
	
	//support
	ruleXML += "<Support>";
	convertedNum = convertitoa(int(support));
	StringHelper::trim(convertedNum," \0");
	ruleXML += convertedNum;
	ruleXML += "</Support>";

	//coverage
	ruleXML += "<Coverage>";
	convertedNum = convertitoa(int(coverage));
	StringHelper::trim(convertedNum," \0");
	ruleXML += convertedNum;
	ruleXML += "</Coverage>";

	//Consequent Support
	ruleXML += "<ConsequentSupport>";
	convertedNum = convertitoa(int(consequent_support));
	StringHelper::trim(convertedNum," \0");
	ruleXML += convertedNum;
	ruleXML += "</ConsequentSupport>";

	//J-Measure
	ruleXML += "<JMeasure>";
	convertedNum = convertfcvt(get_j_measure(type));
	StringHelper::trim(convertedNum,"0 \0");
	ruleXML += convertedNum;
	ruleXML += "</JMeasure>";

	ruleXML += "</Rule>";

	return ruleXML;
}

// Dan Li, Oct29 add one more parameter "type" to output
void mos_rule::output(ostream& outs, char type) const
{
	outs.setf(ios::showpoint);
	outs.setf(ios::fixed);
	outs<<"| "; //mos_rule
	output_ant(outs);
	outs<<"  ==>  ";
	output_cons(outs);
	//modified 3/21/02
	outs<<" | ";  //last occurrence
	outs<<setw(9)<<last_occurrence;
	outs<<" | "<<setw(10); //Confidence
	outs.precision(2);
	outs<<confidence;
	outs<<" | "<<setw(7); //Support
	outs.precision(0);
	outs<<int(support);
	outs<<" | "<<setw(8); //Coverage
	outs<<int(coverage);
	outs<<" | "<<setw(9); //Consequent support
	outs<<int(consequent_support);
// Dan Li modify Oct29	
	outs<<" | "<<setw(8); //J-measure
	outs.precision(5);
	outs<<setw(9)<<get_j_measure(type);
	outs<<" |"<<endl;
}

string mos_rule::output_antXML() const
//Purpose: To output the anticedents of this rule as an XML string
//Author: Matt Culver
//Last Modified: 7 July 2003
{
	string antListXML;
	string theAnt;

	antListXML = "<AntList>";

	const_t_stamp_vec_ITR i = ant.begin();
	while ( i != ant.end())
	{
		antListXML += "<Anticedent>";
		theAnt = (*i).get_events();
		StringHelper::trim(theAnt);  
		antListXML += theAnt;
		antListXML += "</Anticedent>";
		i++;
	}
	
	antListXML += "</AntList>";

	return antListXML;
}

void mos_rule::output_ant(ostream& outs) const
{
	int count = 0;
	const_t_stamp_vec_ITR i = ant.begin();
	while ( i != ant.end())
	{
		outs<<(*i);
		count= count+ (*i).get_length() + 2;  //add space for the parenthesis
		i++;
		if (i != ant.end())
		{
			outs<<", ";
			count+=2;
		}
	}
	while (count < 18)
	{
		outs<<" ";
		count++;
	}
}

string mos_rule::output_consXML() const
//Purpose: To output the consequents of this rule as an XML string
//Author: Matt Culver
//Last Modified: 7 July 2003
{
	string consListXML;
	string theCons;

	consListXML = "<ConsList>";

	const_t_stamp_vec_ITR i = cons.begin();
	while ( i != cons.end())
	{
		consListXML += "<Consequent>";
		theCons = (*i).get_events();
		StringHelper::trim(theCons);  
		consListXML += theCons;
		consListXML += "</Consequent>";
		i++;
	}
	
	consListXML += "</ConsList>";

	return consListXML;
}

void mos_rule::output_cons(ostream& outs) const
{
	int count = 0;
	const_t_stamp_vec_ITR i = cons.begin();
	while ( i != cons.end())
	{
		outs<<(*i);
		count= count+(*i).get_length()+2;
		i++;
		if (i != cons.end())
		{
			outs<<", ";
			count+=2;
		}
	}
		while (count < 21)
	{
		outs<<" ";
		count++;
	}
}


mos_rule::mos_rule( )
    //Constructor.
{
	time_stamp empty;
	ant.push_back(empty);
	cons.push_back(empty);
	ant.clear();
	cons.clear();
	confidence = 0;
	support = 0;
}


// Dan Li modify 9/16/2001
// calculate j-measure according to Klemettinen's Ph.D. Dissertation charpter4
double mos_rule::get_j_measure() const
{
	int num_records = moitemset::seenum_records();
	if (confidence == 1)
		return 1;
	else
		return ((coverage/num_records)*((confidence*(log(confidence/(consequent_support/num_records))/log(2.0)))
				+(1-confidence)*(log((1-confidence)/(1-consequent_support/num_records)))/log(2.0)));
}

//modified by Dan Li Oct29. Considering different episode type, the total number should be changed
double mos_rule::get_j_measure(char type) const
{
	int num_records = moitemset::seenum_records();
	int ant_total, cons_total;
	if (type == 'p' || type == 'q' || type == 't')
	{
		ant_total = num_records - (int)ant.size() + 1;
		cons_total = num_records - (int)cons.size() + 1;
	}
	else
	{
		ant_total = (int) (num_records/(int)ant.size());
		cons_total = (int) (num_records/(int)cons.size());
	}
	if (confidence == 1) 
		return ((coverage/ant_total)*(confidence*(log(confidence/(consequent_support/cons_total))/log(2.0))));
	else
		return ((coverage/ant_total)*((confidence*(log(confidence/(consequent_support/cons_total))/log(2.0)))
					+(1-confidence)*(log((1-confidence)/(1-consequent_support/cons_total))/log(2.0)))); 
}
