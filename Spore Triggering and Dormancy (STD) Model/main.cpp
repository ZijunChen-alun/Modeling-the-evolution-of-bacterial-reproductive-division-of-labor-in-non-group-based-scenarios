/* Library */
#include <csignal> // for signal handling
#include <sstream> // for std::stringstream
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <algorithm> // std::shuffle
#include <array>     // std::array
#include <cstdlib> // rand() and srand()
#include <ctime> // time()
#include <fstream>
/* My library */
#include "cellular-automata.hpp"
#include "cash-display.hpp"

/* Other headers */
#include "automaton.hpp"

// Function prototypes
void saveCellStates(const std::string& filename, CA2D<Automaton>& ca_curr, unsigned n_row, unsigned n_col);

// Global variables
CA2D<Automaton>* ca_curr = nullptr;
CashDisplay* display_p = nullptr;
unsigned n_row = 100;
unsigned n_col = 100;
double t = 1;


//Global random number generator
namespace ran_gen{
 /* Instantiate a random number generator */
std::mt19937_64 random;
std::uniform_real_distribution<double> uniform(0.0, 1.0);
}

// read history file
void loadCellStates(const std::string& filename, CA2D<Automaton>& ca_curr) {
    std::ifstream inFile(filename);
    if (!inFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        std::stringstream ss(line);
        unsigned row, col, state;
        double da, ka, db, kb; // Adjusted the order to match the saving order

        if (!(ss >> row >> col >> state >> da >> ka >> db >> kb)) {
            std::cerr << "Error reading line: " << line << std::endl;
            inFile.close();
            return;
        }

        try {
            auto& cell = ca_curr.cell(row, col);
            cell.set_state(state);
            cell.set_keep(da, ka, db, kb);  // Adjusted the order to match the saving order
        } catch (const std::exception& e) {
            std::cerr << "Error processing cell at (" << row << ", " << col << "): " << e.what() << std::endl;
            inFile.close();
            return; 
        }
    }

    inFile.close();
}


// record history
void saveCellStates(const std::string& filename, CA2D<Automaton>& ca_curr, unsigned n_row, unsigned n_col) {
    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    // Iterate over the grid row by row and column by column
    for (unsigned row = 1; row <= n_row; ++row) {
        for (unsigned col = 1; col <= n_col; ++col) {
            auto& cell = ca_curr.cell(row, col);
            // Write the cell's parameters to the file
            outFile << row << " " << col << " " << cell.get_state() << " " 
                    << cell.get_da() << " " << cell.get_ka() << " " 
                    << cell.get_db() << " " << cell.get_kb() << "\n";
        }
    }

    outFile.close();
}

int main(int argc, char** argv)
{

     if (argc < 6) {
        std::cerr << "Usage: " << argv[0] << " Move_chance Mutation Death RandomSeed [InputFile]" << std::endl;
        return 1;
    }
    double par1 = std::atof(argv[1]); // move
    double par2 = std::atof(argv[2]); // mutation
    double par3 = std::atof(argv[3]); // death
    unsigned random_seed = std::stoul(argv[4]); // random seed
    unsigned runtime = std::stoul(argv[5]);//maxtime
    std::cout << par1 << " " << par2 << " " << par3 << " " << random_seed << " " << runtime <<std::endl;

    // Set the random seed
    std::seed_seq seed{random_seed};
    ran_gen::random = std::mt19937_64(seed);

    /* Instantiate 100x100 cellular automata. In this demo, we demonstrate
       synchronously updated CA, so we need two CA objects. */
    ca_curr = new CA2D<Automaton>(n_row, n_col);

    /* Set parameters needed to display the CA. For this demo, we create
       only one panel */
    std::vector<CashPanelInfo> panel_info(1);

    /* Set a display panel size to 100x100 */
    panel_info[0].n_row = n_row;
    panel_info[0].n_col = n_col;

    /* Set the origin coordinate of the display panel */
    panel_info[0].o_row = 0;
    panel_info[0].o_col = 0;

    /* Instantiate the display object */
    try {
        /* Window size is set to 100x100, but can be bigger if more than one
           panel needs to be drawn. Only one window is allowed to open. */
        display_p = new CashDisplay(n_row, n_col, panel_info);
    }
    catch (std::bad_alloc) {
        std::cerr << "main(): Error, memory exhaustion" << std::endl;
        exit(-1);
    }

    /* Parameters */
    bool show_display = true;
    bool make_movie = true;

    /* If an X window needed, initialize things */
    // if (show_display) {
    //     display_p->open_window("Demo CA");
    // }

    /* If PNG slides are needed, initialize things */
    if (make_movie) {
        display_p->open_png("movie");
    }

    // Used to generate random numbers between 1 and nrow or ncol
    std::uniform_int_distribution<unsigned> dist_row(1, panel_info[0].n_row); 
    std::uniform_int_distribution<unsigned> dist_col(1, panel_info[0].n_col); 
    // Used to generate random numbers between 1 and 8
    std::uniform_int_distribution<unsigned> dist_8(1, 8);


    /* Initialize the CA. Note that [0][col], [101][col], [row][0],
       [row][101] are the boundaries, whose states are usually fixed. */
    // Load initial cell states if input file is provided
    if (argc > 6) {
        std::string input_file = argv[6];
        loadCellStates(input_file, *ca_curr);
    } else {
        // Initialize the CA if no input file is provided
        for (unsigned row = 1; row <= panel_info[0].n_row; ++row) {
            for (unsigned col = 1; col <= panel_info[0].n_col; ++col) {
                ca_curr->cell(row, col).set_state((ran_gen::uniform(ran_gen::random) < 0.5) ? 1 : 0);
                if (ca_curr->cell(row, col).get_state() == 1) {
                    // Two different types of bacteria are randomly generated
                    ca_curr->cell(row, col).set_state((ran_gen::uniform(ran_gen::random) < 0.5) ? 1 : 2);
                }
                ca_curr->cell(row, col).set_ances(ca_curr->cell(row, col).get_state());
            }
        }
    }

    //set move chance
    for (unsigned row = 1; row <= panel_info[0].n_row; ++row) {
            for (unsigned col = 1; col <= panel_info[0].n_col; ++col) {
              ca_curr->cell(row,col).set_move(par1);
              ca_curr->cell(row,col).set_death(par3);
        }
    }
    

    // Create a file output stream object
    std::ofstream cellOutFile("cell_states.csv"); 

    // The header of the output file
    cellOutFile << "TimeStep,State1AvgKa,State1AvgDa,State2AvgKb,State2AvgDb,countState1,countState2,totalCount\n";

    // Create a file output stream object
    std::ofstream ancestorOutFile("ancestor_states.csv"); 

    // The header of the output file
    ancestorOutFile <<"TimeStep,Num1,Num2\n";

    //killing time
    unsigned lastKillTime = 0;
    unsigned nextKillTime = 5000000 / t;

    /* Update the CA & display */
    unsigned max_time = runtime / t; 
    for (unsigned time = 0; time < max_time; ++time) {
        
        //check ancestor state
        if (time % 10000 == 0){
            int num1 = 0;
            int num2 = 0;
            for (unsigned row = 1; row <= panel_info[0].n_row; ++row) {
                for (unsigned col = 1; col <= panel_info[0].n_col; ++col) {
                    if (ca_curr->cell(row, col).get_state() != 0)
                    {
                        switch (ca_curr->cell(row, col).get_ances()) {
                            case 1:
                                num1 += 1;
                                break;
                            case 2:
                                num2 += 1;
                                break;
                        }    
                    }
                    
                }
            }
            ancestorOutFile << time*t << "," << num1 << "," << num2 <<"\n";
            ancestorOutFile.flush();
            if (num1 == 0 || num2 == 0)
                    {
                        for (unsigned row = 1; row <= panel_info[0].n_row; ++row) {
                            for (unsigned col = 1; col <= panel_info[0].n_col; ++col) {
                            if (ca_curr->cell(row, col).get_state() != 0){
                                ca_curr->cell(row, col).set_ances(ca_curr->cell(row, col).get_state());
                                }
                            }
                        }
                    }
        }

        //Randomly kill cells
        if (time >= lastKillTime + nextKillTime) {
            std::cout << "kill cells" <<std::endl;
            unsigned livingCells = 0;
            for (unsigned row = 1; row <= n_row; ++row) {
                for (unsigned col = 1; col <= n_col; ++col) {
                    if (ca_curr->cell(row, col).get_state() != 0) {
                        ++livingCells;
                    }
                }
            }
            unsigned cellsToKill = static_cast<unsigned>(livingCells * 0.90);
            unsigned killedCells = 0;
            while (killedCells < cellsToKill) {
                unsigned killrow = dist_row(ran_gen::random);
                unsigned killcol = dist_col(ran_gen::random);

                if (ca_curr->cell(killrow, killcol).get_state() != 0) {
                    ca_curr->cell(killrow, killcol).set_state(0);
                    ++killedCells;
                }
            }
            //nextKillTime = static_cast<unsigned>(runtime / (10 + time / (max_time / 5)));
            lastKillTime = time;
        }
        
         unsigned totalCountg;
        // Reset the accumulator variable at the beginning of each time step
        if (time % 10000 == 0){
        double sumKxState1 = 0, sumDxState1 = 0, sumKxState2 = 0, sumDxState2 = 0;
        unsigned countState1 = 0, countState2 = 0;

        // Traverse all Automatons to compute the accumulator
        for (unsigned row = 1; row <=  panel_info[0].n_row; ++row) {
            for (unsigned col = 1; col <=  panel_info[0].n_col; ++col) {
                auto& cell = ca_curr->cell(row, col);
                if (cell.get_state() == 1) {
                    sumKxState1 += cell.get_ka();
                    sumDxState1 += cell.get_da();
                    countState1++;
                } 
                else if (cell.get_state() == 2) {
                    sumKxState2 += cell.get_kb();
                    sumDxState2 += cell.get_db();
                    countState2++;
                }
            }
        }

        // Calculate and output the average value
        if (countState1 > 0) {
            sumKxState1 /= countState1;
            sumDxState1 /= countState1;
        }
        if (countState2 > 0) {
            sumKxState2 /= countState2;
            sumDxState2 /= countState2;
        }
        // Calculate the total count of states 1 and 2
        unsigned totalCount = countState1 + countState2;
        totalCountg = totalCount;
        // Write to outFile
        
        cellOutFile << time*t << "," << sumKxState1 << "," << sumDxState1 << "," 
                << sumKxState2 << "," << sumDxState2 << "," 
                << countState1 << "," << countState2 << "," << totalCount << "\n";
                cellOutFile.flush();
        }

        if (totalCountg == 0) {
                std::cerr << "Extinction occurred at time step: " << time << std::endl;
                cellOutFile.close();    
                delete ca_curr;
                delete display_p;
                return (0);
                }
        
        if (time % 10000 == 0) {
            std::stringstream ss;
            ss << "cell_state_history_" << ".txt";  
            saveCellStates(ss.str(), *ca_curr, panel_info[0].n_row, panel_info[0].n_col);
        }
        /* Update display */
        unsigned char color;
        for (unsigned row = 1; row <= panel_info[0].n_row; ++row) {
            for (unsigned col = 1; col <=  panel_info[0].n_col; ++col) {
                switch (ca_curr->cell(row, col).get_state()) {
                    case 0: // Dead state
                        color = CashColor::BLACK;
                        break;
                    case 1: // Bacteria A
                        if (ca_curr->cell(row, col).get_ka() < 0.2) {
                            color = CashColor::YELLOW; 
                            } 
                        else if (ca_curr->cell(row, col).get_ka()> 0.8) {
                            color = CashColor::WHITE;
                            }
                        else{
                           color = CashColor::RED; 
                        }
                         break;
                    case 2: // Bacteria B
                        if (ca_curr->cell(row, col).get_kb() < 0.2) {
                            color = CashColor::VIOLET;
                            } 
                        else if (ca_curr->cell(row, col).get_kb() > 0.8) {
                            color = CashColor::BLUE; 
                            }
                        else{
                           color = CashColor::GRAY; 
                        }
                         break;
                    }
             display_p->put_pixel(0, row, col, color);
             }
        }

        //record each time
        //saveCellStates("cell_state_history.txt", *ca_curr, panel_info[0].n_row, panel_info[0].n_col);

        /* If an X window needed, draw things */
        // if (show_display && time % 100000==0) {
        //     display_p->draw_window();
        // }

        /* If PNG slides are needed, draw things */
        if (make_movie && time % 500000==0) {
            display_p->draw_png();
        }

        //The current location is randomly selected, the number of rows multiplied by the number of columns
        for (int i = 0; i < panel_info[0].n_row*panel_info[0].n_col; ++i) { 
             
            //Pick a location at random
            unsigned row = dist_row(ran_gen::random); 
            unsigned col = dist_col(ran_gen::random);
            // probability 
            double p = ran_gen::uniform(ran_gen::random);
            //Automatons' parameter update
            switch (ca_curr->cell(row, col).get_state()) {
                case 1:{
                    if ((ca_curr->cell(row, col).get_death())*t > p)
                    {
                        ca_curr->cell(row, col).set_state(0);
                    }
                    // //Automatons move randomly 
                    else if ((ca_curr->cell(row, col).get_move()+ca_curr->cell(row, col).get_death())*t > p)
                        {
                            unsigned random_row = 0;
                            unsigned random_col = 0;
                            unsigned nei = dist_8(ran_gen::random);
                            ca_curr->xy_neigh_wrap(row,col,nei,random_row,random_col);
                            swap(ca_curr->cell(row,col),ca_curr->cell(random_row,random_col));
                        }      
                    break;
                }
                case 2:{
                    if ((ca_curr->cell(row, col).get_death())*t > p)
                    {
                        ca_curr->cell(row, col).set_state(0);
                    }
                    //Automatons move randomly 
                    else if ((ca_curr->cell(row, col).get_move()+ ca_curr->cell(row, col).get_death())*t > p )
                        {
                            unsigned random_row = 0;
                            unsigned random_col = 0;
                            unsigned nei = dist_8(ran_gen::random);
                            ca_curr->xy_neigh_wrap(row,col,nei,random_row,random_col);
                            swap(ca_curr->cell(row,col),ca_curr->cell(random_row,random_col));
                        }
                    break;
                }
                /*
                If there are viable cells in the neighborhood of the space cell, the average 
                concentration of common property perceived by the neighborhood is used to calculate 
                whether to produce offspring at that location.
                */
                case 0:{
                        double M = par2;//mutation rate
                        unsigned neirow = 0;
                        unsigned neicol = 0;
                        unsigned nei = dist_8(ran_gen::random);
                        ca_curr->xy_neigh_wrap(row,col,nei,neirow,neicol);
                        //Randomly selected neighbors of a living automaton produces offspring at this location
                        if (ca_curr->cell(neirow,neicol).get_state() != 0)
                        {
                            double average_k = ca_curr->cell(neirow,neicol).cal_average_k(ca_curr,neirow,neicol);
                            switch (ca_curr->cell(neirow,neicol).get_state())
                            {
                                case 1:{
                                    if ((average_k*(1-ca_curr->cell(neirow,neicol).get_ka())*ca_curr->cell(neirow,neicol).get_da())*t > p)
                                    {
                                        ca_curr->cell(row, col).set_state(2);                              
                                        if (M > ran_gen::uniform(ran_gen::random))
                                        {
                                            ca_curr->cell(row,col).set_mutation(ca_curr->cell(neirow,neicol).get_da(),ca_curr->cell(neirow,neicol).get_ka(),ca_curr->cell(neirow,neicol).get_db(),ca_curr->cell(neirow,neicol).get_kb());
                                            ca_curr->cell(row,col).set_ances(ca_curr->cell(neirow,neicol).get_ances());
                                        }
                                        else
                                        {
                                            ca_curr->cell(row,col).set_keep(ca_curr->cell(neirow,neicol).get_da(),ca_curr->cell(neirow,neicol).get_ka(),ca_curr->cell(neirow,neicol).get_db(),ca_curr->cell(neirow,neicol).get_kb());
                                            ca_curr->cell(row,col).set_ances(ca_curr->cell(neirow,neicol).get_ances());
                                        }
                                    }
                                    //if ((average_k*(1-ca_curr->cell(neirow,neicol).get_ka()))*d > p && p >= (average_k*(1-ca_curr->cell(neirow,neicol).get_ka())*ca_curr->cell(neirow,neicol).get_da())*d)
                                    else if ((average_k*(1-ca_curr->cell(neirow,neicol).get_ka()))*t > p )
                                    {
                                        ca_curr->cell(row, col).set_state(1);                                  
                                        if (M > ran_gen::uniform(ran_gen::random))
                                        {
                                            ca_curr->cell(row,col).set_mutation(ca_curr->cell(neirow,neicol).get_da(),ca_curr->cell(neirow,neicol).get_ka(),ca_curr->cell(neirow,neicol).get_db(),ca_curr->cell(neirow,neicol).get_kb());
                                            ca_curr->cell(row,col).set_ances(ca_curr->cell(neirow,neicol).get_ances());
                                        }
                                        else
                                        {
                                            ca_curr->cell(row,col).set_keep(ca_curr->cell(neirow,neicol).get_da(),ca_curr->cell(neirow,neicol).get_ka(),ca_curr->cell(neirow,neicol).get_db(),ca_curr->cell(neirow,neicol).get_kb());
                                            ca_curr->cell(row,col).set_ances(ca_curr->cell(neirow,neicol).get_ances());
                                        }
                                    }
                                    break;
                                }
                                case 2:{
                                    if ((average_k*(1-ca_curr->cell(neirow,neicol).get_kb())*ca_curr->cell(neirow,neicol).get_db())*t > p )
                                    {
                                        ca_curr->cell(row, col).set_state(1);                                    
                                        if (M > ran_gen::uniform(ran_gen::random))
                                        {
                                            ca_curr->cell(row,col).set_mutation(ca_curr->cell(neirow,neicol).get_da(),ca_curr->cell(neirow,neicol).get_ka(),ca_curr->cell(neirow,neicol).get_db(),ca_curr->cell(neirow,neicol).get_kb());
                                            ca_curr->cell(row,col).set_ances(ca_curr->cell(neirow,neicol).get_ances());
                                        }
                                        else
                                        {
                                            ca_curr->cell(row,col).set_keep(ca_curr->cell(neirow,neicol).get_da(),ca_curr->cell(neirow,neicol).get_ka(),ca_curr->cell(neirow,neicol).get_db(),ca_curr->cell(neirow,neicol).get_kb());
                                            ca_curr->cell(row,col).set_ances(ca_curr->cell(neirow,neicol).get_ances());
                                        }                        
                                    }
                                    //if ((average_k*(1-ca_curr->cell(neirow,neicol).get_kb()))*d > p && p >= (average_k*(1-ca_curr->cell(neirow,neicol).get_kb())*ca_curr->cell(neirow,neicol).get_db())*d)
                                    else if ((average_k*(1-ca_curr->cell(neirow,neicol).get_kb()))*t > p )
                                    {
                                        ca_curr->cell(row, col).set_state(2);
                                        if (M > ran_gen::uniform(ran_gen::random))
                                        {
                                            ca_curr->cell(row,col).set_mutation(ca_curr->cell(neirow,neicol).get_da(),ca_curr->cell(neirow,neicol).get_ka(),ca_curr->cell(neirow,neicol).get_db(),ca_curr->cell(neirow,neicol).get_kb());
                                            ca_curr->cell(row,col).set_ances(ca_curr->cell(neirow,neicol).get_ances());
                                        }
                                        else
                                        {
                                            ca_curr->cell(row,col).set_keep(ca_curr->cell(neirow,neicol).get_da(),ca_curr->cell(neirow,neicol).get_ka(),ca_curr->cell(neirow,neicol).get_db(),ca_curr->cell(neirow,neicol).get_kb());
                                            ca_curr->cell(row,col).set_ances(ca_curr->cell(neirow,neicol).get_ances());
                                        } 
                                    }
                                    break;
                                }
                                
                            }
                            
                            
                        }
                    break;
                }
            }
     
        }

    }
    // Save the current state of all cells
    saveCellStates("cell_state_history.txt", *ca_curr, panel_info[0].n_row, panel_info[0].n_col);    
    cellOutFile.close();
    ancestorOutFile.close();
    if (ca_curr) {
        delete ca_curr;
        ca_curr = nullptr;
    }
    if (display_p) {
        delete display_p;
        display_p = nullptr;
    }
    return (0);
}  