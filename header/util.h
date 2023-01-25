#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>
#include <iostream>
using namespace std;

class Util {

public :
	static vector<string> splitString(string text,string delimiter) {
		size_t pos = 0;
		string token;
		vector<string> results;
		while ((pos = text.find(delimiter)) != string::npos) {
			token = text.substr(0, pos);
			//std::cout << token << std::endl;
			results.push_back(token);
			text.erase(0, pos + delimiter.length());
		}
		if (results.size() == 3)
			results.push_back(text);
		//std::cout << results[0] << results[1] << results[2] <<results[3]<< endl;
		return results;
	}
};

#endif // !UTIL_H
