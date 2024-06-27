
#include "Parser.hpp"
#include "Server.hpp"
#include "WebServ.hpp"

int main(int ac, char **av)
{
        try
        {
                Parser parser(ac, av);
                WebServ program(parser.startParsing());
                program.runWebServ();
        }
        catch(const std::exception& e){std::cerr << e.what() << '\n';}
}
