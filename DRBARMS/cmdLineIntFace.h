/*
filename: momain.cpp
author: Sherri Harms
last modified: 7/29/02
description: The implementation for Minimal Occurrences with constraints,
timelags, and RAR (Representative Episodal Association Rules)
******************************************************************************
Known bugs:
  The constraints are all disjuctive singular events.
  input validation is not done
*******************************************************************************
Assumptions:

	ANTECEDENT EVENTS AND CONSEQUENCE EVENTS ARE NEVER EVER MIXED TOGETHER!!!!
	(per Tsegaye.)

  Parameters have been set (done in the main program).  These include:

  Data file istream& fin has been opened.  It is a flat file with one record for each event.
     The record contains the time stamp followed by the event.
	 The file must be in increasing time order, but may include events at the same
	 time stamp.
  Parameters from the parameter file (in the order they must be specified):
		Sample: 5 .625 4 1 3 s 	read by: fin>>min_freq>>min_confidence>>wina>>timelag>>winc>>type;

		int min_fr - the minimum frequency.  Only episodes that are at least this 
			 frequent will be kept from the list of candidate episodes. Must be > 0.
			 This is a raw count and must be greater than 0.
			 This program could be modified to use a percent of windows instead of the raw count.
		double min_confidence - the minimum confidence.  Only rules that are
			at least this confident will be kept from the list of Rep. Episodal AR. 
			Must be between 0 and 1.
		int wina - the size of the sliding antecedent window.  Must be greater than 0.
		int timelag - the maximum number of steps between the start of the antecedent episode and the start of the consequence episode.
			Must be greater than 0.
		int winc - the size of the sliding consequence window.  Must be greater than 0.
		char type - 
			p for injective parallel, 
			q for parallel with repeats allowed
			s for injective serial
			t for serial with repeats allowed

  Other parameters from the user:
		antecedent eventlist filename
		consequent eventlist filename
		input file name
		output file name
		type of rules:
			1 - all rules
			2 - representative episodal association rules
		Type of lag - 
			m for maximum lag
			f for fixed lag

		
  itemset& fca_k_b will be initialized.  This is the current set of antecedent candidates that meet the constraints
  itemset& fcc_k_b will be initialized.  This is the current set of consequence candidates that meet the constraints
  itemset& fa[k] will keep the items from fca_k_b that are frequent
  itemset& fc[k] will keep the items from fcc_k_b that are frequent
 const event_set& A - the set of antesedent constraints.  Read from a separate file, such as ant.txt.
 const event_set& C - the set of consequence constraints.  Read from a separate file, such as cons.txt.
     Currently, the constraints are singletons - any event not in the list
	 will not be included in the analysis.  This needs to be modified to allow
	 for constraints in DNF format.  
*******************************************************************************
*******************************************************************************
Global Data structures:
class event - stores a event as a string.

class moitem - stores:
	 int support - the number of occurrences of this episode
 	 occur_list mos; - the list of occurences of this episode
	int blockstart; -used in the candidate generation process
class mop_item: inherits from item + stores:
	 a vector of events (called an episode), 
class mos_item: inherits from item + stores:
	 a vector of timestamps (called an episode), 
class mop_ruleitem: inherits from item + stores:
	 a vector of events (called the ant), 
	 a vector of events (called the cons), 
	int antsupport; support of the antecedent
class mos_ruleitem: inherits from item + stores:
	 a vector of timestamps (called the ant), 
	 a vector of timestamps (called the cons), 
	int antsupport; support of the antecedent

class moitemset - stores a vector of item pointers.
  also uses a static variable largest_item_size to
  keep track of the size of the largest item

class B_WIN - stores:
	event_deque B_win - the set of events that are currently in the window;
	int_deque time_stamp - the set of timestamps for the matching set of events;

class sB_WIN - stores:
	time_stamp_deque B_win - the set of timestamps that are currently in the window;

class morule - stores a single parallel episodal association rule, with:
	vector of events for the  ant;
	vector of events for the  cons;
	double confidence;
	double freq;

class moruleset - stores the set of parallel Representative episodal association rules, with:
    vector of rules for the set of parallel rules

class mos_rule - stores a single serial episodal association rule, with:
	vector of timestamps for the  ant;
	vector of timestamps for the  cons;
	double confidence;
	double freq;

class mos_ruleset - stores the set of serial Representative episodal association rules, with:
    vector of rules for the set of serial rules

class timestamp - stores a set of events that occur together within one timestamp from 
	multiple data sets, for serial items only, with:
	event_list t_stamp;

class occur - the start and end time of a single occurence
	int start;
	int stop;

Global variables:
vector<itemset> fca - in main, used to store the sets of candidate antecedent items, 
	where each set contains items with the same number of events
vector<itemset> fcc - in main, used to store the sets of candidate consequence items, 
	where each set contains items with the same number of events
vector<itemset> fa - in main, used to store the sets of frequent antecedent items,
    where each set contains items with the same number of events in the episode, 
vector<itemset> fc - in main, used to store the sets of frequent consequence items,
    where each set contains items with the same number of events in the episode, 
  
Local Data Structures:
Local variables: 
TimeF - used to create the timestamps from the frequent individual events, knowing
	which events come from which data source, and only using the events that meet the constraints
TimeFvec - vector of TimeF's, one for each data source

*******************************************************************************/
#ifndef CmdLineIntFace_H
#define CmdLineIntFace_H


#pragma warning(disable:4786)
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <ctime>
#include "event.h"
#include "timestamp.h"
#include "occur.h"
#include "moitem.h"
#include "moitemset.h"
#include "morule.h"
#include "moruleset.h"
#include "moserialrule.h"
#include "moserialruleset.h"
using namespace std;

typedef vector<event> event_vec;

class CmdLineIntFace 
{
public:
	int runMo();
	//Runs the algorithm through the command line 

	void get_pars(istream& fin, int& min_freq, double& min_confidence, int& wina, int& lag, int& winc, char& type);
	//get the parameters from a file
	//read by: fin>>min_freq>>min_confidence>>wina>>timelag>>winc>>step>>type;

	void get_events(istream& fin, event_vec_vec& TimeFvec, const event_set& A, const event_set& C);

	void get_events(istream& fin, event_vec& F);
	//get the list of events from each data source separately

	void getsingles(t_stamp_vec& SingleA, t_stamp_vec SingleC, const event_set& A, const event_set C, int sources);
	//create timestamp single events by taking the antecedent and consequence events separately

	//may need these if the system is modified so that ruleconstraints are not required
	//void getsingles(const event_set& A, const event_set& C);
	//create candidate timestamp events in fca and fcc for antecedent and consequence event lists

	//void crossTimeevents(const event_vec_vec& TimeFvec, t_stamp_vec& TimeF, int sources);
	//create timestamp events by taking the cross product with the events from the separate data sources

	//void reduce (event_vec_vec& TimeFvec, const moitemset& fck, int& sources);
	//void crossfcTime(const event_vec_vec& TimeFvec, moitem_set& fcc_k, int sources);
	//create timestamp events taking the cross product with the FREQUENT events fron the separate data sources

	void get_constraints(istream& fin, event_set& B);
	//get the constraints from a file
	//NEED to change to DNF
	bool intersects(const event_set& A, const event_set& C);
	void outputparams(int win, int winc, int lag, char lag_type, ostream& outstream);
	void outputEpisodeCounts(double FrequencyThreshold, int Candidates, int FrequentEpisodes, int Iterations, unsigned int Time, ostream& outs);
	void outputRuleCounts(double min_confidence, int RuleCount, unsigned int Time, int Windows, ostream& outs);
	void outputtype(char type, ostream& out,int ans);
	void output_constraints(ostream& out, event_set& B);
	//output the constraints from a file
	//NEED to change to DNF


	void Read_Parallel_Data(ifstream& fin, ostream& outs, const event_set& A, const event_set& C);
	void Read_Serial_Data(ifstream& fin, ostream& outs, const event_set& A, const event_set& C);
	//generate the first set of minimal occurrences from the file 

	void Generate_Parallel_CTMINEPI(ifstream& fin, int min_fr, int& k, ostream& outs, int wina, int winc, const event_set& A, const event_set& C, char type);

	void Generate_Serial_CTMINEPI(ifstream& fin, int min_fr, int& k, ostream& outs, int wina, int winc, const event_set& A, const event_set& C, char type, int sources, const event_vec_vec& TimeFvec);

	void Generate_MOWCATL_RULES(ostream& out, double min_conf, int maxk, int lag, int min_fr, char type, const event_set& T, const event_set& C,int ans, char lag_type);
	//generate the CT MINEPI representative association rules
private:
	vector<moitemset> fca;//candidate ant items
	vector<moitemset> fa; //frequent ant items
	vector<moitemset> fcc; //candidate consequent items
	vector<moitemset> fc;  //frequent cons items
	vector<moitemset> fac;  //frequent rule items
	moitemset garbage;

};

#endif //CmdLineIntFace_H

