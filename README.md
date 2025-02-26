# **Only division of labor emerges by modeling the Evolution of Bacteria in Groups without Inherent and Structured Boundaries**

This project simulates the evolutionary dynamics of bacterial populations using an agent-based computational model. The model is implemented in continuous time and designed to investigate the factors influencing bacterial survival, reproduction, and public goods production. It specifically explores whether the **Low Relatedness Hypothesis** can still drive the emergence of reproductive division of labor in groups without inherent and structured boundaries, such as bacterial populations. The model is adapted from prior studies (e.g., **Devlin et al., 2023**; **Takeuchi et al., 2019**).

# CA Model with Exponential Mutation

The folder contains the code and necessary files for running the **Cellular Automaton (CA) model with exponential mutation**, which simulates bacterial evolution in non-group-based environments with exponential mutation. The following results, obtained by running the model with different initial variables, correspond to sections **4.1 - 4.3** of the thesis:

- **4.1**: Initial phenotype values are set to \( k_a = 0.5 \), \( k_b = 0.5 \), \( d_a = 0.5 \), and \( d_b = 0.5 \).
- **4.2**: Initial phenotype values represent a **reproductive division of labor** state.
- **4.3**: Initial phenotype values represent a more extreme **division of labor (DOL)** state, with distinct bacterial types.

# CA Model with Linear Mutation

The folder contains the code and necessary files for running the **Cellular Automaton (CA) model with linear mutation**, which simulates bacterial evolution in non-group-based environments with linear mutation. The following results, obtained by running the model with the initial variables below, correspond to section **4.4** of the thesis:

- **4.4**: Initial phenotype values are set to \( k_a = 0.5 \), \( k_b = 0.5 \), \( d_a = 0.5 \), and \( d_b = 0.5 \).

# Spore Triggering and Dormancy (STD) Model

The folder contains the code and necessary files for running the **Spore Triggering and Dormancy (STD) Model**, which builds upon bacterial evolution in non-group-based environments with exponential mutation by introducing spore triggering and dormancy cycles. The following results, obtained by running the model with the initial variables below, correspond to section **4.5** of the thesis:

- **4.5**: Initial phenotype values are set to \( k_a = 0.5 \), \( k_b = 0.5 \), \( d_a = 0.5 \), and \( d_b = 0.5 \). Additionally, every 5000 time steps, 90% of individuals are killed to simulate the cycle of spore triggering and dormancy.

# Periodic Local Extinction Model

The folder contains the code and necessary files for running the **Periodic Local Extinction Model**, which simulates bacterial evolution in non-group-based environments by introducing periodic local extinction events. The following results, obtained by running the model with the initial variables below, correspond to section **4.6** of the thesis:

- **4.6**: Initial phenotype values are set to \( k_a = 0.5 \), \( k_b = 0.5 \), \( d_a = 0.5 \), and \( d_b = 0.5 \). Every 5000 or 200 time steps, a local extinction event occurs where a central area of 50x50 units is killed, simulating local selective pressures like the application of localized antibiotics, causing bacterial death within that area.

# Normal Competition Test Model

The folder contains the code and necessary files for running the **Normal Competition Test Model**, which simulates competition between different bacterial systems under normal (non-well-mixed) conditions. In this condition, the model calculates the average public goods production and individuals reproducing, both of which depend on the local neighborhood. This approach involves taking half of the population from each of two different systems and merging them into a single .txt file for initialization. The model reads this file to initialize the simulation. To compare the evolutionary advantages of the DOL system and the non-division system (system p), the file should contain half of the population from the DOL system and half from system p, each evolved under the same parameters. If measuring the time for neutral evolution, the file should contain equal parts from two identical system p populations, each evolved under the same parameters. Additionally, the model allows modification of whether the two systems accept the public goods produced by the other system when calculating the average public goods production. For further details, please refer to section **6.3** of the thesis.


# Well-Mixed Competition Test Model

The folder contains the code and necessary files for running the **Well-Mixed Competition Test Model**, which simulates competition between different bacterial systems under well-mixed conditions. In well-mixed condition, when calculating the average public goods production and individuals reproducing, the model does not depend on local neighborhoods but instead randomly selects grids from the entire 2D plane. The approach involves taking half of the population from each of two different systems and merging them into a single .txt file for initialization. The model reads this file to initialize the simulation. To compare the evolutionary advantages of the DOL system and the non-division system (system p), the file should contain half of the population from the DOL system and half from system p, each evolved under the same parameters. If measuring the time for neutral evolution, the file should contain equal parts from two identical system p populations, each evolved under the same parameters. For further details, please refer to section **6.3** of the thesis.












