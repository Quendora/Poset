#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <string.h>
// tymczasowe, zeby przejrzysciej sie pisalo
using namespace std;

const int LESSER_ELEMENT = 0;
const int GREATER_ELEMENT = 1;

// Jak przechowywać w unordered_map referencje a nie kopie?
using RelationType = int;
using Id = unsigned long;
using Element = string;
using ElementPointer = const char*;
using Relations = unordered_set<ElementPointer>;
using LessThanRelations = Relations;
using GreaterThanRelations = Relations;
using ElementRelations = pair<LessThanRelations, GreaterThanRelations>;
using Poset = unordered_map<Element, ElementRelations>;
using Posets = unordered_map<Id, Poset>;

// Chwilowo nie mam pojecia jak elegancko zaapliwoac tutaj zmienna globalna, wiec tymczasowo jest tak:

namespace
{
    unsigned long nextFreeId;

    Posets &posets()
    {
        static auto *a = new Posets();
        return *a;
    }

    ElementPointer getElementPointerFromPoset(Poset poset, const Element &elementToFind)
    {
        Element elementFromPoset = poset.find(elementToFind) -> first;
        return elementFromPoset.c_str();
    }

    void addElementToPoset(unsigned long id, const Element &newElement)
    {
        GreaterThanRelations greaterThanRelations;
        LessThanRelations lessThanRelations;
        Poset poset = posets().at(id);

        ElementRelations elementRelations =
                make_pair(lessThanRelations, greaterThanRelations);
        poset.insert({newElement, elementRelations});
    }

    bool checkIfPosetExists(unsigned long id)
    {
        return posets().find(id) != posets().end();
    }

    bool checkIfElementExistsInPoset(unsigned long id, char const *value)
    {
        Element element = string(value);
        Poset poset = posets().at(id);

        return poset.find(element) != poset.end();
    }

    bool checkIfElementIsLessThanTheOther(unsigned long id, char const *value1,
            char const *value2)
    {
        Element lessElement = string(value1);
        Element greaterElement = string(value2);
        Poset poset = posets().at(id);
        ElementRelations elementRelations = poset.at(lessElement);
        GreaterThanRelations greaterThanRelations =
                get<GREATER_ELEMENT>(elementRelations);
        ElementPointer elementPointer = getElementPointerFromPoset(poset, greaterElement);
//
        return greaterThanRelations.find(elementPointer)
            != greaterThanRelations.end();
    }

    Relations getSpecificRelationType(ElementRelations elementRelations,
            RelationType relationType)
    {
        if (relationType == LESSER_ELEMENT)
        {
            return get<LESSER_ELEMENT>(elementRelations);
        }
        else
        {
            return get<GREATER_ELEMENT>(elementRelations);
        }
    }

    void eraseRelationsFromElement(Poset &poset, const Element &elementToErase,
            const Relations &relations, const RelationType relationType)
    {
        for (const Element &elementInRelation: relations)
        {
            ElementRelations elementRelations = poset.at(elementInRelation);
            Relations elementInRelationRelations = getSpecificRelationType(elementRelations, relationType);

            ElementPointer elementToErasePointer = getElementPointerFromPoset(poset, elementToErase);
            elementInRelationRelations.erase(elementToErasePointer);
        }
    }

    void removeElementFromPoset(unsigned long id, char const *value)
    {
        Element element = string(value);
        Poset poset = posets().at(id);
        ElementRelations elementRelations = poset.at(element);
        LessThanRelations lessThanRelations = elementRelations.first;
        GreaterThanRelations greaterThanRelations = elementRelations.second;

        eraseRelationsFromElement(poset, element, lessThanRelations,
                GREATER_ELEMENT);
        eraseRelationsFromElement(poset, element, greaterThanRelations,
                LESSER_ELEMENT);

        poset.erase(element);
    }

    bool checkIfElementsAreEqual(char const *value1, char const *value2) //TODO
    {
        return strcmp(value1, value2) == 0;
    }


    bool checkIfElementsAreInRelation(unsigned long id, char const *value1,
            char const *value2)
    {
        return checkIfElementsAreEqual(value1, value2) ||
            checkIfElementIsLessThanTheOther(id, value1, value2) ||
            checkIfElementIsLessThanTheOther(id, value2, value1);
    }


    bool checkIfElementBelongsToRelations(const Poset &poset, Relations relations, Element element)
    {
        ElementPointer elementPointer = getElementPointerFromPoset(poset, element);
        return relations.find(elementPointer) != relations.end();
    }

    void addRelationsToSingleElement(const Poset &poset, const Element &elementA,
            Relations elementBRelations)
    {
        for (const Element &elementFromElementBRelations: elementBRelations)
        {
            if (checkIfElementBelongsToRelations(poset, elementBRelations,
                    elementA))
            {
                continue;
            }

            ElementPointer elementAPointer = getElementPointerFromPoset(poset, elementA);
            elementBRelations.insert(elementAPointer);
        }
    }

    void addRelationBetweenSingleElements(const Poset &poset,
            const Element &elementA, const Element &elementB,
            Relations elementARelations, Relations elementBRelations)
    {
        ElementPointer elementAPointer = getElementPointerFromPoset(poset, elementA);
        ElementPointer elementBPointer = getElementPointerFromPoset(poset, elementB);
        elementARelations.insert(elementBPointer);
        elementBRelations.insert(elementAPointer);
    }

    void addRelationBetweenElementsRelations(const Poset &poset,
            Relations elementBRelations, Relations elementARelations)
    {
        for (const Element &elementFromElementBRelations: elementBRelations)
        {
            if (checkIfElementBelongsToRelations(poset, elementARelations,
                    elementFromElementBRelations))
            {
                continue;
            }

            ElementPointer elementPointer = getElementPointerFromPoset(poset, elementFromElementBRelations);
            elementARelations.insert(elementPointer);
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

            addRelationBetweenElementsRelations(poset, elementBRelations, specificRelationsOfElementFromElementARelations);
        }
    }

    void linkElementsFromRelationsAndClosureRelation(Poset poset,
            const Element &lessElement, const Element &greaterElement)
    {
        LessThanRelations lessThanRelationsOfLessElement =
                poset.at(lessElement).first;
        GreaterThanRelations greaterThanRelationsOfGreaterElement =
                poset.at(greaterElement).second;

        closureRelationBetweenElements(poset, greaterElement, lessElement,
                greaterThanRelationsOfGreaterElement,
                lessThanRelationsOfLessElement, LESSER_ELEMENT);

        closureRelationBetweenElements(poset, lessElement, greaterElement,
                lessThanRelationsOfLessElement,
                greaterThanRelationsOfGreaterElement, GREATER_ELEMENT);
    }

    void linkLessAndGreaterElementWithRelation(Poset poset,
            const Element &lessElement, const Element &greaterElement)
    {
        LessThanRelations lessThanRelationsOfGreaterElement =
                poset.at(greaterElement).first;
        GreaterThanRelations greaterThanRelationsOfLessElement =
                poset.at(lessElement).second;

        addRelationsToSingleElement(poset, greaterElement,
                greaterThanRelationsOfLessElement);
        addRelationsToSingleElement(poset, lessElement,
                lessThanRelationsOfGreaterElement);

        addRelationBetweenSingleElements(poset, greaterElement, lessElement,
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
            Poset poset = posets().at(id);

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

    bool checkIfAnyOfElementsIsLessThanTheOther(unsigned long id, char const *value,
        const Relations &greaterThanRelations)
    {
        for (ElementPointer element : greaterThanRelations)
        {
            if (checkIfElementIsLessThanTheOther(id, element, value))
            {
                return false;
            }
        }

        return true;
    }

    bool checkIfNoOtherWay(unsigned long id, char const *value1, char const *value2)
    {
        Poset poset = posets().at(id);
        ElementRelations element1Relations = poset.at(value1);
        Relations greaterThanElement1Relations = getSpecificRelationType(element1Relations, GREATER_ELEMENT);

        return checkIfAnyOfElementsIsLessThanTheOther(id, value2, greaterThanElement1Relations);
    }

    void deleteElementsFromRelations(unsigned long id, char const *value1, char const *value2)
    {
        Poset poset = posets().at(id);
        Element element1 = string(value1);
        Element element2 = string(value2);

        ElementRelations element1Relations = poset.at(element1);
        GreaterThanRelations greaterThanElement1 =
                getSpecificRelationType(element1Relations, GREATER_ELEMENT);

        ElementRelations element2Relations = poset.at(element2);
        GreaterThanRelations lessThanElement2 =
                getSpecificRelationType(element2Relations, LESSER_ELEMENT);

        ElementPointer element1Pointer = getElementPointerFromPoset(poset, element1);
        ElementPointer element2Pointer = getElementPointerFromPoset(poset, element2);

        greaterThanElement1.erase(element1Pointer);
        lessThanElement2.erase(element2Pointer);
    }
}


unsigned long poset_new(void)
{
    Poset poset;
    unsigned long newId = nextFreeId;

    posets().insert({newId, poset});
    nextFreeId++;

    return newId;
}

void poset_delete(unsigned long id)
{
    posets().erase(id);
}

size_t poset_size(unsigned long id)
{
    return posets().size();
}

bool poset_insert(unsigned long id, char const *value)
{
    Element newElement = string(value);

    if (checkIfPosetExists(id) && !checkIfElementExistsInPoset(id, value))
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

bool poset_del(unsigned long id, char const *value1, char const *value2)
{
    if (!(checkIfPosetExists(id) && checkIfElementExistsInPoset(id, value1) &&
            checkIfElementExistsInPoset(id, value2) &&
            !checkIfElementsAreEqual(value1, value2) && //nie mozna usunąć zwrotnosci
            checkIfElementIsLessThanTheOther(id, value1, value2) &&
            checkIfNoOtherWay(id, value1, value2)))
    {
        return false;
    }

    deleteElementsFromRelations(id, value1, value2);

    return true;
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
        Poset poset = posets().at(id);
        poset.clear();
//
//        posets().erase(id);
//
//        Poset emptyPoset;
//        posets().insert({id, emptyPoset});
    }
}

int main()
{
//    char const *value1 = "sdfljsdljsdf";
//    char const *value2 = "sdfljsdljsdf";
//
//    string vString = string(value1);
//    string vString1 = string(value2);
//
//    bool x = vString == vString1;
//
//    cout << x;
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