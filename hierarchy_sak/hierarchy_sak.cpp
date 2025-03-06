// hierarchy_sak.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
//
//#include <iostream>
//#include <optional>
//#include <sstream>
//#include <string>
//#include <vector>

#include "generic_heriarchy.h"
#include "handle_json.h"
#include "handle_text.h"

#include <iostream>

int main()
{
	const char* json_text = "{\"name\":\"John Smith\",\"programmer\":true,\"pi\":3.14159,\"age\":30,\"cars\":[{\"name\":\"Ford\",\"models\":[\"Fiesta\",\"Focus\",\"Mustang\"]},{\"name\":\"BMW\",\"models\":[\"320\",\"X3\",\"X5\"]},{\"name\":\"Fiat\",\"models\":[\"500\",\"Panda\"]}]}";
	handle_json json_handler;
	generic_heirarchy* h = json_handler.produce_heirarchy(json_text);
	
	//handle_text text_handler;
	//std::cout << text_handler.consume_heirarchy(*h) << std::endl;

	//std::ostringstream ss;
	//h->dump(ss);
	//std::cout << ss.str() << std::endl;

	std::cout << json_handler.consume_heirarchy(*h) << std::endl;

	delete h;
}