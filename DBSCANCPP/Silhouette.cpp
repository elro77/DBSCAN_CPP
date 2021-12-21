#include "Silhouette.h"




Silhouette::Silhouette()
{
}

double Silhouette::calculateSilhouetteValue(std::vector<std::vector<double>> dataset, std::vector<int> clusters)
{
	createClustersDictionaryIndexes(dataset, clusters);

	createClusterGravityPoint(dataset);

	findClusterPairs();

	return 0.0;
}

//this function will organize the indexes 
void Silhouette::createClustersDictionaryIndexes(std::vector<std::vector<double>> dataset, std::vector<int> clusters)
{
	int pIndex = -1;
	for (int cluster : clusters)
	{
		pIndex++;
		//if the point is a noise
		if (cluster == -1)
			continue;

		//add pIndex to cluster map
		auto it = m_clusterDictionaryIndexes.find(cluster);
		if (it == m_clusterDictionaryIndexes.end())
		{
			std::vector<int> indexes;
			m_clusterDictionaryIndexes.insert({ cluster , indexes });
		}
		m_clusterDictionaryIndexes.find(cluster)->second.push_back(pIndex);
	}
}

void Silhouette::createClusterGravityPoint(std::vector<std::vector<double>> dataset)
{
	for (std::map<int, std::vector<int>>::iterator iter = m_clusterDictionaryIndexes.begin(); iter != m_clusterDictionaryIndexes.end(); ++iter)
	{
		int cluster = iter->first;
		std::vector<int> pIndexesVector = iter->second;
		std::vector<double> gravityPoint;
		for (int i = 0; i < dataset[0].size(); i++)
		{
			gravityPoint.push_back(0);
		}
		for (int pIndex : pIndexesVector)
		{
			for (int i = 0; i < dataset[pIndex].size(); i++)
			{
				gravityPoint[i] += dataset[pIndex][i];
			}
		}
		size_t sizeOfCluster = m_clusterDictionaryIndexes.find(cluster)->second.size();
		for (int i = 0; i < gravityPoint.size(); i++)
		{
			gravityPoint[i] /= sizeOfCluster;
		}

		auto it = m_clusterGravityPointDictionary.find(cluster);
		if (it == m_clusterGravityPointDictionary.end())
		{
			std::vector<double> values;
			values = gravityPoint;
			m_clusterGravityPointDictionary.insert({ cluster , values });
		}
	}
}

void Silhouette::findClusterPairs()
{
	//create array for pairs
	int amountClusters = static_cast<int>(m_clusterGravityPointDictionary.size());

	m_clusterPairs = (int*)malloc(sizeof(int) * amountClusters);
	if (!m_clusterPairs)
		exit(1);
	std::thread threadArray[4];

	//run threads to find those pairs
	int size = static_cast<int> (amountClusters / 4.0);
	for (int i = 0; i < 4; i++)
	{
		if (i == 3)
		{
			threadArray[i] = std::thread(workFindPair, i * size, amountClusters, m_clusterPairs, m_clusterGravityPointDictionary);
		}
		else
			threadArray[i] = std::thread(workFindPair, i * size, (i + 1) * size, m_clusterPairs, m_clusterGravityPointDictionary);

	}

	//wait for all threads to finish
	for (int i = 0; i < 4; i++)
	{
		threadArray[i].join();
	}

	int size2 = static_cast<int> (amountClusters / 4.0);




}

void Silhouette::calculateAvgSilhoueteOfCluster()
{
}

void Silhouette::calculateClusterAValue()
{
}

void Silhouette::calculateBValues()
{
}

double Silhouette::calcDistance(std::vector<double> p, std::vector<double> q)
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

void Silhouette::workFindPair(const int startIndex, const int endIndex, int* clusterPairs, std::map<int, std::vector<double>> clusterGravityPointDictionary)
{
	for (int i = startIndex; i < endIndex; i++)
	{
		double minDist = 999999;
		int minimumIndex = 0;
		auto v1 = clusterGravityPointDictionary.find(i)->second;
		//calcualte distance to all neighbors on same grid group
		for (int j = 0; j < endIndex; j++)
		{
			auto v2 = clusterGravityPointDictionary.find(j)->second;
			//they are in range, connect them by their real indexes
			double dist = calcDistance(v1, v2);
			if (dist == 0)
				continue;
			if (dist < minDist)
			{
				minimumIndex = j;
				minDist = dist;
			}
		}
		clusterPairs[i] = minimumIndex;
	}
}
