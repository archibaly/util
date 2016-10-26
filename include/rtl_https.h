#ifndef _RTL_HTTPS_H_
#define _RTL_HTTPS_H_

#include <stdint.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>

struct ssl {
	int sockfd;
	SSL *ssl;
	SSL_CTX *ctx;
};

int rtl_https_send_get_request(struct ssl *ssl, const char *path,
							   const char *host, int port);
int rtl_https_send_post_request(struct ssl *ssl, const char *path,
								const char *host, int port, const uint8_t *body,
								size_t body_len);
int rtl_https_recv_response(struct ssl *ssl, uint8_t *resp, size_t size);
int rtl_https_save_body_to_file(struct ssl *ssl, const char *filename);

#endif /* _RTL_HTTPS_H_ */
