import subprocess
import os
import re
import select
# Chemin absolu vers le fichier JAR du client
client_jar = "/home/ubuntu/client.jar"

# Adresse IP de la machine master
master_ip = "192.168.165.27"

# Chemin absolu vers le répertoire contenant les requêtes
queries_dir = "/home/ubuntu/exp_setup_full/queries/watdiv/gStore/"

# Chemin absolu vers le répertoire exp_setup_full
exp_setup_full_dir = "/home/ubuntu/exp_setup_full"

# Durée d'exécution maximale en secondes
max_execution_time = 200
nb_plan=-1
# Fonction pour exécuter la commande Java
def execute_java_command(plan_id, query_path):
    try:
        # Exécution de la commande Java
        command = ["java", "-jar", client_jar, master_ip, query_path, str(plan_id)]
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd="/home/ubuntu")
        # Attend jusqu'à ce que le processus se termine ou que le temps d'exécution maximum soit atteint
        process_output = []
        while True:
            ready = select.select([process.stdout], [], [], max_execution_time)
            if process.stdout in ready[0]:
                output_line = process.stdout.readline().decode("utf-8")
                if not output_line:
                    break
                process_output.append(output_line)
            else:
                break
        
        # Attendre que le processus se termine ou que le temps d'exécution maximum soit atteint
        process.wait(timeout=max_execution_time)

        # Récupération de la sortie du processus
        output = ''.join(process_output)

        # Analyse de la sortie pour extraire le temps d'exécution et le nombre de résultats
        execution_time = None
        nb_results = None
        for line in output.split("\n"):
            if "Execution time" in line:
                execution_time = int(line.split(":")[1].strip().split()[0])
            elif "Nb result" in line:
                nb_results = int(line.split(":")[1].strip())

        # Vérification si le temps d'exécution et le nombre de résultats ont été extraits
        if execution_time is None or nb_results is None:
            raise ValueError("Temps d'exécution ou nombre de résultats non disponibles")

        return execution_time, nb_results
    except subprocess.TimeoutExpired:
        process.kill()
        return "Timeout", -1
    except Exception as e:
        return "Exception", str(e)

# Fonction pour arrêter toutes les machines sur la machine master
def stop_all():
    print("Arrêt de toutes les machines...")
    subprocess.run(["python3", "stop-all", "pqdag"], cwd=exp_setup_full_dir)

# Fonction pour redémarrer toutes les machines sur la machine master
def start_all():
    print("Redémarrage de toutes les machines...")
    subprocess.run(["python3", "start-all", "pqdag"], cwd=exp_setup_full_dir)
def check_exception_in_log():
    global nb_plan
    try:
        # Lecture du contenu du fichier pqdag.master.log sur la machine master
        command = ["ssh", "ubuntu@192.168.165.27", "cat", "/home/ubuntu/pqdag/logs/pqdag.master.log"]
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output, _ = process.communicate()
        log_content = output.decode("utf-8")
        match = re.search(r'nbPlan:(\d+)', log_content)
        if match:
            nb_plan = int(match.group(1))
    except Exception as e:
        print("Une erreur s'est produite lors de la lecture du fichier de log :", str(e))
        return False
# Appeler stop_all avant le commencement
stop_all()
start_all()

# Ouvrir le fichier results.txt en mode écriture
with open("results.txt", "a") as results_file:
    # Parcourir le répertoire des requêtes
    for query_name in os.listdir(queries_dir):
        query_path = os.path.join(queries_dir, query_name)
        print(f"Traitement de la requête {query_name}...")
        # Appeler start_all avant l'exécution de chaque plan
        #start_all()
        # Boucle sur chaque ID de plani
        start_id=0
        for plan_id in range(start_id, 41):
            print(f"Exécution du plan {plan_id} de la requête {query_name}...")
            # Exécute la commande Java et récupère le temps d'exécution et le nombre de résultats
            execution_time, nb_results = execute_java_command(plan_id, query_path)
            if nb_plan == -1:
                check_exception_in_log();
                print("nb plans:",str(nb_plan))
            if check_exception_in_log():
                print("L'exception java.lang.IndexOutOfBoundsException a été détectée dans le fichier de log. Passer à la requête suivante...")
                stop_all()
                start_all()
                break
            print(f"{query_name} Plan{plan_id} {execution_time}ms {nb_results}\n")

            # Écriture des résultats dans le fichier results.txt
            results_file.write(f"{query_name} Plan{plan_id} {execution_time}ms {nb_results}\n")
            results_file.flush()
            if plan_id + 1 >= nb_plan:
                print("L'exception java.lang.IndexOutOfBoundsException a été détectée dans le fichier de log. Passer à la requête suivante...")
                stop_all()
                start_all()
                nb_plan = -1; 
                break
            # Appeler stop_all après l'exécution de chaque plan
            stop_all()
            # Appeler start_all avant l'exécution du plan suivant
            start_all()

# Appeler stop_all à la fin
stop_all()
