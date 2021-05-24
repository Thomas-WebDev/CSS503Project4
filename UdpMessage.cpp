#include "UdpMessage.h"

size_t MAXBUFFER = 1024;

void printMessage(char m[], int n) {
	cout << "the message length is: " << n << endl;
	for (int i = 0; i < n; i++) {
		cout << i << " ";
	}
	cout << endl;
	for (int i = 0; i < n; i++) {
		cout << (int)(unsigned char)m[i] << " ";
	}
	cout << endl;
	for (int i = 0; i < n; i++) {
		cout << char((int)(unsigned char)m[i]) << " ";
	}
	cout << endl;
	cout.flush();
}

string int2string(int i) {
	stringstream out;
	out << i;
	return out.str();
}

void int2byte(unsigned long value, unsigned char* buffer) {
	buffer[0] = value & 0xFF;
	buffer[1] = (value >> 8) & 0xFF;
	buffer[2] = (value >> 16) & 0xFF;
	buffer[3] = (value >> 24) & 0xFF;
}

void StringIPToByteArray(string stringIp, unsigned char bytes[]) {
	struct sockaddr_in sa;
	inet_pton(AF_INET, stringIp.c_str(), &(sa.sin_addr));
	unsigned char ipBytes[4] = { 0 };
	int2byte(sa.sin_addr.s_addr, ipBytes);
}

void ByteArrayToStringIP(string& stringIp, unsigned char bytes[]) {
	for (int i = 0; i < 4; i++) {
		int part = bytes[i];
		stringIp.append(int2string(part));
		stringIp.append(".");
	}
	stringIp = stringIp.substr(0, stringIp.size() - 1);
}

UdpMessage::UdpMessage() {

}

UdpMessage::UdpMessage(char* rMessage) {
	char dummy;
	for (int i = 0; i < 3; i++) {
		dummy = *(rMessage + i);
		if ((int)dummy != (-32 + i)) {
			cerr << "Wrong Message Format" << endl;
		}
	}
	int numOfIpsInTheList = int(*(rMessage + 3));
	char* messageStart = rMessage + 4;
	int messageIndex = 4;
	for (int i = 0; i< numOfIpsInTheList; i++) {
		string nextIp;
		ByteArrayToStringIP(nextIp, (unsigned char*)rMessage + messageIndex);
		IPList.push_back(nextIp);
		messageStart += 4;
		messageIndex += 4;
	}
	delete modifiedMessage;
	modifiedMessage = new char[strlen(messageStart)];
	bzero(modifiedMessage, strlen(messageStart));
	strncpy(modifiedMessage, messageStart, strlen(messageStart));
}

void UdpMessage::setModifiedMessage(char* sMessage, char* groupIp) {
	bzero(modifiedMessage, strlen(sMessage));
	strncpy(modifiedMessage, sMessage, strlen(sMessage));
	IPList.push_back(groupIp);
}

void UdpMessage::addIP(char* ip) {
	IPList.push_back(ip);
}

char* UdpMessage::getModifiedMessage() {
	return modifiedMessage;
}

char* UdpMessage::getMessage() {
	char* message = new char[MAXBUFFER];
	bzero(message, MAXBUFFER);
	*message = -32;
	*(message + 1) = -31;
	*(message + 2) = -30;
	*(message + 3) = IPList.size();
	*(message + 4) = '\0';

	int messagePointer = 0;
	for (int i = 0; i< IPList.size(); i++) {
		unsigned char* nextIp = new unsigned char[4];
		bzero(nextIp, 4);
		messagePointer += 4;
		struct sockaddr_in sa;
		inet_pton(AF_INET, IPList[i].c_str(), &(sa.sin_addr));
		int2byte(sa.sin_addr.s_addr, nextIp);

		bcopy((void*)nextIp, (void*)(message + messagePointer), 4);
	}

	messagePointer += 4;
	messagePointer += 1;
	message[messagePointer] = '\0';
	strcat(message, modifiedMessage);
	return message;
}

bool UdpMessage::ipExists(char* myIp) {
	for (int i = 0; i< IPList.size(); i++) {
		if (strcmp(myIp, IPList[i].c_str()) == 0) {
			return true;
		}
	}
	return false;
}

