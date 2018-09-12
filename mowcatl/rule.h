/*
filename: rule.h
author: Sherri Harms
last modified: 11/20/00
description: Class rule interface file for episode rules.
  	Values of this type are a composite of 
	an episode antecedant and an episode consequent
	This implementation uses a vector an episode.
******************************************************************************
Known bugs:
  Serial episodes are NOT completed.
  The constraints are all disjuctive singular events.
  step 5 from alg 5 generate candidatges has not been implemented
  Time lag is not completed.
  input validation is not done
*******************************************************************************
Assumptions:
  Parameters have been set (done in the main program).  These include:

  Data file istream& fin has been opened.  It is a flat file with one record for each event.
     The record contains the time stamp followed by the event.
	 The file must be in increasing time order, but may include events at the same
	 time stamp.
  Parameters from the parameter file (in the order they must be specified):
		Sample: 5 .625 p 4 1 1 	read by: fin>>min_freq>>min_confidence>>type>>win>>step>>endpoints;

		int min_fr - the minimum frequency.  Only episodes that are at least this 
			 frequent will be kept from the list of candidate episodes. Must be > 0.
			 This is a raw count and must be greater than 0.
			 This program could be modified to use a percent of windows instead of the raw count.
		double min_confidence - the minimum confidence.  Only rules that are
			at least this confident will be kept from the list of Rep. Episodal AR. 
			Must be between 0 and 1.
		char type - the type of episodes we are looking for. Must have one of the values:
			p = parallel injunctive, Currently the only type implemented.
			q = parallel non-injunctive
			s = serial injunctive, 
			t = serial noninjunctive
		int win - the size of the sliding window.  Must be greater than 0.
		int step - the step of the timestamps.  Most files the step size will be 1.  
			Must be greater than 0 and < win.
		bool endpoints - true if you want to process using all the data, including the 
			beginning and ending windows that are not completely full.  For large data
			sets, not using the endpoints will give you a tighter closure.  This is 
			because the first event will be seen in a window by itself and thus
			its closure will be equal to itself. Must be 0 or 1.
  itemset& fcc_k_b will be initialized.  This is the current set of candidates
  itemset& fc[k] will keep the items from fcc_k_b that are frequent, 
		and whose closure has not been generated. 
 const event_set& F - the set of events possible. Read from a separate file, such as eventlist.txt.
 const event_set& B - the set of constraints.  Read from a separate file, such as constraints.txt.
     Currently, the constraints are singletons - any event not in the list
	 will not be included in the analysis.  This needs to be modified to allow
	 for constraints in DNF format.  
*******************************************************************************
Global Data structures:
class event - stores a event as a string.
class item - stores:
	 a vector of events (called an episode), 
	 a deque of events   (called the closure),
	 int freq - the number of windows this episode appears in
class p_item: inherits from item + stores:
	 int event_count - the current count of events within the episode that 
						are in the window - used in the counting process;
	 int in_window - the starting timestamp where the episode has all of its
	                    events in the window - used in the counting process;
class s_item: inherits from item + stores:
	 int in_window - the starting timestamp where the episode has all of its
	                    events in the window - used in the counting process;
	 vector[1 .. |episode|] of timestamps that show when the current
						part of the episode was initialized

class itemset - stores a vector of item pointers.
  also uses a static variable largest_item_size to
  keep track of the size of the largest item

class B_WIN - stores:
	event_deque B_win - the set of events that are currently in the window;
	int_deque time_stamp - the set of timestamps for the matching set of events;

class rule - stores a single episodal association rule, with:
	vector of events for the  ant;
	vector of events for the  cons;
	double confidence;
	double freq;

class ruleset - stores the set of Representative episodal association rules, with:
    vector of rules

Global variables:
vector<itemset> fcc - in main, used to store the sets of candidate items, 
	where each set contains items with the same number of events
vector<itemset> fc - in main, used to store the sets of closed items,
    where each set contains items with the same number of events in the episode, 
	but the closure sizes may be very different
  
Local Data Structures:
Local variables:
*****************************************************************************
rule operations include:
input, output, constructors, copy constructor, assignment operator, 
destructor, and relational operators, accessor functions
and operations to separate the antecedant and consequence
*******************************************************************************/

#ifndef RULE_H
#define RULE_H

#include "event.h"
#include <vector>
//#include <deque>
#include <algorithm>
#include <iostream>
using namespace std;

typedef vector<event> event_set;

class rule
{
public:

       void clear();

	void insert_ant(const event& e);
	//insert event e into the antecedant

	void insert_cons(const event& e);
	//insert event e into the consequence

	void insert(const event_set& a, double a_support, const event_set& c, double c_support, char type);
	//create a new rule from the antecedent a, and consequence c
	//with frequency = c_support
	//and confidence = c_support/a_support

	void insert(const event_set& a, double a_support, const event_set& c, double c_support, char type, const event_set& C);

	void remove_ant_from_cons();
	//remove the antecedant from the consequence

	bool ant_contains_event(const event& A) const;
	//true if the antecedent already contains event A

	bool cons_contains_event(const event& A) const;
	//true if the consequence already contains event A

	bool cons_empty() const;
	//true if the cons is empty

	void output(ostream& outs) const;
	void output_ant(ostream& outs) const;
	void output_cons(ostream& outs) const;

    friend bool operator ==(const rule& i1, const rule& i2);
    //Returns true if i1 and i2 represent the same rule; 
    //otherwise, returns false.

    friend bool operator !=(const rule& i1, const rule& i2);
    //Returns true if i1 != i2 ; otherwise, returns false.

    rule( );
    //Constructor.

    friend istream& operator >>(istream& ins, rule& the_object);
    //Overloads the >> operator for input values of type rule.
    //Precondition: If ins is a file input stream, then ins has 
    //already been connected to a file. 

    friend ostream& operator << (ostream& outs, const rule& the_object);
    //Overloads the << operator for output values of type rule.
    //Precondition: If outs is a file output stream, then outs as 
    //already been connected to a file.
private:

	event_set ant;
	event_set cons;
	double confidence;
	double freq;
	double coverage;
};


#endif //rule_H



