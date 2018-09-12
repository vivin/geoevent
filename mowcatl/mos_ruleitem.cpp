/*
filename: mos_ruleitem.cpp
author: Sherri Harms
last modified: 8/8/01
description: Class mos_ruleitem Inherited IMPLEMENTATION file for serial MINEPI rule items.
  	Values of this type are a composite of 
	time_stamp_vec ant, time_stamp_vec consequent, support, and a list of occurrences;
	This implementation uses a list for the occurrences and a time_stamp_vec for ant and cons.
******************************************************************************
Known bugs:
  The constraints are all disjuctive singular events.
  step 5 from alg 5 generate candidatges has not been implemented
  input validation is not done
*******************************************************************************
Local Data Structures:
Local variables:
*****************************************************************************
serial moitem operations include:
constructors, relational operators,
serial moitem operations used to generate the representative episodal association rules.
*******************************************************************************/
#pragma warning(disable:4786)

#include "moitemset.h"
#include "mos_ruleitem.h"
#include <math.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
using namespace std;

mos_ruleitem::mos_ruleitem( ) :moitem()
{
	time_stamp empty;
	ant.push_back(empty);
	ant.clear();
	cons.push_back(empty);
	cons.clear();
	antsupport=0;
	conssupport=0;
}

void mos_ruleitem::clear()
	//clear the moitem & its ant & closure;
{
	t_stamp_vec_ITR i = ant.begin();

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
	mos.clear();
	support = 0;
}



void mos_ruleitem::set_antsupport(int in_support)
{
	antsupport=in_support;
}

int mos_ruleitem::get_antsupport() const
{
	return antsupport;
}

void mos_ruleitem::set_conssupport(int in_support)
{
	conssupport=in_support;
}

int mos_ruleitem::get_conssupport() const
{
	return conssupport;
}

void mos_ruleitem::insertant(const t_stamp_vec&  e)
{
	for (const_t_stamp_vec_ITR i = e.begin(); i != e.end(); i++)
	{
		ant.push_back(*i);
	}
}
void mos_ruleitem::insertcons(const t_stamp_vec&  e)
{
	for (const_t_stamp_vec_ITR i = e.begin(); i != e.end(); i++)
	{
		cons.push_back(*i);
	}
}

void mos_ruleitem::recordmobetween(const moitem* faitem, const moitem* fcitem, int lag, char lag_type)
{//serial rule items
	const mos_ruleitem *antptr;
	antptr = static_cast<const mos_ruleitem*>(faitem);
	const mos_ruleitem *consptr;
	consptr = static_cast<const mos_ruleitem*>(fcitem);

	const_occur_list_ITR consitr = consptr->mos.begin();
	const_occur_list_ITR antitr = antptr->mos.begin(); 

	bool done = false;

	if (consitr == consptr->mos.end() || antitr == antptr->mos.end())
		done= true;


	if (lag_type == 'f')
	{
			//Algorithm: Ex: ant CD cons 11, Ex: ant 11D, cons CE 
		//FIXED TIME LAG ALGORITM
		// 1. Keep looking for a mo from ant and a mo for cons where 
		//		the start of ant < start cons
		//		and the start of cons == start of ant + lag
		//		and the stop of ant < stop of cons.  WE do NOT make sure the
		//		mo between ant and cons is the MINIMAL occurrence
		// 2.Once you know you have found one, insert it
		//3. Whenever you run out of mo for either ant or cons, you are done

		do
		{
			if ( ((*consitr).get_start() == (*antitr).get_start() +lag ) //CHANGED FOR FIXED
				&& (*antitr).get_stop() <= (*consitr).get_stop()
				&& (*consitr).get_start() > (*antitr).get_start())
				//Tsegaye does NOT want a zero lag for serial episodes
			{
				occur one((*antitr).get_start(),(*consitr).get_stop());
				mos.push_back(one);
				support++;	//could update support later, but I chose to do it here for the rule items
				antitr++;
				consitr++;
			}
			else if ((*antitr).get_start() +lag > (*consitr).get_start())
			{
				consitr++;
			}
			else if ((*antitr).get_start() + lag < (*consitr).get_start())
			{
				antitr++;
			}
			else //if ((*consitr).get_start() == (*antitr).get_start() +lag ) 
				//ant to wide
			{
				consitr++;
			} 
		}while (antitr != antptr->mos.end() && consitr != consptr->mos.end());
	} //end if FIXED TIME LAG

	else
	{
		//MAXIMUM TIME LAG ALGORITHM
	
		//Algorithm: Ex: ant CD cons 11, Ex: ant 11D, cons CE 
		// 1. Keep looking for an mo from ant and a mo for cons where 
		//		the start of ant < start cons
		//		and the start of cons <= start of ant + lag
		//		and the stop of ant < stop of cons.  
		//		The occurrence between ant and cons is NOT NECCESSARILY
		//		the MINIMAL occurrence  4/11/02
		//		if there are several ant in a row, the first
		//		to meet the lag requirement will be counted 
		// 2.Once you know you have found one, insert it
		//3. Whenever you run out of mo for either ant or cons, you are done

		do
		{
			while (!done && (*antitr).get_start() >= (*consitr).get_start())
			{
				consitr++;
				if (consitr == consptr->mos.end() )
					done = true;
			}

			if (!done)
			{
				if ( ((*consitr).get_start() <= (*antitr).get_start() +lag ) 
					&& (*antitr).get_stop() <= (*consitr).get_stop()
					&& (*consitr).get_start() > (*antitr).get_start())
					//Tsegaye does NOT want a zero lag for serial episodes
				{
					occur one((*antitr).get_start(),(*consitr).get_stop());
					mos.push_back(one);
					support++;	//could update support later, but I chose to do it here for the rule items
					antitr++;
					consitr++;
				}
				else
					antitr++;
			}
		}while (!done && antitr != antptr->mos.end() && consitr != consptr->mos.end());
	} //end of MAXIMUM TIME LAG Algorithm
//	output(cout);
}


t_stamp_vec mos_ruleitem::get_episode(int ignore) const
{
	t_stamp_vec s;
    const_t_stamp_vec_ITR i;
	for (i = ant.begin(); i != ant.end(); i++)
	{
		s.push_back(*i);
	}
	for (i = cons.begin(); i != cons.end(); i++)
		s.push_back(*i);

	return s;
}

event_set mos_ruleitem::get_episode() const
{
	event_set s;
	return s;
}

t_stamp_vec mos_ruleitem::get_antepisode(int ignore) const
{
//	t_stamp_vec s;
//	for (const_t_stamp_vec_ITR i = ant.begin(); i != ant.end(); i++)
//	{
//		s.push_back(*i);
//	}
	return ant;
}

t_stamp_vec mos_ruleitem::get_consepisode(int ignore) const
{
//	t_stamp_vec s;
//	for (const_t_stamp_vec_ITR i = cons.begin(); i != cons.end(); i++)
//		s.push_back(*i);
//	return s;
	return cons;
}

void mos_ruleitem::create_1_ante(moitemset& A, const event_set& T) const
{//put the events in the current moitem that are in the constraint file T
//	output(cout);
	for (const_t_stamp_vec_ITR i = ant.begin(); i != ant.end(); i++)
	{
			moitem * current = new mos_ruleitem;
			current->insertantepisode(*i);
			current->insertcons(cons);
//			current->output(cout);
			A.insertitem(current);
	}
}//create_1_ante

void mos_ruleitem::insertantepisode(const time_stamp&  e) 
{
	ant.push_back(e);
//	output(cout);
}

double mos_ruleitem::get_max_support(const fc_type& split_fac)
 //get the max support = support of a larger ant that contains this ant if there is one, otherwise 0
{
	double sup = 0;
	if (getconsnumberofevents() == moitemset::seeconslargest()) 
		return sup;
	else
	{
		int index = getantnumberofevents()*moitemset::seeconslargest()-1;
		int j = moitemset::seeconslargest();
		while (j > getconsnumberofevents())
		{
			double levelsupport=0;
			if (!split_fac[index].empty() )
				levelsupport = split_fac[index].computelevelsupport(this);
			if (levelsupport > sup && levelsupport <= support)
				sup = levelsupport;
			index--;
			j--;
		}
	}
	return sup;
}//get_max_support

bool mos_ruleitem::does_contain(const moitem* Y, int& with_freq) const
{
	bool contains = false;

	if (does_contain(Y))
	{
		contains = true;
		with_freq=support;
	}
	else
		with_freq=0;
	return contains;
}

bool mos_ruleitem::does_contain(const moitem* Y) const
{
	//serial is in the specified order
	//parallel is in lexigraphical order
	//parallel rule items, the Y antecendent must be contained in the calling items antecendent
	// and the Y consequence must be contained in the calling item's consequence
	//serial rule items, the Y antecendent must be contained in the calling items antecendent
	// and the Y consequence must be contained in the calling item's consequence
	bool contains = true;
	const mos_ruleitem *sitemptr;
	sitemptr = static_cast<const mos_ruleitem*>(Y);

	const_t_stamp_vec_ITR itr = ant.begin();
	const_t_stamp_vec_ITR Yi = sitemptr->ant.begin();

	//testing:
	//cout<<"Check out, cand = "<<*pitemptr<<endl;
	//cout<<" moitem = "<<*this<<endl;
	//system("pause");
	
//look through the episode for each of the events in moitem Y,
//  be sure to find them in the correct order
//  the events in a time stamp in Y need to be together in a timestamp in the calling moitem
//     but the calling moitem may have more events in a timestamp
//  the first timestamp in Y may not be the first timestamp in the calling moitem
// and the next timestamp in Y may be in a later time stamp in the calling moitem
// but the order is maintained
// return true if Y is contained within the calling moitem

	do
	{
		//find one time stamp at a time
		bool found = false;
		while (! found && itr != ant.end() && Yi != sitemptr->ant.end() )
		{ 
			if ( (*itr).doescontain(*Yi) )
				found = true;
			itr++;  //increment - if not found, look in the next timestamp
					// if found, we will start looking for the next Yi timestamp in the next timestamp
		}

		if (found)
		{// find the next timestamp
			Yi++;
		}
		else
			contains = false;

	}	while (contains && itr != ant.end() && Yi != sitemptr->ant.end());
	if (itr == ant.end() && Yi != sitemptr->ant.end())
		contains = false;
	if (contains)
	{
		itr = cons.begin();
		Yi = sitemptr->cons.begin();

		while (contains && itr != cons.end() && Yi != sitemptr->cons.end())
		{
		//find one time stamp at a time
			bool found = false;
			while (! found && itr != cons.end() && Yi != sitemptr->cons.end() )
			{ 
				if ( (*itr).doescontain(*Yi) )
					found = true;
				itr++;  //increment - if not found, look in the next timestamp
						// if found, we will start looking for the next Yi timestamp in the next timestamp
			}

			if (found)
			{// find the next timestamp
				Yi++;
			}
			else
				contains = false;
		}
		if (itr == cons.end() && Yi != sitemptr->cons.end())
			contains = false;
	}
	return contains;
}

int mos_ruleitem::getepisodelen() const
{ return ant.size(); }

int mos_ruleitem::getnumberofevents() const
{
	int count=0;    
    const_t_stamp_vec_ITR epi_itr;
	for (epi_itr = ant.begin(); epi_itr != ant.end(); ++epi_itr)
	{
		count += (*epi_itr).get_number_events();
	}
	for (epi_itr = cons.begin(); epi_itr != cons.end(); ++epi_itr)
	{
		count += (*epi_itr).get_number_events();
	}
	return count;
}

int mos_ruleitem::getantnumberofevents() const
{//really get the number of timestamps
//	int count=0;
//	for (const_t_stamp_vec_ITR epi_itr = ant.begin(); epi_itr != ant.end(); ++epi_itr)
//	{
//		count += (*epi_itr).get_number_events();
//	}
//	return count;
	return ant.size();
}

int mos_ruleitem::getconsnumberofevents() const
{//really get the number of timestamps
//	int count=0;
//	for (const_t_stamp_vec_ITR epi_itr = cons.begin(); epi_itr != cons.end(); ++epi_itr)
//	{
//		count += (*epi_itr).get_number_events();
//	}
//	return count;
	return cons.size();
}

moitem* mos_ruleitem::generate_serial_rule_candidate(const moitem* E, const moitem* Z) const
//generate an moitem candidate in the rule cand generation phase - has no closures anymore!
{
	const mos_ruleitem *pitemptr;
	pitemptr = static_cast<const mos_ruleitem*>(Z);

	moitem * Znew = new mos_ruleitem;
	for (const_t_stamp_vec_ITR epi_itr = ant.begin(); epi_itr != ant.end(); ++epi_itr)
	{
		Znew->insertantepisode(*epi_itr);
	}

	//E contains only one timestamp in its closure and episode
	const mos_ruleitem *sitemptr = static_cast<const mos_ruleitem*>(E);
	const_t_stamp_vec_ITR Eeitr = sitemptr->ant.begin();
	Znew->insertantepisode(*Eeitr);
	Znew->insertcons(pitemptr->cons);
	return Znew;
}

moitem* mos_ruleitem::generate_serial_candidate(const moitem* E) const
{
	moitem * Znew = new mos_ruleitem;
	for (const_t_stamp_vec_ITR epi_itr = ant.begin(); epi_itr != ant.end(); ++epi_itr)
	{
		Znew->insertantepisode(*epi_itr);
	}

	//E contains only one timestamp in its closure and episode
	const mos_ruleitem *sitemptr = static_cast<const mos_ruleitem*>(E);
	const_t_stamp_vec_ITR Eeitr = sitemptr->ant.begin();
	Znew->insertantepisode(*Eeitr);
	Znew->insertcons(cons);
	return Znew;
}

moitem* mos_ruleitem::get_last_episode() const 
{
	moitem* temp = new mos_ruleitem;
	const_t_stamp_vec_ITR i = ant.end();
	i--;
	temp->insertantepisode((*i));
	temp->insertcons(cons);
 return temp;
}

event mos_ruleitem::get_last_event() const
{ 
	event e;
 return e;
}

bool mos_ruleitem::fast_gen_support_subs(const moitemset& f_l, int k) const
{ 
	bool support = true;
	int y = 0,x=0;
	while (support && y < k-1)
	{
		moitem * beta = new mos_item;
		for (x =0;x<y;x++)
			beta->insertantepisode(ant[x]);
		for (x = y; x<k;x++)
		{
			beta->insertantepisode(ant[x+1]);
		}
		beta->insertcons(cons);

		//Testing
		//cout<<"beta = ";
		//beta->output_episode(cout);
		//cout<<endl;

		if (!f_l.does_contain_serial_rule_item(beta))
		{
			support = false;
		}
		y++;
		delete beta;
	}
	return support;
}//end fast_gen_support_sub

bool mos_ruleitem::matchs() const
//called when generating the next generation set of candidate episodes
// only for serial injunctive episodes
//NEEDS at least two timestamps in the episode to work
{
	bool found = false, duplicate = false;
	const_t_stamp_vec_ITR last = ant.end();
	last--;
	const_t_stamp_vec_ITR i = ant.begin(), j;
	do
	{
	j=i;
	j++;
		//find one time stamp at a time
		while (! found && j != ant.end())
		{ 
			if ( (*j).doescontain(*i) )
				found = true;
			else
				j++;  //increment - if not found, look in the next timestamp
		}

		if (!found)
		{// find the next timestamp
			i++;
		}
		else
			duplicate = true;

	}	while (!duplicate && j != ant.end() && i != last);

	last = cons.end();
	last--;
	i = cons.begin();
	j=i;
	while (!duplicate && j != cons.end() && i != last)	
	{
		j=i;
		j++;
		//find one time stamp at a time
		while (! found && j != cons.end())
		{ 
			if ( (*j).doescontain(*i) )
				found = true;
			else
				j++;  //increment - if not found, look in the next timestamp
		}

		if (!found)
		{// find the next timestamp
			i++;
		}
		else
			duplicate = true;
	}
	return !duplicate;
} //matchs

//virtual functions for parallel and serial ants
void mos_ruleitem::insertepisode(const event&  e){}
bool mos_ruleitem::contains_event(const event& A) const
{
	//finds the first occurrence of an event
	bool found = false;
    const_t_stamp_vec_ITR event_itr;
	for (event_itr = ant.begin(); event_itr != ant.end(); ++event_itr)
	{
		if ( (*event_itr).doescontain(A) )
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		for (event_itr = cons.begin(); event_itr != cons.end(); ++event_itr)
		{
		if ( (*event_itr).doescontain(A) )
			{
				found = true;
				break;
			}
		}
	}
	return found;
}
bool mos_ruleitem::matchB(const event_set& B) const
{ 
	bool constraint = true;

	const_t_stamp_vec_ITR i = ant.begin();

	while(constraint && i != ant.end())
	{
		if (!(*i).matchB(B) )
		{	//found an timestamp that does not meet the constraints
			// - do not add this episode
			constraint = false;
		} //if
		++i;
	}//while

	i =   cons.begin();
	while(constraint && i != cons.end())
	{
		if (!(*i).matchB(B) )
		{	//found an timestamp that does not meet the constraints
			// - do not add this episode
			constraint = false;
		} //if
		++i;
	}//while

	return constraint;
} //matchB
bool mos_ruleitem::matchp() const
//called when generating the next generation set of candidate ants
// only for parallel injunctive ants
{ return false;} //matchp

/*bool mos_ruleitem::doescontainany(const moitem* Y) const
{return false;}
*/

void mos_ruleitem::bringinnew(int t)
{
/*	occur now(t,t);
	mos.push_back(now);
	support++;*/
}

moitem* mos_ruleitem::getfirstsub(int k) const
{
	moitem * sub = new mos_ruleitem;
/*	event_set::const_iterator epi_itr = ant.begin(); 
	for (int i = 0; i<k;i++)
	{
		event P = *epi_itr;
		sub->insertant(P);
		++epi_itr;
	}*/
	return sub;
}

moitem* mos_ruleitem::getsecondsub(int k) const
{
	moitem * sub = new mos_ruleitem;
/*	event_set::const_iterator epi_itr = ant.begin(); 
	for (int i = 0; i<k-1;i++)
	{
		event P = *epi_itr;
		sub->insertant(P);
		++epi_itr;
	}
	++epi_itr;
	event P = *epi_itr;
	sub->insertant(P);
*/
	return sub;
}

moitem* mos_ruleitem::generate_parallel_candidate(const event& E) const
{
	moitem * Z = new mos_ruleitem;
/*	for (event_set::const_iterator epi_itr = ant.begin(); epi_itr != ant.end(); ++epi_itr)
	{
		event P = *epi_itr;
		Z->insertant(P);
	}

	Z->insertant(E);*/
	return Z;
}

void mos_ruleitem::sortit()
{	sort(ant.begin(), ant.end());
	sort(cons.begin(), cons.end());
}

bool mos_ruleitem::equalitem(const moitem* Y) const
{
	const mos_ruleitem* pitemptr = static_cast<const mos_ruleitem*>(Y);
	return (ant == pitemptr->ant && cons == pitemptr->cons);
}

bool mos_ruleitem::issubsetofY(const moitem* Y) const
{
	return false;
}

event mos_ruleitem::get_ith_event(int i) const
	//return the ith event in the moitem
{
	event e;
	return e;
}

event mos_ruleitem::get_first_event() const
{
	time_stamp first = *ant.begin();
	return first.get_first_event();
}


void mos_ruleitem::output_episode(ostream& outs) const
{
	outs<<"{";
	const_t_stamp_vec_ITR e = ant.begin();
	if (e != ant.end())
	{
		outs<<*e;
		++e;
	}
	while (e != ant.end())
	{
		outs<<", ";
		outs<<*e;
		++e;
	}
	outs<<" ==> ";
	e = cons.begin();
	if (e != cons.end())
	{
		outs<<*e;
		++e;
	}
	while (e != cons.end())
	{
		outs<<", ";
		outs<<*e;
		++e;
	}
	outs<<"}";
	
/*	outs<<" occurs at: ";

	occur_list::const_iterator x = mos.begin();
	if (x != mos.end())
	{
		outs<<*x;
		++x;
	}
	while (x != mos.end())
	{
		outs<<", ";
		outs<<*x;
		++x;
	}
*/
	outs<<" support: "<<support<<endl;
} //output_cons

void mos_ruleitem::output(ostream& outs) const
{
	output_episode(outs);
}

time_stamp mos_ruleitem::get_ith_time_stamp(int i) const
{
	return ant[i];
}

void mos_ruleitem::insertepisode(const time_stamp& t) 
{
	ant.push_back(t);
}

time_stamp mos_ruleitem::get_last_time_stamp() const
	//return the last timestamp in the moitem
{
	const_t_stamp_vec_ITR i = cons.end();
	i--;
 return *i;
}

time_stamp mos_ruleitem::get_first_time_stamp() const
{
	return (*ant.begin());
}

void mos_ruleitem::recordmobetween(const moitem* fcsub1, const moitem* fcsub2, bool twins) {}

/*moitem* mos_ruleitem::get_last_split_ant() const 
	//return the last timestamp ant as a new moitem ptr
	//used in generating serial rules
{
	moitem* ptr = new mos_ruleitem;
	return ptr;
}*/

//for parallel episodes

//called from support_subs_parallel


moitem* mos_ruleitem::getsub1withintimestamp(int k) const
{
	moitem* p = new mos_ruleitem;
	return p;
}
moitem* mos_ruleitem::getsub2withintimestamp(int k) const
{
	moitem* p = new mos_ruleitem;
	return p;
}

void mos_ruleitem::recordtimestamps(const moitem* fcsub1, const moitem* fcsub2) {}
	//calculate the timestamps for this item (where the events occur together)
