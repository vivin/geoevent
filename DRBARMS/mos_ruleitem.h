/*
filename: mos_ruleitem.h
author: Sherri Harms
last modified: 8/8/01
description: Class mos_ruleitem interface file for serial rule episodes.
  	Values of this type are a composite of 
	antecedent episode, consequent episode, support, and a list of occurrences;
	This implementation uses a list for the occurrences and a vector for episodes.
******************************************************************************
Known bugs:
  The constraints are all disjuctive singular events.
  step 5 from alg 5 generate candidates has not been implemented
  input validation is not done
*******************************************************************************
Local Data Structures:
Local variables:
*****************************************************************************
serial mo ruleitem operations include:
constructors, relational operators,
and generate the representative episodal association rules.
*******************************************************************************/

#ifndef sruleITEM_H
#define sruleITEM_H

//#include "event.h"
#include "moitem.h"
#include <vector>
#include <deque>
#include <algorithm>
#include <iostream>
#include <fstream>
using namespace std;

class mos_ruleitem : public moitem
{
public:

	void insertant(const t_stamp_vec& e) ;
	//Add the event e to the episode in the calling moitem

	void insertcons(const t_stamp_vec& e) ;
	//Add the event e to the episode in the calling moitem

	void recordmobetween(const moitem* faitem, const moitem* fcitem, int lag, char lag_type);
	//calculate the min occurrences for this item
	
	void set_antsupport(int in_support);
	int get_antsupport() const;
	void set_conssupport(int in_support);
	int get_conssupport() const;

	
	virtual int getantnumberofevents() const;
	virtual int getconsnumberofevents() const;

	virtual t_stamp_vec get_antepisode(int ignore) const;
	virtual t_stamp_vec get_consepisode(int ignore) const;
	void insertantepisode(const time_stamp&  e);
	moitem* generate_serial_rule_candidate(const moitem* E, const moitem* Z) const;

	virtual bool equalitem(const moitem* Y) const;

    mos_ruleitem( );    //Constructor.

friend bool operator ==(const mos_ruleitem& i1, const mos_ruleitem& i2)
{ 
	//parallel items are stored in lexigraphical sorted order
	return (i1.ant == i2.ant && i1.cons == i2.cons); 
}

friend bool operator <(const mos_ruleitem& i1, const mos_ruleitem& i2)
//Returns true if i1 < i2 ; otherwise, returns false.
{ 
	//parallel items are stored in lexigraphical sorted order
	return (i1.ant < i2.ant || i1.ant == i2.ant && i1.cons < i2.cons ); 
}

friend bool operator >(const mos_ruleitem& i1, const mos_ruleitem& i2)
//Returns true if i1 > i2 ; otherwise, returns false.
{
		//parallel items are stored in lexigraphical sorted order
	return (i1.ant > i2.ant || i1.ant == i2.ant && i1.cons > i2.cons ); 
}

friend bool operator <=(const mos_ruleitem& i1, const mos_ruleitem& i2)
//Returns true if i1 <= i2 ; otherwise, returns false.
{ 
		//parallel items are stored in lexigraphical sorted order
	return (i1.ant <= i2.ant || i1.ant == i2.ant && i1.cons <= i2.cons ); 
}

friend bool operator >=(const mos_ruleitem& i1, const mos_ruleitem& i2)
//Returns true if i1 >= i2 ; otherwise, returns false.
{ 
	//parallel items are stored in lexigraphical sorted order
	return (i1.ant >= i2.ant || i1.ant == i2.ant && i1.cons >= i2.cons ); 
}

friend bool operator !=(const mos_ruleitem& i1, const mos_ruleitem& i2)
//Returns true if i1 != i2 ; otherwise, returns false.
{ 
	//parallel items are stored in lexigraphical sorted order
	return ( !(i1.ant == i2.ant && i1.cons == i2.cons) ); 

}


	virtual void output(ostream& outs) const;
	virtual void output_episode(ostream& outs) const;
	
//pure virtual functions for serial and parallel items
	virtual void recordmobetween(const moitem* fcsub1, const moitem* fcsub2, bool twins);
	virtual moitem* getfirstsub(int k) const;
	virtual moitem* getsecondsub(int k) const;
	virtual void insertepisode(const event& e) ;
	virtual bool contains_event(const event& A) const;
	virtual void clear();
	bool matchB(const event_set& B) const;
	virtual	bool matchp() const;
	virtual	bool does_contain(const moitem* Y, int& with_freq) const;
	virtual	bool does_contain(const moitem* Y) const;
	virtual void bringinnew(int t);
	virtual moitem* generate_parallel_candidate(const event& E) const;
	virtual void sortit();
	bool fast_gen_support_subs(const moitemset& f_l, int k) const;
	virtual bool issubsetofY(const moitem* Y) const;
	virtual event_set get_episode() const;
	virtual event get_first_event() const;
	virtual event get_last_event() const;
	virtual event get_ith_event(int i) const;
	int getepisodelen() const;//used in the finding smallest process of Gen-REAR
	virtual void create_1_ante(moitemset& A, const event_set& T) const;
	virtual double get_max_support(const fc_type& fc); 
	virtual bool matchs() const;
	virtual t_stamp_vec get_episode(int ignore) const;
	virtual time_stamp get_ith_time_stamp(int i) const;
	virtual void insertepisode(const time_stamp& t) ;
	virtual time_stamp get_last_time_stamp() const;
	virtual moitem* get_last_episode() const;
	virtual time_stamp get_first_time_stamp() const;
	virtual moitem* generate_serial_candidate(const moitem* E) const;
	virtual int getnumberofevents() const;
	virtual moitem* getsub1withintimestamp(int k) const;
	virtual moitem* getsub2withintimestamp(int k) const;
	virtual void recordtimestamps(const moitem* fcsub1, const moitem* fcsub2);
	//calculate the timestamps for this item (where the events occur together)

private:
	t_stamp_vec ant;
	t_stamp_vec cons;
	int antsupport;
	int conssupport;
};


#endif //pITEM_H



