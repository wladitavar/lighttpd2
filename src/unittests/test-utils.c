
#include <lighttpd/base.h>

#define perror(msg) g_error("(%s:%i) %s failed: %s", __FILE__, __LINE__, msg, g_strerror(errno))

static void test_send_fd(void) {
	int pipefds[2], sockfd[2], rfd = -1;
	char buf[6];

	if (-1 == pipe(pipefds)) {
		perror("pipe");
	}

	if (-1 == socketpair(AF_UNIX, SOCK_STREAM, 0, sockfd)) {
		perror("socketpair");
	}

	if (-1 == li_send_fd(sockfd[0], pipefds[0])) {
		perror("li_send_fd");
	}

	if (-1 == li_receive_fd(sockfd[1], &rfd)) {
		perror("li_receive_fd");
	}

	write(pipefds[1], CONST_STR_LEN("test\0"));

	if (-1 == read(rfd, buf, 5)) {
		perror("read");
	}

	buf[5] = '\0';
	g_test_message("received: %s", buf);

	g_assert_cmpstr(buf, ==, "test");

	close(pipefds[0]); close(pipefds[1]);
	close(sockfd[0]); close(sockfd[1]);
	close(rfd);
}

static void test_apr_sha1_base64(void) {
	GString *dest = g_string_sized_new(0);
	GString pass = li_const_gstring(CONST_STR_LEN("bar"));

	li_apr_sha1_base64(dest, &pass);

	g_assert_cmpstr(dest->str, ==, "{SHA}Ys23Ag/5IOWqZCw9QGaVDdHwH00=");
}

int main(int argc, char **argv) {
	g_test_init(&argc, &argv, NULL);

	g_test_add_func("/utils/send_fd", test_send_fd);
	g_test_add_func("/utils/apr_sha1_base64", test_apr_sha1_base64);

	return g_test_run();
}