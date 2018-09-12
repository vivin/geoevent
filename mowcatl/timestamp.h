/*
filename: timestamp.h
author: Sherri Harms
last modified: 8/8/01
description: Class time_stamp interface file for serial episodes.
  Values of this type is a list of events 

  Used in computing serial episodes and closures

  A single serial moitem is a composite of several timestamps.  That is, within a serial
  moitem, there are many timestamps that are in serial order.  Within a single timestamp,
  there are many events, and because they all occur at the same timestamp,
  no order is specified. 
******************************************************************************
Known bugs:
  The constraints are all disjuctive singular events.
  step 5 from alg 5 generate candidatges has not been implemented
  Time lag is not completed.
  input validation is not done
*******************************************************************************
Assumptions:
*******************************************************************************
Global Data structures:
*******************************************************************************
Local Data Structures:
Local variables:
*****************************************************************************
time_stamp operations include:
input, output, constructors, and relational operators
*******************************************************************************/

#ifndef time_moitem_H
#define time_moitem_H

#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <deque>
#include "event.h"
using namespace std;
class event; //forward declaration
class moitemset;

typedef vector<event> event_set;  //for matching constraints B
typedef deque<event> event_deque; //used for creating closure from B_win
typedef list<event> event_list;
typedef list<event>::iterator  event_list_ITR;
typedef list<event>::const_iterator  const_event_list_ITR;

class time_stamp
{
public:
typedef vector<time_stamp> t_stamp_vec;
typedef deque<time_stamp> t_stamp_q;
typedef vector<time_stamp>::iterator  t_stamp_vec_ITR;
typedef vector<time_stamp>::const_iterator  const_t_stamp_vec_ITR;
typedef deque<time_stamp>::iterator  t_stamp_q_ITR;
typedef deque<time_stamp>::const_iterator  const_t_stamp_q_ITR;

	event_list get_time_stamp() const; //access moitem

	event get_first_event() const; //access moitem
	event_deque convert_time_stamp() const;//access moitem

	friend class moitemset;
friend bool operator ==(const time_stamp& t1, const time_stamp& t2)
{	return ( t1.t_stamp == t2.t_stamp);}

friend bool operator <(const time_stamp& t1, const time_stamp& t2)
//Returns true if t1 < t2 ; otherwise, returns false.
{	return ( t1.t_stamp < t2.t_stamp);}

friend bool operator >(const time_stamp& t1, const time_stamp& t2)
//Returns true if t1 > t2 ; otherwise, returns false.
{	return ( t1.t_stamp > t2.t_stamp);}

friend bool operator <=(const time_stamp& t1, const time_stamp& t2)
//Returns true if t1 <= t2 ; otherwise, returns false.
{	return ( t1.t_stamp <= t2.t_stamp);}

friend bool operator >=(const time_stamp& t1, const time_stamp& t2)
//Returns true if t1 >= t2 ; otherwise, returns false.
{	return ( t1.t_stamp >= t2.t_stamp);}

friend bool operator !=(const time_stamp& t1, const time_stamp& t2)
//Returns true if t1 != t2 ; otherwise, returns false.
{	return ( t1.t_stamp != t2.t_stamp);}

friend ostream& operator << (ostream& outs, const time_stamp& the_object)
//Overloads the << operator for output values of type time_stamp.
//Precondition: If outs is a file output stream, then outs as 
//already been connected to a file.
{
	outs<<"(";
	event_list::const_iterator e = the_object.t_stamp.begin();
	if (e != the_object.t_stamp.end())
	{
		outs<<*e;
		++e;
	}
	while (e != the_object.t_stamp.end())
	{
		outs<<", ";
		outs<<*e;
		++e;
	}
	outs<<")";
	return outs;

}



    time_stamp( ) ;
    //Initializes the time_stamp member variables.

	time_stamp(const event& in_event); //create a timestamp with the initial event

	time_stamp(const event_deque& in_events);

	t_stamp_q makecombinations() const;

	void insert(const event& in_event); //insert the in_event into the current timestamp 

	void clear();

	bool empty () const;

	bool issingle() const;

	bool doescontain(const time_stamp& smaller) const; 
	//if true, the calling timestamp list of events contains the smaller list of events
	// if false, it does not contain it, or the smaller list is really not smaller
	// true if the lists are identical also

	bool doescontainany(const time_stamp& smaller) const; 
	//true if the calling timestamp list of events contains at least one of the events]
	// in the smaller timestamp list

	time_stamp intersect(const time_stamp& in_window) const;

	void sort();
	
	time_stamp foundat(const t_stamp_q& convertedB_win) const;
	//return the complete time_stamp where the calling timestamp is found in the que
	
	time_stamp makeunique() const;

	void addeventstoset(event_set& s) const;
	//add the events in the eventlist to the eventset s
	//used in the getepisode accessor function
	
//	bool doescontain(const event& e); 
	//if true, the calling timestamp list of events contains the event e

	bool matchB(const event_set& B) const;
	//true if the events in the timestamp are in the constraint set B

	void removeifdoesnotmatchB(const event_set& B);
	//remove the events in the timestamp that are not in the constraint set B

	time_stamp get_sub1(int k) const;
	time_stamp get_sub2(int k) const;

	int get_length() const;
	int get_number_events() const;

	string get_events() const;
	//Purpose: to return the list of events in this time stamp as a comma 
	//		delimited string

	void output(ostream& outs) const;
private:
	event_list t_stamp;
};

#endif //time_stamp_H
