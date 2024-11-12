#!/usr/bin/python3
# Usage ./propogate source_file nodes dest_file

import sys, os, time, shutil,subprocess
import csv, array, string

def main():
	#change working dir
	WORKING_DIRECTORY = os.getcwd()
	SCRIPT_PATH = os.path.realpath(__file__)
	SCRIPT_DIRECTORY = os.path.dirname(SCRIPT_PATH)
	SETUP_DIR = SCRIPT_DIRECTORY+"/.."
	os.chdir(SETUP_DIR)

	#setup a log file 
	log_file_path = "log.txt"
	mode = 'a' if os.path.exists(log_file_path) else 'w'
	log_file = open(log_file_path,mode)

	if len(sys.argv)!=3:
		print("usage: fragments_loader db_name archive_name is_compressed db_path")
		log_file.write("bad arguments, usage: fragments_loader db_name archive_name is_compressed db_path")
		log_file.close()
		exit(1)

	
	temp_db_path = sys.argv[1]
	db_path = sys.argv[2]
	#is_compressed	= sys.argv[3].upper() == "TRUE"
	#archive_path	= sys.argv[4] + "/" + db_name + "/" + archive_name
	
	#start time counter
	start_time = time.time()

	#extracting the files
	#print(sys.argv[4] + "/"+db_name+"/")
	#log_file.write(sys.argv[4] + "/"+db_name+"/")
	#Extract_status = extract_files(archive_path,is_compressed,sys.argv[4] + "/"+db_name+"/")
	#if extract_status != 0:
		#print("Failed to extract the files")
		#log_file.write("Failed to extract the files")
		#log_file.close()
		#exit(2)

	#remove old archive
	#os.system("rm %s"%(archive_path))

	#add file to indicate loading
	#os.system("touch /home/ubuntu/huge_datasets/dataBoumi/"+ "data/%s/loading_status.txt"%(db_name))

	#start loading into binary files
	fragments_loader(temp_db_path,db_path)
	
	#remove loading flag file
	#os.system("rm /home/ubuntu/huge_datasets/dataBoumi/data/%s/loading_status.txt"%(db_name))
	
	print("--- %s seconds ---" % (time.time() - start_time))
	#print("loading %s is done!!"%(archive_name))
	log_file.close()


def extract_files(archive_path,is_compressed,extration_dir):
	extract_status = 0
	if is_compressed:
		#print("tar -C %s -zxvf %s" %(extration_dir,archive_path)) # >/dev/null 2>&1
		extract_status = os.system("tar -C %s -zxvf %s" %(extration_dir,archive_path))
	else:
		extract_status = os.system("tar -C %s -xvf %s" %(extration_dir,archive_path))
	
	return extract_status

def fragments_loader(sgFolder,storageFolder):	
	# Files existence verification
	if os.path.exists(storageFolder):
		shutil.rmtree(storageFolder)
	
	
	os.system("mkdir -p %s >/dev/null 2>&1"%(storageFolder))

	# Creation of the affectation file
	onlyfiles = next(os.walk(sgFolder))[2]
	f= open("%s/affectation"%(sgFolder),"w+")
	for file in onlyfiles :
		if ".data" in file:
			f.write("%s\r\n" % (file.split('.')[0]))
	f.close()
	
	os.system("java -jar -Djava.library.path=solibs/ jars/segmentsLoader.jar %s %s " %(sgFolder,storageFolder))

	affectfile = sgFolder+"/affectation"

	# Loading dictionary on sqlite3
	#print("Start loading dictionary ...")
	with open(affectfile) as f:
		lines = f.readlines()
		for line in lines:
			line=line.strip()
			#print("loading dico %s ..."%(line.strip()))
			os.system("sqlite3 %s/dic_%s  -cmd \"CREATE TABLE dic_%s (id LONG PRIMARY KEY,value TEXT NOT NULL,sgin INTEGER,sgout INTEGER);\" -cmd \".separator \\t\" -cmd \".import %s.dic dic_%s\" \".exit\" " %(storageFolder,line, line, sgFolder+"/"+line,line))
			os.system("sqlite3 %s/dic_%s  -cmd \"create virtual table dic_%s_index using fts4(id,value,sgin,sgout);\" -cmd \".separator  \\t\" -cmd \".import %s.dic dic_%s_index\" \".exit\" " %(storageFolder,line, line, sgFolder+"/"+line,line))

	#print("loading dictionary Completed")

	os.system("cp %s/*.schema %s "%(sgFolder,storageFolder))
	os.system("cp %s/affectation %s "%(sgFolder,storageFolder))
	os.system("cp %s/predicates.txt %s "%(sgFolder,storageFolder))
	os.system("cp %s/spo_index.txt %s "%(sgFolder,storageFolder))
	os.system("cp %s/ops_index.txt %s "%(sgFolder,storageFolder))
	#print "loading database Completed"
	


"""
	#Initialisation
	WORKING_DIRECTORY = os.getcwd()
	SCRIPT_PATH = os.path.realpath(__file__)
	SCRIPT_DIRECTORY = os.path.dirname(SCRIPT_PATH)
	
	compress_files(SCRIPT_DIRECTORY)
	#sendFilesToWorkers()
	#extractFilesInWorkers
	#loadInBinaryFiles
	#deleteTempFiles

def compress_files(SCRIPT_DIRECTORY,compress = False):
	#initialisation
	machines_list = {}
	
	#read workers list
	
	#read allocation file
	try:
		allocation_file_path = SCRIPT_DIRECTORY + "/temp_files/allocation/final_allocation_metis.txt"
		with open(allocation_file_path) as allocation_file:
			tsvreader = csv.reader(allocation_file, delimiter="\t") 
			for line in tsvreader:
				segments_list = []
				if line[1] in machines_list:
					segments_list = machines_list[line[1]]
				
				segments_list.append(line[0])
				machines_list[line[1]] = segments_list; 
 				
	
	except FileNotFoundError:
		print("No such file: " + allocation_file_path)
		print("Allocation file not found, exiting the loader.\n")
		exit()

	#change working dirctory to results directory
	os.chdir(SCRIPT_DIRECTORY + "/temp_files/results")

	#generate and execute loading commands
	for machine_index in machines_list:
		compressCMD = ""
		removeCMD = "rm"
		if compress == True:
			compressCMD = "tar -cvzf "+ machine_index + ".tar"
		else:
			compressCMD = "tar -cvf "+ machine_index + ".tar"
		  
		for segment in machines_list[machine_index]:
			#add files to compress command
			compressCMD = compressCMD + " " + segment + ".data"
			compressCMD = compressCMD + " " + segment + ".dic"
			compressCMD = compressCMD + " " + segment + ".schema"

			#add files to remove command
			removeCMD = removeCMD + " " + segment + ".data"
			removeCMD = removeCMD + " " + segment + ".dic"
			removeCMD = removeCMD + " " + segment + ".schema"
		
		#archive the files and remove the old files
		compress_status = os.system(compressCMD + " >/dev/null 2>&1")#additional txt to hide output
		if compress_status != 0:
			print("Error while compressing the output, exiting the load scrip")
			exit(1)

		#remove_status = os.system(removeCMD + " >/dev/null 2>&1")
"""
if __name__ == "__main__":
	main()
