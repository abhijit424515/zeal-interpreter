import os
from socket import timeout
import sys
from threading import TIMEOUT_MAX
import argparse
import tarfile
import subprocess
import glob
import re
import csv
import shutil

parser = argparse.ArgumentParser(description="Automatic Evaluator for CS316 Lab Assignments.")
parser.add_argument("--assignment", help="Specify an Assignment to evaluate.",required=True)
parser.add_argument("--csv", help="Generate the csv file with evaluation result for all groups available",action='store_true')
args = parser.parse_args()
if not os.path.isdir('Execution_Logs'):
    os.mkdir('Execution_Logs')
if not os.path.isdir('Expected_Output/Parse'):
    os.mkdir('Expected_Output/Parse')

TESTCASE_TIMEOUT_PARAM = 2

df = []

SEP_START="========================================================================================="
SEP_END="\n\n=========================================================================================\n"
TEST_SEP="\n************************************************************\n"



def clean(group):
    out = subprocess.run(["make", "clean", "-C", "./Submissions/{}/".format(group)], stdout = output_log_file, stderr = output_log_file)
    if out.returncode!=0:
        print("[ERROR]: Couldn't find rule for clean  {}".format(group), file=output_log_file, flush=True)
        print("[ERROR]: Couldn't find rule for clean for {}".format(group))
    # #try:
    #     temp_files=['.out', '.tab.c','.log', '.o','.output']
    #     for f in os.listdir("./Submissions/{}/".format( group)):
    #         for file_ext in temp_files:
    #             if re.search(file_ext, f):
    #                 os.remove("./Submissions/{}/{}".format(group,f))
    #                 break
    # except:
    #     print("Directory does not contain any temporary files for {}".format(group), file=output_log_file, flush=True)
def check_shift_reduce_conflict(group):
    try:
        clean(group)
        yacc_file_count=0
        file_name=""
        for f in os.listdir("./Submissions/{}/".format(group)):
            if re.search('.y', f):
                yacc_file_count=yacc_file_count+1
                file_name=f
        if yacc_file_count == 1:
            out = subprocess.run(["yacc", "-dv", "./Submissions/{}/{}".format( group, file_name), "-o", "./Submissions/{}/y.output".format( group)], stdout = output_log_file, stderr = output_log_file)
            if out.returncode==0:
                if os.path.isfile("Submissions/{}/y.output".format(group))==0:
                    print("[ERROR]: Couldn't find y.output for {}".format(group), file=output_log_file, flush=True)
                    return 0
                else:
                    count=0
                    with open("./Submissions/{}/y.output".format(group), 'r') as file:
                        for line in file: 
                            words = line.split() 
                            for i in words: 
                                if(i.lower()=='conflict'): 
                                    count=count+1
                        
                    return count
            else:
                print("[ERROR]: Couldn't execute yacc -dv for the project for {}".format(group), file=output_log_file, flush=True)
                return 0
        elif yacc_file_count==0:
            print("[ERROR]: Couldn't find yacc file for {}".format(group), file=output_log_file, flush=True)
            return 0
        else:
            print("[ERROR]: Multiple yacc files found for {}".format(group), file=output_log_file, flush=True)
            return 0
    except Exception as e:
        print(e, file=output_log_file, flush=True)
    return 0




def build_project(group):
    clean(group)
    out = subprocess.run(["make", "-C", "./Submissions/{}/".format(group)], stdout = output_log_file, stderr = output_log_file)
    
    if out.returncode != 0:
        print("[ERROR]: Couldn't build the project for {}".format(group), file=output_log_file, flush=True)
        print("[INFO]: Skipping Further Evaluation of current submission", file=output_log_file, flush=True)
        return 0
    if os.path.isfile("Submissions/{}/sclp".format(group))==0:
        print("[ERROR]: Couldn't find sclp for {}".format(group), file=output_log_file, flush=True)
        return 0 
    return 1

def is_test_error_type(path_to_test):
    return path_to_test.split('/')[-2].upper() == 'ERROR'

def evaluate_sclp_phase(group, test, phase_name,phase_name_flag, output_file_type):
    result = []
    for idx, tc in enumerate(glob.glob("{}*.c".format(test))):
        try:
            sr = subprocess.run(["./Submissions/{}/sclp".format(group),phase_name_flag,tc], timeout=TESTCASE_TIMEOUT_PARAM, stdout = output_log_file, stderr = output_log_file)
        except subprocess.TimeoutExpired as e:
            print(e, file=output_log_file, flush=True)
            result.append(0)
        else:
            tc_res = subprocess.run(["diff","-B", "-w", "-N", "{}.{}".format(tc, output_file_type), "./Expected_Output/{}/{}.txt".format(phase_name,tc.split("/")[-1].split(".")[0])],stdout=subprocess.PIPE)
        
            if sum(1 for _ in tc_res.stdout.decode('utf-8')) == 0:
                print("[PASS]: Testcase {}".format(tc), file=output_log_file, flush=True)
                print("[PASS]: Testcase {}".format(tc))
                result.append(1)
            else:
                print("[FAIL]: Testcase {}".format(tc), file=output_log_file, flush=True)
                print(tc_res.stdout.decode('utf-8'), file=output_log_file, flush=True)
                print("[FAIL]: Testcase {}".format(tc))
                result.append(0)
        try:
            os.remove("{}.{}".format(tc, output_file_type))
        except:
            print("[INFO]: Cannot find the file {}.{}".format(tc, output_file_type), file=output_log_file, flush=True)
    return result

def evaluate_error_testcases(test, group):
    result = []
    for tc in glob.glob("{}*.c".format(test)):
        try:
            tc_res = subprocess.run(["./Submissions/{}/sclp".format(group), tc], timeout=TESTCASE_TIMEOUT_PARAM, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        except subprocess.TimeoutExpired as e:
            print(e, file=output_log_file, flush=True)
            print("[FAIL]: Testcase {}".format(tc), file=output_log_file, flush=True)
            print("[FAIL]: Testcase {}".format(tc))
            result.append(0)
            continue

        if tc_res.returncode != 0:
            print("[PASS]: Testcase {}".format(tc), file=output_log_file, flush=True)
            print("[PASS]: Testcase {}".format(tc))
            result.append(1)
        else:
            print("[FAIL]: Testcase {}".format(tc), file=output_log_file, flush=True)
            print(tc_res.stdout.decode('utf-8'), file=output_log_file, flush=True)
            print("[FAIL]: Testcase {}".format(tc))
            result.append(0)
               
        # try:
        #     os.remove("{}.{}".format(tc, output_file_type))
        # except:
        #     print("[INFO]: Cannot find the file {}.{}".format(tc, output_file_type), file=output_log_file, flush=True)    
    return result

def evaluate_parse_phase(test, group):
    result = []
    for idx, tc in enumerate(glob.glob("{}*.c".format(test))):
        try:
            tc_res = subprocess.run(["./Submissions/{}/sclp".format(group), tc], timeout=TESTCASE_TIMEOUT_PARAM, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        except subprocess.TimeoutExpired as e:
            print(e, file=output_log_file, flush=True)
        if tc_res.returncode == 0:
            print("[PASS]: Testcase {}".format(tc), file=output_log_file, flush=True)
            print("[PASS]: Testcase {}".format(tc))
            result.append(1)
        else:
            print("[FAIL]: Testcase {}".format(tc), file=output_log_file, flush=True)
            print(tc_res.stdout.decode('utf-8'), file=output_log_file, flush=True)
            print("[FAIL]: Testcase {}".format(tc))
            result.append(0)
    return result

def evaluate_submission(group):
    Tests = glob.glob("./Testcases/*/")
    result = [group]
    for test in Tests:
        if is_test_error_type(test):
            result.extend(evaluate_error_testcases(test, group))
        else:
            sclp_phases = glob.glob("./Expected_Output/*/")
            for phase in sclp_phases: 
                phase_name = phase.split('/')[-2]
                phase_name = phase_name.upper()     
                if phase_name == 'TOKENS':
                    result.extend(evaluate_sclp_phase(group, test, phase.split('/')[-2], "--show-tokens", "toks"))
                elif phase_name == 'PARSE':
                    result.extend(evaluate_parse_phase(test, group))
                elif phase_name == 'AST':
                    result.extend(evaluate_sclp_phase(group, test, phase.split('/')[-2],"--show-ast", "ast"))
                elif phase_name == 'TAC':
                    result.extend(evaluate_sclp_phase(group, test, phase.split('/')[-2],"--show-tac", "tac"))   
                elif phase_name == 'RTL':
                    result.extend(evaluate_sclp_phase(group, test, phase.split('/')[-2],"--show-rtl", "rtl"))
                elif phase_name == 'ASM':
                    result.extend(evaluate_sclp_phase(group, test, phase.split('/')[-2],"--show-asm", "asm"))

        print(TEST_SEP)
    return result

def validate_submission_name(tar):
    regex = r"group[0-9]{1,}.tar.gz"
    out = re.search(regex, tar)
    return (1 if out else 0)

def create_csv_header():
    Tests = glob.glob("./Testcases/*/")
    cols = []
    for test in Tests:
        if is_test_error_type(test):
            for idx, tc in enumerate(glob.glob("{}*.c".format(test))):
                cols.append(tc.split("/")[-1].split(".")[0])
        else:
            sclp_phases = glob.glob("./Expected_Output/*/")
            for phase in sclp_phases: 
                phase_name = phase.split('/')[-2]
                phase_name = phase_name.upper()
                for idx, tc in enumerate(glob.glob("{}*.c".format(test))):
                    cols.append("{}:{}".format(phase_name, tc.split("/")[-1].split(".")[0]))
    cols.append("Total")
    cols.append("Number of Shift-reduce Conflict")
    cols.append("Remarks")
    return cols

output_log_file = ''
print("\n\n[STARTING]: Automatic Evaluation Begins...")

if __name__ == '__main__':
    print("[INFO]: Evaluating " + args.assignment)
    submissions = glob.glob("./Submissions/*.tar.gz")
    if args.csv:
        cols = ['Group']
        cols.extend(create_csv_header())
        df.append(cols)
        
    for submission in submissions:
        print(SEP_START)
        tar_file_name = submission.split("/")[-1]
        group = tar_file_name.split(".")[0]
        output_log_file = open("./Execution_Logs/{}.log".format(group), 'w+')
        print("[INFO]: Evaluating for " + group, file=output_log_file, flush=True)
        print("[INFO]: Evaluating for " + group)
        if validate_submission_name(tar_file_name):
            print("[INFO]: Valid tarfile name", file=output_log_file, flush=True)
            print("[INFO]: Extracting submission for " + group, file=output_log_file, flush=True)
            tar = tarfile.open('./Submissions/{}.tar.gz'.format(group))
            tar.extractall("./Submissions/")
            tar.close()
            if os.path.isdir('Submissions/{}/{}'.format(group,group)):
                dest = 'Submissions/{}'.format(group)
                for f in os.listdir("./Submissions/{}/{}".format(group, group)):
                    shutil.copytree(f, dest)
                os.rmdir('Submissions/{}/{}'.format(group,group))
            if build_project(group):
                if args.csv:
                    li=evaluate_submission(group)
                    total=0
                    for i in range(1, len(li)):
                        total=total+int(li[i])
                    li.append(total)
                    li.append(check_shift_reduce_conflict(group))
                    df.append(li)
                else:
                    evaluate_submission(group)
                    check_shift_reduce_conflict(group)
            else:
                if args.csv:
                    eval_result_fail = [group]
                    eval_result_fail.extend([0 for i in range(1,len(df[0])-1)])
                    df.append(eval_result_fail)
        else:
            print("[ERROR]: Invalid tarfile name "+tar_file_name, file=output_log_file, flush=True)
            print("[INFO]: Skipping Further Evaluation of current submission", file=output_log_file, flush=True)
        print("\n\n\n")
    if args.csv:
        with open("{}-Result.csv".format(args.assignment), 'w') as f:
            write = csv.writer(f)
            for row in df:
                write.writerow(row)
