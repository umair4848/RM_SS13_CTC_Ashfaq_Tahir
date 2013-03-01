/*
 * FileSettings.h
 *
 *  Created on: Apr 19, 2011
 *      Author: chris
 */

#ifndef FILESETTINGS_H_
#define FILESETTINGS_H_
#include<vector>
#include<string>
#include<cstdlib>
/*
 * Created on: 19.04.2011
 * Author: Christian Mueller
 */

#include<map>
#include <utility>

#define NUM_CLASSES 8
#define LABEL_NEGATIVE 0
#define LABEL_CUP 1
#define LABEL_CAN 2
#define LABEL_BOX 3
#define LABEL_BOTTLE 4
#define LABEL_BOWL 5
#define LABEL_PLATE 6
#define LABEL_BALL 7

#define TRAIN_NEGATIVE "queries/train/"
#define TRAIN_CUP "cup/train/"
#define TRAIN_CAN "can/train/"
#define TRAIN_BOWL "bowl/train/"
#define TRAIN_BOTTLE "bottle/train/"
#define TRAIN_BOX "box/train/"
#define TRAIN_PLATE "plate/train/"
#define TRAIN_BALL "ball/train/"

#define NAME_NEGATIVE "query"
#define NAME_CUP "cup"
#define NAME_CAN "can"
#define NAME_BOWL "bowl"
#define NAME_BOTTLE "bottle"
#define NAME_BOX "box"
#define NAME_PLATE "plate"
#define NAME_BALL "ball"

class CFileSettings {
public:
	static void coutStdVector(std::vector<double> data, bool newLines = false);
	static void saveStdVector(std::string fileName, std::vector<double> data);
	static void saveAppendStdVector(std::string fileName,
			std::vector<double> data, bool reCreateFile = false);
	static std::vector<std::vector<double> >
	loadStdVector(std::string fileName);

	static void
			saveLibSvmItem(bool isAppend, std::string fileName,
					std::vector<int> labels,
					std::vector<std::vector<double> > features);

	static void tokenize(const std::string& str,
			std::vector<std::string>& tokens, const std::string& delimiters =
					" ");
};

#endif /* FILESETTINGS_H_ */

