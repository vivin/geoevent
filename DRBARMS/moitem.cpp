/*
filename: moitem.cpp
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
#pragma warning(disable:4786)

#include "moitem.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
using namespace std;

int moitem::Aindex(const event& A, const event_set& f_list, int size)
{
	int i = 0;
	bool found = false;
	int save=size;
	while (!found && i < size)
	{
		if (A == f_list[i])
		{
			found = true;
			save = i;
		}
		i++;
	}
	return save;
} //end indexofA

int moitem::indexoftimeA(const time_stamp& A, const t_stamp_vec& TimeF, int size)
{
	int i = 0;
	bool found = false;
	int save=size;
	while (!found && i < size)
	{
		if (A == TimeF[i])
		{
			found = true;
			save = i;
		}
		i++;
	}
	return save;
} //end indexoftimeA

  /*void moitem::insertepisode(const epi_ptr&  e)
{
	episode.push_back(e);
}*/

bool moitem::mosempty() const
{
	return (mos.size() == 0);
}

double moitem::get_support() const
{
	return support;
}


double moitem::get_density_ratio() const
{
	return density_ratio;
}

double moitem::get_local_density() const
{
	return local_density;
}

double moitem::get_relative_density_ratio() const
{
	return relative_density_ratio;
}

void moitem::set_support(int in_support)
{
	if (in_support > 0)
		support = in_support;
	else 
		support = 0;
}

void moitem::set_density_ratio(double in_density)
{
	if (in_density > 0)
		density_ratio = in_density;
	else
		density_ratio = 0;
}

void moitem::set_local_density(double in_density)
{
	if (in_density > 0)
		local_density = in_density;
	else
		local_density = 0;
}

void moitem::set_relative_density_ratio(double in_density)
{
	if (in_density > 0)
		relative_density_ratio = in_density;
	else
		relative_density_ratio = 0;
}


void moitem::update_support(int win)
{
	bool change = false;
	occur_list newlist;
	support = 0;
	occur_list_ITR i = mos.begin(); 
	while (i != mos.end())
	{
		if ( ( (i->get_stop()) -  (i->get_start()) ) < win)
		{
			support++;
			newlist.push_back(*i);
		}
		else
		{
			change = true;
		}
		i++;
	}
	if (change)
		mos = newlist;
}

moitem::moitem( )
{
	support = 0;
	occur now;
	mos.push_back(now);
	mos.clear();
	blockstart = 1;
}

void moitem::resetfreq() 
{
	support = 0;
	blockstart = 1;
}

//virtual functions for parallel rule items
void moitem::insertant(const event_set&  e) {}
void moitem::insertcons(const event_set&  e) {}
void moitem::recordmobetween(const moitem* faitem, const moitem* fcitem, int lag, char lag_type) {}
event_set moitem::get_antepisode() const
{
	event_set s;
	return s;
}

event_set moitem::get_consepisode() const
{
	event_set s;
	return s;
}
void moitem::insertantepisode(const event&  e)
{}
moitem* moitem::generate_parallel_candidate(const event& E, const moitem* Z) const
{
	moitem* p ;
	cerr<<"Trying to generate parallel rule item candidate, but did not make it, exiting";
	exit(1);
	return p;
}

void moitem::set_antsupport(int in_support)
{}
int moitem::get_antsupport() const
{return 0;}

void moitem::set_conssupport(int in_support)
{}
int moitem::get_conssupport() const
{return 0;}

double moitem::cal_density_ratio() const
{return 0.0;}

double moitem::cal_local_density() const
{return 0.0;}

void moitem::insertant(const t_stamp_vec& e) {};
void moitem::insertcons(const t_stamp_vec& e) {};
t_stamp_vec moitem::get_antepisode(int ignore) const
{
	t_stamp_vec s;
	return s;
}

t_stamp_vec moitem::get_consepisode(int ignore) const
{
	t_stamp_vec s;
	return s;
}

void moitem::insertantepisode(const time_stamp&  e) {};

moitem* moitem::generate_serial_rule_candidate(const moitem* E, const moitem* Z) const
{
	moitem* p ;
	cerr<<"Trying to generate serial rule item candidate, but did not make it, exiting";
	exit(1);
	return p;
}

int moitem::getantnumberofevents() const
{return 0;}
int moitem::getconsnumberofevents() const
{return 0;}


occur moitem::get_last_occurrence() const
{
	const_occur_list_ITR i = mos.end(); 
	i--;
	return (*i);
}
