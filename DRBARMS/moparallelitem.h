/*
filename: moparallelitem.h
author: Sherri Harms
last modified: 8/8/01
description: Class mop_item Inherited interface file for episodes.
  	Values of this type are a composite of 
	episode, support, and a list of occurrences;
	This implementation uses a list for the occurrences and a vector for episode.
******************************************************************************
Known bugs:
  The constraints are all disjuctive singular events.
  step 5 from alg 5 generate candidatges has not been implemented
  input validation is not done
*******************************************************************************
Local Data Structures:
Local variables:
*****************************************************************************
parallel moitem operations include:
constructors, relational operators,
parallel moitem operations used to generate parallel moitemset fa[k], fc[k]
generate candicates fca[k+1], and fcc[k+1]
and generate the representative episodal association rules.
*******************************************************************************/

#ifndef pITEM_H
#define pITEM_H

//#include "event.h"
#include "moitem.h"
#include <vector>
#include <deque>
#include <algorithm>
#include <iostream>
#include <fstream>
using namespace std;

class mop_item : public moitem
{
public:

//	~mop_item();

	virtual void insertepisode(const event& e) ;
	//Add the event e to the episode in the calling moitem

	virtual bool contains_event(const event& A) const;
	//return true of the calling moitem contains event A

	virtual void clear();
	//clear the moitem & its episode & closure;

	bool matchB(const event_set& B) const;
	//returns true if the parallel moitem matches the constraints
	//used in the generate candidate process
	// CHANGE when constraints fixed to DNF

	virtual	bool matchp() const;
	//return true if the calling moitem is a parallel injunction episode
	//used in the candidate generating process
	//also called when generating base items in the rule generation process

	virtual	bool does_contain(const moitem* Y, int& with_freq) const;
	virtual	bool does_contain(const moitem* Y) const;
//	virtual	bool doescontainany(const moitem* Y) const;

	virtual void bringinnew(int t);
	//bring in a new parallel moitem - update inwindow

	virtual moitem* generate_parallel_candidate(const event& E) const;
	//generate a moitem candidate in the rule generation phase
	//otherwise virtual func

	virtual moitem* getfirstsub(int k) const;
	virtual moitem* getsecondsub(int k) const;
	//generate the two subsets that generated this item
	//used to calculate the min occurences for this item

	virtual void recordmobetween(const moitem* fcsub1, const moitem* fcsub2, bool twins);
	//calculate the min occurrences for this item

	virtual void sortit();

	virtual bool equalitem(const moitem* Y) const;

	bool fast_gen_support_subs(const moitemset& f_l, int k) const;
	//used in fast generation

	virtual bool issubsetofY(const moitem* Y) const;
	//return true if the current moitem is a subset of moitem Y

	virtual event_set get_episode() const;
	//return the episode as a vector

	virtual event get_first_event() const;
	//return the first event in the moitem

	virtual event get_last_event() const;
	//return the last event in the moitem

	virtual event get_ith_event(int i) const;
	//return the ith event in the moitem

	int getepisodelen() const;//used in the finding smallest process of Gen-REAR

	virtual void create_1_ante(moitemset& A, const event_set& T) const;
	//take the current moitem, that match the constraints and add a separate moitem
	//into moitemset A for each event
	//used in the rule generation process

	virtual double get_max_support(const fc_type& fc); 
	//get the max support = support of a larger episode that contains 
	//this episode if there is one, otherwise 0
	//used in the rule generation process
    mop_item( );    //Constructor.

friend bool operator ==(const mop_item& i1, const mop_item& i2)
{ 
	//parallel items are stored in lexigraphical sorted order
	return (i1.episode == i2.episode); 
}

friend bool operator <(const mop_item& i1, const mop_item& i2)
//Returns true if i1 < i2 ; otherwise, returns false.
{ 
	//parallel items are stored in lexigraphical sorted order
	return (i1.episode < i2.episode); 
}

friend bool operator >(const mop_item& i1, const mop_item& i2)
//Returns true if i1 > i2 ; otherwise, returns false.
{
		//parallel items are stored in lexigraphical sorted order
	return (i1.episode > i2.episode); 
}

friend bool operator <=(const mop_item& i1, const mop_item& i2)
//Returns true if i1 <= i2 ; otherwise, returns false.
{ 
		//parallel items are stored in lexigraphical sorted order
	return (i1.episode <= i2.episode); 
}

friend bool operator >=(const mop_item& i1, const mop_item& i2)
//Returns true if i1 >= i2 ; otherwise, returns false.
{ 
	//parallel items are stored in lexigraphical sorted order
	return (i1.episode >= i2.episode); 
}

friend bool operator !=(const mop_item& i1, const mop_item& i2)
//Returns true if i1 != i2 ; otherwise, returns false.
{ 
	//parallel items are stored in lexigraphical sorted order
	return (i1.episode != i2.episode); 

}


	virtual void output(ostream& outs) const;
	virtual void output_episode(ostream& outs) const;
	//pure virtual funcs for serial episodes
	bool matchs() const;
	t_stamp_vec get_episode(int ignore) const;
	virtual time_stamp get_ith_time_stamp(int i) const;
	virtual void insertepisode(const time_stamp& t) ;
	virtual time_stamp get_last_time_stamp() const;
	//return the last timestamp in the moitem
	virtual moitem* get_last_episode() const;
	virtual time_stamp get_first_time_stamp() const;
	//return the last timestamp episode as a new moitem ptr
	//used in generating serial rules

	virtual moitem* generate_serial_candidate(const moitem* E) const;
	//generate a moitem candidate in the rule generation phase
	//otherwise virtual func
	virtual moitem* generate_serial_rule_candidate(const moitem* E) const;
	//generate an moitem candidate in the rule cand generation phase - has no closures anymore!
	virtual int getnumberofevents() const;
	virtual moitem* getsub1withintimestamp(int k) const;
	virtual moitem* getsub2withintimestamp(int k) const;
	virtual void recordtimestamps(const moitem* fcsub1, const moitem* fcsub2);
	//calculate the timestamps for this item (where the events occur together)

private:
	event_set episode;
};


#endif //pITEM_H



