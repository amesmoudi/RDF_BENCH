#!/usr/bin/env python3

import argparse
import subprocess

def read_workers_file(workers_file_path):
    with open(workers_file_path, 'r') as f:
        worker_ips = [line.strip() for line in f if line.strip()]
    return worker_ips

def execute_command_on_workers(worker_ips, command):
    ssh_user = 'ubuntu'
    for ip_address in worker_ips:
        print(f"Executing command on worker {ip_address}...")
        ssh_command = f"ssh {ssh_user}@{ip_address} '{command}'"
        try:
            subprocess.run(ssh_command, shell=True, check=True)
            print(f"Command executed successfully on {ip_address}.")
        except subprocess.CalledProcessError as e:
            print(f"Failed to execute command on {ip_address}. Error: {e}")

def main():
    # Parse command-line arguments
    parser = argparse.ArgumentParser(description='Execute a command on multiple workers.')
    parser.add_argument('workers_file', type=str, help='Path to the workers IP addresses file.')
    parser.add_argument('command', type=str, help='Command to execute on the workers.')
    args = parser.parse_args()

    # Read the worker IP addresses
    worker_ips = read_workers_file(args.workers_file)

    # Execute the command on all workers
    execute_command_on_workers(worker_ips, args.command)

if __name__ == '__main__':
    main()

