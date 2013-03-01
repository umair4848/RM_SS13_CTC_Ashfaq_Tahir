 /*
 *  Created on: Mar 18, 2011
 *      Author: Christian Mueller
 * 
 */


#ifndef __CSVMONEVSONE_H__
#define __CSVMONEVSONE_H__

#include "COCSettings.h"
#include "COneVsOne.h"
#include "CSvm.h"
#include "CFScore.h"
#include "CEntropyScore.h"
#include "CMiScore.h"
#include "CAdaScore.h"

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string>
#include <time.h>

using namespace std;


class CSvmOneVsOne: public COneVsOne
{
protected:

	std::vector<CSvm> svms;
	std::vector< std::vector<vector<double> > > combinedTrainingExamples;
	std::vector< std::vector<vector<double> > > combinedTestingExamples;
	vector<CFeatureSelection*> featureSelector;
	int gridSearchIteration;
public:
	CSvmOneVsOne();
	CSvmOneVsOne(COCSettings &ocSettings, string id="",int gridSearchIteration=GRID_SEARCH_ITERATIONS);
	void combineTrainingSet();
	void combineTestingSet();
	double train();
	void loadModel(); //
	void loadOneVsOneCfg(); //first load cfg then load model!!!
	void saveOneVsOneCfg();
	void initEvaluation();

	char predict(std::vector<double> testExample, bool isLabeled, double* prob=NULL);
	double completeTest();
	double evaluate(std::vector< vector<double> > testingExamples);
};
#endif
