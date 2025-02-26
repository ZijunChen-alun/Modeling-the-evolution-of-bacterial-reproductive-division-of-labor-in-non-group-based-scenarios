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
    ./demo diffusion_rate mutation_rate death_rate random_seed input_file
    ```

    The parameters are explained as follows:
    - **diffusion_rate**: The diffusion rate (e.g., 0.1)
    - **mutation_rate**: The mutation rate (e.g., 0.1)
    - **death_rate**: The death rate (e.g., 0.1)
    - **random_seed**: A random seed for reproducibility (e.g., 1234)
    - **input_file**: The path to the file containing the population data from both systems, e.g., `data.txt`

    Example command to run the simulation when **diffusion rate**, **mutation rate**, and **death rate** are all set to 0.1, the random seed is set to 1234, and the file `data.txt` is processed:

    ```bash
    ./demo 0.1 0.1 0.1 1234 data.txt
    ```

    By default, the maximum time step is set to 20,000,000.


This will run the simulation with the provided parameters and input file.

