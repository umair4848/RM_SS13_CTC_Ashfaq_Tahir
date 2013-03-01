/*
 * CFileSettings.cpp
 *
 *  Created on: Apr 19, 2011
 *      Author: Christian Mueller
 */

#include<iostream>
#include<fstream>
#include<CFileSettings.h>
#include<vector>
#include<map>
#include<string>


void CFileSettings::coutStdVector(std::vector<double> data, bool newLines)
{
	if(newLines)
	{
		std::cout<<"\n";
	}

	for(unsigned int i=0; i < data.size(); ++i)
	{
		std::cout<<data[i]<<" ";
	}

	if(newLines)
	{
		std::cout<<"\n";
	}

}

void CFileSettings::saveLibSvmItem(bool isAppend, std::string fileName,
		std::vector<int> labels, std::vector<std::vector<double> > features) {
	std::fstream fout;

	if (!isAppend) // do not append
	{
		fout.open(std::string(fileName).c_str(), std::ios::out
				| std::ios::binary);
	} else //append
	{
		fout.open(std::string(fileName).c_str(), std::ios::out
				| std::ios::binary | std::ios::app);
	}
	//if (fout.is_open()) {
	if (labels.size() != features.size()) {
		std::cout << "Could not save since label != feaures size!!!";
		fout.close();
		return;
	}

	std::cout << "Saving... " << fileName << "\n";
	for (unsigned int i = 0; i < labels.size(); ++i) {
		fout << labels[i] << " ";
		for (unsigned int j = 0; j < features[i].size(); ++j) {
			fout << j << ":" << features[i][j] << " ";
		}
		fout << "\n";
	}
	fout.close();
	//} else {
	//	std::cout << "Could not save libsvm file!!!" << fileName << "\n";
	//}

}

void CFileSettings::saveStdVector(std::string fileName,
		std::vector<double> data) {

	std::fstream fout;

	std::cout << "Saving... " << fileName << "\n";
	fout.open(fileName.c_str(), std::ios::out | std::ios::binary);

	fout << 1 << "\n" << data.size() << "\n";

	for (unsigned int j = 0; j < data.size(); ++j) {
		fout << data[j] << " ";
	}
	fout << "\n";

	fout.close();
}

void CFileSettings::saveAppendStdVector(std::string fileName, std::vector<
		double> data, bool reCreateFile) {

	std::fstream fout;

	std::cout << "Saving... " << fileName << "\n";

	if (reCreateFile) {
		fout.open(fileName.c_str(), std::ios::out | std::ios::binary);
	} else {
		fout.open(fileName.c_str(), std::ios::out | std::ios::binary
				| std::ios::app);
	}

	fout << 1 << "\n" << data.size() << "\n";

	for (unsigned int j = 0; j < data.size(); ++j) {
		fout << data[j] << " ";
	}
	fout << "\n";

	fout.close();
}

std::vector<std::vector<double> > CFileSettings::loadStdVector(
		std::string fileName) {

	std::fstream fin;
	std::vector<std::vector<double> > readVector;
	std::cout << "Loading... " << fileName << "\n";

	fin.open(std::string(fileName).c_str(), std::ios::in | std::ios::binary);

	if (fin.is_open()) {

		unsigned int numVec;
		unsigned int dim;
		double temp;

		fin >> numVec;
		fin >> dim;
                
                std::cout<<"FIX THAT HERE!!!!";
		for (unsigned int i = 0; i < numVec; ++i) { //numVec
			std::vector<double> data;
			for (unsigned int j = 0; j < dim; ++j) {

				fin >> temp;
                              //if(j<30)
				data.push_back(temp);
			}
			readVector.push_back(data);
                        //if(i == 10)
                         //  break;
		}
		fin.close();
	} else {
		std::cout << "Could not read " << fileName << "\n";
	}

	return readVector;
}

void CFileSettings::tokenize(const std::string& str,
		std::vector<std::string>& tokens, const std::string& delimiters) {
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	std::string::size_type pos = str.find_first_of(delimiters, lastPos);
	while (std::string::npos != pos || std::string::npos != lastPos) {
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}
}

