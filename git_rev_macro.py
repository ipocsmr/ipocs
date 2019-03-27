import subprocess

revision = subprocess.check_output(["git", "describe", "--dirty"]).strip()
print "-DVERSION_RAW='%s'" % revision