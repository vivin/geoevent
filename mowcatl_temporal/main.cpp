#pragma warning(disable:4786)
#include <string>
#include <iostream>
#include "XMLIntFace.h"
#include "StringHelper.h"
#include "cmdLineIntFace.h"
#include "Timer.h"
#include "Mowcatl.h"

using namespace std;

int main(int argc, char* argv[]){

/*	if ( argc < 3 )
	{
		printf("Usage: <input file> <output file>\n");
		exit(0);
	}

	string infile = argv[1];
	string outfile = argv[2];
	*/


	Mowcatl mo;
	ifstream in;
	ofstream out;
	string infile = "../data/xml_gen/output/ships_mocatl_temporal_input.xml";
	string outfile ="../data/mowcatl-temporal/ships_output.xml";
	string paramXML ="";
	string resultXML, thestr;
	
	in.open(infile.c_str(), ios::in);
	if (in.fail( ))
    {
		cout<<"Opening input file failed"<<endl;
        return 0;
    }
	
	out.open(outfile.c_str());
	if(out.fail()){
		cout<<"Opening output file failed"<<endl;
		return 0;
	}

	while (!in.eof())
	{
		in>>thestr;	
		paramXML += thestr;
	}
	
	resultXML = mo.getMowcatlResults(paramXML);

	if(resultXML.length() > 0){
		cout<<"success"<<endl;
		out<<resultXML<<endl;		
		out.close();		
	}

	return 0;
}
