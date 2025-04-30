#include "../headers/stl_helper_functions.h"

template <typename T> 
static void remove_duplicates(std::vector<T> *v)
{
	std::unordered_set<T> *temp = new std::unordered_set<T>();

	helper const int size = v->size();
	for (int i = 0; i < size; ++i) temp->insert((*v)[i]);
	v->clear();

	helper const std::unordered_set<int>::iterator end = temp->end();
	for (std::unordered_set<int>::iterator it = temp->begin(); it != end; ++it)
	{
		v->emplace_back(*it);
	}

	delete temp;
}