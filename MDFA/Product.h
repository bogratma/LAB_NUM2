//
// Created by vlad on 4/4/26.
//

#ifndef LAB_NUM2_PRODUCT_H
#define LAB_NUM2_PRODUCT_H
#include <map>
#include <set>

struct Product {
  std::set<char> alphabet;
  std::map<int, std::map<char,int>> transitions;
  std::map<std::pair<int,int>,int> pairs;
};
#endif //LAB_NUM2_PRODUCT_H