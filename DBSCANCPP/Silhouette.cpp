#include "Silhouette.h"




Silhouette::Silhouette()
{
}

double Silhouette::calculateSilhouetteValue(std::vector<std::vector<double>> dataset, std::vector<int> clusters)
{
	createClustersDictionaryIndexes(dataset, clusters);

	createClusterGravityPoint(dataset);

	findClusterPairs();
	

	//calculate any silhoueete value of each cluster using threads
	std::thread threadArray[4];

	int endClusterIndex = static_cast<int>(m_clusterPairsDictionary.size());
	int startClusterIndex = static_cast<int> (m_clusterPairsDictionary.size() / 4.0);
	for (int i = 0; i < 4; i++)
	{
		if (i == 3)
		{
			threadArray[i] = std::thread(calculateAvgSilhoueteOfCluster, i * startClusterIndex, endClusterIndex, &m_listAvgSilhouette, &m_clusterGravityPointDictionary, &m_clusterDictionaryIndexes, &m_clusterDictionaryVectors, &m_clusterPairsDictionary, &m_silhouetteMutex);
		}
		else
			threadArray[i] = std::thread(calculateAvgSilhoueteOfCluster, i * startClusterIndex, (i + 1) * startClusterIndex, &m_listAvgSilhouette, &m_clusterGravityPointDictionary, &m_clusterDictionaryIndexes, &m_clusterDictionaryVectors, &m_clusterPairsDictionary, &m_silhouetteMutex);

	}

	//wait for all threads to finish
	for (int i = 0; i < 4; i++)
	{
		threadArray[i].join();
	}
	//return average value of silhouette
	double sum = 0;
	for (double value : m_listAvgSilhouette)
	{
		sum += value;
	}
	if (m_listAvgSilhouette.size() == 0)
		return -1;
	return sum / m_listAvgSilhouette.size();
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

		//add vector to cluster map 
		auto it2 = m_clusterDictionaryVectors.find(cluster);
		if (it2 == m_clusterDictionaryVectors.end())
		{
			std::vector<std::vector<double>> vectors;
			m_clusterDictionaryVectors.insert({ cluster , vectors });
		}
		m_clusterDictionaryVectors.find(cluster)->second.push_back(dataset[pIndex]);
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


	std::thread threadArray[4];

	//run threads to find those pairs
	int size = static_cast<int> (amountClusters / 4.0);
	for (int i = 0; i < 4; i++)
	{
		if (i == 3)
		{
			threadArray[i] = std::thread(workFindPair, i * size, amountClusters, &m_clusterPairsDictionary, m_clusterGravityPointDictionary);
		}
		else
			threadArray[i] = std::thread(workFindPair, i * size, (i + 1) * size, &m_clusterPairsDictionary, m_clusterGravityPointDictionary);

	}

	//wait for all threads to finish
	for (int i = 0; i < 4; i++)
	{
		threadArray[i].join();
	}

	int size2 = static_cast<int> (amountClusters / 4.0);

}

void Silhouette::calculateAvgSilhoueteOfCluster(const int startClusterIndex, const int endClusterIndex, std::vector<double>* listAvgSilhouette, std::map<int, std::vector<double>>* clusterGravityPointDictionary, std::map<int, std::vector<int>>* clusterDictionaryIndexes, std::map<int, std::vector<std::vector<double>>>* clusterDictionaryVectors, std::map<int, int>* clusterPairsDictionary, std::mutex* silhouetteMutex)
{
	for (int clusterNumber = startClusterIndex; clusterNumber < endClusterIndex; clusterNumber++)
	{
		std::vector<double> AValueList = calculateClusterAValue(clusterNumber, clusterGravityPointDictionary, clusterDictionaryVectors);
		std::vector<double> BValueList = calculateBValues(clusterNumber, clusterPairsDictionary, clusterGravityPointDictionary, clusterDictionaryVectors);
		std::vector<double> SValueList;
		for (int i = 0; i < AValueList.size(); i++)
		{
			double a = AValueList[i];
			double b = BValueList[i];
			if (a < b)
			{
				SValueList.push_back((1 - (a / b)));
				continue;
			}
			if (a == b)
			{
				SValueList.push_back(0);
				continue;
			}
			SValueList.push_back((b / a) - 1);
		}
		double sum = 0;
		for (double val : SValueList)
		{
			sum += val;
		}
		double avgS = sum / SValueList.size();

		silhouetteMutex->lock();
		listAvgSilhouette->push_back(avgS);
		silhouetteMutex->unlock();

	}	
}

std::vector<double> Silhouette::calculateClusterAValue(const int clusterNumber, std::map<int, std::vector<double>>* clusterGravityPointDictionary, std::map<int, std::vector<std::vector<double>>>* clusterDictionaryVectors)
{
	std::vector<double> AValueList;
	//pull the current cluster vector
	auto iter = clusterGravityPointDictionary->find(clusterNumber);
	std::vector<double> CIVector = iter->second;
	//pull all current cluster vectors
	auto iter2 = clusterDictionaryVectors->find(clusterNumber);	
	std::vector<std::vector<double>> CIVectors = iter2->second;
	double temp = 0;
	for (int i = 0; i < CIVectors.size(); i++)
	{
		temp = calcADistance(CIVectors[i], CIVector, (int)CIVectors.size());
		AValueList.push_back(temp);
	}
	return AValueList;
}
 
std::vector<double> Silhouette::calculateBValues(const int clusterNumber, std::map<int, int>* clusterPairsDictionary, std::map<int, std::vector<double>>* clusterGravityPointDictionary, std::map<int, std::vector<std::vector<double>>>* clusterDictionaryVectors)
{
	std::vector<double> BValueList;
	//pull the index of closest cluset
	auto iter = clusterPairsDictionary->find(clusterNumber);
	int CJNumber = iter->second;
	//pull the closest cluster vector
	auto iter2 = clusterGravityPointDictionary->find(CJNumber);
	std::vector<double> CJVector = iter2 ->second;
	//pull all current cluster vectors
	auto iter3 = clusterDictionaryVectors->find(clusterNumber);
	std::vector<std::vector<double>> CIVectors = iter3->second;
	double temp = 0;
	//caclaulate b Value
	for (int i = 0; i < CIVectors.size(); i++)
	{ 
		temp = calcDistance(CIVectors[i], CJVector);
		BValueList.push_back(temp);
	}
		

	return BValueList;

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

double Silhouette::calcADistance(std::vector<double> a, std::vector<double> ac, int clusterSize)
{
	//equation:
	// sqrt(a(i)^2 + (ac - (a(i)/clusterSize)))
	//fix to:
	// sqrt( sumc((a.D - (ac.D * clustersize - a.D) / clustersize - 1 )^2)    )
	
	double sum = 0;
	double temp = 0;
	if (clusterSize == 1)
		return 0;
	for (int i = 0; i < a.size(); i++)
	{
		//calculate other avg gravity point and clac distance to it
		temp = a[i] - ((ac[i] * clusterSize - a[i]) / (clusterSize - 1));
		sum += temp * temp;
	}
	return sqrt(sum);
}



void Silhouette::workFindPair(const int startIndex, const int endIndex, std::map<int, int>*  clusterPairs, std::map<int, std::vector<double>> clusterGravityPointDictionary)
{
	for (int currentCluster = startIndex; currentCluster < endIndex; currentCluster++)
	{
		double minDist = 999999;
		int minimumIndex = 0;
		auto v1 = clusterGravityPointDictionary.find(currentCluster)->second;
		//calcualte distance to all neighbors on same grid group
		for (int otherCluster = 0; otherCluster < endIndex; otherCluster++)
		{
			auto v2 = clusterGravityPointDictionary.find(otherCluster)->second;
			//they are in range, connect them by their real indexes
			double dist = calcDistance(v1, v2);
			if (dist == 0)
				continue;
			if (dist < minDist)
			{
				minimumIndex = otherCluster;
				minDist = dist;
			}
		}
		auto iter = clusterPairs->find(currentCluster);
		if (iter == clusterPairs->end())
		{
			clusterPairs->insert({ currentCluster, minimumIndex });
		}
	}
}
