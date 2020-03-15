#include <iostream>
#define CATCH_CONFIG_MAIN
#include "Babai/babai.hpp"
#include "Catch2/catch.hpp"
#include <gsl/gsl_sf_bessel.h>

TEST_CASE("Creating Optimizatin Problem", "[problem]") {

  // // set dimension
  // p1->dimension(15)->dimension(30);
  // REQUIRE(p1->get_dimension() == 30);
  // Eigen::VectorXd lb = Eigen::VectorXd::Constant(30, -500);
  // p1->lower_bound(lb);
  // REQUIRE(p1->get_lower_bound()[0] == -500);
  // Eigen::VectorXd up = -lb;
  // p1->upper_bound(up);
  // REQUIRE(p1->get_upper_bound()[0] == 500.0);
  // p1->minimize([](babai::vecr v){
  //   return v.lpNorm<2>();
  // });

  auto p = new babai::problem();
  p->dimension(100)->lower_bound(-10.0)->upper_bound(10.0);
  p->minimize([](auto v) { return v.squaredNorm(); });
  // use Adaptive Particle Swarm optimizer
  auto pso = new babai::PSO();
  pso->npop(20)->problem(p);
  // trace and control iterations
  pso->iterate([](auto trace) {
    // using the trace, you can access all parameters of the solver
    std::cout << "step :" << trace->step() << "loss :" << trace->best()
              << "objective evaluations :" << trace->nfe()
              << "best solution :" << trace->best_position() << std::endl;
    // continue until convergence
    if (trace->best() < 0.01)
      trace->stop();
  });
  delete p;
  delete pso;
}
