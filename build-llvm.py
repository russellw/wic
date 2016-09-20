#!/usr/bin/python
import os.path
import subprocess
import sys
import urllib

class chdir:
    def __init__(self, path):
        self.path = os.path.expanduser(path)

    def __enter__(self):
        self.old = os.getcwd()
        os.chdir(self.path)

    def __exit__(self, etype, value, traceback):
        os.chdir(self.old)

def download(url, filename):
	if os.path.exists(filename):
		return
	urllib.urlretrieve(url, filename)

def download_unpack(url, filename, unpacked, dirname):
	if os.path.exists(dirname):
		return
	download(url, filename)
	subprocess.check_call(('tar', 'xf', filename))
	os.rename(unpacked, dirname)

llvm_version = '3.8.0'
download_unpack(
	'http://llvm.org/releases/'+llvm_version+'/llvm-'+llvm_version+'.src.tar.xz',
	'llvm-'+llvm_version+'.src.tar.xz',
	'llvm-'+llvm_version+'.src',
	'llvm'
)
with chdir('llvm/tools'):
	download_unpack(
		'http://llvm.org/releases/'+llvm_version+'/cfe-'+llvm_version+'.src.tar.xz',
		'cfe-'+llvm_version+'.src.tar.xz',
		'cfe-'+llvm_version+'.src',
		'clang'
	)

if not os.path.exists('build'):
	os.mkdir('build')
	with chdir('build'):
		if sys.platform == 'win32':
			subprocess.check_call((
				'cmake',
				'-DLLVM_ENABLE_TIMESTAMPS=OFF',
				'-G', 'Visual Studio 14 Win64',
				'../llvm'
			))
		else:
			subprocess.check_call((
				'cmake',
				'-DLLVM_ENABLE_TIMESTAMPS=OFF',
				'-G', 'Unix Makefiles',
				'../llvm'
			))

with chdir('build'):
	subprocess.check_call(('make'))
