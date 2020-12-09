#include "stdafx.h"
#include "Idle.h"
#include "Moving.h"
#include "Attacking.h"

void Idle::idle(FiniteStateMachine* a)
{
}

void Idle::moving(FiniteStateMachine* a)
{
#ifdef _DEBUG
	std::cout << "Idle -> Moving" << std::endl;

#endif // _DEBUG
	a->setCurrent(new Moving());
	delete this;
}

void Idle::attacking(FiniteStateMachine* a)
{
#ifdef _DEBUG
	std::cout << "Idle -> Attacking" << std::endl;

#endif // _DEBUG
	a->setCurrent(new Attacking());
	delete this;
}
