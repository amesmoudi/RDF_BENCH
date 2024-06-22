SELECT ?x ?y WHERE { 
	?y <data/univ-bench.owl#subOrganizationOf> <http://www.University0.edu> . 
	?y <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <data/univ-bench.owl#Department> . 
	?x <data/univ-bench.owl#worksFor> ?y . 
	?x <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <data/univ-bench.owl#FullProfessor> . 
}
