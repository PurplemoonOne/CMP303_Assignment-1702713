#pragma once
#include "Application\Server.h"

int main(int argc, char** argv)
{
	auto server = new Server();
	server->Run();
	delete server;
	return 0;
}