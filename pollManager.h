/* 
Bastian Ruppert
*/

#ifndef __POLLMANAGER_H__
#define __POLLMANAGER_H__

#define POLLMNGMAXSOURCES 2 

typedef struct
{
  int fd;
  //  const char * name;
  int (*readFnk)(char * buf,int len,int pMngIndex,void * dat);
  int (*pollhupFnk)(int pMngIndex); // Gegenseite hat aufgelegt
  int (*writeFnk)(char * buf,int pMngIndex);  
}_pollMngSrc_t;

typedef struct
{
  struct pollfd fdinfo[POLLMNGMAXSOURCES];
  _pollMngSrc_t Srcs[POLLMNGMAXSOURCES];
}_pollMngSrcContainer_t;

int pollMngInit(_pollMngSrcContainer_t * pollMngPollSources,int pollSrcsLen);

void pollMngSuspendPolling();

int pollMngPoll();

#endif /* __RUPSOCK_H__ */
