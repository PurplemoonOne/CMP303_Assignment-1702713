#pragma once
#include "Application/Application.h"

int main(int argc, char** argv)
{
	auto app = new Application();
	app->Run();
	delete app;
	app = nullptr;

	return 0;
}