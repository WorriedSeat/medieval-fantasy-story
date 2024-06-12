// including all libraries
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <sstream>
#include <bits/stdc++.h>

using namespace std;

class Potion;

//Accessing the input and output files
ifstream fin("input.txt");
ofstream fout("output.txt");

//class Narrator
class Narrator{
public:
    Narrator() = default;

    void say(int amountOfWords, vector<string> &text){
        fout << "Narrator: ";
        for (int i = amountOfWords-1; i >= 0; --i) {
            fout << text.at(text.size()-i-1) << " ";
        }
        fout << endl;
    };
};


// abstract class Character which will be inherited by all heroes of fantasy story
class Character
{
protected:
    int healthPoints;                              // All characters will have health and name
    string name;
public:
    Character(string &name, int healthPoints){     // ctor of the Character
        this->name = name;
        this->healthPoints = healthPoints;
    }

    ~Character() = default;                       //default dtor of the Character

// The following 2 methods are getters of name, healthPoints of Character
    string getName(){
        return this->name;
    }

    int getHP() const{
        return this->healthPoints;
    }

//  All characters are able to lose(takeDamage) and increase(heal) their healthPoints
    void takeDamage(int damage){
        healthPoints -= damage;
    }

    void heal(int healValue){
        healthPoints += healValue;
    }

//  All characters are able to talk and die(
    void say(int amountOfWords, vector<string> &text){
        fout << this->name << ": ";
        for (int i = amountOfWords-1; i >= 0; --i) {
            fout << text.at(text.size()-i-1) << " ";
        }
        fout << endl;
    };
    virtual void death() = 0;
    virtual bool findPotion(string &potion) = 0;
    virtual Potion getPotion(string &potion) = 0;
    virtual void drink(shared_ptr<Character> supplier, shared_ptr<Character> drinker, string &potionName) = 0;
};


//abstract class PhysicalItem which will be inherited by all item of fantasy story
class PhysicalItem{
protected:
    string name;                        //all items have name
public:
    explicit PhysicalItem(string &name){           // ctor of PhysicalItem
        this->name = name;
    }

    ~PhysicalItem() = default;                   // default dtor of PhysicalItem

    string getName(){                           //getter of the name if the item
        return this->name;
    }

    virtual void use(shared_ptr<Character> target) = 0;         //all items are usable
    virtual int getValue() = 0;                                 //method that will access the value of the object
};


//class Container that will contain objects of any type
template<typename T>
class Container{};
template<typename T> concept DerivedFromPhysicalItem = is_base_of<PhysicalItem, T>::value;

//class Container that will contain objects which are derived from PhysicalItem
template<DerivedFromPhysicalItem  T>class Container<T>{
private:
    map<string, T> elements;
    int maxCapacity{};
public:
    Container() = default;                  //default ctor of the Container

    explicit Container(int size){           //ctor of the Container
        maxCapacity = size;
    }

    ~Container() = default;                 //default dtor

    bool isFull(){                                             //method that will check if our container is full
        return this->elements.size() == maxCapacity;
    }

    void addItem(T item){                                //method that will add item to the container if it is not full
        if(!this->isFull() && !elements.contains(item.getName())) {
            elements.insert({item.getName(), item});
        }
        else{
            fout << "Error caught" << endl;
        }
    }


    T getItem(string &name){                    //method that will get item from the container
        return elements.at(name);
    }

    bool find(string &item){                    //method that will check if the item is present in the container
        return elements.contains(item);
    }

    void showWithValue(){                       //method that will output the elements of hte container
        vector<string> names;
        for (const auto &item: elements){
            names.push_back(item.first);
        }
        sort(names.begin(), names.end());
        for (string &name: names) {
            fout << name << ":" << elements.at(name).getValue() << " ";
        }
        fout << endl;
    }

    void removeItem(string &item){          //method that will remove the item
        elements.erase(item);
    }

    void deleteAll(){                        //method that will clear the container
        elements.clear();
    }

};


//class weapon derived from PhysicalItem
class Weapon: public PhysicalItem{
private:
    int damage;                           //all weapons have damage value
public:
    Weapon(string &name, int damage): PhysicalItem(name){          //weapon ctor by the name and its damage value
        this->damage = damage;
    }

    ~Weapon() = default;                  //default dtor of the weapon

    int getValue() override{              //override method that will get the damage value of weapon
        return damage;
    }

    void use(shared_ptr<Character> target) override{      //override method that will use the weapon
        target->takeDamage(damage);
    }
};
//class Potion derived from PhysicalItem
class Potion: public PhysicalItem
{
private:
    int heal;
public:
    Potion(string &name, int healValue): PhysicalItem(name){          //ctor of the Potion by name and healValue
        this->heal = healValue;
    }

    ~Potion() = default;                       //default dtor of the potion

    int getValue() override{                   //getter of the healValue
        return heal;
    }

    void use(shared_ptr<Character> target) override{        //method that will use the potion
        target->heal(heal);
    }
};
//class Spell derived from PhysicalItem
class Spell: public PhysicalItem
{
private:
    map<string, shared_ptr<Character>> canBeCasted;      //contains all possible targets of the spell
public:
    Spell(string &name, map<string, shared_ptr<Character>> &paper): PhysicalItem(name){ //ctor of the spell by its name and map of possible targets
        this->canBeCasted = paper;
    }
    explicit Spell(string &name) : PhysicalItem(name){}     //ctor of the spell by its name

    ~Spell() = default;            //default dtor of the potion

    int getValue() override{           //method that will return the amount of the targets
        return canBeCasted.size();
    }

    map<string, shared_ptr<Character>> getList(){        //method that will return all targets
        return this->canBeCasted;
    }

    void use(shared_ptr<Character> target) override{      //method that will use the spell
        target->death();
    }
};


// WeaponUser class for characters that can use weapons
class WeaponUser{
protected:
    Container<Weapon> arsenal;          //container for the weapons
    explicit WeaponUser(int capacity)
    {
        arsenal = Container<Weapon>(capacity);
    }

    virtual void attack(shared_ptr<Character> target, string &weaponName) = 0; //method that will attack another characters
};
//PotionUser class for characters that can use weapons
class PotionUser{
protected:
    Container<Potion> medicalBag;     //container for the potions
    explicit PotionUser(int capacity)
    {
        medicalBag = Container<Potion>(capacity);
    }

    virtual void drink(shared_ptr<Character> supplier, shared_ptr<Character> drinker, string &potionName) = 0;  //method that will drink the potion
};
//SpellUser class for characters that can use the spells
class SpellUser{
protected:
    Container<Spell> spellBook;          //container for the spells
    explicit SpellUser(const int &capacity){
        spellBook = Container<Spell>(capacity);
    }

    virtual bool castSpell(shared_ptr<Character> target, string &spellName) = 0;     //method that will use the spell
};


//Fighter class derived from the Character
class Fighter: public WeaponUser, public PotionUser, public Character{
public:
    //ctor of the Fighter
    Fighter(string &name, int HP): Character(name, HP), WeaponUser(3), PotionUser(5){
        fout << "A new fighter came to town, " << this->name << "." << endl;
    }

    //method to obtain an item
    void obtainItem(shared_ptr<PhysicalItem> &item){
        if(typeid(*item) == typeid(Weapon)){
            if(!arsenal.isFull()){
                shared_ptr<Weapon> weapon = dynamic_pointer_cast<Weapon>(item);
                arsenal.addItem(*weapon);
                fout << name << " just obtained a new weapon called " << weapon->getName() << "." << endl;
            }else
                fout << "Error caught" << endl;
        }else if(typeid(*item) == typeid(Potion)){
            if(!medicalBag.isFull()){
                shared_ptr<Potion> potion = dynamic_pointer_cast<Potion>(item);
                medicalBag.addItem(*potion);
                fout << name << " just obtained a new potion called " << potion->getName() << "." << endl;
            }else
                fout << "Error caught" << endl;
        }else{
            fout << "Error caught" << endl;

        }
    }

    //methods to show the weapons and potions
    void showWeapons(){
        arsenal.showWithValue();
    }

    void showPotions(){
        medicalBag.showWithValue();
    }

    //methods to find and get the Potion from the medicalBag
    bool findPotion(string &potion) override{
        return medicalBag.find(potion);
    }

    Potion getPotion(string &potion) override{
        Potion potion_ = medicalBag.getItem(potion);
        medicalBag.removeItem(potion);
        return potion_;
    }

    //method that will attack the character with some weapon
    void attack(shared_ptr<Character> target, string &weaponName) override{
        if(!arsenal.find(weaponName)){
            fout << "Error caught" << endl;
        }else{
            arsenal.getItem(weaponName).use(target);
            fout << name << " attacks " << target->getName() << " with their " << weaponName << "!" << endl;
        }
    }
    //method that will drink the potion from some character
    void drink(shared_ptr<Character> supplier, shared_ptr<Character> drinker, string &potionName) override{
        if(supplier->findPotion(potionName)){
            supplier->getPotion(potionName).use(drinker);
            fout << name << " drinks " << potionName << " from " << supplier->getName() << "." << endl;
        }else
            fout << "Error caught" << endl;
    }
    //method to delete all items of the fighter
    void death() override{
        fout << name << " has died..." << endl;
        arsenal.deleteAll();
        medicalBag.deleteAll();
    }
};
//Wizard class derived from the Character
class Wizard: public PotionUser, public SpellUser, public Character{
public:
    //ctor of the Wizard
    Wizard(string &name, int HP) : Character(name, HP), PotionUser(10), SpellUser(10) {
        fout << "A new wizard came to town, " << this->name << "." << endl;
    }

    //method to obtain an item
    void obtainItem(shared_ptr<PhysicalItem> &item){
        if(typeid(*item) == typeid(Spell)){
            if(!spellBook.isFull()){
                shared_ptr<Spell> spell = dynamic_pointer_cast<Spell>(item);
                spellBook.addItem(*spell);
                fout << name << " just obtained a new spell called " << spell->getName() << "." << endl;
            }else
                fout << "Error caught" << endl;
        }else if(typeid(*item) == typeid(Potion)){
            if(!medicalBag.isFull()){
                shared_ptr<Potion> potion = dynamic_pointer_cast<Potion>(item);
                medicalBag.addItem(*potion);
                fout << name << " just obtained a new potion called " << potion->getName() << "." << endl;
            }else
                fout << "Error caught" << endl;
        }else{
            fout << "Error caught" << endl;
        }
    }

    //methods to show the spells and potions
    void showSpells(){
        spellBook.showWithValue();
    }

    void showPotions(){
        medicalBag.showWithValue();
    }

    //methods to find and get the Potion from the medicalBag
    bool findPotion(string &potion) override{
        return medicalBag.find(potion);
    }

    Potion getPotion(string &potion) override{
        Potion potion_ = medicalBag.getItem(potion);
        medicalBag.removeItem(potion);
        return potion_;
    }

    //method that will cast spell on the character
    bool castSpell(shared_ptr<Character> target, string &spellName) override{
        if(spellBook.find(spellName)){
            if(spellBook.getItem(spellName).getList().contains(target->getName())) {
                fout << name << " casts " << spellName << " on " << target->getName() << "!" << endl;
                spellBook.getItem(spellName).use(target);
                spellBook.removeItem(spellName);
                return true;
            }else
                fout << "Error caught" << endl;
        }else
            fout << "Error caught" << endl;
        return false;
    }

    //method that will drink the potion from some character
    void drink(shared_ptr<Character> supplier, shared_ptr<Character> drinker, string &potionName) override{
        if(supplier->findPotion(potionName)){
            supplier->getPotion(potionName).use(drinker);
            fout << name << " drinks " << potionName << " from " << supplier->getName() << "." << endl;
        }else
            fout << "Error caught" << endl;
    }

    //method to delete all items of the wizard
    void death() override{
        fout << name << " has died..." << endl;
        medicalBag.deleteAll();
        spellBook.deleteAll();
    }
};

//Archer class derived from the Character
class Archer: public WeaponUser, public PotionUser, public SpellUser, public Character{
public:
    //ctor of the Archer
    Archer(string &name, int HP) : Character(name, HP), PotionUser(3), WeaponUser(2), SpellUser(2){
        fout << "A new archer came to town, " << this->name << "." << endl;
    }

    //method to obtain an item
    void obtainItem(shared_ptr<PhysicalItem> &item){
        if(typeid(*item) == typeid(Spell)){
            if(!spellBook.isFull()) {
                shared_ptr<Spell> spell = dynamic_pointer_cast<Spell>(item);
                spellBook.addItem(*spell);
                fout << name << " just obtained a new spell called " << spell->getName() << "." << endl;
            }else
                fout << "Error caught" << endl;
        }else if(typeid(*item) == typeid(Potion)){
            if(!medicalBag.isFull()) {
                shared_ptr<Potion> potion = dynamic_pointer_cast<Potion>(item);
                medicalBag.addItem(*potion);
                fout << name << " just obtained a new potion called " << potion->getName() << "." << endl;
            }else
                fout << "Error caught" << endl;
        }else if(typeid(*item) == typeid(Weapon)){
            if(!arsenal.isFull()) {
                shared_ptr<Weapon> weapon = dynamic_pointer_cast<Weapon>(item);
                arsenal.addItem(*weapon);
                fout << name << " just obtained a new weapon called " << weapon->getName() << "." << endl;
            }else
                fout << "Error caught" << endl;
        }
    }

    //methods to show the weapons, potions and spells
    void showSpells(){
        spellBook.showWithValue();
    }

    void showPotions(){
        medicalBag.showWithValue();
    }

    void showWeapons(){
        arsenal.showWithValue();
    }

    //method to find and get the Potion from the medicalBag
    bool findPotion(string &potion) override{
        return medicalBag.find(potion);
    }

    Potion getPotion(string &potion) override{
        Potion potion_ = medicalBag.getItem(potion);
        medicalBag.removeItem(potion);
        return potion_;
    }

    //method that will attack the character with some weapon
    void attack(shared_ptr<Character> target, string &weaponName) override{
        if(!arsenal.find(weaponName)){
            fout << "Error caught" << endl;
        }else{
            arsenal.getItem(weaponName).use(target);
            fout << name << " attacks " << target->getName() << " with their " << weaponName << "!" << endl;
        }
    }
    //method that will cast spell on the character
    bool castSpell(shared_ptr<Character> target, string &spellName) override{
        if(spellBook.find(spellName)){
            if(spellBook.getItem(spellName).getList().contains(target->getName())) {
                fout << name << " casts " << spellName << " on " << target->getName() << "!" << endl;
                spellBook.getItem(spellName).use(target);
                spellBook.removeItem(spellName);
                return true;
            }else
                fout << "Error caught" << endl;
        }else
            fout << "Error caught" << endl;
        return false;
    }

    //method that will drink the potion from some character
    void drink(shared_ptr<Character> supplier, shared_ptr<Character> drinker, string &potionName) override{
        if(supplier->findPotion(potionName)){
            supplier->getPotion(potionName).use(drinker);
            fout << name << " drinks " << potionName << " from " << supplier->getName() << "." << endl;
        }else
            fout << "Error caught" << endl;
    }

    //method to delete all items of the archer
    void death() override{
        fout << name << " has died..." << endl;
        arsenal.deleteAll();
        medicalBag.deleteAll();
        spellBook.deleteAll();
    }
};

//method that will split the string line into the words
vector<string> textToWords(string &text){
    stringstream  ss(text);
    vector<string> words;
    string word;
    while(getline(ss, word, ' ')){
        words.push_back(word);
    }
    return words;
}

int main(){
    int numOfEvents;
    fin >> numOfEvents;
    map<string, shared_ptr<Character>> allCharacters;
    Narrator narrator = Narrator();
    fin.ignore();
    //algorithm that will accept the command, check requirements and do the commands if no error caught
    for (int i = 0; i < numOfEvents; ++i) {
        string event;
        getline(fin, event);
        vector<string> eventWords = textToWords(event);
        string command = eventWords.at(0);
        //if statement that handle the creation of the characters of items
        if(command == "Create"){
            string createWhat = eventWords.at(1);
            if (createWhat == "character") {
                if(stoi(eventWords.at(4)) > 0){
                    string createWhatType = eventWords.at(2);
                    shared_ptr<Character> character = nullptr;
                    if(createWhatType == "fighter"){
                        character = make_shared<Fighter>(eventWords.at(3), stoi(eventWords.at(4)));
                    }
                    else if(createWhatType == "wizard"){
                        character = make_shared<Wizard>(eventWords.at(3), stoi(eventWords.at(4)));
                    }
                    else if(createWhatType == "archer"){
                        character = make_shared<Archer>(eventWords.at(3), stoi(eventWords.at(4)));
                    }
                    allCharacters.insert(make_pair(eventWords.at(3),character));
                }else
                    fout << "Error caught" << endl;
            }else if (createWhat == "item") {
                string createWhatItem = eventWords.at(2);
                if (allCharacters.contains(eventWords.at(3))) {
                    if (createWhatItem == "spell") {
                        if (stoi(eventWords.at(5)) == 0){
                            shared_ptr<PhysicalItem> spell = make_shared<Spell>(eventWords.at(4));
                            if(dynamic_pointer_cast<Wizard>(allCharacters.at(eventWords.at(3))) != nullptr){
                                shared_ptr<Wizard> wizard = dynamic_pointer_cast<Wizard>(allCharacters.at(eventWords.at(3)));
                                wizard->obtainItem(spell);
                            }else if (dynamic_pointer_cast<Archer>(allCharacters.at(eventWords.at(3))) != nullptr) {
                                shared_ptr<Archer> archer = dynamic_pointer_cast<Archer>(allCharacters.at(eventWords.at(3)));
                                archer->obtainItem(spell);
                            }else
                                fout << "Error caught" << endl;
                        }else{
                            bool flag = false;
                            for (int j = 1; j <= stoi(eventWords.at(5)); ++j){
                                if(!allCharacters.contains(eventWords.at(5 + j))){
                                    fout << "Error caught" << endl;
                                    flag = true;
                                    break;
                                }
                            }
                            if(!flag){
                                map<string, shared_ptr<Character>> targets;
                                for (int j = 1; j <= stoi(eventWords.at(5)); ++j){
                                    targets.insert(make_pair(eventWords.at(5 + j), allCharacters.at(eventWords.at(5 + j))));
                                }
                                shared_ptr<PhysicalItem> spell = make_shared<Spell>(eventWords.at(4), targets);
                                if(dynamic_pointer_cast<Wizard>(allCharacters.at(eventWords.at(3))) != nullptr){
                                    shared_ptr<Wizard> wizard = dynamic_pointer_cast<Wizard>(allCharacters.at(eventWords.at(3)));
                                    wizard->obtainItem(spell);
                                }else if (dynamic_pointer_cast<Archer>(allCharacters.at(eventWords.at(3))) != nullptr) {
                                    shared_ptr<Archer> archer = dynamic_pointer_cast<Archer>(allCharacters.at(eventWords.at(3)));
                                    archer->obtainItem(spell);
                                }else
                                    fout << "Error caught" << endl;
                            }
                        }
                    }
                    else if (stoi(eventWords.at(5)) > 0) {
                        if (createWhatItem == "potion") {
                            shared_ptr<PhysicalItem> potion = make_shared<Potion>(eventWords.at(4), stoi(eventWords.at(5)));
                            if (dynamic_pointer_cast<Fighter>(allCharacters.at(eventWords.at(3))) != nullptr) {
                                shared_ptr<Fighter> fighter = dynamic_pointer_cast<Fighter>(allCharacters.at(eventWords.at(3)));
                                fighter->obtainItem(potion);
                            }
                            else if (dynamic_pointer_cast<Archer>(allCharacters.at(eventWords.at(3))) != nullptr) {
                                shared_ptr<Archer> archer = dynamic_pointer_cast<Archer>(allCharacters.at(eventWords.at(3)));
                                archer->obtainItem(potion);
                            }
                            else if (dynamic_pointer_cast<Wizard>(allCharacters.at(eventWords.at(3))) != nullptr) {
                                shared_ptr<Wizard> wizard = dynamic_pointer_cast<Wizard>(allCharacters.at(eventWords.at(3)));
                                wizard->obtainItem(potion);
                            }
                        }
                        else if (createWhatItem == "weapon") {
                            shared_ptr<PhysicalItem> weapon = make_shared<Weapon>(eventWords.at(4), stoi(eventWords.at(5)));
                            if (dynamic_pointer_cast<Fighter>(allCharacters.at(eventWords.at(3))) != nullptr) {
                                shared_ptr<Fighter> fighter = dynamic_pointer_cast<Fighter>(allCharacters.at(eventWords.at(3)));
                                fighter->obtainItem(weapon);
                            }
                            else if (dynamic_pointer_cast<Archer>(allCharacters.at(eventWords.at(3))) != nullptr) {
                                shared_ptr<Archer> archer = dynamic_pointer_cast<Archer>(allCharacters.at(eventWords.at(3)));
                                archer->obtainItem(weapon);
                            }else
                                fout << "Error caught" << endl;
                        }
                    }else
                        fout << "Error caught" << endl;
                }else
                    fout << "Error caught" << endl;
            }
        }
        //if statement that handle the attack of one character to another
        else if(command == "Attack"){
            string attacker = eventWords.at(1);
            string target = eventWords.at(2);
            string weapon = eventWords.at(3);
            if(allCharacters.contains(attacker) && allCharacters.contains(target)){
                if (dynamic_pointer_cast<Fighter>(allCharacters.at(attacker)) != nullptr) {
                    shared_ptr<Fighter> fighter = dynamic_pointer_cast<Fighter>(allCharacters.at(attacker));
                    fighter->attack(allCharacters.at(target), weapon);
                    if (allCharacters.at(target)->getHP() < 1){
                        allCharacters.at(target)->death();
                        allCharacters.erase(target);
                    }
                }
                else if (dynamic_pointer_cast<Archer>(allCharacters.at(attacker)) != nullptr) {
                    shared_ptr<Archer> archer = dynamic_pointer_cast<Archer>(allCharacters.at(attacker));
                    archer->attack(allCharacters.at(target), weapon);
                    if (allCharacters.at(target)->getHP() < 1){
                        allCharacters.at(target)->death();
                        allCharacters.erase(target);
                    }
                }else
                    fout << "Error caught" << endl;
            }else
                fout << "Error caught" << endl;
        }
        //if statement that handle the cast of the spell to character
        else if(command == "Cast"){
            string user = eventWords.at(1);
            string target = eventWords.at(2);
            string spell = eventWords.at(3);
            if(allCharacters.contains(user) && allCharacters.contains(target)){
                if (dynamic_pointer_cast<Wizard>(allCharacters.at(user)) != nullptr) {
                    shared_ptr<Wizard> wizard = dynamic_pointer_cast<Wizard>(allCharacters.at(user));
                    if(wizard->castSpell(allCharacters.at(target), spell))
                        allCharacters.erase(target);
                }
                else if (dynamic_pointer_cast<Archer>(allCharacters.at(user)) != nullptr) {
                    shared_ptr<Archer> archer = dynamic_pointer_cast<Archer>(allCharacters.at(user));
                    if(archer->castSpell(allCharacters.at(target), spell))
                        allCharacters.erase(target);
                }else
                    fout << "Error caught" << endl;
            }else
                fout << "Error caught" << endl;
        }
        //if statement that handle the drink of the potion by some character
        else if(command == "Drink"){
            string supplier = eventWords.at(1);
            string drinker = eventWords.at(2);
            string potion = eventWords.at(3);
            if(allCharacters.contains(drinker) && allCharacters.contains(supplier)){
                allCharacters.at(drinker)->drink(allCharacters.at(supplier), allCharacters.at(drinker), potion);
            }else
                fout << "Error caught" << endl;
        }
        //if statement that handle the dialogue of the characters or narrator
        else if(command == "Dialogue"){
            if(eventWords.at(1) == "Narrator"){
                narrator.say(stoi(eventWords.at(2)), eventWords);
            }else if(allCharacters.contains(eventWords.at(1))){
                allCharacters.at(eventWords.at(1))->say(stoi(eventWords.at(2)), eventWords);
            }else
                fout << "Error caught" << endl;
        }
        //if statement that handle the show of characters or items obtained by some character
        else if(command == "Show"){
            string showWhat = eventWords.at(1);
            if(showWhat == "characters"){
                if(allCharacters.empty()){
                    fout << endl;
                }else{
                    vector<string> names;
                    for (const auto &item: allCharacters){
                        names.push_back(item.first);
                    }
                    sort(names.begin(), names.end());
                    for (string &name : names) {
                        if (dynamic_pointer_cast<Fighter>(allCharacters.at(name)) != nullptr) {
                            fout << name << ":fighter:" << allCharacters.at(name)->getHP() << " ";
                        }
                        else if (dynamic_pointer_cast<Archer>(allCharacters.at(name)) != nullptr) {
                            fout << name << ":archer:" << allCharacters.at(name)->getHP() << " ";
                        }
                        else if (dynamic_pointer_cast<Wizard>(allCharacters.at(name)) != nullptr) {
                            fout << name << ":wizard:" << allCharacters.at(name)->getHP() << " ";
                        }
                    }
                    fout << endl;
                }
            }else if(showWhat == "weapons"){
                string name = eventWords.at(2);
                if(allCharacters.contains(name)){
                    if (dynamic_pointer_cast<Fighter>(allCharacters.at(name)) != nullptr) {
                        shared_ptr<Fighter> fighter = dynamic_pointer_cast<Fighter>(allCharacters.at(name));
                        fighter->showWeapons();
                    }
                    else if (dynamic_pointer_cast<Archer>(allCharacters.at(name)) != nullptr) {
                        shared_ptr<Archer> archer = dynamic_pointer_cast<Archer>(allCharacters.at(name));
                        archer->showWeapons();
                    }else
                        fout << "Error caught" << endl;
                }else
                    fout << "Error caught" << endl;
            }else if(showWhat == "potions"){
                string name = eventWords.at(2);
                if(allCharacters.contains(name)){
                    if (dynamic_pointer_cast<Fighter>(allCharacters.at(name)) != nullptr) {
                        shared_ptr<Fighter> fighter = dynamic_pointer_cast<Fighter>(allCharacters.at(name));
                        fighter->showPotions();
                    }
                    else if (dynamic_pointer_cast<Archer>(allCharacters.at(name)) != nullptr) {
                        shared_ptr<Archer> archer = dynamic_pointer_cast<Archer>(allCharacters.at(name));
                        archer->showPotions();
                    }else if(dynamic_pointer_cast<Wizard>(allCharacters.at(name)) != nullptr){
                        shared_ptr<Wizard> wizard = dynamic_pointer_cast<Wizard>(allCharacters.at(name));
                        wizard->showPotions();
                    }
                }else
                    fout << "Error caught" << endl;
            }else if(showWhat == "spells"){
                string name = eventWords.at(2);
                if(allCharacters.contains(name)){
                    if (dynamic_pointer_cast<Wizard>(allCharacters.at(name)) != nullptr) {
                        shared_ptr<Wizard> wizard = dynamic_pointer_cast<Wizard>(allCharacters.at(name));
                        wizard->showSpells();
                    }
                    else if (dynamic_pointer_cast<Archer>(allCharacters.at(name)) != nullptr) {
                        shared_ptr<Archer> archer = dynamic_pointer_cast<Archer>(allCharacters.at(name));
                        archer->showSpells();
                    }else
                        fout << "Error caught" << endl;
                }else
                    fout << "Error caught" << endl;
            }
        }
    }

    //just closing the files
    fin.close();
    fout.close();
    return 0;
}