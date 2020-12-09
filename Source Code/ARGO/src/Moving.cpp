#include "stdafx.h"
#include "Moving.h"
#include "Idle.h"
#include "Attacking.h"

void Moving::idle(FiniteStateMachine* a)
{
#ifdef _DEBUG
	std::cout << "Moving -> Idle" << std::endl;

#endif // _DEBUG
	a->setCurrent(new Idle());
	delete this;
}

void Moving::moving(FiniteStateMachine* a)
{
}

void Moving::attacking(FiniteStateMachine* a)
{
#ifdef _DEBUG
	std::cout << "Moving -> Attacking" << std::endl;
#endif // _DEBUG
	a->setCurrent(new Attacking());
	delete this;
}
