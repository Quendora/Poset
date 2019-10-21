#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <string>
// tymczasowe, zeby przejrzysciej sie pisalo
using namespace std;

using Id = unsigned int;
using Element = string;
using Relations = unordered_set<Element>;
using GreaterThanRelations = Relations;
using LessThanRelations = Relations;
using ElementRelations = pair<GreaterThanRelations, LessThanRelations>;
using Poset = unordered_map<Element, ElementRelations>;
using Posets = unordered_map<Id, Poset>;

int main()
{

}