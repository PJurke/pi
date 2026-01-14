import os
import sys
import subprocess

# --- Configuration ---
# Path to your compiler binary (adjust if your build folder is different)
COMPILER_BIN = "./build/picc"
# Directory containing your .pi test files
TEST_DIR = "tests"

# --- Colors for Terminal Output ---
class Colors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'

def run_test(file_path):
    """
    Runs a single test file.
    1. Parses expected output from // CHECK: comments.
    2. Runs the compiler.
    3. Verifies that expected output exists in actual output.
    """
    
    # 1. Parse expectations
    expected_checks = []
    try:
        with open(file_path, 'r') as f:
            for line in f:
                if "// CHECK:" in line:
                    # Extract everything after "CHECK:" and trim whitespace
                    check_content = line.split("// CHECK:")[1].strip()
                    if check_content:
                        expected_checks.append(check_content)
    except Exception as e:
        print(f"{Colors.FAIL}Error reading file {file_path}: {e}{Colors.ENDC}")
        return False

    # If no checks are defined, skip the test (or mark as passed)
    if not expected_checks:
        print(f"{Colors.WARNING}⚠️  SKIPPED: {file_path} (No CHECK lines found){Colors.ENDC}")
        return True

    # 2. Run the compiler
    try:
        # We capture stdout (IR code) and stderr (Logs)
        result = subprocess.run(
            [COMPILER_BIN, file_path],
            capture_output=True,
            text=True
        )
    except FileNotFoundError:
        print(f"{Colors.FAIL}❌ CRITICAL: Compiler not found at '{COMPILER_BIN}'{Colors.ENDC}")
        sys.exit(1)

    # Check if compiler crashed (non-zero exit code)
    # Note: If you have negative tests (tests that SHOULD fail), 
    # you would need to adjust this logic (e.g. look for // RUN: not ...).
    if result.returncode != 0:
        print(f"{Colors.FAIL}❌ FAILED: {file_path} (Compiler returned exit code {result.returncode}){Colors.ENDC}")
        print(f"{Colors.BOLD}Stderr output:{Colors.ENDC}\n{result.stderr}")
        return False

    # 3. Verify Output
    actual_output = result.stdout
    failed_checks = []

    for check in expected_checks:
        if check not in actual_output:
            failed_checks.append(check)

    if failed_checks:
        print(f"{Colors.FAIL}❌ FAILED: {file_path}{Colors.ENDC}")
        print(f"{Colors.BOLD}   Expected but not found in output:{Colors.ENDC}")
        for fc in failed_checks:
            print(f"     - '{fc}'")
        
        # Optional: Print actual output for debugging
        # print(f"\n   {Colors.BOLD}Actual Output:{Colors.ENDC}\n{actual_output}")
        return False

    print(f"{Colors.OKGREEN}✅ PASSED: {file_path}{Colors.ENDC}")
    return True

def main():
    if not os.path.exists(TEST_DIR):
        print(f"{Colors.FAIL}Error: Test directory '{TEST_DIR}' not found.{Colors.ENDC}")
        sys.exit(1)

    print(f"{Colors.HEADER}🚀 Starting Test Runner...{Colors.ENDC}")
    print(f"   Compiler: {COMPILER_BIN}")
    print(f"   Test Dir: {TEST_DIR}\n")

    total_tests = 0
    passed_tests = 0
    failed_tests = 0

    # Walk through the test directory recursively
    for root, dirs, files in os.walk(TEST_DIR):
        for file in files:
            if file.endswith(".pi"):
                total_tests += 1
                full_path = os.path.join(root, file)
                if run_test(full_path):
                    passed_tests += 1
                else:
                    failed_tests += 1

    # Summary
    print("-" * 50)
    print(f"{Colors.BOLD}Test Summary:{Colors.ENDC}")
    print(f"Total:  {total_tests}")
    print(f"{Colors.OKGREEN}Passed: {passed_tests}{Colors.ENDC}")
    
    if failed_tests > 0:
        print(f"{Colors.FAIL}Failed: {failed_tests}{Colors.ENDC}")
        sys.exit(1) # Return non-zero exit code for CI/CD
    else:
        print(f"{Colors.OKBLUE}All tests passed! 🍰{Colors.ENDC}")
        sys.exit(0)

if __name__ == "__main__":
    main()