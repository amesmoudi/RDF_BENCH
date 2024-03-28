import sys

# Mappings of full URIs to their abbreviations
prefixes = {
    "http://purl.org/dc/terms/": "dc:",
    "http://xmlns.com/foaf/": "foaf:",
    "http://purl.org/goodrelations/": "gr:",
    "http://www.geonames.org/ontology#": "gn:",
    "http://purl.org/ontology/mo/": "mo:",
    "http://ogp.me/ns#": "og:",
    "http://purl.org/stuff/rev#": "rev:",
    "http://www.w3.org/1999/02/22-rdf-syntax-ns#": "rdf:",
    "http://www.w3.org/2000/01/rdf-schema#": "rdfs:",
    "http://schema.org/": "sorg:",
    "http://db.uwaterloo.ca/~galuc/wsdbm/": "wsdbm:"
}

# Function to replace prefixes in the second column
def replace_prefixes(line):
    attributes = line.split("\t")  # Assuming attributes are separated by tabs
    second_attribute = attributes[1]  # Get the second attribute
    for full, abbreviation in prefixes.items():
        if full in second_attribute:
            second_attribute = second_attribute.replace(full, abbreviation).replace("<", "").replace(">", "")
            break  # Exit the loop after the first successful replacement
    attributes[1] = second_attribute  # Update the second attribute
    return "\t".join(attributes)  # Join the modified attributes with tabs

# Verifying the correct number of arguments
if len(sys.argv) != 3:
    print("Usage: python script.py <source_file_path> <destination_file_path>")
    sys.exit(1)

# Assigning the file paths from the command line arguments
source_file_path = sys.argv[1]
destination_file_path = sys.argv[2]

# Read the source file, transform, and write the result to the destination file
with open(source_file_path, 'r', encoding='utf-8') as source_file, \
     open(destination_file_path, 'w', encoding='utf-8') as destination_file:
    for line in source_file:
        transformed_line = replace_prefixes(line.strip())  # .strip() to remove spaces and newline characters
        destination_file.write(transformed_line + "\n")  # Add a newline after each transformed line
