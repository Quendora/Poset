#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <string.h>
#include <assert.h>

const int LESSER_ELEMENT = 0;
const int GREATER_ELEMENT = 1;
const bool WANTED_IN_POSET = true;
const bool NOT_WANTED_IN_POSET = false;
const char *NULL_VALUE = nullptr;
const int ONLY_ONE_VALUE = 0;
const int FIRST_VALUE = 1;
const int SECOND_VALUE = 2;
const int CHECK_BOTH = 2;

const char *DOES_NOT_EXIST = "does not exist";
const char *NULL_STRING = "\"NULL\"";
const char *QUOTE = "\"";
const char *POSET_NEW_WITH_BRACKETS = "poset_new()";
const char *POSET_NEW = "poset_new: ";
const char *POSET_ = "poset_";
const char *POSET = "poset ";
const char *SPACE = " ";
const char *OPENING_BRACKET = "(";
const char *CLOSING_BRACKET = ")";
const char *COLON = ": ";
const char *CONTAINS = " contains ";
const char *ELEMENTS_S = " element(s)";
const char *COMMA = ", ";
const char *ELEMENT = "element ";
const char *RELATION = "relation ";
const char *INVALID_VALUE = "invalid value";
const char *NULL_IN_BRANCKETS = " (NULL)";
const char *EXISTS = "exists";
const char *DOES_NOT_EXISTS = "does not exists";
const char *ALREADY_EXISTS = "already exists";
const char *CANNOT_BE_DELETED = "cannot be deleted";
const char *CANNOT_BE_ADDED = "cannot be added";
const char *OR = " or ";
const char *DELETE = "delete";
const char *DELETED = "deleted";
const char *CREATED = "created";
const char *SIZE = "size";
const char *INSERT = "insert";
const char *INSERTED = "inserted";
const char *REMOVE = "remove";
const char *REMOVED = "removed";
const char *ADD = "add";
const char *ADDED = "added";
const char *DEL = "del";
const char *TEST = "test";
const char *CLEAR = "clear";
const char *CLEARED = "cleared";

using RelationType = int;
using Id = unsigned long;
using Element = std::string;
using ElementPointer = const char *;
using Relations = std::unordered_set<ElementPointer>;
using LessThanRelations = Relations;
using GreaterThanRelations = Relations;
using ElementRelations = std::pair<LessThanRelations, GreaterThanRelations>;
using Poset = std::unordered_map<Element, ElementRelations>;
using Posets = std::unordered_map<Id, Poset>;

void cerr_init()
{
	static std::ios_base::Init init;
}

namespace
{
#ifndef NDEBUG
	const bool debug = true;
#else
	const bool debug = false;
#endif

	unsigned long nextFreeId = 0;

	Posets &posets()
	{
		static Posets posets;
		return posets;
	}

	std::string getCaps(const char *value)
	{
		if (value == NULL_VALUE)
		{
			return NULL_STRING;
		}

		return QUOTE + std::string(value) + QUOTE;
	}

	void printErrPoset_newBeg()
	{
		if (debug)
		{
			std::cerr << POSET_NEW_WITH_BRACKETS << std::endl;
			std::cerr << POSET_NEW;
		}
	}

	void printErrBegNoValue(unsigned long id, const std::string &method)
	{
		if (debug)
		{
			std::cerr << POSET_ << method << OPENING_BRACKET << id
			          << CLOSING_BRACKET << std::endl;
			std::cerr << POSET_ << method << COLON;
		}
	}

	void printErrEndNoValue(unsigned long id, const std::string &comment)
	{
		if (debug)
		{
			std::cerr << POSET << id << SPACE << comment << std::endl;
		}
	}

	void printErrPoset_sizeEnd(unsigned long id, size_t posetSize)
	{
		if (debug)
		{
			std::cerr << POSET << id << CONTAINS
			          << posetSize << ELEMENTS_S << std::endl;
		}
	}

	void printErrBegOneValue(unsigned long id, const char *value,
			const std::string &method)
	{
		if (debug)
		{
			std::cerr << POSET_ << method << OPENING_BRACKET << id << COMMA
			          << getCaps(value) << CLOSING_BRACKET << std::endl;
			std::cerr << POSET_ << method << COLON;
		}
	}

	void printErrEndOneValue(unsigned long id, const char *value,
			const std::string &comment)
	{
		if (debug)
		{
			std::cerr << POSET << id << COMMA << ELEMENT << getCaps(value)
			          << SPACE << comment << std::endl;
		}
	}

	void printErrBegTwoValues(unsigned long id, const char *value1,
			const char *value2, const std::string &method)
	{
		if (debug)
		{
			std::cerr << POSET_ << method << OPENING_BRACKET << id << COMMA
			          << getCaps(value1) << COMMA
			          << getCaps(value2) << CLOSING_BRACKET << std::endl;
			std::cerr << POSET_ << method << COLON;
		}
	}

	void printErrEndTwoValues(unsigned long id, const char *value1,
			const char *value2, const std::string &comment)
	{
		if (debug)
		{
			std::cerr << POSET << id << COMMA << RELATION
			          << OPENING_BRACKET << getCaps(value1) << COMMA
			          << getCaps(value2) << CLOSING_BRACKET << SPACE
			          << comment << std::endl;
		}
	}

	void printErrElementIsNULL(int valueNr, bool check)
	{
		if (debug && check)
		{
			std::cerr << INVALID_VALUE;

			if (valueNr > 0)
			{
				std::cerr << valueNr;
			}

			std::cerr << NULL_IN_BRANCKETS << std::endl;
		}
	}

	void printErrRelationExistsOrNotExists(unsigned long id, const char *value1,
			const char *value2, bool check)
	{
		if (debug && check)
		{
			printErrEndTwoValues(id, value1, value2, EXISTS);
		}
		else
		{
			printErrEndTwoValues(id, value1, value2, DOES_NOT_EXISTS);
		}
	}

	void printErrPosetDoesNotExist(unsigned long id, bool check)
	{
		if (debug && !check)
		{
			printErrEndNoValue(id, DOES_NOT_EXISTS);
		}
	}

	void printErrElementExistsOrNotExists(unsigned long id, const char *value,
			bool check, bool wantedInPoset, bool valuesToCheck)
	{
		if (debug && valuesToCheck == ONLY_ONE_VALUE)
		{
			if (!check && wantedInPoset)
			{
				printErrEndOneValue(id, value, DOES_NOT_EXISTS);
			}

			if (check && !wantedInPoset)
			{
				printErrEndOneValue(id, value, ALREADY_EXISTS);
			}
		}
	}

	void printErrRelationCannotBeDeleted(unsigned long id, const char *value1,
			const char *value2, bool check)
	{
		if (debug && !check)
		{
			printErrEndTwoValues(id, value1, value2, CANNOT_BE_DELETED);
		}

	}

	void printErrRelationCannotBeAdded(unsigned long id, const char *value1,
			const char *value2, bool check)
	{
		if (debug && check)
		{
			printErrEndTwoValues(id, value1, value2, CANNOT_BE_ADDED);
		}
	}

	void printErrNotExists(unsigned long id, char const *value1,
			char const *value2, bool check)
	{
		if (debug && !check)
		{
			std::cerr << POSET << id << COMMA << ELEMENT << getCaps(value1)
			          << OR << getCaps(value2) << SPACE
			          << DOES_NOT_EXIST << std::endl;
		}
	}

	void printErrReflexiveRelation(unsigned long id,
			const char *value1, const char *value2, bool check)
	{
		if (debug && check)
		{
			printErrEndTwoValues(id, value1, value2, CANNOT_BE_DELETED);
		}
	}

	ElementPointer getElementPointerFromPoset(Poset &poset,
			const Element &elementToFind)
	{
		return (poset.find(elementToFind)->first).c_str();
	}

	bool checkIfValueIsNULL(char const *value, int valueNr)
	{
		bool check = value == NULL_VALUE;

		//TODO INVALID VALUE (NULL)+ przelacznik
		printErrElementIsNULL(valueNr, check);

		return check;
	}

	bool checkIfAnyElementIsNULL(char const *value1, char const *value2)
	{
		return checkIfValueIsNULL(value1, FIRST_VALUE) ||
				checkIfValueIsNULL(value2, SECOND_VALUE);
	}

	void addElementToPoset(unsigned long id, const char *value)
	{
		Element newElement = std::string(value);
		Poset &poset = posets().at(id);
		GreaterThanRelations greaterThanRelations;
		LessThanRelations lessThanRelations;
		ElementRelations elementRelations = make_pair(lessThanRelations,
				greaterThanRelations);

		poset.insert({newElement, elementRelations});
	}

	bool checkIfPosetExists(unsigned long id)
	{
		bool check = posets().find(id) != posets().end();

		//TODO DEBUG POSET N DOES NOT EXISTS
		printErrPosetDoesNotExist(id, check);

		return check;
	}

	bool checkIfElementExistsInPoset(unsigned long id, char const *value,
			bool wantedInPoset, int valuesToCheck)
	{
		Element element = std::string(value);
		Poset &poset = posets().at(id);

		bool check = poset.find(element) != poset.end();

		// 2 WERSJE ZALEZNE OD WANTED_IN_POSET
		//TODO DEBUG POSET N, ELEMENT "A" DOES NOT EXIST
		//TODO DEBUG POSET N, ELEMENT "A" ALREADY EXISTS
		printErrElementExistsOrNotExists(id, value, check, wantedInPoset,
				valuesToCheck);

		return check;
	}

	bool checkIfBothElementsExistInPoset(unsigned long id, char const *value1,
			char const *value2)
	{
		bool check = checkIfElementExistsInPoset(id, value1,
				WANTED_IN_POSET, CHECK_BOTH) &&
				checkIfElementExistsInPoset(id, value2,
						WANTED_IN_POSET, CHECK_BOTH);

		//TODO DEBUG POSET N, ELEMENT "A" OR "B" DOES NOT EXIST
		printErrNotExists(id, value1, value2, check);

		return check;
	}

	bool checkIfElementsAreEqual(char const *value1, char const *value2)
	{
		return strcmp(value1, value2) == 0;
	}

	bool checkIfReflexiveRelation(unsigned long id,
			const char *value1, const char *value2)
	{
		bool check = checkIfElementsAreEqual(value1, value2);

		printErrReflexiveRelation(id, value1, value2, check);

		return check;
	}


	Relations &getSpecificRelationType(ElementRelations &elementRelations,
			RelationType relationType)
	{
		if (relationType == LESSER_ELEMENT)
		{
			return std::get<LESSER_ELEMENT>(elementRelations);
		}
		else
		{
			return std::get<GREATER_ELEMENT>(elementRelations);
		}
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
			Element lessElement = std::string(value1);
			Element greaterElement = std::string(value2);
			Poset &poset = posets().at(id);
			ElementRelations &elementRelations = poset.at(lessElement);
			GreaterThanRelations &greaterThanRelations =
					getSpecificRelationType(elementRelations, GREATER_ELEMENT);
			ElementPointer elementPointer = getElementPointerFromPoset(poset,
					greaterElement);

			return greaterThanRelations.find(elementPointer) !=
					greaterThanRelations.end();
		}
	}

	bool checkIfElementIsLessThanTheOtherForDel(unsigned long id,
			char const *value1, char const *value2)
	{
		bool check = checkIfElementIsLessThanTheOther(id, value1, value2);

		if (!check)
		{
			printErrEndTwoValues(id, value1, value2, DOES_NOT_EXIST);
		}

		return check;
	}

	void eraseRelationsFromElement(Poset &poset, const Element &elementToErase,
			const Relations &relations, const RelationType relationType)
	{
		for (const Element &elementInRelation: relations)
		{
			ElementRelations &elementRelations = poset.at(elementInRelation);
			Relations &elementInRelationRelations = getSpecificRelationType(
					elementRelations, relationType);

			ElementPointer elementToErasePointer = getElementPointerFromPoset(
					poset, elementToErase);
			elementInRelationRelations.erase(elementToErasePointer);
		}
	}

	void removeElementFromPoset(unsigned long id, char const *value)
	{
		Element element = std::string(value);
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
		bool check = checkIfElementsAreEqual(value1, value2) ||
				checkIfElementIsLessThanTheOther(id, value1, value2) ||
				checkIfElementIsLessThanTheOther(id, value2, value1);

		//TODO DEBUG POSET N, RELATION ("A", "B") CANNOT BE ADDED
		printErrRelationCannotBeAdded(id, value1, value2, check);

		return check;
	}


	bool checkIfElementBelongsToRelations(Poset &poset, Relations &relations,
			const Element &element)
	{
		ElementPointer elementPointer = getElementPointerFromPoset(poset,
				element);
		return relations.find(elementPointer) != relations.end();
	}

	void addRelationsToSingleElement(Poset &poset, const Element &elementA,
			Relations &elementBRelations, Relations &elementARelations,
			RelationType relationType)
	{
		for (const Element &elementFromElementBRelations: elementBRelations)
		{
			ElementRelations &elementRelations = poset.at(
					elementFromElementBRelations);
			Relations &specificRelations = getSpecificRelationType(
					elementRelations, relationType);

			if (checkIfElementBelongsToRelations(poset, specificRelations,
					elementA))
			{
				continue;
			}

			ElementPointer elementPointer = getElementPointerFromPoset(poset,
					elementFromElementBRelations);
			ElementPointer elementAPointer = getElementPointerFromPoset(poset,
					elementA);
			specificRelations.insert(elementAPointer);
			elementARelations.insert(elementPointer);
		}
	}

	void addRelationBetweenSingleElements(Poset &poset,
			const Element &elementA, const Element &elementB,
			Relations &elementARelations, Relations &elementBRelations)
	{
		ElementPointer elementBPointer = getElementPointerFromPoset(poset,
				elementB);
		ElementPointer elementAPointer = getElementPointerFromPoset(poset,
				elementA);
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

			ElementPointer elementPointer = getElementPointerFromPoset(poset,
					elementFromElementBRelations);
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
					getSpecificRelationType(
							relationsOfElementFromElementARelations,
							relationType);

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

	void addAndClosureRelationBetweenElements(unsigned long id,
			char const *value1, char const *value2)
	{
		Poset &poset = posets().at(id);
		Element lessElement = std::string(value1);
		Element greaterElement = std::string(value2);

		if (!checkIfElementsAreEqual(value1, value2))
		{
			linkElementsFromRelationsAndClosureRelation(poset, lessElement,
					greaterElement);
			linkLessAndGreaterElementWithRelation(poset, lessElement,
					greaterElement);
		}
	}

	bool checkIfAnyOfElementsIsLessThanTheOther(unsigned long id,
			char const *value,
			const Relations &greaterThanRelations)
	{
		for (ElementPointer element : greaterThanRelations)
		{
			if (checkIfElementIsLessThanTheOther(id, element, value) &&
				!checkIfElementsAreEqual(value, element))
			{
				return true;
			}
		}

		return false;
	}

	bool checkIfNoOtherWay(unsigned long id, char const *value1,
			char const *value2)
	{
		Poset &poset = posets().at(id);
		ElementRelations &element1Relations = poset.at(value1);
		Relations &greaterThanElement1Relations = getSpecificRelationType(
				element1Relations, GREATER_ELEMENT);

		bool check = checkIfAnyOfElementsIsLessThanTheOther(id, value2,
				greaterThanElement1Relations);

		//TODO DEBUG POSET N, RELATION ("A", "B") CANNOT BE DELETED
		printErrRelationCannotBeDeleted(id, value1, value2, !check);

		return !check;
	}

	void deleteElementsFromRelations(unsigned long id, char const *value1,
			char const *value2)
	{
		Poset &poset = posets().at(id);
		Element element1 = std::string(value1);
		Element element2 = std::string(value2);

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

		greaterThanElement1.erase(element2Pointer);
		lessThanElement2.erase(element1Pointer);
	}
}

extern "C"
{
unsigned long poset_new(void)
{
	printErrPoset_newBeg();

	Poset poset;
	unsigned long newId = nextFreeId;

	posets().insert({newId, poset});
	nextFreeId++;

	//TODO DEBUG POSET N CREATED
	printErrEndNoValue(newId, CREATED);

	return newId;
}

void poset_delete(unsigned long id)
{
	printErrBegNoValue(id, DELETE);

	if (checkIfPosetExists(id))
	{
		posets().erase(id);

		//TODO DEBUG POSET N DELETED
		printErrEndNoValue(id, DELETED);
	}
}

size_t poset_size(unsigned long id)
{
	printErrBegNoValue(id, SIZE);

	if (checkIfPosetExists(id))
	{
		Poset &poset = posets().at(id);
		size_t posetSize = poset.size();

		//TODO DEBUG POSET N CONTAINS K ELEMENT(S)
		printErrPoset_sizeEnd(id, posetSize);

		return posetSize;
	}

	return 0;
}

bool poset_insert(unsigned long id, char const *value)
{
	printErrBegOneValue(id, value, INSERT);

	if (checkIfPosetExists(id) && !checkIfValueIsNULL(value,
			ONLY_ONE_VALUE) &&
			!checkIfElementExistsInPoset(id, value,
					NOT_WANTED_IN_POSET, ONLY_ONE_VALUE))
	{
		addElementToPoset(id, value);

		//TODO DEBUG POSET N, ELEMENT "A" INSERTED
		printErrEndOneValue(id, value, INSERTED);

		return true;
	}
	else
	{
		return false;
	}
}

bool poset_remove(unsigned long id, char const *value)
{
	printErrBegOneValue(id, value, REMOVE);

	if (checkIfPosetExists(id) && !checkIfValueIsNULL(value,
			ONLY_ONE_VALUE) && checkIfElementExistsInPoset(
			id, value, WANTED_IN_POSET, ONLY_ONE_VALUE))
	{
		removeElementFromPoset(id, value);

		//TODO DEBUG POSET N, ELEMENT "A" REMOVED
		printErrEndOneValue(id, value, REMOVED);

		return true;
	}
	else
	{
		return false;
	}
}


bool poset_add(unsigned long id, char const *value1, char const *value2)
{
	printErrBegTwoValues(id, value1, value2, ADD);

	if (checkIfPosetExists(id) &&
			!checkIfAnyElementIsNULL(value1, value2) &&
			checkIfBothElementsExistInPoset(id, value1, value2) &&
			!checkIfElementsAreInRelation(id, value1, value2))
	{
		addAndClosureRelationBetweenElements(id, value1, value2);

		//TODO DEBUG POSET N, RELATION ("A", "B") ADDED
		printErrEndTwoValues(id, value1, value2, ADDED);

		return true;
	}
	else
	{
		return false;
	}
}

bool poset_del(unsigned long id, char const *value1, char const *value2)
{
	printErrBegTwoValues(id, value1, value2, DEL);

	if (checkIfPosetExists(id) &&
			!checkIfAnyElementIsNULL(value1, value2) &&
			checkIfBothElementsExistInPoset(id, value1, value2) &&
			!checkIfReflexiveRelation(id, value1, value2) &&
			checkIfElementIsLessThanTheOtherForDel(id, value1, value2) &&
			checkIfNoOtherWay(id, value1, value2))
	{
		deleteElementsFromRelations(id, value1, value2);

		//TODO DEBUG POSET N, RELATION ("A", "B") DELETED
		printErrEndTwoValues(id, value1, value2, DELETED);

		return true;
	}
	else
	{
		return false;
	}
}

bool poset_test(unsigned long id, char const *value1, char const *value2)
{
	printErrBegTwoValues(id, value1, value2, TEST);

	if (!checkIfAnyElementIsNULL(value1, value2) &&
			checkIfPosetExists(id) &&
			checkIfBothElementsExistInPoset(id, value1, value2))
	{
		bool check = checkIfElementIsLessThanTheOther(id, value1, value2);

		//TODO DEBUG POSET N, RELATION ("A", "B") EXISTS
		//TODO DEBUG POSET N, RELATION ("A", "B") DOES NOT EXIST
		printErrRelationExistsOrNotExists(id, value1, value2, check);

		return check;
	}
	else
	{
		return false;
	}
}

void poset_clear(unsigned long id)
{
	printErrBegNoValue(id, CLEAR);

	if (checkIfPosetExists(id))
	{
		Poset &poset = posets().at(id);
		poset.clear();

		//TODO DEBUG POSET N CLEARED
		printErrEndNoValue(id, CLEARED);
	}
}
}

int main()
{
	unsigned long p1;

	p1 = poset_new();
//	assert(poset_size(p1) == 0);
//	assert(poset_size(p1 + 1) == 0);
//	assert(!poset_insert(p1, NULL));
//	assert(poset_insert(p1, "A"));
//	assert(poset_test(p1, "A", "A"));
//	assert(!poset_insert(p1, "A"));
//	assert(!poset_insert(p1 + 1, "B"));
//	assert(poset_size(p1) == 1);
//	assert(!poset_remove(p1 + 1, "A"));
//	assert(poset_remove(p1, "A"));
//	assert(!poset_remove(p1, "A"));
//	assert(poset_insert(p1, "B"));
//	assert(poset_insert(p1, "C"));
//	assert(poset_add(p1, "B", "C"));
//	assert(!poset_remove(p1, "A"));
//	assert(!poset_add(p1, NULL, "X"));
//	assert(!poset_del(p1, NULL, "X"));
//	assert(!poset_test(p1, NULL, "X"));
//	assert(!poset_add(p1, "X", NULL));
//	assert(!poset_del(p1, "X", NULL));
//	assert(!poset_test(p1, "X", NULL));
//	assert(!poset_add(p1, NULL, NULL));
//	assert(!poset_del(p1, NULL, NULL));
//	assert(!poset_test(p1, NULL, NULL));
//	assert(!poset_add(p1, "C", "D"));
//	assert(!poset_add(p1, "D", "C"));
//	assert(!poset_del(p1, "C", "D"));
//	assert(!poset_del(p1, "D", "C"));
//	assert(!poset_test(p1, "C", "D"));
//	assert(!poset_test(p1, "D", "C"));
//	assert(!poset_add(p1 + 1, "C", "D"));
//	assert(!poset_del(p1 + 1, "C", "D"));
//	assert(!poset_test(p1 + 1, "C", "D"));
//	poset_clear(p1);
//	poset_clear(p1 + 1);
	assert(poset_insert(p1, "E"));
	assert(poset_insert(p1, "F"));
	assert(poset_insert(p1, "G"));
	assert(poset_add(p1, "E", "F"));
	assert(!poset_add(p1, "E", "F"));
	assert(!poset_add(p1, "F", "E"));
	assert(poset_test(p1, "E", "F"));
	assert(!poset_test(p1, "F", "E"));
	assert(poset_add(p1, "F", "G"));
	assert(poset_test(p1, "E", "G"));
	assert(!poset_del(p1, "E", "G"));
	assert(poset_del(p1, "E", "F"));
	assert(!poset_del(p1, "E", "F"));

	assert(!poset_del(p1, "G", "F"));
	assert(!poset_del(p1, "G", "G"));
	assert(poset_size(p1) == 3);
	poset_delete(p1);
	poset_delete(p1);
	poset_delete(p1 + 1);

	return 0;

}