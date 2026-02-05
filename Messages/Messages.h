#pragma once
#include <string>
// Simple file mimicking the layout of UCI structs residing in another file for 
// each service to use.

struct AppStatus {

	std::string appId = {};
	std::string appHealth = {};
	double appRuntime = NULL;
};

struct AppDataRequest1 {

	std::string appId = {};
	std::string appHealth = {};
	uint32_t numberToAdd = NULL;

};

struct AppDataRequest2 {

	std::string appId = {};
	std::string appHealth = {};
	float numberToMultiply = NULL;

};