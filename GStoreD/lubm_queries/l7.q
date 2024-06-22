SELECT ?y ?x ?z WHERE { 
	?x <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <data/univ-bench.owl#UndergraduateStudent> .
	?z <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <data/univ-bench.owl#FullProfessor> .
	?y <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <data/univ-bench.owl#Course> .
	?x <data/univ-bench.owl#advisor> ?z .
	?x <data/univ-bench.owl#takesCourse> ?y .
	?z <data/univ-bench.owl#teacherOf> ?y .
}
