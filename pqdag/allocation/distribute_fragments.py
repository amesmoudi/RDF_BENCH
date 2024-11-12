#!/usr/bin/env python3

import os
import shutil
import argparse
import yaml
import tarfile
from concurrent.futures import ThreadPoolExecutor
def read_affectation_file(affectation_file_path):
    fragment_to_worker = {}
    with open(affectation_file_path, 'r') as f:
        for line in f:
            fragment_id_str, worker_id_str = line.strip().split()
            fragment_id = int(fragment_id_str)
            worker_id = int(worker_id_str)
            fragment_to_worker.setdefault(worker_id, []).append(fragment_id)
    return fragment_to_worker

def read_workers_file(workers_file_path):
    worker_id_to_ip = {}
    with open(workers_file_path, 'r') as f:
        for idx, line in enumerate(f, start=1):
            worker_id_to_ip[idx] = line.strip()
    return worker_id_to_ip

def read_master_ip(master_ip_file):
    with open(master_ip_file, 'r') as f:
        master_ip = f.readline().strip()
    return master_ip

def create_worker_archive(worker_id, fragment_ids, common_files, fragment_files_dir, temp_dir):
    files_to_include = []

    # Paths to common files
    for common_file in common_files:
        src_file = os.path.join(fragment_files_dir, common_file)
        if os.path.exists(src_file):
            files_to_include.append((src_file, os.path.basename(common_file)))
        else:
            print(f"Warning: Common file {common_file} does not exist.")

    # Paths to fragment files
    for fragment_id in fragment_ids:
        for ext in ['.data', '.schema', '.dic']:
            filename = f"{fragment_id}{ext}"
            src_file = os.path.join(fragment_files_dir, filename)
            if os.path.exists(src_file):
                files_to_include.append((src_file, filename))
            else:
                print(f"Warning: Fragment file {filename} does not exist.")

    # Create the worker's affectation file
    affectation_filename = 'affectation'
    affectation_file_path = os.path.join(temp_dir, f'affectation_worker_{worker_id}')
    with open(affectation_file_path, 'w') as f:
        for fragment_id in fragment_ids:
            f.write(f"{fragment_id}\n")
    files_to_include.append((affectation_file_path, affectation_filename))

    # Create the tar file
    tar_filename = os.path.join(temp_dir, f'worker_{worker_id}.tar.gz')
    with tarfile.open(tar_filename, 'w:gz') as tar:
        for file_path, arcname in files_to_include:
            tar.add(file_path, arcname=arcname)
    print(f"Created tar file for worker {worker_id}: {tar_filename}")

def create_worker_archives(fragment_to_worker, common_files, fragment_files_dir, temp_dir):
    with ThreadPoolExecutor() as executor:
        futures = [
            executor.submit(create_worker_archive, worker_id, fragment_ids, common_files, fragment_files_dir, temp_dir)
            for worker_id, fragment_ids in fragment_to_worker.items()
        ]
        for future in futures:
            future.result()

def send_files_to_workers(worker_id_to_ip, temp_dir, destination_path, ssh_user):
    for worker_id, ip_address in worker_id_to_ip.items():
        tar_file = os.path.join(temp_dir, f'worker_{worker_id}.tar.gz')
        if os.path.exists(tar_file):
            # Ensure destination directory exists on worker
            print(f"Creating destination directory on worker {worker_id} at {ip_address} if it doesn't exist...")
            ssh_command = f"ssh {ssh_user}@{ip_address} 'mkdir -p {destination_path}'"
            os.system(ssh_command)

            # Use scp to copy the tar file to the worker
            print(f"Sending tar file to worker {worker_id} at {ip_address}...")
            scp_command = f"scp {tar_file} {ssh_user}@{ip_address}:{destination_path}"
            os.system(scp_command)

            # Extract the tar file on the worker
            print(f"Extracting tar file on worker {worker_id} at {ip_address}...")
            ssh_command = f"ssh {ssh_user}@{ip_address} 'cd {destination_path} && tar -xzf {os.path.basename(tar_file)} && rm {os.path.basename(tar_file)}'"
            os.system(ssh_command)
        else:
            print(f"No tar file found for worker {worker_id}.")

def send_files_to_master(master_ip, common_files, fragment_files_dir, destination_path, ssh_user, affectation_file):
    # Ensure destination directory exists on master
    print(f"Creating destination directory on master at {master_ip} if it doesn't exist...")
    ssh_command = f"ssh {ssh_user}@{master_ip} 'mkdir -p {destination_path}'"
    os.system(ssh_command)

    # Use scp to copy common files to master
    for common_file in common_files:
        src_file = os.path.join(fragment_files_dir, common_file)
        if os.path.exists(src_file):
            print(f"Sending {common_file} to master at {master_ip}...")
            scp_command = f"scp {src_file} {ssh_user}@{master_ip}:{destination_path}/"
            os.system(scp_command)
        else:
            print(f"Warning: Common file {common_file} does not exist.")

    # Copy the global affectation file to the master's destination directory
    dst_affectation_file = 'global_affectation.txt'  # The name of the file on the master
    if os.path.exists(affectation_file):
        print(f"Sending global affectation file to master at {master_ip}...")
        scp_command = f"scp {affectation_file} {ssh_user}@{master_ip}:{destination_path}/{dst_affectation_file}"
        os.system(scp_command)
    else:
        print(f"Warning: Global affectation file {affectation_file} does not exist.")

def ensure_storage_folder(worker_id_to_ip, storage_folder, ssh_user):
    for worker_id, ip_address in worker_id_to_ip.items():
        # Ensure storage folder exists on worker
        print(f"Ensuring storage folder exists on worker {worker_id} at {ip_address}...")
        ssh_command = f"ssh {ssh_user}@{ip_address} 'mkdir -p {storage_folder}'"
        os.system(ssh_command)

def run_loader_script(worker_id_to_ip, destination_path, storage_folder, ssh_user):
    for worker_id, ip_address in worker_id_to_ip.items():
        print(f"Running fragments_loader.py on worker {worker_id} at {ip_address}...")
        # Command to run the script on the worker
        # Adjust the path to 'fragments_loader.py' as needed
        command = f"python3 ~/pqdag/pyscripts/fragments_loader.py {destination_path} {storage_folder}"
        ssh_command = f"ssh {ssh_user}@{ip_address} 'bash -l -c \"{command}\"'"
        os.system(ssh_command)

def cleanup_destination(worker_id_to_ip, master_ip, destination_path, ssh_user):
    """Deletes the content of the destination path on all workers and master."""
    for worker_id, ip_address in worker_id_to_ip.items():
        print(f"Cleaning up destination path on worker {worker_id} at {ip_address}...")
        ssh_command = f"ssh {ssh_user}@{ip_address} 'rm -rf {destination_path}/*'"
        os.system(ssh_command)
    
    # Clean up on the master as well
    print(f"Cleaning up destination path on master at {master_ip}...")
    ssh_command = f"ssh {ssh_user}@{master_ip} 'rm -rf {destination_path}/*'"
    os.system(ssh_command)

def main():
    # Parse command-line arguments
    parser = argparse.ArgumentParser(description='Distribute fragment files to workers and master.')
    parser.add_argument('--config_file', type=str, required=True,
                        help='Path to the configuration YAML file.')
    args = parser.parse_args()

    # Load configuration from YAML file
    with open(args.config_file, 'r') as ymlfile:
        cfg = yaml.safe_load(ymlfile)

    # Assign configuration variables
    fragment_files_dir = cfg['fragment_files_dir']
    affectation_file = cfg['affectation_file']
    workers_file = cfg['workers_file']
    master_ip_file = cfg['master_ip_file']
    temp_dir = cfg['temp_dir']
    destination_path = cfg['destination_path']
    storage_folder = cfg['storage_folder']  # New storage folder path
    ssh_user = cfg.get('ssh_user', 'ubuntu')
    common_files = cfg['common_files']

    # Ensure the temporary directory exists
    os.makedirs(temp_dir, exist_ok=True)

    # Read the affectation and workers files
    fragment_to_worker = read_affectation_file(affectation_file)
    worker_id_to_ip = read_workers_file(workers_file)
    master_ip = read_master_ip(master_ip_file)

    # Create tar archives for each worker
    create_worker_archives(fragment_to_worker, common_files, fragment_files_dir, temp_dir)

    # Send tar files to each worker and extract them
    send_files_to_workers(worker_id_to_ip, temp_dir, destination_path, ssh_user)

    # Send files to master
    print("Sending files to master...")
    send_files_to_master(master_ip, common_files, fragment_files_dir, storage_folder, ssh_user, affectation_file)

    # Ensure storage folder exists on each worker
    ensure_storage_folder(worker_id_to_ip, storage_folder, ssh_user)

    # Run the loader script on each worker
    run_loader_script(worker_id_to_ip, destination_path, storage_folder, ssh_user)

    # Cleanup temporary directory locally
    shutil.rmtree(temp_dir)  # This line deletes the temp_dir after sending

    # Cleanup remote destination paths on workers and master
    cleanup_destination(worker_id_to_ip, master_ip, destination_path, ssh_user)

    print("Distribution and cleanup complete.")

if __name__ == '__main__':
    main()

