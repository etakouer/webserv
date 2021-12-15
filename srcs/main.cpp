#include "Master.hpp"

int main(int ac, char **av)
{
	(void)ac;
	std::string file;
	if (av[1])
		file = av[1];
	Master master(file);
	master.init();
	master.work();
}
