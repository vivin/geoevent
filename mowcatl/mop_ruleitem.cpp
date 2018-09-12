/*
filename: mop_ruleitem.cpp
author: Sherri Harms
last modified: 8/8/01
description: Class mop_ruleitem Inherited IMPLEMENTATION file for CTMINEPI.
  	Values of this type are a composite of 
	vector of events named ant, vector of events named cons, support, and a list of occurrences;
	This implementation uses a list for the occurrences and a vector for ant and cons.
******************************************************************************
Known bugs:
  The constraints are all disjuctive singular events.
  step 5 from alg 5 generate candidatges has not been implemented
  input validation is not done
*******************************************************************************
Local Data Structures:
Local variables:
*****************************************************************************
parallel mo rleitem operations include:
constructors, relational operators,
parallel moitem operations used to generate parallel moitemset fac[k],
and generate the representative episodal association rules.
*****************************************************************************
Note: many of the member functions are not used by mop_ruleitem, but are necessary
because other items (parallel, serial, or mos_ruleitem) need them.
*******************************************************************************/
#pragma warning(disable:4786)

#include "moitemset.h"
#include "mop_ruleitem.h"
#include <math.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
using namespace std;

mop_ruleitem::mop_ruleitem( ) :moitem()
{
	event empty;
	ant.push_back(empty);
	ant.clear();
	cons.push_back(empty);
	cons.clear();
	antsupport=0;
}

void mop_ruleitem::clear()
	//clear the moitem & its ant & closure;
{
	event_set::iterator i = ant.begin();

	while(i != ant.end())
	{
		(*i).clear();
		i++;
	}

	event_set::iterator c = cons.begin();

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

void mop_ruleitem::set_antsupport(int in_support)
{
	antsupport=in_support;
}

int mop_ruleitem::get_antsupport() const
{
	return antsupport;
}

void mop_ruleitem::set_conssupport(int in_support)
{
	conssupport=in_support;
}

int mop_ruleitem::get_conssupport() const
{
	return conssupport;
}

void mop_ruleitem::insertant(const event_set&  e)
{
	ant=e;
}
void mop_ruleitem::insertcons(const event_set&  e)
{
	cons=e;
}

void mop_ruleitem::recordmobetween(const moitem* faitem, const moitem* fcitem, int lag, char lag_type)
{
	const mop_ruleitem *antptr;
	antptr = static_cast<const mop_ruleitem*>(faitem);
	const mop_ruleitem *consptr;
	consptr = static_cast<const mop_ruleitem*>(fcitem);

	const_occur_list_ITR consitr = consptr->mos.begin();
	const_occur_list_ITR antitr = antptr->mos.begin(); 

	bool done = false;

	if (consitr == consptr->mos.end() || antitr == antptr->mos.end())
		done= true;

	if (lag_type == 'f')
	{

		//Algorithm: Ex: ant CD cons 11, Ex: ant 11D, cons CE 
		//FIXED WIDTH TIME LAG
		// 1. Keep looking for a mo from ant and a mo for cons where 
		//		the start of ant < start cons
		//		and the start of cons == start of ant + lag
		//		and the stop of ant <= stop of cons.  DO NOT Make sure the
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
		//	output(cout);
	}//end if FIXED

	else
	{
		//Algorithm: Ex: ant CD cons 11, Ex: ant 11D, cons CE 
		//MAXIMUM TIME LAG ALGORITHM
		// 1. Keep looking for an mo from ant and a mo for cons where 
		//		the start of ant < start cons
		//		and the start of cons <= start of ant + lag
		//		and the stop of ant <= stop of cons.  
		//		The occurrence between ant and cons is NOT NECCESSARILY
		//		the MINIMAL occurrence  4/11/02
		//		if there are several ant in a row, the first
		//		to meet the lag requirement will be counted 
		// 2.Once you know you have found one, insert it
		//3. Whenever you run out of mo for either ant or cons, you are done

		do
		{
			int a = (*antitr).get_start();
			int b = (*consitr).get_start();
			while (!done && (*antitr).get_start() > (*consitr).get_start()) //modify by yun
			//while (!done && (*antitr).get_start() >= (*consitr).get_start())
			{
				consitr++;
				if (consitr == consptr->mos.end() )
					done = true;

				a = (*antitr).get_start();
				b = (*consitr).get_start();
			}

			if (!done)
			{
				a = (*antitr).get_start();
				b = (*consitr).get_start();
				int c = (*antitr).get_stop();
				int d= (*consitr).get_stop();

				if ( ((*consitr).get_start() <= (*antitr).get_start() +lag ) 
					&& (*antitr).get_stop() <= (*consitr).get_stop()
					//&& (*consitr).get_start() > (*antitr).get_start())
					&& (*consitr).get_start() >= (*antitr).get_start())  //modify by yun for lag =0
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
	//	output(cout);
	}
}


event_set mop_ruleitem::get_episode() const
{
	event_set s;
    event_set::const_iterator i;
	for (i = ant.begin(); i != ant.end(); i++)
	{
		s.push_back(*i);
	}
	for (i = cons.begin(); i != cons.end(); i++)
		s.push_back(*i);

	return s;
}

event_set mop_ruleitem::get_antepisode() const
{
	event_set s;
	for (event_set::const_iterator i = ant.begin(); i != ant.end(); i++)
	{
		s.push_back(*i);
	}
	return s;
}

event_set mop_ruleitem::get_consepisode() const
{
	event_set s;
	for (event_set::const_iterator i = cons.begin(); i != cons.end(); i++)
		s.push_back(*i);

	return s;
}

void mop_ruleitem::create_1_ante(moitemset& A, const event_set& T) const
{//put the events in the current moitem that are in the constraint file T
//	output(cout);
	for (event_set::const_iterator i = ant.begin(); i != ant.end(); i++)
	{
			moitem * current = new mop_ruleitem;
			current->insertantepisode(*i);
			current->insertcons(cons);
//			current->output(cout);
			A.insertitem(current);
	}
}//create_1_ante

void mop_ruleitem::insertantepisode(const event&  e) 
{
	ant.push_back(e);
//	output(cout);
}

double mop_ruleitem::get_max_support(const fc_type& split_fac)
 //get the max support = support of a larger cons that contains this cons if there is one, otherwise 0
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

bool mop_ruleitem::does_contain(const moitem* Y, int& with_freq) const
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

bool mop_ruleitem::does_contain(const moitem* Y) const
{
	//serial is in the specified order
	//parallel is in lexigraphical order
	//parallel rule items, the Y antecendent must be contained in the calling items antecendent
	// and the Y consequence must be contained in the calling item's consequence
	bool contains = true;
	const mop_ruleitem *pitemptr;
	pitemptr = static_cast<const mop_ruleitem*>(Y);

	event_set::const_iterator itr = ant.begin();
	event_set::const_iterator Yi = pitemptr->ant.begin();

	//testing:
	//cout<<"Check out, cand = "<<*pitemptr<<endl;
	//cout<<" moitem = "<<*this<<endl;
	//system("pause");
	
	do
	{
		event_set::const_iterator in = find(itr, ant.end(), (*Yi));
		if (in == ant.end())
		{
			contains = false;
		}
		else
		{
			Yi++;
			itr = in;
			itr++;
		}
	}	while (contains && itr != ant.end() && Yi != pitemptr->ant.end());
	if (itr == ant.end() && Yi != pitemptr->ant.end())
		contains = false;

	event_set::const_iterator citr = cons.begin();
	event_set::const_iterator Yci = pitemptr->cons.begin();
	while (contains && citr != cons.end() && Yci != pitemptr->cons.end())
	{
		event_set::const_iterator in = find(citr, cons.end(), (*Yci));
		if (in == cons.end())
		{
			contains = false;
		}
		else
		{
			Yci++;
			citr = in;
			citr++;
		}
	}
	if (citr == cons.end() && Yci != pitemptr->cons.end())
		contains = false;

	return contains;
}

int mop_ruleitem::getepisodelen() const
{ return ant.size(); }

int mop_ruleitem::getantnumberofevents() const
{	return ant.size();}
int mop_ruleitem::getconsnumberofevents() const
{
	return cons.size();
}

moitem* mop_ruleitem::generate_parallel_candidate(const event& E, const moitem* Z) const
{
	const mop_ruleitem *pitemptr;
	pitemptr = static_cast<const mop_ruleitem*>(Z);

	moitem * Znew = new mop_ruleitem;
	for (event_set::const_iterator epi_itr = ant.begin(); epi_itr != ant.end(); ++epi_itr)
	{
		event P = *epi_itr;
		Znew->insertantepisode(P);
	}

	Znew->insertantepisode(E);
	Znew->insertcons(pitemptr->cons);
	return Znew;
}

event mop_ruleitem::get_last_event() const
{
	event_set::const_iterator i = ant.end();
	i--;
 return *i;
}

bool mop_ruleitem::fast_gen_support_subs(const moitemset& f_l, int k) const
{ 
	bool support = true;
	int y = 0,x=0;
	while (support && y < k-1)
	{
		moitem * beta = new mop_item;
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

		if (!f_l.does_contain_parallel_rule_item(beta))
		{
			support = false;
		}
		y++;
		delete beta;
	}
	return support;
}//end fast_gen_support_sub


//virtual functions for parallel and serial ants
void mop_ruleitem::insertepisode(const event&  e){}
bool mop_ruleitem::contains_event(const event& A) const
{
	//finds the first occurrence of an event
	bool found = false;
    event_set::const_iterator event_itr;
	for (event_itr = ant.begin(); event_itr != ant.end(); ++event_itr)
	{
		if (A == *event_itr )
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		for (event_itr = cons.begin(); event_itr != cons.end(); ++event_itr)
		{
			if (A == *event_itr )
			{
				found = true;
				break;
			}
		}
	}
	return found;
}
bool mop_ruleitem::matchB(const event_set& B) const
{ 
	bool constraint = true;

	event_set::const_iterator i = ant.begin();

	while(i != ant.end() && constraint)
	{
		if (!binary_search(B.begin(), B.end(), *i) )
		{	//found an event that does not meet the constraints
			// - remove this  ant
			//MUST BE SORTED FIRST _ WONT WORK FOR SERIAL
			constraint = false;
		} //if
		++i;
	}//while
	i =   cons.begin();

	while(i != cons.end() && constraint)
	{
		if (!binary_search(B.begin(), B.end(), *i) )
		{	//found an event that does not meet the constraints
			// - remove this  ant
			//MUST BE SORTED FIRST _ WONT WORK FOR SERIAL
			constraint = false;
		} //if
		++i;
	}//while
	return constraint;
} //matchB
bool mop_ruleitem::matchp() const
//called when generating the next generation set of candidate ants
// only for parallel injunctive ants
{
	bool found = false;
	event_set::const_iterator result;
	event_set::const_iterator i = ant.begin(), j;
	event one;
	while(i != ant.end() && !found)
	{
		j = i;
		j++;
		one = *i;
		result = find(j, ant.end(), one);
		if (result != ant.end()) 
		{	//found an event that is already in the ant
			// - do not add this ant
			found = true;
		} //if
		++i;
	}//while
return found;
} //matchp

/*bool mop_ruleitem::doescontainany(const moitem* Y) const
{return false;}
*/

void mop_ruleitem::bringinnew(int t)
{
/*	occur now(t,t);
	mos.push_back(now);
	support++;*/
}

moitem* mop_ruleitem::getfirstsub(int k) const
{
	moitem * sub = new mop_ruleitem;
/*	event_set::const_iterator epi_itr = ant.begin(); 
	for (int i = 0; i<k;i++)
	{
		event P = *epi_itr;
		sub->insertant(P);
		++epi_itr;
	}*/
	return sub;
}

moitem* mop_ruleitem::getsecondsub(int k) const
{
	moitem * sub = new mop_ruleitem;
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


moitem* mop_ruleitem::generate_parallel_candidate(const event& E) const
{
	moitem * Z = new mop_ruleitem;
/*	for (event_set::const_iterator epi_itr = ant.begin(); epi_itr != ant.end(); ++epi_itr)
	{
		event P = *epi_itr;
		Z->insertant(P);
	}

	Z->insertant(E);*/
	return Z;
}

void mop_ruleitem::sortit()
{	sort(ant.begin(), ant.end());
}

bool mop_ruleitem::equalitem(const moitem* Y) const
{
	const mop_ruleitem* pitemptr = static_cast<const mop_ruleitem*>(Y);
	return (ant == pitemptr->ant && cons == pitemptr->cons);
}


bool mop_ruleitem::issubsetofY(const moitem* Y) const
{
	return false;
}

event mop_ruleitem::get_first_event() const
{
 return *ant.begin();
}

event mop_ruleitem::get_ith_event(int i) const
	//return the ith event in the moitem
{
	return ant[i];
}


void mop_ruleitem::output_episode(ostream& outs) const
{
	outs<<"{";
	event_set::const_iterator e = ant.begin();
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

void mop_ruleitem::output(ostream& outs) const
{
	output_episode(outs);
}

bool mop_ruleitem::matchs() const
{return false;}

t_stamp_vec mop_ruleitem::get_episode(int ignore) const
{
	t_stamp_vec temp;
	return temp;
}

time_stamp mop_ruleitem::get_ith_time_stamp(int i) const
{
	time_stamp temp;
	return temp;
}

void mop_ruleitem::insertepisode(const time_stamp& t) 
{}

time_stamp mop_ruleitem::get_last_time_stamp() const
	//return the last timestamp in the moitem
{
	time_stamp temp;
	return temp;
}

time_stamp mop_ruleitem::get_first_time_stamp() const
{
	time_stamp temp;
	return temp;
}

moitem* mop_ruleitem::generate_serial_candidate(const moitem* E) const
{
	moitem * ptr = new mop_ruleitem;
	return ptr;
}

moitem* mop_ruleitem::generate_serial_rule_candidate(const moitem* E) const
	//generate an moitem candidate in the rule cand generation phase - has no closures anymore!
{
	moitem * ptr = new mop_ruleitem;
	return ptr;
}

moitem* mop_ruleitem::get_last_episode() const 
{
	moitem* ptr = new mop_ruleitem;
	return ptr;
}

void mop_ruleitem::recordmobetween(const moitem* fcsub1, const moitem* fcsub2, bool twins) {}

/*moitem* mop_ruleitem::get_last_split_ant() const 
	//return the last timestamp ant as a new moitem ptr
	//used in generating serial rules
{
	moitem* ptr = new mop_ruleitem;
	return ptr;
}*/

int mop_ruleitem::getnumberofevents() const
{return ant.size() + cons.size();}

//for parallel episodes

//called from support_subs_parallel


moitem* mop_ruleitem::getsub1withintimestamp(int k) const
{
	moitem* p = new mop_ruleitem;
	return p;
}
moitem* mop_ruleitem::getsub2withintimestamp(int k) const
{
	moitem* p = new mop_ruleitem;
	return p;
}

void mop_ruleitem::recordtimestamps(const moitem* fcsub1, const moitem* fcsub2) {}
	//calculate the timestamps for this item (where the events occur together)
