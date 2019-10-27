#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <string_view>
// tymczasowe, zeby przejrzysciej sie pisalo
using namespace std;

const int LESS_THAN_RELATIONS = 0;
const int GREATER_THAN_RELATIONS = 1;

// Jak przechowywać w unordered_map referencje a nie kopie?
using RelationType = int;
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
unsigned int nextFreeId;

namespace
{
    void addElementToPoset(unsigned long id, const Element &newElement)
    {
        GreaterThanRelations greaterThanRelations;
        LessThanRelations lessThanRelations;
        Poset poset = posets.at(id);

        ElementRelations elementRelations =
                make_pair(lessThanRelations, greaterThanRelations);
        poset.insert({newElement, elementRelations});
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

    bool checkIfElementIsLessThanTheOther(unsigned long id, char const *value1,
            char const *value2)
    {
        Element lessElement = string(value1), greaterElement = string(value2);
        Poset poset = posets.at(id);
        ElementRelations elementRelations = poset.at(lessElement);
        GreaterThanRelations greaterThanRelations =
                get<GREATER_THAN_RELATIONS>(elementRelations);

        return greaterThanRelations.find(greaterElement)
            != greaterThanRelations.end();
    }

    void eraseRelationsFromElement(Poset &poset, const Element &elementToErase,
            const Relations &relations, const RelationType relationType)
    {
        for (const Element &elementInRelation: relations)
        {
            Relations elementInRelationRelations;

            if (relationType == LESS_THAN_RELATIONS)
            {
                elementInRelationRelations = poset.at(elementInRelation).first;
            }
            else
            {
                elementInRelationRelations = poset.at(elementInRelation).second;
            }

            elementInRelationRelations.erase(elementToErase);
        }
    }

    void removeElementFromPoset(unsigned long id, char const *value)
    {
        Element element = string(value);
        Poset poset = posets.at(id);
        ElementRelations elementRelations = poset.at(element);
        LessThanRelations lessThanRelations = elementRelations.first;
        GreaterThanRelations greaterThanRelations = elementRelations.second;

        eraseRelationsFromElement(poset, element, lessThanRelations,
                GREATER_THAN_RELATIONS);
        eraseRelationsFromElement(poset, element, greaterThanRelations,
                LESS_THAN_RELATIONS);

        poset.erase(element);
    }

    bool checkIfElementsAreEqual(char const *value1, char const *value2)
    {
        Element element1 = string(value1);
        Element element2 = string(value2);

        return element1 != element2;
    }


    bool checkIfElementsAreInRelation(unsigned long id, char const *value1,
            char const *value2)
    {
        return checkIfElementsAreEqual(value1, value2) ||
            checkIfElementIsLessThanTheOther(id, value1, value2) ||
            checkIfElementIsLessThanTheOther(id, value2, value1);
    }

    void linkElementsWithRelation(Poset &poset, const Element &elementToAdd,
            const Relations &relations, const RelationType relationType)
    {

    }

    bool checkIfElementBelongsToRelations(Relations relations, Element element)
    {
        return relations.find(element) != relations.end();
    }

    Relations getSpecificRelationType(ElementRelations elementRelations,
            RelationType relationType)
    {
        if (relationType == LESS_THAN_RELATIONS)
        {
            return get<LESS_THAN_RELATIONS>(elementRelations);
        }
        else
        {
            return get<GREATER_THAN_RELATIONS>(elementRelations);
        }
    }

    void addRelationsToSingleElement(const Element &elementA,
            Relations elementBRelations)
    {
        for (const Element &elementFromElementBRelations: elementBRelations)
        {
            if (checkIfElementBelongsToRelations(elementBRelations,
                    elementA))
            {
                continue;
            }

            elementBRelations.insert(elementA);
        }
    }

    void addRelationBetweenSingleElements(const Element &elementA,
            const Element &elementB, Relations elementARelations,
            Relations elementBRelations)
    {
        elementARelations.insert(elementB);
        elementBRelations.insert(elementA);
    }

    void addRelationBetweenElementsRelations(Relations elementBRelations,
            Relations elementARelations)
    {
        for (const Element &elementFromElementBRelations: elementBRelations)
        {
            if (checkIfElementBelongsToRelations(
                    elementARelations, elementFromElementBRelations))
            {
                continue;
            }

            elementARelations.insert(elementFromElementBRelations);
        }
    }


    void closureRelationBetweenElements(Poset poset, const Element &elementA,
            const Element &elementB, Relations elementARelations,
            const Relations &elementBRelations, RelationType relationType)
    {
        for (const Element &elementFromElementARelations: elementARelations)
        {
            ElementRelations relationsOfElementFromElementARelations =
                    poset.at(elementFromElementARelations);

            Relations specificRelationsOfElementFromElementARelations =
                    getSpecificRelationType(relationsOfElementFromElementARelations, relationType);

            addRelationBetweenElementsRelations(elementBRelations, specificRelationsOfElementFromElementARelations);
        }
    }

    void linkElementsFromRelationsAndClosureRelation(Poset poset, Element lessElement,
            const Element &greaterElement)
    {
        LessThanRelations lessThanRelationsOfLessElement =
                poset.at(lessElement).first;
        GreaterThanRelations greaterThanRelationsOfGreaterElement =
                poset.at(greaterElement).second;

        closureRelationBetweenElements(poset, greaterElement, lessElement,
                greaterThanRelationsOfGreaterElement,
                lessThanRelationsOfLessElement, LESS_THAN_RELATIONS);

        closureRelationBetweenElements(poset, lessElement, greaterElement,
                lessThanRelationsOfLessElement,
                greaterThanRelationsOfGreaterElement, GREATER_THAN_RELATIONS);
    }

    void linkLessAndGreaterElementWithRelation(Poset poset,
            const Element &lessElement, const Element &greaterElement)
    {
        LessThanRelations lessThanRelationsOfGreaterElement =
                poset.at(greaterElement).first;
        GreaterThanRelations greaterThanRelationsOfLessElement =
                poset.at(lessElement).second;

        addRelationsToSingleElement(greaterElement,
                greaterThanRelationsOfLessElement);
        addRelationsToSingleElement(lessElement,
                lessThanRelationsOfGreaterElement);

        addRelationBetweenSingleElements(greaterElement, lessElement,
                lessThanRelationsOfGreaterElement,
                greaterThanRelationsOfLessElement);
    }

    void addAndClosureRelationBetweenElements(unsigned long id, char const *value1,
            char const *value2)
    {
        Element lessElement = string(value1);
        Element greaterElement = string(value2);

        if (lessElement != greaterElement)
        {
            Poset poset = posets.at(id);

            linkElementsFromRelationsAndClosureRelation(poset, lessElement, greaterElement);
            linkLessAndGreaterElementWithRelation(poset, lessElement, greaterElement);
        }


//        //Do zbioru mniejszych WIEKSZEGO elementu dodaje mniejsze elementy
//        for (const Element &greaterElement: greaterThanRelationsOfGreaterElement)
//        {
//            ElementRelations greaterElementRelations = poset.at(greaterElement);
//            Relations greaterElementLessRelations = greaterElementRelations.first;
//
//            for (const Element &lessElement: lessThanRelationsOfLessElement)
//            {
//                if (checkIfElementBelongsToRelations(greaterElementLessRelations, lessElement))
//                {
//                    continue;
//                }
//
//                greaterElementLessRelations.insert(lessElement);
//            }
//        }
//
//        // Do zbioru wiekszych MNIEJSZEGO elementu dodaje wieksze elementy
//        for (const Element &lessElement: lessThanRelationsOfLessElement)
//        {
//            ElementRelations lessElementRelations = poset.at(lessElement);
//            Relations lessElementGreaterRelations = lessElementRelations.second;
//
//            for (const Element &greaterElement: greaterThanRelationsOfGreaterElement)
//            {
//                if (checkIfElementBelongsToRelations(lessElementGreaterRelations, greaterElement))
//                {
//                    continue;
//                }
//
//                lessElementGreaterRelations.insert(greaterElement);
//            }
//        }
    }
}

//TODO Zeby to zrobic trzeba ogarnac jak zainicjowac zmienna globalna, a potem ja incrementowac.
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

bool poset_insert(unsigned long id, char const *value)
{
    Element newElement = string(value);

    if (checkIfPosetExists(id) && !checkIfPosetExistsInPosets(id, newElement))
    {
        addElementToPoset(id, newElement);

        return true;
    }
    else
    {
        return false;
    }
}

bool poset_remove(unsigned long id, char const *value)
{
    if (checkIfPosetExists(id) && checkIfElementExistsInPoset(id, value))
    {
        removeElementFromPoset(id, value);

        return true;
    }
    else
    {
        return false;
    }
}


bool poset_add(unsigned long id, char const *value1, char const *value2)
{
    if (checkIfPosetExists(id) && checkIfElementExistsInPoset(id, value1) &&
            checkIfElementExistsInPoset(id, value2) &&
            checkIfElementsAreInRelation(id, value1, value2))
    {
        addAndClosureRelationBetweenElements(id, value1, value2);

        return true;
    }
    else
    {
        return false;
    }
}

//TODO
bool poset_del(unsigned long id, char const *value1, char const *value2)
{
    return false;
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

void poset_clear(unsigned long id)
{
    if (checkIfPosetExists(id))
    {
        posets.erase(id);

        Poset emptyPoset;
        posets.insert({id, emptyPoset});
    }
}

int main()
{
//    char const *value = "sdfljsdljsdf";
//    string vString = string(value);
//    string vString1 = string(value);
//
//    unordered_map<string, int> map;
//    map.insert({value, 0});
//
//    cout << vString <<endl;
//    string_view vView = string_view(value);
//    string_view vView1 = vString1;
//
//    string convert = (string) vView.substr(0);
//    cout << (string) vView << "LUL" << endl;
//
//    cout << map.at(convert) << endl;
//
//    bool tak = vString == convert;
//    cout << tak;

}