Here’s the complete README based on your instructions:

---

# PQDAG Experiment Setup and Execution

This README provides a step-by-step guide for setting up and executing experiments with the PQDAG system across a distributed setup involving a master node, worker nodes, and a client machine.

## Prerequisites

- **Python 3.x**
- **SSH Access**: Ensure passwordless SSH access is set up between the master, workers, and client machine if necessary.
- **Configuration File**: You should have a `config.properties` file to specify necessary paths and settings.

---

## Setup Instructions

### 1. Deploy the `pqdag` Folder

Copy the `pqdag` folder to the following locations:
- The master node
- Each worker node

### 2. Configure `config.properties`

On each machine where `pqdag` is deployed (master and workers), open the `config.properties` file located in the `pqdag` folder. Update the file with the correct dataset directory path to point to the dataset location on each respective machine.

---

## Running the Experiment

1. **Run the Experiment from the Client Machine**:
   - Open `exp_runner.py` on the client machine.
   - Modify the path to the queries folder within `exp_runner.py` to match the correct location of the query files.
   
2. **Execute the Experiment**:
   - On the client machine, run the following command to execute the experiment:
     ```bash
     python3 exp_runner.py
     ```

3. **Retrieve Results**:
   - The experiment results will be saved to `results.txt` on the client machine.

---



