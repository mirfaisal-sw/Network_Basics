#include <mqueue.h>
#include <sys/dcmd_chr.h>
#include <sys/neutrino.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef FDSOCKTESTPR001
#define FDSOCKTESTPR001 "17686"
#endif
#ifndef FDSOCKTESTPR002
#define FDSOCKTESTPR002 "17687"
#endif
#ifndef FDSOCKTESTPR003
#define FDSOCKTESTPR003 "n/a"
#endif
#ifndef FDSOCKTESTPR004
#define FDSOCKTESTPR004 "n/a"
#endif
#ifndef FDSOCKTESTPR005
#define FDSOCKTESTPR005 "n/a"
#endif
#ifndef FDSOCKTESTPR006
#define FDSOCKTESTPR006 "n/a"
#endif
#ifndef FDSOCKTESTPR007
#define FDSOCKTESTPR007 "n/a"
#endif
#ifndef FDSOCKTESTPR008
#define FDSOCKTESTPR008 "n/a"
#endif
#ifndef FDSOCKTESTPR009
#define FDSOCKTESTPR009 "n/a"
#endif

/****************************************************************************
*
*						Subroutine testbody
*
*****************************************************************************
*
*	Purpose:	A generic signal handler.
*
****************************************************************************/
void testbody(int fd, char* id, int isfdatty)
{
        int rc, saved, fdflags;
        socklen_t slen;
        struct sockaddr saddr;
        struct msghdr msg;
        iov_t iov[1];
        char buf[1024];
        struct cmsghdr cmsg;
        struct _server_info servinfo;
        struct stat sbuf;
        struct mq_attr mqattr;

        rc = fstat(fd, &sbuf);
        assert(rc != -1);
        fdflags = fcntl(fd, F_GETFL);
        if (fdflags == -1) {
                SPRNT(buf, sizeof buf, "%s is not a real filesystem", id);
                testnote(buf);
                return;
        }
        fdflags &= O_ACCMODE;

        /***********************************************************************/
        /*
	 * mq_getattr()
	 */
        if (!S_TYPEISMQ(&sbuf)) {
                SPRNT(buf, sizeof buf, "devctl mq_getattr w. %s", id);
                testpntbegin(buf);

                alarm(3);
                errno = 0;
                rc = mq_getattr(fd, &mqattr);
                saved = errno;
                alarm(0);

                if ((rc == -1) && (saved == EBADF)) {
                        if (ConnectServerInfo(0, fd, &servinfo) != -1) {
                                testpntpass(strerror(saved));
                        } else {
                                testpntfail(strerror(saved));
                        }
                } else if ((rc == -1) && (saved == ENOTTY)) {
                        testpntsetupxfail(FDSOCKTESTPR001);
                        testpntfail(strerror(saved));
                } else {
                        testpntfail(strerror(saved));
                }

                testpntend();
        }
        /***********************************************************************/

        /***********************************************************************/
        /*
	 * isatty()
	 */
        if (isfdatty) {
                SPRNT(buf, sizeof buf, "devctl isatty (%d) w. %s", isfdatty, id);
                testpntbegin(buf);

                alarm(3);
                errno = 0;
                if (isatty(fd) == 1) {
                        if (ConnectServerInfo(0, fd, &servinfo) != -1) {
                                testpntpass(strerror(errno));
                        } else {
                                testpntfail(strerror(errno));
                        }
                } else {
                        testpntfail(strerror(errno));
                }
                alarm(0);

                testpntend();
        } else {
                SPRNT(buf, sizeof buf, "devctl isatty (%d) w. %s", isfdatty, id);
                testpntbegin(buf);

                alarm(3);
                if ((errno = devctl(fd, DCMD_CHR_ISATTY, 0, 0, 0)) == ENOTTY) {
                        if (ConnectServerInfo(0, fd, &servinfo) != -1) {
                                testpntpass(strerror(errno));
                        } else {
                                testpntfail(strerror(errno));
                        }
                } else {
                        testpntfail(strerror(errno));
                }
                alarm(0);

                testpntend();
        }
        /***********************************************************************/

        /***********************************************************************/
        /*
	 * shutdown()
	 */
        if (!S_ISSOCK(sbuf.st_mode)) {
                SPRNT(buf, sizeof buf, "devctl shutdown w. %s", id);
                testpntbegin(buf);
                slen = sizeof saddr;

                alarm(3);
                errno = 0;
                if ((shutdown(fd, SHUT_RD) == -1) && (errno == ENOTSOCK)) {
                        if (ConnectServerInfo(0, fd, &servinfo) != -1) {
                                testpntpass(strerror(errno));
                        } else {
                                testpntfail(strerror(errno));
                        }
                } else {
                        testpntfail(strerror(errno));
                }
                alarm(0);

                testpntend();
        }
        /***********************************************************************/

        /***********************************************************************/
        /*
	 * accept()
	 */
        if (!S_ISSOCK(sbuf.st_mode)) {
                SPRNT(buf, sizeof buf, "xtype accept w. %s", id);
                testpntbegin(buf);
                slen = sizeof saddr;

                alarm(3);
                errno = 0;
                rc = accept(fd, &saddr, &slen);
                saved = errno;
                alarm(0);

                if ((rc == -1) && (saved == ENOTSOCK)) {
                        if (ConnectServerInfo(0, fd, &servinfo) != -1) {
                                testpntpass(strerror(saved));
                        } else {
                                testpntfail(strerror(saved));
                        }
                } else if (rc >= 0) {
                        testpntsetupxfail(FDSOCKTESTPR004);
                        testpntfail("no xtype processing");
                } else if ((rc == -1) && (saved == EMSGSIZE)) {
                        testpntsetupxfail(FDSOCKTESTPR003);
                        testpntfail(strerror(saved));
                } else if ((rc == -1) && (saved == EINVAL)) {
                        testpntsetupxfail(FDSOCKTESTPR002);
                        testpntfail(strerror(saved));
                } else {
                        testpntfail(strerror(saved));
                }

                testpntend();
        }
        /***********************************************************************/

        /***********************************************************************/
        /*
	 * bind()
	 */
        if (!S_ISSOCK(sbuf.st_mode)) {
                SPRNT(buf, sizeof buf, "devctl bind w. %s", id);
                testpntbegin(buf);
                slen = sizeof saddr;

                alarm(3);
                errno = 0;
                if ((bind(fd, &saddr, slen) == -1) && (errno == ENOTSOCK)) {
                        if (ConnectServerInfo(0, fd, &servinfo) != -1) {
                                testpntpass(strerror(errno));
                        } else {
                                testpntfail(strerror(errno));
                        }
                } else {
                        testpntfail(strerror(errno));
                }
                alarm(0);

                testpntend();
        }
        /***********************************************************************/

        /***********************************************************************/
        /*
	 * getsockname()
	 */
        if (!S_ISSOCK(sbuf.st_mode)) {
                SPRNT(buf, sizeof buf, "devctl getsockname w. %s", id);
                testpntbegin(buf);
                slen = sizeof saddr;

                alarm(3);
                errno = 0;
                if ((getsockname(fd, &saddr, &slen) == -1) && (errno == ENOTSOCK)) {
                        if (ConnectServerInfo(0, fd, &servinfo) != -1) {
                                testpntpass(strerror(errno));
                        } else {
                                testpntfail(strerror(errno));
                        }
                } else {
                        testpntfail(strerror(errno));
                }
                alarm(0);

                testpntend();
        }
        /***********************************************************************/

        /***********************************************************************/
        /*
	 * getsocktopt()
	 */
        if (!S_ISSOCK(sbuf.st_mode)) {
                SPRNT(buf, sizeof buf, "mgrid getsockopt w. %s", id);
                testpntbegin(buf);
                slen = sizeof saddr;

                alarm(3);
                errno = 0;
                if ((getsockopt(fd, SOL_SOCKET, SO_TYPE, &saddr, &slen) == -1) && (errno == ENOTSOCK)) {
                        if (ConnectServerInfo(0, fd, &servinfo) != -1) {
                                testpntpass(strerror(errno));
                        } else {
                                testpntfail(strerror(errno));
                        }
                } else {
                        testpntfail(strerror(errno));
                }
                alarm(0);

                testpntend();
        }
        /***********************************************************************/

        /***********************************************************************/
        /*
	 * setsockopt()
	 */
        if (!S_ISSOCK(sbuf.st_mode)) {
                SPRNT(buf, sizeof buf, "mgrid setsockopt w. %s", id);
                testpntbegin(buf);
                slen = sizeof saddr;

                alarm(3);
                errno = 0;
                if ((setsockopt(fd, SOL_SOCKET, SO_TYPE, &saddr, slen) == -1) && (errno == ENOTSOCK)) {
                        if (ConnectServerInfo(0, fd, &servinfo) != -1) {
                                testpntpass(strerror(errno));
                        } else {
                                testpntfail(strerror(errno));
                        }
                } else {
                        testpntfail(strerror(errno));
                }
                alarm(0);

                testpntend();
        }
        /***********************************************************************/

        /***********************************************************************/
        /*
	 * mq_send()
	 */
        if (!S_TYPEISMQ(&sbuf)) {
                SPRNT(buf, sizeof buf, "xtype mq_send w. %s", id);
                testpntbegin(buf);

                alarm(3);
                errno = 0;
                rc = mq_send(fd, buf, 1, NULL);
                saved = errno;
                alarm(0);

                if ((rc == -1) && (saved == EBADF)) {
                        if (ConnectServerInfo(0, fd, &servinfo) != -1) {
                                testpntpass(strerror(saved));
                        } else {
                                testpntfail(strerror(saved));
                        }
                } else if (rc >= 0) {
                        testpntsetupxfail(FDSOCKTESTPR004);
                        testpntfail("no xtype processing");
                } else if ((rc == -1) && (saved == EMSGSIZE)) {
                        testpntsetupxfail(FDSOCKTESTPR003);
                        testpntfail(strerror(saved));
                } else if ((rc == -1) && (saved == EINVAL)) {
                        testpntsetupxfail(FDSOCKTESTPR002);
                        testpntfail(strerror(saved));
                } else if ((rc == -1) && (saved == ENOSYS)) {
                        testpntsetupxfail(FDSOCKTESTPR006);
                        testpntfail(strerror(saved));
                } else {
                        testpntfail(strerror(saved));
                }

                testpntend();
        }
        /***********************************************************************/

        /***********************************************************************/
        /*
	 * mq_receive()
	 */
        if (!S_TYPEISMQ(&sbuf)) {
                SPRNT(buf, sizeof buf, "xtype mq_receive w. %s", id);
                testpntbegin(buf);

                alarm(3);
                errno = 0;
                rc = mq_receive(fd, buf, 1, NULL);
                saved = errno;
                alarm(0);

                if ((rc == -1) && (saved == EBADF)) {
                        if (ConnectServerInfo(0, fd, &servinfo) != -1) {
                                testpntpass(strerror(saved));
                        } else {
                                testpntfail(strerror(saved));
                        }
                } else if (rc >= 0) {
                        testpntsetupxfail(FDSOCKTESTPR004);
                        testpntfail("no xtype processing");
                } else if ((rc == -1) && (saved == EBADF)) {
                        testpntsetupxfail(FDSOCKTESTPR002);
                        testpntfail(strerror(saved));
                } else if ((rc == -1) && (saved == EINVAL)) {
                        testpntsetupxfail(FDSOCKTESTPR005);
                        testpntfail("no default processing");
                } else if ((rc == -1) && (saved == ENOSYS)) {
                        testpntsetupxfail(FDSOCKTESTPR006);
                        testpntfail(strerror(saved));
                } else {
                        testpntfail(strerror(saved));
                }

                testpntend();
        }
        /***********************************************************************/

        /***********************************************************************/
        /*
	 * connect()
	 */
        if (!S_ISSOCK(sbuf.st_mode)) {
                SPRNT(buf, sizeof buf, "devctl connect w. %s", id);
                testpntbegin(buf);

                alarm(3);
                errno = 0;
                if ((connect(fd, &saddr, slen) == -1) && (errno == ENOTSOCK)) {
                        if (ConnectServerInfo(0, fd, &servinfo) != -1) {
                                testpntpass(strerror(errno));
                        } else {
                                testpntfail(strerror(errno));
                        }
                } else {
                        testpntfail(strerror(errno));
                }
                alarm(0);

                testpntend();
        }
        /***********************************************************************/

        /***********************************************************************/
        /*
	 * recv()
	 */
        if (!S_ISSOCK(sbuf.st_mode)) {
                SPRNT(buf, sizeof buf, "xtype recv w. %s", id);
                testpntbegin(buf);
                slen = sizeof saddr;

                alarm(3);
                errno = 0;
                rc = recv(fd, buf, 1, MSG_WAITALL);
                saved = errno;
                alarm(0);

                if ((rc == -1) && (saved == ((fdflags == O_WRONLY) ? EBADF : ENOTSOCK))) {
                        if (ConnectServerInfo(0, fd, &servinfo) != -1) {
                                testpntpass(strerror(saved));
                        } else {
                                testpntfail(strerror(saved));
                        }
                } else if (rc >= 0) {
                        testpntsetupxfail(FDSOCKTESTPR004);
                        testpntfail("no xtype processing");
                } else if ((rc == -1) && (saved == EMSGSIZE)) {
                        testpntsetupxfail(FDSOCKTESTPR003);
                        testpntfail(strerror(saved));
                } else if ((rc == -1) && (saved == EINVAL)) {
                        testpntsetupxfail(FDSOCKTESTPR002);
                        testpntfail(strerror(saved));
                } else {
                        testpntfail(strerror(saved));
                }

                testpntend();
        }
        /***********************************************************************/

        /***********************************************************************/
        /*
	 * recvfrom()
	 */
        if (!S_ISSOCK(sbuf.st_mode)) {
                SPRNT(buf, sizeof buf, "xtype recvfrom w. %s", id);
                testpntbegin(buf);
                slen = sizeof saddr;

                alarm(3);
                errno = 0;
                rc = recvfrom(fd, buf, 1, MSG_WAITALL, &saddr, &slen);
                saved = errno;
                alarm(0);

                if ((rc == -1) && (saved == ((fdflags == O_WRONLY) ? EBADF : ENOTSOCK))) {
                        if (ConnectServerInfo(0, fd, &servinfo) != -1) {
                                testpntpass(strerror(saved));
                        } else {
                                testpntfail(strerror(saved));
                        }
                } else if (rc >= 0) {
                        testpntsetupxfail(FDSOCKTESTPR004);
                        testpntfail("no xtype processing");
                } else if ((rc == -1) && (saved == EMSGSIZE)) {
                        testpntsetupxfail(FDSOCKTESTPR003);
                        testpntfail(strerror(saved));
                } else if ((rc == -1) && (saved == EINVAL)) {
                        testpntsetupxfail(FDSOCKTESTPR002);
                        testpntfail(strerror(saved));
                } else {
                        testpntfail(strerror(saved));
                }

                testpntend();
        }
        /***********************************************************************/

        /***********************************************************************/
        /*
	 * recvmsg()
	 */
        if (!S_ISSOCK(sbuf.st_mode)) {
                SPRNT(buf, sizeof buf, "xtype recvmsg w. %s", id);
                testpntbegin(buf);
                slen = sizeof saddr;

                memset(&cmsg, 0x00, sizeof cmsg);
                memset(&msg, 0x00, sizeof msg);
                SETIOV(iov, buf, 1024);
                msg.msg_iov = iov;
                msg.msg_iovlen = 1;
                msg.msg_control = &cmsg;
                msg.msg_controllen = sizeof(cmsg);

                alarm(3);
                errno = 0;
                rc = recvmsg(fd, &msg, MSG_WAITALL);
                saved = errno;
                alarm(0);

                if ((rc == -1) && (saved == ((fdflags == O_WRONLY) ? EBADF : ENOTSOCK))) {
                        if (ConnectServerInfo(0, fd, &servinfo) != -1) {
                                testpntpass(strerror(saved));
                        } else {
                                testpntfail(strerror(saved));
                        }
                } else if (rc >= 0) {
                        testpntsetupxfail(FDSOCKTESTPR004);
                        testpntfail("no xtype processing");
                } else if ((rc == -1) && (saved == EMSGSIZE)) {
                        testpntsetupxfail(FDSOCKTESTPR003);
                        testpntfail(strerror(saved));
                } else if ((rc == -1) && (saved == EINVAL)) {
                        testpntsetupxfail(FDSOCKTESTPR002);
                        testpntfail(strerror(saved));
                } else {
                        testpntfail(strerror(saved));
                }

                testpntend();
        }
        /***********************************************************************/

        /***********************************************************************/
        /*
	 * send()
	 */
        if (!S_ISSOCK(sbuf.st_mode)) {
                SPRNT(buf, sizeof buf, "xtype send w. %s", id);
                testpntbegin(buf);
                slen = sizeof saddr;

                alarm(3);
                errno = 0;
                rc = send(fd, buf, 1, MSG_WAITALL);
                saved = errno;
                alarm(0);

                if ((rc == -1) && (saved == ((fdflags == O_RDONLY) ? EBADF : ENOTSOCK))) {
                        if (ConnectServerInfo(0, fd, &servinfo) != -1) {
                                testpntpass(strerror(saved));
                        } else {
                                testpntfail(strerror(saved));
                        }
                } else if (rc >= 0) {
                        testpntsetupxfail(FDSOCKTESTPR004);
                        testpntfail("no xtype processing");
                } else if ((rc == -1) && (saved == EMSGSIZE)) {
                        testpntsetupxfail(FDSOCKTESTPR003);
                        testpntfail(strerror(saved));
                } else if ((rc == -1) && (saved == EINVAL)) {
                        testpntsetupxfail(FDSOCKTESTPR002);
                        testpntfail(strerror(saved));
                } else {
                        testpntfail(strerror(saved));
                }

                testpntend();
        }
        /***********************************************************************/

        /***********************************************************************/
        /*
	 * sendto()
	 */
        if (!S_ISSOCK(sbuf.st_mode)) {
                SPRNT(buf, sizeof buf, "xtype sendto w. %s", id);
                testpntbegin(buf);
                slen = sizeof saddr;

                alarm(3);
                errno = 0;
                rc = sendto(fd, buf, 1, MSG_WAITALL, &saddr, slen);
                saved = errno;
                alarm(0);

                if ((rc == -1) && (saved == ((fdflags == O_RDONLY) ? EBADF : ENOTSOCK))) {
                        if (ConnectServerInfo(0, fd, &servinfo) != -1) {
                                testpntpass(strerror(saved));
                        } else {
                                testpntfail(strerror(saved));
                        }
                } else if (rc >= 0) {
                        testpntsetupxfail(FDSOCKTESTPR004);
                        testpntfail("no xtype processing");
                } else if ((rc == -1) && (saved == EMSGSIZE)) {
                        testpntsetupxfail(FDSOCKTESTPR003);
                        testpntfail(strerror(saved));
                } else if ((rc == -1) && (saved == EINVAL)) {
                        testpntsetupxfail(FDSOCKTESTPR002);
                        testpntfail(strerror(saved));
                } else {
                        testpntfail(strerror(saved));
                }

                testpntend();
        }
        /***********************************************************************/

        /***********************************************************************/
        /*
	 * sendmsg()
	 */
        if (!S_ISSOCK(sbuf.st_mode)) {
                SPRNT(buf, sizeof buf, "xtype sendmsg w. %s", id);
                testpntbegin(buf);
                slen = sizeof saddr;

                memset(&cmsg, 0x00, sizeof cmsg);
                memset(&msg, 0x00, sizeof msg);
                SETIOV(iov, buf, 1024);
                msg.msg_iov = iov;
                msg.msg_iovlen = 1;
                msg.msg_control = &cmsg;
                msg.msg_controllen = sizeof(cmsg);

                alarm(3);
                errno = 0;
                rc = sendmsg(fd, &msg, MSG_WAITALL);
                saved = errno;
                alarm(0);

                if ((rc == -1) && (saved == ((fdflags == O_RDONLY) ? EBADF : ENOTSOCK))) {
                        if (ConnectServerInfo(0, fd, &servinfo) != -1) {
                                testpntpass(strerror(saved));
                        } else {
                                testpntfail(strerror(saved));
                        }
                } else if (rc >= 0) {
                        testpntsetupxfail(FDSOCKTESTPR004);
                        testpntfail("no xtype processing");
                } else if ((rc == -1) && (saved == EMSGSIZE)) {
                        testpntsetupxfail(FDSOCKTESTPR003);
                        testpntfail(strerror(saved));
                } else if ((rc == -1) && (saved == EINVAL)) {
                        testpntsetupxfail(FDSOCKTESTPR002);
                        testpntfail(strerror(saved));
                } else {
                        testpntfail(strerror(saved));
                }

                testpntend();
        }
        /***********************************************************************/
}
