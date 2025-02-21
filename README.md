# Modeling-the-evolution-of-bacterial-reproductive-division-of-labor-in-non-group-based-scenarios
This project simulates the evolutionary dynamics of bacterial populations using an agent-based computational model. The model is implemented in continuous time and designed to investigate the factors influencing bacterial survival, reproduction, and public goods production. It specifically explores whether the Low Relatedness Hypothesis can still drive the emergence of reproductive division of labor in groups without inherent and structured boundaries, such as bacterial populations. The model is adapted from prior studies (e.g., Devlin et al., 2023; Takeuchi et al., 2019)

Key Features
Agent-Based Model: The model uses a 2D cellular automaton with periodic boundary conditions to simulate bacterial populations in a boundary-free scenario. Each bacterium is treated as an independent and autonomous unit.

Bacterial Types and Traits: Two distinct bacterial phenotypes, type a and type b, are simulated. The model tracks key traits such as differentiation probabilities ($d_a$, $d_b$) and public goods production levels ($k_a$, $k_b$) for each bacterium.

Reproduction and Differentiation: The model allows bacteria to reproduce either by self-replication or differentiation into the other type, with probabilities depending on the trait values of the parent bacterium.

Death, Diffusion, and Reproduction Processes: Bacteria undergo random death, diffusion, and reproduction events based on predefined probabilities, with reproduction occurring only when there is an empty neighboring grid unit.

Mutation Mechanisms: Mutation introduces variation into bacterial traits over time, with two mutation models: Exponential Mutation and Linear Mutation, both affecting the bacterial traits.

Periodic Models: Several additional models simulate specific environmental pressures and dynamics, including:

Spore Triggering and Dormancy (STD) Model: Simulates transitions between active bacterial growth and spore dormancy.
Periodic Local Extinction Model: Models localized environmental disturbances leading to periodic extinctions and recolonization.
Note: All periodic models incorporate exponential mutation to introduce variation in bacterial traits over time.
