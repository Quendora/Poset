#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <string>
// tymczasowe, zeby przejrzysciej sie pisalo
using namespace std;

const int LESS_THAN_RELATIONS = 0;
const int GREATER_THAN_RELATIONS = 1;

// Jak przechowywac w unordered_map referencje a nie kopie?
using Id = unsigned long;
using Element = string;
using Relations = unordered_set<Element>;
using LessThanRelations = Relations;
using GreaterThanRelations = Relations;
using ElementRelations = pair<LessThanRelations, GreaterThanRelations>;
using Poset = unordered_map<Element, ElementRelations>;
using Posets = unordered_map<Id, Poset>;

// Chwilowo nie mam pojecia jak elegancko zaapliwoac tutaj zmienna globalna, wiec tymczasowo jest tak:
Posets posets;

unsigned long poset_new(void)
{
    return 0;
}

void poset_delete(unsigned long id)
{
    posets.erase(id);
}

size_t poset_size(unsigned long id)
{
    return posets.size();
}

void makeNewPoset(unsigned long id, Element &newElement)
{
    GreaterThanRelations greaterThanRelations;
    LessThanRelations lessThanRelations;
    Poset newPoset;

    ElementRelations elementRelations = make_pair(lessThanRelations, greaterThanRelations);
    newPoset.insert({newElement, elementRelations});
    posets.insert({id, newPoset});
}

bool checkIfPosetExists(unsigned long id)
{
    return posets.find(id) != posets.end();
}

bool checkIfPosetExistsInPosets(unsigned long id, string &newElement)
{
    Poset poset = posets.at(id);

    return poset.find(newElement) != poset.end();
}

bool poset_insert(unsigned long id, char const *value)
{
    Element newElement = string(value);

    if (checkIfPosetExists(id) && !checkIfPosetExistsInPosets(id, newElement))
    {
        makeNewPoset(id, newElement);

        return true;
    }
    else
    {
        return false;
    }
}

bool checkIfElementExistsInPoset(unsigned long id, char const *value)
{
    if (posets.find(id) != posets.end())
    {
        Element element = string(value);
        Poset poset = posets.at(id);

        if (poset.find(element) == poset.end())
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool checkIfElementIsLessThanTheOther(unsigned long id, char const *value1, char const *value2)
{
    Element lessElement = string(value1), greaterElement = string(value2);
    Poset poset = posets.at(id);
    ElementRelations elementRelations = poset.at(lessElement);
    GreaterThanRelations greaterThanRelations = get<GREATER_THAN_RELATIONS>(elementRelations);

    return greaterThanRelations.find(greaterElement) != greaterThanRelations.end();
}

bool poset_test(unsigned long id, char const *value1, char const *value2)
{
    if (checkIfPosetExists(id) && checkIfElementExistsInPoset(id, value1) &&
        checkIfElementExistsInPoset(id, value2))
    {
        return checkIfElementIsLessThanTheOther(id, value1, value2);
    }
    else
    {
        return false;
    }
}

int main()
{

}