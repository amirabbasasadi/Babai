#include <iostream>
// include main Babai header
#include "Babai/babai.hpp"
int main() {
  // create an optimization problem
  auto p = new babai::problem();

  // set number of objective function variables
  p->dimension(2);

  // set lower bound and upper bounds for variables
  // if the bounds are not same for all variables you can pass a vector
  // for example : p->lower_bound(v) where v is an Eigen::RowVectorXd
  p->lower_bound(-5.12)->upper_bound(5.12);

  // define the objective using either minimize or maximize method
  // the objective function could be a lambda function
  // type of the input is a reference to Eigen::RowVectorXd
  // you can use all Eigen vector operations or simply access the elements and
  // define your own operation
  p->minimize([](auto v) {
    return v[0] * v[0] + v[1] * v[1] - 10 * cos(2 * 3.14 * v[0]) -
           10 * cos(2 * 3.14 * v[1]) + 20;
  });

  // create an instance of Adaptive Particle Swarm optimizer
  auto pso = new babai::PSO();

  // set number of particles and problem
  pso->npop(100)->problem(p);

  // trace and control iterations
  // this function runs in every iteration
  // type of the input is same as the pso variable (i.e a pointer to the
  // optimizer)
  std::cout << "iteration,loss,particle,velocity,x,y" << std::endl;
  pso->iterate([](auto trace) {
    // using the trace, you can access all parameters of the solver
    for (int i = 0; i < trace->npop(); i++) {
      std::cout << trace->step() << "," << trace->best() << "," << i << ",";
      std::cout << trace->velocity().row(i).norm() << ",";
      std::cout << trace->particles()(i, 0) << ",";
      std::cout << trace->particles()(i, 1);
      std::cout << std::endl;
    }

    // continue until convergence
    if (trace->step() > 70)
      trace->stop(); // stop iterations
  });
  return 0;
}
