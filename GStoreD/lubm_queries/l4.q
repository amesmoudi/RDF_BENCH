SELECT ?x ?y1 ?y2 ?y3 WHERE { 
	?x <data/univ-bench.owl#worksFor> <http://www.Department0.University0.edu> . 
	?x <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <data/univ-bench.owl#FullProfessor> .
	?x <data/univ-bench.owl#name> ?y1 . 
	?x <data/univ-bench.owl#emailAddress> ?y2 . 
	?x <data/univ-bench.owl#telephone> ?y3 . 
}
