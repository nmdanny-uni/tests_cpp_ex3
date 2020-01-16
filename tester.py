from __future__ import annotations
import os
from dataclasses import dataclass
import sys
from pathlib import Path
import pytest
import subprocess
from typing import List, Union
from tempfile import NamedTemporaryFile

USE_VALGRIND = True

TEST_DIR = Path(__file__).parent

EXECUTABLE_PATH = Path(Path(TEST_DIR) / "../cmake-build-debug/SpamDetector")
SCHOOL_EXECUTABLE = Path(Path(TEST_DIR) / "./SchoolSpamDetector")

if not EXECUTABLE_PATH.exists():
    print(f"Couldn't find your executable at {EXECUTABLE_PATH}", file=sys.stderr)
    sys.exit(-1)

if not SCHOOL_EXECUTABLE.exists():
    print(f"Couldn't find school executable at {SCHOOL_EXECUTABLE}", file=sys.stderr)
    sys.exit(-1)

VALID_DIR = TEST_DIR / "valid"
INVALID_DIR = TEST_DIR / "invalid"


@dataclass
class Output:
    return_code: int
    stdout: str
    stderr: str
    valgrind_out: str

    def compare_to(self, other: Output):
        # "self" is the school's output and "other" is YOUR output
        assert self.return_code == other.return_code, "Return code mismatch(left=school, right=yours)"
        assert self.stdout == other.stdout, "STDOUT mismatch(left=school, right=yours)"
        assert self.stderr == other.stderr, "STDERR mismatch(left=school, right=yours)"

    def check_valgrind_out(self):
        if "ERROR SUMMARY: 0" not in self.valgrind_out:
            print(f"Valgrind failed:\n{self.valgrind_out}", file=sys.stderr)

def run_with_cmd(command_list: List[str], str="", valgrind=False) -> Output:
    """
    Execute the given command list with the input
    """
    valgrind_outfile, valgrind_output = None, ""
    if valgrind:
        valgrind_outfile = NamedTemporaryFile(mode='r+', encoding='utf-8')
        command_list = ['valgrind', '--leak-check=yes', f'--log-file={valgrind_outfile.name}'] + command_list

    print(f"Running command \"{' '.join(command_list)}\"")
    try:
        process = subprocess.run(command_list, shell=False, input=str,
                                 stdout=subprocess.PIPE,
                                 stderr=subprocess.PIPE, text=True)

        if valgrind:
            valgrind_outfile.seek(0)
            valgrind_output = valgrind_outfile.read()
            valgrind_outfile.close()

        return Output(process.returncode, process.stdout, process.stderr, valgrind_output)
    except Exception as e:
        print(f"Error while running subprocess: {e}")
        sys.exit(-1)


TEST_CASES = [
    # the score for the text is 145
    ("A valid DB+text file", "valid/00.db", "email-text", 144),
    ("A valid DB+text file", "valid/00.db", "email-text", 145),
    ("A valid DB+text file", "valid/00.db", "email-text", 146),
    ("A non existent text file", "valid/00.db", "file-doesnt-exist.txt", 50),
    ("A valid DB+text file, with an invalid threshold", "valid/00.db", "email-text", 0),
    ("A valid DB+text file, with an invalid threshold", "valid/00.db", "email-text", -5),
    ("A valid DB+text file, with an invalid threshold", "valid/00.db", "email-text", "five"),
    ("A valid DB+text file, with an invalid threshold", "valid/00.db", "email-text", 3.141),
    ("Empty extra column", "invalid/empty_extra_col.db", "email-text", 50),
    ("Extra column", "invalid/extra_col.db", "email-text", 50),
    ("Non integer field", "invalid/non_integer_field.db", "email-text", 50),
    ("Wrong structure", "invalid/wrong_structure.db", "email-text", 50),
    ("Wrong structure", "invalid/wrong_structure2.db", "email-text", 50),
    ("Wrong structure", "invalid/wrong_structure3.db", "email-text", 50),
    ("Empty row(newline)", "invalid/empty_row.db", "email-text", 50),
    ("Empty row(spaces)", "invalid/empty_row_spaces.db", "email-text", 50),

]


@pytest.mark.parametrize("label,csv_path,txt_path,threshold", TEST_CASES)
def test(label: str, csv_path: str, txt_path: str, threshold: Union[str, int]):
    print("Testing: ", label)
    my_out = run_with_cmd([str(EXECUTABLE_PATH), csv_path, txt_path, str(threshold)], valgrind=USE_VALGRIND)
    school_out = run_with_cmd([str(SCHOOL_EXECUTABLE), csv_path, txt_path, str(threshold)], valgrind=False)
    school_out.compare_to(my_out)
    if USE_VALGRIND:
        my_out.check_valgrind_out()

if __name__ == '__main__':
    exit_code = pytest.main([__file__, '-vvs'])
    sys.exit(exit_code)


