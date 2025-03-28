
#include "client.h"
#include <thread>

int main() {

	Client s = Client(8085, 4096);
	// Start the connection to the server
	s.connection();

	// Creating threads by ref the class and the object
	std::thread resp(&Client::response, &s);
	std::thread rec(&Client::receive, &s);
	
	// This wait for the threads to finish 
	// To then do other actions
	resp.join();
	rec.join();



	return 0;
}
