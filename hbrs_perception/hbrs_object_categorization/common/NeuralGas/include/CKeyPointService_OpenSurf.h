/*
 * Created on: 19.04.2011
 * Author: Christian Mueller
 */

#ifndef __CKEYPOINTSERVICEOPENSURF_H__
#define __CKEYPOINTSERVICEOPENSURF_H__
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string>
#include <cv.h>

//OpenSurf
#include "3rdParty/OpenSURF/src/surflib.h"
#include "3rdParty/OpenSURF/src/utils.h"
#include <CKeyPoint.h>

#include <ctime>

#define WINDOW_LIVE_IMAGE "Live Image Capture"
using namespace std;

class CKeyPointService_OpenSurf {

public:
	CKeyPointService_OpenSurf();
	void extractedKeyPoints(string fileName, char label,
			std::vector<CKeyPoint> &keyPoints);
	void readKeyPointsOpenSurf(IpVec ipts, int keyPointDim, std::vector<
			CKeyPoint> &keyPoints);
};

#endif
