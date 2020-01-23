#include <errno.h>        
#include <net/if.h>       
#include <netdb.h>        
#include <netinet/in.h>   
#include <stdio.h>        
#include <stdlib.h>       
#include <string.h>       
#include <sys/socket.h>   
#include <unistd.h>       

#define DFLT_HOST      "localhost"   
#define DFLT_SCOPE_ID  "eth0"        
#define DFLT_SERVICE   "daytime"     
#define INVALID_DESC   -1            
#define MAXBFRSIZE     256           
#define VALIDOPTS      "s:v"         

typedef enum { false = 0, true } boolean;
typedef struct sockaddr_in       sockaddr_in_t;
typedef struct sockaddr_in6      sockaddr_in6_t;

static int  openSckt( const char   *host,
                      const char   *service,
                      unsigned int  scopeId );
static void tod( int sckt );

static const char *pgmName;           
static boolean     verbose = false;   

#define USAGE                                                            
        {                                                                
           fprintf( stderr,                                              
                    "Usage: %s [-v] [-s scope_id] [host [service]]\n",   
                    pgmName );                                           
           exit( 127 );                                                  
        }  

static __inline boolean SYSCALL( const char *syscallName,
                                 int         lineNbr,
                                 int         status )
{
   if ( ( status == -1 ) && verbose )
   {
      fprintf( stderr,
               "%s (line %d): System call failed ('%s') - %s.\n",
               pgmName,
               lineNbr,
               syscallName,
               strerror( errno ) );
   }
   return status != -1;   
}  
int main( int   argc,
          char *argv[ ] )
{
   const char   *host     = DFLT_HOST;
   int           opt;
   int           sckt;
   unsigned int  scopeId  = if_nametoindex( DFLT_SCOPE_ID );
   const char   *service  = DFLT_SERVICE;
   
   pgmName = (const char*) strrchr( argv[ 0 ], '/' );
   pgmName = pgmName == NULL  ?  argv[ 0 ]  :  pgmName+1;
   
   opterr = 0;   
   while ( ( opt = getopt( argc, argv, VALIDOPTS ) ) != -1 )
   {
      switch ( opt )
      {
         case 's':   
         {
            scopeId = if_nametoindex( optarg );
            if ( scopeId == 0 )
            {
               fprintf( stderr,
                        "%s: Unknown network interface (%s).\n",
                        pgmName,
                        optarg );
               USAGE;
            }
            break;
         }
         case 'v':   
         {
            verbose = true;
            break;
         }
         default:
         {
            USAGE;
         }
      }  
   } 
   
   switch ( argc - optind )
   {
      case 2:   
      {
          service = argv[ optind + 1 ];
          
      }
      case 1:   
      {
          host = argv[ optind ];
          
      }
      case 0:   
      {
          break;
      }
      default:
      {
         USAGE;
      }
   }  
   
   if ( ( sckt = openSckt( host,
                           service,
                           scopeId ) ) == INVALID_DESC )
   {
      fprintf( stderr,
               "%s: Sorry... a connectionless socket could "
               "not be set up.\n",
               pgmName );
      exit( 1 );
   }
   
   tod( sckt );
   
   (void) SYSCALL( "close",
                   __LINE__,
                   close( sckt ) );
   return 0;
}  
static int openSckt( const char   *host,
                     const char   *service,
                     unsigned int  scopeId )
{
   struct addrinfo *ai;
   int              aiErr;
   struct addrinfo *aiHead;
   struct addrinfo  hints;
   sockaddr_in6_t  *pSadrIn6;
   int              sckt;
   memset( &hints, 0, sizeof( hints ) );
   hints.ai_family   = PF_UNSPEC;     
   hints.ai_socktype = SOCK_DGRAM;    
   hints.ai_protocol = IPPROTO_UDP;   
   
   if ( ( aiErr = getaddrinfo( host,
                               service,
                               &hints,
                               &aiHead ) ) != 0 )
   {
      fprintf( stderr,
               "%s (line %d): ERROR - %s.\n",
               pgmName,
               __LINE__,
               gai_strerror( aiErr ) );
      return INVALID_DESC;
   }
   
   for ( ai = aiHead,   sckt = INVALID_DESC;
         ( ai != NULL ) && ( sckt == INVALID_DESC );
         ai = ai->ai_next )
   {
      
      if ( ai->ai_family == PF_INET6 )
      {
         pSadrIn6 = (sockaddr_in6_t*) ai->ai_addr;
         if ( pSadrIn6->sin6_scope_id == 0 )
         {
            pSadrIn6->sin6_scope_id = scopeId;
         }  
      }  
      
      if ( verbose )
      {
         
         char hostBfr[ NI_MAXHOST ];
         char servBfr[ NI_MAXSERV ];
         
         fprintf( stderr,
                  "Address info:\n"
                  "   ai_flags     = 0x%02X\n"
                  "   ai_family    = %d (PF_INET = %d, PF_INET6 = %d)\n"
                  "   ai_socktype  = %d (SOCK_STREAM = %d, SOCK_DGRAM = %d)\n"
                  "   ai_protocol  = %d (IPPROTO_TCP = %d, IPPROTO_UDP = %d)\n"
                  "   ai_addrlen   = %d (sockaddr_in = %d, "
                  "sockaddr_in6 = %d)\n",
                  ai->ai_flags,
                  ai->ai_family,
                  PF_INET,
                  PF_INET6,
                  ai->ai_socktype,
                  SOCK_STREAM,
                  SOCK_DGRAM,
                  ai->ai_protocol,
                  IPPROTO_TCP,
                  IPPROTO_UDP,
                  ai->ai_addrlen,
                  sizeof( struct sockaddr_in ),
                  sizeof( struct sockaddr_in6 ) );
         
         getnameinfo( ai->ai_addr,
                      ai->ai_addrlen,
                      hostBfr,
                      sizeof( hostBfr ),
                      servBfr,
                      sizeof( servBfr ),
                      NI_NUMERICHOST | NI_NUMERICSERV );
         switch ( ai->ai_family )
         {
            case PF_INET:   
            {
               sockaddr_in_t *pSadrIn = (sockaddr_in_t*) ai->ai_addr;
               fprintf( stderr,
                        "   ai_addr      = sin_family: %d (AF_INET = %d, "
                        "AF_INET6 = %d)\n"
                        "                  sin_addr:   %s\n"
                        "                  sin_port:   %s\n",
                        pSadrIn->sin_family,
                        AF_INET,
                        AF_INET6,
                        hostBfr,
                        servBfr );
               break;
            }  
            case PF_INET6:   
            {
               pSadrIn6 = (sockaddr_in6_t*) ai->ai_addr;
               fprintf( stderr,
                        "   ai_addr      = sin6_family:   %d (AF_INET = %d, "
                        "AF_INET6 = %d)\n"
                        "                  sin6_addr:     %s\n"
                        "                  sin6_port:     %s\n"
                        "                  sin6_flowinfo: %d\n"
                        "                  sin6_scope_id: %d\n",
                        pSadrIn6->sin6_family,
                        AF_INET,
                        AF_INET6,
                        hostBfr,
                        servBfr,
                        pSadrIn6->sin6_flowinfo,
                        pSadrIn6->sin6_scope_id );
               break;
            }  
            default:   
            {
               fprintf( stderr,
                        "%s (line %d): ERROR - Unknown protocol family (%d).\n",
                        pgmName,
                        __LINE__,
                        ai->ai_family );
               break;
            }  
         }  
      }  
      
      if ( !SYSCALL( "socket",
                     __LINE__,
                     sckt = socket( ai->ai_family,
                                    ai->ai_socktype,
                                    ai->ai_protocol ) ) )
      {
         sckt = INVALID_DESC;
         continue;   
      }
      
      if ( !SYSCALL( "connect",
                     __LINE__,
                     connect( sckt,
                              ai->ai_addr,
                              ai->ai_addrlen ) ) )
      {
         (void) close( sckt );   
         sckt = INVALID_DESC;
         continue;   
      }
   }  
   
   freeaddrinfo( aiHead );
   return sckt;
}  
static void tod( int sckt )
{
   char bfr[ MAXBFRSIZE+1 ];
   int  inBytes;
   if ( !SYSCALL( "write",
                  __LINE__,
                  write( sckt, "Are you there?", 14 ) ) )
   {
      return;
   }
   
   if ( !SYSCALL( "read",
                  __LINE__,
                  inBytes = read( sckt,
                                  bfr,
                                  MAXBFRSIZE ) ) )
   {
      return;
   }
   bfr[ inBytes ] = '\0';   
   fputs( bfr, stdout );    
   fflush( stdout );
}  