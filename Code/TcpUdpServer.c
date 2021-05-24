#include <stdio.h>      // print, fprintf, perror
#include <string.h>     // strlen, strncmp, bzero
#include <netdb.h>      // gethostbyname
#include <arpa/inet.h>  // inet_ntoa
#include <netinet/in.h> // inet_ntoa
#include <stdlib.h>     // exit
#include <sys/socket.h> // socket, connect, inet_ntoa
#include <sys/types.h>  // socket, connect
#include <unistd.h>     // close

#define TRUE 1

void udp( int serverSd ) {
  while ( TRUE ) {
    struct sockaddr_in srcAddr;
    socklen_t addrlen = sizeof( srcAddr );
    bzero( ( char *)&srcAddr, sizeof( srcAddr ) );

    char buf[256];
    int nRead = recvfrom( serverSd, buf, 256, 0, (struct sockaddr*)&srcAddr, 
			  &addrlen );
    buf[nRead] = 0;
    printf( "received from (%s:%d) %s\n", inet_ntoa( srcAddr.sin_addr ),
	    ntohs( srcAddr.sin_port ), buf );
  }
  close( serverSd );
}

void tcp( int serverSd ) {
  while ( TRUE ) {
    int newSd = 0;
    struct sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof( newSockAddr );

    listen( serverSd, 5 );
    if ( ( newSd = accept( serverSd, (struct sockaddr*)&newSockAddr, 
			   &newSockAddrSize ) ) < 0 ) {
      perror( "Cannot accept from another host." );
      exit( -1 );
    }
    char buf[256];
    int nRead = 0;
    int ret;
    for ( ret = 0; ( ret = read( newSd, buf+nRead, 256-nRead ) ) > 0; 
	  nRead += ret );
    buf[nRead] = 0;
    printf( "accepted (%s:%d) %s\n", inet_ntoa( newSockAddr.sin_addr ),
	    ntohs( newSockAddr.sin_port ), buf );
    close( newSd );
  }
}

int main( int arc, char *argv[] ) {
  if ( arc == 3 ) {
    int isUdp = ( strncmp( argv[1], "udp", 3 ) == 0 );
    int isTcp = ( strncmp( argv[1], "tcp", 3 ) == 0 );
    int port = atoi( argv[2] );

    if ( ( isUdp == TRUE || isTcp == TRUE ) && port > 0 ) {
      
      struct sockaddr_in acceptSockAddr;
      bzero( ( char* )&acceptSockAddr, sizeof( acceptSockAddr ) );
      acceptSockAddr.sin_family      = AF_INET;
      acceptSockAddr.sin_addr.s_addr = htonl( INADDR_ANY );
      acceptSockAddr.sin_port        = htons( port );

      int serverSd = socket( AF_INET, ( isUdp ) ? SOCK_DGRAM : SOCK_STREAM, 
			     0 );
      if( bind( serverSd, (struct sockaddr*)&acceptSockAddr,
		sizeof( acceptSockAddr ) ) < 0 ) {
	perror( "Cannot bind the local address to the server socket." );
	return -1;
      }

      if ( isUdp == TRUE ) {
	udp( serverSd );
      }
      else { // isTcp
	tcp( serverSd );
      }
    }
  }
  fprintf( stderr, "Usage: server udp|tcp port\n" );
  return -1;
}
