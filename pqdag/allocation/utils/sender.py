import os
import argparse
import paramiko
from scp import SCPClient

def create_scp_client(ip, username):
    """Creates an SCP client for transferring files using key-based authentication."""
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    ssh.connect(ip, username=username)
    return SCPClient(ssh.get_transport())

def send_file_or_directory(scp, source, remote_path):
    """Send a file or directory to the remote path."""
    if os.path.isfile(source):
        scp.put(source, remote_path=remote_path)
        print(f"File {source} sent successfully.")
    elif os.path.isdir(source):
        scp.put(source, remote_path=remote_path, recursive=True)
        print(f"Directory {source} sent successfully.")
    else:
        print(f"Error: {source} is neither a file nor a directory.")

def main():
    parser = argparse.ArgumentParser(description="Send a file or directory to multiple IP addresses.")
    parser.add_argument("source", help="The file or directory to send.")
    parser.add_argument("ip_file", help="File containing IP addresses (one per line).")
    parser.add_argument("remote_path", help="Path on the remote machines to send the file or directory.")
    parser.add_argument("--username", default="ubuntu", help="Username for SSH (default: ubuntu)")

    args = parser.parse_args()

    if not os.path.exists(args.source):
        print(f"Error: {args.source} does not exist.")
        return

    # Read IP addresses from file
    with open(args.ip_file, 'r') as f:
        ip_addresses = [line.strip() for line in f if line.strip()]

    # Send the file or directory to each IP address
    for ip in ip_addresses:
        try:
            scp = create_scp_client(ip, args.username)
            send_file_or_directory(scp, args.source, args.remote_path)
            scp.close()
        except Exception as e:
            print(f"Error sending to {ip}: {e}")

if __name__ == "__main__":
    main()

