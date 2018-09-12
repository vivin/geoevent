/*
filename: serialitem.cpp
author: Sherri Harms
last modified: 7/06/01
description: Class mos_item implementation file for episodes.
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
#pragma warning(disable:4786)

#include "moserialitem.h"
#include "moitemset.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
using namespace std;

mos_item::mos_item( ) :moitem()
{
	time_stamp t;
	episode.push_back(t);
	episode.clear();
	occur now;
	mos.push_back(now);
	mos.clear();
}


mos_item::mos_item(const time_stamp& t)    //Constructor.
{
	episode.push_back(t);
	occur now;
	mos.push_back(now);
	mos.clear();
}

mos_item::mos_item(const time_stamp& t, const occur& o)    //Constructor.
{
	episode.push_back(t);
	mos.push_back(o);
}

void mos_item::insertepisode(const time_stamp& e)
{//add this timestamp to the episodelist
		episode.push_back(e);
}

void mos_item::insertnewtimeepisode(const event& e) 
	//Add the event e to the episode in the calling moitem
{
	time_stamp temp(e);
	episode.push_back(temp);
}

void mos_item::clear()
	//clear the moitem & its episode & closure;
{
	t_stamp_vec_ITR i = episode.begin();

	while(i != episode.end())
	{
		(*i).clear();
		i++;
	}

	episode.clear();

	mos.clear();
	support = 0;
}

bool mos_item::does_contain(const moitem* Y) const
{  //serial is in the specified order
	//parallel is in lexigraphical order
	bool contains = true;
	const mos_item *sitemptr = static_cast<const mos_item*>(Y);

	const_t_stamp_vec_ITR itr = episode.begin();
	const_t_stamp_vec_ITR Yi = sitemptr->episode.begin();

	/*//testing:
	cout<<"Check out, cand = "<<*sitemptr<<endl;
	cout<<" moitem = "<<*this<<endl;
	system("pause");
	*/

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
		while (! found && itr != episode.end() && Yi != sitemptr->episode.end() )
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

	}	while (contains && itr != episode.end() && Yi != sitemptr->episode.end());
	if (itr == episode.end() && Yi != sitemptr->episode.end())
		contains = false;
	return contains;
}

bool mos_item::does_contain(const moitem* Y, int& with_freq) const
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

bool mos_item::contains_event(const event& A) const
{//finds the first occurrence of an event
	bool found = false;
	for (const_t_stamp_vec_ITR event_itr = episode.begin(); event_itr != episode.end(); ++event_itr)
	{
		if ( (*event_itr).doescontain(A) )
		{
			found = true;
			break;
		}
	}
	return found;
}

bool mos_item::matchB(const event_set& B) const
{//return true if all the events contained in the calling moitem meet the constraints
	//return false if any of the events in the calling moitem are not in the constraint list

	bool constraint = true;

	const_t_stamp_vec_ITR i = episode.begin();

	while(constraint && i != episode.end())
	{
		if (!(*i).matchB(B) )
		{	//found an timestamp that does not meet the constraints
			// - do not add this episode
			constraint = false;
		} //if
		++i;
	}//while
	return constraint;
}

bool mos_item::matchp() const
//called when generating the next generation set of candidate episodes
// only for parallel injunctive episodes
{ return false;} //matchp

bool mos_item::matchs() const
//called when generating the next generation set of candidate episodes
// only for serial injunctive episodes
//NEEDS at least two timestamps in the episode to work
{
	bool match = false;
	const_t_stamp_vec_ITR cand= episode.end();
	cand--;
	const_t_stamp_vec_ITR i = episode.begin();
	const_t_stamp_vec_ITR te = episode.end();
	te--; //last timestamp is the candidate, we are making sure that
	//none of the events in this last timestamp are in the previous timestamps
	while(!match && i != te)
	{
		if ((*i).doescontainany(*cand)) 
		{	//found an event that is already in the episode
			// - do not add this episode
			match = true;
		} //if
		++i;
	}//while
return match;
} //matchs

void mos_item::bringinnew(int t) 
	//bring in a new serial moitem - update inwindow
{
	//testing:
/*	 cout<<"Bring in episode: ";
	 output(cout);
	 cout<<endl;
	// system("pause");
*/		
	occur now(t,t);
	mos.push_back(now);
	support++;
}//end bring in new serial

moitem* mos_item::getfirstsub(int k) const
{
	moitem * sub = new mos_item;
	const_t_stamp_vec_ITR epi_itr = episode.begin(); 
	for (int i = 0; i<k;i++)
	{
		time_stamp P = *epi_itr;
		sub->insertepisode(P);
		++epi_itr;
	}
	return sub;
}

moitem* mos_item::getsub1withintimestamp(int k) const
{
	moitem * sub = new mos_item;
	const_t_stamp_vec_ITR epi_itr = episode.begin(); 
	time_stamp P = (*epi_itr).get_sub1(k);
	sub->insertepisode(P);
	return sub;
}

moitem* mos_item::getsub2withintimestamp(int k) const
{
	moitem * sub = new mos_item;
	const_t_stamp_vec_ITR epi_itr = episode.begin(); 
	time_stamp P = (*epi_itr).get_sub2(k);
	sub->insertepisode(P);
	return sub;
}

moitem* mos_item::getsecondsub(int k) const
{
	moitem * sub = new mos_item;
	const_t_stamp_vec_ITR epi_itr = episode.begin(); 
	epi_itr++;//move past the first timestamp
	for (int i = 1; i<=k;i++)
	{
		time_stamp P = *epi_itr;
		sub->insertepisode(P);
		++epi_itr;
	}
	return sub;
}

void mos_item::recordmobetween(const moitem* fcsub1, const moitem* fcsub2, bool twins)
{//serial
	const mos_item *sub1ptr;
	sub1ptr = static_cast<const mos_item*>(fcsub1);
	const mos_item *sub2ptr;
	sub2ptr = static_cast<const mos_item*>(fcsub2);

	const_occur_list_ITR sub2itr = sub2ptr->mos.begin();
	const_occur_list_ITR sub1itr = sub1ptr->mos.begin(); 

	bool done = false;

	if (sub2itr == sub2ptr->mos.end())
		done= true;


	//Algorithm: Ex: CDE, subs CD and DE, Ex: D,11D, subs D (11D)
	// 1. Keep looking for a mo for sub1 and a mo for sub2 where the start of sub1 < start sub2
	//		and the stop of sub1 < stop of sub2.  Make sure the
	//		mo between sub1 and sub2 is the MINIMAL occurrence
	// 2.Once you know you have found one, insert it
	//3. Whenever you run out of mo for either sub1 or sub2, you are done

	do
	{
		while (!done && (*sub1itr).get_start() >= (*sub2itr).get_start())
		{
			sub2itr++;
			if (sub2itr == sub2ptr->mos.end() )
				done = true;
		}

		if (!done)
		{
					bool donebig = false;
					while (!donebig && (*sub1itr).get_start() < (*sub2itr).get_start())
					{
						sub1itr++;
						if (sub1itr == sub1ptr->mos.end() )
							donebig = true;
					}
					sub1itr--;//went one to far in the last loop
			if ((*sub1itr).get_stop() < (*sub2itr).get_stop())
			{
				occur one((*sub1itr).get_start(),(*sub2itr).get_stop());
				mos.push_back(one);
				//could update support here, but I chose to do it seperately based on window size
			}
			sub1itr++;
			sub2itr++;
		}
	}while (!done && sub1itr != sub1ptr->mos.end() && sub2itr != sub2ptr->mos.end());
//	output(cout);
}

void mos_item::recordtimestamps(const moitem* fcsub1, const moitem* fcsub2)
{//serial, record the miminal occurrences where the two subitems are both contained 
	//within the same timestamp
	//used to generate the minimal occurrences for timestamp items
	const mos_item *sub1ptr;
	sub1ptr = static_cast<const mos_item*>(fcsub1);
	const mos_item *sub2ptr;
	sub2ptr = static_cast<const mos_item*>(fcsub2);

	const_occur_list_ITR sub2itr = sub2ptr->mos.begin();
	const_occur_list_ITR sub1itr = sub1ptr->mos.begin(); 

	const_occur_list_ITR first, second;
	bool order = true;
	do
	{
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
		bool donebig = false;
		while (!donebig && (*first).get_start() <= (*second).get_start()  )
		{
			first++;
			if (first == sub1ptr->mos.end() && order || first == sub2ptr->mos.end() && !order )
				donebig = true;
		}
		first--;//went one to far in the last loop
 
		bool added = false;
		if ( (*first).get_start() == (*second).get_start() 
			&& (*first).get_stop() == (*second).get_stop()
			&& (*first).get_start() == (*first).get_stop())
		{//both occur within the same timestamp
			occur one((*first).get_start(),(*second).get_stop());
			mos.push_back(one);
			added = true;
			//could update support here, but I chose to do it seperately based on window size
		}
 
		if (order)
		{
			sub1itr=first;
			sub1itr++;
			if (added)
				sub2itr++;
		}
		else
		{
			sub2itr=first;
			sub2itr++;
			if (added)
				sub1itr++;
		}
 
	}while (sub1itr != sub1ptr->mos.end() && sub2itr != sub2ptr->mos.end());
//	output(cout);
}
int mos_item::getnumberofevents() const
{
	int esize=0;
	for( const_t_stamp_vec_ITR epi_itr = episode.begin(); epi_itr != episode.end(); epi_itr++)
	{
		esize+= (*epi_itr).get_number_events();
	}

	return esize;
}

moitem* mos_item::generate_serial_candidate(const moitem* E) const
{
	moitem * Z = new mos_item;
	for (const_t_stamp_vec_ITR epi_itr = episode.begin(); epi_itr != episode.end(); ++epi_itr)
	{
		Z->insertepisode(*epi_itr);
	}
	//E contains only one timestamp in its closure and episode
	const mos_item *sitemptr = static_cast<const mos_item*>(E);
	const_t_stamp_vec_ITR Eeitr = sitemptr->episode.begin();
	Z->insertepisode(*Eeitr);
	return Z;
}

moitem* mos_item::generate_serial_rule_candidate(const moitem* E) const
{
	moitem * Z = new mos_item;
	for (const_t_stamp_vec_ITR epi_itr = episode.begin(); epi_itr != episode.end(); ++epi_itr)
	{
		Z->insertepisode(*epi_itr);
	}

	//E contains only one timestamp in its episode
	const mos_item *sitemptr = static_cast<const mos_item*>(E);
	const_t_stamp_vec_ITR Eeitr = sitemptr->episode.begin();
	Z->insertepisode(*Eeitr);
	return Z;
}

moitem* mos_item::get_last_episode() const
//return the last timestamp episode and closure as a new moitem ptr
//used in generating serial candidates
{
	moitem* temp = new mos_item;
	const_t_stamp_vec_ITR i = episode.end();
	i--;
	temp->insertepisode((*i));

 return temp;
}

bool mos_item::issubsetofY(const moitem* Y) const
{
	//return true if the current moitem is a subset of moitem Y
	const mos_item* sitemptr = static_cast<const mos_item*>(Y);
	bool subset = true;
	const_t_stamp_vec_ITR itr = episode.begin();
	const_t_stamp_vec_ITR Yi = sitemptr->episode.begin();
	const_t_stamp_vec_ITR Yend = sitemptr->episode.end();
	do
	{
		const_t_stamp_vec_ITR in = find(Yi, Yend, (*itr));
		if (in == sitemptr->episode.end())
		{
			subset = false;
		}
		else
		{
			Yi = in;
			Yi++;
			itr++;
		}
	}	while (subset && itr != episode.end() && Yi != sitemptr->episode.end());
	if (subset && itr != episode.end() && Yi == sitemptr->episode.end())
		subset = false;
	return subset;
}//end is subset of Y

t_stamp_vec mos_item::get_episode(int ignore) const
{
	return episode;
}

time_stamp mos_item::get_first_time_stamp() const
{
 return *episode.begin();
}

time_stamp mos_item::get_last_time_stamp() const
{
	const_t_stamp_vec_ITR i = episode.end();
	i--;
 return *i;
}

//called from support_subs_serial
time_stamp mos_item::get_ith_time_stamp(int i) const
	//return the ith event in the moitem
{
	return episode[i];
}

int mos_item::getepisodelen() const
{ return episode.size(); }

void mos_item::create_1_ante(moitemset& A, const event_set& T) const
{//put the timestamps in the current moitem that are in the constraint file T
	//as separate timestamps into the moitemset A
	for (const_t_stamp_vec_ITR i = episode.begin(); i != episode.end(); i++)
	{
		if ( (*i).matchB(T))
		{
			moitem * current = new mos_item;
			current->insertepisode(*i);
			A.insertitem(current);
		}
	}
}//create_1_ante

//double mos_item::get_max_support(const fc_type& fc, int i, int k, char type)
double mos_item::get_max_support(const fc_type& fc)
 //get the max support = support of a larger episode that contains this episode if there is one, otherwise 0
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


void mos_item::output_episode(ostream& outs) const
{
	outs<<"{";
	const_t_stamp_vec_ITR e = episode.begin();
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


void mos_item::output(ostream& outs) const
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

bool mos_item::fast_gen_support_subs(const moitemset& f_l, int k) const
{
	bool support = true;

	int y = 0,x=0;
	while (support && y < k-1)
	{
		moitem * beta = new mos_item;
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

		if (!f_l.does_contain_serial(beta))
		{
			support = false;
		}
		y++;
		delete beta;
	}
return support;
}//end fast_gen_support_sub

bool mos_item::equalitem(const moitem* Y) const
{
	const mos_item* sitemptr = static_cast<const mos_item*>(Y);
	return (episode == sitemptr->episode);
}

void mos_item::sortit()
{
	sort(episode.begin(), episode.end());
}

//for parallel episodes

//called from support_subs_parallel
event mos_item::get_ith_event(int i) const
	//return the ith event in the moitem
{
	event e;
	return e;
}

event mos_item::get_last_event() const
{ 
	event e;
 return e;
}

event mos_item::get_first_event() const
{
	time_stamp first = *episode.begin();
	return first.get_first_event();
}

moitem* mos_item::generate_parallel_candidate(const event& E) const
{
	moitem * Z = new mos_item;
	return Z;
}

void mos_item::insertepisode(const event& e)
{}

event_set mos_item::get_episode() const
{
	event_set s;
	for (const_t_stamp_vec_ITR i = episode.begin(); i != episode.end(); i++)
	{
		(*i).addeventstoset(s);
	}

	return s;
}

