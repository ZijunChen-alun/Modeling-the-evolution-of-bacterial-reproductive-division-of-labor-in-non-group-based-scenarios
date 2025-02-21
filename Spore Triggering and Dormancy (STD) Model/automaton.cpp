#include "automaton.hpp"


Automaton::Automaton()
    : state(0), da(0.5), db(0.5), death(0.1), ka(0.5), kb(0.5), Move_chance(0.5) {
}

Automaton::Automaton(const Automaton& other)
    : state(other.state), da(other.da), db(other.db), ka(other.ka), 
      kb(other.kb) {
}

//assignment operation
Automaton& Automaton::operator=(const Automaton& other) {
    if (this != &other) { 
        state = other.state;
        da = other.da;
        ka = other.ka;
        db = other.db;
        kb = other.kb;
    }
    return *this; 
}

//Calculation of average public goods
double Automaton::cal_average_k(CA2D<Automaton>* ca, unsigned row, unsigned col) {
    double total_k = 0.0;
    unsigned n_alive = 0;
    int nrow = ca->get_nrow();
    int ncol = ca->get_ncol();

    int initial_row = row - 2;
    int initial_col = col - 2;
    int end_row = row + 2;
    int end_col = col + 2;

    //test output
    //std::cout << "Center cell row: " << row << ", col: " << col << std::endl;

    for (int r = initial_row; r <= end_row; ++r) {
        for (int c = initial_col; c <= end_col; ++c) {
            // Periodic boundary conditions
           int wrapped_r, wrapped_c;
            
            if (r <= 0) {
                wrapped_r = nrow + r;
            } else if (r > nrow) {
                wrapped_r = r - nrow;
            } else {
                wrapped_r = r;
            }

            if (c <= 0) {
                wrapped_c = ncol + c;
            } else if (c > ncol) {
                wrapped_c = c - ncol;
            } else {
                wrapped_c = c;
            }

            // Skip Centre Cell
            if (wrapped_r == row && wrapped_c == col) {
                continue;
            }
            // Print each calculated cell position
            //std::cout << "Checking cell at wrapped_r: " << wrapped_r << ", wrapped_c: " << wrapped_c << std::endl;

            const auto& neighbor = ca->cell(wrapped_r, wrapped_c);
            if (neighbor.state == 1) {
                total_k += neighbor.ka;
                n_alive += 1;
                //std::cout << "State: 1, ka: " << self.ka << std::endl;
            } else if (neighbor.state == 2) {
                total_k += neighbor.kb;
                n_alive += 1;
                //std::cout << "State: 2, kb: " << self.kb << std::endl;
            }
        }
    }

    return n_alive > 0 ? total_k / n_alive : 0.0;
}

//exponential mutation
double Automaton::mutate_trait(double p) {
    static std::normal_distribution<double> dist(0.0, var); 

    double delta = dist(ran_gen::random); 
    double p_prime = p * std::exp(-delta); 

    if (p_prime > 1.0) {
        p_prime = 1.0;
    } 
    else if (p_prime < 0.0) {
        p_prime = 0.0;
    }

    return p_prime; 
}

void Automaton::set_mutation(double Newda, double Newka,double Newdb, double Newkb) {
    da = this->mutate_trait(Newda);
    ka = this->mutate_trait(Newka);
    db = this->mutate_trait(Newdb);
    kb = this->mutate_trait(Newkb);
}

void Automaton::set_keep(double Newda, double Newka,double Newdb, double Newkb) {
    da = Newda;
    ka = Newka;
    db = Newdb;
    kb = Newkb;
}

void Automaton::set_move(double Move) {
    Move_chance = Move;
}

void Automaton::set_death(double dea) {
    death = dea;
}

int Automaton::get_state() {
    return state;
}

void Automaton::set_state(unsigned newone) {
     state = newone;
}

void Automaton::set_ances(unsigned newone) {
     ances = newone;
}

int Automaton::get_ances () const{
    return ances;
}

double Automaton::get_death () const{
    return death;
}

double Automaton::get_move () const{
    return Move_chance;
}

double Automaton::get_da() const{
    return da;
}

double Automaton::get_ka() const{
    return ka;
}

double Automaton::get_db() const{
    return db;
}

double Automaton::get_kb() const{
    return kb;
}


// Implementing Friendly Functions
void swap(Automaton& first, Automaton& second) noexcept {
    std::swap(first.state, second.state);
    std::swap(first.da, second.da);
    std::swap(first.ka, second.ka);
    std::swap(first.db, second.db);
    std::swap(first.kb, second.kb);
}
