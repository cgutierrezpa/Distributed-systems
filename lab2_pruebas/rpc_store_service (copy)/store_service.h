/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _STORE_SERVICE_H_RPCGEN
#define _STORE_SERVICE_H_RPCGEN

#include <rpc/rpc.h>

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SIZE 256
#define MAX_MD5 33

struct response {
	char *msg;
	char *md5;
};
typedef struct response response;


struct store_1_argument {
	char *sender;
	char *receiver;
	u_int msg_id;
	char *msg;
	char *md5;
};
typedef struct store_1_argument store_1_argument;

struct getmessage_1_argument {
	char *user;
	u_int msg_id;
	response *res;
};
typedef struct getmessage_1_argument getmessage_1_argument;

#define STORE_SERVICE 666
#define STORE_VERSION 1

#if defined(__STDC__) || defined(__cplusplus)
#define init 1
extern  enum clnt_stat init_1(void *, CLIENT *);
extern  bool_t init_1_svc(void *, struct svc_req *);
#define store 2
extern  enum clnt_stat store_1(char *, char *, u_int , char *, char *, int *, CLIENT *);
extern  bool_t store_1_svc(char *, char *, u_int , char *, char *, int *, struct svc_req *);
#define getNumMessages 3
extern  enum clnt_stat getnummessages_1(char *, int *, CLIENT *);
extern  bool_t getnummessages_1_svc(char *, int *, struct svc_req *);
#define getMessage 4
extern  enum clnt_stat getmessage_1(char *, u_int , response *, int *, CLIENT *);
extern  bool_t getmessage_1_svc(char *, u_int , response *, int *, struct svc_req *);
extern int store_service_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define init 1
extern  enum clnt_stat init_1();
extern  bool_t init_1_svc();
#define store 2
extern  enum clnt_stat store_1();
extern  bool_t store_1_svc();
#define getNumMessages 3
extern  enum clnt_stat getnummessages_1();
extern  bool_t getnummessages_1_svc();
#define getMessage 4
extern  enum clnt_stat getmessage_1();
extern  bool_t getmessage_1_svc();
extern int store_service_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_response (XDR *, response*);
extern  bool_t xdr_response (XDR *, response*);
extern  bool_t xdr_store_1_argument (XDR *, store_1_argument*);
extern  bool_t xdr_getmessage_1_argument (XDR *, getmessage_1_argument*);

#else /* K&R C */
extern bool_t xdr_response ();
extern bool_t xdr_response ();
extern bool_t xdr_store_1_argument ();
extern bool_t xdr_getmessage_1_argument ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_STORE_SERVICE_H_RPCGEN */
