#include "AlgorithmDBSCAN.h"


AlgorithmDBSCAN::AlgorithmDBSCAN(const double eps_, const unsigned int minPits_, const unsigned int size_)
{
	m_eps = eps_;
	m_minPts = minPits_;
	m_currentCluster = -1;
	for (int i = 0; i < size_; i++)
	{
		m_clusters.push_back(-1);
		m_noisePoints.push_back(false);
		m_undefinedPoints.push_back(true);

		m_isInConnetions.push_back(false);
		std::vector<int> indexes;
		m_connectionsArray.push_back(indexes);
	}
}

std::vector<int> AlgorithmDBSCAN::startClustering(std::vector<std::vector<double>> dataset)
{
	createGraph(dataset);
	for (int pIndex = 0; pIndex < dataset.size(); pIndex++)
	{
		//std::cout << "running pIndex  : " << pIndex << std::endl;
		if (!m_isInConnetions[pIndex])
			continue;
		if (m_undefinedPoints[pIndex] == false)
		{
			continue;
		}
		auto neighbors = m_connectionsArray[pIndex];
		if (neighbors.size() < m_minPts)
		{
			m_noisePoints[pIndex] = true;
			m_undefinedPoints[pIndex] = false;
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
			auto qNeighbors = m_connectionsArray[qIndex];
			if (qNeighbors.size() >= m_minPts)
			{
				for (auto value : qNeighbors)
					seedSet.push_back(value);
			}
		}
	}

	return m_clusters;
}

inline std::vector<int> AlgorithmDBSCAN::rangeQuery(std::vector<std::vector<double>> dataset, int pIndex)
{
	/*
	std::vector<int> neighbors;
	for (int qIndex = 0; qIndex < dataset.size(); qIndex++)
	{
		if (qIndex == pIndex || calcDistance(dataset[pIndex], dataset[qIndex]) <= m_eps)
		{
			neighbors.push_back(qIndex);
		}
	}
	*/
	return m_connectionsArray[pIndex];
}

void AlgorithmDBSCAN::createGraph(std::vector<std::vector<double>> dataset)
{
	auto start = high_resolution_clock::now();
	//
	initgridDictionaryVectors(dataset);
	//
	auto stop = high_resolution_clock::now();
	seconds duration = duration_cast<seconds>(stop - start);
	//cout << "initgridDictionaryVectors() time last: " << duration.count() << " seconds" << endl;

	start = high_resolution_clock::now();
	//
	initGraph();
	//
	stop = high_resolution_clock::now();
	duration = duration_cast<seconds>(stop - start);
	//cout << "initGraph() time last: " << duration.count() << " seconds" << endl;

}
void AlgorithmDBSCAN::zipGrid()
{
	for (int currentKey : m_actualKeys)
	{
		for (int key = currentKey - m_eps; key <= (currentKey + m_eps); key++)
		{
			if (key < 0 || currentKey == key)
				continue;
			//check if key is in m_actualKeys
			if (isKeyActual(key) == false)
				continue;
			uniteVectors(m_gridDictionaryIndexes.find(currentKey)->second, m_gridDictionaryIndexes.find(key)->second);
			uniteVectors(m_gridDictionaryVectors.find(currentKey)->second, m_gridDictionaryVectors.find(key)->second);
		}
	}
}
void AlgorithmDBSCAN::initgridDictionaryVectors(std::vector<std::vector<double>> dataset)
{
	int currentKey;
	for (int pIndex = 0; pIndex < dataset.size(); pIndex++)
	{
		currentKey = calcAvg(dataset[pIndex]);
		m_actualKeys.insert(currentKey);
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

	zipGrid();
}


void AlgorithmDBSCAN::connectNodes()
{

}


void AlgorithmDBSCAN::searchForConnections(const int startIndex, const int endIndex, const double eps, std::vector<std::vector<double>> vectorList, std::vector<int> indexes, std::map<int, std::vector<int>> *connectionMap)
{
	int pIndex, qIndex;
	//for each point
	for (int i = startIndex; i < endIndex; i++)
	{
		pIndex = indexes[i];
		//calcualte distance to all neighbors on same grid group
		for (int j = 0; j < vectorList.size(); j++)
		{
			//they are in range, connect them by their real indexes
			if (calcDistance(vectorList[i], vectorList[j]) <= eps)
			{
				qIndex = indexes[j];
				auto it = connectionMap->find(pIndex);
				if (it == connectionMap->end())
				{
					std::vector<int> connections;
					connectionMap->insert({ pIndex , connections });
				}
				connectionMap->find(pIndex)->second.push_back(qIndex);

			}
		}
	}
}


void AlgorithmDBSCAN::initGraph()
{

	for (int frr = 0; frr < m_actualKeys.size(); frr++) //frr -> free running
	{
		//pull the key from a set in index frr
		std::set<int>::iterator it = m_actualKeys.begin();
		std::advance(it, frr);
		int key = *it;
		//running witth threads to find connections to our points
		//to enable independence for each thread we will use 4 diffrent maps
		//eachc thread wil lcalculate distance to other points and check if they are connected, if so connect them
		std::map<int, std::vector<int>> connectionMaps[4];
		std::thread threadArray[4];
		int size = static_cast<int> (m_gridDictionaryVectors.find(key)->second.size() / 4.0);
		int maxSize = static_cast<int> (m_gridDictionaryVectors.find(key)->second.size());

		auto start = high_resolution_clock::now();


		for (int i = 0; i < 4; i++)
		{
			std::vector<std::vector<double>> vectorList = m_gridDictionaryVectors.find(key)->second;
			std::vector<int> indexes = m_gridDictionaryIndexes.find(key)->second;

			if (i == 3)
			{
				threadArray[i] = std::thread(searchForConnections, i* size, maxSize, m_eps, vectorList, indexes, &connectionMaps[i]);
				//threadArray[i] = std::thread(connectNodes);
			}
			else
				threadArray[i] = std::thread(searchForConnections, i* size, (i+1)*size, m_eps, vectorList, indexes, &connectionMaps[i]);
				//threadArray[i] = std::thread(connectNodes);
			
		}

		//wait for all threads to finish
		for (int i = 0; i < 4; i++)
		{
			threadArray[i].join();
		}
	
	
		//after we ran on all the points we will merge the results for DBSCAN graph
		for (int i = 0; i < 4; i++)
		{
			for (std::map<int, std::vector<int>>::iterator iter = connectionMaps[i].begin(); iter != connectionMaps[i].end(); ++iter)
			{
				int key = iter->first;
				std::vector<int> connections = iter->second;
				if (connections.size() >= m_minPts)
				{
					//m_connectionsMap.insert({ key , connections });
					m_isInConnetions[key] = true;
					m_connectionsArray[key] = connections;

				}
					
			}
		}

		auto stop = high_resolution_clock::now();
		seconds duration = duration_cast<seconds>(stop - start);
		//cout << "running with thrads time last: " << duration.count() << " seconds" << endl;

	}
	
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







int AlgorithmDBSCAN::calcAvg(std::vector<double> vec)
{
	int sum = 0;
	for (int i = 0; i < vec.size(); i++)
	{
		sum += vec[i];
	}
	return (int)(sum / vec.size());
}

bool AlgorithmDBSCAN::isKeyActual(int key)
{
	std::set<int>::iterator it = m_actualKeys.find(key);
	// Check if iterator it is valid
	return it != m_actualKeys.end();
}

template<class T>
void AlgorithmDBSCAN::uniteVectors(std::vector<T> v1, std::vector<T> v2)
{
	int minSize = 0;
	if (v1.size() == v2.size())
	{
		for (int i = 0; i < v1.size(); i++)
		{
			v1.push_back(v2[i]);
			v2.push_back(v1[i]);
		}
		return;
	}
	if (v1.size() < v2.size())
	{
		minSize = v1.size();
		for (int i = 0; i < minSize; i++)
		{
			v1.push_back(v2[i]);
			v2.push_back(v1[i]);
		}
		for (int i = minSize; i < v2.size(); i++)
			v1.push_back(v2[i]);
		return;
	}

	minSize = v2.size();
	for (int i = 0; i < minSize; i++)
	{
		v1.push_back(v2[i]);
		v2.push_back(v1[i]);
	}
	for (int i = minSize; i < v1.size(); i++)
		v2.push_back(v1[i]);
}



