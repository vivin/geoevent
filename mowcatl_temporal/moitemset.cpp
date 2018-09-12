/*
 filename: moitemset.cpp
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

#pragma warning(disable:4786)
#include <stdexcept>
#include <stdlib.h>
#include "moitem.h"
#include "moitemset.h"
#include <deque>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
using namespace std;

int moitemset::largest_ant_size;
int moitemset::largest_cons_size;
int moitemset::num_records;

/**************************************************************
 Functions for injunctive itemsets

 ***************************************************************/

int moitemset::indexofA(const event& A, const event_set& f_list, int size) {
	int i = 0;
	bool found = false;
	int save = size;
	while (!found && i < size) {
		if (A == f_list[i]) {
			found = true;
			save = i;
		}
		i++;
	}
	return save;
} //end indexofA

int moitemset::indexoftimeA(const time_stamp& A, const t_stamp_vec& TimeF,
		int size) {
	int i = 0;
	bool found = false;
	int save = size;

	//testing
	//cout<<A;

	while (!found && i < size) {
		//cout<<i<<" "<<TimeF[i]<<" ";
		if (A == TimeF[i]) {
			found = true;
			save = i;
		}
		i++;
	}

	//cout<<endl<<endl;
	//system("pause");
	return save;
} //end indexoftimeA

int moitemset::access_size() const {
	return is.size();
}

int moitemset::access_num_records() const {
	return num_records;
}

void moitemset::clear() {
	is.erase(is.begin(), is.end());
}

void moitemset::cleanup() {
	try {
		moITEM_ITR gitr = is.begin();
		while (gitr != is.end()) {
//			 	(*gitr)->output(cout);
			(*gitr)->clear();
			delete *gitr;
			*gitr = 0;
			++gitr;
		}
		clear();
	} //end try
	catch (...) {
//		cout<<"exception occurred"<<endl;
	}

}

void moitemset::insertitem(moitem *p) {
	is.push_back(p);
}

void moitemset::get_parallel_episodes(const event_set& B) {
	moitem *p;
	for (event_set::const_iterator e = B.begin(); e != B.end(); ++e) {
		p = new mop_item;
		p->insertepisode(*e);
		insertitem(p);
	}
//	output_episodes(cout);
//	cout<<endl;
}

void moitemset::get_serial_episodes(const event_set& B) {
	moitem *p;
	for (event_set::const_iterator e = B.begin(); e != B.end(); ++e) {
		p = new mos_item;

		time_stamp one(*e);
		p->insertepisode(one);
		insertitem(p);
	}
//	output_episodes(cout);
//	cout<<endl;
}

void moitemset::addtimestampitems(const event_vec_vec& TimeFvec,
		const event_set& B, int sources, int win)
		//develop timestamps from the frequent events that
//POSSIBLE ENHANCEMENT: must occur within the window width
//kept out intentionally for now, so that we could iterate over the window width
// and not have to recompute the support values

//add timestamp items, used for serial processing

//POSSIBLE ENHANCEMENT: use closures
		{

	//testing
	//fc_k.output(cout);

	int choose = 2;
	while (choose <= sources) {

		if (timer->isTimeOver()) {
			return;
		}

		int pawn = choose - 1;
		vector<int> index;
		for (int s = 0; s < choose; s++)
			index.push_back(s);

		//start the iters array with the beginning iterators
		vector<vector<event>::const_iterator> iters;

		bool more_pawns_to_move = true;
		while (more_pawns_to_move) {

			if (timer->isTimeOver()) {
				return;
			}

			bool more_moves_for_current_pawn = true;
			while (more_moves_for_current_pawn) {

				if (timer->isTimeOver()) {
					return;
				}

				//testing
				//for (int testitr = 0; testitr <choose;testitr++)
				//	cout<<index[testitr]<<" ";
				//cout<<endl;
				//system("pause");

				iters.clear();
				for (int itrarr = 0; itrarr < choose; itrarr++) {
					iters.push_back(TimeFvec[index[itrarr]].begin());
				}
				bool more_itr_settings = true;
				int mover = choose - 1;

				while (more_itr_settings) {

					if (timer->isTimeOver()) {
						return;
					}

					while (iters[mover] != TimeFvec[index[pawn]].end()) {

						if (timer->isTimeOver()) {
							return;
						}

						//add the timestamp created from the current iterator values
						time_stamp temp;
						for (int start = 0; start < choose; start++)
							temp.insert(*iters[start]);
						temp.sort();

						//testing
						//	cout<<"temp = ";
						//	temp.output(cout);
						//	cout<<" ";

						bool needtoadd = temp.matchB(B);
						if (needtoadd) {
							moitem *p;
							p = new mos_item;
							p->insertepisode(temp);
							insertitem(p);

							//testing
							//		cout<<"Made it: ";
							//		p->output(cout);
							//		system("pause");
						}

						//increment the mover setting
						iters[mover]++;
					}					//add one moitem

					//increment the iterators
					int next = mover;
					do {
						next--;
						if (next >= 0)
							iters[next]++;
					} while (next >= 0
							&& iters[next] == TimeFvec[index[next]].end());
					if (next < 0)
						more_itr_settings = false;
					else
						for (int p = next + 1; p <= mover; p++)
							iters[p] = TimeFvec[index[p]].begin();
				}  //end while more itr settings

				//increment the current pawn
				index[pawn]++;
				if (index[pawn] == sources)
					more_moves_for_current_pawn = false;
			} //end whilemore moves for current pawn

			int nextpawn = pawn;
			int max = sources;

			while (nextpawn >= 0 && (index[nextpawn] == max)) {
				nextpawn--;
				max--;
				if (nextpawn >= 0)
					index[nextpawn]++;
			}
			if (nextpawn < 0)
				more_pawns_to_move = false;
			else {
				for (int j = nextpawn + 1; j <= pawn; j++)
					index[j] = index[j - 1] + 1;
			}
		}  //end while more pawns to move
		choose++;

//		system("pause");

	}  //end while choose<sources

	//now calculate the minimal occurrences for the timestamp items
	int k = 2;
	for (const_moITEM_ITR itemitr = is.begin(); itemitr != is.end();
			++itemitr) { //MUST BE PROCCESSED IN ORDER OF k - all episodes of length 2, then 3, etc.
						 //testing
						 //(*itemitr)->output(cout);
//		system("pause");

		if (timer->isTimeOver()) {
			return;
		}

		if ((*itemitr)->getnumberofevents() == k) {
			moitem* sub1 = (*itemitr)->getsub1withintimestamp(k - 1);
			moitem* sub2 = (*itemitr)->getsub2withintimestamp(k - 1);
			//(sub1)->output(cout);
			//(sub2)->output(cout);
			//system("pause");
			bool found = false;
			const_moITEM_ITR fcsub1 = is.begin();
			while (!found && fcsub1 != is.end()) {
				if ((*fcsub1)->equalitem(sub1))
					found = true;
				else
					fcsub1++;
			}

			found = false;
			const_moITEM_ITR fcsub2 = is.begin();
			while (!found && fcsub2 != is.end()) {
				if ((*fcsub2)->equalitem(sub2))
					found = true;
				else
					fcsub2++;
			}

			//testing
//			(*itemitr)->output(cout);
//			(*fcsub1)->output(cout);
//			(*fcsub2)->output(cout);
//			system("pause");

			/*			bool twins = false;
			 if ( (*fcsub1) == (*fcsub2) || (*fcsub1)->does_contain(*fcsub2) || (*fcsub2)->does_contain(*fcsub1))
			 twins = true;
			 */
			(*itemitr)->recordtimestamps(*fcsub1, *fcsub2);
			(*itemitr)->update_support(win);
			delete sub1;
			delete sub2;
		} else if ((*itemitr)->getepisodelen() > k) {
			k++;
		}
	}

	//testing - output all the Timestamp events
//	for (const_t_stamp_vec_ITR titr = TimeF.begin(); titr != TimeF.end(); titr++)
//	{
//		cout<<*titr<<"\t";
//	}
//	cout<<endl;
//	system("pause");
}

/*
 void moitemset::resetfreqs()
 //reset freqs  - used when getting the first timestamp serial episodes
 {
 for (moITEM_ITR itemitr = is.begin(); itemitr != is.end(); ++itemitr)
 (*itemitr)->resetfreq();
 }

 void moitemset::convert(const t_stamp_vec& TimeF)
 {
 for (const_t_stamp_vec_ITR t = TimeF.begin(); t != TimeF.end(); t++)
 {
 moitem* s = new mos_item(*t);
 insertitem(s);
 }
 }
 */

event_set moitemset::getevents() const {
	event_set temp;
	for (const_moITEM_ITR itemitr = is.begin(); itemitr != is.end();
			++itemitr) {
		event_set one = (*itemitr)->get_episode();
		for (event_set::const_iterator e = one.begin(); e != one.end(); e++)
			temp.push_back(*e);
	}
	return temp;
}

t_stamp_vec moitemset::accessitems() const
//return the list of episodes as a vector of timestamps
//called only when the items contain single timestamps
{
	t_stamp_vec allitems;
	for (const_moITEM_ITR itemitr = is.begin(); itemitr != is.end();
			++itemitr) {
		time_stamp one = (*itemitr)->get_first_time_stamp();
		allitems.push_back(one);
	}
	return allitems;
}

void moitemset::bringinnew(const event& E, int t)
//reads in the new events at the current time stamp
//called in the read new data process
		{
	for (const_moITEM_ITR itemitr = is.begin(); itemitr != is.end();
			++itemitr) {
		if ((*itemitr)->contains_event(E)) {
			(*itemitr)->bringinnew(t);
			break;	//only one item should contain this event, as
					//when we are reading from the file for parallel items,
					// we are reading only the singletons
		}
		//many of the events that we read will not be in the itemset (because of the constraints)
		//we are not concerned with these events, and they should be weeded out
		//before calling this function
	}
}

void moitemset::Gen_mo(moitemset& fc_k_b, int k, char type)
//update the minimal occurrences in the fca[k] and fcc[k] itemsets from
// the minimal occurrences in the fa[k-1] and fc[k-1] itemsets
//for parallel items
//see the published algorithm for Mannila et. al.
//I have enhanced this algorithm to allow for antecedent and consequent constraints
//CHANGE when constraints are converted to DNF
		{
	/*Declarations: */

	/*Initialization Phase: */

	/* Step 2. Record the minimal occurrences in fcc_k_b */

	for (moITEM_ITR itemitr = is.begin(); itemitr != is.end(); ++itemitr)
	// update the minimal occurrences
	// for each item,
	//find its two sub items in the fc[k-1] itemset and record the occurences between them
	//record the occurrence 

	//POSSIBLE ENHANCEMENT:
	//This could be changed to record only the minimal occurrences that occur within the window width
	// updating the support at the same time
	//this would be faster for a single possible win width
	//but the current method is faster when using a range of window widths
			{

		moitem* sub1 = (*itemitr)->getfirstsub(k);
		moitem* sub2 = (*itemitr)->getsecondsub(k);

//		(sub1)->output(cout);
//		(sub2)->output(cout);

		bool found = false;
		const_moITEM_ITR fcsub1 = fc_k_b.is.begin();
		while (!found && fcsub1 != fc_k_b.is.end()) {
			if ((*fcsub1)->equalitem(sub1))
				found = true;
			else
				fcsub1++;
		}

		found = false;
		const_moITEM_ITR fcsub2 = fc_k_b.is.begin();
		while (!found && fcsub2 != fc_k_b.is.end()) {
			if ((*fcsub2)->equalitem(sub2))
				found = true;
			else
				fcsub2++;
		}

		//testing
//		(*itemitr)->output(cout);
//		(*fcsub1)->output(cout);
//		(*fcsub2)->output(cout);
//		system("pause");

		bool twins = false;
		if ((*fcsub1) == (*fcsub2))
			twins = true;

		(*itemitr)->recordmobetween(*fcsub1, *fcsub2, twins);
		delete sub1;
		delete sub2;
		//testing
		//	(*item_set_itr)->output(cout);
		//	system("pause");
	}
}

void moitemset::addfreq(moitemset& fcc_k_b, moitemset& garbage, int min_fr) {

	for (moITEM_ITR item_set_itr = fcc_k_b.is.begin();
			item_set_itr != fcc_k_b.is.end(); ++item_set_itr)
			// find all episodes that are frequent
			{
		//testing
//		(*item_set_itr)->output(cout);
//		system("pause");

		if ((*item_set_itr)->get_support() >= min_fr) {
			insertitem(*item_set_itr);
		} else
			garbage.insertitem(*item_set_itr);
	}

}

void moitemset::addDensity(moitemset& fca_k_b, moitemset& garbage,
		const fc_type& fc, char type, int lag, double min_density,
		char lag_type) {

	bool done = false;
	int i;
	for (const_moITEM_ITR faitr = fca_k_b.is.begin(); faitr != fca_k_b.is.end();
			++faitr) {

		i = 0;
		done = false;
		try {
			done = (fc[i]).empty();
		} catch (out_of_range e) {
			done = true;
		}

		while (!done) {
			for (const_moITEM_ITR fcitr = fc[i].is.begin();
					fcitr != fc[i].is.end(); ++fcitr) {
				//Check if the algorithm has run past the max time allowed and exit if it has
				if (timer->isTimeOver()) {
					return;
				}

				moitem* p;
				switch (type) {
				case 'p':
				case 'q': {
					p = new mop_ruleitem;
					p->insertant((*faitr)->get_episode());
					int supp = (int) (*faitr)->get_support();
					p->set_antsupport(supp);
					p->insertcons((*fcitr)->get_episode());
					supp = (int) (*fcitr)->get_support();
					p->set_conssupport(supp);
					break;
				}
				case 's':
				case 't': {
					p = new mos_ruleitem;
					p->insertant((*faitr)->get_episode(1));
					int supp = (int) (*faitr)->get_support();
					p->set_antsupport(supp);
					p->insertcons((*fcitr)->get_episode(1));
					supp = (int) (*fcitr)->get_support();
					p->set_conssupport(supp);
					break;
				}
				};

				p->recordmobetween(*faitr, *fcitr, lag, lag_type);
				double ratio = p->cal_denisty_ratio();
				p->set_density(ratio);
				if (ratio >= 1.2){
					insertitem(*faitr);
				}
				p->clear();
				delete p;
				p = 0;
				//garbage.insertitem(p);
				//garbage.insertitem(*faitr);
				//garbage.insertitem(*fcitr);
			}				//end of for for fcitr
			i++;

			try {
				if (i >= fc.size()) {
					//done = true;
					break;
				} else {
					done = (fc[i]).empty();
				}
			} catch (out_of_range e) {
				done = true;
			}
		}				//end of while (!done)
	}				// end of faitr

}	//end of while (!done)

void moitemset::Update_Support(int win)
//count how many minimal occurrences occur within the win width
		{
	for (moITEM_ITR item_set_itr = is.begin(); item_set_itr != is.end();
			++item_set_itr)
			// find all episodes that are frequent
			{
//testing
//	(*item_set_itr)->output(cout);
//	system("pause");

		(*item_set_itr)->update_support(win);
	}
}

void moitemset::fast_generate_parallel_candidates(moitemset& f_l, char type,
		int l) {// Algorithm 3 from Mannila, Toivonen and Verkamo, "Discovery of frequent episodes
// in event sequences" Department of Computer Science Series of Publications C,
// report C-1997-15, University of Helsinki, Finland
//with added closures

//parallel items have their events in lexigraphical sorted order
//serial items have their events in serial order 
	int k = 0;
	int current_block_start;
	moITEM_ITR current_start_itr = f_l.is.begin();
	moITEM_ITR old_i = f_l.is.begin();
	for (moITEM_ITR i_itr = f_l.is.begin(); i_itr != f_l.is.end(); ++i_itr) {
		current_block_start = k + 1;
		moITEM_ITR j_itr;
		if ((*i_itr)->blockstart != (*old_i)->blockstart) {
			current_start_itr = i_itr;
		}
		old_i = i_itr;

		j_itr = i_itr;

		while ((j_itr != f_l.is.end())
				&& ((*j_itr)->blockstart == (*i_itr)->blockstart)) {

			if (timer->isTimeOver()) {
				return;
			}

			//for injective episodes, don't add a duplicate event
			if ('q' == type || i_itr != j_itr) {

				//Step 1. create the candidate episodes - one at a time
				event e = (*j_itr)->get_last_event();
				bool needtoadd = true;
				moitem * alpha = (*i_itr)->generate_parallel_candidate(e);//creates new object
				//Testing:
				//			{
//			cout<<"Candidate = "<<*alpha<<endl;
//				system("pause");
				//			}

				//Step 2. Build and test subepisodes Beta that do not contain alpha[y]
				// Add the candidate if all the subepisodes are in f_l
				// do not add if the episodes that have sub-episodes that do not have min support

//				needtoadd= fast_gen_support_subs(alpha, f_l, l+1);
				needtoadd = alpha->fast_gen_support_subs(f_l, l + 1);

				//Step 3. Do not add the episodes where the closure of some k-subset of X contains X
				if (needtoadd) {
					//Testing:
					//cout<<"Made it = "<<*alpha<<endl;
					//system("pause");
					k++;
					alpha->blockstart = current_block_start;
					insertitem(alpha);
				} else {
					alpha->clear();
					delete alpha;
				}

			} //end if q or if i != j
			j_itr++;
		} //for one added candidate for an current episode
	} //for all current episodes
	/*//testing:
	 output_episodes(cout);
	 cout<<endl;
	 */
//	system("pause");

} //end fast generate parallel candidates

void moitemset::fast_generate_serial_candidates(moitemset& f_l, char type,
		int l) { // Algorithm 3 from Mannila, Toivonen and Verkamo, "Discovery of frequent episodes
// in event sequences" Department of Computer Science Series of Publications C,
// report C-1997-15, University of Helsinki, Finland
//with added closures

//parallel items have their events in lexigraphical sorted order
//serial items have their events in serial order 
	int k = 0;
	int current_block_start;
	moITEM_ITR current_start_itr = f_l.is.begin();
	moITEM_ITR old_i = f_l.is.begin();
	for (moITEM_ITR i_itr = f_l.is.begin(); i_itr != f_l.is.end(); ++i_itr) {
		current_block_start = k + 1;
		moITEM_ITR j_itr;
		if ((*i_itr)->blockstart != (*old_i)->blockstart) {
			current_start_itr = i_itr;
		}
		old_i = i_itr;

		j_itr = current_start_itr;

		while ((j_itr != f_l.is.end())
				&& ((*j_itr)->blockstart == (*i_itr)->blockstart)) {
			if (timer->isTimeOver()) {
				return;
			}

			//for injective episodes, don't add a duplicate timestamp
			if ('t' == type || i_itr != j_itr) {

				//Step 1. create the candidate episodes - one at a time
				moitem* e = (*j_itr)->get_last_episode();
				bool needtoadd = true;
				moitem * alpha = (*i_itr)->generate_serial_candidate(e);
				//Testing:
				/*{
				 cout<<"Last episode = ";
				 e->output(cout);
				 cout<<"Candidate = ";
				 alpha->output(cout);
				 cout<<endl;
				 system("pause");
				 }*/

//			if (needtoadd && k==1&&ruleconstraints)//first time generating candidates
//				needtoadd= (alpha)->firstmatchruleconstraints(T);
				if (needtoadd && 's' == type)
					needtoadd = !alpha->matchs();

				//Step 2. Build and test subepisodes Beta that do not contain alpha[y]
				// Add the candidate if all the subepisodes are in f_l
				// do not add if the episodes that have sub-episodes that do not have min support

//				needtoadd= fast_gen_support_subs(alpha, f_l, l+1);
				if (needtoadd) {
					needtoadd = alpha->fast_gen_support_subs(f_l, l + 1);
					//Step 3. Do not add the episodes where the closure of some k-subset of X contains X
					if (needtoadd) {
						/*//Testing:
						 cout<<"Made it = ";
						 alpha->output(cout);
						 cout<<endl;
						 system("pause");
						 */
						k++;
						alpha->blockstart = current_block_start;
//						alpha->sortit();
						insertitem(alpha);
					} else {
						alpha->clear();
						delete alpha;
					}
				} else {
					alpha->clear();
					delete alpha;
				}
			} //end if t or if i != j
			j_itr++;
		} //for one added candidate for an current episode
	} //for all current episodes

//testing:
//output_episodes(cout);
//cout<<endl;
//system("pause");

} //end fast generate candidates serial

void moitemset::generate_parallel_candidates(const moitemset& fc_k_b, char type,
		const event_set& B, int k) {
//Step 1. create the candidate episodes - one at a time
	for (const_moITEM_ITR item_set_itr = fc_k_b.is.begin();
			item_set_itr != fc_k_b.is.end(); ++item_set_itr) {
		for (event_set::const_iterator event_itr = B.begin();
				event_itr != B.end(); ++event_itr) {

			bool needtoadd = true;
			moitem * cand = (*item_set_itr)->generate_parallel_candidate(
					*event_itr);

			//Testing:
			//if (calltype == 'r')
//			{
//				cout<<"Candidate = "<<*cand<<endl;
//				system("pause");
//			}

			//Step 2. Do not add the episodes that do not satisfy the partial ordering
			needtoadd = matchp(cand, type);

			//Step 3. do not add if the episode does not match the constraints
//			if (needtoadd)
//				needtoadd=(cand)->matchB(B);
			//guaranteed- built using only the constraints

			//testing:
			/*	output_episodes(cout);
			 cout<<endl;
			 */

			//Step 4. do not add if the episodes that have sub-episodes that do not have min support
			if (needtoadd) {
				needtoadd = support_subs_parallel(cand, fc_k_b, k + 1);
				//Step 5. Add new episodes that meet the AND constraints
				//NOT YET IMPLEMENTED

				//Step 6. Do not add the episodes where the closure of some k-subset of X contains X
				if (needtoadd) {
					/*//Testing:
					 cout<<"Made it = "<<*cand<<endl;
					 system("pause");
					 */
					insertitem(cand);
				} else {
					cand->clear();
					delete cand;
				}
			} else {
				cand->clear();
				delete cand;
			}
		} //for one added candidate for an current episode
	} //for all current episodes
	/*//testing:
	 output_episodes(cout);
	 cout<<endl;
	 */

} //end generate parallel candidates

void moitemset::generate_parallel_rule_candidates(const moitemset& A,
		const moitem* Z, char type, const event_set& B, int j)
//generate the subsets of moitem Z, that meet constraints B
//used only from the morule-generation process.
//CHANGE when constraints are converted to DNF
		{	//Step 1. create the candidate episodes - one at a time

	for (const_moITEM_ITR itr = A.is.begin(); itr != A.is.end(); itr++) {
		for (event_set::const_iterator event_itr = B.begin();
				event_itr != B.end(); ++event_itr) {
			bool needtoadd = true;
			moitem * cand = (*itr)->generate_parallel_candidate(*event_itr);

			//Step 2. Do not add the episodes that do not satisfy the partial ordering
			needtoadd = matchp(cand, type);

			//Step 3. do not add if the episode does not match the constraints
//			if (needtoadd)
//				needtoadd=(cand)->matchB(B);

			//Step 4a. do not add if the episodes that have sub-episodes that do not have min support
			if (needtoadd) {
				needtoadd = support_subs_parallel(cand, A, j + 1);
				//Step 4b. do not add if the episodes could not have been generated from the original episode
				if (needtoadd) {
					mop_item *pitemptr;
					pitemptr = static_cast<mop_item*>(cand);
					if (!Z->does_contain(pitemptr))
						needtoadd = false;
				}
				//Step 5. Add new episodes that meet the AND constraints
				//NOT YET IMPLEMENTED

				if (needtoadd) {
					insertitem(cand);
					/*//Testing:
					 cout<<"Made it = "<<*cand<<endl;
					 system("pause");
					 */
				} else {
					cand->clear();
					delete cand;
				}
			} else {
				cand->clear();
				delete cand;
			}
		} //for one added candidate for an current episode
	} //for all current episodes
	/*//testing:
	 output_episodes(cout);
	 cout<<endl;
	 */
} //end generate parallel morule candidates

void moitemset::fast_generate_parallel_rule_candidates(moitemset& f_l,
		const moitem* Z, char type, int l) { // Algorithm 3 from Mannila, Toivonen and Verkamo, "Discovery of frequent episodes
// in event sequences" Department of Computer Science Series of Publications C,
// report C-1997-15, University of Helsinki, Finland

//with added closures
//generate the subsets of moitem Z
//used only from the morule-generation process.
//CHANGE when constraints are converted to DNF
//parallel items have their events in lexigraphical sorted order
//serial items have their events in serial order 
	int k = 0;
	int current_block_start;
	moITEM_ITR current_start_itr = f_l.is.begin();
	moITEM_ITR old_i = f_l.is.begin();
	for (moITEM_ITR i_itr = f_l.is.begin(); i_itr != f_l.is.end(); ++i_itr) {
		current_block_start = k + 1;
		moITEM_ITR j_itr;
		if ((*i_itr)->blockstart != (*old_i)->blockstart) {
			current_start_itr = i_itr;
		}
		old_i = i_itr;

		j_itr = i_itr;
		while ((j_itr != f_l.is.end())
				&& ((*j_itr)->blockstart == (*i_itr)->blockstart)) {
			//for injective episodes, don't add a duplicate event
			if ('q' == type || 't' == type || i_itr != j_itr) {

				//Step 1. create the candidate episodes - one at a time
				event e = (*j_itr)->get_last_event();
				bool needtoadd = true;
				moitem * alpha = (*i_itr)->generate_parallel_candidate(e, Z);
				//Testing:
				//			{
//			cout<<"Candidate = "<<*alpha<<endl;
//				system("pause");
				//			}

				//Step 2. Build and test subepisodes Beta that do not contain alpha[y]
				// Add the candidate if all the subepisodes are in f_l
				// do not add if the episodes that have sub-episodes that do not have min support

				needtoadd = alpha->fast_gen_support_subs(f_l, l + 1);

				//Step 4b. do not add if the episodes could not have been generated from the original episode
				if (needtoadd) {
					mop_item *pitemptr;
					pitemptr = static_cast<mop_item*>(alpha);
					if (!Z->does_contain(pitemptr))
						needtoadd = false;
//					if (! Z->does_contain(alpha))
//						needtoadd = false;
				}

				if (needtoadd) {
					//Testing:
					//cout<<"Made it = "<<*alpha<<endl;
					//system("pause");
					k++;
					alpha->blockstart = current_block_start;
					insertitem(alpha);
				} else {
					alpha->clear();
					delete alpha;
				}

			} //end if q or if i != j
			j_itr++;
		} //for one added candidate for an current episode
	} //for all current episodes
	/*//testing:
	 output_episodes(cout);
	 cout<<endl;
	 */
//	system("pause");

} //end fastgenerate parallel morule candidates

void moitemset::generate_serial_candidates(const moitemset& fc_k_b, char type,
		const moitemset& B1, int k) {
//Step 1. create the candidate episodes - one at a time
	for (const_moITEM_ITR item_set_itr = fc_k_b.is.begin();
			item_set_itr != fc_k_b.is.end(); ++item_set_itr) {
		for (const_moITEM_ITR event_itr = B1.is.begin();
				event_itr != B1.is.end(); ++event_itr) {
			bool needtoadd = true;
			moitem * cand = (*item_set_itr)->generate_serial_candidate(
					*event_itr);

			//Testing:
			//if (calltype == 'r')
//			{
//				cout<<"Candidate = "<<*cand<<endl;
//				system("pause");
//			}

			//Step 2. Do not add the episodes that do not satisfy the partial ordering
			needtoadd = matchs(cand, type);

			//Step 3. do not add if the episode does not match the constraints
//			if (needtoadd)
//				needtoadd=(cand)->matchB(B);

			//testing:
			/*	output_episodes(cout);
			 cout<<endl;
			 */

			//Step 4. do not add if the episodes that have sub-episodes that do not have min support
			if (needtoadd) {
				needtoadd = support_subs_serial(cand, fc_k_b, k + 1);

				//Step 5. Add new episodes that meet the AND constraints
				//NOT YET IMPLEMENTED

				//Step 6. Do not add the episodes where the closure of some k-subset of X contains X
				if (needtoadd) {
					/*//Testing:
					 cout<<"Made it = "<<*cand<<endl;
					 system("pause");
					 */
					insertitem(cand);
				} else {
					cand->clear();
					delete cand;
				}
			} else {
				cand->clear();
				delete cand;
			}

		} //for one added candidate for an current episode
	} //for all current episodes
	/*//testing:
	 output_episodes(cout);
	 cout<<endl;
	 */

} //end generate serial candidates

void moitemset::generate_serial_rule_candidates(const moitemset& A,
		const moitem* Z, char type, moitemset B1, int j)
//generate the subsets of moitem Z, that meet constraints B
//used only from the morule-generation process.
//CHANGE when constraints are converted to DNF
		{	//Step 1. create the candidate episodes - one at a time

	for (const_moITEM_ITR itr = A.is.begin(); itr != A.is.end(); itr++) {
		for (const_moITEM_ITR event_itr = B1.is.begin();
				event_itr != B1.is.end(); ++event_itr) {
			bool needtoadd = true;
			moitem * cand = (*itr)->generate_serial_candidate(*event_itr);

			//Step 2. Do not add the episodes that do not satisfy the partial ordering
			needtoadd = matchs(cand, type);

			//Step 3. do not add if the episode does not match the constraints
//			if (needtoadd)
//				needtoadd=(cand)->matchB(B);

			//Step 4a. do not add if the episodes that have sub-episodes that do not have min support
			if (needtoadd)
				needtoadd = support_subs_serial(cand, A, j + 1);
			//Step 4b. do not add if the episodes could not have been generated from the original episode
			if (needtoadd) {
				mos_item *sitemptr;
				sitemptr = static_cast<mos_item*>(cand);
				if (!Z->does_contain(sitemptr))
					needtoadd = false;
			}

			//Step 5. Add new episodes that meet the AND constraints
			//NOT YET IMPLEMENTED

			if (needtoadd) {
				insertitem(cand);
				/*//Testing:
				 cout<<"Made it = "<<*cand<<endl;
				 system("pause");
				 */
			} else {
				cand->clear();
				delete cand;
			}

		} //for one added candidate for an current episode
	} //for all current episodes
	/*//testing:
	 output_episodes(cout);
	 cout<<endl;
	 */
} //end generate serial morule candidates

void moitemset::fast_generate_serial_rule_candidates(moitemset& f_l,
		const moitem* Z, char type, int l) { // Algorithm 3 from Mannila, Toivonen and Verkamo, "Discovery of frequent episodes
// in event sequences" Department of Computer Science Series of Publications C,
// report C-1997-15, University of Helsinki, Finland

//with added closures
//generate the subsets of moitem Z
//used only from the morule-generation process.
//CHANGE when constraints are converted to DNF
//parallel items have their events in lexigraphical sorted order
//serial items have their events in serial order 
	int k = 0;
	int current_block_start;
	moITEM_ITR current_start_itr = f_l.is.begin();
	moITEM_ITR old_i = f_l.is.begin();
	for (moITEM_ITR i_itr = f_l.is.begin(); i_itr != f_l.is.end(); ++i_itr) {
		current_block_start = k + 1;
		moITEM_ITR j_itr;
		if ((*i_itr)->blockstart != (*old_i)->blockstart) {
			current_start_itr = i_itr;
		}
		old_i = i_itr;

		j_itr = current_start_itr;

		while ((j_itr != f_l.is.end())
				&& ((*j_itr)->blockstart == (*i_itr)->blockstart)) {
			//for injective episodes, don't add a duplicate event
			if ('t' == type || i_itr != j_itr) {

				//Step 1. create the candidate episodes - one at a time
				moitem* e = (*j_itr)->get_last_episode();
				bool needtoadd = true;
				moitem * alpha = (*i_itr)->generate_serial_rule_candidate(e, Z);
				//Testing:
				//			{
//			cout<<"Candidate = "<<*alpha<<endl;
//				system("pause");
				//			}

				//Step 2. Build and test subepisodes Beta that do not contain alpha[y]
				// Add the candidate if all the subepisodes are in f_l
				// do not add if the episodes that have sub-episodes that do not have min support

//				if ('s' == type)
//					needtoadd = !alpha->matchs();
				if (needtoadd)
					needtoadd = alpha->fast_gen_support_subs(f_l, l + 1);

				//Step 4b. do not add if the episodes could not have been generated from the original episode
				if (needtoadd) {
					mos_item *sitemptr;
					sitemptr = static_cast<mos_item*>(alpha);
					if (!Z->does_contain(sitemptr))
						needtoadd = false;
//					if (! Z->does_contain(alpha))
//						needtoadd = false;
				}

				if (needtoadd) {
					//Testing:
					//cout<<"Made it = "<<*alpha<<endl;
					//system("pause");
					k++;
					alpha->blockstart = current_block_start;
					insertitem(alpha);
				} else {
					alpha->clear();
					delete alpha;
				}
			} //end if q or if i != j
			j_itr++;
		} //for one added candidate for an current episode
	} //for all current episodes
	/*//testing:
	 output_episodes(cout);
	 cout<<endl;
	 */
//	system("pause");

} //end fastgenerate serial morule candidates

bool moitemset::matchp(moitem* cand, char type) const {
	bool found = false;

	switch (type) {
	case 's':
	case 't':
		return found;
		break;
	case 'q':
		cand->sortit();
		break;
	case 'p':
//sort
		cand->sortit();
//do not add episodes that have events that occur more that once in the episode
		found = cand->matchp();
		break;
	};
//find duplicate moitem, if there is one
	mop_item *pitemptr;
	pitemptr = static_cast<mop_item*>(cand);
	const_moITEM_ITR itr = is.begin();
	while (!found && itr != is.end()) {
		if ((*itr)->equalitem(pitemptr))
			found = true;
		itr++;
	}

//testing:
	/*	output_episodes(cout);
	 cout<<endl;
	 */

	return !found;
} //end matchp

bool moitemset::matchs(moitem* cand, char type) const {
	bool found = false;

//do not add episodes that have events that occur more that once in the episode
	if ('s' == type)
		found = cand->matchs();

//find duplicate moitem, if there is one
	/*
	 mos_item *sitemptr;
	 sitemptr = static_cast<mos_item*>(cand);
	 const_moITEM_ITR itr =  is.begin();
	 while(!found && itr != is.end() )
	 {
	 if ( (*itr)->equalitem(sitemptr)
	 found = true;
	 itr++;
	 }
	 */
//testing:
	/*	output_episodes(cout);
	 cout<<endl;
	 */

	return !found;
} //end matchs

bool moitemset::does_contain_parallel(const moitem* Y) const { //parallel
	const mop_item *pitemptr;
	pitemptr = static_cast<const mop_item*>(Y);
	bool found = false;
	const_moITEM_ITR itr = is.begin();
	while (!found && itr != is.end()) {
		if ((*itr)->equalitem(pitemptr))
			found = true;
		itr++;
	}
	return found;
}

bool moitemset::does_contain_parallel_rule_item(const moitem* Y) const { //parallel rule itme
	const mop_ruleitem *pitemptr;
	pitemptr = static_cast<const mop_ruleitem*>(Y);
	bool found = false;
	const_moITEM_ITR itr = is.begin();
	while (!found && itr != is.end()) {
		if ((*itr)->equalitem(pitemptr))
			found = true;
		itr++;
	}
	return found;
}

bool moitemset::does_contain_serial_rule_item(const moitem* Y) const { //parallel rule itme
	const mos_ruleitem *sitemptr;
	sitemptr = static_cast<const mos_ruleitem*>(Y);
	bool found = false;
	const_moITEM_ITR itr = is.begin();
	while (!found && itr != is.end()) {
		if ((*itr)->equalitem(sitemptr))
			found = true;
		itr++;
	}
	return found;
}

bool moitemset::does_contain_serial(const moitem* Y) const { //serial
	const mos_item *sitemptr;
	sitemptr = static_cast<const mos_item*>(Y);
	bool found = false;
	const_moITEM_ITR itr = is.begin();
	while (!found && itr != is.end()) {
		if ((*itr)->equalitem(sitemptr))
			found = true;
		itr++;
	}
	return found;
}

bool moitemset::support_subs_parallel(const moitem* Y, const moitemset& fc_k_b,
		int k) const {
	int i, j; //looping indices
	bool support = true;
	vector<int> index;
	for (i = 0; i < k; i++) {
		index.push_back(i);
	}

	moitem * sub = new mop_item;
	const mop_item *pitemptr;
	pitemptr = static_cast<const mop_item*>(Y);

	for (j = 0; j < k; j++) {
		int one = index[j];
		sub->insertepisode(pitemptr->get_ith_event(one));
	}

	if (!fc_k_b.does_contain_parallel(sub)) {
		support = false;
	}
	i = k - 1;
	while (support && i >= 0) {
		sub->clear();
		index[i]++;
		for (j = 0; j < k; j++) {
			int one = index[j];
			sub->insertepisode(pitemptr->get_ith_event(one));
		}
		if (!fc_k_b.does_contain_parallel(sub)) {
			support = false;
		}
		i--;
	}
	sub->clear();
	delete[] sub;
	return support;
} //end support_sub parallel

bool moitemset::support_subs_serial(const moitem* Y, const moitemset& fc_k_b,
		int k) const {
	int i, j;
	bool support = true;
	vector<int> index;
	for (i = 0; i < k; i++) {
		index.push_back(i);
	}

	moitem * sub = new mos_item;
	const mos_item *sitemptr;
	sitemptr = static_cast<const mos_item*>(Y);

	for (j = 0; j < k; j++) {
		int one = index[j];
		sub->insertepisode(sitemptr->get_ith_event(one));
	}

	if (!fc_k_b.does_contain_serial(sub)) {
		support = false;
	}
	i = k - 1;
	while (support && i >= 0) {
		sub->clear();
		index[i]++;
		for (j = 0; j < k; j++) {
			int one = index[j];
			sub->insertepisode(sitemptr->get_ith_event(one));
		}
		if (!fc_k_b.does_contain_serial(sub)) {
			support = false;
		}
		i--;
	}
	delete[] sub;
	return support;
} //end support_sub serial

bool moitemset::empty() const {
	return is.empty();
}

void moitemset::output(ostream& outs) const {
	moitem * match;

	outs << endl;
	outs << "Episodes: { ";
	const_moITEM_ITR i = is.begin();
	if (i != is.end()) {
		match = *i;
		match->output(outs);
		++i;
	}
	while (i != is.end()) {
//		outs<<", ";
		match = *i;
		match->output(outs);
		++i;
	}
	outs << " }";
	outs << endl;
}

void moitemset::output_episodes(ostream& outs) const {
	moitem * match;
	outs << "Episodes: { ";
	const_moITEM_ITR i = is.begin();
	if (i != is.end()) {
		match = *i;
		match->output_episode(outs);
		++i;
	}
	while (i != is.end()) {
		outs << ", ";
		match = *i;
		match->output_episode(outs);
		++i;
	}
	outs << " }";
}

/*ostream& operator << (ostream& outs, const moitemset& the_object)
 //Overloads the << operator for output values of type moitemset.
 //Precondition: If outs is a file output stream, then outs as
 //already been connected to a file.
 {
 ostream_iterator<moitem*> output(outs, " ");
 copy(the_object.is.begin(), the_object.is.end(), output);
 return outs;
 }
 */
moitemset::moitemset() {
	moitem *empty = new mop_item;
	is.push_back(empty);
	is.clear();
	timer = new Timer();
}

moitem * moitemset::get_first_item() const {
	const_moITEM_ITR i = is.begin();
	return *i;
}

moitem* moitemset::findsmallest(const fc_type& split_fac,
		const moitem* X) const {
	bool item_found = false;
	const_moITEM_ITR i;
	fc_type::const_iterator j = split_fac.begin();

//testing
//	cout<<"look for moitem: ";
//	X->output_episode(cout);
//	cout<<"\t";
//	cout<<"in items: ";

//no need to look in smaller itemsets
	int size = X->getantnumberofevents(); //returns the antecedent size
	size = (size - 1) * moitemset::seeconslargest(); //move past the smaller antecedents
	int movej;
	for (movej = 1; movej <= size; movej++)
		j++;
	for (movej = 1; movej < X->getconsnumberofevents(); movej++)
		; //move past the same size antecedent, but smaller consequents
	j++;

	while (!item_found && j != split_fac.end()) { //look through each level j

		i = j->is.begin();
		while (!item_found && i != j->is.end()) { //look through each moitem i at the current level j
//testing:
//			(*i)->output_episode(cout);
//			cout<<"\t";
			if ((*i)->does_contain(X)) {

				item_found = true;
			} else
				i++;
		} //end while i
		if (!item_found)
			j++;
	} //end while j

	if (item_found)
		return (*i);
//gets to this point if the moitem is not found
//for example: clay98, q episodes.  A2 contains moitem CC,
// this moitem is not in any moitemset
	moitem* empty = new mop_item;
	return empty;

} //end find smallest moitem that contains X

double moitemset::computelevelsupport(const moitem* Z) const { //level support for s and t serial episodes
															   //and p and q parallel episodes

	double levelsupport = 0;
	for (const_moITEM_ITR itr = is.begin(); itr != is.end(); itr++) {
		int support = 0;
		if ((*itr)->does_contain(Z, support) && support > levelsupport)
			//contains ALL the events
			levelsupport = support;
	}
	return levelsupport;
}

void moitemset::putinsplit(fc_type& split_fc, int& k) const {
	for (const_moITEM_ITR i = is.begin(); i != is.end(); i++) {
		int index = (*i)->getantnumberofevents() * moitemset::seeconslargest()
				- (moitemset::seeconslargest() - (*i)->getconsnumberofevents());
		(*i)->output(cout);
		split_fc[index - 1].insertitem(*i);
		if (k < index)
			k = index;
	}
	k--;
}

/*
 old loop1 - goes through the antecedent one at a time
 with separate rule constraints, knowing that 33,44,45=>10,3 is not
 accurately represented by 44 =>10,3 even if this rule meets the min confidence.

 void moitemset::loop1parallel(const fc_type& split_fac, double c, moruleset& REAR, int i, int k, char type, const event_set& T, const event_set& C)
 {

 double maxsupport = 0;
 fc_type A;
 for (rmoITEM_ITR itr = is.rbegin(); itr != is.rend(); itr++)
 {
 A.clear();
 moitemset emptyset;
 A.push_back(emptyset);

 //testing
 //		(*itr)->output_episode(cout);
 //system("pause");

 //compute new max support
 if (i < k)
 maxsupport = (*itr)->get_max_support(split_fac,i,k, type); // get support from a larger episode that contains this one
 else
 maxsupport = 0; //the largest episode - no need to look for a larger one

 if ((*itr)->get_support() != maxsupport)
 {
 A[0].clear();
 (*itr)->create_1_ante(A[0],T); //create 1 antecedents from closure
 //if you are only interested in rules that only have these constraints
 //currently, Tsegaye is interested in rules that have only the constraints
 bool removed = false;

 //start loop 2
 int j = 0;
 while (j <= i &&!A[j].empty())
 {
 moITEM_ITR X = A[j].is.begin();
 while (X != A[j].is.end())
 {
 //					(*X)->output(cout);
 const moitem* Y;
 Y = findsmallest(split_fac,(*X)); //find support of X

 //testing
 //					Y->output(cout);
 //					system("pause");

 int X_antsupport = Y->get_antsupport();
 int i_freq = (*itr)->get_support();
 (*X)->set_antsupport(X_antsupport);

 if ( X_antsupport != 0 && (double(i_freq)/X_antsupport >= c ) &&
 (double(maxsupport)/X_antsupport < c) )
 {
 morule r;
 //Tsegaye said to insert the morule if ANY of the antecedent constraints were met
 //if all antecedent constraints are not met, do not insert the morule
 //						if ( (*X)->matchruleconstraints(T) )
 //						{
 r.insert((*X)->get_antepisode(), double(X_antsupport), (*itr)->get_consepisode(), double(i_freq));
 //						}
 //						r.output(cout);  //for testing
 //						system("pause");
 if (!r.cons_empty() && !REAR.contain(r))
 REAR.insertrule(r);
 A[j].is.erase(remove(A[j].is.begin(), A[j].is.end(), (*X)), A[j].is.end());
 removed = true;
 } //end if
 else
 //split the consequence & try those , until the consequence size is 1
 //if the consequence size is 1, and you went through the entire list,
 //then increment X
 //						else if
 X++;
 } //end while
 if (!A[j].empty())
 {
 A.push_back(emptyset);
 if (j+1 < i)
 {
 //		Uncomment this section when DNF constraints with AND conditions are used
 //						event_set F;
 //						for (const_moITEM_ITR nitr = A[j].is.begin(); nitr != A[j].is.end(); nitr++)
 //						{
 //							event e = (*nitr)->get_last_event();
 //only need the last events as longer
 // episodes of length k+1 are built from smaller episodes of length k
 // if an event is on the end of a smaller episode of length k
 // then we can look for another episode with the same k-1 first events
 // and build a k+1 length episode by combinging the two events
 // in k positions with the first k-1 matching events
 //							event_set::const_iterator location = find(F.begin(), F.end(), e);
 //							if (location == F.end())
 //								F.push_back(e);
 //						}
 //						sort(F.begin(), F.end());
 //Testing:
 //(*itr)->output_episode(cout);
 //cout<<endl;
 //system("pause");

 //						if (equalBF)
 A[j+1].fast_generate_parallel_rule_candidates(A[j], (*itr), type, j);
 //						else
 //							A[j+1].generate_parallel_rule_candidates(A[j], (*itr), type, F, j);
 //							use this when DNF constraints are fixed

 //Testing:
 //						A[j+1].output(cout);
 //						cout<<endl;
 //						system("pause");

 } //end if
 else if (j+1 == i && ! removed)
 A[j+1].insertitem(*itr);

 j++;
 }  //end if
 } //end loop 2
 }//end if
 }//end loop 1
 } //end loop 1 parallel


 void moitemset::loop1serial(const fc_type& split_fac, double c, mos_ruleset& REAR, int i, int k, char type, const event_set& T, const event_set& C)
 {
 double maxsupport = 0;
 fc_type A;
 for (rmoITEM_ITR itr = is.rbegin(); itr != is.rend(); itr++)
 {
 A.clear();
 moitemset emptyset;
 A.push_back(emptyset);

 //testing
 (*itr)->output_episode(cout);
 //system("pause");

 //compute new max support
 if (i < k)
 maxsupport = (*itr)->get_max_support(split_fac,i,k, type); // get support from a larger episode that contains this one
 else
 maxsupport = 0; //the largest episode - no need to look for a larger one

 if ((*itr)->get_support() != maxsupport)
 {
 A[0].clear();
 (*itr)->create_1_ante(A[0],T); //create 1 antecedents from closure
 //if you are only interested in rules that only have these constraints
 //currently, Tsegaye is interested in rules that have only the constraints

 //start loop 2
 int j = 0;
 bool removed=false;
 while (j <= i &&!A[j].empty())
 {
 moITEM_ITR X = A[j].is.begin();
 while (X != A[j].is.end())
 {
 const moitem* Y;
 Y = findsmallest(split_fac,(*X)); //find support of X
 //testing
 Y->output(cout);
 //					system("pause");

 int X_antsupport = Y->get_antsupport();
 int i_freq = (*itr)->get_support();
 (*X)->set_antsupport(X_antsupport);

 if ( X_antsupport != 0 && (double(i_freq)/X_antsupport >= c ) &&
 (double(maxsupport)/X_antsupport < c) )
 {
 mos_rule r;
 //Tsegaye said to insert the morule only if the antecedent contains only events from the antecedent constraint list
 //if all antecedent constraints are not met, do not insert the morule
 //							if ( (*X)->matchruleconstraints(T) )
 //							{
 r.insert((*X)->get_antepisode(1), double(X_antsupport), (*itr)->get_consepisode(1), double(i_freq));
 //							}
 //r.output(cout);  //for testing
 if (!r.cons_empty() && !REAR.contain(r))
 REAR.insertrule(r);
 A[j].is.erase(remove(A[j].is.begin(), A[j].is.end(), (*X)), A[j].is.end());
 removed=true;
 } //end if
 else
 X++;
 } //end while
 if (!A[j].empty())
 {
 A.push_back(emptyset);
 if (j+1 < i)
 {
 //		Uncomment this section when you use DNF constraints that have AND conditions
 //						moitemset F1;
 //						for (const_moITEM_ITR nitr = A[j].is.begin(); nitr != A[j].is.end(); nitr++)
 //						{
 //							moitem* e = (*nitr)->get_last_split_episode();
 //only need the last events as longer
 // episodes of length k+1 are built from smaller episodes of length k
 // if an event is on the end of a smaller episode of length k
 // then we can look for another episode with the same k-1 first events
 // and build a k+1 length episode by combinging the two events
 // in k positions with the first k-1 matching events
 //							if (!F1.does_contain_serial(e))
 //								F1.insertitem(e);
 //						}
 //Testing:
 //	(*itr)->output_episode(cout);
 //	cout<<endl;
 //	system("pause");

 //						if (equalBF)
 A[j+1].fast_generate_serial_rule_candidates(A[j], (*itr), type, j);
 //						else
 //							A[j+1].generate_serial_rule_candidates(A[j], (*itr), type, F1, j);
 //						use when DNF constraints are fixed

 //Testing:
 //A[j+1].output(cout);
 //cout<<endl;
 //system("pause");

 } //end if
 else if (j+1 == i && ! removed)
 A[j+1].insertitem(*itr);
 j++;
 }  //end if
 } //end loop 2
 }//end if
 }//end loop 1
 } //end loop 1 serial
 */

void moitemset::loop1parallel(const fc_type& split_fac, double c,
		moruleset& REAR, int i, int k, char type, const event_set& T,
		const event_set& C) {

	double maxsupport = 0;
	for (rmoITEM_ITR itr = is.rbegin(); itr != is.rend(); itr++) {
//testing
//(*itr)->output_episode(cout);
//system("pause");

//compute new max support
		if (i < k)
			maxsupport = (*itr)->get_max_support(split_fac);// get support from a larger episode that contains this one
		else
			maxsupport = 0;	//the largest episode - no need to look for a larger one

		if ((*itr)->get_support() != maxsupport) {

			//testing
//					Y->output(cout);
//					system("pause");

			int X_antsupport = (*itr)->get_antsupport();
			int i_freq = int((*itr)->get_support());

			if (X_antsupport != 0 && (double(i_freq) / X_antsupport >= c)
					&& (double(maxsupport) / X_antsupport < c)) {
				morule r;
				r.insert((*itr)->get_antepisode(), double(X_antsupport),
						(*itr)->get_consepisode(), double(i_freq),
						(*itr)->get_last_occurrence(),
						(*itr)->get_conssupport(), 0);
//						r.output(cout);  //for testing
//						system("pause");
				if (!r.cons_empty() && !REAR.contain(r))
					REAR.insertrule(r);
			} //end if
		} //end if
	} //end loop 1
} //end loop 1 parallel

void moitemset::loop1serial(const fc_type& split_fac, double c,
		mos_ruleset& REAR, int i, int k, char type, const event_set& T,
		const event_set& C) {
	double maxsupport = 0;
	for (rmoITEM_ITR itr = is.rbegin(); itr != is.rend(); itr++) {
//testing
//	(*itr)->output_episode(cout);
//system("pause");

//compute new max support
		if (i < k)
			maxsupport = (*itr)->get_max_support(split_fac); // get support from a larger episode that contains this one
		else
			maxsupport = 0; //the largest episode - no need to look for a larger one

		if ((*itr)->get_support() != maxsupport) {

			int X_antsupport = int((*itr)->get_antsupport());
			int i_freq = int((*itr)->get_support());

			if (X_antsupport != 0 && (double(i_freq) / X_antsupport >= c)
					&& (double(maxsupport) / X_antsupport < c)) {
				mos_rule r;
				r.insert((*itr)->get_antepisode(1), double(X_antsupport),
						(*itr)->get_consepisode(1), double(i_freq),
						(*itr)->get_last_occurrence(),
						(*itr)->get_conssupport());
				//r.output(cout);  //for testing
				if (!r.cons_empty() && !REAR.contain(r))
					REAR.insertrule(r);
			} //end if
		} //end if
	} //end loop 1
} //end loop 1 serial

void moitemset::combine(const moitemset& fa, moitemset& garbage,
		const fc_type& fc, char type, int lag, int min_fr1, char lag_type)

//combine the ant and cons into ruleitems
		{
	setantlargest(0);
	setconslargest(0);
	bool done = false;
	int i;
// 	moitem*  tryit;

	for (const_moITEM_ITR faitr = fa.is.begin(); faitr != fa.is.end();
			++faitr) {

		i = 0;
		done = false;
		try {
			// tryit = fc[i];
			done = (fc[i]).empty();
		} catch (out_of_range e) {
			done = true;
		}

		while (!done) {
			for (const_moITEM_ITR fcitr = fc[i].is.begin();
					fcitr != fc[i].is.end(); ++fcitr) {

				//Check if the algorithm has run past the max time allowed and exit if it has
				if (timer->isTimeOver()) {
					return;
				}

				moitem* p;
				switch (type) {
				case 'p':
				case 'q': {
					p = new mop_ruleitem;
					p->insertant((*faitr)->get_episode());
					int supp = (int) (*faitr)->get_support();
					p->set_antsupport(supp);
					//				(*faitr)->output(cout);
					p->insertcons((*fcitr)->get_episode());
					supp = (int) (*fcitr)->get_support();
					p->set_conssupport(supp);
					break;
				}
				case 's':
				case 't': {
					p = new mos_ruleitem;
					p->insertant((*faitr)->get_episode(1));
					int supp = (int) (*faitr)->get_support();
					p->set_antsupport(supp);
					//				(*faitr)->output(cout);
					p->insertcons((*fcitr)->get_episode(1));
					supp = (int) (*fcitr)->get_support();
					p->set_conssupport(supp);
					break;
				}
				};
//				(*fcitr)->output(cout);
//				system("pause");
//				p->output(cout);
				p->recordmobetween(*faitr, *fcitr, lag, lag_type);
//				p->output(cout);
//				system("pause");

				double ratio = p->cal_denisty_ratio();
				p->set_density(ratio);
				//if (p->cal_denisty_ratio() >= 1.2)
				if (p->get_support() >= min_fr1) {
					//	cout<<"Keep ";
					//	p->output(cout);
					insertitem(p);
					if (p->getantnumberofevents() > seeantlargest())
						setantlargest(p->getantnumberofevents());
					if (p->getconsnumberofevents() > seeconslargest())
						setconslargest(p->getconsnumberofevents());
				} else {
//					cout<<"Delete ";
//				p->output(cout);
					p->clear();
					delete p;
					p = 0;
//					garbage.insertitem(p);
				}
			}				//end of for for fcitr
			i++;

			try {
				/*  *****previous code*********
				 change made on 02/15/07.
				 reason: fc[i].empty() returns false even if i is equal to or beyond the size of the fc vector.
				 done = (fc[i].empty());
				 */
				if (i >= fc.size()) {
					//done = true;
					break;
				} else {
					done = (fc[i]).empty();
				}
			} catch (out_of_range e) {
				done = true;
			}
		}				//end of while (!done)
	}				// end of faitr
}

void moitemset::addrules(double c, moruleset& REAR, int i) {
	for (rmoITEM_ITR itr = is.rbegin(); itr != is.rend(); itr++) {
//testing
//		(*itr)->output_episode(cout);
//system("pause");

		int X_antsupport = (*itr)->get_antsupport();
		int i_freq = int((*itr)->get_support());

		if (X_antsupport != 0 && (double(i_freq) / X_antsupport >= c)) {
			morule r;
			r.insert((*itr)->get_antepisode(), double(X_antsupport),
					(*itr)->get_consepisode(), double(i_freq),
					(*itr)->get_last_occurrence(), (*itr)->get_conssupport(),
					(*itr)->get_density());
//			r.output(cout);  //for testing
//			system("pause");
			if (!r.cons_empty() && !REAR.contain(r))
				REAR.insertrule(r);
		} //end if
	}
} //end add parallel rules

void moitemset::addrules(double c, mos_ruleset& REAR, int i) {
	for (rmoITEM_ITR itr = is.rbegin(); itr != is.rend(); itr++) {
//testing
//		(*itr)->output_episode(cout);
//		system("pause");

		int X_antsupport = (*itr)->get_antsupport();
		int i_freq = int((*itr)->get_support());
		if (X_antsupport != 0 && (double(i_freq) / X_antsupport >= c)) {
			mos_rule r;
			r.insert((*itr)->get_antepisode(1), double(X_antsupport),
					(*itr)->get_consepisode(1), double(i_freq),
					(*itr)->get_last_occurrence(), (*itr)->get_conssupport());
//			r.output(cout);  //for testing
//			system("pause");
			if (!r.cons_empty() && !REAR.contain(r))
				REAR.insertrule(r);
		} //end if
	}
} //end add serial rules

void moitemset::setTimer(Timer* t) {
//Sets this objects timer data member.
	timer = t;
}
