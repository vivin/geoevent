/*
filename: moserialitem.h
author: Sherri Harms
last modified: 7/20/01
description: Class mos_item interface file for episodes.
  	Values of this type are a composite of 
		moitem (support)  	
		episode ( vector of time stamps), each time stamp is a list of events
		mo (list of occurrences)
	This implementation uses a deque for the closure and a vector for episode.
******************************************************************************
Known bugs:
  The constraints are all disjuctive singular events.
  input validation is not done
*******************************************************************************
Assumptions:  See the documentation file
*******************************************************************************
 
Local Data Structures:
Local variables:
*****************************************************************************
serial moitem operations include:
constructors, relational operators,
serial moitem operations used to generate serial moitemset fc[k],
generate candidates fcc[k+1],
and generate the representative episodal association rules.
*******************************************************************************/
#ifndef smoitem_H
#define smoitem_H

#include "moitem.h"
#include <vector>
#include <deque>
#include <list>
#include <algorithm>
#include <iostream>
#include <fstream>
using namespace std;

class mos_item : public moitem
{
public:
	virtual void insertepisode(const event& e) ;
	//Add the event e to the episode in the calling moitem

	virtual void insertepisode(const time_stamp& t) ;
	//Add the timestamp t to the episode in the calling moitem

	virtual void insertnewtimeepisode(const event& e) ;
	//Add the event e to the episode in the calling moitem

	virtual void clear();
	//clear the moitem & its episode & closure;

	virtual	bool does_contain(const moitem* Y) const;
	virtual	bool does_contain(const moitem* Y, int& with_freq) const;

//	virtual	bool doescontainany(const moitem* Y) const;

	virtual bool contains_event(const event& A) const;
	//return true of the calling moitem contains event A

	bool matchB(const event_set& B) const;
	//returns true if the parallel moitem matches the constraints
	//used in the generate candidate process
	// CHANGE when constraints fixed to DNF

	virtual	bool matchs() const;
	//return true if the calling moitem is a parallel injunction episode
	//used in the candidate generating process
	//also called when generating base moitems in the rule generation process

	void bringinnew(int t);
	//bring in a new serial moitem - update inwindow

	virtual moitem* getfirstsub(int k) const;
	virtual moitem* getsecondsub(int k) const;
	//generate the two subsets that generated this item
	//used to calculate the min occurences for this item
	virtual moitem* getsub1withintimestamp(int k) const;
	virtual moitem* getsub2withintimestamp(int k) const;

	virtual void recordmobetween(const moitem* fcsub1, const moitem* fcsub2, bool twins);
	//calculate the min occurrences for this item

	void recordtimestamps(const moitem* fcsub1, const moitem* fcsub2);
	//calculate the timestamps for this item (where the events occur together)
	
	virtual moitem* generate_serial_candidate(const moitem* E) const;
	//generate a moitem candidate in the cand generation phase
	virtual moitem* generate_serial_rule_candidate(const moitem* E) const;
	//generate an moitem candidate in the rule cand generation phase - has no closures anymore!

	virtual bool issubsetofY(const moitem* Y) const;
	//return true if the current moitem is a subset of moitem Y

	virtual event_set get_episode() const;
	//return the episode as a vector

	virtual t_stamp_vec get_episode(int ignore) const;
	//return the episode as a vector

	virtual event get_first_event() const;
	//return the first event in the moitem

	virtual time_stamp get_first_time_stamp() const;

	virtual event get_last_event() const;
	//return the last event in the moitem

	virtual time_stamp get_last_time_stamp() const;
	//return the last timestamp in the moitem

	virtual moitem* get_last_episode() const; 
	//return the last timestamp episode and closure as a new moitem ptr
	//used in generating serial candidates

//	virtual moitem* get_last_split_episode() const; 
	//return the last timestamp episode as a new moitem ptr
	//used in generating serial rules

	virtual event get_ith_event(int i) const;
	//return the ith event in the moitem
	
	virtual time_stamp get_ith_time_stamp(int i) const;

	int getepisodelen() const;//used in the finding smallest process of Gen-REAR
	int getnumberofevents() const;

	virtual void create_1_ante(moitemset& A, const event_set& T) const;
	//take the current moitem, that match the constraints and add a separate moitem
	//into moitemset A for each event
	//used in the rule generation process

	virtual double get_max_support(const fc_type& fc); 

	//get the max support = support of a larger episode that contains 
	//this episode if there is one, otherwise 0
	//used in the rule generation process
    mos_item( );    //Constructor.
    mos_item(const time_stamp& t);    //Constructor.
    mos_item(const time_stamp& t, const occur& o);    //Constructor.

friend bool operator ==(const mos_item& i1, const mos_item& i2)
{ 
	return (i1.episode == i2.episode); 
}

friend bool operator <(const mos_item& i1, const mos_item& i2)
//Returns true if i1 < i2 ; otherwise, returns false.
{ 
	return (i1.episode < i2.episode); 
}

friend bool operator >(const mos_item& i1, const mos_item& i2)
//Returns true if i1 > i2 ; otherwise, returns false.
{ 
	return (i1.episode > i2.episode); 
}

friend bool operator <=(const mos_item& i1, const mos_item& i2)
//Returns true if i1 <= i2 ; otherwise, returns false.
{ 
	return (i1.episode <= i2.episode); 
}

friend bool operator >=(const mos_item& i1, const mos_item& i2)
//Returns true if i1 >= i2 ; otherwise, returns false.
{ 
	return (i1.episode >= i2.episode); 
}

friend bool operator !=(const mos_item& i1, const mos_item& i2)
//Returns true if i1 != i2 ; otherwise, returns false.
{ 
	return (i1.episode != i2.episode); 
}

	virtual void output(ostream& outs) const;
	virtual void output_episode(ostream& outs) const;

	//pure virtual funcs for parallel episodes
	bool matchp() const;
	bool fast_gen_support_subs(const moitemset& f_l, int k) const;
	virtual moitem* generate_parallel_candidate(const event& E) const;
	//generate a moitem candidate in the rule generation phase
	//otherwise virtual func

	//used in fast generation
	virtual void sortit();
	virtual bool equalitem(const moitem* Y) const;

private:

	t_stamp_vec episode;

};

#endif //smoitem_H
