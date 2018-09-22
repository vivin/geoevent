#include "cmdLineIntFace.h"

const int MINSOURCES=1;
const int MAXSOURCES=100;

int CmdLineIntFace::runMo()
{

    char par_file_name[256], in_file_name[256], out_file_name[256], event_file_name[256],
		ant_constraint_file_name[256], cons_constraint_file_name[256];
    ifstream par_stream, in_stream, constraint_stream, event_stream;
    ofstream out_stream;


    event_set F, A, C;  //constraint sets: A for antecendents, C for consequences
	t_stamp_vec SingleA, SingleC; //list of possible timestamp events (combination of events from all the data sources)
	event_vec_vec TimeFvec;

	//Set the parameters
    cout << "Enter the parameter file name:\n";
    cin >> par_file_name;

    par_stream.open(par_file_name, ios::in);
//    par_stream.open(par_file_name, ios::in|ios::nocreate);
    if (par_stream.fail( ))
    {
        cout << "Parameter file opening failed.\n";
        exit(1);
    }

	double min_confidence = 0;
	int min_freq = 1;
	int wina = 4;
	int winc = 3;
	int lag = 2;
//	int step = 1;
	int sources = 1;
	char type = 'p';

	get_pars(par_stream, min_freq, min_confidence, wina, lag, winc, type);
	par_stream.close();

	if (wina == 1 && lag == 0 && winc==1)
	//when the window width is 1 timestamp, always compute as parallel episodes,
	//since there really is no serial episode then.
	{
		if (type == 's')
		{
			type = 'p';
			cout<<"Because the window width is one timestamp,"<<endl
				<<"injective parallel episodes will be used instead serial episodes."<<endl;
		}
		else if (type == 't')
		{
			type = 'q';
			cout<<"Because the window width is one timestamp,"<<endl
				<<"parallel episodes (with repeats allowed) will be used instead serial episodes."<<endl;
		}
	}

	do
	{
		//get the ante & cons constraints from files
		//Get the list of constraints (eventually in DNF form) 
		// currently by the list of events desired
		cout<<"Please make sure the antecendent and consequent constraint files do not have any duplicated events."<<endl;
		cout << "Enter the antecedent constraint file name:\n";
		cin >> ant_constraint_file_name;
		constraint_stream.clear();
		constraint_stream.open(ant_constraint_file_name, ios::in);
		//constraint_stream.open(constraint_file_name, ios::in|ios::nocreate);
		if (constraint_stream.fail( ))
		{
			cout << "Antecedent constraint file opening failed.\n";
			exit(1);
		}

		get_constraints(constraint_stream, A);
		//output the constraints - test validation
		/*for(event_set::const_iterator bi = T.begin(); bi != T.end(); ++bi)
		{
			cout<<*bi;
		}
		cout<<endl;
		*/
		constraint_stream.close();

		//Get the list of constraints (eventually in DNF form) 
	// currently by the list of events desired
		cout << "Enter the consequent constraint file name:\n";
		cin >> cons_constraint_file_name;
		constraint_stream.clear();
		constraint_stream.open(cons_constraint_file_name, ios::in);
		//constraint_stream.open(constraint_file_name, ios::in|ios::nocreate);
		if (constraint_stream.fail( ))
		{
			cout << "Consequent constraint file opening failed.\n";
			exit(1);
		}

		get_constraints(constraint_stream, C);
		//output the constraints - test validation
		/*for(event_set::const_iterator bi = C.begin(); bi != C.end(); ++bi)
		{
			cout<<*bi;
		}
		cout<<endl;
		*/
		constraint_stream.close();

	} while (intersects(A,C));


	switch (type)
	{
	case 'p':
	case 'q':
			//Get the list of events
//			cout << "Enter the event list file name:\n";
//			cin >> event_file_name;
//			event_stream.open(event_file_name, ios::in); //nocreate is not in the new iostream library
		//    event_stream.open(event_file_name, ios::in|ios::nocreate);
//			if (event_stream.fail( ))
//			{
//				cout << "Event file opening failed.\n";
//				exit(1);
//			}

//			get_events(event_stream, F);

			/*	//output the events - test validation
			for(event_set::const_iterator i = F.begin(); i != F.end(); ++i)
			{
				cout<<*i;
			}
			cout<<endl;
			*/
			
//			event_stream.close();
			break;

	case 's':
	case 't':
		cout<<"For Minimal occurrences serial episodes, we need to know the list of possible events"<<endl<<" from each data source separately."<<endl;
		do
		{
			cout<<"How many data sources do you have?";
			cin>>sources;
			if (sources < MINSOURCES || sources > MAXSOURCES)
				cout<<"Invalid number of data sources.  The number of data sources must be between "<<MINSOURCES<<" and "<<MAXSOURCES<<" ,inclusively."<<endl;
		} while (sources < MINSOURCES || sources > MAXSOURCES);

		for (int i = 1; i <= sources;i++)
		{
//Get the list of events
			cout << "Enter the event list file name for data source "<<i<<":\n";
			cin >> event_file_name;
			event_stream.clear();
			event_stream.open(event_file_name, ios::in); //nocreate is not in the new iostream library
			if (event_stream.fail( ))
			{
				cout << "Event file for data source "<<i<<" failed to open.\n";
				exit(1);
			}

			get_events(event_stream, TimeFvec,A,C);
			event_stream.close();
		}
		
//			getsingles(TimeFvec, TimeF, sources);

			/*	//output the events - test validation
			for(const_t_stamp_vec_ITR i = TimeF.begin(); i != TimeF.end(); ++i)
			{
				cout<<*i;
			}
			cout<<endl;
			*/
		break;
	}
			

    cout << "Enter the input file name:\n";
    cin >> in_file_name;
    cout << "Enter the output file name (maximum of 15 characters):\n";
    cin >> out_file_name;

    in_stream.open(in_file_name, ios::in);
    //in_stream.open(in_file_name, ios::in|ios::nocreate);
    if (in_stream.fail( ))
    {
        cout << "Input file opening failed.\n";
        exit(1);
    }

    out_stream.open(out_file_name);
    if (out_stream.fail( ))
    {
        cout << "Output file opening failed.\n";
        exit(1);
    }

	int ans;
	do
	{
		cout<<"What type of rules do you want?  "<<endl
			<<"Enter 1 for all Association Rules, or"<<endl
			<<"2 for Representative Association Rules"<<endl;
		cin>>ans;
		if ( ! (ans == 1 || ans ==2) )
			cout<<"Please enter a 1 or a 2"<<endl;
	} while (! (ans == 1 || ans ==2) );

	char lag_type = 'm';
	do
	{
		cout<<"What type of lag do you want to use?  "<<endl
			<<"Enter m for maximum time lag, or"<<endl
			<<"f for fixed time lag"<<endl;
		cin>>lag_type;
		lag_type = tolower(lag_type);
		if ( ! (lag_type == 'm' || lag_type == 'f') )
			cout<<"Please enter either 'm' or 'f'"<<endl;
	} while (! (lag_type == 'm' || lag_type =='f') );

	
	system("cls");
	out_stream<<"MOWCATL with constraints, ";
	if (lag_type == 'm')
		out_stream<<"MAX";
	else 
		out_stream<<"FIXED";
	out_stream<<" lags and ";
	if (ans == 1)
		out_stream<<"All ";
	else
		out_stream<<"Representative ";
	out_stream<<" Association Rules"<<endl;
	out_stream<<"Parameter file: "<<par_file_name<<endl;
    out_stream<<"Input file: "<<in_file_name<<endl;
	out_stream<<"Output file: "<<out_file_name<<endl;
	if (type == 's' || type == 't')
		out_stream<<"Last event list file: "<<event_file_name<<endl;
	out_stream<<"Antecedant constraint file: "<<ant_constraint_file_name<<endl;
	out_stream<<"Consequent constraint file: "<<cons_constraint_file_name<<endl;
	outputparams(wina, winc, lag, lag_type, cout);
	outputparams(wina, winc, lag, lag_type, out_stream);

	out_stream<<"Antecedent constraints:  ";
	output_constraints(out_stream, A);
	out_stream<<endl<<"Consequent constraints:  ";
	output_constraints(out_stream,C);
	out_stream<<endl;

	cout<<"Antecedent constraints:  ";
	output_constraints(cout, A);
	cout<<endl<<"Consequent constraints:  ";
	output_constraints(cout,C);
	cout<<endl;

	outputtype(type, out_stream,ans);
	outputtype(type, cout,ans);

	int k = 0;
	switch(type)
	{
		case 'p':
		case 'q':
			Generate_Parallel_CTMINEPI(in_stream, min_freq, k, out_stream, wina, winc, A, C, type);
			in_stream.close();
			Generate_MOWCATL_RULES(out_stream, min_confidence, k, lag, min_freq, type, A, C,ans, lag_type);
			//generate the CT MINEPI representative association rules
		//generate the first set of minimal occurrences from the file 
			break;
		case 's':
		case 't':
			Generate_Serial_CTMINEPI(in_stream, min_freq, k, out_stream, wina, winc, A, C, type, sources, TimeFvec);
			in_stream.close();

			Generate_MOWCATL_RULES(out_stream, min_confidence, k, lag, min_freq, type, A, C,ans, lag_type);
			//generate the CT MINEPI representative association rules
			break;
		default:
			cerr<<"Invalid type in main program, exiting."<<endl;
			exit(1);
	};


	out_stream.close();
	return 0;
} //end main

void CmdLineIntFace::get_pars(istream& fin, int& min_freq, double& min_confidence, int& wina, int& lag, int& winc, char& type)
//get the parameters from a file
{
	fin>>min_freq>>min_confidence>>wina>>lag>>winc>>type;
}

void CmdLineIntFace::get_events(istream& fin, event_vec_vec& TimeFvec, const event_set& A, const event_set& C)
//get the list of events from each data source separately
{
	event e;
	event_vec F;

	while (	fin>>e)
	{
		if ( e.matchB(A) || e.matchB(C) )
		{
			F.push_back(e);
		}
	}
	TimeFvec.push_back(F);
}

void CmdLineIntFace::get_events(istream& fin, event_vec& F)
{
	event e;

	while (	fin>>e)
	{
		F.push_back(e);
	}
	sort(F.begin(), F.end());
}

bool CmdLineIntFace::intersects(const event_set& A, const event_set& C)
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

void CmdLineIntFace::get_constraints(istream& fin, event_set& B)
{
	event e;
	while (	fin>>e)
	{
		B.push_back(e);
	}
	sort(B.begin(), B.end());
}

void CmdLineIntFace::outputparams(int wina, int winc, int lag, char lag_type, ostream& out)
{
	out.setf(ios::fixed);
	out.setf(ios::showpoint);
	out.precision(4);
	out<<"Frequent Minimal Occurrence Episodes "<<endl
		<<"Generated with Constraints and ";
	if (lag_type == 'm')
		out<<"MAX";
	else
		out<<"FIXED";
	out<<" Time lags"<<endl;
	out<<"Time stamps per antecedent window = "<<wina<<endl;
	out<<"Time stamps per consequence window = "<<winc<<endl;
	if (lag_type == 'm')
		out<<"MAX";
	else
		out<<"FIXED";
	out<<" Time lag between the start of the antecedent and"<<endl
		<<"the start of the consequence = "<<lag<<endl;
//	out<<"Step size = "<<step<<endl;
	out<<"______________________________________________________________________"<<endl<<endl;
}

void CmdLineIntFace::outputtype(char type, ostream& out,int ans)
{
	if (1== ans)
		out<<"All ";
	else
		out<<"Representative ";
	switch (type)
	{
	case 'p': out<<"parallel association rules with no repeated events";
		break;
	case 'q': out<<"parallel association rules with repeated events";
		break;
	case 's': out<<"serial association rules with no repeated events";
		break;
	case 't': out<<"serial association rules with repeated events";
		break;
	};
	out<<endl<<endl;
}

void CmdLineIntFace::output_constraints(ostream& out, event_set& B)
{
	for (event_set::const_iterator i = B.begin(); i != B.end(); i++)
	{
		out<<*i<<" ";
	}
}

void CmdLineIntFace::outputEpisodeCounts(double FrequencyThreshold, int Candidates, int FrequentEpisodes, int Iterations, unsigned int Time, ostream& outs)
{
	outs<<"Frequency"
		<<setw(15)<<"Candidates"
		<<setw(15)<<"Frequent"
		<<setw(15)<<"Iterations"
		<<setw(15)<<"Freq. Episode"<<endl;
	outs<<"threshold"
		<<setw(15)<<" "
		<<setw(15)<<"Episodes"
		<<setw(15)<<" "
		<<setw(15)<<"gen. time (s)"<<endl;
	outs<<"____________________________________________________________________________"<<endl;
	outs<<FrequencyThreshold
		<<setw(15)<<Candidates
		<<setw(15)<<FrequentEpisodes
		<<setw(15)<<Iterations
		<<setw(15)<<Time<<endl;
	outs<<"____________________________________________________________________________"<<endl<<endl;
}

void CmdLineIntFace::outputRuleCounts(double min_confidence, int RuleCount, unsigned int Time, int Windows, ostream& outs)
{
	outs<<endl;
	outs.setf(ios::fixed);
	outs.precision(4);
	outs.setf(ios::showpoint);
	outs<<"Support"
//		<<setw(15)<<"Total"
		<<setw(17)<<"Distinct"
		<<setw(15)<<"Rule gen."
		<<endl;
	outs<<"threshold"
//		<<setw(15)<<"Windows"
		<<setw(15)<<"Rules"
		<<setw(15)<<"time (s)"<<endl;
	outs<<"_________________________________________________________"<<endl;
	outs<<min_confidence
//		<<setw(15)<<Windows
		<<setw(15)<<RuleCount
		<<setw(15)<<Time<<endl;
	outs<<"_________________________________________________________"<<endl;
}


void CmdLineIntFace::Read_Parallel_Data(ifstream& fin, ostream& outs, const event_set& A, const event_set& C)
//generate the first set of minimal occurrences from the file 
	/*
	This is currently set up to only read a time stamp followed by an event
	from a flat file.
	This would need to be modified for other types of files or file formats
	*/
{	
	moitemset emptyset;
	moitemset::setnum_records(0);
	fca.push_back(emptyset); //the vector must not be empty - or errors occur
	fcc.push_back(emptyset); //the vector must not be empty - or errors occur
	fca[0].get_parallel_episodes(A);
	fcc[0].get_parallel_episodes(C); //get singleton episodes that match the constraints

	int t, previous_t; // previous_t is added by Dan Li to count the number of timestamps
	event E;
	fin>>t;
// initialize previous_t
	previous_t = t;
// initialize record number
	moitemset::incrnum_records(1);

	while (!fin.eof())
	{
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
		fin>>E;
		if (E.matchB(A) )
			fca[0].bringinnew(E, t);
		else if (E.matchB(C) )
			fcc[0].bringinnew(E, t);
		fin>>t;
				
	}  //end while not eof

	//testing
//	fca[0].output(cout);
//	system("pause");
//	fcc[0].output(cout);

}

void CmdLineIntFace::Read_Serial_Data(ifstream& fin, ostream& outs, const event_set& A, const event_set& C)
//generate the first set of minimal occurrences from the file 
	/*
	This is currently set up to only read a time stamp followed by an event
	from a flat file.
	This would need to be modified for other types of files or file formats
	*/
{	
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
		
		fin>>E;
		if (E.matchB(A) )
			fca[0].bringinnew(E, t);
		else if (E.matchB(C) )
			fcc[0].bringinnew(E, t);
		fin>>t;
				
	}  //end while not eof
	//testing
//	fca[0].output(cout);
//	fcc[0].output(cout);
//	system("pause");
}

void CmdLineIntFace::Generate_Parallel_CTMINEPI(ifstream& fin, int min_fr, int& k, ostream& outs, int wina, int winc, const event_set& A, const event_set& C, char type)
{
	int Candidates=0;
	int FrequentEpisodes=0;
	int Iterations=1;

	unsigned int BeginTime = time(0);

	/* Step 1-3 for single events. Read the Data and record the minimal occurrences of the single events and record their support*/
	
	Read_Parallel_Data(fin, outs, A, C);
	Candidates += fca[0].access_size();
	Candidates += fcc[0].access_size();

//	fca[0].output(cout);
//	fcc[0].output(cout);

	/* Step 4. Keep the frequent items */
	moitemset emptyset;
	fa.push_back(emptyset);//the vector must not be empty - or errors occur
	fc.push_back(emptyset);//the vector must not be empty - or errors occur
	fa[0].addfreq(fca[k], garbage, min_fr);
 	fc[0].addfreq(fcc[k], garbage, min_fr);
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
	fcc[1].fast_generate_parallel_candidates(fc[0],type, k);
//		else
//			fcc[k+1].generate_parallel_candidates(fc[k], type, F, B, k);
//			use this when DNF constraints are fixed.

	Candidates += fca[1].access_size();
//		fca[1].output(cout);

	Candidates += fcc[1].access_size();
//		fcc[1].output(cout);

	/* Now do the same while there are more candidates to look through */
	for (k = 1; (!(fca[k].empty()) || !(fcc[k].empty())); k++)
	{
		Iterations++;

		/* Step 2. Record the minimal occurrences */
		if (!fca[k].empty())
			fca[k].Gen_mo(fa[k-1], k, type);
		if (!fcc[k].empty())
			fcc[k].Gen_mo(fc[k-1], k, type);

		/*Step 3. Update support */  
		//this is kept separate from the previous step to allow for a range of windows to work
		// you can iterate and output for a range of window widths
		if (!fca[k].empty())
			fca[k].Update_Support(wina);
		if (!fcc[k].empty())
			fcc[k].Update_Support(winc);

		/* Step 4. Keep the frequent items */

			fa.push_back(emptyset);
			fc.push_back(emptyset);
		if (!fca[k].empty())
		{
			fa[k].addfreq(fca[k], garbage, min_fr);
			garbage.cleanup();
			FrequentEpisodes += fa[k].access_size();
//			fa[k].output(outs);
//			fa[k].output(cout);
		}

		if (!fcc[k].empty())
		{
			fc[k].addfreq(fcc[k], garbage, min_fr);
			garbage.cleanup();
			FrequentEpisodes += fc[k].access_size();
//			fc[k].output(outs);
//			fc[k].output(cout);
		}


				fca.push_back(emptyset); 
				fcc.push_back(emptyset); 
	/* Step 5. Create candidates of size k+1 */
		if (!fca[k].empty())
		{
	//		if (equalBF)
				fca[k+1].fast_generate_parallel_candidates(fa[k],type, k);
	//		else
	//			fcc[k+1].generate_parallel_candidates(fc[k], type, F, B, k);
	//			use this when DNF constraints are fixed.
//			fca[k+1].output(cout);
			Candidates += fca[k+1].access_size();
		}

		if (!fcc[k].empty())
		{
	//		if (equalBF)
				fcc[k+1].fast_generate_parallel_candidates(fc[k],type, k);
	//			fcc[k+1].output(cout);
	//		else
	//			fcc[k+1].generate_parallel_candidates(fc[k], type, F, B, k);
	//			use this when DNF constraints are fixed.


//			fcc[k+1].output(cout);
			Candidates += fcc[k+1].access_size();
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

	outputEpisodeCounts(double(min_fr)/fa[k].access_num_records() , Candidates, FrequentEpisodes, Iterations, Time, outs);
	outputEpisodeCounts(double(min_fr)/fa[k].access_num_records() , Candidates, FrequentEpisodes, Iterations, Time, cout);
	garbage.cleanup();

/*	//testing:
	cout<<"Remaining Garbage"<<endl;
	garbage.output(cout);
	cout<<"End of garbage"<<endl;
	*/

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

void CmdLineIntFace::Generate_Serial_CTMINEPI(ifstream& fin, int min_fr, int& k, ostream& outs, int wina, int winc, const event_set& A, const event_set& C, char type, int sources, const event_vec_vec& TimeFvec)
{
	int Candidates=0;
	int FrequentEpisodes=0;
	int Iterations=1;
	k=0;

	unsigned int BeginTime = time(0);

	/* Step 1-3 for single events. Read the Data and record the minimal occurrences of the single events and record their support*/
	
	Read_Serial_Data(fin, outs, A, C);

	fca[k].addtimestampitems(TimeFvec, A, sources,wina);
	fcc[k].addtimestampitems(TimeFvec, C, sources,winc);


	Candidates += fca[k].access_size();
	Candidates += fcc[k].access_size();

//	fca[k].output(cout);
//	fcc[k].output(cout);
//	system("pause");

	/* Step 4. Keep the frequent items */
	moitemset emptyset;
	fa.push_back(emptyset);//the vector must not be empty - or errors occur
	fc.push_back(emptyset);//the vector must not be empty - or errors occur
	fa[k].addfreq(fca[k], garbage, min_fr);
	fc[k].addfreq(fcc[k], garbage, min_fr);
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
			fa[k].addfreq(fca[k], garbage, min_fr);
			garbage.cleanup();
//			fa[k].output(outs);
//			fa[k].output(cout);
			FrequentEpisodes += fa[k].access_size();
		}

		if (!fcc[k].empty())
		{
			fc[k].addfreq(fcc[k], garbage, min_fr);
			garbage.cleanup();
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

	outputEpisodeCounts(double(min_fr)/fa[k].access_num_records() , Candidates, FrequentEpisodes, Iterations, Time, outs);
	outputEpisodeCounts(double(min_fr)/fa[k].access_num_records() , Candidates, FrequentEpisodes, Iterations, Time, cout);
	garbage.cleanup();  //clean up remaining candidates that we did not use 


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

void CmdLineIntFace::Generate_MOWCATL_RULES(ostream& out, double min_conf, int maxk, int lag, int min_fr, char type, const event_set& T, const event_set& C,int ans, char lag_type)
{//modified 2/19/05 - ignore Representative rules - always generate all rules
	unsigned int BeginTime = time(0);
	bool rar = false;
	if (ans ==2)
		rar=true;

	moitemset empty;
      int i;
	for (i = 0; i <= maxk; i++)
		fac.push_back(empty);
//cout<<"maxk="<<maxk<<endl; 
	//use only the active itemsets and combine them into a set of ruleitems
//system("pause");
	for (i = 0; i <=maxk;i++)
	{
//			cout<<"i="<<i<<endl;
		if (!fa[i].empty())
			fac[i].combine(fa[i], garbage, fc, type,lag, min_fr, lag_type);
//		if (!fac[i].empty() )
//		{
//			fac[i].output(cout);
//			fac[i].output(out);
//			system("pause");
//		}
	}
/*2/19/05 mod - remove this code
	//split the frequent episodes according to size
	fc_type split_fac;
	int largest = moitemset::seeantlargest() * moitemset::seeconslargest()- 
							(moitemset::seeconslargest()-1) ;
	int k=0;
      int h;
	for (h = 0; h < largest; h++)
		split_fac.push_back(empty);

	//use only the active itemsets
	for (h = 0; h <=maxk;h++)
	{
		if (!fac[h].empty())
			fac[h].putinsplit(split_fac, k);
	}
*/

//maxk is now the index for the largest
//0 is the index for 1-items
	if (type == 'p' || type == 'q')
	{
		moruleset REAR;
		for (i = maxk; i>=0;i--)  //mod 2.19 - use maxk instead of k
		{
			//Testing:
//					fac[i].output(cout);
			//		cout<<endl;
			/*2/19/05 mod - remove this code
			if (!split_fac[i].empty())
				if (1==ans)
					split_fac[i].addrules(min_conf, REAR, i, k);
				else
					split_fac[i].loop1parallel(split_fac, min_conf, REAR, i, k, type, T,C);
					*/
			if (!fac[i].empty())
					fac[i].addrules(min_conf, REAR, i);
		}
/*		if (2==ans)
		{
			cout<<"Representative Association Rules represent several rules that have at least the given confidence values"<<endl;
			out<<"Representative Association Rules represent several rules that have at least the given confidence values"<<endl;
		}
*/
//		REAR.output(cout,rar);

		// Dan Li, Oct 29, overwrite output function with one more function "type"
		//REAR.output(out,rar);
		REAR.output(out,type,rar);

		outputRuleCounts(min_conf, REAR.AccessCount(), time(0) - BeginTime, moitemset::seenum_records(), out);
		outputRuleCounts(min_conf, REAR.AccessCount(), time(0) - BeginTime, moitemset::seenum_records(), cout);
 		REAR.cleanup();
	}
	else
	{
		mos_ruleset REAR;
		for (i = maxk; i>=0;i--)//mod2.19 - use maxk instead of k
		{
			//Testing:
			//		fac[i].output(cout);
			//		cout<<endl;
/*			if (!split_fac[i].empty()) //mod 2.19 - remove the Representative rules code
				if (1==ans)
	 				split_fac[i].addrules(min_conf, REAR, i, k);
				else
					split_fac[i].loop1serial(split_fac, min_conf, REAR, i, k, type, T,C);
*/
			if (!fac[i].empty())
	 				fac[i].addrules(min_conf, REAR, i);

		}
/*		if (2==ans)
		{
			cout<<"Representative Association Rules represent several rules that have at least the given confidence values"<<endl;
			out<<"Representative Association Rules represent several rules that have at least the given confidence values"<<endl;
		}
*/
//		REAR.output(cout,rar);
	
		// Dan Li, Oct 29, overwrite output function with one more function "type"
		//REAR.output(out,rar);
		REAR.output(out,type, rar);

		outputRuleCounts(min_conf, REAR.AccessCount(), time(0) - BeginTime, moitemset::seenum_records(), out);
		outputRuleCounts(min_conf, REAR.AccessCount(), time(0) - BeginTime, moitemset::seenum_records(), cout);
			REAR.cleanup();
 }
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
                 fac[g].cleanup();  //this throws an "Access violation" exception sometimes
			}
			garbage.cleanup();  //shouldn't be anything left

}

