
#include "generic_heriarchy.h"
#include "handle_json.h"
#include "handle_text.h"
#include "handle_digraph.h"

#include <iostream>
#include <sstream>
#include <fstream>

int main(int argc, char** argv)
{
	std::string json_text;
	
	if (argc > 1)
	{
		std::ifstream inputFile(argv[1]);
		std::ostringstream ss;
		if (inputFile.is_open())
		{
			std::string line;
			while (std::getline(inputFile, line))
			{
				ss << line;
			}

			json_text = ss.str().c_str();
		}
		else
		{
			std::cerr << "Failed to read " << argv[1] << std::endl;
		}
	}
	else
	{
		json_text = "{\"name\":\"John Smith\",\"programmer\":true,\"pi\":3.14159,\"age\":30,\"cars\":[{\"name\":\"Ford\",\"models\":[\"Fiesta\",\"Focus\",\"Mustang\"]},{\"name\":\"BMW\",\"models\":[\"320\",\"X3\",\"X5\"]},{\"name\":\"Fiat\",\"models\":[\"500\",\"Panda\"]}]}";
	}
	
	handle_json json_handler;
	generic_heirarchy* h = json_handler.produce_heirarchy(json_text);
	
	//handle_text text_handler;
	//std::cout << text_handler.consume_heirarchy(*h) << std::endl;
	
	handle_digraph digraph_handler;
	std::cout << digraph_handler.consume_heirarchy(*h) << std::endl;

	//std::ostringstream ss;
	//h->dump(ss);
	//std::cout << ss.str() << std::endl;

	//std::cout << json_handler.consume_heirarchy(*h) << std::endl;

	delete h;
}