/*
filename: documentation.cpp
author: Sherri Harms
last modified: 3/21/02


**********************************************************************
Maintenance:
2/19/05 Removed the Representative rules from output & added memory leak cleanup
		The repr. rules take longer to produce, so I decided to just output
		all of the rules as they come in.  We may look into outputting them in 
		descending sorted order by confidence.
7/29/2002 Combined MAX lag & Fixed lag together into this program: MOWCATL
		Added a new parameter in the parameter file (M for Max, F for fixed lag)

7/12/2002 Corrected j-measure calculation.  Added the consequent support field, got
			its value from the support of the consequent episode, and then use it 
			in the j-measure calculation for the p(consequent) calculation.

	4/11/2002 changed the definition of an occurrence that
			occurs between an antecedent and a consequent

		now:  once an occurrence is found that meets our criteria, keep it
		before:  we made sure it was the minimal occurrence
		reason for change:  if several ant occurrences are in a row
				and several cons occurrences are in  a row
				then, we have several occurrences, not just
				the single occurrence between the closest 
				ant and cons occurrences.
	3/21/2002 output last occurred starting and ending time
	3/18/2002 update documentation
	2/2002  fix j-measure
	10/2001 add j-measure
  
	
**********************************************************
	  description: System documentation for the MOWCATL system (previously called CT-MINEPI)

The implementation for MOWCATL (Minimal Occurrence Episodes) 
with Constraints and Timelags. (Yes Timelags are now here!)

  (does not have closures, but this could be added)

MOWCATL incorporates Manilla's et al's MINEPI algorithm, 
	Representative association rules from Kryszkiewicz, constraints from Srikant, which 
	are all built on Agrawals' apriori algorithm.
	ADDS timelags and MUST MUST have  ruleconstraints, and the antecedent and
	consequent constraints must not overlap.

MOWCATL uses timestamp episodes for serial items, and event episodes for parallel items.
MOWCATL uses constraints to keep track of only the events the user is interested in
MOWCATL uses rule antecedent and consequence constraints to keep track of only the events
the user is interested on the left and right side of the rule, respectively.
MOWCATL can show representative association rules or all associations rules.
MOWCATL is FAST YES, FAST for both serial rules and parallel rules.

  Using constraints greatly reduces the running time (as well as the number of rules presented), 
  so you can quickly focus on the interesting rules.
 
   Representative association rules also gives you a smaller set of rules (especially for
  parallel items), but still shows the interesting ones.  Each rule covered by a
  representative association rule has at least the same confidence as the RAR that covers it. 
  (But the covered rules may infact have a higher confidence.)  When you show all rules, 
  you will see each rule's exact confidence value.
 

MOWCATL has three window parameters: the maximum window width of the antecedent, 
   the maximum window width of the consequent, and the maximum width of the time lag.

  RULES MUST BE LAGGED AS FOLLOWS, for EITHER SERIAL OR PARALLEL
  the starting time of the antecedent must be less than
	the starting time of the consequent
  the starting time of the consequent must be less than or equal to the
  starting time of the antecedent plus the timelag.  (A zero timelag is not allowed.)  
  If you don't want a time lag, use REAR.  
  Also, the consequence ending time must be greater 
  than the ending time of the antecedent.
  These requirements are per Tsegaye Tadesse, and can be changed if the needs change.

  For parallel episodes, the starting time of the consequent and the starting time of the
  antecedent can differ at most by the timelag, but the antecedent may not be first in time.
  The order of the events in the parallel   episodes is not important, but basically can be 
  used to see if the events in one episode occur  "close" to the events in the other episode.
  
  Counting the frequency of an episode in MOWCATL is DIFFERENT than how it is done in REAR.
  We count the actual minimal occurrences of the episode rather than how many 
  sliding windows it occurs in. (With a sliding window, one event at a given timestamp
  occurs in as many windows as the width of the window.  This is because the window slides
  through time.)  Here, we find the smallest amount of time between the start and end of the
  episode. This counts as one minimal occurrence, if it is within the window width.  The maximum 
  window width for an antecendent episode, is the antecedent window width. The maximum window 
  width for a consequence episode, is the consequence window width.  The maximum window width
  for a combined episode is described in the two preceeding paragraphs, where the starting time,
  ending times, and timelag all factor in.

  We first go through the data file storing the occurrences of the single events (or timestamps
  in the case of serial episodes), for those events in the antecedent and consequence 
  constraints (separately).  Then, looking through the single items, we find
  where pairs minimally occur. And so on, until we have no more candidates to look through.

  Then we go through the antecedent frequent episodes and match them to the consequence
  frequent episodes and find the minimal occurrences of these matches.
  The frequent matches are used to create the rules from.  But the rules can only have
  antecendent events/timestamps on the left, and consequence event/timestamps on the right.

  It is fast because it reads through the data only one time.
	
   Varying the minimum frequency, window width, and confidence values change the results, to
   either add more rules or remove rules, depending on what you need.  The wider the window,
   the more relationships you will find, but the less meaningful they will be in nature.  The
   smaller the frequency, the more episodes you will find (and thus more rules), but
   their occurrence is not as frequent.  The smaller the confidence, the more rules
   you will find, but the confidence in the rules with smaller confidence values is reduced.

******************************************************************************
Known bugs:
  The constraints are all disjunctive singular events.
  Input validation is not done

*******************************************************************************
Assumptions:

  Parameters have been set (done in the main program).  These include:

  1. Data file istream& fin has been opened.  It is a flat file with one record for each event per timestamp.
     The record contains the time stamp followed by the event.
	 The file must be in increasing time order, but may include events at the same
	 time stamp.
  2. itemsets& fca and fcc (frequent candidate antecedent, frequent candidate consequence)
	have been initialized.  These are the current set of candidates of length k 
		that meet the constraints.
  3.fa[k] and fc[k] keep the items from fca and fcc respectively that are frequent. 
  4. int wina - the size of the maximum width of an antecedent episode.  Must be greater than 0.
  5. int winc - the size of the maximum width of a consequent episode.  Must be greater than 0.
  6. int timelag - the size of the maximum difference in time between the start of the
	 antecedent episode and the start of the consequence episode.  Must be greater than 0.
  7. int min_fr - the minimum frequency.  Only episodes that are at least this 
     frequent will be kept from the list of candidate episodes.  
	 This is a raw count and must be greater than 0.
	 This program could be modified to use a percent of windows instead of the raw count.
  8. char type - the type of episodes we are looking for. Must have one of the values:
		p = parallel injunctive, currently implemented.
		q = parallel non-injunctive, implemented
		s = serial injunctive, not yet implemented
		t = serial noninjunctive, not yet implemented
 10. const event_set& T - the set of rule antecedent constraints.   Read from a separate file, such as constraints.txt.
     Currently, the constraints are singletons - any event not in the list
	 will not be included in the analysis.  This needs to be modified to allow
	 for constraints in DNF format.  
 11. const event_set& C - the set of rule consequence constraints.  Read from a separate file, such as constraints.txt.
     Currently, the constraints are singletons - any event not in the list
	 will not be included in the analysis.  This needs to be modified to allow
	 for constraints in DNF format.  

 For serial episodes, we need to know how many data sources, and read each set of events separately, such as epsi1.txt epsi2.txt
 const event_set& T - the set of antecedent rule constraints.  Read from a separate file, such as rulet.txt.
 const event_set& C - the set of consequence rule constraints.  Read from a separate file, such as rulec.txt.
     Currently, the constraints are singletons - any event not in the list
	 will not be included in the analysis.  This needs to be modified to allow
	 for constraints in DNF format.  
 We can output either the representative association rules,
 or the entire list of association rules, depending on the user choice
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

  
********************************************************************	
To run:
  1. give the parameter file name (parameters in this file  (in order) are:
		count of how often an episode needs to occurs to be kept (int)
		minimum confidence as a decimal
		antecedent max window width (int)
		time lag (int)
		consequence max window width (int)
		type (p, q, s, t - for parallel, parallel with repeats, serial, or serial with repeats)
 2. give the antecedent constraint file name (list of all events to be considered in the antecedent)
 3. give the consequent constraint file name (list of all events to be considered i the consequent)
 4. give the data file name (each event with its timestamp is on its own line.  Several events in a row may have the same timestamp.)
 5. give the output file name
 6. give the type of rules to use.

*******************************************************************************/
#pragma warning(disable:4786)
