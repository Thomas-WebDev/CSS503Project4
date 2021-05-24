#include "UdpRelay.h"

typedef map<string, pair <pthread_t, int> >::iterator mIterator;
map<string, pair<pthread_t, int> > ip_list;
int port;
char group[16];

// hardcoded data
size_t MAXBUFFERSIZE = 1024;
pthread_t relay_out_thread_id;

int uniquePort = 27526;
Socket tcp_conn(uniquePort);

bool online = true;
struct relayParam {
	int clientSd;
	UdpMulticast udp;
};

void *acceptThread(void *);
void *relayInThread(void *);
void *relayOutThread(void *);
void *commandThread(void *);

void UdpRelay::IPandPortInput(char* input, char* group, int& port) {
	char* parts = strtok(input, ":");
	strcpy(group, parts);
	parts = strtok(NULL, ":");
	port = atoi(parts);
}

UdpRelay::UdpRelay(char* input) {
	IPandPortInput(input, group, port);
	UdpMulticast udp(group, uniquePort);
	if (port < 5001) {
		cerr << "usage: UdpRelay group:port" << endl;
		return;
	}
	cout << "UdpRelay: booted up at " << group << ":" << port << endl;

	pthread_t commandThread_t;
	pthread_create(&commandThread_t, NULL, commandThread, (void*)&udp);

	pthread_t acceptThread_t;
	pthread_create(&acceptThread_t, NULL, acceptThread, (void*)&udp);


	pthread_t relayInThread_t;
	pthread_create(&relayInThread_t, NULL, relayInThread, (void*) new relayParam{ 0,udp });

	pthread_join(commandThread_t, NULL);
	pthread_cancel(relayInThread_t);
	pthread_cancel(acceptThread_t);
}

vector<string> string_split(const string &source, const char* delimiter = " ", bool keep_empty = false) {
	vector<string> results;
	size_t prev = 0;
	size_t next = 0;

	while ((next = source.find_first_of(delimiter, prev)) != std::string::npos) {
		if (keep_empty || (next - prev != 0)) {
			results.push_back(source.substr(prev, next - prev));
		}
		prev = next + 1;
	}
	if (prev < source.size()) {
		results.push_back(source.substr(prev));
	}
	return results;
}


string getIP(const string &source, const char* delimiter = ":") {
	struct hostent *he;
	struct in_addr **addr_list;
	he = gethostbyname(source.substr(0, source.find(delimiter)).c_str());
	if (he == NULL) {
		herror("gethostbyName");
	}
	else
	{
		addr_list = (struct in_addr **) he->h_addr_list;
		return inet_ntoa(*addr_list[0]);
	}
	return NULL;
}

void add_connection(string hostname, UdpMulticast udp) {
	string ip = getIP(hostname);
	char ip_new[MAXBUFFERSIZE];
	strcpy(ip_new, ip.c_str());
	int client_sd = tcp_conn.getClientSocket(ip_new);
	cout << "UdpRelay: registered " << hostname << endl;
	pthread_create(&relay_out_thread_id, NULL, relayOutThread, new relayParam{ client_sd, udp });
	pair<pthread_t, int> sd_and_tid = make_pair(relay_out_thread_id, client_sd);
	ip_list.insert(pair<string, pair <pthread_t, int> >(ip, sd_and_tid));

	cout << "UdpRelay: added " << hostname << ":" << client_sd << endl;
}

void delete_connection(string ip) {
	mIterator it = ip_list.find(ip);
	if (it != ip_list.end()) {
		pthread_cancel(it->second.first);
		ip_list.erase(ip);
		cout << "UdpRelay: deleted " << ip << endl;
	}
	else {
		cout << ip << " was not found" << endl;
	}
}

void DeleteConnectionIfExists(string ip) {
	if (ip_list.find(ip) != ip_list.end()) {
		delete_connection(ip);
	}
}

void display_help() {
	cout << "\nUdbRelay.commandThread: accepts...\n"
		<< "\tadd remoteIp:remoteTcpPort\n"
		<< "\tdelete remoteIp\n"
		<< "\tshow\n"
		<< "\thelp\n"
		<< "\tquit\n" << endl;
}

void display_ips() {
	cout << "Available IPs:" << endl;
	for (mIterator ip = ip_list.begin(); ip != ip_list.end(); ip++) {
		cout << "\t" << ip->first << endl;
	}
}

void *commandThread(void *arg) {
	string cmd = "";
	string ip = "";
	vector<string> results;
	while (online) {
		getline(cin, cmd);
		results = string_split(cmd);
		cmd = results[0];
		if (cmd.compare("add") == 0) {
			char ip_new[MAXBUFFERSIZE];
			strcpy(ip_new, results[1].c_str());
			DeleteConnectionIfExists(getIP(ip_new));
			add_connection(results[1], *(UdpMulticast*)arg);
		}
		else if (cmd.compare("delete") == 0) {
			char ip_new[MAXBUFFERSIZE];
			strcpy(ip_new, results[1].c_str());
			delete_connection(getIP(ip_new));
		}
		else if (cmd.compare("show") == 0) { display_ips(); }
		else if (cmd.compare("help") == 0) { display_help(); }
		else if (cmd.compare("quit") == 0) { online = false; }
		else { cout << "unrecognized command." << endl; }
	}
	return NULL;
}


void *acceptThread(void *arg) {
	int socketDesc;
	while (online) {
		socketDesc = tcp_conn.getServerSocket();
		struct sockaddr_in clientAddr;
		socklen_t addr_size = sizeof(struct sockaddr_in);
		int res = getpeername(socketDesc, (struct sockaddr*) &clientAddr, &addr_size);
		char clientIP[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
		DeleteConnectionIfExists(clientIP);
		pthread_t relayOutThread_t;
		pthread_create(&relayOutThread_t, NULL, relayOutThread, (void*) new relayParam{ socketDesc, *(UdpMulticast*)arg });
		pair<pthread_t, int> sd_and_tid = make_pair(relay_out_thread_id, socketDesc);
		ip_list.insert(pair<string, pair <pthread_t, int> >(clientIP, sd_and_tid));
		cout << "UdpRelay: registered " << clientIP << endl;
	}
}


void *relayInThread(void *udpParam) {
	char *buff = new char[MAXBUFFERSIZE];
	bzero(buff, MAXBUFFERSIZE);

	UdpMulticast udp = ((relayParam*)udpParam)->udp;
	udp.getServerSocket();
	while (online) {
		udp.recv(buff, MAXBUFFERSIZE);
		UdpMessage message(buff);
		if (!message.ipExists(group)) {
			message.addIP(group);
			for (mIterator ip = ip_list.begin(); ip != ip_list.end(); ip++) {
				write(ip->second.second, message.getMessage(), MAXBUFFERSIZE);
				cout << "UdpRelay: relay " << message.getModifiedMessage() << " to remoteGroup[" << ip->first << ":" << uniquePort << "]" << endl;
			}
		}
	}
}


void *relayOutThread(void *relayParamArg) {
	int fd = ((relayParam*)relayParamArg)->clientSd;
	if (fd < 0) {
		cerr << "no server socket was created" << endl;
		pthread_cancel(NULL);
	}

	UdpMulticast udp = ((relayParam*)relayParamArg)->udp;
	udp.getClientSocket();
	char *buf = new char[MAXBUFFERSIZE];
	bzero(buf, MAXBUFFERSIZE);
	while (online) {
		while (read(fd, buf, MAXBUFFERSIZE) <= 0 && online);
		if (!online) {
			break;
		}
		string ipValue = "";
		for (mIterator ip = ip_list.begin(); ip != ip_list.end(); ip++) {
			if (ip->second.second == fd) {
				ipValue = ip->first;
			}
		}
		UdpMessage message(buf);
		cout << "UdpRelay: recieved" << strlen(buf) << " from " << ipValue << " msg = " << message.getModifiedMessage() << endl;
		if (!message.ipExists(group)) {
			message.addIP(group);
			udp.multicast(message.getMessage());
			cout << "UdpRelay: broadcast buf[" << strlen(message.getMessage()) << " bytes] to " << group << ":" << uniquePort << endl;
		}
	}
	return NULL;
}


