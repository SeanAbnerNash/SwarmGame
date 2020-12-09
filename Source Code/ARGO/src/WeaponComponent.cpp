#include "stdafx.h"
#include "WeaponComponent.h"

WeaponComponent::WeaponComponent() :
	Component(ComponentType::Weapon),
	m_currentWeapon(Weapon::Pistol),
	m_glowStickCooldown(0.0f),
	m_gunCooldown(0.0f)
{
	for (int index = 0; index < S_NUMBER_OF_WEAPONS; index++)
	{
		m_ammo[index] = 0;
	}
}

void WeaponComponent::reduceCooldowns(float t_dt)
{
	if (m_glowStickCooldown > 0.0f)
	{
		m_glowStickCooldown -= t_dt;
	}
	if (m_gunCooldown > 0.0f)
	{
		m_gunCooldown -= t_dt;
	}
}

bool WeaponComponent::fireGun()
{
	if (m_ammo[static_cast<int>(m_currentWeapon)] != 0 || m_currentWeapon == Weapon::Pistol)
	{
		if (m_gunCooldown <= 0.0f)
		{
			switch (m_currentWeapon)
			{
			case Weapon::Pistol:
				m_gunCooldown = PISTOL_COOLDOWN;
				break;
			case Weapon::MachineGun:
				m_gunCooldown = MACHINEGUN_COOLDOWN;
				break;
			case Weapon::GrenadeLauncher:
				m_gunCooldown = GREANADE_LAUNCHER_COOLDOWN;
				break;
			case Weapon::Shotgun:
				m_gunCooldown = SHOTGUN_COOLDOWN;
				break;
			default:
				break;
			}

			if (m_currentWeapon != Weapon::Pistol)
			{
				m_ammo[static_cast<int>(m_currentWeapon)]--;
				if (m_ammo[static_cast<int>(m_currentWeapon)] == 0)
				{
					m_currentWeapon = Weapon::Pistol;
				}
			}
			return true;
		}
	}
	return false;
}

bool WeaponComponent::throwGlowStick()
{
	if (m_glowStickCooldown <= 0.0f)
	{
		m_glowStickCooldown = GLOWSTICK_COOLDOWN;
		return true;
	}
	return false;
}

Weapon WeaponComponent::getCurrent()
{
	return m_currentWeapon;
}

int WeaponComponent::getMaxAmmo(Weapon t_ammoType)
{
	switch (t_ammoType)
	{
	case Weapon::MachineGun:
		return MACHINEGUN_MAX_AMMO;
		break;
	case Weapon::GrenadeLauncher:
		return GRENADE_LAUNCHER_MAX_AMMO;
		break;
	case Weapon::Shotgun:
		return SHOTGUN_MAX_AMMO;
		break;
	default:
		break;
	}
	return 0;
}

int WeaponComponent::getAmmo(Weapon t_ammoType)
{
	return m_ammo[static_cast<int>(t_ammoType)];
}

void WeaponComponent::setCurrent(Weapon t_weapon)
{
	m_currentWeapon = t_weapon;
}

void WeaponComponent::fillAmmo(Weapon t_type)
{
	m_currentWeapon = t_type;
	switch (m_currentWeapon)
	{
	case Weapon::Pistol	:
		m_ammo[static_cast<int>(Weapon::Pistol)] = 0;
		break;
	case Weapon::MachineGun:
		m_ammo[static_cast<int>(Weapon::MachineGun)] = MACHINEGUN_MAX_AMMO;
		break;
	case Weapon::GrenadeLauncher:
		m_ammo[static_cast<int>(Weapon::GrenadeLauncher)] = GRENADE_LAUNCHER_MAX_AMMO;
		break;
	case Weapon::Shotgun:
		m_ammo[static_cast<int>(Weapon::Shotgun)] = SHOTGUN_MAX_AMMO;
		break;
	default:
		break;
	}
}

void WeaponComponent::cycleCurrentWeapon(bool t_isUp)
{
	int currentWeaponIndex = static_cast<int>(m_currentWeapon);
	if (t_isUp)
	{
		currentWeaponIndex--;
		if (currentWeaponIndex < 0)
		{
			currentWeaponIndex = S_NUMBER_OF_WEAPONS - 1;
		}
	}
	else
	{
		currentWeaponIndex++;
		if (currentWeaponIndex >= S_NUMBER_OF_WEAPONS - 1)
		{
			currentWeaponIndex = 0;
		}
	}
	m_currentWeapon = static_cast<Weapon>(currentWeaponIndex);

}
