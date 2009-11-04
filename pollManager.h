/* 
Bastian Ruppert
*/

#ifndef __POLLMANAGER_H__
#define __POLLMANAGER_H__

typedef struct
{
  int fd;
  //  const char * name;
  int (*readFnk)(char * buf,int len,int pMngIndex,void * dat);
  int (*pollhupFnk)(int pMngIndex); // Gegenseite hat aufgelegt
  int (*writeFnk)(char * buf,int pMngIndex);  
}_pollMngSrc_t;

int pollMngInit(_pollMngSrc_t * pollMngPollSources,int pollSrcsLen);

void pollMngSuspendPolling();

int pollMngPoll(_pollMngSrc_t * pollMngPollSources,int pollSrcsLen);

#endif /* __RUPSOCK_H__ */
