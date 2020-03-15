#include<iostream>
// include main Babai header
#include "Babai/babai.hpp"
int main(){
  // create an optimization problem
  auto p = new babai::problem();

  // set number of objective function variables
  p->dimension(100);

  // set lower bound and upper bounds for variables
  // if the bounds are not same for all variables you can pass a vector
  // for example : p->lower_bound(v) where v is an Eigen::RowVectorXd
  p->lower_bound(-10.0)->upper_bound(10.0);

  // define the objective using either minimize or maximize method
  // the objective function could be a lambda function
  // type of the input is a reference to Eigen::RowVectorXd
  // you can use all Eigen vector operations or simply access the elements and define your own operation
  p->minimize([](auto v) { return v.norm(); });

  // create an instance of Adaptive Particle Swarm optimizer
  auto pso = new babai::PSO();


  // set number of particles and problem
  pso->npop(20)->problem(p);


  // trace and control iterations
  // this function runs in every iteration
  // type of the input is same as the pso variable (i.e a pointer to the optimizer)
  pso->iterate([](auto trace) {
    // using the trace, you can access all parameters of the solver
    std::cout << "step :" << trace->step() << " | "
              << "loss :" << trace->best() << " | "
              << "objective evaluations :" << trace->nfe()
              << std::endl;

    // continue until convergence
    if (trace->best() < 0.01)
      trace->stop(); // stop iterations
  });
  // print the best found position
  std::cout << "best found position : " << std::endl;
  std::cout << pso->best_position() << std::endl;
  return 0;
}
