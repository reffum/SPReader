//
// Created by oleg on 10/21/26.
//

#ifndef SPREADER_CONSTANTS_H
#define SPREADER_CONSTANTS_H
#include <qnamespace.h>

constexpr std::string ORGANIZATION_NAME = "ReffumComp";
constexpr std::string APPLICATION_NAME = "SPReader";

//
// Columns in contents and bookmarks tree view
//
constexpr int COLUMN_COUNT = 2;

constexpr int COLUMN_TITLE = 0;
constexpr int COLUMN_PAGE = 1;

// Data roles in bookmarks and contents tree models
constexpr int PAGE_NUMBER_DATA_ROLE = Qt::UserRole + 1;

#endif //SPREADER_CONSTANTS_H
