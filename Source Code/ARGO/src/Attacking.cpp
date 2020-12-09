#include "stdafx.h"
#include "Attacking.h"
#include "Moving.h"
#include "Idle.h"

void Attacking::idle(FiniteStateMachine* a)
{
#ifdef _DEBUG
	std::cout << "Attacking -> Idle" << std::endl;

#endif // _DEBUG
	a->setCurrent(new Idle());
	delete this;
}

void Attacking::moving(FiniteStateMachine* a)
{
#ifdef _DEBUG
	std::cout << "Attacking -> Moving" << std::endl;

#endif // _DEBUG
	a->setCurrent(new Moving());
	delete this;
}

void Attacking::attacking(FiniteStateMachine* a)
{
}
