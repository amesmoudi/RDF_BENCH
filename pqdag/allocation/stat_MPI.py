#!/usr/bin/python3
import pandas as pd 
import sys 
import time 
from pathlib import Path 
from mpi4py import MPI 
#import multiprocessing 
#multiprocessing.cpu_count() 
#mpiexec -n 8 python stat_MPI.py ../tmp/watdiv100k/results/ ../tmp/db.stat 
def main(): 
	 
	comm = MPI.COMM_WORLD 
	rank = comm.Get_rank() 
	cpus_number=comm.Get_size() 
	print("Start Working in rank : ",rank) 
	path = sys.argv[1] 
	spath = sys.argv[2] 
 
	if rank ==0: 
		fgs_list=get_fg_list(path,cpus_number) 
		then_g = time.time() 
	else: 
		fgs_list=None 
	fgs_list=comm.scatter(fgs_list, root=0) 
	 
	l=statcalculation(path,spath,fgs_list,rank) 
	l = comm.gather(l, root=0) 
	 
	 
	if rank==0: 
		now_g = time.time() 
		print("Gathering took: ", now_g-then_g, " seconds") 
		f=open(spath+".stat",'a+') 
		for line in l: 
			f.write(str(line)) 
 
	 
def get_fg_list(path,cpus_number): 
	spo_index = path+"spo_index.txt" 
	ops_index = path+"ops_index.txt" 
	count = 0 
	fgs_list = [] 
	# spo 
	f = open(spo_index, "r") 
	lignes = f.readlines() 
	for ligne in lignes: 
		count += 1 
	 
	for i in range(cpus_number): 
		fgs_list.append([]) 
	c=0 
	for ligne in lignes: 
		fgs_list[c%cpus_number].append(ligne.strip().split(";")[1]) 
		c+=1 
	f.close() 
	#ops 
	f = open(ops_index, "r") 
	lignes = f.readlines() 
	for ligne in lignes: 
		count += 1 
	c=0 
	for ligne in lignes: 
		fgs_list[c%cpus_number].append(ligne.strip().split(";")[1]) 
		c+=1 
	f.close() 
	return fgs_list 
 
def get_fg_list_Order(path,cpus_number): 
	spo_index = path+"spo_index.txt" 
	ops_index = path+"ops_index.txt" 
	count = 0 
	fgs_list = [] 
	# spo 
	f = open(spo_index, "r") 
	lignes = f.readlines() 
	for ligne in lignes: 
		count += 1 
	 
	for i in range(cpus_number): 
		fgs_list.append([]) 
	 
	c=0 
	for ligne in lignes: 
		fgs_list[c%cpus_number].append(ligne.strip().split(";")[1]) 
		c+=1 
	f.close() 
	#ops 
	f = open(ops_index, "r") 
	lignes = f.readlines() 
	for ligne in lignes: 
		count += 1 
	c=0 
	for ligne in lignes: 
		fgs_list[c%cpus_number].append(ligne.strip().split(";")[1]) 
		c+=1 
	f.close() 
	return fgs_list 
 
def statcalculation(path,spath,fgs_list,rankNbr): 
	index_colnames= ['0','1'] 
	spo_index = Path(path+"spo_index.txt") 
	spo_index_df = pd.read_csv(spo_index, delimiter=';', names=index_colnames, header=None) 
 
	ops_index = Path(path+"ops_index.txt") 
	ops_index_df = pd.read_csv(ops_index, delimiter=';', names=index_colnames, header=None) 
 
	then = time.time() 
	result="" 
	rank=rankNbr 
	for filenbr in fgs_list: 
		 
		filename = Path(path+filenbr+'.data') 
		if not filename.exists(): 
			break;		 
 
		colnames= ['0','1','2','3','4','5'] 
		df = pd.read_csv(filename, delimiter=' ', names=colnames, header=None) 
		 
#		print("File " + str(filenbr)) 
		#distinct 
		GF = str(df['0'].nunique()) 
		 
 
		#count_P_O 
		countP = "" 
		predicat = pd.DataFrame(df.groupby(['2']).size().reset_index(name='count')) 
 
		count_P_O_df = df[['2','3']].drop_duplicates(keep='first') 
		Objects = pd.DataFrame(count_P_O_df.groupby(['2']).size().reset_index(name='count')) 
		 
		for i in range(0, len(predicat)): 
			countP = countP + str(predicat.iloc[i]['2']) + " " + str(predicat.iloc[i]['count']) + " " + str(Objects.iloc[i]['count']) 
			if i != len(predicat) - 1: 
				countP = countP + "," 
		 
 
		#SF  1 
		seg1 = pd.DataFrame(df.groupby(['4','2']).size().reset_index(name='count')) 
 
		df3  = df[['2','3','4']].drop_duplicates(keep='first') 
		seg11 = pd.DataFrame(df3.groupby(['4','2']).size().reset_index(name='count')) 
		 
		SF = "" 
		for i in range(len(seg1)): 
			SF += str(seg1.iloc[i]['4']) + " " +str(seg1.iloc[i]['2']) + " " + str(seg1.iloc[i]['count']) + " " + str(seg11.iloc[i]['count']) 
			if i != len(seg1) - 1: 
				SF = SF + "," 
 
		seg2 = pd.DataFrame(df.groupby(['5','2']).size().reset_index(name='count')) 
 
		df4 = df[['2','3','5']].drop_duplicates(keep='first') 
		seg21 = pd.DataFrame(df4.groupby(['5','2']).size().reset_index(name='count')) 
 
		SFg = "" 
		for i in range(len(seg2)): 
			SFg += str(seg2.iloc[i]['5']) + " " +str(seg2.iloc[i]['2']) + " " + str(seg2.iloc[i]['count']) + " " + str(seg21.iloc[i]['count']) 
			if i != len(seg2) -1: 
				SFg = SFg + "," 
		 
		#SF  -1 
		Subject = pd.DataFrame(df.groupby(['1']).size().reset_index(name='count')) 
		df5 = df[['0','1']].drop_duplicates(keep='first') 
		Subject1 = pd.DataFrame(df5.groupby(['1']).size().reset_index(name='count')) 
		 
		SFb = "" 
		for i in range(len(Subject)): 
			SFb += str(Subject.iloc[i]['1']) + " -1 " + str(Subject.iloc[i]['count']) + " " + str(Subject1.iloc[i]['count']) 
			if i != len(Subject) - 1: 
				SFb = SFb + "," 
		 
		out=0 
		if spo_index_df.loc[spo_index_df["1"] == int(filenbr)].empty == False: 
			out=1 
		if ops_index_df.loc[ops_index_df["1"] == int(filenbr)].empty == False: 
			out=-1 
		 
 
		result =result+ filenbr + ";" + GF + ";" + countP + ";" + SF + "," + SFg + "," + SFb + ";"+str(out)+"\n" 
 
	now = time.time() 
	#with open(spath+str(rankNbr)+".stat",'a+') as res: 
	#		res.write(result) 
	print(str(rankNbr)+" took: ", now-then, " seconds") 
	return result 
if __name__ == "__main__": 
    main() 

