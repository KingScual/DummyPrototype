#pragma once
#include <string>
// Simple file mimicking the layout of UCI structs residing in another file for 
// each service to use.
struct Message
{
	virtual ~Message() = default;
};

struct AppStatus : public Message
{

	std::string appId{};
	std::string appHealth{};
	double appRuntime{ 0.0 };
};

struct AppDataRequest1 : public Message
{

	std::string appId{};
	std::string appHealth{};
	uint32_t numberToAdd{0};

};

struct AppDataRequest2 : public Message
{

	std::string appId{};
	std::string appHealth{};
	float numberToMultiply{ 0.0f };

};