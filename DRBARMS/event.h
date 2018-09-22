/*
filename: event.h
author: Sherri Harms
last modified: 7/16/01
description: Class event interface file for episodes.
  	Values of this type are a string.
******************************************************************************
Known bugs:
  The constraints are all disjuctive singular events.
  step 5 from alg 5 generate candidates has not been implemented (constraints with conjunctions)
  Time lag is not completed.
  input validation is not done
*******************************************************************************
Assumptions:
See the documentation.cpp file for system documentation.
*******************************************************************************
Local Data Structures:
Local variables: string e
*****************************************************************************
event operations include:
input, output, constructors, 
relational operators, accessor functions, get length, and matchB
*******************************************************************************/
#ifndef EVENT_H
#define EVENT_H

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;

class event 
{
public:
	typedef vector<event> event_set;
 //   friend bool operator ==(const event& e1, const event& e2);
    //Returns true if e1 and e2 represent the same event; 
    //otherwise, returns false.
/*
    friend bool operator <(const event& e1, const event& e2);
    //Returns true if e1 < e2 ; otherwise, returns false.
    friend bool operator >(const event& e1, const event& e2);
    //Returns true if e1 > e2 ; otherwise, returns false.
    friend bool operator <=(const event& e1, const event& e2);
    //Returns true if e1 <= e2 ; otherwise, returns false.
    friend bool operator >=(const event& e1, const event& e2);
    //Returns true if e1 >= e2 ; otherwise, returns false.
    friend bool operator !=(const event& e1, const event& e2);
    //Returns true if e1 != e2 ; otherwise, returns false.
*/
friend bool operator ==(const event& e1, const event& e2)
{ return (e1.e == e2.e); }

friend bool operator <(const event& e1, const event& e2)
//Returns true if e1 < e2 ; otherwise, returns false.
{ return (e1.e < e2.e); }

friend bool operator >(const event& e1, const event& e2)
//Returns true if e1 > e2 ; otherwise, returns false.
{ return (e1.e > e2.e); }

friend bool operator <=(const event& e1, const event& e2)
//Returns true if e1 <= e2 ; otherwise, returns false.
{ return (e1.e <= e2.e); }

friend bool operator >=(const event& e1, const event& e2)
//Returns true if e1 >= e2 ; otherwise, returns false.
{ return (e1.e >= e2.e); }

friend bool operator !=(const event& e1, const event& e2)
//Returns true if e1 != e2 ; otherwise, returns false.
{ return (e1.e != e2.e); }

friend istream& operator >>(istream& ins, event& the_object)
//Overloads the >> operator for input values of type event.
//Precondition: If ins is a file input stream, then ins has 
//already been connected to a file. 
{
	ins>>the_object.e;
	return ins;
}

friend ostream& operator << (ostream& outs, const event& the_object)
//Overloads the << operator for output values of type event.
//Precondition: If outs is a file output stream, then outs as 
//already been connected to a file.
{
	outs<<the_object.e;
	return outs;
}



    event( ) {e = "";};
    //Initializes the event member variables to 0.

	void clear();

	event(string in_event) {e = in_event;};

	event(const event& e1);
	const event& operator = (const event& e1);

	string getEvent() const;
	//returns the e value

	int get_length() const;
//	~event();


  /*  friend istream& operator >>(istream& ins, event& the_object);
    //Overloads the >> operator for input values of type event.
    //Precondition: If ins is a file input stream, then ins has 
    //already been connected to a file. 

    friend ostream& operator << (ostream& outs, 
                                                 const event& the_object);
    //Overloads the << operator for output values of type event.
    //Precondition: If outs is a file output stream, then outs as 
    //already been connected to a file.
*/
	bool matchB(const event_set& B) const;
  //true if the event is in the constraint list

private:
	string e;
};

#endif //EVENT_H



