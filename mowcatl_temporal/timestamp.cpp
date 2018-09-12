/*
filename: time_stamp.cpp
author: Sherri Harms
last modified: 8/8/01
description: Class time_stamp implementation file for serial moitems.
  Values of this type is a list of events 

  Used in computing serial episodes and closures

  A single serial moitem is a composite of several timestamps.  That is, within a serial
  moitem, there are many timestamps that are in serial order.  Within a single timestamp,
  there are many events, and because they all occur at the same timestamp,
  no order is specified. 
******************************************************************************
Known bugs:

*******************************************************************************
Assumptions:
See the documentation file.
*******************************************************************************
Data structures:
t_stamp is a list of events
*******************************************************************************/

#include "timestamp.h"
#include <iostream>
#include <fstream>
using namespace std;

time_stamp::time_stamp( ) 
{
	event e;
	t_stamp.push_back(e);
	t_stamp.clear();
}
//Initializes the time_stamp member variables.

time_stamp::time_stamp(const event& in_event)
{
	t_stamp.clear();
	t_stamp.push_back(in_event);
}

time_stamp time_stamp::makeunique() const
{
	time_stamp newlist;
	for (const_event_list_ITR y= t_stamp.begin();y!= t_stamp.end();y++)
	{
		const_event_list_ITR in = find(newlist.t_stamp.begin(), newlist.t_stamp.end(), *y);
		if ( in == newlist.t_stamp.end() )
		{
			 newlist.t_stamp.push_back(*y);
		}
	}
	return newlist;
}

time_stamp time_stamp::foundat(const t_stamp_q& convertedB_win) const
//return the complete time_stamp where the calling timestamp is found in the que
{
	bool found = false;
	const_t_stamp_q_ITR q = convertedB_win.begin();
	while (!found && q != convertedB_win.end())
	{
		if ((*q).doescontain(*this))
			found = true;
		else
			q++;
	}
	if (found)
		return (*q);
	else
	{
		cerr<<"The timestamp ";
		output(cerr);
		cerr<<" is not found in the que convertedB_win"<<endl;
		cerr<<"Error in function foundat in timestamp class"<<endl;
		exit(1);
	}
}


deque<time_stamp> time_stamp::makecombinations() const
{
	t_stamp_q newlist;

	//temporarily put the events into a vector for easier combination processing
	event_set newevents;
	for (const_event_list_ITR y= t_stamp.begin();y!= t_stamp.end();y++)
	{
		newevents.push_back(*y);
	}

	int sources = t_stamp.size();
	int choose = 2;
	while (choose <= sources)
	{
		int pawn=choose-1;
		vector<int> index;
		for (int s =0;s<choose;s++)
			index.push_back(s);

		bool more_pawns_to_move = true;
		while (more_pawns_to_move)
		{
			bool more_moves_for_current_pawn=true;
			while (more_moves_for_current_pawn)
			{
				time_stamp temp;
				for (int start=0;start<choose;start++)
				{
					event e = newevents[index[start]];
					temp.insert(e);
				}
				newlist.push_back(temp);

				//increment the current pawn
				index[pawn]++;
				if (index[pawn] == sources)
					more_moves_for_current_pawn = false;
			} //end whilemore moves for current pawn

			int nextpawn = pawn;
			int max = sources;

			while ( nextpawn>=0 && (index[nextpawn] == max) )
			{
				nextpawn--;
				max--;
				if (nextpawn >=0)
					index[nextpawn]++;
			} 
			if (nextpawn<0)
				more_pawns_to_move = false;
			else
			{
				for (int j = nextpawn+1;j <= pawn;j++)
				{
					index[j] = index[j-1]+1;
				}//end for
			}//end else
		}	  //end while more pawns to move
		choose++;
	}//end while choose<sources

	//testing
	//for (t_stamp_q::const_iterator titr = newlist.begin(); titr != newlist.end(); titr++)
	//		cout<<*titr;
	//system("pause");
	return newlist;
}

void time_stamp::addeventstoset(event_set& s) const
	//add the events in the eventlist to the eventset s
	//used in the getepisode accessor function
{
	for (const_event_list_ITR y= t_stamp.begin();y!= t_stamp.end();y++)
	{
		s.push_back(*y);
	}
}

/*void time_stamp::addeventstoque(event_deque& s) const
	//add the events in the eventlist to the eventset s
	//used in the getepisode accessor function
{
	for (const_event_list_ITR y= t_stamp.begin();y!= t_stamp.end();y++)
	{
		s.push_back(*y);
	}
}*/

void time_stamp::removeifdoesnotmatchB(const event_set& B)
{
	event_list temp;
	event_list_ITR y = t_stamp.begin();
	while ( y != t_stamp.end())
	{
		if ( (*y).matchB(B))
			temp.push_back(*y);
		y++;
	}
	t_stamp.clear();
	t_stamp = temp;
}

time_stamp::time_stamp(const event_deque& in_events)
//create a timestamp with the events from the deque
{
	for (event_deque::const_iterator e = in_events.begin(); e != in_events.end(); e++)
	{
		t_stamp.push_back(*e);
	}
	t_stamp.sort(); 
}

void time_stamp::insert(const event& in_event)
{
	t_stamp.push_back(in_event);
//	t_stamp.sort(); //maybe not the best place
}


void time_stamp::sort()
{
	t_stamp.sort();
}

void time_stamp::clear()
{
	t_stamp.clear();
}

bool time_stamp::empty () const
{
	return t_stamp.empty();
}

bool time_stamp::issingle() const
{
	return (t_stamp.size() == 1);
}

event_list time_stamp::get_time_stamp() const //access the pointer to the time stamp list of events
{	return t_stamp;}

event_deque time_stamp::convert_time_stamp() const//access moitem
{
	event_deque temp;
	for (const_event_list_ITR y = t_stamp.begin(); y != t_stamp.end(); y++)
	{
		temp.push_back(*y);
	}
	return temp;
}

event time_stamp::get_first_event() const //access moitem
{
	return (*t_stamp.begin());
}

time_stamp time_stamp::intersect(const time_stamp& in_window) const
{  //intersect within a single timestamp with the events in the current window timestamp
	//there should never be duplicates within a single timestamp, as all events
	//at a single timestamp must be coming from different sources
	time_stamp temp;
	for (const_event_list_ITR y= t_stamp.begin();y!= t_stamp.end();y++)
	{
		const_event_list_ITR in = find(in_window.t_stamp.begin(), in_window.t_stamp.end(), *y);
		if ( in != in_window.t_stamp.end() )
		{
			 temp.insert(*y);
		}
	}
	return temp;
}

time_stamp time_stamp::get_sub1(int k) const
{
	time_stamp temp;
	const_event_list_ITR y= t_stamp.begin();
	for (int i = 0; i<k;i++)
	{
		temp.insert(*y);
		++y;
	}
	return temp;
}

time_stamp time_stamp::get_sub2(int k) const
{
	time_stamp temp;
	const_event_list_ITR y= t_stamp.begin();
	++y;
	for (int i = 1; i<=k;i++)
	{
		temp.insert(*y);
		++y;
	}
	return temp;
}

bool time_stamp::doescontain(const time_stamp& smaller) const
	//if true, the calling timestamp list of events contains the smaller list of events
	// if false, it does not contain it, or the smaller list is really not smaller
	// true if the lists are identical also
{
	bool contains = true;

	event_list::const_iterator tb = t_stamp.begin();
	event_list::const_iterator te = t_stamp.end();
	event_list::const_iterator st = smaller.t_stamp.begin();
	do
	{
		event_list::const_iterator in = find(tb, te, (*st));
		if (in == te)
		{
			contains = false;
		}
		else
		{
			st++;
			tb = in;
			tb++;
		}
	}	while (contains && tb != te && st != smaller.t_stamp.end());
	if (tb == te && st != smaller.t_stamp.end())
		contains = false;
	return contains;
}

bool time_stamp::doescontainany(const time_stamp& smaller) const
	//true if the calling timestamp list of events contains at least one of the events
	// in the smaller timestamp list
{
	bool contains = false;

	event_list::const_iterator tb = t_stamp.begin();
	event_list::const_iterator te = t_stamp.end();
	event_list::const_iterator st = smaller.t_stamp.begin();
	do
	{
		event_list::const_iterator in = find(tb, te, (*st));
		if (in != te)
		{
			contains = true;
		}
		else
		{
			st++;
		}
	}	while (!contains && tb != te && st != smaller.t_stamp.end());
	return contains;
}


bool time_stamp::matchB(const event_set& B) const
{//return true if the events in the timestamp are all in the constraint set
	// CHANGE WHEN CONSTRAINTS ARE DNF
	bool constraints = true;
	event_list::const_iterator tb = t_stamp.begin();
	event_set::const_iterator be = B.end();
	event_set::const_iterator bb = B.begin();

	while (constraints && tb != t_stamp.end() )
	{
		event_set::const_iterator in = find(bb,be,*tb);
		if (in == be)
			constraints = false;
		tb++;
	}
	return constraints;
}

/*bool time_stamp::matchruleconstraints(const event_set& T) const
//return true if all the events match the antecedent rule contraint set T
	// CHANGE WHEN CONSTRAINTS ARE DNF
{
	bool match = true;
	event_list::const_iterator tb = t_stamp.begin();
	event_set::const_iterator be = T.end();
	event_set::const_iterator bb = T.begin();

	while (match && tb != t_stamp.end() )
	{
		event_set::const_iterator in = find(bb,be,*tb);
		if (in == be)
			match = false;
		tb++;
	}
	return match;
}
*/

/*bool time_stamp::containsmix(const event_set& T, const event_set& C)
{
	bool nomix = true;
	event_list_ITR y = t_stamp.begin();

	if ( (*y).matchB(T))
	{
		y++;
		while (nomix &&  y != t_stamp.end())
		{
			if ( !(*y).matchB(T))
				nomix = false;
			else
				y++;
		}
	}
	else if ( (*y).matchB(C))
	{
		y++;
		while (nomix &&  y != t_stamp.end())
		{
			if ( !(*y).matchB(C))
				nomix = false;
			else
				y++;
		}
	}
	return !nomix;
}
*/

int time_stamp::get_length() const
{
	int count = 0;
	int len;
	for (event_list::const_iterator itr = t_stamp.begin(); itr != t_stamp.end();itr++)
	{
		len = (*itr).get_length();  //get the event length
		if (count == 0)
			count+=len;
		else
			count= count + len + 2; //add the length of the ", "
	}
	return count;
}

int time_stamp::get_number_events() const
{
	return t_stamp.size();
}

string time_stamp::get_events() const
//Purpose: to return the list of events in this time stamp
//Author:	Sherri Harms
//Returns:	string - The comma delimited list of events in this time stamp surrounded
//			by parentheses
{
	string events;

	events = "(";
	event_list::const_iterator e = t_stamp.begin();
	if (e != t_stamp.end())
	{
		events += (*e).getEvent();
		++e;
	}
	while (e != t_stamp.end())
	{
		events += ",";
		events += (*e).getEvent();
		++e;
	}
	events += ")";

	return events;
}

void time_stamp::output(ostream& outs) const
{
	outs<<"(";
	event_list::const_iterator e = t_stamp.begin();
	if (e != t_stamp.end())
	{
		outs<<*e;
		++e;
	}
	while (e != t_stamp.end())
	{
		outs<<", ";
		outs<<*e;
		++e;
	}
	outs<<")";
}
