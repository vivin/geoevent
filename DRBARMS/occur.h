/*
filename: occur.h
author: Sherri Harms
last modified: 7/20/01
description: Class occur interface file for episodes.
  	Values of this type are a start time and and end time (both integers).
******************************************************************************
Known bugs:
  The constraints are all disjuctive singular occurs.
  input validation is not done
*******************************************************************************
Assumptions:
See the documentation.cpp file for system documentation.
*******************************************************************************
Local Data Structures:
Local variables: int start, int stop
*****************************************************************************
occur operations include:
input, output, constructors, 
relational operators, accessor functions,
*******************************************************************************/
#ifndef occur_H
#define occur_H

#include <iostream>
#include <fstream>
#include <list>
using namespace std;

class occur 
{
public:
	typedef list<occur> occur_list;

friend bool operator ==(const occur& o1, const occur& o2)
{ return (o1.start == o2.start && o1.stop == o2.stop); }

friend bool operator <(const occur& o1, const occur& o2)
//Returns true if o1 < o2 ; otherwise, returns false.
{ return (o1.start < o2.start || o1.start == o2.start && o1.stop < o2.stop); }

friend bool operator >(const occur& o1, const occur& o2)
//Returns true if o1 > o2 ; otherwise, returns false.
{ return (o1.start > o2.start || o1.start == o2.start && o1.stop > o2.stop); }

friend bool operator <=(const occur& o1, const occur& o2)
//Returns true if o1 <= o2 ; otherwise, returns false.
{ return (o1 == o2 || o1 < o2); }

friend bool operator >=(const occur& o1, const occur& o2)
//Returns true if o1 >= o2 ; otherwise, returns false.
{ return (o1 == o2 || o1 > o2); }

friend bool operator !=(const occur& o1, const occur& o2)
//Returns true if o1 != o2 ; otherwise, returns false.
{ return (o1.start != o2.start && o1.stop != o2.stop); }

friend istream& operator >>(istream& ins, occur& the_object)
//Overloads the >> operator for input values of type occur.
//Precondition: If ins is a file input stream, then ins has 
//already been connected to a file. 
{
	ins>>the_object.start>>the_object.stop;
	return ins;
}

friend ostream& operator << (ostream& outs, const occur& the_object)
//Overloads the << operator for output values of type occur.
//Precondition: If outs is a file output stream, then outs as 
//already been connected to a file.
{
	outs<<the_object.start<<" "<<the_object.stop;
	return outs;
}



    occur( ) {start=0;stop=0;};
    //Initializes the occur member variables to 0.

	occur(int in_start, int in_stop) {start = in_start; stop = in_stop;};

	occur(const occur& o1);
	const occur& operator = (const occur& o1);

	int get_start() const;

	int get_stop() const;

//	~occur();

private:
	int start;
	int stop;
};

#endif //occur_H



