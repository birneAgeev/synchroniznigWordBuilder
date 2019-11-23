#pragma once
#include <vector>
#include <stack>
#include <algorithm>
#include <cmath>

#include "TypeDefinitions.h"

class ClusterStructure
{
public:
	struct VertexClusterInfo;
	struct ClusterInfo;

private:
	std::vector<VertexClusterInfo> vertexInfos;
	std::vector<ClusterInfo> clusterInfos;
	int clusterCount;
	std::vector<int> mapping;
	Graph invertedMapping;
	int highestTreeHeight;

public:
	struct VertexClusterInfo
	{
		int clusterIndex;
		int closestCycleState;
		int height;

		VertexClusterInfo()
			: clusterIndex(-1), closestCycleState(-1), height(-1){};

		VertexClusterInfo(int clusterIndex, int closestCycleState, int height)
			: clusterIndex(clusterIndex), closestCycleState(closestCycleState), height(height) {}

		bool IsCyclicState() const
		{
			return height == 0;
		}

		void Print() const
		{
			printf("clusterIndex: %d, closestCycleState: %d, height: %d\n", clusterIndex, closestCycleState, height);
		}
	};

	struct ClusterInfo
	{
		size_t clusterSize;
		size_t cycleLength;
		std::vector<int> cycleStates;

		ClusterInfo()
			: clusterSize(-1), cycleLength(-1), cycleStates(std::vector<int>()){};

		ClusterInfo(int clusterSize, int cycleLength)
			: clusterSize(clusterSize), cycleLength(cycleLength), cycleStates(std::vector<int>()) {}

		void Print() const
		{
			printf("clusterSize: %d, cycleLength: %d, cycleStates: [", (int)clusterSize, (int)cycleLength);
			for (size_t i = 0; i < cycleStates.size(); ++i)
			{
				printf("%d ", cycleStates[i]);
			}
			printf("]\n");
		}
	};

	ClusterStructure()
	{
		vertexInfos = std::vector<VertexClusterInfo>();
		clusterInfos = std::vector<ClusterInfo>();
		invertedMapping = std::vector<std::vector<int>>();
		mapping = std::vector<int>();
		clusterCount = 0;
		highestTreeHeight = -1;
	}

	ClusterStructure(const std::vector<int> &mapping)
	{
		this->mapping = mapping;
		highestTreeHeight = 0;

		auto stateCount = mapping.size();
		invertedMapping.resize(stateCount);
		vertexInfos.resize(stateCount);

		BuildSingleLetterGraphs();

		std::vector<bool> used(stateCount, false);
		std::vector<bool> isCycleState(stateCount, false);
		clusterCount = 0;

		for (size_t state = 0; state < stateCount; ++state)
		{
			if (used[state] || mapping[state] == -1)
				continue;

			++clusterCount;
			auto clusterInfo = ProcessCluster(state, used, isCycleState);
			clusterInfos.push_back(clusterInfo);
		}
	}

	void Print() const
	{
		printf("custerCunt: %d\n", clusterCount);
		printf("highestTreeHeight: %d\n", highestTreeHeight);
		printf("vertexInfos:\n");
		for (int i = 0; i < vertexInfos.size(); ++i)
		{
			printf("#%d: ", i);
			vertexInfos[i].Print();
		}

		printf("clusterInfos:\n");
		for (const auto &clusterInfo : clusterInfos)
		{
			clusterInfo.Print();
		}
		printf("**********************");
	}

	int GetHighestTreeHeight() const
	{
		return highestTreeHeight;
	}

	std::vector<ClusterInfo> GetClusterInfos() const
	{
		return clusterInfos;
	}

	std::vector<VertexClusterInfo> GetVertexInfos() const
	{
		return vertexInfos;
	}

private:
	void BuildSingleLetterGraphs()
	{
		for (size_t i = 0; i < mapping.size(); ++i)
		{
			if (mapping[i] != -1)
				invertedMapping[mapping[i]].push_back(i);
		}
	}

	ClusterInfo ProcessCluster(int startState, std::vector<bool> &used, std::vector<bool> &isCycleState)
	{
		auto cycleStates = FindCycle(startState, used, isCycleState);

		auto currentClusterInfo = ClusterInfo();
		currentClusterInfo.cycleLength = cycleStates.size();
		currentClusterInfo.clusterSize = currentClusterInfo.cycleLength;
		currentClusterInfo.cycleStates = cycleStates;

		for (size_t i = 0; i < cycleStates.size(); ++i)
		{
			int root = cycleStates[i];
			vertexInfos[root] = VertexClusterInfo(clusterCount, root, 0);
			for (size_t j = 0; j < invertedMapping[root].size(); ++j)
			{
				int to = invertedMapping[root][j];
				if (isCycleState[to])
					continue;

				int height = TreeDfs(to, used, root, clusterCount, currentClusterInfo.clusterSize, 1);

				if (height >= highestTreeHeight)
					highestTreeHeight = height;
			}
		}

		return currentClusterInfo;
	}

	int TreeDfs(int v, std::vector<bool> &used, int root, int clusterIndex, size_t &clusterSize, int height)
	{
		used[v] = true;
		++clusterSize;
		vertexInfos[v] = VertexClusterInfo(clusterIndex, root, height);
		int ans = height;

		for (size_t i = 0; i < invertedMapping[v].size(); ++i)
		{
			int to = invertedMapping[v][i];
			if (used[to])
				continue;

			int subtreeHeight = TreeDfs(to, used, root, clusterIndex, clusterSize, height + 1);
			ans = std::max(ans, subtreeHeight);
		}

		return ans;
	}

	std::vector<int> FindCycle(int startState, std::vector<bool> &used, std::vector<bool> &isCycleState)
	{
		std::stack<int> stack;
		int currentState = startState;
		while (!used[currentState])
		{
			used[currentState] = true;
			stack.push(currentState);
			currentState = mapping[currentState];
		}

		std::vector<int> cycleStates;
		int currentTreeRoot = currentState;

		while (!stack.empty() && stack.top() != currentTreeRoot)
		{
			cycleStates.push_back(stack.top());
			isCycleState[stack.top()] = true;
			stack.pop(); //enumerate cycle states
		}
		stack.pop();
		cycleStates.push_back(currentTreeRoot);
		isCycleState[currentTreeRoot] = true;
		std::reverse(cycleStates.begin(), cycleStates.end());

		while (!stack.empty())
		{
			used[stack.top()] = false; //unuse non-cycle states
			stack.pop();
		}

		return cycleStates;
	}
};