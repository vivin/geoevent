#include "xmlintface.h"
#include "conversions.h"

const int MINSOURCES=1;
const int MAXSOURCES=100;

XMLIntFace::XMLIntFace(){
	timer = new Timer(); 

}

XMLIntFace::~XMLIntFace(){
	delete timer;
}

string XMLIntFace::runMo(string paramXML){
//Purpose - To run the MOCATL algorithm using the passed in parameters.
//paramXML - An XML string
	string strVal, eList, resultXML, epCntsXML, ruleListXML, ruleCntsXML;
	string *eSet;
	int eSetLen;
	event_set F,  // constraint sets:
			  A,  // A for antecendents
			  C;  // C for consequences
	event_vec_vec TimeFvec;

	//read in the parameters from the passed in XML string
	getParameters(paramXML);
	
	//read in the list of events from the passed in XML string
	eList = getEventList(paramXML);


	//start the timer that will be used to determine if the algorithm is taking too long
	//to execute and we need to cut out of it.
	timer->setMaxTimeAllowed(maxTimeAllowed);
	timer->start();

	//when the window width is 1 timestamp, always compute as parallel episodes,
	//since there really is no serial episode then.
	if (wina == 1 && lag == 0 && winc==1) {
		if (type == 's') {
			type = 'p';
		}
		else if (type == 't') {
			type = 'q';
		}
	}

	//get the constraints
	A = makeEventList(ants,true);
	C = makeEventList(cons,true);

	if(intersects(A,C)){
		errorHandler.addError("Anticedents and Consequents intersect",1000);
		return errorHandler.toXML();
	}

	switch (type)
	{
	case 'p':
	case 'q':

			break;

	case 's':
	case 't':
			eSet = getEventSets(paramXML, eSetLen);
			
			for (int i = 0; i < eSetLen;i++)
			{
				get_events(eSet[i], TimeFvec,A,C);	
			}

		break;
	}

	if(timer->isTimeOver()){
		errorHandler.addError("Maximum Time Limit Exceeded",2000);
		return errorHandler.toXML();
	}
	
	
	//Create the XML output
	resultXML = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
	resultXML += "<Results><Status>SUCCESS</Status>";
	//Add the parameters used to run the algorithm to the result XML string
	resultXML += outputparams(paramXML, type);
	
	int k = 0;
	switch(type)
	{
		case 'p':
		case 'q':

			Generate_Parallel_CTMINEPI(eList, min_density, min_freq, k, wina, winc, A, C, type, epCntsXML);
			if(timer->isTimeOver()){
				errorHandler.addError("Maximum Time Limit Exceeded",2000);
				return errorHandler.toXML();
			}

			Generate_MOWCATL_RULES(min_conf, k, lag, min_freq, type, A, C,ruletype, lagtype, ruleListXML, ruleCntsXML);
			//generate the CT MINEPI representative association rules
		    //generate the first set of minimal occurrences from the file
			break;
		case 's':
		case 't':
			Generate_Serial_CTMINEPI(eList, min_density, min_freq, k, wina, winc, A, C, type, eSetLen, TimeFvec, epCntsXML);
			if(timer->isTimeOver()){
				errorHandler.addError("Maximum Time Limit Exceeded",2000);
				return errorHandler.toXML();
			}				
			
			Generate_MOWCATL_RULES(min_conf, k, lag, min_freq, type, A, C,ruletype, lagtype, ruleListXML, ruleCntsXML);
			//generate the CT MINEPI representative association rules
			break;
		default:
			errorHandler.addError("Rule type invalid",1001);
			return errorHandler.toXML();			
	};

	if(timer->isTimeOver()){
		errorHandler.addError("Maximum Time Limit Exceeded",2000);
		return errorHandler.toXML();
	}	

	resultXML += epCntsXML;
	resultXML += ruleListXML;
	resultXML += ruleCntsXML;
	resultXML += "</Results>";

	if(errorHandler.hasError()){
		resultXML = errorHandler.toXML();
	}

	return resultXML;
}

event_set XMLIntFace::makeEventList(string constraints, bool sortit)
//Purpose - To store a comma delimited list of events in a vector
{
   event e;
   vector<string> event_List;
   event_set b;

   event_List = StringHelper::parse(constraints, ",");
 
   for( int i=0; i < event_List.size(); ++i) {
      e = *new event(event_List[i]);

      b.push_back(e);
   }
   if(sortit){
		sort(b.begin(), b.end());
   }
   return b;
}

bool XMLIntFace::intersects(const event_set& A, const event_set& C)
//Purpose - To check if any events are common to both the constraints and anticedents
{
	bool inter=false;
	for (event_set::const_iterator aitr = A.begin(); aitr != A.end(); aitr++)
	{
		if ( (*aitr).matchB(C) )
		{
			inter=true;
			break;
		}
	}
	return inter;
}

void XMLIntFace::get_events(string eventSet, event_vec_vec& TimeFvec, const event_set& A, const event_set& C)
//Purpose - To store the lists of possible events for serial episodes
{
	event_vec F;
	event e;
	vector<string> event_List;

	
	event_List = StringHelper::parse(eventSet, ",");

	for( int i=0; i < event_List.size(); ++i) {
		e = *new event(event_List[i]);
		
		if ( e.matchB(A) || e.matchB(C) )
		{
			F.push_back(e);
		}
	}
	
	TimeFvec.push_back(F);
}


string XMLIntFace::outputparams(string &inputXML, char &type)
//Purpose: To extract the parameters from the input XML string so the can be included
//		in the result XML string
//inputXML - The XML string that was passed in as input
//type - The type of episode that the algorithm is looking for.
{
	string paramsXML;
	//paramsXML -  The portion of the result XML string that contains the parameters used
	//			to run the algorithm
	paramsXML = "<Parameters>";
	paramsXML += StringHelper::extractXML(inputXML,"MinFreq",1,false);
	paramsXML += StringHelper::extractXML(inputXML,"MinConf",1,false);
	paramsXML += StringHelper::extractXML(inputXML,"AntWidth",1,false);
	paramsXML += StringHelper::extractXML(inputXML,"ConsWidth",1,false);
	paramsXML += StringHelper::extractXML(inputXML,"LagWidth",1,false);
	if(type == 's' || type == 't')
		paramsXML += "<EpisodeType>serial</EpisodeType>";
	else
		paramsXML += "<EpisodeType>parallel</EpisodeType>";
	paramsXML += StringHelper::extractXML(inputXML,"AllowRepeats",1,false);
	paramsXML += StringHelper::extractXML(inputXML,"Anticedents",1,false);
	paramsXML += StringHelper::extractXML(inputXML,"Consequents",1,false);
	paramsXML += StringHelper::extractXML(inputXML,"RuleType",1,false);
	paramsXML += "</Parameters>";

	return paramsXML;
}

void XMLIntFace::Generate_Parallel_CTMINEPI(string eList, int min_fr, double min_density, int& k, int wina, int winc, const event_set& A, const event_set& C, char type, string &epCntsXML)
{
	int Candidates=0;
	int FrequentEpisodes=0;
	int Iterations=1;

	unsigned int BeginTime = time(0);

	/* Step 1-3 for single events. Read the Data and record the minimal occurrences of the single events and record their support*/
	
	Read_Parallel_Data(eList, A, C);
	Candidates += fca[0].access_size();
	Candidates += fcc[0].access_size();

//	fca[0].output(cout);
//	fcc[0].output(cout);

	/* Step 4. Keep the frequent items */
	moitemset emptyset;
	fa.push_back(emptyset);//the vector must not be empty - or errors occur
	fc.push_back(emptyset);//the vector must not be empty - or errors occur
	//fa[0].addfreq(fca[k], garbage, min_freq);
	//fc[0].addfreq(fcc[k], garbage, min_freq);
	fc[0].addfreq(fcc[k], garbage, 0);
	fa[0].addDensity(fca[k], garbage, fc, type, lag, min_density, lagtype);
	//garbage.cleanup();


	FrequentEpisodes += fa[k].access_size();
	FrequentEpisodes += fc[k].access_size();
//	fa[k].output(cout);
//	fc[k].output(cout);
//	fa[k].output(outs);
//	fc[k].output(outs);
//	system("pause");

	/* Step 5. Create candidates of size k+1 */
	fca.push_back(emptyset); 
	fcc.push_back(emptyset); 

//		if (equalBF)
	fca[1].fast_generate_parallel_candidates(fa[0],type, k);
//		else
//			fcc[k+1].generate_parallel_candidates(fc[k], type, F, B, k);
//			use this when DNF constraints are fixed.

//		if (equalBF)
	//fcc[1].fast_generate_parallel_candidates(fc[0],type, k);
//		else
//			fcc[k+1].generate_parallel_candidates(fc[k], type, F, B, k);
//			use this when DNF constraints are fixed.


	if(timer->isTimeOver()){
		 int g;
			for ( g = 0; (!(fa[g].empty()) ); g++)
			{
				fa[g].cleanup();
			}
			for ( g = 0; (!(fc[g].empty()) ); g++)
			{
				fc[g].cleanup();
			}
			for ( g = 0; (!(fca[g].empty()) ); g++)
			{
				fca[g].cleanup();
			}
			for ( g = 0; (!(fcc[g].empty()) ); g++)
			{
				fcc[g].cleanup();
			}
			//garbage.cleanup();
			return;
	}

	Candidates += fca[1].access_size();
//		fca[1].output(cout);

	//Candidates += fcc[1].access_size();
//		fcc[1].output(cout);

	

	/* Now do the same while there are more candidates to look through */
	for (k = 1; (!(fca[k].empty()) /*|| !(fcc[k].empty())*/); k++)
	{
		Iterations++;

		/* Step 2. Record the minimal occurrences */
		if (!fca[k].empty())
			fca[k].Gen_mo(fa[k-1], k, type);
		/*if (!fcc[k].empty())
			fcc[k].Gen_mo(fc[k-1], k, type);*/

		/*Step 3. Update support */  
		//this is kept separate from the previous step to allow for a range of windows to work
		// you can iterate and output for a range of window widths
		if (!fca[k].empty())
			fca[k].Update_Support(wina);
		/*if (!fcc[k].empty())
			fcc[k].Update_Support(winc);*/

		/* Step 4. Keep the frequent items */


		if(timer->isTimeOver()){
		 int g;
			for ( g = 0; (!(fa[g].empty()) ); g++)
			{
				fa[g].cleanup();
			}
			for ( g = 0; (!(fc[g].empty()) ); g++)
			{
				fc[g].cleanup();
			}
			for ( g = 0; (!(fca[g].empty()) ); g++)
			{
				fca[g].cleanup();
			}
			for ( g = 0; (!(fcc[g].empty()) ); g++)
			{
				fcc[g].cleanup();
			}
			//garbage.cleanup();
			return;
		}

		fa.push_back(emptyset);
		//fc.push_back(emptyset);
		if (!fca[k].empty())
		{
			//fa[k].addfreq(fca[k], garbage, min_fr);
			fa[k].addDensity(fca[k], garbage, fc, type, lag, min_density, lagtype);
			//garbage.cleanup();
			FrequentEpisodes += fa[k].access_size();
//			fa[k].output(outs);
//			fa[k].output(cout);
		}

		/*if (!fcc[k].empty())
		{
			//fc[k].addfreq(fcc[k], garbage, min_freq);
			fc[k].addfreq(fcc[k], garbage, 0);
			//garbage.cleanup();
			FrequentEpisodes += fc[k].access_size();
//			fc[k].output(outs);
//			fc[k].output(cout);
		}*/


		if(timer->isTimeOver()){
		 int g;
			for ( g = 0; (!(fa[g].empty()) ); g++)
			{
				fa[g].cleanup();
			}
			for ( g = 0; (!(fc[g].empty()) ); g++)
			{
				fc[g].cleanup();
			}
			for ( g = 0; (!(fca[g].empty()) ); g++)
			{
				fca[g].cleanup();
			}
			for ( g = 0; (!(fcc[g].empty()) ); g++)
			{
				fcc[g].cleanup();
			}
			//garbage.cleanup();
			return;
		}

		fca.push_back(emptyset); 
		//fcc.push_back(emptyset);
	/* Step 5. Create candidates of size k+1 */
		if (!fca[k].empty())
		{
	//		if (equalBF)
			fca[k+1].setTimer(timer);	
			fca[k+1].fast_generate_parallel_candidates(fa[k],type, k);
	//		else
	//			fcc[k+1].generate_parallel_candidates(fc[k], type, F, B, k);
	//			use this when DNF constraints are fixed.
//			fca[k+1].output(cout);
			Candidates += fca[k+1].access_size();
		}

		if(timer->isTimeOver()){
		 int g;
			for ( g = 0; (!(fa[g].empty()) ); g++)
			{
				fa[g].cleanup();
			}
			for ( g = 0; (!(fc[g].empty()) ); g++)
			{
				fc[g].cleanup();
			}
			for ( g = 0; (!(fca[g].empty()) ); g++)
			{
				fca[g].cleanup();
			}
			for ( g = 0; (!(fcc[g].empty()) ); g++)
			{
				fcc[g].cleanup();
			}
			//garbage.cleanup();
			return;
		}

	/*	if (!fcc[k].empty())
		{
	//		if (equalBF)
			fcc[k+1].setTimer(timer);	
			fcc[k+1].fast_generate_parallel_candidates(fc[k],type, k);
	//			fcc[k+1].output(cout);
	//		else
	//			fcc[k+1].generate_parallel_candidates(fc[k], type, F, B, k);
	//			use this when DNF constraints are fixed.


//			fcc[k+1].output(cout);
			Candidates += fcc[k+1].access_size();
		}*/

		if(timer->isTimeOver()){
		 int g;
			for ( g = 0; (!(fa[g].empty()) ); g++)
			{
				fa[g].cleanup();
			}
			for ( g = 0; (!(fc[g].empty()) ); g++)
			{
				fc[g].cleanup();
			}
			for ( g = 0; (!(fca[g].empty()) ); g++)
			{
				fca[g].cleanup();
			}
			for ( g = 0; (!(fcc[g].empty()) ); g++)
			{
				fcc[g].cleanup();
			}
			//garbage.cleanup();
			return;
		}

	} //for
	
	k--;

	while (k>0 &&  fa[k].empty()) //don't use the empty itemsets in generating rules
	{
//		cout<<"changed k ";   
		k--;
	}
//	cout<<"k="<<k<<endl;
//	system("pause");

	unsigned int Time = time(0) - BeginTime;
//	cout<<"k="<<k<<endl;
//	system("pause");

	epCntsXML = outputEpisodeCounts(double(min_fr)/fa[k].access_num_records() , Candidates, FrequentEpisodes, Iterations, Time);
	
	//garbage.cleanup();
/*	//testing:
	for (int i = 0; i <=k;i++)
		fa[i].output(cout);
	cout<<endl;
	*/
/*	//testing:
	for (int i = 0; i <=k;i++)
		fc[i].output(cout);
	cout<<endl;
	*/


} //end Generate_Parallel_CTMINEPI

void XMLIntFace::Generate_MOWCATL_RULES(double min_conf, int maxk, int lag, int min_fr1, char type, const event_set& T, const event_set& C,int ans, char lag_type, string& ruleListXML, string& ruleCntsXML)
{


	unsigned int BeginTime = time(0);
	//bool rar = false;
/*	if (ans ==2)  //modified 2/19/05 to remove representative rules
		rar=true;
*/
	moitemset empty;
    int i;
	for (i = 0; i <= maxk; i++){
		fac.push_back(empty);
	}


	//cout<<"maxk="<<maxk<<endl; 

	//use only the active itemsets and combine them into a set of ruleitems
	for (i = 0; i <=maxk;i++)
	{
		
//			cout<<"i="<<i<<endl;
		if (!fa[i].empty()){
			fac[i].setTimer(timer);
			fac[i].combine(fa[i], garbage, fc, type,lag, min_fr1, lag_type);
		}
			
		if(timer->isTimeOver()){
		 int g;
			for ( g = 0; (!(fa[g].empty()) ); g++)
			{
				fa[g].cleanup();
			}
			for ( g = 0; (!(fc[g].empty()) ); g++)
			{
				fc[g].cleanup();
			}
			for ( g = 0; (!(fca[g].empty()) ); g++)
			{
				fac[g].cleanup();
			}
			//garbage.cleanup();
			return;
		}

/*		if (!fac[i].empty() )
		{
			fac[i].output(cout);
			fac[i].output(out);
			system("pause");
		}
*/
	}

/*  //modified 2/19/05 removed this code that created a split_fac 
	// we nolonger find representative rules
	//split the frequent episodes according to size
	fc_type split_fac;
	int largest = moitemset::seeantlargest() * moitemset::seeconslargest();
	int k=0;
    int h;
	for (h = 0; h < largest; h++){
		if(timer->isTimeOver()){
		 int g;
			for ( g = 0; (!(fa[g].empty()) ); g++)
			{
				fa[g].cleanup();
			}
			for ( g = 0; (!(fc[g].empty()) ); g++)
			{
				fc[g].cleanup();
			}
			for ( g = 0; (!(fca[g].empty()) ); g++)
			{
				fac[g].cleanup();
			}
			return;
		}
		split_fac.push_back(empty);
	}

	//use only the active itemsets
	for (h = 0; h <=maxk;h++)
	{
		if (!fac[h].empty())
			fac[h].putinsplit(split_fac, k);
	}
	

	if(timer->isTimeOver()){
		 int g;
			for ( g = 0; (!(fa[g].empty()) ); g++)
			{
				fa[g].cleanup();
			}
			for ( g = 0; (!(fc[g].empty()) ); g++)
			{
				fc[g].cleanup();
			}
			for ( g = 0; (!(split_fac[g].empty()) ); g++)
			{ 
                 fac[g].cleanup();
			}
		return;
	}
*/

//maxk is now the index for the largest
//0 is the index for 1-items
	if (type == 'p' || type == 'q')
	{

		moruleset REAR;
		int i;
		for (i = maxk; i>=0;i--) //mod 2.19 - use maxk instead of k
		{
			//Testing:
//					fac[i].output(cout);
			//		cout<<endl;


			if(timer->isTimeOver()){
			int g;
			for ( g = 0; (!(fa[g].empty()) ); g++)
			{
				fa[g].cleanup();
			}
			for ( g = 0; (!(fc[g].empty()) ); g++)
			{
				fc[g].cleanup();
			}
			for ( g = 0; (!(fac[g].empty()) ); g++)
			{ 
                 fac[g].cleanup();
			}
			REAR.cleanup();
			//garbage.cleanup();
				return;
			}//end out of time process
/* 
			if (!split_fac[i].empty())  //commented out 2/19/05 
				if (1==ans)
					split_fac[i].addrules(min_conf, REAR, i);
					else  
					split_fac[i].loop1parallel(split_fac, min_conf, REAR, i, k, type, T,C);
*/

			if (!fac[i].empty())
				fac[i].addrules(min_conf, REAR, i);

		 	}                        //end for loop


//		if (2==ans)  //commented out 2/19/05 
//		{
//			out<<"Representative Association Rules represent several rules that have at least the given confidence values"<<endl;
//		}
//		REAR.output(cout,rar);

		// Dan Li, Oct 29, overwrite output function with one more function "type"
		//REAR.output(out,rar);
		ruleListXML = REAR.outputXML(type);

		ruleCntsXML = outputRuleCounts(min_conf, REAR.AccessCount(), time(0) - BeginTime, moitemset::seenum_records());
  
              //done with the output, now start final cleanup
	 		int g;
			//for ( g = 0; (!(fa[g].empty()) ); g++) 03/07/05
			for( g = 0; g < fa.size(); g++ )
			{
				fa[g].cleanup();
			}			
			
			//for ( g = 0; (!(fc[g].empty()) ); g++)	02/16/05 
			for( g = 0; g < fc.size(); g++ )		
			{
				cout << "g = ";
				cout << g << endl;
				fc[g].cleanup();
			}
			for ( g = 0; (!(fca[g].empty()) ); g++)
			{
				fac[g].cleanup();
			}
    	REAR.cleanup();
		//garbage.cleanup();
	}    //end if parallel
	else
	{

		mos_ruleset REAR;
		int i;
		for (i = maxk; i>=0;i--) //mod 2.19 - use maxk instead of k
		{
			if(timer->isTimeOver()){
			int g;
			for ( g = 0; (!(fa[g].empty()) ); g++)
			{
				fa[g].cleanup();
			}
			for ( g = 0; (!(fc[g].empty()) ); g++)
			{
				fc[g].cleanup();
			}
			for ( g = 0; (!(fac[g].empty()) ); g++)
			{ 
                 fac[g].cleanup();
			}
			REAR.cleanup();
			//garbage.cleanup();
				return;
			}
			 
			//Testing:
			//		fac[i].output(cout);
			//		cout<<endl;
/*			if (!split_fac[i].empty())  //commented out 2/19/05
				if (1==ans)
	 				split_fac[i].addrules(min_conf, REAR, i);
				else
					split_fac[i].loop1serial(split_fac, min_conf, REAR, i, k, type, T,C);
*/
			if (!fac[i].empty())
				fac[i].addrules(min_conf, REAR, i);
		}

//		if (2==ans)  //commented out 2/19/05
//		{
//			out<<"Representative Association Rules represent several rules that have at least the given confidence values"<<endl;
//		}
//		REAR.output(cout,rar);
	
		// Dan Li, Oct 29, overwrite output function with one more function "type"
		//REAR.output(out,rar);
		
		if(timer->isTimeOver()){
		  int g;
			for ( g = 0; (!(fa[g].empty()) ); g++)
			{
				fa[g].cleanup();
			}
			for ( g = 0; (!(fc[g].empty()) ); g++)
			{
				fc[g].cleanup();
			}
			for ( g = 0; (!(fac[g].empty()) ); g++)
			{ 
                 fac[g].cleanup();
			}
			REAR.cleanup();
			//garbage.cleanup();
				return;
		}
		
		int ruleGenTime = time(0) - BeginTime;

		ruleListXML = REAR.outputXML(type);
 
		if(timer->isTimeOver()){
			REAR.cleanup();
	 		int g;
			for ( g = 0; (!(fa[g].empty()) ); g++)
			{
				fa[g].cleanup();
			}
			for ( g = 0; (!(fc[g].empty()) ); g++)
			{
				fc[g].cleanup();
			}
			for ( g = 0; (!(fac[g].empty()) ); g++)
			{ 
                 fac[g].cleanup();
			}
			//garbage.cleanup();
				return;
		}

		ruleCntsXML = outputRuleCounts(min_conf, REAR.AccessCount(), ruleGenTime, moitemset::seenum_records());
			REAR.cleanup();
 		int g;
			for ( g = 0; (!(fa[g].empty()) ); g++)
			{
				fa[g].cleanup();
			}
			for ( g = 0; (!(fc[g].empty()) ); g++)
			{
				fc[g].cleanup();
			}
			for ( g = 0; (!(fac[g].empty()) ); g++)
			{ 
                 fac[g].cleanup();
			}
			//garbage.cleanup();
 	
	}


}

void XMLIntFace::Generate_Serial_CTMINEPI(string eList, int min_fr, double min_density, int& k, int wina, int winc, const event_set& A, const event_set& C, char type, int sources, const event_vec_vec& TimeFvec, string &epCntsXML)
{

	int Candidates=0;
	int FrequentEpisodes=0;
	int Iterations=1;
	k=0;
	
	

	unsigned int BeginTime = time(0);

	/* Step 1-3 for single events. Read the Data and record the minimal occurrences of the single events and record their support*/
	Read_Serial_Data(eList, A, C);

	fca[k].setTimer(timer);
	fcc[k].setTimer(timer);

	if(timer->isTimeOver()){
		 int g;
			for ( g = 0; (!(fa[g].empty()) ); g++)
			{
				fa[g].cleanup();
			}
			for ( g = 0; (!(fc[g].empty()) ); g++)
			{
				fc[g].cleanup();
			}
			for ( g = 0; (!(fca[g].empty()) ); g++)
			{
				fca[g].cleanup();
			}
			for ( g = 0; (!(fcc[g].empty()) ); g++)
			{
				fcc[g].cleanup();
			}
		return;
	}
	
	fca[k].addtimestampitems(TimeFvec, A, sources,wina); //This is the part that takes a long time
	
	if(timer->isTimeOver()){
		 int g;
			for ( g = 0; (!(fa[g].empty()) ); g++)
			{
				fa[g].cleanup();
			}
			for ( g = 0; (!(fc[g].empty()) ); g++)
			{
				fc[g].cleanup();
			}
			for ( g = 0; (!(fca[g].empty()) ); g++)
			{
				fca[g].cleanup();
			}
			for ( g = 0; (!(fcc[g].empty()) ); g++)
			{
				fcc[g].cleanup();
			}
		return;
	}
	
	fcc[k].addtimestampitems(TimeFvec, C, sources,winc);
	
	if(timer->isTimeOver()){
		 int g;
			for ( g = 0; (!(fa[g].empty()) ); g++)
			{
				fa[g].cleanup();
			}
			for ( g = 0; (!(fc[g].empty()) ); g++)
			{
				fc[g].cleanup();
			}
			for ( g = 0; (!(fca[g].empty()) ); g++)
			{
				fca[g].cleanup();
			}
			for ( g = 0; (!(fcc[g].empty()) ); g++)
			{
				fcc[g].cleanup();
			}
		return;
	}


	Candidates += fca[k].access_size();
	Candidates += fcc[k].access_size();


//	fca[k].output(cout);
//	fcc[k].output(cout);
//	system("pause");
	 
	/* Step 4. Keep the frequent items */
	moitemset emptyset;
	fa.push_back(emptyset);//the vector must not be empty - or errors occur
	fc.push_back(emptyset);//the vector must not be empty - or errors occur
	//fa[k].addfreq(fca[k], garbage, min_fr);
	//fc[k].addfreq(fcc[k], garbage, min_fr);
	fa[k].addDensity(fca[k], garbage, fc, type, lag, min_density, lagtype);
	fc[k].addfreq(fcc[k], garbage, 0);
	//garbage.cleanup();
	FrequentEpisodes += fa[k].access_size();
	FrequentEpisodes += fc[k].access_size();
//	fa[k].output(cout);
//	fc[k].output(cout);
//	fa[k].output(outs);
//	fc[k].output(outs);
//	system("pause");
 

	/* Step 5. Create candidates of size k+1 */
	fca.push_back(emptyset); 
	fcc.push_back(emptyset); 

	
//		if (equalBF)
	fca[k+1].fast_generate_serial_candidates(fa[k],type, k);
//		else
//			fcc[k+1].generate_serial_candidates(fc[k], type, F, B, k);
//			use this when DNF constraints are fixed.
 
//		if (equalBF)
	fcc[k+1].fast_generate_serial_candidates(fc[k],type, k);
//		else
//			fcc[k+1].generate_serial_candidates(fc[k], type, F, B, k);
//			use this when DNF constraints are fixed.
 

	Candidates += fca[k+1].access_size();
//		fca[k+1].output(cout);

	Candidates += fcc[k+1].access_size();
//		fcc[k+1].output(cout);
//		system("pause");

	

	/* Now do the same while there are more candidates to look through */
	for (k = 1; (!(fca[k].empty()) || !(fcc[k].empty())); k++)
	{

		if(timer->isTimeOver()){
		 int g;
			for ( g = 0; (!(fa[g].empty()) ); g++)
			{
				fa[g].cleanup();
			}
			for ( g = 0; (!(fc[g].empty()) ); g++)
			{
				fc[g].cleanup();
			}
			for ( g = 0; (!(fca[g].empty()) ); g++)
			{
				fca[g].cleanup();
			}
			for ( g = 0; (!(fcc[g].empty()) ); g++)
			{
				fcc[g].cleanup();
			}
			//garbage.cleanup();
			return;
		}

		Iterations++;

		/* Step 2. Record the minimal occurrences */
		if (!fca[k].empty())
			fca[k].Gen_mo(fa[k-1], k, type);
		if (!fcc[k].empty())
			fcc[k].Gen_mo(fc[k-1], k, type);


		/*Step 3. Update support and remove the occurrences that do not meet the window width*/  
		//this is kept separate from the previous step to allow for a range of windows to work
		// you can iterate and output for a range of window widths
		if (!fca[k].empty())
			fca[k].Update_Support(wina);
		if (!fcc[k].empty())
			fcc[k].Update_Support(winc);


//		fcc[k].output(cout);
//		fca[k].output(cout);
//		system("pause");

		/* Step 4. Keep the frequent items */

			fc.push_back(emptyset);
			fa.push_back(emptyset);
		if (!fca[k].empty())
		{
			//fa[k].addfreq(fca[k], garbage, min_fr);
			fa[k].addDensity(fca[k], garbage, fc, type, lag, min_density, lagtype);
			//garbage.cleanup();
//			fa[k].output(outs);
//			fa[k].output(cout);
			FrequentEpisodes += fa[k].access_size();
		}


		if (!fcc[k].empty())
		{
			//fc[k].addfreq(fcc[k], garbage, min_fr);
			fc[k].addfreq(fcc[k], garbage, 0);
			//garbage.cleanup();
//			fc[k].output(outs);
//			fc[k].output(cout);
			FrequentEpisodes += fc[k].access_size();
		}


			fca.push_back(emptyset); 
			fcc.push_back(emptyset); 
	/* Step 5. Create candidates of size k+1 */
		if (!fca[k].empty())
		{
	//		if (equalBF)
				fca[k+1].setTimer(timer);
				fca[k+1].fast_generate_serial_candidates(fa[k],type, k);
	//		else
	//			fcc[k+1].generate_serial_candidates(fc[k], type, F, B, k);
	//			use this when DNF constraints are fixed.
			Candidates += fca[k+1].access_size();
	//		fca[k+1].output(cout);
		}


		if (!fcc[k].empty())
		{
	//		if (equalBF)
				fcc[k+1].setTimer(timer);
				fcc[k+1].fast_generate_serial_candidates(fc[k],type, k);
	//		else
	//			fcc[k+1].generate_serial_candidates(fc[k], type, F, B, k);
	//			use this when DNF constraints are fixed.

			Candidates += fcc[k+1].access_size();
	//		fcc[k+1].output(cout);
	//		system("pause");
		}

	} //for
	

	k--;
	while (k>0 && fa[k].empty()) //don't use the empty itemsets in generating rules
	{
		k--;
	}

//	cout<<"k="<<k<<endl;
//	system("pause");

	unsigned int Time = time(0) - BeginTime;
	
	epCntsXML = outputEpisodeCounts(double(min_fr)/fa[k].access_num_records() , Candidates, FrequentEpisodes, Iterations, Time);

	//garbage.cleanup();
/*	//testing:
	for (int i = 0; i <=k;i++)
		fa[i].output(cout);
	cout<<endl;
	*/
/*	//testing:
	for (int i = 0; i <=k;i++)
		fc[i].output(cout);
	cout<<endl;
	*/
} //end Generate_serial_CTMINEPI

void XMLIntFace::Read_Parallel_Data(string eList, const event_set& A, const event_set& C)
//generate the first set of minimal occurrences from the file 
	/*
	This is currently set up to only read a time stamp followed by an event
	from a flat file.
	This would need to be modified for other types of files or file formats
	*/
{	
	int numEvents,i, tagStart=0;
	string eventXML;

	moitemset emptyset;
	moitemset::setnum_records(0);
	fca.push_back(emptyset); //the vector must not be empty - or errors occur
	fcc.push_back(emptyset); //the vector must not be empty - or errors occur
	fca[0].get_parallel_episodes(A);
	fcc[0].get_parallel_episodes(C); //get singleton episodes that match the constraints
			
	int t, previous_t = -1; // previous_t is added by Dan Li to count the number of timestamps
	event E;

	numEvents = StringHelper::occurs(eList,"<Event>");

	for(i=0;i<numEvents;i++){
		eventXML = StringHelper::extractXML(eList,tagStart,"Event",false);
	 
		t = atoi((StringHelper::extractXML(eventXML,"TimeStamp",1,true)).c_str());
		E = event(StringHelper::extractXML(eventXML,"Value",1,true));
		/* Bring new events into the window,
		/* while the timestamp is the current timestamp: */

		// Dan Li modify 9/
		//moitemset::incrnum_records(1);
		if (previous_t != t) 
		{
			moitemset::incrnum_records(1);
			previous_t = t;
		}
		// End of modify

		if (E.matchB(A))
			fca[0].bringinnew(E, t);
		else if (E.matchB(C) )
			fcc[0].bringinnew(E, t);		
	}

	//testing
//	fca[0].output(cout);
//	system("pause");
//	fcc[0].output(cout);
}

void XMLIntFace::Read_Serial_Data(string eList, const event_set& A, const event_set& C)
//generate the first set of minimal occurrences from the file 
	/*
	This is currently set up to only read a time stamp followed by an event
	from a flat file.
	This would need to be modified for other types of files or file formats
	*/
{	
	int numEvents,i, tagStart=0;
	string eventXML;
	
	moitemset emptyset;
	moitemset::setnum_records(0);
	fca.push_back(emptyset); //the vector must not be empty - or errors occur
	fcc.push_back(emptyset); //the vector must not be empty - or errors occur
	fca[0].get_serial_episodes(A);
	fcc[0].get_serial_episodes(C); //get singleton episodes that match the constraints
			
	int t, previous_t = -1; // previous_t is added by Dan Li to count the number of timestamps
	event E;

	numEvents = StringHelper::occurs(eList,"<Event>");
	
	for(i=0;i<numEvents;i++){
		eventXML = StringHelper::extractXML(eList,tagStart,"Event", false);
		
		t = atoi((StringHelper::extractXML(eventXML,"TimeStamp",1,true)).c_str());
		E = event(StringHelper::extractXML(eventXML,"Value",1,true));
		/* Bring new events into the window,
		/* while the timestamp is the current timestamp: */

		// Dan Li modify 9/
		//moitemset::incrnum_records(1);
		if (previous_t != t) 
		{
			moitemset::incrnum_records(1);
			previous_t = t;
		}
		// End of modify

		if (E.matchB(A) ){
			fca[0].bringinnew(E, t);
		}
		else if (E.matchB(C) ){
			fcc[0].bringinnew(E, t);		
		}
	}


	//testing
//	fca[0].output(cout);
//	system("pause");
//	fcc[0].output(cout);
	

/*
	moitemset emptyset;
	moitemset::setnum_records(0);
	fca.push_back(emptyset); //the vector must not be empty - or errors occur
	fcc.push_back(emptyset); //the vector must not be empty - or errors occur
	fca[0].get_serial_episodes(A);
	fcc[0].get_serial_episodes(C); //get singleton episodes that match the constraints

	int t, previous_t; // previous_t is added by Dan Li to count the number of records
 	event E;
	fin>>t;
// initialze previous_t and record number
	previous_t = t;
	moitemset::incrnum_records(1);

	while (!fin.eof())
	{
		// Bring new events into the window,
		// while the timestamp is the current timestamp: 
// Dan Li modify 9/
		//moitemset::incrnum_records(1);
		if (previous_t != t) 
		{
			moitemset::incrnum_records(1);
			previous_t = t;
		}
// End of modify
		
		fin>>E;
		if (E.matchB(A) )
			fca[0].bringinnew(E, t);
		else if (E.matchB(C) )
			fcc[0].bringinnew(E, t);
		fin>>t;
				
	}  //end while not eof
	//testing
//	fca[0].output(cout);
//	fcc[0].outpu
//	system("pause");*/
}

string XMLIntFace::outputEpisodeCounts(double FrequencyThreshold, int Candidates, int FrequentEpisodes, int Iterations, unsigned int Time)
{
	string epCntXML;
	string convertedNum, convertedDouble = "";
	//char convertedInt[30];
	//int sign, decimal,i;

	epCntXML = "<EpisodeCounts>";
	
	//Frequency Threshold tag
	epCntXML += "<FrequencyThreshold>";
	convertedNum = convertfcvt(FrequencyThreshold);
	StringHelper::trim(convertedNum,"0 \0");
	epCntXML += convertedNum;
	epCntXML += "</FrequencyThreshold>";

	//Candidates tag
	epCntXML += "<Candidates>";
	convertedNum = convertitoa(Candidates);
	StringHelper::trim(convertedNum," \0");
	epCntXML += convertedNum;
	epCntXML += "</Candidates>";

	//Frequent Episodes tag
	epCntXML += "<FrequentEpisodes>";
	convertedNum = convertitoa(FrequentEpisodes);
	StringHelper::trim(convertedNum," \0");
	epCntXML += convertedNum;
	epCntXML += "</FrequentEpisodes>";

	//Iterations tag
	epCntXML += "<Iterations>";
	convertedNum = convertitoa(Iterations);
	StringHelper::trim(convertedNum," \0");
	epCntXML += convertedNum;
	epCntXML += "</Iterations>";

	//Frequent Episode Generation Time tag
	epCntXML += "<FreqEpisodeGenTime>";
	convertedNum = convertitoa(Time);
	StringHelper::trim(convertedNum," \0");
	epCntXML += convertedNum;
	epCntXML += "</FreqEpisodeGenTime>";
	
	epCntXML += "</EpisodeCounts>";

	return epCntXML;
}

string XMLIntFace::outputRuleCounts(double min_confidence, int RuleCount, unsigned int Time, int Windows)
//Purpose: To put the rule counts into an XML string 
//Author: Matt Culver
//Last Modified: 6 July 2003
{
	string ruleCntsXML;
	string convertedNum, convertedDouble = "";
	//char convertedInt[30];
	//int sign, decimal,i;

	ruleCntsXML = "<RuleCounts>";
	
	//Support Threshold
	ruleCntsXML += "<SupportThreshold>";
	convertedNum = convertfcvt(min_confidence);
	StringHelper::trim(convertedNum,"0 \0");
	ruleCntsXML += convertedNum;
	ruleCntsXML += "</SupportThreshold>";
	
	//Distinct Rules
	ruleCntsXML += "<DistinctRules>";
	convertedNum = convertitoa(RuleCount);
	StringHelper::trim(convertedNum," \0");
	ruleCntsXML += convertedNum;
	ruleCntsXML += "</DistinctRules>";

	//Rule Generation Time
	ruleCntsXML += "<RuleGenTime>";
	convertedNum = convertitoa(Time);
	StringHelper::trim(convertedNum," \0");
	ruleCntsXML += convertedNum;
	ruleCntsXML += "</RuleGenTime>";

	ruleCntsXML += "</RuleCounts>";

	return ruleCntsXML;
}

void XMLIntFace::getParameters(string paramXML){
//Purpose:	To read in all of the algorithm parameters from the passed in XML string
//			and store them in the appropriate data members.
//Author:	Matt Culver
//Last Modified:	4 Sept 2003
//paramXML:	The XML string that is passed into the algorithm that contains all of the 
//			parameters and data.

	//Extract all of the parameters from the xml string 
	maxTimeAllowed = atoi((StringHelper::extractXML(paramXML,"MaxTimeAllowed",1,true)).c_str());

	min_freq = atoi((StringHelper::extractXML(paramXML,"MinFreq",1,true)).c_str());
	min_conf = atof((StringHelper::extractXML(paramXML,"MinConf",1,true)).c_str());
	min_density = atof((StringHelper::extractXML(paramXML,"MinDensity",1,true)).c_str());

	wina = atoi((StringHelper::extractXML(paramXML,"AntWidth",1,true)).c_str());

	winc = atoi((StringHelper::extractXML(paramXML,"ConsWidth",1,true)).c_str());
	lag = atoi((StringHelper::extractXML(paramXML,"LagWidth",1,true)).c_str());
	if(StringHelper::extractXML(paramXML,"EpisodeType",1,true) == "parallel"){
		if(StringHelper::extractXML(paramXML,"AllowRepeats",1,true) == "true")
			type = 'q';	
		else
			type = 'p';
	}else{
		if(StringHelper::extractXML(paramXML,"AllowRepeats",1,true) == "true")
			type = 't';
		else
			type = 's';
	}
	ants = StringHelper::extractXML(paramXML,"Anticedents",1,true);
	cons = StringHelper::extractXML(paramXML,"Consequents",1,true);
	if(StringHelper::extractXML(paramXML,"LagType",1,true) == "max")
		lagtype = 'm';
	else
		lagtype = 'f';
	ruletype = atoi((StringHelper::extractXML(paramXML,"RuleType",1,true)).c_str());
}

string XMLIntFace::getEventList(string paramXML){
//Purpose:	To parse out the eventlist portion of the passed in XML string
//			and return it.
//Author:	Matt Culver
//Returns:	string - The event list portion of the passed in XML string
//paramXML:	The XML string that is passed into the algorithm that contains all of the 
//			parameters and data.
	return StringHelper::extractXML(paramXML,"EventList",1,false);
}


string* XMLIntFace::getEventSets(string paramXML, int &eSetLen){
//Purpose:	To parse out the eventset portion of the passed in XML string
//			and return it.
//Author:	Matt Culver
//Returns:	string[] - An array of all the eventsets in the passed in XML string
//paramXML:	The XML string that is passed into the algorithm that contains all of the 
//			parameters and data.
//eSetLen:	The number of event sets.

	string eSetXML;
	string* eSet;
	int i;
	//eSetXML:	The event set portion of the passed in XML string.
	//eSet:		An array that holds all of the event sets.
	//i:		A variable for iterating throug for loops.
	eSetXML = StringHelper::extractXML(paramXML,"EventSet",1,false);
 
	eSetLen = StringHelper::occurs(eSetXML,"<ESValue>");

	//Make sure we have a valid number of event lists
	if(eSetLen < MINSOURCES || eSetLen > MAXSOURCES){
		errorHandler.addError("Number of envent sets invalid",1002);				
	}

	eSet = new string[eSetLen];
	
	for(i=0;i<eSetLen;i++){
		eSet[i] = StringHelper::extractXML(eSetXML,"ESValue",i+1,true);
	}
	
	return eSet;
}

/*
//This needs to go in a different object
bool isMaxTimeExceeded(){
//Purpose:	Checks if the maximum time that the algorithm is allowed to run is exceeded.
//Returns:	Boolean - Whether the max time has been exceeded
//Author:	Matt Culver
//Last Modified:	4 Sept 2003

	bool timeExceeded = (time(0) - startTime) > maxTimeAllowed;
	//timeExceeded:	Whether the max time allowed for the algorithm to run has been exceeded
	
	if(timeExceeded){
		errorHandler.addError("Maximum time allowed exceeded",2000)
	}
	
}

*/
