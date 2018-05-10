#include "shalloc.hpp"
#include "gtest/gtest.h"
#include <dlmalloc.h>
#include <iostream>

#include <map>
#include <set>
#include <unordered_map>

using namespace nano;

struct Example
{
	int value;

	Example (int v) :
	value (v)
	{
	}

	bool operator< (Example const & other) const
	{
		return value < other.value;
	}
};

void alloc_too_many ()
{
	std::set<Example, std::less<Example>, nalloc<Example>> foo;
	for (int i = 0; i < 100000; i++)
	{
		foo.insert (Example (i));
	}
}
// Disabled: cannot recover from the test exhausting memory?
TEST (Alloc, DISABLED_SetWithDeallocAtTheEnd)
{
	EXPECT_THROW (alloc_too_many (), std::bad_alloc);
	// Dellocated
}

TEST (Alloc, SetWithDealloc)
{
	for (int i = 0; i < 100000; i++)
	{
		// Increase scope
		{
			std::set<Example, std::less<Example>, nalloc<Example>> foo;
			foo.insert (Example (i));
		}
	}
}

TEST (Map, MapOfStrings)
{
	//typedef nalloc<std::pair<const shm_string,shm_string>> strmap_alloc;
	//using strmap = std::unordered_map<shm_string,shm_string, std::hash<shm_string>, std::equal_to<shm_string>, strmap_alloc>;

	auto map = new (dlmalloc (sizeof (umap<shm_string, shm_string>))) umap<shm_string, shm_string> ();
	auto & mapref = *map;
	mapref["hi"] = "og hå";
	mapref["test"] = shm_string ("og hå");
	for (auto & entry : mapref)
	{
		std::cout << entry.first << " = " << entry.second << std::endl;
	}
	//	map->insert(std::make_pair(*key, *value));

	//shm_string key("demo");
	//(*map)[key] = shm_string("value");

	map->~umap<shm_string,shm_string>();
	dlfree(map);
}

TEST (Map, MapOfInts)
{
	//typedef nalloc<std::pair<const shm_string,shm_string>> strmap_alloc;
	//using strmap = std::unordered_map<shm_string,shm_string, std::hash<shm_string>, std::equal_to<shm_string>, strmap_alloc>;

	auto map = new (dlmalloc (sizeof (umap<int, int>))) umap<int, int> ();
	map->insert (std::make_pair (1, 2));

	//shm_string key("demo");
	//(*map)[key] = shm_string("value");

	map->~umap<int,int>();
	dlfree (map);
}

TEST (C, DISABLED_Plain)
{
	for (int i = 0; i < 100; i++)
	{
		char * str = (char *)dlmalloc (1024);
		if (str)
		{
			std::cout << "Got shared memory!" << std::endl;
			strcpy (str, "test string");
			std::cout << "Test:" << str << std::endl;
		}
		else
		{
			std::cout << "Out of shared memory!" << std::endl;
			break;
		}
	}
}
