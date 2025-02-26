/* Library */
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
        double da, ka, db, kb;  // Adjusted the order to match the saving order

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
        if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " Move_chance Mutation Death RandomSeed [InputFile]" << std::endl;
        return 1;
    }
    double par1 = std::atof(argv[1]); // move
    double par2 = std::atof(argv[2]); // mutation
    double par3 = std::atof(argv[3]); // death
    unsigned random_seed = std::stoul(argv[4]); // random seed
    std::cout << par1 << " " << par2 << " " << par3 << " " << random_seed << std::endl;

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
    if (make_movie ) {
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
    if (argc > 5) {
        std::string input_file = argv[5];
        loadCellStates(input_file, *ca_curr);
    } else {
        // Initialize the CA if no input file is provided
        for (unsigned row = 1; row <= panel_info[0].n_row; ++row) {
            for (unsigned col = 1; col <= panel_info[0].n_col; ++col) {
                ca_curr->cell(row, col).set_state((ran_gen::uniform(ran_gen::random) < 0.5) ? 3 : 0);
                if (ca_curr->cell(row, col).get_state() == 1) {
                    // Give the initial bacteria a unique label
                    ca_curr->cell(row, col).set_id();
                    // Two different types of bacteria are randomly generated
                    ca_curr->cell(row, col).set_state((ran_gen::uniform(ran_gen::random) < 0.5) ? 1 : 2);
                }
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
    std::ofstream outFile("cell_states.csv"); 

    // The header of the output file
    outFile << "TimeStep,State1AvgKa,State1AvgDa,State2AvgKb,State2AvgDb,State3AvgKa,State3AvgDa,State4AvgKb,State4AvgDb,countState1,countState2,countState3,countState4,totalCount1,totalCount2,K21,K11,K12,K22\n";

    //Create a file for contribution
    std::ofstream outcfile("contribution_states.csv");

    // The header of the output file
    outcfile << "TimeStep,Total1Avgcon,Total2Avgcon\n";

    /* Update the CA & display */
    /* State 1 or 2 represents bacteria type a or b in the DOL system, while state 3 or 4 represents
     bacteria type a or b in system p. However, only one of states 3 or 4 can exist in a single simulation at a time. */
    unsigned max_time =20000000;
    for (unsigned time = 0; time < max_time; ++time) {
       
        // Reset the accumulator variable at the beginning of each time step
        double sumKxState1 = 0, sumDxState1 = 0, sumKxState2 = 0, sumDxState2 = 0;
        double sumKxState3 = 0, sumDxState3 = 0, sumKxState4 = 0, sumDxState4 = 0;
        unsigned countState1 = 0, countState2 = 0;
        unsigned countState3 = 0, countState4 = 0;
        double K21 = 0, K11 = 0, K12 = 0, K22 = 0;

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
                else if (cell.get_state() == 3) {
                    sumKxState3 += cell.get_ka();
                    sumDxState3 += cell.get_da();
                    countState3++;
                }
                else if (cell.get_state() == 4) {
                    sumKxState4 += cell.get_kb();
                    sumDxState4 += cell.get_db();
                    countState4++;
                }
            }
        }

        for (unsigned row = 1; row <=  panel_info[0].n_row; ++row) {
            for (unsigned col = 1; col <=  panel_info[0].n_col; ++col) {
                auto& cell = ca_curr->cell(row, col);
                if (cell.get_state() == 1 || cell.get_state() == 2) {
                    K11 += cell.count_contribution_kfrom1(ca_curr, row, col);
                    K21 += cell.count_contribution_kfrom2(ca_curr, row, col);
                } 
                else if (cell.get_state() == 3 || cell.get_state() == 4) {
                    K12 += cell.count_contribution_kfrom1(ca_curr, row, col);
                    K22 += cell.count_contribution_kfrom2(ca_curr, row, col);  
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
        if (countState3 > 0) {
            sumKxState3 /= countState3;
            sumDxState3 /= countState3;
        }
        if (countState4 > 0) {
            sumKxState4 /= countState4;
            sumDxState4 /= countState4;
        }

        // Calculate the total count of states
        unsigned totalCount1 = countState1 + countState2;
        unsigned totalCount2 = countState3 + countState4;

        K11 /= (totalCount1*totalCount1);
        K21 /= (totalCount1*totalCount2);
        K12 /= (totalCount1*totalCount2);
        K22 /= (totalCount2*totalCount2);

        // Write to outFile
        if (time % 100 == 0) { outFile << time << "," << sumKxState1 << "," << sumDxState1 << "," 
                << sumKxState2 << "," << sumDxState2 << ","  << sumKxState3 << "," << sumDxState3 << ","
                << sumKxState4 << "," << sumDxState4 << ","
                << countState1 << "," << countState2 << "," << countState3 << "," << countState4 << "," 
                << totalCount1 << "," << totalCount2 << "," << K21 << "," << K11 << "," << K12 << "," 
                << K22 <<"\n";
                outFile.flush();
        }
        //If DOL or system p goes extinct in the simulation, the simulation will stop immediately
        if (totalCount1 == 0) {
                std::cerr << "Extinction totalCount1 occurred at time step: " << time << std::endl;
                outFile.close();
                outcfile.close();
                saveCellStates("cell_state_history.txt", *ca_curr, panel_info[0].n_row, panel_info[0].n_col);    
                delete ca_curr;
                delete display_p;
                return (0);
                }
        if (totalCount2 == 0) {
                std::cerr << "Extinction totalCount2 occurred at time step: " << time << std::endl;
                outFile.close();
                outcfile.close();
                saveCellStates("cell_state_history.txt", *ca_curr, panel_info[0].n_row, panel_info[0].n_col);    
                delete ca_curr;
                delete display_p;
                return (0);
                }

        /* Update display */
        unsigned char color;
        for (unsigned row = 1; row <= panel_info[0].n_row; ++row) {
            for (unsigned col = 1; col <= panel_info[0].n_col; ++col) {     
                    switch (ca_curr->cell(row, col).get_state()) {
                        case 0: 
                            color = CashColor::BLACK;
                            break;
                        case 1: 
                            color = CashColor::RED;
                            break;
                        case 2: 
                            color = CashColor::GREEN;
                            break;
                        case 3: 
                            color = CashColor::BLUE;
                            break;
                        case 4: 
                            color = CashColor::CYAN;
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
        if (make_movie && time%5000==0) {
            display_p->draw_png();
        }


        //Contribution from total count1 
        double Con_1 =0;

        //Contribution from total count2
        double Con_2 =0;

        //the number of replication
        unsigned number_re1 =0;
        unsigned number_re2 =0;


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
                    if (ca_curr->cell(row, col).get_death() > p)
                    {
                        ca_curr->cell(row, col).set_state(0);
                    }
                    // //Automatons move randomly 
                    else if (ca_curr->cell(row, col).get_move()+ca_curr->cell(row, col).get_death() > p)
                        {
                            unsigned random_row = 0;
                            unsigned random_col = 0;
                            unsigned nei = dist_8(ran_gen::random);
                            ca_curr->xy_neigh_wrap(row ,col ,nei,random_row,random_col);
                            swap(ca_curr->cell(row,col),ca_curr->cell(random_row,random_col));
                        }      
                break;
                }
                case 2:{
                    if (ca_curr->cell(row, col).get_death() > p)
                    {
                        ca_curr->cell(row, col).set_state(0);
                    }
                    //Automatons move randomly 
                    else if (ca_curr->cell(row, col).get_move()+ ca_curr->cell(row, col).get_death() > p )
                        {
                            unsigned random_row = 0;
                            unsigned random_col = 0;
                            unsigned nei = dist_8(ran_gen::random);
                            ca_curr->xy_neigh_wrap(row ,col ,nei,random_row,random_col);
                            swap(ca_curr->cell(row,col),ca_curr->cell(random_row,random_col));
                        }
                    break;
                }
                case 3:{
                    if (ca_curr->cell(row, col).get_death() > p)
                    {
                        ca_curr->cell(row, col).set_state(0);
                    }
                    //Automatons move randomly 
                    else if (ca_curr->cell(row, col).get_move()+ ca_curr->cell(row, col).get_death() > p )
                        {
                            unsigned random_row = 0;
                            unsigned random_col = 0;
                            unsigned nei = dist_8(ran_gen::random);
                            ca_curr->xy_neigh_wrap(row ,col ,nei,random_row,random_col);
                            swap(ca_curr->cell(row,col),ca_curr->cell(random_row,random_col));
                        }
                    break;
                }
                case 4:{
                    if (ca_curr->cell(row, col).get_death() > p)
                    {
                        ca_curr->cell(row, col).set_state(0);
                    }
                    //Automatons move randomly 
                    else if (ca_curr->cell(row, col).get_move()+ ca_curr->cell(row, col).get_death() > p )
                        {
                            unsigned random_row = 0;
                            unsigned random_col = 0;
                            unsigned nei = dist_8(ran_gen::random);
                            ca_curr->xy_neigh_wrap(row ,col ,nei,random_row,random_col);
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
                        //Reproduction is no longer just within the neighbourhood
                        unsigned neirow = dist_row(ran_gen::random);
                        unsigned neicol = dist_col(ran_gen::random);
                        //Randomly selected neighbors of a living automaton produces offspring at this location
                        if (ca_curr->cell(neirow,neicol).get_state() != 0)
                        {
                            double average_k = ca_curr->cell(neirow,neicol).cal_average_k(ca_curr,neirow,neicol);
                            switch (ca_curr->cell(neirow,neicol).get_state())
                            {
                            case 1 :{
                                if (average_k*(1-ca_curr->cell(neirow,neicol).get_ka())*ca_curr->cell(neirow,neicol).get_da() > p)
                                {
                                    number_re1++;
                                    Con_2 += ca_curr->cell(neirow,neicol).count_contribution_k2(ca_curr,neirow,neicol);
                                    ca_curr->cell(row, col).set_state(2);                              
                                    if (M > ran_gen::uniform(ran_gen::random))
                                    {
                                        ca_curr->cell(row,col).set_mutation(ca_curr->cell(neirow,neicol).get_da(),ca_curr->cell(neirow,neicol).get_ka(),ca_curr->cell(neirow,neicol).get_db(),ca_curr->cell(neirow,neicol).get_kb());
                                    }
                                    else
                                    {
                                        ca_curr->cell(row,col).set_keep(ca_curr->cell(neirow,neicol).get_da(),ca_curr->cell(neirow,neicol).get_ka(),ca_curr->cell(neirow,neicol).get_db(),ca_curr->cell(neirow,neicol).get_kb());
                                    }
                                }
                                if (average_k*(1-ca_curr->cell(neirow,neicol).get_ka()) > p && p >= average_k*(1-ca_curr->cell(neirow,neicol).get_ka())*ca_curr->cell(neirow,neicol).get_da())
                                {
                                   number_re1++;
                                   Con_2 += ca_curr->cell(neirow,neicol).count_contribution_k2(ca_curr,neirow,neicol);
                                   ca_curr->cell(row, col).set_state(1);                                  
                                   if (M > ran_gen::uniform(ran_gen::random))
                                    {
                                        ca_curr->cell(row,col).set_mutation(ca_curr->cell(neirow,neicol).get_da(),ca_curr->cell(neirow,neicol).get_ka(),ca_curr->cell(neirow,neicol).get_db(),ca_curr->cell(neirow,neicol).get_kb());
                                    }
                                    else
                                    {
                                        ca_curr->cell(row,col).set_keep(ca_curr->cell(neirow,neicol).get_da(),ca_curr->cell(neirow,neicol).get_ka(),ca_curr->cell(neirow,neicol).get_db(),ca_curr->cell(neirow,neicol).get_kb());
                                    }
                                }
                                
                                break;
                            }
                            case 2:{
                                 if (average_k*(1-ca_curr->cell(neirow,neicol).get_kb())*ca_curr->cell(neirow,neicol).get_db()  > p )
                                {
                                    number_re1++;
                                    Con_2 += ca_curr->cell(neirow,neicol).count_contribution_k2(ca_curr,neirow,neicol);
                                    ca_curr->cell(row, col).set_state(1);                                    
                                    if (M > ran_gen::uniform(ran_gen::random))
                                    {
                                        ca_curr->cell(row,col).set_mutation(ca_curr->cell(neirow,neicol).get_da(),ca_curr->cell(neirow,neicol).get_ka(),ca_curr->cell(neirow,neicol).get_db(),ca_curr->cell(neirow,neicol).get_kb());
                                    }
                                    else
                                    {
                                        ca_curr->cell(row,col).set_keep(ca_curr->cell(neirow,neicol).get_da(),ca_curr->cell(neirow,neicol).get_ka(),ca_curr->cell(neirow,neicol).get_db(),ca_curr->cell(neirow,neicol).get_kb());
                                    }                        
                                }
                                if (average_k*(1-ca_curr->cell(neirow,neicol).get_kb()) > p && p >= average_k*(1-ca_curr->cell(neirow,neicol).get_kb())*ca_curr->cell(neirow,neicol).get_db())
                                {
                                   number_re1++;
                                   Con_2 += ca_curr->cell(neirow,neicol).count_contribution_k2(ca_curr,neirow,neicol);
                                   ca_curr->cell(row, col).set_state(2);
                                    if (M > ran_gen::uniform(ran_gen::random))
                                    {
                                        ca_curr->cell(row,col).set_mutation(ca_curr->cell(neirow,neicol).get_da(),ca_curr->cell(neirow,neicol).get_ka(),ca_curr->cell(neirow,neicol).get_db(),ca_curr->cell(neirow,neicol).get_kb());
                                    }
                                    else
                                    {
                                        ca_curr->cell(row,col).set_keep(ca_curr->cell(neirow,neicol).get_da(),ca_curr->cell(neirow,neicol).get_ka(),ca_curr->cell(neirow,neicol).get_db(),ca_curr->cell(neirow,neicol).get_kb());
                                    } 
                                }

                                break;
                            }
                            case 3:{
                                if (average_k*(1-ca_curr->cell(neirow,neicol).get_ka()) > p )
                                {
                                    number_re2++;
                                    Con_1 += ca_curr->cell(neirow,neicol).count_contribution_k1(ca_curr,neirow,neicol);
                                    ca_curr->cell(row, col).set_state(3);
                                    if (M > ran_gen::uniform(ran_gen::random))
                                    {
                                        ca_curr->cell(row,col).set_mutation(ca_curr->cell(neirow,neicol).get_da(),ca_curr->cell(neirow,neicol).get_ka(),ca_curr->cell(neirow,neicol).get_db(),ca_curr->cell(neirow,neicol).get_kb());
                                        ca_curr->cell(row,col).set_d(0,0);
                                    }
                                    else
                                    {
                                        ca_curr->cell(row,col).set_keep(ca_curr->cell(neirow,neicol).get_da(),ca_curr->cell(neirow,neicol).get_ka(),ca_curr->cell(neirow,neicol).get_db(),ca_curr->cell(neirow,neicol).get_kb());
                                        ca_curr->cell(row,col).set_d(0,0);
                                    } 
                                }

                                break;
                            }
                            case 4:{
                                if (average_k*(1-ca_curr->cell(neirow,neicol).get_kb()) > p )
                                {
                                    number_re2++;
                                    Con_1 += ca_curr->cell(neirow,neicol).count_contribution_k1(ca_curr,neirow,neicol);
                                    ca_curr->cell(row, col).set_state(4);
                                    if (M > ran_gen::uniform(ran_gen::random))
                                    {
                                        ca_curr->cell(row,col).set_mutation(ca_curr->cell(neirow,neicol).get_da(),ca_curr->cell(neirow,neicol).get_ka(),ca_curr->cell(neirow,neicol).get_db(),ca_curr->cell(neirow,neicol).get_kb());
                                        ca_curr->cell(row,col).set_d(0,0);
                                    }
                                    else
                                    {
                                        ca_curr->cell(row,col).set_keep(ca_curr->cell(neirow,neicol).get_da(),ca_curr->cell(neirow,neicol).get_ka(),ca_curr->cell(neirow,neicol).get_db(),ca_curr->cell(neirow,neicol).get_kb());
                                        ca_curr->cell(row,col).set_d(0,0);
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
        double Total1Avgcon = 0.0, Total2Avgcon = 0.0;

        // if (totalCount1 > 0) {
        //     Total1Avgcon = Con_1 / totalCount1;
        // } else {
        //     Total1Avgcon = 0.0; 
        // }

        // if (totalCount2 > 0) {
        //     Total2Avgcon = Con_2 / totalCount2;
        // } else {
        //     Total2Avgcon = 0.0; 
        // }
        if (number_re2 > 0)
        {
           Total1Avgcon = Con_1 / number_re2;
        }
        else {
             Total1Avgcon = 0.0; 
        }
        if (number_re1 > 0)
        {
           Total2Avgcon = Con_2 / number_re1;
        }
        else {
             Total2Avgcon = 0.0; 
        }
          
        // outcfile << time << "," << Total1Avgcon << "," << Total2Avgcon << "\n";
        outcfile << time << "," << Total1Avgcon << "," << Total2Avgcon << "\n";
        outcfile.flush();


    }
    // Save the current state of all cells
    saveCellStates("cell_state_history.txt", *ca_curr, panel_info[0].n_row, panel_info[0].n_col);    
    outFile.close();
    outcfile.close();
    delete ca_curr;
    delete display_p;
    return (0);
}  