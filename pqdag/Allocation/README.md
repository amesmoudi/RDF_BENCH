Here's the complete, well-structured README file in a single block that you can copy:

---

# Fragment Distribution and Allocation System

This system is designed to organize, allocate, and distribute dataset fragments across multiple worker nodes for parallel processing. The following instructions provide a step-by-step guide on setting up and executing the system using the example dataset `watdiv100k`.

---

## Table of Contents

- [Prerequisites](#prerequisites)
- [Directory Structure](#directory-structure)
- [Setup Instructions](#setup-instructions)
  - [1. Prepare the Dataset](#1-prepare-the-dataset)
  - [2. Generate Dataset Statistics](#2-generate-dataset-statistics)
  - [3. Generate the Fragment Graph](#3-generate-the-fragment-graph)
  - [4. Allocate Fragments Using Weighted METIS](#4-allocate-fragments-using-weighted-metis)
  - [5. Distribute Fragments and Load into BTrees](#5-distribute-fragments-and-load-into-btrees)
- [Configuration](#configuration)
- [Example Commands](#example-commands)
- [Notes](#notes)
- [License](#license)

---

## Prerequisites

- **Python 3.x**
- **MPI for Python (`mpi4py`)**
  - Install via pip:
    ```bash
    pip install mpi4py
    ```
- **METIS Library**
  - Required for the weighted METIS allocation strategy.
  - Install METIS and its Python bindings according to your system's requirements.
- **Java Runtime Environment (JRE)**
  - Ensure Java is installed on all worker nodes if any scripts rely on Java (e.g., `fragments_loader.py` uses a JAR file).
  - Verify Java installation:
    ```bash
    java --version
    ```
- **Passwordless SSH Access**
  - Set up SSH keys to allow passwordless SSH access from the master node to all worker nodes.

---



## Setup Instructions

### 1. Prepare the Dataset

- Place all generated dataset files into the `datasetfolder` under a subfolder named after the dataset (e.g., `watdiv100k`).
- Files to include:
  - Fragment files: `*.data`, `*.schema`, `*.dic`
  - Index files: `spo_index`, `ops_index`, `predicates`
- Add @ips of cluster machines in master and workers files
### 2. Generate Dataset Statistics

Generate the `db.stat` file containing dataset statistics, which will be used for fragment allocation.

**Command:**

```bash
mpiexec -n 4 python3 stat_MPI.py datasetfolder/watdiv100k/ datasetfolder/watdiv100k/db.stat
```

- **Explanation:**
  - `mpiexec -n 4` runs the script using 4 MPI processes.
  - `stat_MPI.py` computes statistics based on the dataset files.
  - Input directory: `datasetfolder/watdiv100k/`
  - Output file: `datasetfolder/watdiv100k/db.stat`

### 3. Generate the Fragment Graph

Using the `db.stat` file, generate the summary graph `fragments_graph.quad` for fragment allocation.

**Command:**

```bash
python3 generate_fragments_graph.py datasetfolder/watdiv100k/db.stat datasetfolder/watdiv100k/fragments_graph.quad
```

- **Explanation:**
  - `generate_fragments_graph.py` processes `db.stat` to create a fragment graph.
  - Input file: `datasetfolder/watdiv100k/db.stat`
  - Output file: `datasetfolder/watdiv100k/fragments_graph.quad`

### 4. Allocate Fragments Using Weighted METIS

Allocate each fragment to a machine using the weighted METIS strategy.

**Command:**

```bash
python3 weighted_metis.py datasetfolder/watdiv100k/fragments_graph.quad datasetfolder/watdiv100k/allocation_result/affectation_weighted_metis.txt 10
```

- **Explanation:**
  - `weighted_metis.py` performs allocation based on the fragment graph.
  - Input file: `datasetfolder/watdiv100k/fragments_graph.quad`
  - Output file: `datasetfolder/watdiv100k/allocation_result/affectation_weighted_metis.txt`
  - `10` specifies the number of machines (e.g., allocating over 10 machines).
- **Output File Structure:**
  - The `affectation_weighted_metis.txt` file contains two columns:
    - **Fragment ID**
    - **Machine ID**

### 5. Distribute Fragments and Load into BTrees

Update the `config.yaml` file with information about the current dataset (e.g., `watdiv100k`), including paths and configurations.

**Command:**

```bash
python3 distribute_fragments.py --config_file config.yaml
```

- **Explanation:**
  - `distribute_fragments.py` handles the distribution of fragments and loading them into BTrees on each worker.
  - The script reads from `config.yaml` for configuration details.

---

## Configuration

Edit the `config.yaml` file to specify dataset paths, machine configurations, and parameters.

**Example `config.yaml`:**

```yaml
fragment_files_dir: '/path/to/datasetfolder/watdiv100k'
affectation_file: '/path/to/datasetfolder/watdiv100k/allocation_result/affectation_weighted_metis.txt'
workers_file: '/path/to/workers_ip.txt'
master_ip_file: '/path/to/master_ip.txt'
temp_dir: '/path/to/temp_dir'
destination_path: '/path/on/worker/machine/data'
storage_folder: '/path/on/worker/machine/storage'
ssh_user: 'ubuntu'
common_files:
  - 'spo_index'
  - 'ops_index'
  - 'predicates'
```

- **Parameters:**
  - `fragment_files_dir`: Path to the dataset's fragment files.
  - `affectation_file`: Path to the fragment allocation file.
  - `workers_file`: Path to the file containing worker node IP addresses.
  - `master_ip_file`: Path to the file containing the master node IP address.
  - `temp_dir`: Temporary directory used during distribution.
  - `destination_path`: Path on worker nodes where fragments will be stored.
  - `storage_folder`: Path on worker nodes where fragments will be loaded into BTrees.
  - `ssh_user`: SSH username for the master and worker nodes.
  - `common_files`: List of common files to distribute (e.g., indices).

---





---

**Note:** Follow each step carefully, updating paths and configurations as needed to match your environment. Ensure all prerequisites are met and configurations are properly set before executing the commands.