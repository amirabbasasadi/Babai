#ifndef BABAI_SOLVERS_PSO_GUARD
#define BABAI_SOLVERS_PSO_GUARD
#include "Babai/problems/problem.hpp"
#include "Babai/random_generator.hpp"
#include <cmath>
#include <iostream>
#include <limits>

namespace babai {
  class PSO {
public:
  PSO *problem(problem *p) {
    _problem = p;
    return this;
  }
  PSO *max_iter(unsigned n) {
    _max_iterations = n;
    return this;
  }
  PSO *npop(unsigned n) {
    _n_particles = n;
    return this;
  }
  PSO* stop(){
    _force_stop = true;
    return this;
  }
  PSO* iterate(std::function<void(PSO*)> trace){
    _force_stop = false;
    if(!_initialized){
      initialize();
      _initialized = true;
    }
    while(!stop_iteration()){
      _iterations++;
      trace(this);
      iter();
    }
    return this;
  }
  PSO *iter() {
      // particle swarm main loop
      update_best_global_solution();
      update_evolutionary_factor();
      // adapt inertia weight
      adapt_inertia_weight();
      update_conv_state();
      adapt_acceleration_coeffs();
      // elistic learning
      elistic_learning();
      // update particles velocity
      update_particles_velocity();
      // update particles positions
      update_particles_position();
      _unchanged_iterations++;
    return this;
  }
  double best() const{
    if(_problem->type() == MIN)
      return _best_global_obj;
    return -_best_global_obj;
  }
  unsigned step() const{
    return _iterations;
  }
  unsigned nfe() const{
    return _nfe;
  }
  unsigned npop() const{
    return _n_particles;
  }
  double inertial_weight() const{
    return _inertia_weight;
  }
  double social_influence() const{
    return _social_influence;
  }
  double self_cognition() const{
    return _self_cognition;
  }
  double evolutinary_factor() const{
    return _evolutionary_factor;
  }
  double elistic_learning_rate() const{
    return _elistic_learning_rate;
  }
  const Eigen::RowVectorXd& best_position() const{
    return _best_global_position;
  }
  const Eigen::MatrixXd& particles() const{
    return _position;
  }
  const Eigen::MatrixXd& velocity() const{
    return _velocity;
  }
protected:
  bool _force_stop;
  bool _initialized;
  babai::problem *_problem;
  unsigned _n_particles;
  unsigned _iterations;
  Eigen::MatrixXd _position;
  Eigen::MatrixXd _velocity;
  Eigen::MatrixXd _best_particle_position;
  Eigen::RowVectorXd _best_global_position;
  unsigned _worst_particle_index;
  double _worst_particle_obj;
  double _best_global_obj;
  Eigen::RowVectorXd _best_iteration_position;
  double _best_iteration_obj;
  Eigen::VectorXd _best_particle_obj;
  Eigen::MatrixXd _rp;
  Eigen::MatrixXd _rg;

  unsigned _nfe;
  unsigned _unchanged_iterations;
  unsigned _max_iterations;
  double _inertia_weight;
  double _self_cognition;
  double _social_influence;
  double _elistic_learning_rate;
  long double _evolutionary_factor;
  enum convergence_state { S1, S2, S3, S4 };
  convergence_state _conv_state;
  std::function<long double(vecr)> _obj;
  bool stop_iteration(){
    if(_force_stop)
      return true;
    return false;
  }
  void generate_objective(){
    if(_problem->type() == MIN){
      _obj = [this](vecr v){
        return this->_problem->obj(v);
      };
    }else{
      _obj = [this](vecr v){
        return -(this->_problem->obj(v));
      };
    }
  }
  void initialize() {
    _iterations = 0;
    // initialize random number generator
    Random::init();
    generate_objective();
    _nfe = 0;
    // set best obective to inf
    _best_global_obj = std::numeric_limits<double>::infinity();
    _worst_particle_obj = -std::numeric_limits<double>::infinity();
    // set matrix and vectors to zero
    _position = Eigen::MatrixXd::Zero(_n_particles, _problem->get_dimension());
    _best_particle_obj = Eigen::VectorXd::Zero(_n_particles);
    _best_iteration_obj = std::numeric_limits<double>::infinity();
    // _best_global_position = Eigen::VectorXd::Zero(_n_particles);
    _velocity = Eigen::MatrixXd::Zero(_n_particles, _problem->get_dimension());
    _rp = Eigen::MatrixXd::Zero(_n_particles, _problem->get_dimension());
    _rg = Eigen::MatrixXd::Zero(_n_particles, _problem->get_dimension());
    _unchanged_iterations = 0;
    // initialize positions using uniformly distributed random numbers
    initialize_parameters();
    for (unsigned i = 0; i < _position.rows(); i++)
      for (unsigned j = 0; j < _position.cols(); j++)
        _position(i, j) =
            gsl_ran_flat(Random::generator(), _problem->get_lower_bound()[j],
                         _problem->get_upper_bound()[j]);
    // set initial best particle positions
    _best_particle_position = _position;
    // set best particle objectives
    for (unsigned i = 0; i < _n_particles; i++)
      _best_particle_obj(i) = std::numeric_limits<double>::infinity();
    // initialize particles velocity
    Eigen::VectorXd velocity_bound =
        _problem->get_upper_bound() - _problem->get_lower_bound();
    for (unsigned i = 0; i < _velocity.rows(); i++)
      for (unsigned j = 0; j < _velocity.cols(); j++)
        _velocity(i, j) = gsl_ran_flat(Random::generator(), -velocity_bound[j],
                                       velocity_bound[j]);
  }
  long double objective(const Eigen::RowVectorXd& p){
    _nfe++;
    return _obj(p);
  }
  void initialize_parameters() {
    _inertia_weight = 0.9;
    _self_cognition = 1;
    _social_influence = 1;
    _elistic_learning_rate = 1;
  }
  void update_best_found_position(long double obj, const Eigen::RowVectorXd& p){
    _best_global_obj = obj;
    _best_global_position = p;
    _unchanged_iterations = 0;
  }
  void update_conv_state() {
    if (_evolutionary_factor <= 0.3)
      _conv_state = S3;
    return;
    if (_evolutionary_factor <= 0.6) {
      _conv_state = S2;
      return;
    }
    if (_evolutionary_factor <= 0.8) {
      _conv_state = S3;
      return;
    }
    _conv_state = S4;
  }
  void update_evolutionary_factor() {
    long double _global_distance;
    long double _min_distance = std::numeric_limits<long double>::infinity();
    long double _max_distance = -std::numeric_limits<long double>::infinity();
    // compute evolutinary factor for all particles
    long double temp_distance;
    for (unsigned i = 0; i < _n_particles; i++) {
      temp_distance =
          (_position.rowwise() - _position.row(i)).rowwise().lpNorm<2>().sum();
      // update minimum evolutionary factor
      if (temp_distance < _min_distance)
        _min_distance = temp_distance;
      // update maximum evolutionary factor
      if (temp_distance > _max_distance)
        _max_distance = temp_distance;
    }
    // update global evolutionary factor
    _global_distance = (_position.rowwise() - _best_global_position)
                           .rowwise()
                           .lpNorm<2>()
                           .sum();
    if (_global_distance < _min_distance)
      _min_distance = _global_distance;
    if (_global_distance > _max_distance)
      _max_distance = _global_distance;
    _evolutionary_factor =
        (_global_distance - _min_distance) / (_max_distance - _min_distance);
  }
  void adapt_inertia_weight() {
    _inertia_weight = 1.0 / (1 + 1.5 * exp(-2.6 * _evolutionary_factor));
  }
  void adapt_acceleration_coeffs() {
    long double delta_slight, delta;
    delta = gsl_ran_flat(Random::generator(), 0.05, 0.1);
    delta_slight = gsl_ran_flat(Random::generator(), 0.05, 0.5);
    if (_conv_state == S1) {
      _self_cognition += delta;
      _social_influence -= delta;
    }
    if (_conv_state == S2) {
      _social_influence -= delta_slight;
      _self_cognition += delta_slight;
    }
    if (_conv_state == S3) {
      _social_influence += delta_slight;
      _self_cognition += delta_slight;
    }
    if (_conv_state == S4) {
      _self_cognition -= delta;
      _social_influence += delta;
    }
    // acceleration coeffs bound
    long double temp_sum = _self_cognition + _social_influence;
    if (_self_cognition <= 0)
      _self_cognition = 0.1;
    if (_social_influence <= 0)
      _social_influence = 0.1;
    if (temp_sum > 4) {
      _social_influence = 4.0 * (_social_influence / temp_sum);
      _self_cognition = 4.0 * (_self_cognition / temp_sum);
    }
  }
  // perform elistic learning in convergence state
  void elistic_learning() {
    // only run during convergence
    if (_conv_state == S3) {
      for (unsigned i = 0; i < _unchanged_iterations + 1; i++) {
        Eigen::RowVectorXd temp_particle = _best_global_position;
        // choose a dimension randomly
        unsigned random_dim =
            gsl_rng_uniform_int(Random::generator(), _problem->get_dimension());

        // change the dimension
        double delta =
            gsl_ran_gaussian(Random::generator(), _elistic_learning_rate);
        temp_particle(random_dim) += (_problem->get_upper_bound()(random_dim) -
                                      _problem->get_lower_bound()(random_dim)) *
                                     delta;
        // std::cout << "Trace dim " << random_dim << " delat :" << delta  <<
        // std::endl; check bounds
        if (temp_particle(random_dim) < _problem->get_lower_bound()(random_dim))
          temp_particle(random_dim) =
              0.90 * _problem->get_lower_bound()(random_dim);
        if (temp_particle(random_dim) > _problem->get_upper_bound()(random_dim))
          temp_particle(random_dim) =
              0.90 * _problem->get_upper_bound()(random_dim);

        long double temp_obj = objective(temp_particle);
        if (temp_obj < _best_global_obj) {
          update_best_found_position(temp_obj, temp_particle);
          break;
        } else {
          // relapce worst particle by temp particle
          _position.row(_worst_particle_index) = temp_particle;
          _best_particle_position.row(_worst_particle_index) = temp_particle;
          _best_particle_obj(_worst_particle_index) = temp_obj;
        }
      }
    }
  }
  void update_best_global_solution() {
    _best_iteration_obj = std::numeric_limits<double>::infinity();
    _worst_particle_obj = -std::numeric_limits<double>::infinity();
    // evaluate solutions and update the best solution
    for (unsigned i = 0; i < _n_particles; i++) {
      double temp = objective(_position.row(i));
      if (temp > _worst_particle_obj) {
        _worst_particle_obj = temp;
        _worst_particle_index = i;
      }
      if (temp < _best_iteration_obj) {
        _best_iteration_position = _position.row(i);
        _best_iteration_obj = temp;
      }
      if (temp < _best_particle_obj[i]) {
        _best_particle_obj(i) = temp;
        _best_particle_position.row(i) = _position.row(i);
        if (temp < _best_global_obj) {
          // update best solution found
          update_best_found_position(temp, _position.row(i));
        }
      }
    }
  }

  void update_particles_velocity() {
    _rp = _rp.unaryExpr(
        [](double x) { return gsl_ran_flat(Random::generator(), 0.0, 1.0); });
    _rg = _rg.unaryExpr(
        [](double x) { return gsl_ran_flat(Random::generator(), 0.0, 1.0); });

    Eigen::MatrixXd _max_velocity =
        (-_position).rowwise() + _problem->get_upper_bound();
    Eigen::MatrixXd _min_velocity =
        (-_position).rowwise() + _problem->get_lower_bound();

    _velocity = _inertia_weight * _velocity +
                _self_cognition *
                    _rp.cwiseProduct(_best_particle_position - _position) +
                _social_influence * _rg.cwiseProduct((-_position).rowwise() +
                                                     _best_global_position);
    _velocity =
        0.95 * (_velocity.cwiseMax(_min_velocity).cwiseMin(_max_velocity));
  }
  void update_particles_position() { _position += _velocity; }
};
};     // namespace babai
#endif /* end of include guard: BABAI_SOLVERS_PSO_GUARD */
