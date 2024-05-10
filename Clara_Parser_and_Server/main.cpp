#include "Parser.hpp"
#include "Server.hpp"

int main(int ac, char **av)
{
        Parser parser(ac, av);
        std::vector<Server>	servers = parser.startParsing();
        std::cout << "boa" << std::endl;
}