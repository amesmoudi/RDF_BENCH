import sys

def parse_line(line):
    """Parse a single line from the input file."""
    parts = line.strip().split(';')
    fragment_id = parts[0]
    neighbors = parts[3]
    return fragment_id, neighbors

def extract_triplets(fragment_id, neighbors):
    """Extract triplets from the neighbors part of the line."""
    triplets = []
    neighbors_list = neighbors.split(',')
    for neighbor in neighbors_list:
        neighbor_info = neighbor.strip().split()
        neighbor_id = neighbor_info[0]
        predicate = neighbor_info[1]
        weight = neighbor_info[2]
        # Ignore neighbors with id 0 or predicate -1
        if neighbor_id != '0' and predicate != '-1':
            triplets.append((fragment_id, predicate, neighbor_id, weight))
    return triplets

def main(input_file, output_file):
    # Load the predicate dictionary (if needed)
    with open(input_file, 'r') as infile, open(output_file, 'w') as outfile:
        for line in infile:
            fragment_id, neighbors = parse_line(line)
            triplets = extract_triplets(fragment_id, neighbors)
            for triplet in triplets:
                outfile.write(f"{triplet[0]} {triplet[1]} {triplet[2]} {triplet[3]}\n")

if __name__ == "__main__":
    # Ensure the correct number of arguments
    if len(sys.argv) != 3:
        print("Usage: python script.py <input_file> <output_file>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]
    main(input_file, output_file)

