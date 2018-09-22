#include "conversions.h"

string convertitoa(int a) {
	/*const char digitMap[] = "0123456789";
	string result;
	for(int i=30; a > 30 && i > 0; i--)
	{
		result = digitMap[a % 10] + result;
		a /= 10;
	}
	return result;*/
	
	char convertedInt[30];
	sprintf(convertedInt,"%d",a);
	return convertedInt;
	
}

string convertfcvt(double a) {
	/*int temp = int(a * 100);
	string result;
	result = convertitoa(temp);
	result = result.substr(0, result.length() - 2) + "." + result.substr(result.length() - 2, 2);
	return result;*/
		
	char convertedFloat[30];
	sprintf(convertedFloat,"%f",a);
	return convertedFloat;		
}
