#include "character.h"


int Character::getAtk(){
	return this->Atk;
}

int Character::getDef(){
	return this->Def;
}

int Character::getHP(){
	return this->HP;
}

int Character::getGold(){
	return this->gold;
}

posn Character::getPosition(){
	return this->position;
}

std::string Character::getRace(){
	return this->race;
}

char Character::getSymbol(){
	return this->symbol;
}

std::string Character::dealDamage(Character * opponent){
	int amount;
	std::string combatLog;
	// add implementation later
	opponent->takeDamage(amount);
	combatLog = "test";
	return combatLog;
}

void Character::takeDamage(int amount){
	this->HP -= amount;
	if (this->HP < 0){
		this->HP = 0;
	}
}