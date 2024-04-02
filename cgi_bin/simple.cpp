#include <iostream>

int main(int ac, char *av[], char *env[])
{
	(void) ac;
	(void) av;

	std::cout << "Content-Type: text/plain\n\n";
	for (int i = 0; env[i]; ++i)
		std::cout << env[i] << std::endl;

	return (0);
}
