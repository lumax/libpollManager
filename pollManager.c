/*
Bastian Ruppert
*/

#include <poll.h>
#include <defs.h>
#include "pollManager.h"

static int PollManagerPollTrue = 0;
static int PollManagerSingleton = 0;
static int PollMngSrcsLen=0;

static _pollMngSrcContainer_t * pollMngSrcCont = 0;

/**
 * \brief fügt Daten der #_pollMngSrc_t dem fdinfo Array hinzu
 *
 * Initialisiert das fd Array mit den Daten aller PollSrcs. Das fd Array 
 * wird von poll benötigt.
 */
int pollMngSetSrc(_pollMngSrc_t * src,int index)
{
  if(index<0||index>=PollMngSrcsLen)
    {
      errno = EINVAL;
      return -1;
    }
  if(0==src)
    {
      errno = EINVAL;
      return -1;
    }
  
  memcpy((void *)&pollMngSrcCont->Srcs[index],\
	 (void *)src,\
	 sizeof(_pollMngSrc_t));
  //pollMngSrcCont->fdinfo[index] resetten!
  if(pollMngSrcCont->Srcs[index].readFnk|| \
     pollMngSrcCont->Srcs[index].conListenerFnk)
    {
      pollMngSrcCont->fdinfo[index].events = POLLIN | POLLPRI;
    }
  if(pollMngSrcCont->Srcs[index].writeFnk)
    {
      pollMngSrcCont->fdinfo[index].events = POLLOUT | POLLWRNORM;
    }
  pollMngSrcCont->fdinfo[index].fd=pollMngSrcCont->Srcs[index].fd;
  return 0;
}


/* \brief Initialisiert den PollManager!
 * \param pollMngPollSources Der Parameter pollMngPollSources muss vom Programmierer zur Verfügung gestellt werden.
 *        
 *        Bsp: 
 *            _pollMngSrc_t  pollMngPollSources[]={
 *             [0] = {
 *                    .pollfd_t.fd = -1,
 *                    .name = "myAmazingFileDescriptor",
 *                    .readFnk = 0,
 *                    .writeFnk = 0,
 *                   },
 *            };
 *        pollMngPollSources{ 
 * \return 0 on success, -1 on failure and errno is set to EBUSY or EINVAL
 */
int pollMngInit(_pollMngSrcContainer_t * thePollMngPollSources,int pollSrcsLen)
{
  int i = 0;
    //  printf("sizeof(fdinfo)/sizeof(..) : %i \n",sizeof(fdinfo)/sizeof(struct pollfd));
  
  if((sizeof(pollMngSrcCont->fdinfo)/sizeof(struct pollfd))<pollSrcsLen)
    {
      errno = EINVAL;
      return -1;
    }
  if(0==thePollMngPollSources)
    {
      errno = EINVAL;
      return -1;
    }
  pollMngSrcCont = thePollMngPollSources;
  PollMngSrcsLen = pollSrcsLen;
  if(PollManagerSingleton)
    {
      errno = EBUSY;
      return -1;
    }
  
  if(PollManagerSingleton)
    {
      errno = EBUSY;
      return -1;
    }
  else
    {
      for(i=0;i<PollMngSrcsLen;i++)
	{
	  if(pollMngSetSrc(&pollMngSrcCont->Srcs[i],i)!=0)
	    {
	      return -1;
	    }
	}
      PollManagerPollTrue = 1;
      PollManagerSingleton = 1;
    }
  return 0;
}



/**
 * \brief führt zum return der Funktion #pollMngPoll nach return von poll()
 *
 */
void pollMngSuspendPolling()
{
  PollManagerPollTrue=0;
  PollManagerSingleton=0;
}

/**
 * \brief poll Fnk des pollManangers
 * \param timeout timeout in ms, negativ = kein timeout
 * \return 0 on timeout , 1 after pollMngSuspendPolling and -1 on error
 *
 * Führt den poll Systemcall mit den fd's aus der pollManager Struktur aus.
 * Wenn der pollManager-Struktur Funktionen für Lese und Schreib-events 
 * hinzugefügt worden sind,werden diese aufgerufen.
 */
int pollMngPoll(int timeout)
{
  int i = 0;
  int numEvents;
  int len = PollMngSrcsLen;
  char buf[256];
  char * writebuf;
  int tmp;
  if(!PollManagerSingleton)
    {
      errno = EACCES;
      return -1;
    }
  while(PollManagerPollTrue)
    {
      ec_neg1( numEvents = poll(pollMngSrcCont->fdinfo,len,-1) )
	/*	for(i=0;i < PollMngSrcsLen ; i++)
	  {
	    	printf("numEvts:%i index:%i,fd:%i,events:[0x%x],revents:[0x%x] \n",
	       numEvents,\
	       i,\
	       pollMngSrcCont->fdinfo[i].fd,\
	       pollMngSrcCont->fdinfo[i].events,\
	       pollMngSrcCont->fdinfo[i].revents);
	  }*/
	if(0==numEvents)         //timeout occured
	  {
	    return 0;
	  }
	for(i=0;i < PollMngSrcsLen ; i++)
	  {
	    if(pollMngSrcCont->fdinfo[i].revents & POLLHUP && \
	       pollMngSrcCont->Srcs[i].pollhupFnk) 
	      {
		ec_neg1( pollMngSrcCont->Srcs[i].pollhupFnk(i,\
							    pollMngSrcCont->Srcs[i].userDat))
		  break;
	      }
	    if(pollMngSrcCont->fdinfo[i].revents & POLLNVAL) 
	      {
		errno = EINVAL;
		EC_FAIL;
	      }
	    if(pollMngSrcCont->fdinfo[i].revents & POLLERR) 
	      {
		errno = EIO;
		EC_FAIL;
	      }	
	    if(pollMngSrcCont->fdinfo[i].revents & POLLRDHUP) 
	      {
		errno = ENOTCONN;
		EC_FAIL;
	      }
	    if(pollMngSrcCont->fdinfo[i].revents & (POLLIN | POLLPRI) )
	      {
		if(pollMngSrcCont->Srcs[i].conListenerFnk)
		  {
		    ec_neg1( pollMngSrcCont->Srcs[i].conListenerFnk(i, \
									   pollMngSrcCont->Srcs[i].userDat))
		      break;
		      }
		if(pollMngSrcCont->Srcs[i].readFnk)
		  {
		    /*printf("Srcs[i].readFnk(i:%i, fd:%i\n",i,	\
		      pollMngSrcCont->fdinfo[i].fd     );*/
		    ec_neg1( tmp = read(pollMngSrcCont->fdinfo[i].fd, buf, 256))
		      //if(tmp)
		      //  {
		      ec_neg1( pollMngSrcCont->Srcs[i].readFnk(buf,	\
							       tmp,	\
							       i,	\
							       pollMngSrcCont->Srcs[i].userDat	\
							       ))
		      // }
		      //continue;
		      //break;
		      }
	      }
	    if(pollMngSrcCont->fdinfo[i].revents & (POLLOUT | POLLWRNORM) 
	       && pollMngSrcCont->Srcs[i].writeFnk)
	      {                             //daten zum senden holen
		tmp = pollMngSrcCont->Srcs[i].writeFnk(writebuf,i,\
							    pollMngSrcCont->Srcs[i].userDat);
		ec_neg1( write(pollMngSrcCont->fdinfo[i].fd,writebuf,tmp) )
		  //break;
	      }
	  }//end for
    }//end while
  return 1;
EC_CLEANUP_BGN
  return -1;
EC_CLEANUP_END
}
