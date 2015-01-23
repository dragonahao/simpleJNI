/* 头文件begin */
#include "localSocket.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
/* 头文件end */

#ifdef __cplusplus
extern "C" {
#endif



/* 创建本地socket客户端 */
int socket_local_client(const char *name, int namespaceId, int type)
{
    int socketID;
    int ret;

    socketID = socket(AF_LOCAL, type, 0);
    if(socketID < 0)
    {
        return CREATE_ERR;
    }

    ret = socket_local_client_connect(socketID, name, namespaceId, type);
    if (ret < 0)
    {
        close(socketID);

        return ret;
    }

    return socketID;
}

/* 连接到相应的fileDescriptor上 */
int socket_local_client_connect(int fd, const char *name, int namespaceId, int type)
{
    struct sockaddr_un addr;
    socklen_t socklen;
    size_t namelen;
    int ret;

    ret = socket_make_sockaddr_un(name, namespaceId, &addr, &socklen);
    if (ret < 0)
    {
        return ret;
    }

    if(connect(fd, (struct sockaddr *) &addr, socklen) < 0)
    {
        return CONNECT_ERR;
    }

    return fd;
}

/* 构造sockaddr_un */
int socket_make_sockaddr_un(const char *name, int namespaceId, struct sockaddr_un *p_addr, socklen_t *socklen)
{
    size_t namelen;

    MEM_ZERO(p_addr, sizeof(*p_addr));
#ifdef HAVE_LINUX_LOCAL_SOCKET_NAMESPACE

    namelen  = strlen(name);

    // Test with length +1 for the *initial* '\0'.
    if ((namelen + 1) > sizeof(p_addr->sun_path))
    {
        return LINUX_MAKE_ADDRUN_ERROR;
    }
    p_addr->sun_path[0] = 0;
    memcpy(p_addr->sun_path + 1, name, namelen);

#else

    namelen = strlen(name) + strlen(FILESYSTEM_SOCKET_PREFIX);

    /* unix_path_max appears to be missing on linux */
    if (namelen > (sizeof(*p_addr) - offsetof(struct sockaddr_un, sun_path) - 1))
    {
        return NO_LINUX_MAKE_ADDRUN_ERROR;
    }

    strcpy(p_addr->sun_path, FILESYSTEM_SOCKET_PREFIX);
    strcat(p_addr->sun_path, name);

#endif

    p_addr->sun_family = AF_LOCAL;
    *socklen = namelen + offsetof(struct sockaddr_un, sun_path) + 1;

    return NO_ERR;
}

#ifdef __cplusplus
}
#endif



int sendmsgtojava(char *data){
	int socketID;
	struct sockaddr_un serverAddr;
	char path[] = "hjw_local_socket\0";
	int ret;

	socketID = socket_local_client(path, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM);
	if (socketID < 0)
	{
		return socketID;
	}

	//send message
	int ret2;
	//char buf[] = "1__jj__com.dotgears.flappybird";
	//char buf[] = "2__jj__8270";
	ret2 = write(socketID, data, strlen(data));
	printf("sendMessage return val:%d\n",ret2);


	ret = close(socketID);
	if (ret < 0)
	{
		return CLOSE_ERR;
	}

	return NO_ERR;
}


/*
int main(int argc, char *argv[]) {
	int val = sendmsgtojava();
	printf("exec:%d\n", val);
	printf("hello world\n");
	return 0;
 }
*/



