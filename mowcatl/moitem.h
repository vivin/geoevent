/*
filename: moitem.h
author: Sherri Harms
last modified: 7/20/01
description: BASE Class moitem implementation file for mo episodes.
  	Values of this type store a support value;
	and a list of occurrences
	parallel moitems and serial moitems inherit from this class
	as do parallel ruleitems and serial ruleitems

  Class moitem is an abstract data type, and no variables should be declared of this type.  
  We do create many moitem pointer variables, that point to either a parallel or a serial
  moitem.

  Class moitemset contains a vector of moitem pointers.
******************************************************************************
Known bugs:
  The constraints are all disjuctive singular events.
  input validation is not done
*******************************************************************************
Assumptions:
See documentation.cpp for system documentation.
*****************************************************************************
Local Data Structures:
Local variables: int support
*****************************************************************************
moitem operations include:
input, output, constructors, relational operators, accessors, insertepisode, 
moitem operations used to generate parallel moitemset fc[k],
generate serial moitemset fc[k], generate candicates fcc[k+1],
and generate the representative episodal association rules.
*******************************************************************************/

#ifndef moITEM_H
#define moITEM_H

#include "event.h"
#include "timestamp.h"
#include "occur.h"
#include <vector>
#include <list>
#include <deque>
#include <algorithm>
#include <iostream>
#include <fstream>
using namespace std;

class moitemset;
class time_stamp;

typedef vector<event> event_set;
typedef deque<event> event_deque;
typedef vector<moitemset> fc_type;
typedef vector<time_stamp> t_stamp_vec;
typedef vector<time_stamp>::iterator  t_stamp_vec_ITR;
typedef vector<time_stamp>::const_iterator  const_t_stamp_vec_ITR;
typedef deque<time_stamp> t_stamp_q;
typedef deque<time_stamp>::iterator  t_stamp_q_ITR;
typedef deque<time_stamp>::const_iterator  const_t_stamp_q_ITR;
typedef deque<time_stamp>::const_reverse_iterator  const_r_t_stamp_q_ITR;
typedef deque<time_stamp>::reverse_iterator  r_t_stamp_q_ITR;
typedef list<occur> occur_list;
typedef list<occur>::iterator  occur_list_ITR;
typedef list<occur>::const_iterator  const_occur_list_ITR;

class moitem
{
public:
	virtual void insertepisode(const event& e) =0;
	//Add the episode moitem e to the episode in the calling moitem

	virtual void insertepisode(const time_stamp& t) =0;
	virtual void clear() =0;
	//clear the moitem & its closure and episode

	virtual bool contains_event(const event& A) const = 0;
	//return true of the calling moitem contains event A

	virtual bool matchB(const event_set& B) const = 0;
	//return true if the calling moitem matchs the constraints
	//used in the candidate generating process
	//CHANGE when constraints fixed

	virtual	bool matchp() const = 0;
	//return true if the calling moitem is a parallel injunction episode
	//used in the candidate generating process
	//also called when generating base moitems in the rule generation process

	virtual bool matchs() const =0 ;
	//return true if the calling moitem is a serial episode
	//used in the candidate generating process
	//also called when generating base moitems in the rule generation process

	virtual bool mosempty() const;
	//return true if the mos list is empty

	virtual	bool does_contain(const moitem* Y, int& with_freq) const = 0;
//	virtual	bool doescontainany(const moitem* Y) const =0;
	virtual	bool does_contain(const moitem* Y) const = 0;

//	virtual void countevents(const event_set& F, int f_size, vector<int>& A_count) const = 0;

	virtual void bringinnew(int t) =0;
	//pure virtual func for parallel episodes
	//pure virtual func for serial episodes

	virtual moitem* getfirstsub(int k) const=0;
	virtual moitem* getsecondsub(int k) const=0;
	//generate the two subsets that generated this item
	//used to calculate the min occurences for this item
	virtual moitem* getsub1withintimestamp(int k) const=0;
	virtual moitem* getsub2withintimestamp(int k) const=0;
	virtual void recordtimestamps(const moitem* fcsub1, const moitem* fcsub2) = 0;
	//calculate the timestamps for this item (where the events occur together)

	virtual void recordmobetween(const moitem* fcsub1, const moitem* fcsub2, bool twins)=0;
	//calculate the min occurrences for this item

	virtual int getnumberofevents() const=0;
	virtual int getantnumberofevents() const;
	virtual int getconsnumberofevents() const;

	virtual occur get_last_occurrence() const;

	virtual void update_support(int win);

	virtual moitem* generate_parallel_candidate(const event& E) const = 0;
	//generate a moitem candidate in the rule generation phase
	//otherwise virtual func

	virtual moitem* generate_serial_candidate(const moitem* E) const=0;
	//generate a moitem candidate in the rule generation phase
	//otherwise virtual func
//	virtual moitem* generate_serial_rule_candidate(const moitem* E) const =0;

	virtual bool fast_gen_support_subs(const moitemset& f_l, int k) const =0;
	//used in fast generation

	virtual void sortit() = 0;
	virtual bool equalitem(const moitem* Y) const = 0;

	virtual bool issubsetofY(const moitem* Y) const = 0;
	//return true if the current moitem is a subset of moitem Y

	virtual event_set get_episode() const =0;
	//return the episode as a vector

	virtual t_stamp_vec get_episode(int ignore) const =0;
	//return the episode as a time_stamp vector

	virtual event get_first_event() const = 0;
	//return the first event in the moitem

	virtual time_stamp get_first_time_stamp() const = 0;

	virtual event get_last_event() const = 0;
	//return the last event in the moitem

	virtual time_stamp get_last_time_stamp() const=0;
	//return the last timestamp in the moitem

	virtual moitem* get_last_episode() const =0;
	//return the last timestamp episode and closure as a new s_moitem
	//used in generating candidates

	virtual event get_ith_event(int i) const = 0;
	//return the ith event in the moitem

	virtual time_stamp get_ith_time_stamp(int i) const =0;

	virtual double get_support() const ;
	//return the support

	virtual void set_support(int in_support);
	//set the support

	virtual void resetfreq();

	virtual int getepisodelen() const =0;//used in the finding smallest process of Gen-REAR

	virtual void create_1_ante(moitemset& A, const event_set& T) const = 0;
	//take the current moitem, that match the constraints and add a separate moitem
	//into moitemset A for each event
	//used in the rule generation process

	virtual double get_max_support(const fc_type& split_fac)=0; 
	//get the max support = support of a larger episode that contains 
	//this episode if there is one, otherwise 0
	//used in the rule generation process

	//parallel ruleitem member functions
	virtual void insertant(const event_set&  e);
	virtual void insertcons(const event_set&  e);
	virtual void recordmobetween(const moitem* faitem, const moitem* fcitem, int lag, char lag_type);
	virtual event_set get_antepisode() const;
	virtual event_set get_consepisode() const;
	virtual void insertantepisode(const event&  e);
	virtual moitem* generate_parallel_candidate(const event& E, const moitem* Z) const;
	virtual void set_antsupport(int in_support);
	virtual int get_antsupport() const;
	virtual void set_conssupport(int in_support);
	virtual int get_conssupport() const;

	//serial ruleitem member functions
	virtual void insertant(const t_stamp_vec& e) ;
	virtual void insertcons(const t_stamp_vec& e) ;
	virtual t_stamp_vec get_antepisode(int ignore) const;
	virtual t_stamp_vec get_consepisode(int ignore) const;
	virtual void insertantepisode(const time_stamp&  e);
	virtual moitem* generate_serial_rule_candidate(const moitem* E, const moitem* Z) const;

	virtual void output(ostream& outs) const =0;
	virtual void output_episode(ostream& outs) const =0;

    moitem( );    //Constructor.
	friend class moitemset; //used for the blockstart info for Mannila's fast_gen

protected:
	int support;
	occur_list mos;
	int blockstart;
	static int Aindex(const event& A, const event_set& f_list, int size);
	static int indexoftimeA(const time_stamp& A, const t_stamp_vec& TimeF, int size);
};

#endif //moITEM_H
