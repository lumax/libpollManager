/*
Bastian Ruppert
*/

#include <poll.h>
#include <defs.h>
#include "pollManager.h"

static int PollManagerPollTrue = 0;
static int PollManagerSingleton = 0;

static struct pollfd fdinfo[2];

/* \brief Initialisiert den PollManager!
 * \param pollMngPollSources Der Parameter pollMngPollSources muss vom 
 *        Programmierer zur Verfügung gestellt werden.
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
int pollMngInit(_pollMngSrc_t * pollMngPollSources,int pollSrcsLen)
{
  int i = 0;
  int len = pollSrcsLen;
  //  printf("sizeof(fdinfo)/sizeof(..) : %i \n",sizeof(fdinfo)/sizeof(struct pollfd));
  if(PollManagerSingleton)
    {
      errno = EBUSY;
      return -1;
    }
  else if((sizeof(fdinfo)/sizeof(struct pollfd))<len)
    {
      errno = EINVAL;
      return -1;
    }
  else
    {
      for(i=0;i<len;i++)
	{
	  if(pollMngPollSources[i].readFnk)
	    {
	      fdinfo[i].events = POLLIN | POLLPRI;
	    }
	  if(pollMngPollSources[i].writeFnk)
	    {
	      fdinfo[i].events = POLLOUT | POLLWRNORM;
	    }
	  fdinfo[i].fd=pollMngPollSources[i].fd;
	}
      PollManagerPollTrue = 1;
      PollManagerSingleton = 1;
    }
  return 0;
}

void pollMngSuspendPolling()
{
  PollManagerPollTrue=0;
  PollManagerSingleton=0;
}

int pollMngPoll(_pollMngSrc_t  * pollMngPollSources,int pollSrcsLen)
{
  int i = 0;
  int numEvents;
  int len = pollSrcsLen;
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
      ec_neg1( numEvents = poll(fdinfo,len,-1) )
	/*printf("numEvts :%i,fd:%i,events:[0x%x],revents:[0x%x] \n",
	       numEvents,\
	       fdinfo[0].fd,\
	       fdinfo[0].events,\
	       fdinfo[0].revents);*/
	for(i=0;i< len ; i++)
	  {
	    if(fdinfo[i].revents & POLLHUP && pollMngPollSources[i].pollhupFnk) 
	      {
		ec_neg1( pollMngPollSources[i].pollhupFnk(i))
		  break;
	      }
	    if(fdinfo[i].revents & POLLNVAL) 
	      {
		errno = EINVAL;
		EC_FAIL;
	      }
	    if(fdinfo[i].revents & POLLERR) 
	      {
		errno = EIO;
		EC_FAIL;
	      }	
	    if(fdinfo[i].revents & POLLRDHUP) 
	      {
		errno = ENOTCONN;
		EC_FAIL;
	      }
	    if(fdinfo[i].revents & (POLLIN | POLLPRI) 
	       && pollMngPollSources[i].readFnk)
	      {
		/*printf("pollMngPollSources[i].readFnk(i:%i, fd:%i\n",i, \
		  fdinfo[i].fd     );*/

		ec_neg1( tmp = read(fdinfo[i].fd, buf, 256))
		  //if(tmp)
		  //  {
		  ec_neg1( pollMngPollSources[i].readFnk(buf,\
							 tmp,\
							 i,\
							 (void*)&pollMngPollSources[i]\
			   ))
		  // }
		  //continue;
		  //break;
	      }
	    if(fdinfo[i].revents & (POLLOUT | POLLWRNORM) 
	       && pollMngPollSources[i].writeFnk)
	      {                             //daten zum senden holen
		tmp = pollMngPollSources[i].writeFnk(writebuf,i);
		ec_neg1( write(fdinfo[i].fd,writebuf,tmp) )
		  //break;
	      }
	  }//end for
    }//end while
  return 0;
EC_CLEANUP_BGN
  return -1;
EC_CLEANUP_END
}
