# Running the Model

To run this model, follow the steps below:

1. **Download All Files**: First, download all the necessary files from the repository to a single directory on your machine.

2. **Navigate to the Directory**: In a Linux system, open the terminal and navigate to the directory where you saved all the files.

    ```bash
    cd /path/to/your/folder
    ```

3. **Compile the Code**: Use the `make` command to compile the code. This will generate an executable file called **demo**.

    ```bash
    make
    ```

4. **Run the Model**: After compiling, you can run the model by executing the **demo** file with the following 5 parameters:

    ```bash
    ./demo diffusion_rate mutation_rate death_rate random_seed max_time_step
    ```

    The parameters are explained as follows:
    - **diffusion_rate**: The diffusion rate (e.g., 0.1)
    - **mutation_rate**: The mutation rate (e.g., 0.1)
    - **death_rate**: The death rate (e.g., 0.1)
    - **random_seed**: A random seed for reproducibility (e.g., 1234)
    - **max_time_step**: The maximum number of time steps for the simulation (e.g., 5000)

    Example command to run the simulation:

    ```bash
    ./demo 0.1 0.1 0.1 1234 5000
    ```

5. **Optional: Specify File Location**: If you need to read a specific input file during the simulation, you can add the file path as an additional parameter at the end of the command. For example:

    ```bash
    ./demo 0.1 0.1 0.1 1234 5000 /path/to/input_file.txt
    ```

6. **Change Initial Phenotype Values and Intervals**: If you need to change the initial phenotype values and intervals for killing bacteria, you must modify them in the source code before compiling the program. These values are defined within the code and are not configurable through command-line parameters.

This will run the simulation with the provided parameters and input file (if applicable).

