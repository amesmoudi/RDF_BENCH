#!/usr/bin/env python
#

import sys
import getopt
import subprocess
import os

# --------------------------------------------------------------------------------

dataSetName = None
statisticsDir = None
S2RDFQueryTranslator = "/opt/workspace/QueryTranslator/S2RDF_QueryTranslator/queryTranslator-1.1.jar"
scaleUB = "0.25"

# --------------------------------------------------------------------------------

def loadListOfQueries(mypath):
    from os import listdir
    from os.path import isfile, join
    onlyfiles = [f for f in listdir(mypath) if isfile(join(mypath, f))]
    return onlyfiles

def parseFileName(str):
    end = str.rfind(".")
    begin = str.rfind("/")
    if begin < 0:
        begin = 0
    begin = begin + 1
    baseName = str[begin:-(len(str)-end)]
    temp = baseName.split("__")
    testName = temp[0]
    tableSets = temp[1]
    return testName, tableSets

def parseArgs(str):
    result = ""
    if "SO" in str: result += " -so"
    if "OS" in str: result += " -os"
    if "SS" in str: result += " -ss"
    return result

def readFileToString(fileName):    
    with open(fileName) as myFile: return myFile.read()
    
def addStringToFile(fileName, str, queryName):
    with open(fileName, "a") as myFile: myFile.write(">>>>>>" + queryName + "\n" + str)
    
def executeCommand(fileName, sqlDir, tableName, baseName, args):
    global statisticsDir, S2RDFQueryTranslator, scaleUB
    outPutFileName = (sqlDir + "/" + baseName + "__" + tableName).replace("//", "/")
    command = ("java -jar " + S2RDFQueryTranslator
    + " -i " + fileName + " -o " + outPutFileName
    + " " + args
    + " -sd " + statisticsDir + " -sUB " + scaleUB)
    #print("\n" + command)
    status = subprocess.call(command, shell=True)
    query = readFileToString(outPutFileName + ".sql")
    #print("\n" + sqlDir)
    # add to composite query file (all queries of the input directory)
    addStringToFile(sqlDir + "/compositeQueryFile.txt", query, baseName + "__" + tableName)
    return status

def translateQuery(fileName, sqlDir):    
    global dataSetName
    if ("~" not in fileName) and ("rdf3x" not in fileName):
        #print("Parse " + fileName)
        testName, tableSets = parseFileName(fileName)
        for case in tableSets.split("_"):
            args = parseArgs(case)
            status = executeCommand(fileName, sqlDir, dataSetName, testName + "--" + case, args)
        subprocess.call("rm -f " + sqlDir + "/*.log", shell=True)

# --------------------------------------------------------------------------------
# Main function pour traiter les arguments de ligne de commande et lancer le processus de traduction

def main(argv):
    global dataSetName, statisticsDir, scaleUB
    
    sparqlDir = ''
    sqlDir = ''
    dataSetPath = ''
    
    try:
        opts, args = getopt.getopt(argv, "hs:t:d:u:", ["sdir=", "tdir=", "dbdir=", "scaleUB="])
    except getopt.GetoptError:
        print('Usage: translateWatDivQueries.py -s <sparqlDir> -t <sqlDir> -d <dataSetPath> -u <scaleUB>')
        sys.exit(2)
    
    for opt, arg in opts:
        if opt == '-h':
            print('Usage: translateWatDivQueries.py -s <sparqlDir> -t <sqlDir> -d <dataSetPath> -u <scaleUB>')
            sys.exit()
        elif opt in ("-s", "--sdir"):
            sparqlDir = arg
        elif opt in ("-t", "--tdir"):
            sqlDir = arg
        elif opt in ("-d", "--dbdir"):
            dataSetPath = arg
            dataSetName = os.path.basename(dataSetPath) 
            statisticsDir = dataSetPath+"/stats/"
        elif opt in ("-u", "--scaleUB"):
            scaleUB = arg

    if not sparqlDir or not sqlDir or not dataSetPath:
        print('Missing required directories. Usage: translateWatDivQueries.py -s <sparqlDir> -t <sqlDir> -d <dataSetPath> -u <scaleUB>')
        sys.exit()

    print(f'Input Dir is "{sparqlDir}"')
    print(f'Output Dir is "{sqlDir}"')
    print(f'Dataset Name is "{dataSetName}"')
    print(f'Scale UB is "{scaleUB}"')

    sparqlList = loadListOfQueries(sparqlDir)
    subprocess.call("rm -f " + sqlDir + "/*.*", shell=True)
    
    for fileName in sparqlList:
        translateQuery(os.path.join(sparqlDir, fileName), sqlDir)

if __name__ == "__main__":
    main(sys.argv[1:])
