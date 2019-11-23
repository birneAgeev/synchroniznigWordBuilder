#pragma once
#define NOMINMAX
#include <string>
#include <fstream>
#include <random>
#include <iostream>
#include <cstdlib>

#include "TypeDefinitions.h"

// const std::random_device random_device();

// std::mt19937 eng(random_device());

// inline unsigned int RandomNumber(unsigned int min, unsigned int max) {
// 	return std::uniform_int_distribution<unsigned int>(min, max)(eng);
// }

inline Graph GenerateRandomAutomaton(int n, int sigma)
{
	Graph result;
	result.resize(n);

	for (int i = 0; i < sigma; ++i)
	{
		for (int v = 0; v < n; ++v)
		{
			//result[v].push_back(RandomNumber(0, n - 1));
			result[v].push_back(rand() % n);
		}
	}

	return result;
}

inline void PrintInGraphViz(const std::vector<int> &mapping, const std::string &fileName)
{
	std::string dot = "digraph G{\n";
	for (size_t i = 0; i < mapping.size(); ++i)
	{
		if (mapping[i] == -1)
			continue;

		dot += " " + std::to_string(i) + "->" + std::to_string(mapping[i]) + ";\n";
	}
	dot += "}";

	std::ofstream out(fileName.c_str());
	out << dot;
	out.close();

	system(("C:\\Program Files (x86)\\Graphviz2.38\\bin\\sfdp.exe " + fileName + " + -Tjpg -O").c_str());
}