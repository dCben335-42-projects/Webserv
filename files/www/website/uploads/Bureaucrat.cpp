#include "Bureaucrat.hpp"
#include "Form.hpp"
#include <iostream>

Bureaucrat::Bureaucrat() : m_name("Jordan"), m_grade(150) {}

Bureaucrat::Bureaucrat(std::string name, int grade) : m_name(name), m_grade(grade)
{
	if (grade > 150)
		throw GradeTooLowException();
	if (grade < 1)
		throw GradeTooHighException();
}

Bureaucrat::~Bureaucrat() {}

Bureaucrat& Bureaucrat::operator=(Bureaucrat& obj)
{
	if (this != &obj)
		m_grade = obj.m_grade;
	return (*this);
}

Bureaucrat::Bureaucrat(Bureaucrat &obj) : m_name(obj.m_name), m_grade(obj.m_grade) {}

//Getters
std::string Bureaucrat::getName() {return (m_name);}

int Bureaucrat::getGrade() {return (m_grade);}

//Exceptions
const char *Bureaucrat::GradeTooHighException::what() const throw()
{
	return (GRADE_TOO_HIGH);
}

const char *Bureaucrat::GradeTooLowException::what() const throw()
{
	return (GRADE_TOO_LOW);
}


void Bureaucrat::incrementGrade()
{
	if (m_grade == 1)
		throw GradeTooHighException();
	m_grade--;
}

void Bureaucrat::decrementGrade()
{
	if (m_grade == 150)
		throw GradeTooLowException();
	m_grade++;
}

std::ostream& operator<<(std::ostream& os, Bureaucrat& obj)
{
	os << obj.getName() << ", bureaucrat grade " << obj.getGrade() << std::endl;
	return (os);
}

void Bureaucrat::signForm(Form& form)
{
	try
	{
		form.beSigned(*this);
	}
	catch(const std::exception& e)
	{
		std::cout << this->getName() << " couldn't sign " << form.getName() << " because " << e.what() << '\n';
		return ;
	}
	std::cout << this->getName() << " signed " << form.getName() << '\n';
}

