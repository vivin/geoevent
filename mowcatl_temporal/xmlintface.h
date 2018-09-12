#ifndef XMLIntFace_H
#define XMLIntFace_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <ctime>
#include <string>
#include "event.h"
#include "timestamp.h"
#include "occur.h"
#include "moitem.h"
#include "moitemset.h"
#include "morule.h"
#include "moruleset.h"
#include "moserialrule.h"
#include "moserialruleset.h"
#include "stringhelper.h"
#include "errorhandler.h"
#include "timer.h"

using namespace std;


typedef vector<event> event_vec;

//////////////////////////////////////////////////////////
//                                           
class XMLIntFace {
public:
	XMLIntFace();

	~XMLIntFace();

	string runMo(string paramXML);
	//Purpose - To run the MOCATL algorithm using the passed in parameters.
	//paramXML - An XML string that contains all of the parameters needed to run the 
	
	event_set makeEventList(string constraints, bool sortit);
	//Purpose - To store a comma delimited list of events in a vector

	bool intersects(const event_set& A, const event_set& C);
	//Purpose - To check if any events are common to both the constraints and anticedents

	void get_events(string eventSet, event_vec_vec& TimeFvec, const event_set& A, const event_set& C);
	//Purpose - To store the lists of possible events serial episodes

	string outputparams(string &inputXML, char &type);
	void output_constraints(ostream& out, event_set& B);
	void outputtype(char type, ostream& out,int ans);
	void Generate_Parallel_CTMINEPI(string eList, int min_fr, double min_density, int& k, int wina, int winc, const event_set& A, const event_set& C, char type, string &epCntsXML);
	void Generate_MOWCATL_RULES(double min_conf, int maxk, int lag, int min_fr, char type, const event_set& T, const event_set& C,int ans, char lag_type, string& ruleListXML, string& ruleCntsXML);
	void Generate_Serial_CTMINEPI(string eList, int min_fr, double min_density, int& k, int wina, int winc, const event_set& A, const event_set& C, char type, int sources, const event_vec_vec& TimeFvec, string &epCntXML);
	void Read_Serial_Data(string eList, const event_set& A, const event_set& C);
	void Read_Parallel_Data(string eList, const event_set& A, const event_set& C);
	string outputEpisodeCounts(double FrequencyThreshold, int Candidates, int FrequentEpisodes, int Iterations, unsigned int Time);
	string outputRuleCounts(double min_confidence, int RuleCount, unsigned int Time, int Windows);
	//Purpose: To put the rule counts into an XML string 
	//Author: Matt Culver
	//Last Modified: 6 July 2003
	void getParameters(string paramXML);
	//Purpose:	To read in all of the algorithm parameters from the passed in XML string
	//			and store them in the appropriate data members.
	//Author:	Matt Culver
	//Last Modified:	4 Sept 2003
	//paramXML:	The XML string that is passed into the algorithm that contains all of the 
	//			parameters and data.
	string getEventList(string paramXML);
	//Purpose:	To parse out the eventlist portion of the passed in XML string
	//			and return it.
	//Author:	Matt Culver
	//Returns:	string - The event list portion of the passed in XML string
	//paramXML:	The XML string that is passed into the algorithm that contains all of the 
	//			parameters and data.
	string* getEventSets(string paramXML, int &eSetLen);
	//Purpose:	To parse out the eventset portion of the passed in XML string
	//			and return it.
	//Author:	Matt Culver
	//Returns:	string[] - An array of all the eventsets in the passed in XML string
	//paramXML:	The XML string that is passed into the algorithm that contains all of the 
	//			parameters and data.

	
private:
	int min_freq,
		 wina,
		 winc,
		 lag,
		 ruletype;
	double min_conf;
	double min_density;
	char type, lagtype;
	string ants,
          cons;
	unsigned int maxTimeAllowed;
	ErrorHandler errorHandler;
	Timer* timer;

	vector<moitemset> fca;//candidate ant items
	vector<moitemset> fa; //frequent ant items
	vector<moitemset> fcc; //candidate consequent items
	vector<moitemset> fc;  //frequent cons items
	//vector<moitemset> fcac; //candidate rule items
	vector<moitemset> fac;  //frequent rule items
	moitemset garbage;

};
//////////////////////////////////////////////////////////
#endif

