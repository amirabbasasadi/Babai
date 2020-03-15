#ifndef BABAI_PROBLEM_GUARD
#define BABAI_PROBLEM_GUARD
#include "Babai/random_generator.hpp"
#include <Eigen/Dense>
#include <cmath>
// Babai problem defination class
namespace babai {
typedef Eigen::RowVectorXd vec;
typedef const Eigen::RowVectorXd &vecr;
enum ProblemType { MAX, MIN };
class problem {

public:
  problem *dimension(unsigned dimension) {
    this->_dimension = dimension;
    return this;
  }
  unsigned get_dimension(void) const { return _dimension; }
  problem *lower_bound(const Eigen::RowVectorXd &v) {
    this->_lower_bound = v;
    return this;
  }
  problem *lower_bound(double l) {
    this->_lower_bound = Eigen::RowVectorXd::Constant(_dimension, l);
    return this;
  }
  const Eigen::RowVectorXd &get_lower_bound(void) const { return _lower_bound; }
  problem *upper_bound(const Eigen::RowVectorXd &v) {
    this->_upper_bound = v;
    return this;
  }
  problem *upper_bound(double u) {
    this->_upper_bound = Eigen::RowVectorXd::Constant(_dimension, u);
    return this;
  }
  const Eigen::RowVectorXd &get_upper_bound(void) const { return _upper_bound; }
  problem *initial_value(const Eigen::VectorXd &v) {
    this->_initial_value = v;
    return this;
  }
  problem *maximize(std::function<long double(vecr)> f) {
    _obj = f;
    _type = MAX;
    return this;
  }
  problem *minimize(std::function<long double(vecr)> f) {
    _obj = f;
    _type = MIN;
    return this;
  }
  ProblemType type() { return _type; }
  long double obj(vecr v) { return _obj(v); }
  const Eigen::VectorXd &initial_value(void) const { return _initial_value; }

protected:
  // optimization problem dimension
  unsigned _dimension;
  // variables lower bound
  Eigen::RowVectorXd _lower_bound;
  // variables upper bound
  Eigen::RowVectorXd _upper_bound;
  // variables initial value
  Eigen::VectorXd _initial_value;
  // objective functions
  ProblemType _type;
  std::function<long double(vecr)> _obj;
};
};     // namespace babai
#endif /* end of include guard: BABAI_PROBLEM_GUARD */
