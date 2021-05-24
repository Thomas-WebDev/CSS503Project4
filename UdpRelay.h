#ifndef UDPRELAY_H_
#define UDPRELAY_H_

#include <string>
#include "UdpMulticast.h"
#include <pthread.h>
#include "Socket.h"
#include <vector>
#include <algorithm>
#include "UdpMessage.h"
#include <map>

class UdpRelay {
public:
	UdpRelay(char*);
	void ParseInput(char*, char*, int*);
private:
	string getIP(const string&, const char*);
	vector<string> string_split(const string &, const char*, bool);
	void IPandPortInput(char*, char*, int&);
};

#endif