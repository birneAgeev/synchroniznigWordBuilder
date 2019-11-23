#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <unordered_set>
#include <string>

#include "ClusterStructure.h"
#include "Utils.h"

int theLength;
std::string theWord;

std::vector<int> BuildSingleLetterGraph(const Graph &automaton, int letter)
{
	std::vector<int> result;
	for (int i = 0; i < automaton.size(); ++i)
	{
		result.push_back(automaton[i][letter]);
	}

	return result;
}

std::vector<int> GetIdentityMapping(int n)
{
	std::vector<int> result;
	for (int i = 0; i < n; ++i)
	{
		result.push_back(i);
	}

	return result;
}

struct CompressionArtifact
{
	CompressionArtifact(
		const ClusterStructure &compressedMappingClusterStructure,
		const std::vector<int> &statesCompressingMapping,
		const std::vector<int> &compressedMapping)
		: compressedMappingClusterStructure(compressedMappingClusterStructure),
		  statesCompressingMapping(statesCompressingMapping),
		  compressedMapping(compressedMapping) {}

	ClusterStructure compressedMappingClusterStructure;
	std::vector<int> statesCompressingMapping;
	std::vector<int> compressedMapping;
};

CompressionArtifact CompressMapping(
	const Graph &automaton,
	const std::vector<int> &mappingToCompress,
	const std::vector<int> &previousStatesCompressingMapping,
	int compressingWordParticleLength,
	int letter)
{

	int n = automaton.size();
	auto clusterStructure = ClusterStructure(mappingToCompress);
	int compressingWordLength = clusterStructure.GetHighestTreeHeight();

	auto statesCompressingMapping = std::vector<int>();
	for (int i = 0; i < n; ++i)
	{
		statesCompressingMapping.push_back(previousStatesCompressingMapping[i]);
	}
	for (int i = 0; i < compressingWordLength; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			statesCompressingMapping[j] = mappingToCompress[statesCompressingMapping[j]];
		}
	}

	auto vertexInfos = clusterStructure.GetVertexInfos();

	int nonCyclic = 0;
	std::vector<int> compressedMapping = std::vector<int>();
	for (int i = 0; i < n; ++i)
	{
		if (!vertexInfos[i].IsCyclicState())
		{
			++nonCyclic;
			compressedMapping.push_back(-1);
			continue;
		}

		int toState = i;
		for (int j = 0; j < compressingWordParticleLength; ++j)
		{
			toState = automaton[toState][letter];
		}
		compressedMapping.push_back(statesCompressingMapping[toState]);
	}
	//PrintInGraphViz(compressedMapping, "output_compressed.jpg");

	auto compressedMappingClusterStructure = ClusterStructure(compressedMapping);
	theLength += compressedMappingClusterStructure.GetHighestTreeHeight() * (1 + theLength);

	return CompressionArtifact(compressedMappingClusterStructure, statesCompressingMapping, compressedMapping);
}

bool TrySinchronizePairwize(
	const Graph &automaton,
	const std::vector<int> &mappingToCompress,
	const std::vector<int> &previousStatesCompressingMapping)
{

	int n = automaton.size();
	std::unordered_set<int> statesToSynchronize;
	for (int i = 0; i < n; ++i)
	{
		if (mappingToCompress[i] != -1)
			statesToSynchronize.insert(i);
	}

	int maxCompressingWordLength = 100; // std::ceil(sqrt(std::ceil(sqrt(n))));

	auto statesCompressingMapping = std::vector<int>();
	for (int i = 0; i < n; ++i)
	{
		statesCompressingMapping.push_back(previousStatesCompressingMapping[i]);
	}

	while (statesToSynchronize.size() != 1)
	{
		auto iterator = statesToSynchronize.begin();
		int p = *iterator;
		++iterator;
		int q = *iterator;

		int syncWordLength = -1;
		for (int i = 0; i < maxCompressingWordLength; ++i)
		{
			p = automaton[p][1];
			q = automaton[q][1];

			if (statesCompressingMapping[p] == statesCompressingMapping[q])
			{
				syncWordLength = i + 1;
				break;
			}
		}

		if (syncWordLength == -1)
			return false;

		auto newStatesCompressingMapping = std::vector<int>();

		for (int i = 0; i < n; ++i)
		{
			int toState = i;
			for (int j = 0; j < syncWordLength; ++j)
			{
				toState = automaton[toState][1];
			}
			newStatesCompressingMapping.push_back(statesCompressingMapping[toState]);
		}
		statesCompressingMapping = newStatesCompressingMapping;

		std::unordered_set<int> newStatesToSynchronize;
		for (const auto &state : statesToSynchronize)
		{
			newStatesToSynchronize.insert(statesCompressingMapping[state]);
		}
		statesToSynchronize = newStatesToSynchronize;
	}

	return true;
}

int MagicFunction(int n)
{
	return 2 * sqrt(n * log2(n));
}

int main()
{
	freopen("input.txt", "rt", stdin);
	freopen("output.txt", "wt", stdout);

	auto qxx = GenerateRandomAutomaton(16, 2);
	auto zzz = BuildSingleLetterGraph(qxx, 0);
	PrintInGraphViz(zzz, "output_compressed.jpg");
	return 0;

	int n = 1000;
	int sigma = 2;
	int magicNumber = MagicFunction(MagicFunction(MagicFunction(n)));
	int experimentsCount = 10000;

	printf("Magic number: %d\n", magicNumber);
	double avgCycleLen = 0;
	double avgHeight = 0;
	double avgWordLength = 0;
	int synchronized = 0;
	for (int i = 0; i < experimentsCount; ++i)
	{
		auto a = GenerateRandomAutomaton(n, sigma);
		theLength = 0;
		theWord = "";

		auto singleLetterMapping = BuildSingleLetterGraph(a, 0);
		auto stateCompressingMapping = GetIdentityMapping(n);
		//auto initClusterStructure = ClusterStructure(singleLetterMapping);
		//for (int j = 0; j < initClusterStructure.GetHighestTreeHeight(); ++j) {
		//	theWord += 'a';
		//}

		auto clusterStructure1 = ClusterStructure(singleLetterMapping);
		int compressingWordLength = clusterStructure1.GetHighestTreeHeight();
		theLength += compressingWordLength * (1 + theLength);
		//std::string s = "";
		//for (int i = 0; i < 1; ++i) {
		//	s += 'a';
		//}
		//auto prev = theWord;
		//for (int i = 0; i < compressingWordLength; ++i) {
		//	theWord += s + prev;
		//}
		//theWord = prev + theWord;

		auto artifacts = CompressMapping(a, singleLetterMapping, stateCompressingMapping, 1, 1);

		ClusterStructure clusterStructure;
		int cycleStatesCount;
		for (int i = 2; theLength < n * n && i < 100; ++i)
		{
			artifacts = CompressMapping(a, artifacts.compressedMapping, artifacts.statesCompressingMapping, i, 1);

			clusterStructure = artifacts.compressedMappingClusterStructure;
			cycleStatesCount = 0;
			for (const auto &clusterInfo : clusterStructure.GetClusterInfos())
			{
				cycleStatesCount += clusterInfo.cycleLength;
			}
			if (cycleStatesCount == 1)
				break;
		}

		bool isSynchronized = TrySinchronizePairwize(a, artifacts.compressedMapping, artifacts.statesCompressingMapping);
		//isSynchronized = false;

		avgCycleLen += cycleStatesCount;
		avgHeight += clusterStructure.GetHighestTreeHeight();

		if (isSynchronized || cycleStatesCount == 1)
		{
			++synchronized;
			avgWordLength += theLength;
		}
		//printf("Height: %d, Cycle length: %d\n", clusterStructure.GetHighestTreeHeight(), cycleStatesCount);
		//printf("Word length: %d, %d\n", theLength, cycleStatesCount);

		if (std::max(clusterStructure.GetHighestTreeHeight(), cycleStatesCount) > magicNumber)
		{
			printf("ALARM!! %d, %d", i, std::max(clusterStructure.GetHighestTreeHeight(), cycleStatesCount));
			return 0;
		}
		//break;
	}

	printf("avgHeight: %.2lf, avgCycleLen: %.2lf, length: %.2lf, synchronized: %d", avgHeight / experimentsCount, avgCycleLen / experimentsCount, avgWordLength / experimentsCount, synchronized);
}