#include "floor.h"

Floor::Floor(int level, bool won, bool hostile, bool spawned, Player * player) {
	this->level = level;
	this->won = won;
	this->stairVisible = false;
	this->merchantHostile = hostile;
	this->barrierSpawned = spawned;
	this->player = player;
	this->stair = NULL;
}

Floor::Floor() {
	this->level = 1;
	this->won = false;
	this->stairVisible = false;
	this->merchantHostile = false;
	this->barrierSpawned = false;
	this->player = NULL;
	this->stair = NULL;
}

Floor::~Floor() {
	for (Enemy *enemy : this->enemies) {
		delete enemy;
	}
	for (Item *item : this->items) { 
		delete item; 
	}
	for (Chamber *chamber : this->chambers) {
		delete chamber;
	}
	delete this->player;
	delete this->stair;
	delete this->suit;
}

void Floor::generatePosition(int &id, Posn &pos) {
	id = rand() % 5 + 1;
	Chamber *c = chambers.at(id - 1);
	Posn p;
	do {
		p = c->getPosition();
	} while (!this->validTile(p));
	pos = p;
}

Posn Floor::dragonPosition(Posn pos) {
	Posn p;
	do {
		int i = rand() % 8;
		std::string dir[8] = {"ea", "we", "no", "so", "ne", "nw", "sw", "se"};
		p = pos.getNew(dir[i]);
	} while (!this->validTile(p));
	return p;
}

void Floor::initFloor(char type) {
	int id;
	int dragons;
	srand(time(NULL));
	generateChamber();
	generatePlayer(type, id);
	generateStair(id);
	generateItems(dragons);
	generateEnemies(dragons);
}

void Floor::generateChamber() {
	for(int i = 0; i < 5; i++) {
		Chamber *c = new Chamber(i);
		this->chambers.push_back(c);
	}
}

void Floor::generatePlayer(char type, int &id) {
	Posn p;
	generatePosition(id, p);
	if (type == 'h') {
	 	this->player = new Human(140, 0, p);
	} else if (type == 'o') {
		this->player = new Orc(180, 0, p);
	} else if (type == 'e') {
		this->player = new Orc(140, 0, p);
	} else if (type == 'd') {
		this->player = new Orc(100, 0, p);
	}
	displayGrid[p.y - 1][p.x - 1] = '@';
}

void Floor::generateStair(int id) {
	int id2;
	Posn p;
	do {
		generatePosition(id2, p);
	} while (id == id2);
	this->stair = new Stair(p);
}

void Floor::generateEnemies(int dragons) {
	for (int i = 0; i < 20 - dragons; i++) {
		int num = rand() % 18 + 1;
		int id;
		Posn pos;
		generatePosition(id, pos);
		Enemy *e;
		if (num <= 4) {
			e = new Werewolf(pos, id);
			this->enemies.push_back(e);
		} else if (num <= 7) {
			e = new Vampire(pos, id);
			this->enemies.push_back(e);
		} else if (num <= 12) {
			e = new Goblin(pos, id);
			this->enemies.push_back(e);
		} else if (num <= 14) {
			e = new Troll(pos, id);
			this->enemies.push_back(e);
		} else if (num <= 16) {
			e = new Phoenix(pos, id);
			this->enemies.push_back(e);
		} else if (num <= 18) {
			e = new Merchant(pos, id);
			this->enemies.push_back(e);
		}
		displayGrid[pos.y - 1][pos.x - 1] = e->getSymbol();
	}
}

void Floor::generateItems(int &dragons) {
	for (int j = 0; j < 10; j++) {
		int num = rand() % 6 + 1;
		int id;
		Posn pos;
		generatePosition(id, pos);
		Item *i;
		if (num == 1) {
			i = new Potion(pos, "RH");
			this->items.push_back(i);
		} else if (num == 2) {
			i = new Potion(pos, "BA");
			this->items.push_back(i);
		} else if (num == 3) {
			i = new Potion(pos, "BD");
			this->items.push_back(i);
		} else if (num == 4) {
			i = new Potion(pos, "PH");
			this->items.push_back(i);
		} else if (num == 5) {
			i = new Potion(pos, "WA");
			this->items.push_back(i);
		} else if (num == 6) {
			i = new Potion(pos, "WD");
			this->items.push_back(i);
		}
		displayGrid[pos.y - 1][pos.x - 1] = i->getSymbol();
	}
	for (int j = 0; j < 6; j++) {
		int num = rand() % 8 + 1;
		int id;
		Posn pos;
		generatePosition(id, pos);
		Item *i;
		if (num <= 5) {
			i = new normalHoard(pos);
			this->items.push_back(i);
		} else if (num <= 7) {
			i = new smallHoard(pos);
			this->items.push_back(i);
		} else if (num == 8) {
			dragonHoard *dh = new dragonHoard(pos, NULL);
			i = dh;
			this->items.push_back(i);
			Posn dpos = dragonPosition(pos);
			Enemy *e = new Dragon(dpos, dh, NULL, id);
			dh->setDragon(e);
			this->enemies.push_back(e);
			++dragons;
			displayGrid[dpos.y - 1][dpos.x - 1] = e->getSymbol();
		}
		displayGrid[pos.y - 1][pos.x - 1] = i->getSymbol();
	}
	int num = rand() % (6 - this->level);
	if (num == 0 && !this->barrierSpawned) {
		int id;
		Posn pos;
		generatePosition(id, pos);
		this->suit = new barrierSuit(pos, NULL);
		this->barrierSpawned = true;
		displayGrid[pos.y - 1][pos.x - 1] = this->suit->getSymbol();
		Posn dpos = dragonPosition(pos);
		Enemy *e = new Dragon(dpos, NULL, this->suit, id);
		this->suit->setDragon(e);
		this->enemies.push_back(e);
		++dragons;
		displayGrid[dpos.y - 1][dpos.x - 1] = e->getSymbol();
	}
}

bool Floor::validMove(Posn pos) {
	char t = this->displayGrid[pos.y - 1][pos.x - 1];
	return t == '.' || t == '+' || t == '#';
}

bool Floor::validTile(Posn pos) {
	return this->displayGrid[pos.y - 1][pos.x - 1] == '.';
}

void Floor::setVisible(bool visible) {
	this->stairVisible = visible;
}

void Floor::setLevel(int level) {
	this->level = level;
}

void Floor::setWon(bool won) {
	this->won = won;
}

void Floor::setHostile(bool hostile) {
	this->merchantHostile = hostile;
}

void Floor::setSpawned(bool spawned) {
	this->barrierSpawned = spawned;
}

bool Floor::getVisible() {
	return this->stairVisible;
}

int Floor::getLevel() {
	return this->level;
}

bool Floor::getWon() {
	return this->won;
}

bool Floor::getHostile() {
	return this->merchantHostile;
}

bool Floor::getSpawned() {
	return this->barrierSpawned;
}