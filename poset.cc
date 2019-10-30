#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <string.h>
#include <assert.h>
// tymczasowe, zeby przejrzysciej sie pisalo
using namespace std;

const int LESSER_ELEMENT = 0;
const int GREATER_ELEMENT = 1;
const string NULL_VALUE = "NULL";

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
    unsigned long nextFreeId = 0;

    Posets &posets()
    {
        static Posets posets;
        return posets;
    }

    ElementPointer getElementPointerFromPoset(Poset &poset, const Element &elementToFind)
    {
        return (poset.find(elementToFind) -> first).c_str();
    }

    bool checkIfElementIsNULL(char const* value)
    {
        return value == nullptr;
    }

    void addElementToPoset(unsigned long id, const Element &newElement)
    {
        GreaterThanRelations greaterThanRelations;
        LessThanRelations lessThanRelations;
        Poset &poset = posets().at(id);

        ElementRelations elementRelations = make_pair(lessThanRelations, greaterThanRelations);
        poset.insert({newElement, elementRelations});
    }

    bool checkIfPosetExists(unsigned long id)
    {
        return posets().find(id) != posets().end();
    }

    bool checkIfElementExistsInPoset(unsigned long id, char const *value)
    {
        Element element = string(value);
        Poset &poset = posets().at(id);

        return poset.find(element) != poset.end();
    }

    bool checkIfElementsAreEqual(char const *value1, char const *value2)
    {
        return strcmp(value1, value2) == 0;
    }

    bool checkIfElementIsLessThanTheOther(unsigned long id, char const *value1,
            char const *value2)
    {
        if (checkIfElementsAreEqual(value1, value2))
        {
            return true;
        }
        else
        {
            Element lessElement = string(value1);
            Element greaterElement = string(value2);
            Poset &poset = posets().at(id);
            ElementRelations &elementRelations = poset.at(lessElement);
            GreaterThanRelations &greaterThanRelations =
                    get<GREATER_ELEMENT>(elementRelations);
            ElementPointer elementPointer = getElementPointerFromPoset(poset, greaterElement);

            return greaterThanRelations.find(elementPointer)
                    != greaterThanRelations.end();
        }

    }

    Relations& getSpecificRelationType(ElementRelations &elementRelations,
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
            ElementRelations &elementRelations = poset.at(elementInRelation);
            Relations &elementInRelationRelations = getSpecificRelationType(elementRelations, relationType);

            ElementPointer elementToErasePointer = getElementPointerFromPoset(poset, elementToErase);
            elementInRelationRelations.erase(elementToErasePointer);
        }
    }

    void removeElementFromPoset(unsigned long id, char const *value)
    {
        Element element = string(value);
        Poset &poset = posets().at(id);
        ElementRelations &elementRelations = poset.at(element);
        LessThanRelations &lessThanRelations = elementRelations.first;
        GreaterThanRelations &greaterThanRelations = elementRelations.second;

        eraseRelationsFromElement(poset, element, lessThanRelations,
                GREATER_ELEMENT);
        eraseRelationsFromElement(poset, element, greaterThanRelations,
                LESSER_ELEMENT);

        poset.erase(element);
    }


    bool checkIfElementsAreInRelation(unsigned long id, char const *value1,
            char const *value2)
    {
        return checkIfElementsAreEqual(value1, value2) ||
            checkIfElementIsLessThanTheOther(id, value1, value2) ||
            checkIfElementIsLessThanTheOther(id, value2, value1);
    }


    bool checkIfElementBelongsToRelations(Poset &poset, Relations &relations, const Element &element)
    {
        ElementPointer elementPointer = getElementPointerFromPoset(poset, element);
        return relations.find(elementPointer) != relations.end();
    }

    void addRelationsToSingleElement(Poset &poset, const Element &elementA,
            Relations &elementBRelations, Relations &elementARelations, RelationType relationType)
    {
        for (const Element &elementFromElementBRelations: elementBRelations)
        {
            ElementRelations &elementRelations = poset.at(elementFromElementBRelations);
            Relations &specificRelations = getSpecificRelationType(elementRelations, relationType);

            if (checkIfElementBelongsToRelations(poset, specificRelations,
                    elementA))
            {
                continue;
            }

            ElementPointer elementPointer = getElementPointerFromPoset(poset, elementFromElementBRelations);
            ElementPointer elementAPointer = getElementPointerFromPoset(poset, elementA);
            specificRelations.insert(elementAPointer);
            elementARelations.insert(elementPointer);
        }
    }

    void addRelationBetweenSingleElements(Poset &poset,
            const Element &elementA, const Element &elementB,
            Relations &elementARelations, Relations &elementBRelations)
    {
        ElementPointer elementBPointer = getElementPointerFromPoset(poset, elementB);
        ElementPointer elementAPointer = getElementPointerFromPoset(poset, elementA);
        elementARelations.insert(elementBPointer);
        elementBRelations.insert(elementAPointer);
    }

    void addRelationBetweenElementsRelations(Poset &poset,
            Relations &elementBRelations, Relations &elementARelations)
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


    void closureRelationBetweenElements(Poset &poset, const Element &elementA,
            const Element &elementB, Relations &elementARelations,
            Relations &elementBRelations, RelationType relationType)
    {
        for (const Element &elementFromElementARelations: elementARelations)
        {
            ElementRelations &relationsOfElementFromElementARelations =
                    poset.at(elementFromElementARelations);

            Relations &specificRelationsOfElementFromElementARelations =
                    getSpecificRelationType(relationsOfElementFromElementARelations, relationType);

            addRelationBetweenElementsRelations(poset, elementBRelations,
                    specificRelationsOfElementFromElementARelations);
        }
    }

    void linkElementsFromRelationsAndClosureRelation(Poset &poset,
            const Element &lessElement, const Element &greaterElement)
    {
        LessThanRelations &lessThanRelationsOfLessElement =
                poset.at(lessElement).first;
        GreaterThanRelations &greaterThanRelationsOfGreaterElement =
                poset.at(greaterElement).second;

        closureRelationBetweenElements(poset, greaterElement, lessElement,
                greaterThanRelationsOfGreaterElement,
                lessThanRelationsOfLessElement, LESSER_ELEMENT);

        closureRelationBetweenElements(poset, lessElement, greaterElement,
                lessThanRelationsOfLessElement,
                greaterThanRelationsOfGreaterElement, GREATER_ELEMENT);
    }

    void linkLessAndGreaterElementWithRelation(Poset &poset,
            const Element &lessElement, const Element &greaterElement)
    {
        LessThanRelations &lessThanRelationsOfGreaterElement =
                poset.at(greaterElement).first;
        LessThanRelations &lessThanRelationsOfLessElement =
                poset.at(lessElement).first;
        GreaterThanRelations &greaterThanRelationsOfGreaterElement =
                poset.at(greaterElement).second;
        GreaterThanRelations &greaterThanRelationsOfLessElement =
                poset.at(lessElement).second;

        addRelationsToSingleElement(poset, greaterElement,
                lessThanRelationsOfLessElement,
                lessThanRelationsOfGreaterElement, GREATER_ELEMENT);
        addRelationsToSingleElement(poset, lessElement,
                greaterThanRelationsOfGreaterElement,
                greaterThanRelationsOfLessElement, LESSER_ELEMENT);

        addRelationBetweenSingleElements(poset, greaterElement, lessElement,
                lessThanRelationsOfGreaterElement,
                greaterThanRelationsOfLessElement);
    }

    void addAndClosureRelationBetweenElements(unsigned long id, char const *value1,
            char const *value2)
    {
        Poset &poset = posets().at(id);
        Element lessElement = string(value1);
        Element greaterElement = string(value2);

        if (!checkIfElementsAreEqual(value1, value2))
        {
            linkElementsFromRelationsAndClosureRelation(poset, lessElement, greaterElement);
            linkLessAndGreaterElementWithRelation(poset, lessElement, greaterElement);
        }
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
        Poset &poset = posets().at(id);
        ElementRelations &element1Relations = poset.at(value1);
        Relations &greaterThanElement1Relations = getSpecificRelationType(
                element1Relations, GREATER_ELEMENT);

        return checkIfAnyOfElementsIsLessThanTheOther(id, value2,
                greaterThanElement1Relations);
    }

    void deleteElementsFromRelations(unsigned long id, char const *value1, char const *value2)
    {
        Poset &poset = posets().at(id);
        Element element1 = string(value1);
        Element element2 = string(value2);

        ElementRelations &element1Relations = poset.at(element1);
        GreaterThanRelations &greaterThanElement1 =
                getSpecificRelationType(element1Relations, GREATER_ELEMENT);

        ElementRelations &element2Relations = poset.at(element2);
        GreaterThanRelations &lessThanElement2 =
                getSpecificRelationType(element2Relations, LESSER_ELEMENT);

        ElementPointer element1Pointer = getElementPointerFromPoset(poset,
                element1);
        ElementPointer element2Pointer = getElementPointerFromPoset(poset,
                element2);

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
    if (checkIfPosetExists(id))
    {
        Poset &poset = posets().at(id);
        return poset.size();
    }
    else
    {
        return 0;
    }
}

bool poset_insert(unsigned long id, char const *value)
{

    if (checkIfPosetExists(id) && !checkIfElementIsNULL(value) &&
            !checkIfElementExistsInPoset(id, value))
    {
        Element newElement = string(value);
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
    if (checkIfPosetExists(id) && !checkIfElementIsNULL(value) &&
            checkIfElementExistsInPoset(id, value))
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
    if (checkIfPosetExists(id) && !checkIfElementIsNULL(value1) &&
            !checkIfElementIsNULL(value2) && checkIfElementExistsInPoset(id, value1) &&
            checkIfElementExistsInPoset(id, value2) &&
            !checkIfElementsAreInRelation(id, value1, value2))
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
    if (!(!checkIfElementIsNULL(value1) && !checkIfElementIsNULL(value2) &&
            checkIfPosetExists(id) && checkIfElementExistsInPoset(id, value1) &&
            checkIfElementExistsInPoset(id, value2) &&
            !checkIfElementsAreEqual(value1, value2) &&
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
    if (!checkIfElementIsNULL(value1) && !checkIfElementIsNULL(value2) &&
        checkIfPosetExists(id) && checkIfElementExistsInPoset(id, value1) &&
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
        Poset &poset = posets().at(id);
        poset.clear();
    }
}

int main()
{
    Id poset_id = poset_new();
    assert(!poset_size(poset_id));
    assert(poset_insert(poset_id, "a"));
    assert(!poset_insert(poset_id, "a"));
    assert(poset_insert(poset_id, "b"));
    assert(poset_insert(poset_id, "c"));
    assert(poset_insert(poset_id, "d"));
    assert(poset_add(poset_id, "b", "c"));
    assert(poset_test(poset_id, "b", "c"));
    assert(!poset_test(poset_id, "c", "b"));
    assert(poset_test(poset_id, "b", "b"));
    assert(!poset_insert(poset_id, nullptr));
    assert(!poset_add(poset_id, "a", "a"));
    assert(!poset_add(poset_id, "c", "b"));
    assert(!poset_add(poset_id, "b", "c"));
    assert(poset_add(poset_id, "c", "d"));
    assert(poset_test(poset_id, "c", "d"));
    assert(poset_test(poset_id, "b", "d"));
    assert(poset_add(poset_id, "a", "b"));
    assert(poset_test(poset_id, "a", "b"));
    assert(poset_test(poset_id, "a", "d"));
    assert(!poset_test(poset_id, "d", "a"));
    poset_clear(poset_id);
    assert(!poset_test(poset_id, "a", "d"));
    assert(!poset_test(poset_id, "b", "c"));
    assert(!poset_test(poset_id, "b", "d"));
    assert(poset_insert(poset_id, "a"));
//
//
    poset_clear(poset_id);
    assert(poset_size(poset_id) == 0);
//
    assert(poset_insert(poset_id, "a"));
    assert(poset_insert(poset_id, "b"));
    assert(poset_insert(poset_id, "c"));
    assert(poset_add(poset_id, "a", "b"));
    assert(poset_add(poset_id, "b", "c"));
//
    assert(!poset_del(poset_id, "a", "c"));
    assert(poset_insert(poset_id, "d"));
    assert(!poset_del(poset_id, "a", "d"));
    assert(!poset_del(poset_id, "a", nullptr));
    assert(!poset_del(poset_id, "a", "e"));
//
    assert(poset_remove(poset_id, "b"));
    assert(poset_remove(poset_id, "c"));
    assert(poset_remove(poset_id, "d"));
    assert(poset_size(poset_id) == 1);
    assert(!poset_test(poset_id, "b", "d"));
    assert(!poset_test(poset_id, "a", "d"));

}