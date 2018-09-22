/*
filename: moitemset.h
author: Sherri Harms
last modified: 7/20/01
description: Class moitemset implementations for parallel 
episodes. This is the implementation FILE moitemset.cpp for the class moitemset.
 	Values of this type are sets of moitem pointers,
	where an moitem is a composite of 
	episode, support, and a list of occurrences

	This implementation uses vectors for the moitemset.
******************************************************************************
Known bugs:
  The constraints are all disjuctive singular events.  
  (need to add Step 5 from alg 5)
  input validation is not done
*******************************************************************************

Local Data Structures:
vector<moitemset> split_fac - used in the representative morule generation process
   has the items from fa and fc, separated by total length of the ant & cons. combined
*****************************************************************************
moitemset operations include:
output, constructors, clear, insertitem, generate parallel moitemset fc[k],
generate serial moitemset fc[k], generate candicates fcc[k+1],
and moitemset operations used in generating the representative episodal association rules.
*******************************************************************************/

#ifndef moITEMSET_H
#define moITEMSET_H

#include "event.h"
#include "moitem.h"
#include "moparallelitem.h"
#include "moserialitem.h"
#include "mop_ruleitem.h"
#include "mos_ruleitem.h"
#include "moserialrule.h"
#include "moserialruleset.h"
#include "morule.h"
#include "moruleset.h"
#include "timer.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
using namespace std;

typedef vector<moitem> moitem_set;
typedef vector<event> event_set;
typedef vector<event> event_vec;
typedef vector<event_vec> event_vec_vec;
typedef vector<event_vec>::const_iterator const_event_vec_vec_ITR;
typedef deque<event> event_deque;
typedef vector<moitem *> moitem_vec;
typedef vector<moitem *>::iterator  moITEM_ITR;
typedef vector<moitem *>::const_iterator  const_moITEM_ITR;
typedef vector<moitem *>::reverse_iterator  rmoITEM_ITR;
typedef vector<moitem *>::const_reverse_iterator  rconst_moITEM_ITR;
typedef vector<moITEM_ITR> int_vec;
typedef vector<moitemset> fc_type;

//Ê²Ã´ÊÇitemset?
class moitemset
{
public:

	void cleanup(); //delete the used memory in the process of clearing the itemset;

	static void setantlargest(int in_size) {largest_ant_size=in_size;}
	//Description: static function to set the size of the largest item in all itemsets
	//Precondition:in_size is valid integer >0
	//Postcondition: largest_item_size has its value set

	static int seeantlargest() {return largest_ant_size;}
	//Description: accessor function for largest_item_size

	static void setconslargest(int in_size) {largest_cons_size=in_size;}
	//Description: static function to set the size of the largest item in all itemsets
	//Precondition:in_size is valid integer >0
	//Postcondition: largest_item_size has its value set

	static int seeconslargest() {return largest_cons_size;}
	//Description: accessor function for largest_item_size

	static void setnum_records(int in_size) {num_records=in_size;}
	//Description: static function to set the number of records
	//Precondition:in_size is valid integer >0
	//Postcondition: num_records has its value set

	static void incrnum_records(int in_size) {num_records+=in_size;}
	//Description: static function to add to the number of records
	//Precondition:in_size is valid integer >0
	//Postcondition: num_records has its value set

	static void decrnum_records(int in_size) {num_records-=in_size;}
	//Description: static function to subtract to the number of records
	//Precondition:in_size is valid integer >0
	//Postcondition: num_records has its value set

	static int seenum_records() {return num_records;}
	//Description: accessor function for num_records

	void clear();
	//Description:  clear the moitemset - back to an empty moitemset

	void insertitem(moitem* p);
	//Description:  add the moitem pointer p to the moitemset
	//Preconditon: moitem pointer p must point to a existing valid moitem

	void get_parallel_episodes(const event_set& B);
	//Description: Add to the moitemset an moitem for each event in F that matches the constraints in B
	//Precondition: event_set B contains the events that meet the constraints
	//CHANGE when using DNF constraints

	void get_serial_episodes(const event_set& B);
	//Description: Add to the moitemset an moitem for each event in F that matches the constraints in B
	//Precondition: event_set B contains the events that meet the constraints
	//CHANGE when using DNF constraints

//	void get_time_stamp_episodes(const event_vec_vec& TimeFvec, const moitemset& fc_k, const event_set& B, int sources);

	t_stamp_vec accessitems() const;
	//return the list of episodes as a vector of timestamps

//	void resetfreqs();
	//reset freqs  - used when getting the first timestamp serial episodes 

	moitem* get_first_item() const;
	//Return a pointer to the first moitem in the moitemset

//	void convert(const t_stamp_vec& TimeF);

	event_set getevents() const;

	void bringinnew(const event& E, int t);
	//reads in the new events at the current time stamp
	//called in the read new data process

void addtimestampitems(const event_vec_vec& TimeFvec, const event_set& B, int sources, int win);
//	void addtimestampitems(int min_fr);
	//add timestamp items, used for serial processing

//	void bringinnewserial(const event& E, int t);
	//reads in the new events at the current time stamp
	//called in the read new data process

	void addfreq(moitemset& fcc_k_b, moitemset& garbage, int min_fr);
	//add the frequent candidates to the frequent item set, add the infrequent to garbage


	void Gen_mo(moitemset& fc_k_b, int k, char type);
	//generate the fc[k] moitemset for parallel and serial items
	//see the published algorithm
	//CHANGE when constraints are converted to DNF

//	void Gen_Parallel_mo(moitemset& fc_k_b, int k, char type);
//	void Gen_Serial_mo(moitemset& fc_k_b, int k, char type);
	//generate the fc[k] moitemset for serial items
	//see the published algorithm
	//CHANGE when constraints are converted to DNF

	void Update_Support(int win);
	//count how many minimal occurrences occur within the win width

	void fast_generate_parallel_candidates(moitemset& fc_k_b, char type, int k);
	//generate the next set of candidates from fc_k_b crossed with F, for a given type, 
	//also used to generate candidates from the morule-generation process, which generates base items.
	void fast_generate_serial_candidates(moitemset& fc_k_b, char type, int k);
	//generate the next set of candidates from fc_k_b crossed with F, for a given type, 
	//also used to generate candidates from the morule-generation process, which generates base items.
	void generate_parallel_candidates(const moitemset& fc_k_b, char type, const event_set& B, int k);
	//generate the next set of candidates from fc_k_b crossed with B
	//also used to generate candidates from the morule-generation process, which generates base items.
	//CHANGE when constraints are converted to DNF
	void generate_serial_candidates(const moitemset& fc_k_b, char type, const moitemset& B1, int k);
	//generate the next set of candidates from fc_k_b crossed with constraints B
	//also used to generate candidates from the morule-generation process, which generates base items.
	//CHANGE when constraints are converted to DNF

	void fast_generate_parallel_rule_candidates(moitemset& f_l, const moitem* Z, char type, int l);
	void generate_parallel_rule_candidates(const moitemset& A, const moitem* Z, char type, const event_set& B, int k);
	//generate the subsets of serial moitem Z, that meet constraints B
	//used only from the morule-generation process.
	//CHANGE when constraints are converted to DNF

	void fast_generate_serial_rule_candidates(moitemset& f_l, const moitem* Z, char type, int l);
	void generate_serial_rule_candidates(const moitemset& A, const moitem* Z, char type, moitemset B1, int k);
	//generate the subsets of serial moitem Z, that meet constraints B
	//used only from the morule-generation process.
	//CHANGE when constraints are converted to DNF

	moitem* findsmallest(const fc_type& fc, const moitem* X) const;
	//used in the representative morule generation process to find the smallest
	//episode that contains moitem X from all of the itemsets

	void combine(const moitemset& fa, moitemset& garbage, const fc_type& fc, char type, int lag, int min_fr, char lag_type);
//combine the ant and cons into ruleitems

	void putinsplit(fc_type& split_fc, int& k) const;

	void loop1parallel(const fc_type& split_fc, double c, moruleset& REAR, int i, int k, char type, const event_set& T, const event_set& C);
	//the outer loop used in the representative morule generation process
	//called from the main program as:	split_fc[i].loop1(split_fc, min_conf, REAR, i, k, type, B);
	void loop1serial(const fc_type& split_fc, double c, mos_ruleset& REAR,  int i, int k, char type, const event_set& T, const event_set& C);
	//the outer loop used in the representative morule generation process
	//called from the main program as:	split_fc[i].loop1(split_fc, min_conf, REAR, i, k, type, B);
	void addrules(double c, moruleset& REAR, int i);
	void addrules(double c, mos_ruleset& REAR, int i);

	double computelevelsupport(const moitem* Z) const;
	// used in the representative morule generation process
	//when working with one moitem Z, 
	//called from an moitem's get_max_support member function
	//to find the frequency for an episode that contains moitem Z
	// at this closure length (or level)
	//get the max support = support of a larger episode that contains this episode if there is one, otherwise 0

	bool does_contain_parallel(const moitem* Y) const;
	bool does_contain_parallel_rule_item(const moitem* Y) const;
	bool does_contain_serial_rule_item(const moitem* Y) const;
	bool does_contain_serial(const moitem* Y) const;
	//return true if the calling moitemset contains moitem Y
	//called in the candidate generation process
	
	bool empty() const;
	//return true if the moitemset is empty

	int access_size() const;
	int access_num_records() const;

	void output(ostream& outs) const;
	void output_episodes(ostream& outs) const;

    moitemset( );
    //Constructor.

	~moitemset(){
//		delete timer;
	}

    friend ostream& operator << (ostream& outs, const moitemset& the_object);
    //Overloads the << operator for output values of type moitemset.
    //Precondition: If outs is a file output stream, then outs as 
    //already been connected to a file.

	void setTimer(Timer* t);
	//Sets this objects timer data member.

private:
	bool matchp(moitem* cand, char type) const; 
	//return true if the candidate is a parallel injunctive episode
	//called in the candidate generation process
	//calls the individual moitem matchp member function
	
	bool matchs(moitem* cand, char type) const; 
	//return true if the candidate is a serial injunctive episode
	//called in the candidate generation process
	//calls the individual moitem matchs member function

	bool support_subs_parallel(const moitem* Y, const moitemset& fc_k_b, int k) const;
	bool support_subs_serial(const moitem* Y, const moitemset& fc_k_b, int k) const;
	//return true if the moitemset fc_k_b contains all k sized subsets of Y
	//called in the candidate generation process
	
	moitem_vec is;
	static int largest_ant_size;
	static int largest_cons_size;
	static int num_records;
	static int indexofA(const event& A, const event_set& f_list, int size);
	static int indexoftimeA(const time_stamp& A, const t_stamp_vec& TimeF, int size);
	Timer* timer;

};

#endif //ITEM_H



