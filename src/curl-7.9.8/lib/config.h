/* lib/config.h.  Generated automatically by configure.  */
/* lib/config.h.in.  Generated automatically from configure.in by autoheader.  */
/* Name of this package! */
#define PACKAGE "curl"

/* Version number of this archive. */
#define VERSION "7.9.8"

/* Define if you have the getpass function.  */
/* #undef HAVE_GETPASS */

/* Define cpu-machine-OS */
#define OS "i686-pc-linux-gnu"

/* Define if you have the gethostbyaddr_r() function with 5 arguments */
/* #undef HAVE_GETHOSTBYADDR_R_5 */

/* Define if you have the gethostbyaddr_r() function with 7 arguments */
/* #undef HAVE_GETHOSTBYADDR_R_7 */

/* Define if you have the gethostbyaddr_r() function with 8 arguments */
#define HAVE_GETHOSTBYADDR_R_8 1

/* Define if you have the gethostbyname_r() function with 3 arguments */
/* #undef HAVE_GETHOSTBYNAME_R_3 */

/* Define if you have the gethostbyname_r() function with 5 arguments */
/* #undef HAVE_GETHOSTBYNAME_R_5 */

/* Define if you have the gethostbyname_r() function with 6 arguments */
#define HAVE_GETHOSTBYNAME_R_6 1

/* Define if you have the inet_ntoa_r function declared. */
/* #undef HAVE_INET_NTOA_R_DECL */

/* Define if you need the _REENTRANT define for some functions */
/* #undef NEED_REENTRANT */

/* Define if you have the Kerberos4 libraries (including -ldes) */
/* #undef KRB4 */

/* Define if you want to enable IPv6 support */
/* #undef ENABLE_IPV6 */

/* Define this to 'int' if ssize_t is not an available typedefed type */
/* #undef ssize_t */

/* Define this to 'int' if socklen_t is not an available typedefed type */
/* #undef socklen_t */

/* Define this as a suitable file to read random data from */
#define RANDOM_FILE "/dev/urandom"

/* Define this to your Entropy Gathering Daemon socket pathname */
/* #undef EGD_SOCKET */

/* Define if you have a working OpenSSL installation */
/* #undef OPENSSL_ENABLED */

/* Define the one correct non-blocking socket method below */
/* #undef HAVE_FIONBIO */
/* #undef HAVE_IOCTLSOCKET */
/* #undef HAVE_IOCTLSOCKET_CASE */
#define HAVE_O_NONBLOCK 1
/* #undef HAVE_DISABLED_NONBLOCKING */

/* Define this to 'int' if in_addr_t is not an available typedefed type */
/* #undef in_addr_t */

/* Define to disable DICT */
#define CURL_DISABLE_DICT 1

/* Define to disable FILE */
#define CURL_DISABLE_FILE 1

/* Define to disable FTP */
#define CURL_DISABLE_FTP 1

/* Define to disable GOPHER */
#define CURL_DISABLE_GOPHER 1

/* Define to disable HTTP */
/* #undef CURL_DISABLE_HTTP */

/* Define to disable LDAP */
#define CURL_DISABLE_LDAP 1

/* Define to disable TELNET */
#define CURL_DISABLE_TELNET 1

/* Set to explicitly specify we don't want to use thread-safe functions */
/* #undef DISABLED_THREADSAFE */

/* Define if you want to enable IPv6 support */
/* #undef ENABLE_IPV6 */

/* Define if you have the <alloca.h> header file. */
#define HAVE_ALLOCA_H 1

/* Define if you have the <arpa/inet.h> header file. */
#define HAVE_ARPA_INET_H 1

/* Define if you have the `closesocket' function. */
/* #undef HAVE_CLOSESOCKET */

/* Define if you have the <crypto.h> header file. */
/* #undef HAVE_CRYPTO_H */

/* Define if you have the <des.h> header file. */
/* #undef HAVE_DES_H */

/* Define if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define if you have the `dlopen' function. */
#define HAVE_DLOPEN 1

/* Define if you have the <err.h> header file. */
/* #undef HAVE_ERR_H */

/* Define if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define if getaddrinfo exists and works */
/* #undef HAVE_GETADDRINFO */

/* Define if you have the `geteuid' function. */
#define HAVE_GETEUID 1

/* Define if you have the `gethostbyaddr' function. */
#define HAVE_GETHOSTBYADDR 1

/* Define if you have the `gethostbyaddr_r' function. */
#define HAVE_GETHOSTBYADDR_R 1

/* Define if you have the `gethostbyname_r' function. */
#define HAVE_GETHOSTBYNAME_R 1

/* Define if you have the `gethostname' function. */
#define HAVE_GETHOSTNAME 1

/* Define if you have the `getpass_r' function. */
/* #undef HAVE_GETPASS_R */

/* Define if you have the `getpwuid' function. */
#define HAVE_GETPWUID 1

/* Define if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define if you have the `gmtime_r' function. */
#define HAVE_GMTIME_R 1

/* Define if you have the `inet_addr' function. */
#define HAVE_INET_ADDR 1

/* Define if you have the `inet_ntoa' function. */
#define HAVE_INET_NTOA 1

/* Define if you have the `inet_ntoa_r' function. */
/* #undef HAVE_INET_NTOA_R */

/* Define if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if you have the <io.h> header file. */
/* #undef HAVE_IO_H */

/* Define if you have the `krb_get_our_ip_for_realm' function. */
/* #undef HAVE_KRB_GET_OUR_IP_FOR_REALM */

/* Define if you have the <krb.h> header file. */
/* #undef HAVE_KRB_H */

/* Define if you have the `crypto' library (-lcrypto). */
/* #undef HAVE_LIBCRYPTO */

/* Define if you have the `dl' library (-ldl). */
#define HAVE_LIBDL 1

/* Define if you have the `nsl' library (-lnsl). */
/* #undef HAVE_LIBNSL */

/* Define if you have the `resolv' library (-lresolv). */
/* #undef HAVE_LIBRESOLV */

/* Define if you have the `resolve' library (-lresolve). */
/* #undef HAVE_LIBRESOLVE */

/* Define if you have the `socket' library (-lsocket). */
/* #undef HAVE_LIBSOCKET */

/* Define if you have the `ssl' library (-lssl). */
/* #undef HAVE_LIBSSL */

/* Define if you have the `ucb' library (-lucb). */
/* #undef HAVE_LIBUCB */

/* Define if you have the `localtime_r' function. */
#define HAVE_LOCALTIME_R 1

/* Define if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1

/* Define if you have the <netinet/if_ether.h> header file. */
#define HAVE_NETINET_IF_ETHER_H 1

/* Define if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define if you have the <net/if.h> header file. */
#define HAVE_NET_IF_H 1

/* Define if you have the <openssl/crypto.h> header file. */
/* #undef HAVE_OPENSSL_CRYPTO_H */

/* Define if you have the <openssl/engine.h> header file. */
/* #undef HAVE_OPENSSL_ENGINE_H */

/* Define if you have the <openssl/err.h> header file. */
/* #undef HAVE_OPENSSL_ERR_H */

/* Define if you have the <openssl/pem.h> header file. */
/* #undef HAVE_OPENSSL_PEM_H */

/* Define if you have the <openssl/rsa.h> header file. */
/* #undef HAVE_OPENSSL_RSA_H */

/* Define if you have the <openssl/ssl.h> header file. */
/* #undef HAVE_OPENSSL_SSL_H */

/* Define if you have the <openssl/x509.h> header file. */
/* #undef HAVE_OPENSSL_X509_H */

/* Define if you have the <pem.h> header file. */
/* #undef HAVE_PEM_H */

/* Define if you have the `perror' function. */
#define HAVE_PERROR 1

/* Define if you have the <pwd.h> header file. */
#define HAVE_PWD_H 1

/* Define if you have the `RAND_egd' function. */
/* #undef HAVE_RAND_EGD */

/* Define if you have the `RAND_screen' function. */
/* #undef HAVE_RAND_SCREEN */

/* Define if you have the `RAND_status' function. */
/* #undef HAVE_RAND_STATUS */

/* Define if you have the <rsa.h> header file. */
/* #undef HAVE_RSA_H */

/* Define if you have the `select' function. */
#define HAVE_SELECT 1

/* Define if you have the <setjmp.h> header file. */
#define HAVE_SETJMP_H 1

/* Define if you have the `setvbuf' function. */
#define HAVE_SETVBUF 1

/* Define if you have the <sgtty.h> header file. */
#define HAVE_SGTTY_H 1

/* Define if you have the `sigaction' function. */
#define HAVE_SIGACTION 1

/* Define if you have the `signal' function. */
#define HAVE_SIGNAL 1

/* Define if you have the `sigsetjmp' function. */
#define HAVE_SIGSETJMP 1

/* Define if you have the `socket' function. */
#define HAVE_SOCKET 1

/* Define if you have the <ssl.h> header file. */
/* #undef HAVE_SSL_H */

/* Define if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define if you have the `strcasecmp' function. */
#define HAVE_STRCASECMP 1

/* Define if you have the `strcmpi' function. */
/* #undef HAVE_STRCMPI */

/* Define if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define if you have the `strftime' function. */
#define HAVE_STRFTIME 1

/* Define if you have the `stricmp' function. */
/* #undef HAVE_STRICMP */

/* Define if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define if you have the `strlcat' function. */
/* #undef HAVE_STRLCAT */

/* Define if you have the `strlcpy' function. */
/* #undef HAVE_STRLCPY */

/* Define if you have the `strstr' function. */
#define HAVE_STRSTR 1

/* Define if you have the `strtok_r' function. */
#define HAVE_STRTOK_R 1

/* Define if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define if you have the <sys/sockio.h> header file. */
/* #undef HAVE_SYS_SOCKIO_H */

/* Define if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define if you have the <sys/utime.h> header file. */
/* #undef HAVE_SYS_UTIME_H */

/* Define if you have the `tcgetattr' function. */
#define HAVE_TCGETATTR 1

/* Define if you have the `tcsetattr' function. */
#define HAVE_TCSETATTR 1

/* Define if you have the <termios.h> header file. */
#define HAVE_TERMIOS_H 1

/* Define if you have the <termio.h> header file. */
#define HAVE_TERMIO_H 1

/* Define if you have the <time.h> header file. */
#define HAVE_TIME_H 1

/* Define if you have the `uname' function. */
#define HAVE_UNAME 1

/* Define if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define if you have the `utime' function. */
#define HAVE_UTIME 1

/* Define if you have the <utime.h> header file. */
#define HAVE_UTIME_H 1

/* Define if you have the <winsock.h> header file. */
/* #undef HAVE_WINSOCK_H */

/* Define if you have the <x509.h> header file. */
/* #undef HAVE_X509_H */

/* Name of package */
#define PACKAGE "curl"

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* Define if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Version number of package */
#define VERSION "7.9.8"

/* Define if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
#ifndef _ALL_SOURCE
/* # undef _ALL_SOURCE */
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
#define _FILE_OFFSET_BITS 64

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* type to use in place of in_addr_t if not defined */
/* #undef in_addr_t */

/* Define to `unsigned' if <sys/types.h> does not define. */
/* #undef size_t */

/* type to use in place of socklen_t if not defined */
/* #undef socklen_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef ssize_t */
