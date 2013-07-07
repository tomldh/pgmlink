// stl
#include <set> // std::set
#include <algorithm> // std::copy_if
#include <functional> //std::not_equal_to
#include <iterator> // std::insert_iterator

// boost
#include <boost/range/adaptors.hpp> // boost::adaptors::filter
#include <boost/range/algorithm.hpp> // boost::copy

// lemon
#include <lemon/list_graph.h> // lemon::ListGraph
#include <lemon/maps.h> // lemon::IterableValueMap

//pgmlink
#include <pgmlink/region_graph.h>


namespace ba = boost::adaptors;


namespace pgmlink {
  ////
  //// RegionGraph
  ////
  RegionGraph::RegionGraph() :
    maximum_label_(0u) {
    add(node_neighbors())
      .add(arc_dissimilarity())
      .add(node_label())
      .add(node_contains())
      .add(node_conflicts())
      .add(node_level());
  }


  void RegionGraph::union_neighbors(const Region& r1,
                                    const Region& r2,
                                    const Region& new_region) {
    NeighborMap& neighbor_map = get(node_neighbors());
    ConflictMap& conflict_map = get(node_conflicts());
    const std::set<Region>& neighbors1 = neighbor_map[r1];
    const std::set<Region>& neighbors2 = neighbor_map[r2];

    std::set<Region>& neighbors_new = neighbor_map.get_value(new_region);
    const std::set<Region>& conflicts_new = conflict_map[new_region];
    IsInRegionSet is_contained_functor(conflicts_new.begin(),
                                       conflicts_new.end());
    std::unary_negate<IsInRegionSet > region_chooser =
      std::unary_negate<IsInRegionSet >(is_contained_functor);

    conditional_copy(neighbors1.begin(),
                     neighbors1.end(),
                     std::inserter(neighbors_new, neighbors_new.begin()),
                     region_chooser);
    conditional_copy(neighbors2.begin(),
                     neighbors2.end(),
                     std::inserter(neighbors_new, neighbors_new.begin()),
                     region_chooser);
    for (std::set<Region>::iterator it = neighbors_new.begin();
         it != neighbors_new.end();
         ++it) {
      neighbor_map.get_value(*it).insert(new_region);
    }
  }


  void RegionGraph::union_conflicts(const Region& r1,
                                    const Region& r2,
                                    const Region& new_region) {
    ConflictMap& conflict_map = get(node_conflicts());

    const std::set<Region>& conflicts1 = conflict_map[r1];
    const std::set<Region>& conflicts2 = conflict_map[r2];

    std::set<Region>& conflicts_new = conflict_map.get_value(new_region);
    std::copy(conflicts1.begin(),
              conflicts1.end(),
              std::inserter(conflicts_new, conflicts_new.begin()));
    std::copy(conflicts2.begin(),
              conflicts2.end(),
              std::inserter(conflicts_new, conflicts_new.begin()));
    conflicts_new.insert(r1);
    conflicts_new.insert(r2);
    for (std::set<Region>::iterator it = conflicts_new.begin();
         it != conflicts_new.end();
         ++it) {
      conflict_map.get_value(*it).insert(new_region);
    }
  }

  
  int RegionGraph::merge_regions(Region r1, Region r2) {
    if (!valid(r1)) {
      return 1;
    }
    if (!valid(r2)) {
      return 2;
    }
    Region new_region = addNode();
    ++maximum_label_;

    
    // DissimilarityMap& dissimilarity_map = get(arc_dissimilarity());
    LabelMap& labels_map = get(node_label());
    LevelMap& level_map = get(node_level());
    ContainingMap& contains_map = get(node_contains());

    union_conflicts(r1, r2, new_region);
    union_neighbors(r1, r2, new_region);
    labels_map.set(new_region, maximum_label_);
    level_map.set(new_region,
                  std::min(level_map[r1], level_map[r2]));
    std::set<Region>& contains_new = contains_map.get_value(new_region);
    contains_new.insert(r1);
    contains_new.insert(r2);
    

    
    return 0;
  }

  int RegionGraph::merge_regions(int, int) {
    
    return 0;
  }

}
