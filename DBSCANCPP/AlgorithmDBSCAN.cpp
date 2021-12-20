#include "AlgorithmDBSCAN.h"


AlgorithmDBSCAN::AlgorithmDBSCAN(const float eps_, const unsigned int minPits_, const unsigned int size_)
{
	m_eps = eps_;
	m_minPts = minPits_;
	m_currentCluster = -1;
	for (int i = 0; i < size_; i++)
	{
		m_clusters.push_back(-1);
		m_noisePoints.push_back(false);
		m_undefinedPoints.push_back(true);
	}
}

std::vector<int> AlgorithmDBSCAN::startClustering(std::vector<std::vector<double>> dataset)
{
	createGraph(dataset);
	for (int pIndex = 0; pIndex < dataset.size(); pIndex++)
	{
		std::cout << "running pIndex  : " << pIndex << std::endl;
		if (m_undefinedPoints[pIndex] == false)
		{
			continue;
		}
		auto neighbors = rangeQuery(dataset, pIndex);
		if (neighbors.size() < m_minPts)
		{
			m_noisePoints[pIndex] = true;
			continue;
		}
		m_currentCluster++;
		m_clusters[pIndex] = m_currentCluster;
		m_undefinedPoints[pIndex] = false;

		auto seedSet = neighbors;
		for (int i = 0 ;i < seedSet.size(); i++)
		{
			int qIndex = seedSet[i];
			if (m_noisePoints[qIndex] == true)
			{
				m_clusters[qIndex] = m_currentCluster;
				m_noisePoints[qIndex] = false;
			}
			if (m_undefinedPoints[qIndex] == false)
				continue;
			m_clusters[qIndex] = m_currentCluster;
			m_undefinedPoints[qIndex] = false;
			auto qNeighbors = rangeQuery(dataset, qIndex);
			if (qNeighbors.size() >= m_minPts)
			{
				for (auto value : qNeighbors)
					seedSet.push_back(value);
			}
		}
	}

	return m_clusters;
}

std::vector<int> AlgorithmDBSCAN::rangeQuery(std::vector<std::vector<double>> dataset, int pIndex)
{
	std::vector<int> neighbors;
	for (int qIndex = 0; qIndex < dataset.size(); qIndex++)
	{
		if (qIndex == pIndex || calcDistance(dataset[pIndex], dataset[qIndex]) <= m_eps)
		{
			neighbors.push_back(qIndex);
		}
	}
	return neighbors;
}
double AlgorithmDBSCAN::calcDistance(std::vector<double> p, std::vector<double> q)
{
	double sum = 0;
	double sub = 0;
	for (int i = 0; i < p.size(); i++)
	{
		sub = p[i] - q[i];
		sum += (sub * sub);
	}
	return pow(sum, 0.5);
}
void AlgorithmDBSCAN::createGraph(std::vector<std::vector<double>> dataset)
{
	auto start = high_resolution_clock::now();
	//
	initgridDictionaryVectors(dataset);
	//
	auto stop = high_resolution_clock::now();
	seconds duration = duration_cast<seconds>(stop - start);
	cout << "initgridDictionaryVectors() time last: " << duration.count() << " seconds" << endl;

	start = high_resolution_clock::now();
	//
	initGraph();
	//
	stop = high_resolution_clock::now();
	duration = duration_cast<seconds>(stop - start);
	cout << "initGraph() time last: " << duration.count() << " seconds" << endl;

}
void AlgorithmDBSCAN::zipGrid()
{

}
void AlgorithmDBSCAN::initgridDictionaryVectors(std::vector<std::vector<double>> dataset)
{
	int currentKey;
	for (int pIndex = 0; pIndex < dataset.size(); pIndex++)
	{
		currentKey = calcAvg(dataset[pIndex]);
		m_actualKeys.push_back(currentKey);
		auto it = m_gridDictionaryVectors.find(currentKey);
		if (it == m_gridDictionaryVectors.end())
		{
			std::vector<int> indexes;
			std::vector<std::vector<double>> vectors;
			m_gridDictionaryIndexes.insert({ currentKey ,indexes });
			m_gridDictionaryVectors.insert({ currentKey ,vectors });
		}
		m_gridDictionaryIndexes.find(currentKey)->second.push_back(pIndex);
		m_gridDictionaryVectors.find(currentKey)->second.push_back(dataset[pIndex]);
	}

}
void AlgorithmDBSCAN::connectNodes()
{

}
void AlgorithmDBSCAN::initGraph()
{

}

int AlgorithmDBSCAN::calcAvg(std::vector<double> vec)
{
	int sum = 0;
	for (int i = 0; i < vec.size(); i++)
	{
		sum += vec[i];
	}
	return (int)(sum / vec.size());
}

