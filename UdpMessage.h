#ifndef UDPMESSAGE_H_
#define UDPMESSAGE_H_

#include <vector>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sstream>

using namespace std;

class UdpMessage {
public:
	UdpMessage(char*);
	UdpMessage();
	void addIP(char*);
	bool ipExists(char*);
	void setModifiedMessage(char*, char*);
	char* getMessage();
	char* getModifiedMessage();
private:

	vector<string> IPList;
	char* rawMessage;
	char* modifiedMessage;
};

#endif
