
#include "include/server.h"

int main() {

	Server s = Server(8085);
	s.start();
	s.connection();
	s.stop();

	return 0;
}
