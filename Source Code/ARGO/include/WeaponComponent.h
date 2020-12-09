#pragma once
#include "Component.h"

enum class Weapon
{
	Pistol,
	MachineGun,
	GrenadeLauncher,
	Shotgun
};

class WeaponComponent :
	public Component
{
public:
	WeaponComponent();
	void reduceCooldowns(float t_dt);
	bool fireGun();
	bool throwGlowStick();
	Weapon getCurrent();
	int getMaxAmmo(Weapon t_ammoType);
	int getAmmo(Weapon t_ammoType);
	void setCurrent(Weapon t_weapon);
	void fillAmmo(Weapon t_type);
	void cycleCurrentWeapon(bool t_isUp);

private:
	float m_gunCooldown;
	float m_glowStickCooldown;
	Weapon m_currentWeapon;
	static const int S_NUMBER_OF_WEAPONS = 4;
	int m_ammo[S_NUMBER_OF_WEAPONS];

#ifdef _DEBUG
	const float GLOWSTICK_COOLDOWN = 60;
#else
	const float GLOWSTICK_COOLDOWN = 600;
#endif // _DEBUG

	const float PISTOL_COOLDOWN = 20;
	const float MACHINEGUN_COOLDOWN = 10;
	const float GREANADE_LAUNCHER_COOLDOWN = 45;
	const float SHOTGUN_COOLDOWN = 25;

	const int MACHINEGUN_MAX_AMMO = 60;
	const int SHOTGUN_MAX_AMMO = 20;
	const int GRENADE_LAUNCHER_MAX_AMMO = 10;
};

