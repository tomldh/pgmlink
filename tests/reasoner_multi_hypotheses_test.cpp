#define BOOST_TEST_MODULE reasoner_multi_hypotheses_test

// stl
#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>


// boost
#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>

// pgmlink
#include "pgmlink/graph.h"
#include "pgmlink/multi_hypotheses_graph.h"
#include "pgmlink/feature.h"
#include "pgmlink/reasoner_multi_hypotheses.h"
#include "pgmlink/traxels.h"

using namespace pgmlink;
typedef MultiHypothesesGraph::Node Node;
typedef MultiHypothesesGraph::Arc Arc;



BOOST_AUTO_TEST_CASE( MultiHypothesesGraph_build_hyp ) {
  std::cout << "Constructing MultiHypothesesGraph" << std::endl;
  std::cout << std::endl;

  MultiHypothesesGraph g;
  MultiHypothesesGraph::ContainedRegionsMap& regions = g.get(node_regions_in_component());
  
  std::cout << "Adding nodes and arcs" << std::endl;
  std::cout << std::endl;

  Node n1 = g.add_node(0);
  Node n2 = g.add_node(1);
  Node n3 = g.add_node(2);

  g.addArc(n1, n2);
  g.addArc(n1, n3);

  std::cout << "Adding contained regions to nodes" << std::endl;
  std::cout << std::endl;
  std::vector<Traxel>& t1 = regions.get_value(n1);
  std::vector<Traxel>& t2 = regions.get_value(n2);
  std::vector<Traxel>& t3 = regions.get_value(n3);

  feature_array com(3,1.);

  
  t1.push_back(Traxel(1, 0));
  float conflict_arr11[] = {2., 3., 4., 5.};
  (t1.end()-1)->features["conflicts"] = feature_array(conflict_arr11, conflict_arr11 + 4);
  (t1.end()-1)->features["level"].push_back(0.);
  (t1.end()-1)->features["com"] = com;

  t1.push_back(Traxel(2, 0));
  float conflict_arr12[] = {1., 4., 5.};
  std::fill(com.begin(), com.end(), 2.);
  (t1.end()-1)->features["conflicts"] = feature_array(conflict_arr12, conflict_arr12 + 3);
  (t1.end()-1)->features["level"].push_back(1);
  (t1.end()-1)->features["com"] = com;
  
  t1.push_back(Traxel(3, 0));
  float conflict_arr13[] = {1.};
  std::fill(com.begin(), com.end(), 0.);
  (t1.end()-1)->features["conflicts"] = feature_array(conflict_arr13, conflict_arr13 + 1);
  (t1.end()-1)->features["level"].push_back(1);
  (t1.end()-1)->features["com"] = com;
  
  t1.push_back(Traxel(4, 0));
  float conflict_arr14[] = {1., 2.};
  std::fill(com.begin(), com.end(), 2.); com[0] = 1;
  (t1.end()-1)->features["conflicts"] = feature_array(conflict_arr14, conflict_arr14 + 2);
  (t1.end()-1)->features["level"].push_back(2);
  (t1.end()-1)->features["com"] = com;
  
  t1.push_back(Traxel(5, 0));
  float conflict_arr15[] = {1., 2.};
  com[0] = 3;
  (t1.end()-1)->features["conflicts"] = feature_array(conflict_arr15, conflict_arr15 + 2);
  (t1.end()-1)->features["level"].push_back(2);
  (t1.end()-1)->features["com"] = com;
  
  
  t2.push_back(Traxel(1, 1));
  float conflict_arr21[] = {3., 4.};
  std::fill(com.begin(), com.end(), 0.); com[0] = 1;
  (t2.end()-1)->features["conflicts"] = feature_array(conflict_arr21, conflict_arr21 + 2);
  (t2.end()-1)->features["level"].push_back(0);
  (t2.end()-1)->features["com"] = com;
  
  t2.push_back(Traxel(3, 1));
  float conflict_arr23[] = {1.};
  com[0] = 0;
  (t2.end()-1)->features["conflicts"] = feature_array(conflict_arr23, conflict_arr23 + 1);
  (t2.end()-1)->features["level"].push_back(1);
  (t2.end()-1)->features["com"] = com;
  
  t2.push_back(Traxel(4, 1));
  com[0] = 2;
  float conflict_arr24[] = {1.};
  (t2.end()-1)->features["conflicts"] = feature_array(conflict_arr24, conflict_arr24 + 1);
  (t2.end()-1)->features["level"].push_back(1);
  (t2.end()-1)->features["com"] = com;
  
  
  t3.push_back(Traxel(2, 1));
  std::fill(com.begin(), com.end(), 2.);
  float conflict_arr22[] = {5., 6.};
  (t3.end()-1)->features["conflicts"] = feature_array(conflict_arr22, conflict_arr22 + 2);
  (t3.end()-1)->features["level"].push_back(0);
  (t3.end()-1)->features["com"] = com;
                                            
  t3.push_back(Traxel(5, 1));
  com[0] = 1;
  float conflict_arr25[] = {2.};
  (t3.end()-1)->features["conflicts"] = feature_array(conflict_arr25, conflict_arr25 + 1);
  (t3.end()-1)->features["level"].push_back(1);
  (t3.end()-1)->features["com"] = com;
  
  t3.push_back(Traxel(6, 1));
  com[0] = 3;
  float conflict_arr26[] = {2.};
  (t3.end()-1)->features["conflicts"] = feature_array(conflict_arr26, conflict_arr26 + 1);
  (t3.end()-1)->features["level"].push_back(1);
  (t3.end()-1)->features["com"] = com;


  ConstantFeature det(10);
  ConstantFeature mis(10);
  ConstantFeature div(50000000);
  pgm::multihypotheses::TrainableModelBuilder builder( ConstantFeature(1000), // appearance
                                                       ConstantFeature(1000), // disappearance
                                                       SquaredDistance(), // move
                                                       1000000, // forbidden_cost
                                                       50, // max_division_level
                                                       3 // max_count
                                                       );
  builder
      .with_detection_vars(det, mis)
      .with_divisions(div);

  MultiHypotheses reasoner(&builder);

  std::cout << " -> workflow: formulating model" << std::endl;
  reasoner.formulate( g );

  ////
  //// Topology
  ////
  const pgm::OpengmModel* model = reasoner.get_graphical_model();
  std::cout << "Checking the topology of the graphical model...\n";
  std::cout << "Number of variables: " << model->numberOfVariables() << '\n';
  std::cout << "Number of factors:   " << model->numberOfFactors() << '\n';

  BOOST_CHECK_EQUAL( model->numberOfVariables(), 41);


  std::cout << " -> workflow: infer" << std::endl;
  reasoner.infer();

  std::cout << " -> workflow: conclude" << std::endl;
  reasoner.conclude( g );

  for (MultiHypothesesGraph::NodeIt n(g); n != lemon::INVALID; ++n) {
    std::vector<Traxel>& traxels = regions.get_value(n);
    std::cout << "Region " << traxels[0].Id << " at time " << traxels[0].Timestep << '\n';
    for (std::vector<Traxel>::iterator t = traxels.begin(); t != traxels.end(); ++t) {
      std::cout << *t << " is active? " << t->features["active"][0];
      if (t->features["active"][0] > 0.) {
        std::cout << "   descendants: ";
        std::ostream_iterator<feature_type> os_it(std::cout, ", ");
        std::copy(t->features["outgoing"].begin(),
                  t->features["outgoing"].end(),
                  os_it);
      }
      std::cout << '\n';
    }
    std::cout << '\n';
  }
  
  
}
