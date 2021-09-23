#pragma once
#include <iostream>
#include "Application/Application.h"

int main(int argc, char** argv)
{
	auto app = new Application();
	printf("CMP303 - Application");
	app->Run();
	delete app;
	app = nullptr;

	return 0;
}