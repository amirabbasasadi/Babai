# Babai
The library has not been released yet and is under development. contributions are welcomed.
## C++ Optimization Library (Version 0.1)
Babai(means sheep) is a C++ Optimization library based on Eigen and GSL.
## Features
- Solving Unconstrained Optimization Problems
- [to do] Multi-Objective Optimization Problems
- [to do] Constrained Optimization Problems
- [to do] Stochastic Optimization Problems
## How to use Babai
Babai requirements:
- C++ compiler that support C++17 (GCC, ...)
- CMake
- GNU Scientific Library  

to use Babai, you only need to include the `include/Babai/babai.hpp`. It will include all problem types and optimizers. to build your program, don't forget to link the `gsl` and `gslcblas`.also you can use `-O3 -march=native` flags to increase the performance. this is an example using CMake:
```cmake
cmake_minimum_required(VERSION 3.0)
project(BabaiTest)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -march=native")
include_directories(include)
add_executable(app source.cpp)
target_link_libraries(app gsl gslcblas)
```

### Gradient-Free Solvers
#### Adaptive Particle Swarm Optimization
The APSO solver was implemented based on this paper:
- Zhan, Z. H., Zhang, J., Li, Y., & Chung, H. S. H. (2009). Adaptive particle swarm optimization. IEEE Transactions on Systems, Man, and Cybernetics, Part B (Cybernetics), 39(6), 1362-1381.  

Parameters Adaption and Elitist Learning are implemented.
##### Examples
minimizing the function:  

 <div style="text-align: center"><img src="https://render.githubusercontent.com/render/math?math=f(x_1,x_2,...,x_D) = \sqrt{\sum^D_{i=1}{x_i^2}}"></div>   

```cpp
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

```
##### Parameters
The parameters and methods which are accessible inside the trace function are as follow:
- `stop()` stops optimizer iterations   
- `step()` returns number of performed iterations
- `best()` returns best objective function value
- `nfe()`  returns number of the objective function evaluations
- `npop()` returns number of particles  
- `best_position()` returns best found position as a vector
- `best_local_positions()` returns best local position for all particles as a matrix
- `positions()` returns the positions of all particles as a matrix
- `velocity()` returns the velocity of all particles as a matrix  
to see explanation for the parameters listed below, refer to the APSO paper
- `inertia_weight()`, returns the inertia weight
- `self_cognition()` returns the Self Cognition
- `‍social_influence()` returns the Social Influence
- `evolutionary_factor()`, returns the Evolutionary Factor
- `elitist_learning_rate()` returns the Elitist Learning Rate
### Gradient-Based Solvers
[to do]
## Developers
- Amirabbas Asadi, (amir137825@gmail.com)
## References
- Zhan, Z. H., Zhang, J., Li, Y., & Chung, H. S. H. (2009). Adaptive particle swarm optimization. IEEE Transactions on Systems, Man, and Cybernetics, Part B (Cybernetics), 39(6), 1362-1381.  
