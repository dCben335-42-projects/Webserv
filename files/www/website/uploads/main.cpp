#include "Bureaucrat.hpp"
#include <iostream>

int	main(void)
{
	Bureaucrat Henri("Henri", 5);
	Form form("form", 4, 4);

	Henri.signForm(form);
	return (0);
}
