#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <stdexcept>

using namespace std;

// ввод числа с проверкой
int readInt(string text, int minV, int maxV) {
    while (true) {
        cout << text;
        string s;
        if (!getline(cin, s)) {
            cout << "\nВвод закрыт, выход.\n";
            exit(0);
        }
        try {
            size_t pos = 0;
            int x = stoi(s, &pos);
            if (pos != s.size() || x < minV || x > maxV) {
                throw out_of_range("неверное число");
            }
            return x;
        }
        catch (exception&) {
            cout << "Ошибка! Нужно ввести число от " << minV << " до " << maxV << ".\n";
        }
    }
}

// ввод имени с проверкой
string readName() {
    while (true) {
        cout << "Имя героя: ";
        string s;
        if (!getline(cin, s)) {
            cout << "\nВвод закрыт, выход.\n";
            exit(0);
        }
        try {
            if (s.empty()) {
                throw runtime_error("имя не может быть пустым");
            }
            if (s.size() > 40) {
                throw runtime_error("слишком длинное имя");
            }
            return s;
        }
        catch (exception& e) {
            cout << "Ошибка: " << e.what() << "\n";
        }
    }
}

// базовый класс героя
class Character {
protected:
    string name;
    int hp;
    int maxHp;
    int attack;
    int defense;
    bool defending;

public:
    Character(string name, int hp, int attack, int defense) {
        this->name = name;
        this->hp = hp;
        this->maxHp = hp;
        this->attack = attack;
        this->defense = defense;
        defending = false;
    }

    virtual ~Character() {}

    virtual string getType() = 0;   // название класса
    virtual int calcDamage() = 0;   // урон атаки
    virtual bool ignoresDefense() { return false; }

    // геттеры
    string getName() { return name; }
    int getHp() { return hp; }
    int getMaxHp() { return maxHp; }
    int getAttack() { return attack; }
    int getDefense() { return defense; }

    // сеттеры (для редактирования)
    void setName(string n) { name = n; }
    void setHp(int h) { maxHp = h; hp = h; }
    void setAttack(int a) { attack = a; }
    void setDefense(int d) { defense = d; }

    bool isAlive() { return hp > 0; }

    // перед боем
    void resetHp() {
        hp = maxHp;
        defending = false;
    }

    // начало хода
    void startTurn() {
        defending = false;
    }

    // защита
    void defend() {
        defending = true;
        cout << name << " встал в защиту.\n";
    }

    // получение урона
    int takeDamage(int dmg, bool ignoreDef) {
        if (!ignoreDef) {
            dmg -= defense;
        }
        if (defending) {
            dmg /= 2;
        }
        if (dmg < 1) {
            dmg = 1;
        }
        hp -= dmg;
        if (hp < 0) {
            hp = 0;
        }
        return dmg;
    }

    // атака цели
    void hit(Character& target) {
        int dmg = calcDamage();
        int real = target.takeDamage(dmg, ignoresDefense());
        cout << name << " атакует " << target.getName() << " и наносит " << real << " урона.\n";
    }

    // вывод информации
    void printInfo() {
        cout << name << " (" << getType() << ") | HP: " << maxHp
             << " | Атака: " << attack << " | Защита: " << defense << "\n";
    }
};

// лучник
class Archer : public Character {
public:
    Archer(string name, int hp, int attack, int defense) : Character(name, hp, attack, defense) {}

    string getType() { return "Лучник"; }

    int calcDamage() {
        int dmg = attack + rand() % 5 - 2;  // разброс -2..+2
        if (rand() % 100 < 25) {            // шанс крита 25%
            cout << "Критический выстрел!\n";
            dmg *= 2;
        }
        return dmg;
    }
};

// воин
class Warrior : public Character {
public:
    Warrior(string name, int hp, int attack, int defense) : Character(name, hp, attack, defense) {}

    string getType() { return "Воин"; }

    int calcDamage() {
        return attack + rand() % 5;  // стабильный урон
    }
};

// маг
class Mage : public Character {
public:
    Mage(string name, int hp, int attack, int defense) : Character(name, hp, attack, defense) {}

    string getType() { return "Маг"; }

    int calcDamage() {
        return attack + rand() % 6;
    }

    // магия игнорирует броню
    bool ignoresDefense() { return true; }
};

// арена (хранит героев и бои)
class Arena {
private:
    vector<Character*> heroes;

public:
    // начальные данные
    Arena() {
        heroes.push_back(new Archer("Робин", 90, 18, 4));
        heroes.push_back(new Warrior("Конан", 130, 15, 8));
        heroes.push_back(new Mage("Мерлин", 80, 20, 2));
        heroes.push_back(new Warrior("Рагнар", 120, 16, 7));
    }

    // чистим память
    ~Arena() {
        for (int i = 0; i < (int)heroes.size(); i++) {
            delete heroes[i];
        }
    }

    // список героев
    void showHeroes() {
        cout << "\nСписок героев:\n";
        for (int i = 0; i < (int)heroes.size(); i++) {
            cout << i + 1 << ". ";
            heroes[i]->printInfo();
        }
    }

    // выбор героя (0 - назад)
    int chooseHero(string text) {
        return readInt(text, 0, (int)heroes.size()) - 1;
    }

    // создание героя
    void createHero() {
        cout << "\nКакой класс создать?\n";
        cout << "1 - Лучник\n2 - Воин\n3 - Маг\n0 - Назад\n";
        int type = readInt("Выбор: ", 0, 3);
        if (type == 0) {
            return;
        }

        string name = readName();
        int hp = readInt("HP (10-500): ", 10, 500);
        int attack = readInt("Атака (1-100): ", 1, 100);
        int defense = readInt("Защита (0-50): ", 0, 50);

        if (type == 1) {
            heroes.push_back(new Archer(name, hp, attack, defense));
        }
        else if (type == 2) {
            heroes.push_back(new Warrior(name, hp, attack, defense));
        }
        else {
            heroes.push_back(new Mage(name, hp, attack, defense));
        }
        cout << "Герой создан!\n";
    }

    // изменение героя
    void editHero() {
        showHeroes();
        int i = chooseHero("Номер героя для изменения (0 - назад): ");
        if (i == -1) {
            return;
        }

        cout << "\nЧто изменить?\n";
        cout << "1 - Имя\n2 - HP\n3 - Атака\n4 - Защита\n0 - Назад\n";
        int field = readInt("Выбор: ", 0, 4);

        if (field == 1) {
            heroes[i]->setName(readName());
        }
        else if (field == 2) {
            heroes[i]->setHp(readInt("Новое HP (10-500): ", 10, 500));
        }
        else if (field == 3) {
            heroes[i]->setAttack(readInt("Новая атака (1-100): ", 1, 100));
        }
        else if (field == 4) {
            heroes[i]->setDefense(readInt("Новая защита (0-50): ", 0, 50));
        }
        else {
            return;
        }
        cout << "Герой изменён!\n";
    }

    // удаление героя
    void deleteHero() {
        if (heroes.size() <= 2) {
            cout << "\nНельзя удалить, для боя нужно минимум 2 героя!\n";
            return;
        }
        showHeroes();
        int i = chooseHero("Номер героя для удаления (0 - назад): ");
        if (i == -1) {
            return;
        }
        delete heroes[i];
        heroes.erase(heroes.begin() + i);
        cout << "Герой удалён!\n";
    }

    // бой, true - игрок выиграл
    bool fight(Character& player, Character& enemy) {
        player.resetHp();
        enemy.resetHp();
        int round = 1;

        cout << "\n===== " << player.getName() << " VS " << enemy.getName() << " =====\n";

        while (player.isAlive() && enemy.isAlive()) {
            cout << "\n--- Раунд " << round << " ---\n";
            cout << player.getName() << ": " << player.getHp() << "/" << player.getMaxHp() << " HP\n";
            cout << enemy.getName() << ": " << enemy.getHp() << "/" << enemy.getMaxHp() << " HP\n";

            // ход игрока
            player.startTurn();
            cout << "\n1 - Атака\n2 - Защита\n";
            int action = readInt("Ваше действие: ", 1, 2);
            if (action == 1) {
                player.hit(enemy);
            }
            else {
                player.defend();
            }

            if (!enemy.isAlive()) {
                break;
            }

            // ход соперника
            enemy.startTurn();
            if (rand() % 100 < 25) {
                enemy.defend();
            }
            else {
                enemy.hit(player);
            }

            round++;
        }

        return player.isAlive();
    }

    // запуск игры, false - игрок проиграл
    bool startGame() {
        if (heroes.size() < 2) {
            cout << "\nНужно минимум 2 героя, создайте ещё!\n";
            return true;
        }

        showHeroes();
        int p = chooseHero("\nВыберите своего героя (0 - назад): ");
        if (p == -1) {
            return true;
        }

        cout << "\nВыбор соперника:";
        showHeroes();
        int e;
        while (true) {
            e = chooseHero("\nВыберите соперника (0 - назад): ");
            if (e == -1) {
                return true;
            }
            if (e == p) {
                cout << "Нельзя драться с самим собой!\n";
            }
            else {
                break;
            }
        }

        bool win = fight(*heroes[p], *heroes[e]);

        if (win) {
            cout << "\nПОБЕДА! Вы выиграли бой, " << heroes[e]->getName() << " повержен!\n";
            return true;
        }
        cout << "\nПОРАЖЕНИЕ! Вы проиграли бой.\nИгра окончена!\n";
        return false;
    }
};

int main() {
    srand((unsigned)time(0));

    try {
        Arena arena;
        bool play = true;

        cout << "===== АРЕНА ГЕРОЕВ =====\n";

        while (play) {
            cout << "\n--- Главное меню ---\n";
            cout << "1 - Выбрать героя и начать бой\n";
            cout << "2 - Список героев\n";
            cout << "3 - Создать героя\n";
            cout << "4 - Изменить героя\n";
            cout << "5 - Удалить героя\n";
            cout << "0 - Выход\n";

            int choice = readInt("Ваш выбор: ", 0, 5);

            switch (choice) {
            case 1:
                if (!arena.startGame()) {
                    play = false;  // проиграли - конец
                }
                break;
            case 2:
                arena.showHeroes();
                break;
            case 3:
                arena.createHero();
                break;
            case 4:
                arena.editHero();
                break;
            case 5:
                arena.deleteHero();
                break;
            case 0:
                play = false;
                break;
            }
        }
    }
    catch (exception& e) {
        cout << "Критическая ошибка: " << e.what() << "\n";
    }

    cout << "\nСпасибо за игру!\n";
    return 0;
}
