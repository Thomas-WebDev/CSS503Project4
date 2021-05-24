// Program 4:  Udp Multicast Relay
// Stephen Gomez-Fox and Kevin Thomas Kehoe
// CSS 503 Spring 2018



#include "UdpRelay.h"


typedef map<string, pair <pthread_t, int> >::iterator mIterator;

// hardcoded data and constants
size_t MAXBUFFERSIZE = 1024;
pthread_t relay_out_thread_id;
int uniquePort = 27526;
Socket tcp_conn(uniquePort);


// program control
bool online = true;
map<string, pair<pthread_t, int> > ip_list;  // added hosts
int port;                                    // multicast port
char group[MAXIPSTR];                        // multicast group
char ifaddr[MAXIPSTR];                       // string ip address to add to relay messages
char myhostname[MAXHOSTNAME];                // my hostname
map<string, string> hostnames;               // ip to hostname resolution
vector<in_addr_t> my_ip_addrs;               // interface ip addresses 
in_addr_t my_ip_addr;                        // ip address of selected interface
struct relayParam {
   int clientSd;
   UdpMulticast udp;
};


// converts a string to a numeric ip address
in_addr_t strToIp(char saddr[]) {
   in_addr_t addr;
   int result = inet_pton(AF_INET, saddr, &addr);
   return (result = 0 ? 0 : addr);
}


// converts a cstring ip address to numeric 
bool ipToStr(in_addr_t addr, char str[]) {
   bzero(str, MAXIPSTR);
   const char* result = inet_ntop(AF_INET, &addr, str, MAXIPSTR);
   return(result == NULL ? false : true);
}


// tries to determine the host name from the ip address.  Returns the hostname if found,
// or the ip address if not.  Updates the hostnames map as needed.
string getHostName(string ip) {
   char ipcstr[MAXIPSTR];   // input string to cstring
   bzero(ipcstr, MAXIPSTR);
   ip.copy(ipcstr, MAXIPSTR, 0);
   char *name = new char[MAXHOSTNAME];  // buffer for hostname
   bzero(name, MAXHOSTNAME);

   struct in_addr ipaddr;
   struct hostent *he;

   // attempt to get host name info from the given address
   try {
      // get host info
      if (!inet_aton(ipcstr, &ipaddr) || (he = gethostbyaddr(&ipaddr, sizeof(ipcstr), AF_INET)) == NULL) {
         ip.copy(name, MAXIPSTR, 0);
      }
      else if (strlen(he->h_name) > 0) {
         // hostname was found
         strncpy(name, he->h_name, sizeof(name));
         if (name[strlen(name) - 1] = '.')    //remove trailing dot, if present
            name[strlen(name) - 1] = '\0';
      }
      else {
         ip.copy(name, MAXIPSTR, 0);
      }
   }
   catch (exception e) {
      ip.copy(name, MAXIPSTR, 0);
   }
   string hostname = name;
   delete name;
   // save it in the hostnames map
   if (hostnames.count(ip) == 0) {
      hostnames[ip] = hostname;
   }
   // overwrite an ip-only mapping if present
   else {
      if (hostnames[ip] == ip) {
         hostnames[ip] = hostname;
      }
   }
   return hostname;
}


// prints an ip address
void printIpAddr(in_addr_t host) {
   char buf[MAXIPSTR];
   ipToStr(host, buf);
   cout << buf << endl;

}

// prints the list of ip addresses in a message
void printIpList(char* buf) {
   char str[MAXIPSTR];
   int hops = buf[HOPS];
   char* hostptr = buf + ADDRSIZE;
   char* msgptr = hostptr + hops * ADDRSIZE;
   for (; hostptr < msgptr; hostptr += ADDRSIZE) {
      in_addr_t* host = (in_addr_t*)hostptr;
      printIpAddr(*host);
   }
}


// checks the header of a message to make sure it is for this application
bool isRelayMessage(char buf[]) {
   return (buf[0] == -32 && buf[1] == -31 && buf[2] == -30);
}

// checks to see if an ip address is already present in a message
bool bufContainsAddr(char* buf, string addr)
{
   int hops = buf[HOPS];
   char* hostptr = buf + ADDRSIZE;
   char* msgptr = hostptr + hops * ADDRSIZE;

   in_addr_t ip = strToIp((char*)addr.c_str());
   for (; hostptr < msgptr; hostptr += ADDRSIZE) {

      in_addr_t host = *(in_addr_t*)hostptr;
      if (ip = host) return true;
      return true;
   }
   return false;
}

// checks to see if any of the machine ip addresses are present in a message
// requires my_ip_addrs tp be set i[ first by getMyAddrList
bool bufContainsMyAddr(char* buf) {

   int hops = buf[HOPS];
   char* hostptr = buf + ADDRSIZE;
   char* msgptr = hostptr + hops * ADDRSIZE;
   // loop through all the ip addresses in the message
   for (; hostptr < msgptr; hostptr += ADDRSIZE) {
      // get an ip address from the message
      in_addr_t host = *(in_addr_t*)hostptr;

      // compare to each of my ip addresses
      for (int i = 0; i < my_ip_addrs.size(); i++) {
         if (host == my_ip_addrs[i])
            return true;
      }
   }
   return false;
}

// looks at all the interfaces and saves all network facing ips into a vector
int getMyAddrList() {
   // vector of host data
   struct ifaddrs *addrList;


   int count = 0;  // number of IPv4 addresses found
   // get the interface addresses
   if (getifaddrs(&addrList) < 0)
      return 0;
   struct ifaddrs *ifaddr = addrList;
   // loop through the addresses to pick out the ones facing the network
   while (ifaddr) {
      if (ifaddr->ifa_addr->sa_family == AF_INET) {
         in_addr_t addr = ((struct sockaddr_in*)ifaddr->ifa_addr)->sin_addr.s_addr;
         // add all interface ip addresses except for loopback and automatic private addresses
         if (addr > 0 && addr != LOOPBACK && ((unsigned long)addr % 65536) != HOSTAUTO) {
            my_ip_addrs.push_back(addr);
            count++;
            // set my_ip_addr to be the first non-loopback found
            if (my_ip_addr == 0)
               my_ip_addr = addr;
         }
      }
      ifaddr = ifaddr->ifa_next; // go to next interface
   }
   return count;
}


// forward declaration of thread functions
void *acceptThread(void *);
void *relayInThread(void *);
void *relayOutThread(void *);
void *commandThread(void *);

// Gets Host and Port from a string in host:port format
bool strToHostAndPort(string &host, in_port_t &port, char hostAndPort[])
{
   char hp[MAXHOSTNAME];
   bzero(hp, MAXHOSTNAME);
   strncpy(hp, hostAndPort, MAXHOSTNAME - 1);  // own copy with guarantee of null terminator
   char *pos = strchr(hp, ':');
   if (pos == NULL) {
      host.assign(hp);
      port = uniquePort;
   }
   else {
      *pos = 0;
      // copy host and move pointer for port
      host.assign(hp);
      pos++;

      // get port
      int portint = atoi(pos);
      if (portint <= 0 || portint > 65535) {
         cerr << "Invalid port" << endl;
         return false;  // bad port
      }
      port = (in_port_t)portint;

   }
   return true;
}


// tokenizes a string
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


// performs a host lookup and returns the ip as a string
// updates the hostnames list as needed
string getIP(const string &source) {
   // copy source string to local cstring
   char host[MAXHOSTNAME];
   bzero(host, MAXHOSTNAME);
   strncpy(host, source.c_str(), MAXHOSTNAME - 1);
   //truncate at delimiter if found
   char* pos = strchr(host, ':');
   if (pos != NULL) {
      *pos = 0;
   }

   // do the host lookup
   struct hostent *he;
   struct in_addr **addr_list;
   he = gethostbyname(host);
   if (he == NULL) {
      herror("gethostbyName");
      return "0.0.0.0";
   }

   // convert the ip address to string format
   addr_list = (struct in_addr **) he->h_addr_list;
   string ip = inet_ntoa(*addr_list[0]);

   // add an entry to the hostnames map if not present
   if (hostnames.count(ip) == 0) {
      hostnames[ip] = ip;
   }

   // update the host list with the a name if it's only an ip 
   ;
   // save the hostname if reverse name returned a hostname
   if (hostnames[ip] == ip) {
      string name = getHostName(ip);
      hostnames[ip] = name;
   }
   // save the hostname if reverse name lookup returned 
   if (hostnames[ip] == ip && source != ip) {
      hostnames[ip] = source;
   }
   return ip;
}


// starts a relay out thread for an added host
void add_connection(string &ip, UdpMulticast &udp) {
   int client_sd;
   // open the socket
   if ((client_sd = tcp_conn.getClientSocket((char*)ip.c_str())) <= 0) {
      cerr << "Could not connect to host" << endl;
      return;
   }
   // display confirmation with hostname/ip
   string name = hostnames[ip];
   cout << "UdpRelay: registered " << name;
   if (name.compare(ip) == 0)
      cout << " [" << ip << "]";
   cout << endl;

   // create the thread
   pthread_create(&relay_out_thread_id, NULL, relayOutThread, new relayParam{ client_sd, udp });

   // update thread list and added host list
   pair<pthread_t, int> sd_and_tid = make_pair(relay_out_thread_id, client_sd);
   ip_list.insert(pair<string, pair <pthread_t, int> >(ip, sd_and_tid));
   cout << "UdpRelay: added " << name << ":" << client_sd << endl;
}


// delete an existing connection, canceling its thread
void delete_connection(string ip) {
   mIterator it = ip_list.find(ip);
   if (it != ip_list.end()) {
      // remove the host and cancel the thread
      ip_list.erase(ip);
      // cancel the thread
      pthread_cancel(it->second.first);
      cout << "UdpRelay: deleted " << hostnames[ip] << endl;
   }
}


// delete an existing connection if it exists
void DeleteConnectionIfExists(string ip) {
   if (ip_list.find(ip) != ip_list.end()) {
      delete_connection(ip);
   }
}


// help display
void display_help() {
   cout << "\nUdbRelay.commandThread: accepts...\n"
      << "\tadd remoteIp:remoteTcpPort\n"
      << "\tdelete remoteIp\n"
      << "\tshow\n"
      << "\thelp\n"
      << "\tquit\n" << endl;
}


// display the list of hosts that have been registered
void display_ips() {
   cout << "Added hosts:" << endl;

   // iterate through the list
   for (mIterator ip = ip_list.begin(); ip != ip_list.end(); ip++) {
      // add ip as hostname if none present
      if (hostnames.count(ip->first) == 0)
         hostnames[ip->first] = ip->first;
      // get the hostname
      string hn = hostnames[ip->first];
      // show hostname and ip if available
      if (hn.compare(ip->first) != 0) {
         cout << hostnames[ip->first] << " [" << ip->first << "]" << endl;
      }
      // otherwise just show the ip
      else {
         cout << "\t" << ip->first;
      }
   }
}


// Process the add command
bool addCommand(vector<string> parts, void* arg) {
   string errmsg = "";
   if (parts.size() != 2) {
      cerr << "Invalid command.  Use:  add hostname:port" << endl;
      return false;
   }
   // copy user input 
   char *hostandport = new char[MAXHOSTNAME];
   bzero(hostandport, MAXHOSTNAME);
   strncpy(hostandport, parts[1].c_str(), MAXHOSTNAME - 1);

   // get the host and port from user input
   string host;
   in_port_t port;
   bool result = strToHostAndPort(host, port, hostandport);

   // convert the Ip format and add host to list
   string ip_new = getIP(host);
   if (ip_new == "0.0.0.0") {  // lookup failed
      cerr << "Host not found";
      return false;
   }
   // delete existing connection if it exists
   DeleteConnectionIfExists(ip_new);

   // add the new connection
   add_connection(ip_new, *(UdpMulticast*)arg);
   return true;
}


// command thread contains the user command interface
void *commandThread(void *arg) {
   string cmd = "";
   string ip = "";
   vector<string> results;
   while (online) {
      cout << "% ";
      cmd = "";
      while (online && cmd.length() == 0) {
         getline(cin, cmd);
      }
      results = string_split(cmd);
      cmd = results[0];
      if (cmd.compare("add") == 0) { addCommand(results, arg); }
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


// accept new connections
void *acceptThread(void *arg) {
   int sd;
   while (online) {
      // get the server socket and determine the ip address that connected
      sd = tcp_conn.getServerSocket();
      struct sockaddr_in clientAddr;
      socklen_t addr_size = sizeof(struct sockaddr_in);
      int res = getpeername(sd, (struct sockaddr*) &clientAddr, &addr_size);

      // get the ip as a character array/string
      char clientIP[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);

      // try to determine the host name
      string name = getHostName(clientIP);
      hostnames[clientIP] = name;
      DeleteConnectionIfExists(clientIP);

      // create the relayOutThread and print a notification
      pthread_t relayOutThread_id;
      pthread_create(&relayOutThread_id, NULL, relayOutThread, (void*) new relayParam{ sd, *(UdpMulticast*)arg });
      pair<pthread_t, int> sd_and_tid = make_pair(relay_out_thread_id, sd);
      ip_list.insert(pair<string, pair <pthread_t, int> >(clientIP, sd_and_tid));
      cout << "UdpRelay: registered " << name << endl;
   }
}


// listens for multicast messages and sends to other another relays
void *relayInThread(void *udpParam) {
   char *buff = new char[MAXBUFFERSIZE];
   bzero(buff, MAXBUFFERSIZE);

   // 
   UdpMulticast udp = ((relayParam*)udpParam)->udp;
   udp.getServerSocket();

   // repeat until quit command issued
   while (online) {
      // wait for multicast
      udp.recv(buff, MAXBUFFERSIZE);
      // check for valid format and not previously processed 
      if (!isRelayMessage(buff) || bufContainsMyAddr(buff))
         continue;

      // create a message object
      UdpMessage message(buff);
      // add this relay's ip address
      message.addIP(ifaddr);

      // send to each added relay
      for (mIterator ip = ip_list.begin(); ip != ip_list.end(); ip++) {
         // only send to relays that haven't previously seen it
         if (!message.ipExists(ip->first)) {
            // send the message and print a confirmation
            write(ip->second.second, message.getMessage(), MAXBUFFERSIZE);
            cout << "UdpRelay: relay " << message.getContents() << " to remoteGroup["
               << hostnames[ip->first] << ":" << uniquePort << "]" << endl;
         }
      }   // end for send to added relays
   }   // repeat until relay quit command
   pthread_exit(NULL);
}

// thread responsible for listening to other relays and 
// multicasting to this relay's group ip and port
void *relayOutThread(void *relayParamArg) {
   // get the socket descriptor for this listener from params
   int fd = ((relayParam*)relayParamArg)->clientSd;
   if (fd < 0) {
      cerr << "no server socket was created" << endl;
      pthread_exit(NULL);
   }

   // get the upd multicast object and create a socket
   UdpMulticast udp = ((relayParam*)relayParamArg)->udp;
   udp.getClientSocket();

   // buffer to receive messages
   char *buf = new char[MAXBUFFERSIZE];
   bzero(buf, MAXBUFFERSIZE);
   //char* name;
   string ipValue = "";
   for (mIterator ip = ip_list.begin(); ip != ip_list.end(); ip++) {
      if (ip->second.second == fd) {
         ipValue = ip->first;
      }
   }
   // add this ip to hostnames if necessary
   if (hostnames.count(ipValue) == 0)
      hostnames[ipValue] = ipValue;

   string hostname = hostnames[ipValue];
   int len = 0;   // length of received relay message
   while (online) {
      while ((len = read(fd, buf, MAXBUFFERSIZE)) <= 0);
      if (online && len > 0 && !bufContainsMyAddr(buf)) {
         // process incoming message
         UdpMessage message(buf);
         // only proceed if my address is not already in the header

         //  CHANGE TO MY FUNCTION
         //if (!message.ipExists(ifaddr) ) {
         cout << "UdpRelay: received " << message.size() << " bytes from "
            << hostnames[ipValue] << " msg = " << message.getContents() << endl;
         message.addIP(ifaddr);
         udp.multicast(message.getMessage());
         cout << "UdpRelay: broadcast buf[" << message.size() << " bytes] to " << group << ":" << port << endl;
      }
   }
   return NULL;
}


// Default constructor
UdpRelay::UdpRelay() {
   cerr << "usage: UdpRelay group:port" << endl;
   return;
}

// Constructor
UdpRelay::UdpRelay(char* input) {

   IPandPortInput(input, group, port);
   UdpMulticast udp(group, port);
   if (port < 5001) {
      cerr << "usage: UdpRelay group:port" << endl;
      return;
   }

   // get the ip address list for this machine
   getMyAddrList();
   // saves a copy of this machines for places where string is needed
   bool result = ipToStr(my_ip_addr, ifaddr);

   char myname[MAXHOSTNAME];
   bzero(myname, MAXHOSTNAME);
   gethostname(myname, MAXHOSTNAME);

   if (strcmp(myname, ifaddr) == 0) {
      cout << "Connect to this relay at " << ifaddr << ":" << uniquePort << endl;
   }
   else
   {
      cout << "Connect to this relay at " << myname << ":" << uniquePort
         << " or " << ifaddr << ":" << uniquePort << endl;
   }
   cout << "UdpRelay: booted up at " << group << ":" << port << endl;

   // Start the program threads
   pthread_t commandThread_id, relayInThread_id, acceptThread_id;
   pthread_create(&commandThread_id, NULL, commandThread, (void*)&udp);
   pthread_create(&acceptThread_id, NULL, acceptThread, (void*)&udp);
   pthread_create(&relayInThread_id, NULL, relayInThread, (void*) new relayParam{ 0,udp });


   // wait for quit
   pthread_join(commandThread_id, NULL);

   // cancel each relayOut thread
   for (mIterator ip = ip_list.begin(); ip != ip_list.end(); ip++)
      delete_connection(ip->first);

   // cancel remaining threads
   pthread_cancel(relayInThread_id);
   pthread_cancel(acceptThread_id);

   // program exit
   cout << "UdpRelay closed" << endl;
   exit(0);
}


// Gets Host and Port from a string in host:port format
void UdpRelay::IPandPortInput(char* input, char* ip, int& port) {
   char* parts = strtok(input, ":");
   strcpy(ip, parts);
   parts = strtok(NULL, ":");
   port = atoi(parts);
}








// UdpMessage Class Methods

string UdpMessage::int2string(int i) {
   stringstream out;
   out << i;
   return out.str();
}


string UdpMessage::addrToStringIp(in_addr_t addr) {
   char str[16];
   bzero(str, 16);
   inet_ntop(AF_INET, &addr, str, 16);
   string stringIp = str;
   return stringIp;
}


in_addr_t UdpMessage::stringIPToAddr(string stringIp) {
   struct sockaddr_in sa;
   char buf[4];
   inet_pton(AF_INET, stringIp.c_str(), buf);
   in_addr_t addr = (in_addr_t)*buf;
   cout << "String ip converted from " << stringIp << " to " << addr << "[" << addrToStringIp(addr) << ']' << endl;
}

// used to create a new udp message
UdpMessage::UdpMessage() {
   contents = NULL;
   messageSize = 0;
}

// used when recieving a message through TCP
UdpMessage::UdpMessage(char* rMessage) {
   char dummy;
   // check first 3 bytes
   for (int i = 0; i < 3; i++) {
      dummy = *(rMessage + i);
      if ((int)dummy != (-32 + i)) {
         cerr << "Wrong Message Format" << endl;
         exit(-1);
      }
   }
   rawMessage = rMessage;
   contents = NULL;
   message = NULL;
   // parse number of ips in list
   int numOfIpsInTheList = int(*(rMessage + 3));
   // parse list of ips
   char* messageStart = rMessage + 4;
   int messageIndex = 4;
   for (int i = 0; i < numOfIpsInTheList; i++) {
      in_addr_t addr = *(in_addr_t*)(rMessage + messageIndex);
      string nextIp = addrToStringIp(addr);
      IPList.push_back(nextIp);
      messageIndex += 4;
   }
   messageSize = MAXBUFFER - messageIndex;
   contents = new char[messageSize];
   bzero(contents, messageSize);
   memcpy(contents, (char*)(rMessage + messageIndex), messageSize - 1);
}

// add local ip to list
void UdpMessage::addIP(char* ip) {
   IPList.push_back(ip);
}

// return content of message
char* UdpMessage::getContents() {
   return contents;
}

// get the char* unmodified message to
char* UdpMessage::getMessage() {
   char* message = new char[MAXBUFFER];
   bzero(message, MAXBUFFER);
   message[0] = -32;
   message[1] = -31;
   message[2] = -30;
   message[3] = IPList.size();

   int messagePointer = 4;
   for (int i = 0; i < IPList.size(); i++) {
      in_addr_t addr;
      inet_pton(AF_INET, IPList[i].c_str(), &addr);
      for (int i = 0; i < 4; i++) {
         unsigned char byte = (unsigned long)addr % 256;
         addr = (unsigned long)addr / 256;
         message[messagePointer + i] = byte;
      }
      messagePointer += 4;
   }
   memcpy((char*)(message + messagePointer), contents, MAXBUFFER - messagePointer);
   return message;
}

// check if local ip exists in message
bool UdpMessage::ipExists(char* myIp) {
   for (int i = 0; i < IPList.size(); i++) {
      if (strcmp(myIp, IPList[i].c_str()) == 0)
         return true;
   }
   return false;
}

// check if local ip exists in message
bool UdpMessage::ipExists(string myIp) {
   for (int i = 0; i < IPList.size(); i++) {
      if (myIp == IPList[i])
         return true;
   }
   return false;
}

UdpMessage::~UdpMessage() {
   if (contents != NULL) delete contents;
   if (message != NULL) delete message;
}

int UdpMessage::size() {
   return 4 + 4 * (IPList.size()) + strlen(contents);
}

