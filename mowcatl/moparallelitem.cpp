/*
filename: moparallelitem.cpp
author: Sherri Harms
last modified: 8/8/01
description: Class mop_item Inherited IMPLEMENTATION file for episodes.
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
#pragma warning(disable:4786)

//#include "event.h"
//#include "moitem.h"
#include "moitemset.h"
#include "moparallelitem.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
using namespace std;

/*mop_item::~mop_item()
{
	event_set::iterator i = episode.begin();

	while(i != episode.end())
	{
		(*i).clear();
		i++;
	}
	episode.clear();
	support = 0;
	mos.clear();

}*/


void mop_item::insertepisode(const event&  e)
{
	episode.push_back(e);
}

void mop_item::clear()
	//clear the moitem & its episode & closure;
{
	event_set::iterator i = episode.begin();

	while(i != episode.end())
	{
		(*i).clear();
		i++;
	}

	episode.clear();
	mos.clear();
	support = 0;
}

bool mop_item::contains_event(const event& A) const
{//finds the first occurrence of an event
	bool found = false;
	for (event_set::const_iterator event_itr = episode.begin(); event_itr != episode.end(); ++event_itr)
	{
		if (A == *event_itr )
		{
			found = true;
			break;
		}
	}
	return found;
}

bool mop_item::matchB(const event_set& B) const
{
	bool constraint = true;

	event_set::const_iterator i = episode.begin();

	while(i != episode.end() && constraint)
	{
		if (!binary_search(B.begin(), B.end(), *i) )
		{	//found an event that does not meet the constraints
			// - remove this  episode
			//MUST BE SORTED FIRST _ WONT WORK FOR SERIAL
			constraint = false;
		} //if
		++i;
	}//while
	return constraint;
} //matchB

bool mop_item::matchp() const
//called when generating the next generation set of candidate episodes
// only for parallel injunctive episodes
{
	bool found = false;
	event_set::const_iterator result;
	event_set::const_iterator i = episode.begin(), j;
	event one;
	while(i != episode.end() && !found)
	{
		j = i;
		j++;
		one = *i;
		result = find(j, episode.end(), one);
		if (result != episode.end()) 
		{	//found an event that is already in the episode
			// - do not add this episode
			found = true;
		} //if
		++i;
	}//while
return found;
} //matchp

bool mop_item::does_contain(const moitem* Y) const
{  //serial is in the specified order
	//parallel is in lexigraphical order
	bool contains = true;
	const mop_item *pitemptr;
	pitemptr = static_cast<const mop_item*>(Y);

	event_set::const_iterator itr = episode.begin();
	event_set::const_iterator Yi = pitemptr->episode.begin();

	/*//testing:
	cout<<"Check out, cand = "<<*pitemptr<<endl;
	cout<<" moitem = "<<*this<<endl;
	system("pause");
	*/

	do
	{
		event_set::const_iterator in = find(itr, episode.end(), (*Yi));
		if (in == episode.end())
		{
			contains = false;
		}
		else
		{
			Yi++;
			itr = in;
			itr++;
		}
	}	while (contains && itr != episode.end() && Yi != pitemptr->episode.end());
	if (itr == episode.end() && Yi != pitemptr->episode.end())
		contains = false;
	return contains;
}

bool mop_item::does_contain(const moitem* Y, int& with_freq) const
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

/*bool mop_item::doescontainany(const moitem* Y) const
{return false;}
*/

void mop_item::bringinnew(int t)
{
	occur now(t,t);
	mos.push_back(now);
	support++;
}

moitem* mop_item::getfirstsub(int k) const
{
	moitem * sub = new mop_item;
	event_set::const_iterator epi_itr = episode.begin(); 
	for (int i = 0; i<k;i++)
	{
		event P = *epi_itr;
		sub->insertepisode(P);
		++epi_itr;
	}
	return sub;
}

moitem* mop_item::getsecondsub(int k) const
{
	moitem * sub = new mop_item;
	event_set::const_iterator epi_itr = episode.begin(); 
	for (int i = 0; i<k-1;i++)
	{
		event P = *epi_itr;
		sub->insertepisode(P);
		++epi_itr;
	}
	++epi_itr;
	event P = *epi_itr;
	sub->insertepisode(P);

	return sub;
}

void mop_item::recordmobetween(const moitem* fcsub1, const moitem* fcsub2, bool twins)
{//parallel
	int start, stop;
	const mop_item *sub1ptr;
	sub1ptr = static_cast<const mop_item*>(fcsub1);
	const mop_item *sub2ptr;
	sub2ptr = static_cast<const mop_item*>(fcsub2);

	const_occur_list_ITR sub2itr = sub2ptr->mos.begin();
	const_occur_list_ITR sub1itr = sub1ptr->mos.begin(); 
	if (twins)
		sub2itr++;
	const_occur_list_ITR first, second;

	bool order;

	if (sub2itr != sub2ptr->mos.end())
	{
		do
		{
			//get the first starting time
			if ( (*sub1itr).get_start() <= (*sub2itr).get_start() )
			{
				order = true;
				first = sub1itr;
				second = sub2itr;
			}
			else
			{
				order = false;
				first = sub2itr;
				second = sub1itr;
			}
			
			//find the closest stopping time
			bool donebig = false;
			while (!donebig && !twins &&(*first).get_start() <= (*second).get_start()  )
			{
				first++;
				if (first == sub1ptr->mos.end() && order || first == sub2ptr->mos.end() && !order )
					donebig = true;
			}

			if (!twins)
				first--;//went one to far in the last loop
			start = (*first).get_start();

			bool equalstart = false;
			if ( (*first).get_start() == (*second).get_start() )
				equalstart = true;

			if ((*first).get_stop() <= (*second).get_stop())
			{
				stop = (*second).get_stop();
			}
			else
			{
				stop = (*first).get_stop();
			}

			occur one(start,stop);
			mos.push_back(one);
				//could update support here, but I chose to do it seperately based on window size
			
			if (order)
			{
				sub1itr=first;
				sub1itr++;
				if (twins || equalstart)
					sub2itr++;
			}
			else
			{
				sub2itr=first;
				sub2itr++;
				if (equalstart)
					sub1itr++;
			}

		}while (sub1itr != sub1ptr->mos.end() && sub2itr != sub2ptr->mos.end());
	}//if another item2
	
}

moitem* mop_item::generate_parallel_candidate(const event& E) const
{
	moitem * Z = new mop_item;
	for (event_set::const_iterator epi_itr = episode.begin(); epi_itr != episode.end(); ++epi_itr)
	{
		event P = *epi_itr;
		Z->insertepisode(P);
	}

	Z->insertepisode(E);
	return Z;
}

void mop_item::sortit()
{	sort(episode.begin(), episode.end());
}

bool mop_item::equalitem(const moitem* Y) const
{
	const mop_item* pitemptr = static_cast<const mop_item*>(Y);
	return (episode == pitemptr->episode);
}

bool mop_item::fast_gen_support_subs(const moitemset& f_l, int k) const
{
	bool support = true;

	int y = 0,x=0;
	while (support && y < k-1)
	{
		moitem * beta = new mop_item;
		for (x =0;x<y;x++)
			beta->insertepisode(episode[x]);
		for (x = y; x<k;x++)
		{
			beta->insertepisode(episode[x+1]);
		}

		//Testing
		//cout<<"beta = ";
		//beta->output_episode(cout);
		//cout<<endl;

		if (!f_l.does_contain_parallel(beta))
		{
			support = false;
		}
		y++;
		delete beta;
	}
return support;
}//end fast_gen_support_sub

bool mop_item::issubsetofY(const moitem* Y) const
{
	//return true if the current moitem is a subset of moitem Y
	const mop_item* pitemptr = static_cast<const mop_item*>(Y);
	bool subset = true;
	event_set::const_iterator itr = episode.begin();
	event_set::const_iterator Yi = pitemptr->episode.begin();
	event_set::const_iterator Yend = pitemptr->episode.end();
	do
	{
		event_set::const_iterator in = find(Yi, Yend, (*itr));
		if (in == pitemptr->episode.end())
		{
			subset = false;
		}
		else
		{
			Yi = in;
			Yi++;
			itr++;
		}
	}	while (subset && itr != episode.end() && Yi != pitemptr->episode.end());
	if (subset && itr != episode.end() && Yi == pitemptr->episode.end())
		subset = false;
	return subset;
}

event_set mop_item::get_episode() const
{
	return episode;
}

event mop_item::get_first_event() const
{
 return *episode.begin();
}

event mop_item::get_last_event() const
{
	event_set::const_iterator i = episode.end();
	i--;
 return *i;
}

event mop_item::get_ith_event(int i) const
	//return the ith event in the moitem
{
	return episode[i];
}


int mop_item::getepisodelen() const
{ return episode.size(); }

void mop_item::create_1_ante(moitemset& A, const event_set& T) const
{//put the events in the current moitem that are in the constraint file T
	//as separate events into the moitemset A
	for (event_set::const_iterator i = episode.begin(); i != episode.end(); i++)
	{
		if ( i->matchB(T))
		{
			moitem * current = new mop_item;
			current->insertepisode(*i);
			A.insertitem(current);
		}
	}
}//create_1_ante

//double mop_item::get_max_support(const fc_type& fc, int i, int k, char type)
 //get the max support = support of a larger episode that contains this episode if there is one, otherwise 0
double mop_item::get_max_support(const fc_type& fc)
{
	double support = 0;
/*	if (i >= k)
		return support;
	else
	{
		int j = k;
		while (j > i)
		{
			double levelsupport;
			levelsupport = fc[j].computelevelsupport(this);
			if (levelsupport > support)
				support = levelsupport;
			j--;
		}
	}*/
	return support;
}//get_max_support

mop_item::mop_item( ) :moitem()
{
	event empty;
	episode.push_back(empty);
	episode.clear();
}


void mop_item::output_episode(ostream& outs) const
{
	outs<<"{";
	event_set::const_iterator e = episode.begin();
	if (e != episode.end())
	{
		outs<<*e;
		++e;
	}
	while (e != episode.end())
	{
		outs<<", ";
		outs<<*e;
		++e;
	}
	outs<<"}";
} //output_episode

void mop_item::output(ostream& outs) const
{
	output_episode(outs);
/*	outs<<" occurs at: ";
	occur_list::const_iterator e = mos.begin();
	if (e != mos.end())
	{
		outs<<*e;
		++e;
	}
	while (e != mos.end())
	{
		outs<<", ";
		outs<<*e;
		++e;
	}
*/
	outs<<" support: "<<support<<endl;
}

bool mop_item::matchs() const
{return false;}

t_stamp_vec mop_item::get_episode(int ignore) const
{
	t_stamp_vec temp;
	return temp;
}

time_stamp mop_item::get_ith_time_stamp(int i) const
{
	time_stamp temp;
	return temp;
}

void mop_item::insertepisode(const time_stamp& t) 
{}

time_stamp mop_item::get_last_time_stamp() const
	//return the last timestamp in the moitem
{
	time_stamp temp;
	return temp;
}

time_stamp mop_item::get_first_time_stamp() const
{
	time_stamp temp;
	return temp;
}

moitem* mop_item::generate_serial_candidate(const moitem* E) const
{
	moitem * ptr = new mop_item;
	return ptr;
}

moitem* mop_item::generate_serial_rule_candidate(const moitem* E) const
	//generate an moitem candidate in the rule cand generation phase - has no closures anymore!
{
	moitem * ptr = new mop_item;
	return ptr;
}

moitem* mop_item::get_last_episode() const 
{
	moitem* ptr = new mop_item;
	return ptr;
}

/*moitem* mop_item::get_last_split_episode() const 
	//return the last timestamp episode as a new moitem ptr
	//used in generating serial rules
{
	moitem* ptr = new mop_item;
	return ptr;
}*/

int mop_item::getnumberofevents() const
{return 0;}

moitem* mop_item::getsub1withintimestamp(int k) const
{
	moitem* p = new mop_item;
	return p;
}
moitem* mop_item::getsub2withintimestamp(int k) const
{
	moitem* p = new mop_item;
	return p;
}
void mop_item::recordtimestamps(const moitem* fcsub1, const moitem* fcsub2){}
	//calculate the timestamps for this item (where the events occur together)
