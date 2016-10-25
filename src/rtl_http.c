#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "rtl_http.h"
#include "rtl_socket.h"

/*
 * @hostname: the remote hostname
 * @path: the path to get
 * @header: built header
 */
int rtl_http_build_get_header(const char *hostname, const char *path, char *header)
{
	const char *getpath = path;
	char *tpl = "GET /%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n";

	if (getpath[0] == '/')
		getpath++;

	sprintf(header, tpl, getpath, hostname);

	return strlen(header);
}

/*
 * @hostname: the remote hostname
 * @path: the path to post
 * @header: built header
 */
int rtl_http_build_post_header(const char *hostname, const char *path, char *header)
{
	const char *postpath = path;
	char *tpl = "POST /%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n";

	if (postpath[0] == '/')
		postpath++;

	sprintf(header, tpl, postpath, hostname);

	return strlen(header);
}

/*
 * @buff: stored response packet
 * @size: size of buff
 */
int rtl_http_get_body_pos(const char *buff, int size)
{
	int i;
	for (i = 0; i < size; i++) {
		if (strncmp(buff + i, "\r\n\r\n", strlen("\r\n\r\n")) == 0) {
			return i + strlen("\r\n\r\n");
		}
	}
	return -1;
}

int rtl_http_send_request(int type, const char *host, uint16_t port, const char *path, char *resp, int len)
{
	char header[1024];
	int header_len;

	if (type == RTL_HTTP_GET)
		header_len = rtl_http_build_get_header(host, path, header);
	else if (type == RTL_HTTP_POST)
		header_len = rtl_http_build_post_header(host, path, header);
	else
		return -1;

	int sockfd = rtl_socket_connect(host, port);
	if (sockfd < 0)
		return -1;

	int ret = 0;
	if (rtl_socket_sendn(sockfd, header, header_len) < 0) {
		ret = -1;
		goto out;
	}

	/* read response */
	ret = rtl_socket_recvn(sockfd, resp, len);

out:
	close(sockfd);
	return ret;
}
