/* 
Bastian Ruppert
*/

#ifndef __POLLMANAGER_H__
#define __POLLMANAGER_H__

#define POLLMNGMAXSOURCES 2 

typedef struct
{
  int fd;
  void * userDat;
  //  const char * name;
  int (*readFnk)(char * buf,int len,int pMngIndex,void * userDat);
  int (*pollhupFnk)(int pMngIndex,void * userDat); // Gegenseite hat aufgelegt
  int (*writeFnk)(char * buf,int pMngIndex,void * userDat);
  int (*conListenerFnk)(int pMngIndex,void * userDat);
}_pollMngSrc_t;

typedef struct
{
  struct pollfd fdinfo[POLLMNGMAXSOURCES];
  _pollMngSrc_t Srcs[POLLMNGMAXSOURCES];
}_pollMngSrcContainer_t;

int pollMngInit(_pollMngSrcContainer_t * pollMngPollSources,int pollSrcsLen);

int pollMngSetSrc(_pollMngSrc_t * src,int index);

void pollMngSuspendPolling();

int pollMngPoll();

#endif /* __RUPSOCK_H__ */
