#include "Bureaucrat.hpp"
#include <iostream>

int	main(void)
{
	Bureaucrat Henri("Henri", 5);
	Form form("form", 4, 4);

	Henri.signForm(form);
	return (0);
}

/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bcabocel <bcabocel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 18:11:19 by bcabocel          #+#    #+#             */
/*   Updated: 2025/10/21 18:01:47 by bcabocel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Materias/Ice/Ice.hpp"
#include "./Materias/Cure/Cure.hpp"
#include "./Characters/Character/Character.hpp"
#include "./MateriaSources/MateriaSource/MateriaSource.hpp"
#include "./Floor/Floor.hpp"
#include <iostream>

int main( void )
{
	std::cout << "\n----- Materia and Character test -----\n" << std::endl;
	{
		AMateria* ice = new Ice();
		AMateria* ice_clone = ice->clone();
		AMateria* cure = new Cure();
		ICharacter* character2 = new Character("Hero");
		ICharacter* character = character2;
		character->equip(ice);
		character->equip(cure);
		character->equip(ice);
		character->equip(ice_clone);
	
		character->use(0, *character);
		character->use(1, *character);
		character->use(2, *character);
		character->use(3, *character);

		delete character;
	}
	std::cout << std::endl;
	std::cout << "----- MateriaSource test -----\n" << std::endl;
	{
		IMateriaSource* src = new MateriaSource();
		AMateria* M1 = new Ice();
		AMateria* M2 = new Cure();
		AMateria* M3 = M1->clone();
		AMateria* M4 = M2->clone();
		AMateria* M5 = new Ice();
		src->learnMateria(NULL);
		src->learnMateria(M1);
		src->learnMateria(M2);
		src->learnMateria(M3);
		src->learnMateria(M4);

		src->learnMateria(M5); // should not be learned

	
		ICharacter* me = new Character("me");
	
		me->equip(src->createMateria("ice"));
		ICharacter* bob = new Character("bob");
		me->equip(src->createMateria("curse"));
	
		me->use(0, *bob);
		me->use(1, *bob);
		me->use(2, *bob);
	
		delete bob;
		delete me;
		delete src;

		delete M5; // not learned, must be deleted here
	}
	std::cout << std::endl;
	std::cout << "----- Floor test -----\n" << std::endl;
	{
		AMateria* ice = new Ice();
		AMateria* cure = new Cure();
		Floor::drop(ice);
		Floor::drop(cure);
		Floor::drop(NULL); // should not be dropped

		AMateria* picked1 = Floor::pick(0);
		AMateria* picked2 = Floor::pick(1);
		AMateria* picked3 = Floor::pick(2); // should be NULL

		ICharacter* character = new Character("FloorTester");
		character->equip(picked1);
		character->equip(picked2);
		character->equip(picked3); // should not equip

		ICharacter* target = new Character("Target");
		character->use(0, *target);
		character->use(1, *target);
		character->use(2, *target); // should not use

		delete target;
		delete character;
	}
	std::cout << "----- Break test -----\n" << std::endl;

	{
		AMateria* ice = new Ice();
		ICharacter* character = new Character("A");
		IMateriaSource* src = new MateriaSource();
		src->learnMateria(ice);
		character->equip(ice);

		IMateriaSource *src2 = src;
		character->equip(src2->createMateria("ice"));
		delete character;
		delete src2;
	}
	std::cout << "----- Floor drop -----\n" << std::endl;
	{
		AMateria* ice1 = new Ice();
		AMateria* ice2 = new Ice();
		AMateria* ice3 = new Ice();
		AMateria* ice4 = new Ice();

		Floor::drop(ice1);
		Floor::drop(ice1);
		Floor::drop(ice2);
		Floor::drop(ice3);
		Floor::drop(ice3);
		Floor::drop(ice4);
	}
	Floor::clean();
	return 0;
}

