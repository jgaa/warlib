#pragma once

#include <boost/property_tree/ptree.hpp>

namespace war {

template <typename dataT, typename contT>
void CopyToContainer(const boost::property_tree::ptree& tree, contT& cont)
{
    for(auto it = tree.begin() ; it != tree.end(); ++it) {
        cont.push_back({it->first, it->second.get_value<dataT>()});
    }
}

template <typename dataT, typename contT>
void CopySingleValueToContainer(const boost::property_tree::ptree& tree, contT& cont)
{
    for(auto it = tree.begin() ; it != tree.end(); ++it) {
        cont.push_back(it->first);
    }
}

template <typename dataT, typename contT>
void CopyToMap(const boost::property_tree::ptree& tree, contT& cont)
{
    for(auto it = tree.begin() ; it != tree.end(); ++it) {
        cont.insert({it->first, it->second.get_value<dataT>()});
    }
}

} // war

