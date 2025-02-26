#include "cellular-automata.hpp"
#include <random>
#include <cmath> 
#include <string>
#include <iostream>
#ifndef AUTOMATON
#define AUTOMATON
namespace ran_gen{
extern std::seed_seq seed;
extern std::mt19937_64 random;
}

class Automaton {
private:
  int state;//State 0: dead cell. State 1: live cell type A. State 2: live cell type B.
  double da = 0.5;//Differentiation probability
  double db = 0.5;//Differentiation probability
  double death = 0.1; //Fixed mortality
  double ka = 0.5;// Quantity of public property production
  double kb = 0.5;// Quantity of public property production
  const double var = 0.02; //Variance of parameter variation
  double Move_chance = 0.5;//Random move probability
  unsigned long id; // ID of the Automaton
  unsigned long parentId; // ID of the parent 
  static unsigned int nextId;

public:
  double cal_average_k(CA2D<Automaton>* ca,unsigned row, unsigned col);//Calculate the current average concentration of public property around the cell
  double mutate_trait(double p);//Parameter mutation function
  void set_mutation(double Newda, double Newka,double Newdb, double Newkb);
  void set_keep(double Newda, double Newka,double Newdb, double Newkb);
  void set_move(double Move);
  void set_death(double dea);
  void set_d(double newda,double newdb);
  double get_da();
  double get_ka();
  double get_db();
  double get_kb();
  double get_death() const;
  double get_move() const;
  unsigned get_state();
  unsigned get_id();
  void set_state(unsigned newone);
  void set_id();//Set the ID initially
  void update_id(unsigned int parent);//Update the ID of the offspring when they are generated


  // Declare friend functions to implement position swapping
  friend void swap(Automaton& first, Automaton& second) noexcept;
  //Copy constructor
  Automaton(const Automaton& other);
  //Declares the assignment operation
  Automaton& operator=(const Automaton& other);
  Automaton(); // Default constructor
};

#endif
