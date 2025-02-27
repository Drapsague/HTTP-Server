
#include "server.h"

int main() {

	Server s = Server(8085);
	s.start();

	return 0;
}
