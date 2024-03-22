import sys

def generate_worker_ids(num_workers, output_file):
    with open(output_file, 'w') as file:
        for i in range(1, num_workers + 1):
            worker_id = "worker{0:02d}".format(i)
            file.write(worker_id + '\n')

def main():
    if len(sys.argv) != 2:
        print("Usage: python genworkers.py <num_workers>")
        return

    try:
        num_workers = int(sys.argv[1])
    except ValueError:
        print("Invalid value for num_workers. Please provide a valid integer.")
        return

    output_file = "workers"  # You can change the output file name as needed

    generate_worker_ids(num_workers, output_file)
    print("{} worker IDs have been generated and saved to {}.".format(num_workers, output_file))

if __name__ == "__main__":
    main()
